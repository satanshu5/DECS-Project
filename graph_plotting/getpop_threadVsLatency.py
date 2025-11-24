import matplotlib.pyplot as plt

# Data
threads = [1, 2, 4, 6, 8, 10, 16, 32, 64, 128]
latency = [0.357461, 0.415341, 0.656642, 0.897353, 1.29081,
           1.58019, 2.56642, 4.97204, 9.93319, 19.9087]  # in ms

plt.figure(figsize=(10,6))

plt.plot(threads, latency, marker='o', linewidth=2)

plt.xlabel("Threads")
plt.ylabel("Average Latency (ms)")
plt.title("Threads vs Average Latency (getpopular workload)")
plt.grid(True)
plt.xscale("log", basex=2)   # optional: makes thread scaling easier

plt.show()
