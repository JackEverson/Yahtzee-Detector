#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;

int main (int argc, char *argv[]) {
    if (argc != 2){
        printf("usage: yahtzee-detect <image_path>\n");
        return -1;
    }

    Mat image;
    image = imread(argv[1], IMREAD_COLOR);

    if (!image.data){
        printf("No image data\n");
        return -1;
    }

    namedWindow("Display Image", WINDOW_NORMAL);
    imshow("Dispay Image", image);

    waitKey(0);
    return 0;
}
