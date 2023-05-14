#include "server.h"

Server::Server() { LOG(INFO) << "Server::Server()"; }

Server::~Server() { LOG(INFO) << "Server::~Server()"; }

void Server::Parse(nng_msg*& msg) {
    LOG(INFO) << "Server::Parse()";
    int rv;
    std::string message(reinterpret_cast<char*>(nng_msg_body(msg)), nng_msg_len(msg));
    // LOG(INFO) << "Received message: " << message;
    nng_msg_clear(msg);
    Request req;
    Response rep;

    if (!req.ParseFromString(message)) {
        LOG(ERROR) << "ParseFromString failed";
        return;
    }
    LOG(INFO) << "Received request: " << req.DebugString();
    switch (req.request_case()) {
        case Request::kLogin: {
            rep.set_id(2);
            *(rep.mutable_timestamp()) = TimeUtil::GetCurrentTime();
            rep.mutable_login()->set_name("test1");
            rep.mutable_login()->set_password("321");
            rep.mutable_login()->set_code(3);
            rep.mutable_login()->set_email("123@qq.com");
        } break;

        default:
            break;
    }

    if ((rv = nng_msg_append(msg, rep.SerializeAsString().c_str(), rep.SerializeAsString().length())) != 0) {
        LOG(ERROR) << "nng_msg_append failed with error code: " << rv;
        return;
    }
    return;
}

void Server::server_cb(void* arg) {
    // LOG(INFO) << "Server::server_cb()";
    struct work* work = (struct work*)arg;
    nng_msg* msg;
    int rv;
    uint32_t when;

    switch (work->state) {
        case work::INIT:
            // LOG(INFO) << "INIT";
            work->state = work::RECV;
            nng_ctx_recv(work->ctx, work->aio);
            break;
        case work::RECV: {
            LOG(INFO) << "RECV";
            if ((rv = nng_aio_result(work->aio)) != 0) {
                LOG(ERROR) << "nng_ctx_recv failed with error code: " << rv;
            }
            msg = nng_aio_get_msg(work->aio);
            if ((rv = nng_msg_trim_u32(msg, &when)) != 0) {
                // bad message, just ignore it.
                nng_msg_free(msg);
                nng_ctx_recv(work->ctx, work->aio);
                return;
            }
            // parse msg and set msg
            Parse(msg);

            work->msg = msg;
            work->state = work::WAIT;
            nng_sleep_aio(when, work->aio);
        } break;
        case work::WAIT:
            LOG(INFO) << "WAIT";
            // We could add more data to the message here.
            nng_aio_set_msg(work->aio, work->msg);
            work->msg = NULL;
            work->state = work::SEND;
            nng_ctx_send(work->ctx, work->aio);
            break;
        case work::SEND:
            LOG(INFO) << "SEND";
            if ((rv = nng_aio_result(work->aio)) != 0) {
                nng_msg_free(work->msg);
                LOG(ERROR) << "nng_ctx_send failed with error code: " << rv;
            }
            work->state = work::RECV;
            nng_ctx_recv(work->ctx, work->aio);
            break;
        default:
            LOG(ERROR) << "bad state " << NNG_ESTATE;
            break;
    }
}

struct work* Server::alloc_work(nng_socket sock) {
    // LOG(INFO) << "Server::alloc_work()";
    struct work* w;
    int rv;

    if ((w = (struct work*)nng_alloc(sizeof(*w))) == NULL) {
        LOG(ERROR) << "nng_alloc failed with error code: " << NNG_ENOMEM;
    }
    if ((rv = nng_aio_alloc(&w->aio, &Server::server_cb, w)) != 0) {
        LOG(ERROR) << "nng_aio_alloc failed with error code: " << rv;
    }
    if ((rv = nng_ctx_open(&w->ctx, sock)) != 0) {
        LOG(ERROR) << "nng_ctx_open failed with error code: " << rv;
    }
    w->state = work::INIT;
    return (w);
}

bool Server::Init(const string& url) {
    LOG(INFO) << "Server::Init()";
    int rv;
    int i;

    /*  Create the socket. */
    rv = nng_rep0_open(&sock);
    if (rv != 0) {
        LOG(ERROR) << "nng_rep0_open failed with error code: " << rv;
        return false;
    }

    for (i = 0; i < PARALLEL; i++) {
        works[i] = alloc_work(sock);  // this alloc work items
    }

    return true;
}

void Server::Run() {
    LOG(INFO) << "Server::Run()";
    int rv;
    int i;

    if ((rv = nng_listen(sock, url.c_str(), NULL, 0)) != 0) {
        LOG(ERROR) << "nng_listen failed with error code: " << rv;
        return;
    }
    for (i = 0; i < PARALLEL; i++) {
        server_cb(works[i]);  // this starts them going (INIT state)
    }

    for (;;) {
        nng_msleep(3600000);  // neither pause() nor sleep() portable
    }
    return;
}
void shutdown_logging() {
    // FlushLogFiles(google::GLOG_INFO);
    google::ShutdownGoogleLogging();
}
int main(const int argc, const char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    // Init glog
    InitGoogleLogging(argv[0]);
    // Set log directory
    FLAGS_log_dir = ".";
    // Modify log destination
    // SetLogDestination(google::INFO, "server.log");
    // Set logtostderr
    FLAGS_logtostderr = true;
    // Set minloglevel
    FLAGS_minloglevel = google::INFO;
    // register shutdown handler
    atexit(shutdown_logging);

    Server server;
    if (!server.Init(url)) {
        return EXIT_FAILURE;
    }
    server.Run();

    return EXIT_SUCCESS;
}