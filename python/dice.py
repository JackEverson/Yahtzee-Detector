from imutils.video import VideoStream
import numpy as np
import cv2 as cv2
import imutils

# dice color in HSV
# measure these while on a typical expected background
greenLower = (43, 83, 103)
greenUpper = (99, 115, 182)
redLower = (137,26,39)
redUpper = (202,59,208)
whiteLower = (0,0,0) 
whiteUpper = (191, 160, 150) 

font = cv2.FONT_HERSHEY_SIMPLEX
topLeftCornerOfText = (10,30)
fontScale = 1
fontColor = (0,0,0)
lineType = 2

vs = VideoStream(src=2).start() #1=external USB cam

while True:
	frame = vs.read()
	if frame is None:
		continue

	frame = imutils.resize(frame, width=600)
	blurred = cv2.GaussianBlur(frame, (11, 11), 0)
	hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)
	
	#try red?
	mask = cv2.inRange(hsv, redLower, redUpper)
	mask = cv2.bitwise_not(mask) # invert
	circles = cv2.HoughCircles(mask, cv2.HOUGH_GRADIENT, 1, 20, param1=30, param2=15, minRadius=6, maxRadius=30)

	if circles is not None:
		circles = np.round(circles[0, :]).astype("int")
		if ((len(circles) > 0) and (len(circles) <=6)): # no point guessing
			cv2.putText(mask,"RED: " + str(len(circles)), topLeftCornerOfText, font, fontScale,fontColor,lineType)
	else:
		# try green?
		mask = cv2.inRange(hsv, greenLower, greenUpper)
		mask = cv2.bitwise_not(mask) # invert
		circles = cv2.HoughCircles(mask, cv2.HOUGH_GRADIENT, 1, 20, param1=30, param2=15, minRadius=6, maxRadius=30)
		if circles is not None:
			output = mask.copy()
			circles = np.round(circles[0, :]).astype("int")
			if ((len(circles) > 0) and (len(circles) <=6)):
				cv2.putText(mask,"GREEN: " + str(len(circles)), topLeftCornerOfText, font, fontScale, fontColor, lineType)
		else:
			# try white
			mask = cv2.inRange(hsv, whiteLower, whiteUpper)
			mask = cv2.bitwise_not(mask) # for white, depending on background color, remark this out
			circles = cv2.HoughCircles(mask, cv2.HOUGH_GRADIENT, 1, 20, param1=30, param2=15, minRadius=6, maxRadius=30)
			if circles is not None:
				output = mask.copy()
				circles = np.round(circles[0, :]).astype("int")
				if ((len(circles) > 0) and (len(circles) <=6)):
					cv2.putText(mask,"WHITE: " + str(len(circles)), topLeftCornerOfText, font, fontScale,fontColor,lineType)

	cv2.imshow("Preview", mask)
	key = cv2.waitKey(1) & 0xFF
	if key == ord("q"):
		break

vs.release()
cv2.destroyAllWindows()
