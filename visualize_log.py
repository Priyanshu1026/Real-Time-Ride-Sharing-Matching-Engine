#this visualisation idea is a work-in-progress and may be improved further in the future.

import matplotlib.pyplot as plt
import pandas as pd
import re

# 1. Load Initial Positions from CSV
try:
    cols = ['type', 'id', 'x', 'y', 'param1', 'param2', 'time']
    df = pd.read_csv('samples/events.csv', names=cols, header=None)
except FileNotFoundError:
    print("Error: Could not find samples/events.csv")
    exit()

drivers = df[df['type'] == 'driver'].set_index('id')
riders = df[df['type'] == 'rider'].set_index('id')

# 2. Parse matches.log for connections
matches = []
try:
    with open('build/matches.log', 'r') as f:
        for line in f:
            m = re.search(r'MATCHED: Rider (\d+) with Driver (\d+)', line)
            if m:
                rider_id = int(m.group(1))
                driver_id = int(m.group(2))
                matches.append((rider_id, driver_id))
except FileNotFoundError:
    print("Warning: build/matches.log not found. Run the C++ sim first! Showing only positions.")

# 3. Plotting
plt.figure(figsize=(12, 10)) # Increase figure size slightly
plt.grid(True, linestyle='--', alpha=0.5)

# Add some padding to the axes limits so labels aren't cut off
plt.xlim(df['x'].min()-3, df['x'].max()+3)
plt.ylim(df['y'].min()-3, df['y'].max()+3)

# Define label style for better visibility
bbox_style = dict(facecolor='white', alpha=0.8, edgecolor='none', boxstyle='round,pad=0.2')

# Draw Drivers (Larger, semi-transparent blue circles)
plt.scatter(drivers['x'], drivers['y'], c='blue', s=150, label='Drivers', edgecolors='black', alpha=0.6, zorder=2)
for id, d in drivers.iterrows():
    # Offset label slightly up and right
    plt.text(d['x']+0.8, d['y']+0.8, f"D{id}", fontsize=10, color='blue', bbox=bbox_style, zorder=5)

# Draw Riders (Red X, semi-transparent, thicker line)
plt.scatter(riders['x'], riders['y'], c='red', marker='x', s=150, linewidth=2, label='Riders', alpha=0.8, zorder=3)
for id, r in riders.iterrows():
    # Offset label slightly down and left
    plt.text(r['x']-0.8, r['y']-0.8, f"R{id}", fontsize=10, color='red', bbox=bbox_style, zorder=5)

# Draw Match Lines (Solid green, thicker, more opaque)
for rid, did in matches:
    if rid in riders.index and did in drivers.index:
        rx, ry = riders.loc[rid].x, riders.loc[rid].y
        dx, dy = drivers.loc[did].x, drivers.loc[did].y
        
        # Solid green line, thicker than before
        plt.plot([rx, dx], [ry, dy], color='green', linestyle='-', linewidth=2, alpha=0.8, zorder=1)
        
        # Midpoint label with a clear box
        midx, midy = (rx + dx)/2, (ry + dy)/2
        plt.text(midx, midy, "MATCH", fontsize=9, color='green', weight='bold', ha='center', va='center',
                 bbox=dict(facecolor='white', edgecolor='green', boxstyle='round,pad=0.3', alpha=1.0), zorder=6)

plt.title('Ride Sharing Simulation: Matches (Improved Visibility)')
plt.xlabel('X Coordinate')
plt.ylabel('Y Coordinate')
plt.legend()
plt.show()