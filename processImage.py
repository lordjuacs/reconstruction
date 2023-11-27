from PIL import Image
from pathlib import Path
import sys


def resize_image_and_convert(
    image_path, max_pixels, height_map_file, colors_file, img_name
):
    # Open the image
    img = Image.open(image_path)

    # Check the image mode
    if img.mode != "RGB":
        # Convert the image to RGB mode if it's not already
        img = img.convert("RGB")

    # Calculate the aspect ratio of the original image
    aspect_ratio = img.width / img.height
    width = img.width
    height = img.height
    # Calculate the new dimensions while maintaining the aspect ratio
    best_width = 0
    best_height = 0
    best_value = 0

    for new_width in range(1, width + 1):
        new_height = int(new_width * (height / width))
        # Ensure the condition is satisfied
        if 2 * (new_width - 1) * (new_height - 1) < max_pixels:
            value = 2 * (new_width - 1) * (new_height - 1)
            if value > best_value:
                best_value = value
                best_width = new_width
                best_height = new_height
    # Resize the image
    resized_img = img.resize((best_width, best_height), Image.BOX)

    # Flip the image horizontally
    resized_img = resized_img.transpose(Image.FLIP_LEFT_RIGHT)

    # Convert the resized image to grayscale
    img_gray = resized_img.convert("L")

    # Get the pixel data
    pixel_data = list(img_gray.getdata())

    # Get image width and height
    width, height = resized_img.size

    # Reshape the pixel data into a 2D array (height map)
    height_map = [pixel_data[i : i + width] for i in range(0, len(pixel_data), width)]

    # Normalize the height map to a range of 0 to 1
    max_height = max(map(max, height_map))
    height_map_normalized = [
        [height / max_height for height in row] for row in height_map
    ]

    # Save the normalized height map as a text file
    with open(height_map_file, "w") as f:
        for row in height_map_normalized:
            f.write(" ".join(map(str, row)) + "\n")

    # Get the RGB values
    rgb_values = list(resized_img.getdata())

    # Reshape the RGB values into a 2D array
    colors = [
        rgb_values[i : i + best_width] for i in range(0, len(rgb_values), best_width)
    ]

    # Normalize the RGB values to a range of 0 to 1
    colors_normalized = [
        [(r / 255.0, g / 255.0, b / 255.0) for r, g, b in row] for row in colors
    ]

    # Save the normalized colors as a text file
    with open(colors_file, "w") as f:
        for row in colors_normalized:
            f.write(" ".join([f"{r} {g} {b}" for r, g, b in row]) + "\n")

    with open("meta.data", "w") as meta_file:
        meta_file.write(f"{img_name}\n")
        meta_file.write(f"{best_height} {best_width}\n")
    print("rows:", best_height, "cols:", best_width)
    print(f"Normalized Height map saved to {height_map_file}")
    print(f"Normalized Colors saved to {colors_file}")
    print("meta.data saved to meta.data")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python imageProcessing.py <image_name>")
        sys.exit(1)

    # Replace 'input_image.jpg' with the path to your input image
    input_image_path = sys.argv[1]
    img_name = Path(input_image_path).stem

    # Replace 9000 with the desired maximum pixel count
    max_pixels = 9000

    # Replace 'height_map.txt' with the desired output height map file name
    height_map_file_path = f"data/elevation/{img_name}.e"

    # Replace 'colors.txt' with the desired output colors file name
    colors_file_path = f"data/rgb/{img_name}.rgb"

    resize_image_and_convert(
        input_image_path, max_pixels, height_map_file_path, colors_file_path, img_name
    )
