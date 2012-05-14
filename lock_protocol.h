// lock protocol

#ifndef lock_protocol_h
#define lock_protocol_h

#include "rpc.h"
#include <pthread.h>

class lock_protocol {
 public:
  enum xxstatus { OK, RETRY, RPCERR, NOENT, IOERR };
  typedef int status;
  typedef unsigned long long lockid_t;
  enum rpc_numbers {
    acquire = 0x7001,
    release,
    stat
  };
};

class lock {
 public:
        enum lkstatus { FREE, BUSY };
        lock_protocol::lockid_t id;
        int status;
        pthread_mutex_t mutex;

        lock(lock_protocol::lockid_t);
        lock(lock_protocol::lockid_t, int);
        ~lock() {};
};

#endif
