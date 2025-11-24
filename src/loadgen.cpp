#include "httplib.h"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <random>
#include <string>
#include <mutex>

std::atomic<long long> total_requests{0};
std::atomic<long long> total_latency_us{0};

std::mutex cout_mutex;

long long randomKey(int tid) {
    static thread_local long long counter = 0;
    return tid * 1000000LL + counter++;
}


void postKV(httplib::Client& cli, int tid) {
    long long key = randomKey(tid);
    std::string value = "val_" + std::to_string(rand() % 1000);

    std::string json = "{\"k\":" + std::to_string(key) +
                       ",\"v\":\"" + value + "\"}";

    cli.Post("/kv", json, "application/json");
}

void getKV(httplib::Client& cli, int tid) {
    long long key = randomKey(tid);
    cli.Get(("/kv?k=" + std::to_string(key)).c_str());
}

void getPopularKV(httplib::Client& cli) {
    static const std::vector<long long> hotKeys = {1, 2, 3, 4, 5};
    long long key = hotKeys[rand() % hotKeys.size()];
    cli.Get(("/kv?k=" + std::to_string(key)).c_str());
}

// ----------------------------------------------------------------------

void clientThread(int tid, int duration, const std::string& workload) {
    httplib::Client cli("127.0.0.1", 8080);
    cli.set_connection_timeout(5, 0);
    cli.set_read_timeout(5, 0);
    cli.set_write_timeout(5, 0);

    auto start = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(duration)) {
        auto req_start = std::chrono::steady_clock::now();

        try {
            if (workload == "putall") {
                postKV(cli, tid);
            } else if (workload == "getall") {
                getKV(cli, tid);
            } else if (workload == "getpopular") {
                getPopularKV(cli);
            } else {
                int r = rand() % 3;
                if (r == 0) postKV(cli, tid);
                else if (r == 1) getKV(cli, tid);
                else getPopularKV(cli);
            }
        } catch (...) {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cerr << "Thread " << tid << " request failed\n";
        }

        auto req_end = std::chrono::steady_clock::now();
        long long latency =
            std::chrono::duration_cast<std::chrono::microseconds>(req_end - req_start).count();

        total_requests++;
        total_latency_us += latency;
    }
}

// ----------------------------------------------------------------------

int main(int argc, char* argv[]) {
    if (argc < 7) {
        std::cerr << "Usage: " << argv[0]
                  << " --workload [putall|getall|getpopular|mixed] --threads N --duration S\n";
        return 1;
    }

    std::string workload = argv[2];
    int threads = std::stoi(argv[4]);
    int duration = std::stoi(argv[6]);

    std::vector<std::thread> pool;
    for (int i = 0; i < threads; i++) {
        pool.emplace_back(clientThread, i, duration, workload);
    }
    for (auto& t : pool) t.join();

    double throughput = total_requests.load() / (double)duration;
    double avg_latency_ms = 
        (total_latency_us.load() / (double)total_requests.load()) / 1000.0;

    std::cout << "Workload: " << workload << "\n";
    std::cout << "Total Reqs: " << total_requests.load() << "\n";
    std::cout << "Threads: " << threads << "\n";
    std::cout << "Duration: " << duration << "s\n";
    std::cout << "Throughput: " << throughput << " req/s\n";
    std::cout << "Average Latency: " << avg_latency_ms << " ms\n";

    return 0;
}
