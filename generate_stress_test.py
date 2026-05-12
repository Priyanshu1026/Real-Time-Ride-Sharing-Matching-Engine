import random

NUM_DRIVERS = 10000
NUM_RIDERS = 100000
GRID_SIZE = 1000

with open("samples/events_stress.csv", "w") as f:
    # Generate Drivers
    for i in range(NUM_DRIVERS):
        x, y = round(random.uniform(0, GRID_SIZE), 2), round(random.uniform(0, GRID_SIZE), 2)
        rating = round(random.uniform(3.0, 5.0), 1)
        f.write(f"driver,{i},{x},{y},{rating}\n")
        
    # Generate Riders
    for i in range(NUM_RIDERS):
        px, py = round(random.uniform(0, GRID_SIZE), 2), round(random.uniform(0, GRID_SIZE), 2)
        dx, dy = round(random.uniform(0, GRID_SIZE), 2), round(random.uniform(0, GRID_SIZE), 2)
        time = random.randint(0, 60000) # Events spread across 60 seconds
        f.write(f"rider,{i},{px},{py},{dx},{dy},{time}\n")

print(f"Generated {NUM_DRIVERS} drivers and {NUM_RIDERS} riders!")