#include "server.h"
#include <iostream>
#include <thread>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv)
{
    int port = 8080;
    size_t cache_sz = 10000;

    // Auto-detect CPU cores
    int workers = thread::hardware_concurrency();
    if (workers == 0) workers = 4; // fallback for systems that return 0

    string db_host = "127.0.0.1", db_user = "root", db_pass = "", db_name = "kvdb";

    // Command-line overrides
    if (argc > 1) port = atoi(argv[1]);
    if (argc > 2) workers = atoi(argv[2]);
    if (argc > 3) cache_sz = atoi(argv[3]);

    const char *env_user = getenv("KV_DB_USER");
    if (env_user) db_user = env_user;
    const char *env_pass = getenv("KV_DB_PASS");
    if (env_pass) db_pass = env_pass;

    cout << "--------------------------------------------\n";
    cout << "Starting KV Server\n";
    cout << "Port: " << port << endl;
    cout << "Detected CPU cores: " << thread::hardware_concurrency() << endl;
    cout << "Using worker threads: " << workers << endl;
    cout << "Cache size: " << cache_sz << endl;
    cout << "--------------------------------------------\n";

    KVServer s(port, workers, cache_sz, db_host, db_user, db_pass, db_name);
    s.run();

    return 0;
}
