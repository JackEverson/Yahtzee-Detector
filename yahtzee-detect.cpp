#include <cstddef>
#include <iostream>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;

// hide the local functions in an anon namespace
namespace {
void help(char **av) {
  cout << "The program captures frames from a video file, image sequence "
          "(01.jpg, 02.jpg ... 10.jpg) or camera connected to your computer."
       << endl
       << "Usage:\n"
       << av[0] << " <video file, image sequence or device number>" << endl
       << "q,Q,esc -- quit" << endl
       << "space   -- save frame" << endl
       << endl
       << "\tTo capture from a camera pass the device number. To find the "
          "device number, try ls /dev/video*"
       << endl
       << "\texample: " << av[0] << " 0" << endl
       << "\tYou may also pass a video file instead of a device number" << endl
       << "\texample: " << av[0] << " video.avi" << endl
       << "\tYou can also pass the path to an image sequence and OpenCV will "
          "treat the sequence just like a video."
       << endl
       << "\texample: " << av[0] << " right%%.2d.jpg" << endl;
}

int process(VideoCapture &capture) {
  int n = 0;
  char filename[200];
  string window_name = "press space to save a picture. q or esc to quit";
  Point text_position(10, 30);
  int font_size = 1;

  // information on program operation
  cout << "press space to save a picture. q or esc to quit" << endl;

  namedWindow(window_name, WINDOW_NORMAL); // resizable window;
  // namedWindow("grey", WINDOW_NORMAL); // resizable window;

  Mat frame;
  Mat grey;
  Mat output;

  for (;;) {
    capture >> frame;
    if (frame.empty())
      break;

    // may need resizing later
    // resize(frame, frame, Size(frame_width, frame_height), INTER_LINEAR); // I

    const int frame_width = output.cols;
    const int frame_height = output.rows;

    // making an output frame and greyscale frame(for houghcircles)
    output = frame.clone();
    cvtColor(frame, grey, COLOR_BGR2GRAY);

    // bluring grey
    medianBlur(grey, grey, 5);

   /*  
    // testing changing image matrix
    for (int x = 0; x < frame_width; x++) {
      for (int y = 0; y < frame_height; y++) {

        Vec3b intensity = output.at<Vec3b>(y, x);
        uchar blue = intensity.val[0];
        uchar green = intensity.val[1];
        uchar red = intensity.val[2];

        if (blue <= 80 && green <= 80 && red >= 130) {
          grey.at<uchar>(y, x) = 0;
        } else {
          grey.at<uchar>(y, x) = 255;
          // grey.at<Vec3b>(y, x) = {255, 255, 255};
        }
      }
    }
 */

    // circles information in vector variable circles
    vector<Vec3f> circles;
    HoughCircles(
        grey, circles, HOUGH_GRADIENT, 1,
        grey.rows / 16, // detect circles with different distances to each other
        100, 30, 1, 30  // last two parameters effect min_radius & max_radius
    );

    for (int i = 0; i < circles.size(); i++) {
      Vec3i c = circles[i];
      Point center = Point(c[0], c[1]); // circle center as type point
      circle(output, center, 1, Scalar(0, 100, 100), 3,
             LINE_AA); // draw center point of detected circles
      int radius = c[2];
      circle(output, center, radius, Scalar(255, 0, 0), 3, LINE_AA);
    }

    int circle_count = circles.size();
    char *circle_info;
    sprintf(circle_info, "Circles: %i", circle_count);

    putText(output, circle_info, text_position, FONT_HERSHEY_DUPLEX, font_size,
            Scalar(0, 0, 0));

    imshow("grey", grey);
    imshow(window_name, output);

    char key = (char)waitKey(
        30); // delay N millis, usually long enough to display and capture input

    switch (key) {
    case 'q':
    case 'Q':
    case 27: // escape key
      return 0;

    case ' ': // Save an image
      snprintf(filename, sizeof(filename), "filename%.3d.jpg", n++);
      imwrite(filename, frame);
      cout << "Saved " << filename << endl;
      break;
    default:
      break;
    }
  }
  return 0;
}
} // namespace

int main(int ac, char **av) {
  cv::CommandLineParser parser(ac, av, "{help h||}{@input||}");
  if (parser.has("help")) {
    help(av);
    return 0;
  }

  std::string arg = parser.get<std::string>("@input");
  if (arg.empty()) {
    help(av);
    return 1;
  }
  VideoCapture capture(arg); // try to open string, this will attempt to open it
                             // as a video file or image sequence
  if (!capture.isOpened())   // if this fails, try to open as a video camera,
                             // through the use of an integer param
    capture.open(atoi(arg.c_str()));
  if (!capture.isOpened()) {
    cerr << "Failed to open the video device, video file or image sequence!\n"
         << endl;
    help(av);
    return 1;
  }

  return process(capture);
}
