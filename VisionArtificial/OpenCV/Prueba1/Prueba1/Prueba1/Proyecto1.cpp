#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat img,imgResize, imgResize2, tablero;

// hmin,smin,vmin,hmax,smax,vmax
vector<vector<int>> myColors{ {6,12,64,40,75,121}, //cuadrado
                              {0,0,62,12,127,139}, //rojo
                              {38,0,0,121,102,102} }; //celeste
vector<Scalar> myColorValues{ {0,255,0},{255,0,0},{116,255,255} };
vector<Point> tablero_tl;
vector<Point> tablero_br;
vector<Point2i> fichaRoja;
vector<Point2i> fichaAzul;
vector<int> matriz={0,0,0,0,0,0,0,0,0};
// Función de comparación para ordenar los puntos por coordenada Y

bool compararPorY(const Point& p1, const Point& p2) {
    return p1.y < p2.y;
}

// Función de comparación para ordenar los puntos
bool compararPuntos(const Point& p1, const Point& p2) {
    const double epsilon = 20; // Umbral para comparar coordenadas y casi iguales
    if (std::abs(p1.y - p2.y) < epsilon) {
        return p1.x < p2.x; // Ordenar por coordenada x si y son casi iguales
    }
    else {
        return p1.y < p2.y; // Ordenar por coordenada x en caso contrario
    }
}

void getContours(Mat imgDil, int j) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(imgDil,contours,hierarchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size();i++) {
        int area = contourArea(contours[i]);
        vector<vector<Point>> conPoly(contours.size());
        vector<Rect> boundRect(contours.size());
        string objectType;
        if (j==0) {
            if (area > 200 && area < 4000) {
                float peri = arcLength(contours[i], true);
                approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
                boundRect[i] = boundingRect(conPoly[i]);
                int objCor = (int)conPoly[i].size();                
                drawContours(imgResize, conPoly, i, Scalar(255, 0, 255), 2);
                rectangle(imgResize, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
                tablero_tl.push_back(boundRect[i].tl());
                tablero_br.push_back(boundRect[i].br()); 
            }
        }
        if (j == 1) {
            if (area > 100 && area < 1000) {
                float peri = arcLength(contours[i], true);
                approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
                boundRect[i] = boundingRect(conPoly[i]);
                int objCor = (int)conPoly[i].size();
                drawContours(imgResize, conPoly, i, Scalar(255, 0, 255), 2);
                rectangle(imgResize,boundRect[i].tl(),boundRect[i].br(), Scalar(0, 0, 255), 5);
                fichaRoja.push_back({boundRect[i].tl().x + boundRect[i].width / 2, boundRect[i].tl().y + boundRect[i].height / 2 });
            }
        }  
        if (j == 2) {
            if (area > 100 && area < 1000) {
                float peri = arcLength(contours[i], true);
                approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
                boundRect[i] = boundingRect(conPoly[i]);
                int objCor = (int)conPoly[i].size();
                drawContours(imgResize, conPoly, i, Scalar(255, 0, 255), 2);
                rectangle(imgResize, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 5);
                fichaAzul.push_back({ boundRect[i].tl().x + boundRect[i].width / 2, boundRect[i].tl().y + boundRect[i].height / 2 });
            }
        }
    }
}
void findColor(Mat img) {    
    Mat imgHSV;
    cvtColor(img, imgHSV, COLOR_BGR2HSV);
    for (int i = 0; i < myColors.size(); i++) {
        Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
        Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);
        Mat mask;
        inRange(imgHSV, lower, upper, mask);
        imshow("Imagen..", imgHSV);
        getContours(mask,i);
    }
}
void ubicacionFichas() {        
    sort(tablero_tl.begin(), tablero_tl.end(),compararPuntos);
    sort(tablero_br.begin(), tablero_br.end(), compararPuntos);
    for (int i = 0; i < tablero_tl.size(); i++) {
        for (int j = 0; j < fichaAzul.size(); j++) {
            if (fichaAzul[j].x > tablero_tl[i].x && fichaAzul[j].x < tablero_br[i].x) {
                if (fichaAzul[j].y > tablero_tl[i].y && fichaAzul[j].y < tablero_br[i].y) {
                    matriz[i] = 1; //hay ficha azul
                }
            }
        }
        for (int j = 0; j < fichaRoja.size(); j++) {
            if (fichaRoja[j].x > tablero_tl[i].x && fichaRoja[j].x < tablero_br[i].x) {
                if (fichaRoja[j].y > tablero_tl[i].y && fichaRoja[j].y < tablero_br[i].y) {
                    matriz[i]=2; //hay ficha roja
                }
            }
        }
    }
}
void main() {
    string path = "Resources/img.jpg";
    string path2 = "Resources/Tablero.png";
    img = imread(path);    
    imgResize2 =img(Rect(Point(480, 63), Point(1210, 800)));
    resize(imgResize2, imgResize, Size(), 0.4, 0.4);
    findColor(imgResize); 
    imshow("Imagen", imgResize);
    ubicacionFichas();
    cout << fichaRoja << endl;
    cout << fichaAzul << endl;
    cout << tablero_tl << endl;
    cout << tablero_br << endl;
    for (int i = 0; i < matriz.size(); i++){
        cout << matriz[i] << endl;
    }    
    waitKey(0);
}