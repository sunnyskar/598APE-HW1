import matplotlib.pyplot as plt

labels = ['Original', 'Memory', 'Compiler', 'Parallelization']
speeds = [2.446816, 1.830028, 0.712313, 0.375353]

plt.figure(figsize=(15, 6))

plt.bar(labels, speeds, color='skyblue')

plt.xlabel('Optimization Techniques')
plt.ylabel('Speed')
plt.title('Speed Comparison of Optimization Techniques')

plt.show()


import matplotlib.pyplot as plt
import numpy as np

labels = ['Globe Original', 'Globe Efficiencies', 'Globe BVH', 
          'Elephant Original', 'Elephant Efficiencies', 'Elephant BVH']
speeds = [144.645523, 115.586830, 8.563027, 
          np.nan, 33.956345, 0.586085]

plt.figure(figsize=(15, 6))

bars = plt.bar(labels, speeds, color='skyblue')

for bar, speed in zip(bars, speeds):
    if np.isnan(speed):
        bar.set_height(0)

plt.xlabel('Model Type')
plt.ylabel('Time (Seconds)')
plt.title('Performance Comparison for Globe and Elephant')

plt.show()
