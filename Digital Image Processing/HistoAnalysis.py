import cv2
import numpy as np
import matplotlib.pyplot as plt


# Calculate the Image Histogram
def calc_histo(image):
  hist = cv2.calcHist([image], [0], None, [256], [0, 256])
  return hist

# Calculate The Cumulative Histogram
def calc_cumm_histo(hist):
  cumm_hist = np.cumsum(hist)
  return cumm_hist

# Shift Image Intensities by a Given Value
def shft_img(image, shift_value):
  shifted_image = np.clip(
      image.astype(np.int16) + shift_value,
      0,
      255
  ).astype(np.uint8)
  return shifted_image

# Display an Image
def dis_img(image, title):
  plt.figure(num=title)
  plt.imshow(image, cmap="gray")
  plt.title(title)
  plt.axis("off")
  plt.show()

# Display a Histogram Graph
def dis_histo(hist, title, xlabel, ylabel):
  plt.figure(num=title)
  plt.plot(hist)
  plt.title(title)
  plt.xlabel(xlabel)
  plt.ylabel(ylabel)
  plt.xlim([0, 256])
  plt.show()

file_name = input("Please Enter File Name With Extension: ")

# Read the image
original_image = cv2.imread(file_name)

# Check if the image was loaded successfully
if original_image is None:
    print("Error: Could not load the image.")

else:
    # Convert the image to grayscale
    original_image = cv2.cvtColor(
        original_image,
        cv2.COLOR_BGR2GRAY
    )

    # Display The Original Image
    dis_img(original_image, "Original Image")

    # Calculate and Display the Original Image Histogram
    org_hist = calc_histo(original_image)
    dis_histo(org_hist, "Original Histogram Graph", "Pixel Intensity", "Number of Pixels")

    # Calculate and Display the Cumulative Image Histogram
    cumulative_hist = calc_cumm_histo(org_hist)
    dis_histo(cumulative_hist, "Cumulative Histogram", "Pixel Intensity", "Cumulative Number of Pixels")

    while True:
        try:
            shift_value = int(input("Enter shift value: "))
            break
        except ValueError:
            print("Error: Please enter a valid integer. Try again.")

    # Generate and Display the Shifted Image
    shifted_image = shft_img(original_image, shift_value)
    dis_img(shifted_image, f"Shifted Image (Shift = {shift_value})")

    # Calculate and Display the Shifted Image Histogram
    shft_hist = calc_histo(shifted_image)
    dis_histo(shft_hist, f"Shifted Histogram (Shift = {shift_value})", "Pixel Intensity", "Number of Pixels")

    # Equalize and Display the Original Image
    equalized_image = cv2.equalizeHist(original_image)
    dis_img(equalized_image, "Equalized Image")

    # Calculate and Display the Equalized Image Histogram
    equalized_hist = calc_histo(equalized_image)
    dis_histo(equalized_hist, "Equalized Histogram", "Pixel Intensity", "Number of Pixels")

    # Calculate and Display the Cumulative Equalized Image Histogram
    cumulative_equalized_hist = calc_cumm_histo(equalized_hist)
    dis_histo(cumulative_equalized_hist, "Equalized Cumulative Histogram", "Pixel Intensity", "Cumulative Number of Pixels")