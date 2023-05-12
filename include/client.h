#pragma once
#include <nng/nng.h>
#include <nng/protocol/reqrep0/req.h>

#include <iostream>
#include <string>

using namespace std;

class Client {
   public:
    Client();
    ~Client();

    bool Init();
    void Run();

   private:
    int socket;
};