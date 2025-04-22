from PIL import Image
import os

# Path to the atlas image
atlas_path = "Assets/atlas.png"
output_dir = "Assets/split_textures"

# Atlas dimensions and block size
atlas_width, atlas_height = 256, 256
block_size = 16

# Create output directory if it doesn't exist
os.makedirs(output_dir, exist_ok=True)

# Open the atlas image
atlas = Image.open(atlas_path)

# Split the atlas into individual blocks
for row in range(atlas_height // block_size):
    for col in range(atlas_width // block_size):
        # Calculate the bounding box for the current block
        left = col * block_size
        upper = row * block_size
        right = left + block_size
        lower = upper + block_size

        # Crop the block from the atlas
        block = atlas.crop((left, upper, right, lower))

        # Save the block as an individual image
        block_filename = f"block_{row}_{col}.png"
        block.save(os.path.join(output_dir, block_filename))

print(f"Textures split and saved in '{output_dir}'")