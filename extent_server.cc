// the extent server implementation

#include "extent_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

extent_server::extent_server()
{
        pthread_mutex_init(&mutex, NULL);
}


int extent_server::put(extent_protocol::extentid_t id, std::string buf, int &)
{
        // You fill this in for Lab 2.
        if (pthread_mutex_lock(&mutex) == 0) {
                std::map<extent_protocol::extentid_t, record>::iterator it;
                it = records.find(id);
                if (it == records.end()) { //add a new record
                        record nr;
                        nr.buf = buf;
                        nr.a.size = buf.size();
                        //nr.a.atime = time();
                        nr.a.mtime = time(NULL);
                        nr.a.ctime = time(NULL);
                        records[id] = nr;
                } else { //modify a existing record
                        record &rd = it->second;
                        rd.buf = buf;
                        rd.a.size = buf.size();
                        rd.a.ctime = time(NULL);
                        rd.a.mtime = time(NULL);
                }
                pthread_mutex_unlock(&mutex);
                return extent_protocol::OK;
        } else {
                printf("pthread_mutex_lock@put error.\n");
                return extent_protocol::IOERR;
        }
}

int extent_server::get(extent_protocol::extentid_t id, std::string &buf)
{
        // You fill this in for Lab 2.
        std::map<extent_protocol::extentid_t, record>::iterator it;
        if (pthread_mutex_lock(&mutex) == 0) {
                it = records.find(id);
                if (it == records.end()) {
                        pthread_mutex_unlock(&mutex);
                        return extent_protocol::NOENT;
                } else {
                        buf = it->second.buf;
                        it->second.a.atime = time(NULL);
                        it->second.a.size = buf.size();
                        pthread_mutex_unlock(&mutex);
                        return extent_protocol::OK;
                }
        } else {
                printf("pthread_mutex_lock@get error.\n");
                return extent_protocol::IOERR;
        }
}

int extent_server::getattr(extent_protocol::extentid_t id, extent_protocol::attr &a)
{
        // You fill this in for Lab 2.
        // You replace this with a real implementation. We send a phony response
        // for now because it's difficult to get FUSE to do anything (including
        // unmount) if getattr fails.
        if (pthread_mutex_lock(&mutex) == 0) {
                std::map<extent_protocol::extentid_t, record>::iterator it;
                it = records.find(id);
                if (it != records.end()) {
                        a = it->second.a;
                        pthread_mutex_unlock(&mutex);
                        return extent_protocol::OK;
                } else {
                        a.size = 0;
                        a.atime = 0;
                        a.mtime = 0;
                        a.ctime = 0;
                        pthread_mutex_unlock(&mutex);
                        return extent_protocol::NOENT;
                }
                return extent_protocol::OK;
        } else {
                printf("pthread_mutex_lock@getattr error.\n");
                return extent_protocol::IOERR;
        }
}

int extent_server::remove(extent_protocol::extentid_t id, int &)
{
        // You fill this in for Lab 2.
        if (pthread_mutex_lock(&mutex) == 0) {
                records.erase(id);
                pthread_mutex_unlock(&mutex);
                return extent_protocol::OK;
        } else {
                printf("pthread_mutex_lock@remove error.\n");
                return extent_protocol::IOERR;
        }
}

