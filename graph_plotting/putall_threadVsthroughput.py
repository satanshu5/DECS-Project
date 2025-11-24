import matplotlib.pyplot as plt

# Data
threads = [1, 2, 4, 6, 8, 12, 16, 32, 64]
throughput = [156.683, 262.503, 497.403, 659.727, 773.633,
              806.537, 826.65, 802.633, 654.83]

plt.figure(figsize=(10,6))

plt.plot(threads, throughput, marker='o', linewidth=2)

plt.xlabel("Threads")
plt.ylabel("Throughput (req/s)")
plt.title("Threads vs Throughput (putall workload)")
plt.grid(True)
plt.xscale("log", basex=2)   # optional for clarity

plt.show()
