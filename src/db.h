#pragma once
#include <string>

using namespace std;

class DB {
public:
    DB(const string &host, const string &user,
       const string &pass, const string &dbname);

    bool get(int k, string &v);
    bool put(int k, const string &v);
    bool del(int k);
};
