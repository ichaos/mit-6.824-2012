// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

lock::lock(lock_protocol::lockid_t id):
        status (lock::FREE)
{
        id = id;
        pthread_mutex_init(&mutex, NULL);
}

lock::lock(lock_protocol::lockid_t id, int stat)
{
        id = id;
        status = stat;
        pthread_mutex_init(&mutex, NULL);
}

lock_server::lock_server():
        nacquire (0)
{
        //lockmap = new std::map<lock_protocol::lockid_t, lock *>();
        pthread_mutex_init(&mutex, NULL);
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
        lock_protocol::status ret = lock_protocol::OK;
        //printf("stat request from clt %d\n", clt);
        r = nacquire;
        return ret;
}

lock_protocol::status
lock_server::acquire(int clt, lock_protocol::lockid_t lid, int &r)
{
        lock_protocol::status ret = lock_protocol::OK;
        std::map<lock_protocol::lockid_t, lock *>::iterator it;
        it = lockmap.find(lid);
        //try to get lock
        if (it != lockmap.end()) {
                if (pthread_mutex_lock(&it->second->mutex) == 0) {
                        it->second->status = lock::BUSY;
                        return ret;
                } else {
                        printf("pthread_mutex_lock(lock) error.\n");
                        return lock_protocol::IOERR;
                }
        } else {
                if (pthread_mutex_lock(&mutex) == 0) {
                        it = lockmap.find(lid);
                        if (it == lockmap.end()) { //add a new lock
                                lock *nl = new lock(lid, lock::BUSY);
                                pthread_mutex_lock(&nl->mutex);
                                lockmap[lid] = nl;
                        } else {
                                //get the lock
                                pthread_mutex_lock(&it->second->mutex);
                                it->second->status = lock::BUSY;
                        }
                        pthread_mutex_unlock(&mutex);
                        return ret;
                } else {
                        printf("pthread_mutex_lock(lockmap) error.\n");
                        return lock_protocol::IOERR;
                }
        }
}

lock_protocol::status
lock_server::release(int clt, lock_protocol::lockid_t lid, int &r)
{
        lock_protocol::status ret = lock_protocol::OK;
        std::map<lock_protocol::lockid_t, lock *>::iterator it;
        it = lockmap.find(lid);
        if (it == lockmap.end()) {
                printf("Some delete a locked lock!\n");
                return lock_protocol::IOERR;
        }
        it->second->status = lock::FREE;
        if (pthread_mutex_unlock(&it->second->mutex) == 0)
                return ret;
        printf("pthread_mutex_unlock(lock) error\n");
        return lock_protocol::IOERR;
}
