#pragma once
#include <unordered_map>
#include <list>
#include <string>
#include <shared_mutex>
#include <mutex>
#include "cache.h"

using namespace std;

class LRUCache {
public:
    explicit LRUCache(size_t cap = 10000) : capacity(cap) {}

    bool get(int key, string &value) {
        {
            shared_lock<shared_mutex> readLock(mapMutex);
            auto it = cacheMap.find(key);
            if (it == cacheMap.end())
                return false;

            value = it->second->second;

            lock_guard<mutex> listLock(listMutex);
            cacheList.splice(cacheList.begin(), cacheList, it->second);
        }
        return true;
    }

    void put(int key, const string &value) {
        unique_lock<shared_mutex> writeLock(mapMutex);
        lock_guard<mutex> listLock(listMutex);

        auto it = cacheMap.find(key);
        if (it != cacheMap.end()) {
            it->second->second = value;
            cacheList.splice(cacheList.begin(), cacheList, it->second);
            return;
        }

        if (cacheList.size() >= capacity) {
            auto last = cacheList.back();
            cacheMap.erase(last.first);
            cacheList.pop_back();
        }

        cacheList.emplace_front(key, value);
        cacheMap[key] = cacheList.begin();
    }

    void del(int key) {
        unique_lock<shared_mutex> writeLock(mapMutex);
        lock_guard<mutex> listLock(listMutex);

        auto it = cacheMap.find(key);
        if (it != cacheMap.end()) {
            cacheList.erase(it->second);
            cacheMap.erase(it);
        }
    }

private:
    size_t capacity;
    list<pair<int, string>> cacheList;
    unordered_map<int, list<pair<int, string>>::iterator> cacheMap;

    mutable shared_mutex mapMutex; 
    mutable mutex listMutex; 
};
