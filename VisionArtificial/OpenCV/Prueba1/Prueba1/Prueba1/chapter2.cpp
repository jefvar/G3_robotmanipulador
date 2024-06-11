#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


/////////////////  Basic Functions  //////////////////////

void main() {

    string path = "Resources/test.png";
    Mat img = imread(path);
    Mat imgGray, imgCanny, imgDil, imgErode;
    cvtColor(img,imgGray,COLOR_BGR2GRAY);
    Canny(img, imgCanny, 50, 150);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(3,3));
    dilate(imgCanny, imgDil,kernel);
    erode(imgDil,imgErode,kernel);

    imshow("Image", img);
    imshow("Image Gray", imgGray);
    imshow("Image Canny", imgCanny);
    imshow("Image Dilation", imgDil);
    imshow("Image Erode", imgErode);
    waitKey(0);

}
