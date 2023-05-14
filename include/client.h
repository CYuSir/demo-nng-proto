#pragma once
#include <glog/logging.h>
#include <nng/nng.h>
#include <nng/protocol/reqrep0/req.h>
#include <nng/supplemental/util/platform.h>

#include <iostream>
#include <string>

using namespace std;
using namespace google;

const string url = "ipc:///tmp/async_demo";
class Client {
   public:
    Client();
    ~Client();

    bool Init(const string &url);
    void Run();

   private:
    nng_socket sock;
    nng_msg *msg;
    nng_time start;
    nng_time end;
    unsigned msec;
};