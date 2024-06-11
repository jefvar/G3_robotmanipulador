#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat imgGray,mask, imgHSV, imgBlur, imgCanny, imgDil, imgErode, img, imgResize, imgResize2;
int hmin = 0, smin = 110, vmin = 153;
int hmax = 19, smax = 240, vmax = 255;

void getContours(Mat imgDil) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); i++) {
        int area = contourArea(contours[i]);
        //cout << area << endl;
        vector<vector<Point>> conPoly(contours.size());
        vector<Rect> boundRect(contours.size());
        string objectType;
        if (area > 200 && area<4000) {
            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
            boundRect[i] = boundingRect(conPoly[i]);            
            drawContours(imgResize, conPoly, i, Scalar(255, 0, 255), 2);
            rectangle(imgResize, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
        }
    }
}

void main() {
    string path = "Resources/img.jpg";
    namedWindow("Trackbars", (640, 200));
    createTrackbar("Hue Min", "Trackbars", &hmin,179);
    createTrackbar("Hue Max", "Trackbars", &hmax, 179);
    createTrackbar("Sat Min", "Trackbars", &smin, 255);
    createTrackbar("Sat Max", "Trackbars", &smax, 255);
    createTrackbar("Val Min", "Trackbars", &vmin, 255);
    createTrackbar("Val Max", "Trackbars", &vmax, 255);

    while (true) {
        img = imread(path);

        imgResize2=img(Rect(Point(480, 63), Point(1210, 800)));
        resize(imgResize2, imgResize, Size(), 0.4, 0.4);
        cvtColor(imgResize, imgHSV, COLOR_BGR2HSV);
        
        Scalar lower(hmin,smin,vmin);
        Scalar upper(hmax,smax,vmax);
        inRange(imgHSV,lower,upper,mask);
        //imshow("Image Resize", imgResize);
        //imshow("Image HSV", imgHSV);
        getContours(mask);
        imshow("Image ", imgResize);
        imshow("Image Mask", mask);
        
        waitKey(1);
    }
    
    

   /* cvtColor(imgResize, imgGray, COLOR_BGR2GRAY);
    GaussianBlur(imgGray, imgBlur,Size(3,3), 3, 0);
    Canny(imgBlur, imgCanny, 25, 75);
    
    Mat kernel = getStructuringElement(MORPH_RECT, Size(1, 1));
    


    dilate(imgCanny, imgDil, kernel);
    erode(imgDil, imgErode, kernel);


    imshow("Image Resize", imgResize);
    imshow("Image Canny", imgCanny);
    imshow("Image Dilation", imgDil);
    imshow("Image Erode", imgErode);

    waitKey(0);*/

}