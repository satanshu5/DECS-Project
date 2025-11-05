#pragma once
#include <atomic>

struct Metrics {
  std::atomic<uint64_t> req_total{0};
  std::atomic<uint64_t> req_success{0};
  std::atomic<uint64_t> cache_hits{0};
  std::atomic<uint64_t> cache_misses{0};
};

static Metrics GLOBAL_METRICS;
