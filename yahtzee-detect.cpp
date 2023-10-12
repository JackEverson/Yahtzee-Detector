#include <iostream>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
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
  cout << "The program captures frames from a video file, image sequence " "(01.jpg, 02.jpg ... 10.jpg) or camera connected to your computer." << endl
       << "Usage:\n"
       << av[0] << " <video file, image sequence or device number>" << endl
       << "q,Q,esc -- quit" << endl
       << "space   -- save frame" << endl << endl 
       << "\tTo capture from a camera pass the device number. To find the " "device number, try ls /dev/video*" << endl
       << "\texample: " << av[0] << " 0" << endl
       << "\tYou may also pass a video file instead of a device number" << endl
       << "\texample: " << av[0] << " video.avi" << endl
       << "\tYou can also pass the path to an image sequence and OpenCV will " "treat the sequence just like a video." << endl
       << "\texample: " << av[0] << " right%%.2d.jpg" << endl;
}

int process(VideoCapture &capture) {
  int n = 0;
  char filename[200];
  string window_name = "video | q or esc to quit";
  cout << "press space to save a picture. q or esc to quit" << endl;

  namedWindow(window_name, WINDOW_NORMAL); // resizable window;

  Mat frame;
  Mat output;
  

  for (;;) {
    capture >> frame;
    if (frame.empty())
      break;
    
    // resize(frame, frame, Size(frame_width, frame_height), INTER_LINEAR); // I may need this later
    output = frame.clone(); 

    const int frame_width = output.cols;
    const int frame_height = output.rows;

    

    // testing changing image matrix
    for (int x = 0; x < frame_width; x++){
      
      for (int y = 0; y < frame_height; y++){
        
        Vec3b intensity = output.at<Vec3b>(y, x);
        uchar blue = intensity.val[0];
        uchar green = intensity.val[1];
        uchar red = intensity.val[2];

        if (blue <= 50 && green <= 50 && red >= 150){
          output.at<Vec3b>(y, x) = (0, 0, 255);
        }
        // else {
        //   output.at<Vec3b>(y, x) = (255, 255, 255);
        // }

      }
    }

    imshow(window_name, output);
    char key = (char)waitKey(30); // delay N millis, usually long enough to display and capture input

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
