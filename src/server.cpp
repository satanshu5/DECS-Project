#include "server.h"
#include "metrics.h"
#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// -------------------- Struct for KV Operations --------------------
struct KVPair {
    int k;
    string v;
};

// JSON serialization helper
void from_json(const json &j, KVPair &kv) {
    j.at("k").get_to(kv.k);
    j.at("v").get_to(kv.v);
}

// -------------------- KVServer Implementation --------------------
KVServer::KVServer(int port_, int workers, size_t cache_size,
                   const string &db_host,
                   const string &db_user,
                   const string &db_pass,
                   const string &db_name)
    : port(port_), cache(cache_size),
      db(db_host, db_user, db_pass, db_name),
      worker_count(workers) {}

void KVServer::run() {
    httplib::Server svr;

    // -------------------- POST /kv (Create or Update) --------------------
    svr.Post("/kv", [this](const httplib::Request &req, httplib::Response &res) {
        GLOBAL_METRICS.req_total++;
        try {
            auto kv = json::parse(req.body).get<KVPair>();
            bool ok = db.put(kv.k, kv.v);
            if (ok) {
                cache.put(kv.k, kv.v);
                GLOBAL_METRICS.req_success++;
                res.status = 200;
                res.set_content("OK", "text/plain");
            } else {
                res.status = 500;
                res.set_content("DB Error", "text/plain");
            }
        } catch (const exception &e) {
            res.status = 400;
            res.set_content(string("Invalid JSON: ") + e.what(), "text/plain");
        }
    });

    // -------------------- GET /kv?k=KEY --------------------
    svr.Get("/kv", [this](const httplib::Request &req, httplib::Response &res) {
        GLOBAL_METRICS.req_total++;

        if (!req.has_param("k")) {
            res.status = 400;
            res.set_content("Missing key", "text/plain");
            return;
        }

        int k = stoi(req.get_param_value("k"));
        string val;
        bool ok = false;

        if (cache.get(k, val)) {
            GLOBAL_METRICS.cache_hits++;
            ok = true;
        } else {
            GLOBAL_METRICS.cache_misses++;
            if (db.get(k, val)) {
                cache.put(k, val);
                ok = true;
            }
        }

        if (ok) {
            GLOBAL_METRICS.req_success++;
            json j = {{"k", k}, {"v", val}};
            res.status = 200;
            res.set_content(j.dump(), "application/json");
        } else {
            res.status = 404;
            res.set_content("NOTFOUND", "text/plain");
        }
    });

    // -------------------- DELETE /kv?k=KEY --------------------
    svr.Delete("/kv", [this](const httplib::Request &req, httplib::Response &res) {
        GLOBAL_METRICS.req_total++;

        if (!req.has_param("k")) {
            res.status = 400;
            res.set_content("Missing key", "text/plain");
            return;
        }

        int k = stoi(req.get_param_value("k"));
        bool ok = db.del(k);
        if (ok) {
            cache.del(k);
            GLOBAL_METRICS.req_success++;
            res.status = 200;
            res.set_content("Deleted", "text/plain");
        } else {
            res.status = 500;
            res.set_content("DB Error", "text/plain");
        }
    });

    cout << "Server running on port " << port
         << " using httplib built-in thread pool ("
         << worker_count << " workers)" << endl;

    svr.new_task_queue = [this] { return new httplib::ThreadPool(worker_count); };
    svr.listen("0.0.0.0", port);
}
