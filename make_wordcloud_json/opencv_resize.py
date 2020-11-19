import sys
import time
import cv2

### VALUES
NUM_REPEAT = 10000

### Read source image
img_src = cv2.imread("lena.jpg")

### Run with CPU
time_start = time.time()
for i in range (NUM_REPEAT):
    img_dst = cv2.resize(img_src, (300, 300))
time_end = time.time()
print ("CPU = {0}".format((time_end - time_start) * 1000 / NUM_REPEAT) + "[msec]")