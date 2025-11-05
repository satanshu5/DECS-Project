#pragma once
#include <mysql/mysql.h>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <stdexcept>

using namespace std;

class ConnectionPool
{
private:
    ConnectionPool() = default;
    ~ConnectionPool() { shutdown(); }
    ConnectionPool(const ConnectionPool &) = delete;
    ConnectionPool &operator=(const ConnectionPool &) = delete;

    mutex mu;
    condition_variable cv;
    queue<MYSQL *> connections;
    size_t capacity = 0;

public:
    static ConnectionPool &getInstance()
    {
        static ConnectionPool instance;
        return instance;
    }

    void init(const string &host, const string &user,
              const string &pass, const string &db,
              size_t poolSize = 10)
    {
        lock_guard<mutex> lock(mu);
        if (!connections.empty())
            return; // already initialized

        for (size_t i = 0; i < poolSize; ++i)
        {
            MYSQL *conn = mysql_init(nullptr);
            if (!mysql_real_connect(conn, host.c_str(), user.c_str(), pass.c_str(),
                                    db.c_str(), 0, nullptr, 0))
            {
                throw runtime_error(string("MySQL connect failed: ") + mysql_error(conn));
            }
            mysql_autocommit(conn, 1);
            connections.push(conn);
        }
        capacity = poolSize;
    }

    MYSQL *acquire()
    {
        unique_lock<mutex> lock(mu);
        cv.wait(lock, [&]
                { return !connections.empty(); });
        MYSQL *conn = connections.front();
        connections.pop();
        return conn;
    }

    void release(MYSQL *conn)
    {
        lock_guard<mutex> lock(mu);
        connections.push(conn);
        cv.notify_one();
    }

    void shutdown()
    {
        lock_guard<mutex> lock(mu);
        while (!connections.empty())
        {
            mysql_close(connections.front());
            connections.pop();
        }
    }
};
