#include "httplib.h"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <random>
#include <string>
#include <mutex>

using namespace std;
atomic<long long> total_requests{0};
atomic<long long> total_latency_us{0};

mutex cout_mutex;

long long randomKey(int th_id) {
    static thread_local long long counter = 0;
    return th_id * 1000000LL + counter++;
}


void postKV(httplib::Client& cli, int th_id) {
    long long key = randomKey(th_id);
    string value = "val_" + to_string(rand() % 1000);

    string json = "{\"k\":" + to_string(key) + ",\"v\":\"" + value + "\"}";

    cli.Post("/kv", json, "application/json");
}

void getKV(httplib::Client& cli, int th_id) {
    long long key = randomKey(th_id);
    cli.Get(("/kv?k=" + to_string(key)).c_str());
}

void getPopularKV(httplib::Client& cli) {
    static const vector<long long> hotKeys = {1, 2, 3, 4, 5};
    long long key = hotKeys[rand() % hotKeys.size()];
    cli.Get(("/kv?k=" + to_string(key)).c_str());
}

// ----------------------------------------------------------------------

void clientThread(int th_id, int duration, const string& workload) {
    httplib::Client cli("127.0.0.1", 8080);
    cli.set_connection_timeout(5, 0);
    cli.set_read_timeout(5, 0);
    cli.set_write_timeout(5, 0);

    auto start = chrono::steady_clock::now();

    while (chrono::steady_clock::now() - start < chrono::seconds(duration)) {
        auto req_start = chrono::steady_clock::now();

        try {
            if (workload == "putall") {
                postKV(cli, th_id);
            } else if (workload == "getall") {
                getKV(cli, th_id);
            } else if (workload == "getpopular") {
                getPopularKV(cli);
            }
        } catch (...) {
            lock_guard<mutex> lock(cout_mutex);
            cerr << "Thread " << th_id << " request failed\n";
        }

        auto req_end = chrono::steady_clock::now();
        long long latency =
            chrono::duration_cast<chrono::microseconds>(req_end - req_start).count();

        total_requests++;
        total_latency_us += latency;
    }
}

// ----------------------------------------------------------------------

int main(int argc, char* argv[]) {


    string workload = argv[2];
    int threads = stoi(argv[4]);
    int duration = stoi(argv[6]);

    vector<thread> pool;
    for (int i = 0; i < threads; i++) {
        pool.emplace_back(clientThread, i, duration, workload);
    }
    for (auto& t : pool) t.join();

    double throughput = total_requests.load() / (double)duration;
    double avg_latency_ms = 
        (total_latency_us.load() / (double)total_requests.load()) / 1000.0;

    cout << "Workload: " << workload << "\n";
    cout << "Total Reqs: " << total_requests.load() << "\n";
    cout << "Threads: " << threads << "\n";
    cout << "Duration: " << duration << "s\n";
    cout << "Throughput: " << throughput << " req/s\n";
    cout << "Mean Latency: " << avg_latency_ms << " ms\n";

    return 0;
}
