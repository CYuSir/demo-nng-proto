#pragma once
#include <nng/nng.h>
#include <nng/protocol/reqrep0/rep.h>

#include <iostream>
#include <string>

using namespace std;

class Server {
   public:
    Server();
    ~Server();
    bool Init();
    void Run();

   private:
    int socket;
};
