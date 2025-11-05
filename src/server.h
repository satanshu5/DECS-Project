#pragma once
#include "cache.h"
#include "db.h"
#include "metrics.h"
#include <string>
using namespace  std;

class KVServer {
private:
    int port;
    LRUCache cache;
    DB db;
    int worker_count;

public:
    KVServer(int port, int workers, size_t cache_size,
             const string &db_host,
             const string &db_user,
             const string &db_pass,
             const string &db_name);

    void run();
};
