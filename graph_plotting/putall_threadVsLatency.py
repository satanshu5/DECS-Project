import matplotlib.pyplot as plt

# Data
threads = [1, 2, 4, 6, 8, 12, 16, 32, 64]
latency = [6.38161, 7.61832, 8.0411, 9.09415, 10.3403,
           14.878, 19.355, 39.8723, 97.7653]   # ms

plt.figure(figsize=(10,6))

plt.plot(threads, latency, marker='o', linewidth=2)

plt.xlabel("Threads")
plt.ylabel("Average Latency (ms)")
plt.title("Threads vs Average Latency (PUT_ALL workload)")
plt.grid(True)
plt.xscale("log", basex=2)   # optional: cleaner scaling

plt.show()
