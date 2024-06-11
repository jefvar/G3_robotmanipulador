#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


/////////////////  Basic Functions  //////////////////////

void main() {

    string path = "Resources/Tablero.png";
    Mat img = imread(path);
    Mat imgResize;
    cout << img.size() << endl;
    resize(img, imgResize, Size(),0.4,0.4);
    

    //imshow("Image", img);
    imshow("Image Resize", imgResize);
   
    waitKey(0);

}
