// this is the lock server
// the lock client has a similar interface

#ifndef lock_server_h
#define lock_server_h

#include <string>
#include "lock_protocol.h"
#include "lock_client.h"
#include "rpc.h"

class lock_server {

 protected:
        int nacquire;

        //cj-hack @ May 13 2012: add locks map
        std::map<lock_protocol::lockid_t, lock *> lockmap;
        pthread_mutex_t mutex;

 public:
        lock_server();
        ~lock_server() {};
        lock_protocol::status stat(int clt, lock_protocol::lockid_t lid, int &);

        lock_protocol::status acquire(int clt, lock_protocol::lockid_t, int &);
        lock_protocol::status release(int clt, lock_protocol::lockid_t, int &);
};

#endif







