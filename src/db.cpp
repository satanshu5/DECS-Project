#include "db.h"
#include "db_pool.h"
#include <mysql/mysql.h>
#include <stdexcept>
#include <iostream>

using namespace std;

DB::DB(const string &host, const string &user,
       const string &pass, const string &dbname) {
    // Initialize the connection pool only once
    ConnectionPool::getInstance().init(host, user, pass, dbname, 10);
}

bool DB::get(int k, string &v) {
    MYSQL* conn = ConnectionPool::getInstance().acquire();
    string q = "SELECT v FROM kv_table WHERE k = " + to_string(k);
    if (mysql_query(conn, q.c_str())) {
        ConnectionPool::getInstance().release(conn);
        return false;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) {
        ConnectionPool::getInstance().release(conn);
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    bool ok = false;
    if (row && row[0]) {
        v = row[0];
        ok = true;
    }

    mysql_free_result(res);
    ConnectionPool::getInstance().release(conn);
    return ok;
}

bool DB::put(int k, const string &v) {
    MYSQL* conn = ConnectionPool::getInstance().acquire();

    char *buf = new char[v.size()*2 + 1];
    unsigned long outlen = mysql_real_escape_string(conn, buf, v.c_str(), v.size());
    string val = "'" + string(buf, outlen) + "'";
    delete [] buf;

    string q = "REPLACE INTO kv_table (k,v) VALUES (" + to_string(k) + "," + val + ")";
    bool ok = (mysql_query(conn, q.c_str()) == 0);

    ConnectionPool::getInstance().release(conn);
    return ok;
}

bool DB::del(int k) {
    MYSQL* conn = ConnectionPool::getInstance().acquire();
    string q = "DELETE FROM kv_table WHERE k = " + to_string(k);
    bool ok = (mysql_query(conn, q.c_str()) == 0);
    ConnectionPool::getInstance().release(conn);
    return ok;
}
