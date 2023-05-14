#pragma once
#include <glog/logging.h>
#include <nng/nng.h>
#include <nng/protocol/reqrep0/rep.h>
#include <nng/supplemental/util/platform.h>

#include <iostream>
#include <string>

using namespace std;
using namespace google;

#ifndef PARALLEL
#define PARALLEL 10
#endif
struct work {
    enum { INIT, RECV, WAIT, SEND } state;
    nng_aio *aio;
    nng_msg *msg;
    nng_ctx ctx;
};

const string url = "ipc:///tmp/async_demo";
class Server {
   public:
    Server();
    ~Server();
    static void server_cb(void *arg);
    struct work *alloc_work(nng_socket sock);
    bool Init(const string &url);
    void Run();

   private:
    nng_socket sock;
    struct work *works[PARALLEL];
};
