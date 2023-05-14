#include "client.h"

Client::Client() { LOG(INFO) << "Client::Client()"; }

Client::~Client() {
    LOG(INFO) << "Client::~Client()";
    nng_close(sock);
}

bool Client::Init(const string& url) {
    LOG(INFO) << "Client::Init()";
    int rv;
    if ((rv = nng_req0_open(&sock)) != 0) {
        LOG(ERROR) << "nng_req0_open failed with error code: " << rv;
        return false;
    }
    msec = 300;
    if ((rv = nng_dial(sock, url.c_str(), NULL, 0)) != 0) {
        LOG(ERROR) << "nng_dial failed with error code: " << rv;
        return false;
    }
    return true;
}

void Client::Run() {
    LOG(INFO) << "Client::Run()";
    int rv;
    start = nng_clock();

    if ((rv = nng_msg_alloc(&msg, 0)) != 0) {
        LOG(ERROR) << "nng_msg_alloc failed with error code: " << rv;
        return;
    }
    if ((rv = nng_msg_append_u32(msg, msec)) != 0) {
        LOG(ERROR) << "nng_msg_append_u32 failed with error code: " << rv;
        nng_msg_free(msg);
        return;
    }
    if ((rv = nng_msg_append(msg, "Hello, Server.", strlen("Hello, World."))) != 0) {
        LOG(ERROR) << "nng_msg_append failed with error code: " << rv;
        nng_msg_free(msg);
        return;
    }

    if ((rv = nng_sendmsg(sock, msg, 0)) != 0) {
        LOG(ERROR) << "nng_send failed with error code: " << rv;
        return;
    }

    if ((rv = nng_recvmsg(sock, &msg, 0)) != 0) {
        LOG(ERROR) << "nng_recvmsg failed with error code: " << rv;
        return;
    }
    std::string message(reinterpret_cast<char*>(nng_msg_body(msg)), nng_msg_len(msg));
    LOG(INFO) << "Received message: " << message;
    end = nng_clock();
    nng_msg_free(msg);

    LOG(INFO) << "Request took " << (uint32_t)(end - start) << " milliseconds.";
    return;
}
void shutdown_logging() {
    // FlushLogFiles(google::GLOG_INFO);
    google::ShutdownGoogleLogging();
}
int main(const int argc, const char* argv[]) {
    // Init glog
    InitGoogleLogging(argv[0]);
    // Set log directory
    FLAGS_log_dir = ".";
    // Modify log destination
    // SetLogDestination(google::INFO, "client.log");
    // Set logtostderr
    FLAGS_logtostderr = true;
    // Set minloglevel
    FLAGS_minloglevel = google::INFO;
    // register shutdown handler
    atexit(shutdown_logging);

    Client client;
    if (!client.Init(url)) {
        return EXIT_FAILURE;
    }
    client.Run();

    return EXIT_SUCCESS;
}