import matplotlib.pyplot as plt

# Data
threads = [1, 2, 4,  8, 10, 16, 32, 64, 128]
throughput = [2792.77, 4808.24, 6085.69,  6197.53,
              6331.36, 6240.42, 6445.62, 6450.52, 6439.54]

plt.figure(figsize=(10,6))

plt.plot(threads, throughput, marker='o', linewidth=2)

plt.xlabel("Threads")
plt.ylabel("Throughput (req/s)")
plt.title("Threads vs Throughput (getpopular workload)")
plt.grid(True)
plt.xscale("log", basex=2)   # optional: makes thread scaling clearer

plt.show()
