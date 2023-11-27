from opensimplex import OpenSimplex
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
import random


def generate_perlin_noise(rows, cols, seed):
    scale = 10.0
    octaves = 6

    world = np.zeros((rows, cols))
    noise_generator = OpenSimplex(seed=seed)

    for i in range(rows):
        for j in range(cols):
            world[i][j] = noise_generator.noise2(i / scale, j / scale)

    world = (world - np.min(world)) / (np.max(world) - np.min(world))

    return world


def classify_terrain(elevation):
    if elevation < 0.05:
        return (0, 0, 1)  # Blue (DEEP OCEAN)
    elif elevation < 0.15:
        return (0, 0, 0.5)  # Dark Blue (SHALLOW OCEAN)
    elif elevation < 0.25:
        return (0, 1, 0)  # Green (COASTAL AREA)
    elif elevation < 0.35:
        return (0, 0.5, 0)  # Dark Green (LOWLANDS)
    elif elevation < 0.45:
        return (0.8, 0.8, 0)  # Light Yellow (DESERT)
    elif elevation < 0.55:
        return (0.5, 0.5, 0)  # Olive (FOOTHILLS)
    elif elevation < 0.65:
        return (0.3, 0.3, 0)  # Dark Olive (MOUNTAIN BASE)
    elif elevation < 0.75:
        return (0.7, 0.7, 0.7)  # Light Gray (HILLS)
    elif elevation < 0.85:
        return (0.5, 0.5, 0.5)  # Gray (MOUNTAINS)
    elif elevation < 0.95:
        return (0.8, 0.8, 0.8)  # Light Gray (HIGH MOUNTAINS)
    else:
        return (1, 1, 1)  # White (HIGHEST MOUNTAINS)


ROWS, COLS = 200, 180
seed_value = random.randint(0, 100)
perlin_noise = generate_perlin_noise(ROWS, COLS, seed_value)
classified_terrain = np.zeros((ROWS, COLS, 3), dtype=np.float32)

for i in range(ROWS):
    for j in range(COLS):
        elevation = perlin_noise[i, j]
        rgb = classify_terrain(elevation)
        classified_terrain[i, j] = rgb

terrain_colors = {
    0: (0, 0, 1),  # Blue (DEEP OCEAN)
    1: (0, 0, 0.5),  # Dark Blue (SHALLOW OCEAN)
    2: (0, 1, 0),  # Green (COASTAL AREA)
    3: (0, 0.5, 0),  # Dark Green (LOWLANDS)
    4: (0.8, 0.8, 0),  # Light Yellow (DESERT)
    5: (0.5, 0.5, 0),  # Olive (FOOTHILLS)
    6: (0.3, 0.3, 0),  # Dark Olive (MOUNTAIN BASE)
    7: (0.7, 0.7, 0.7),  # Light Gray (HILLS)
    8: (0.5, 0.5, 0.5),  # Gray (MOUNTAINS)
    9: (0.8, 0.8, 0.8),  # Light Gray (HIGH MOUNTAINS)
    10: (1, 1, 1),  # White (HIGHEST MOUNTAINS)
}

cmap = LinearSegmentedColormap.from_list(
    "terrain", list(terrain_colors.values()), N=len(terrain_colors)
)

# Save the classified biome values and elevation to files
with open("data/biome.txt", "w") as f:
    for row in classified_terrain:
        for rgb in row:
            f.write(f"{rgb[0]:.5f} {rgb[1]:.5f} {rgb[2]:.5f} ")
        f.write("\n")
np.savetxt("data/elevation.txt", perlin_noise, fmt="%1.5f", delimiter=" ")

# Display the classified terrain using matplotlib
plt.imshow(classified_terrain, interpolation="nearest", cmap=cmap)
plt.colorbar()
plt.show()
