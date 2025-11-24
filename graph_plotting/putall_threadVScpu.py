import matplotlib.pyplot as plt

# Data
threads = [1, 2, 4, 6, 8, 12, 16, 32, 64]
cpu = [65, 85, 90, 90, 90, 91, 91, 91, 91]   # averaged CPU %

plt.figure(figsize=(10,6))

plt.plot(threads, cpu, marker='o', linewidth=2)

plt.xlabel("Threads")
plt.ylabel("CPU Utilization (%)")
plt.title("Threads vs CPU Utilization (PUT_ALL workload)")
plt.grid(True)
plt.xscale("log", basex=2)   # optional

plt.show()
