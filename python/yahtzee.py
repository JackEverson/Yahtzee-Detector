import cv2
import imutils
import numpy as np
import sys

from imutils.video import VideoStream

# variables
colourLower = (0, 0, 100)
colourUpper = (40, 40, 250)

#OpenCV settings
font = cv2.FONT_HERSHEY_SIMPLEX
font_scale = 1
fontColour = (0,0,0)
lineType = 2
text_position = (10,30)

# setting up camera
# vs = VideoStream(src=2).start()
vs = cv2.VideoCapture(2) 

x = 0
# continuous loop for video stream
while True: 
    success, frame = vs.read()

    if not success:
        print("error, no frame")
        sys.exit(-1)
        
    # processing frame
    frame = imutils.resize(frame, width=600)
    # blurred = cv2.GaussianBlur(frame, (11, 11), 0)
    # hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)

    mask = cv2.inRange(frame, colourLower, colourUpper)
    mask = cv2.bitwise_not(mask)
    circles = cv2.HoughCircles(mask, cv2.HOUGH_GRADIENT, 1, 20, param1=30, param2=15, minRadius=4, maxRadius=30)
    output = frame.copy()
    if circles is not None:
        circles = np.round(circles[0, :]).astype("int")
        cv2.putText(output, str(len(circles)) , text_position, font, font_scale, fontColour, lineType)

        for circle in circles:
            cv2.circle(output, (circle[0], circle[1]), circle[2], (255,0,0))
        

    cv2.imshow("output", output)
    # cv2.imshow("blurred", blurred)
    # cv2.imshow("hsv", hsv)
    cv2.imshow("mask", mask)
    key = cv2.waitKey(1) 
    if key == ord("q"):
        print("q input from user, shutting down program")
        break

vs.release()
cv2.destroyAllWindows()
