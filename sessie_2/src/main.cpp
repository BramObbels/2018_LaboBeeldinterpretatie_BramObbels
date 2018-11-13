#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;


const int max_value_H = 360/2;
const int max_value = 255;

int low_H = 0, low_S = 0, low_V = 0;
int high_H = max_value_H, high_S = max_value, high_V = max_value;

static void on_low_H_thresh_trackbar(int, void *)
{
    low_H = low_H;

}
static void on_high_H_thresh_trackbar(int, void *)
{
    high_H = high_H;

}
static void on_low_S_thresh_trackbar(int, void *)
{
    low_S = low_S;

}
static void on_high_S_thresh_trackbar(int, void *)
{
    high_S = high_S;

}
static void on_low_V_thresh_trackbar(int, void *)
{
    low_V = low_V;

}
static void on_high_V_thresh_trackbar(int, void *)
{
    high_V = high_V;

}


int main(int argc, const char ** argv){

    //////////////////////////////////////////////////////////////////////////////////////
    ////                                 SETUP                                         ///
    //////////////////////////////////////////////////////////////////////////////////////

    ///______________INLEZEN VAN ARGUMENTEN_______________
    CommandLineParser parser(argc, argv,
        "{help h usage? |    |show this message}"
        "{bord1 Sign1   |    |(required) message verkeersbord1}"
        "{bord2 Sign2   |    |(required) message verkeersbord2}"
        "{bord3 Sign3   |    |(required) message verkeersbord3}"
        "{bord4 Sign4   |    |(required) message verkeersbord4}"

    );

    if(parser.has("help")){
        parser.printMessage();
        cout << "TIP: gebruik voledige padnamen";
        return -1;
    }

    string bord1(parser.get<string>("bord1"));
    string bord2(parser.get<string>("bord2"));
    string bord3(parser.get<string>("bord3"));
    string bord4(parser.get<string>("bord4"));


    if(bord1.empty() || bord2.empty() || bord3.empty() || bord4.empty()){
        parser.printMessage();
        return -1;
    }


    ///_____________AANMAKEN AVARIABELEN____________________

    Mat sign1;
    Mat sign2;
    Mat sign3;
    Mat sign4;
    /// ____________INLEZEN IMAGES_____________________________

    cout << bord1 << std::endl;
    cout << bord2 << std::endl;
    cout << bord3 << std::endl;
    cout << bord4 << std::endl;

    sign1 = imread(bord1);
    sign2 = imread(bord2);
    sign3 = imread(bord3);
    sign4 = imread(bord4);


    ///_____________TESTEN OF IMAGES CORRECT INGELADEN ZIJN________________

    if(sign1.empty()){
        cout <<  "Could not open or find the image of sign1" << std::endl ;
        return -1;
    }

    if(sign2.empty()){
        cout <<  "Could not open or find the image of sign2" << std::endl ;
        return -1;
    }

    if(sign3.empty()){
        cout <<  "Could not open or find the image of sign3" << std::endl ;
        return -1;
    }

    if(sign3.empty()){
        cout <<  "Could not open or find the image of sign4" << std::endl ;
        return -1;
    }
    /*
    namedWindow( "afbeelding sign.jpg", WINDOW_AUTOSIZE );     // Create a window for display.
    namedWindow( "afbeelding sign1.jpg", WINDOW_AUTOSIZE );    // Create a window for display.
    namedWindow( "afbeelding sign2.jpg", WINDOW_AUTOSIZE );    // Create a window for display.
    namedWindow( "afbeelding sign3.jpg", WINDOW_AUTOSIZE );    // Create a window for display.

    imshow("afbeelding sign.jpg", sign1);
    imshow("afbeelding sign1.jpg", sign2);
    imshow("afbeelding sign2.jpg", sign3);
    imshow("afbeelding sign3.jpg", sign4);
    */

    ///////////////////////////////////////////////////////////////////////////////////
    ////                                OPDRACHTEN                                  ///
    ///////////////////////////////////////////////////////////////////////////////////
    /*

    Sessie 2 - colour space manipulation

    Omdat autonome wagens heel erg in de opmars zijn willen we dat deze wagens zich ook
    aan de verkeersregels houden zoals de maximaal toegelaten snelheid. Hiervoor gaan
    we een camera in de auto monteren en een computer die verkeersborden zal detecteren
    en analyseren. Jouw opdracht is een stukje software schrijven om de rode
    verkeersborden te filteren van de achtergrond zodat deze later kunnen geanalyseerd
    worden.

    Opdracht 1: Segmenteer de verkeersborden in de BGR kleurenruimte
        Wat zijn de voordelen?
        Wat zijn de nadelen?
    Opdracht 2: Segmenteer de verkeersborden in de HSV kleurenruimte
        Wat zijn de voordelen?
        Wat zijn de nadelen?
    Opdracht 3: Gebruik connected component analyse om enkel het stopbord over te houden
    Opdracht 4: Gebruik een trackbar om de optimale threshold waardes te bepalen


    */

    Mat image = sign3;
    ///_________________________________OPDRACHT 1_____________________________________

    vector<Mat> kanalen;
    split(image, kanalen);

    Mat mask_1 = Mat::zeros(image.rows, image.cols, CV_8UC1);
    Mat B = kanalen[0];
    Mat G = kanalen[1];
    Mat R = kanalen[2];

    mask_1 = (R>150);
    mask_1 = mask_1 * 255;

    imshow("huidskleur masker methode 1", mask_1);

    Mat finaal(image.rows, image.cols, CV_8UC3);

    Mat blauwe_pixels = B & mask_1;
    Mat groene_pixels = G & mask_1;
    Mat rode_pixels = R & mask_1;

    vector<Mat> channels_mix;

    channels_mix.push_back(blauwe_pixels);
    channels_mix.push_back(groene_pixels);
    channels_mix.push_back(rode_pixels);

    merge(channels_mix, finaal);

    imshow("huid", finaal);


    ///_________________________________OPDRACHT 2_____________________________________
    Mat image_HSV;
    Mat threshold_HSV;
    cvtColor(image, image_HSV, COLOR_BGR2HSV);
    inRange(image_HSV, Scalar(160, 50, 50), Scalar(180, 255, 255), threshold_HSV);
    imshow("hsv thresholded", threshold_HSV);
    waitKey(0);

    ///_________________________________OPDRACHT 3_____________________________________
    Mat canvas = image.clone();
    vector<vector<Point>> contours;
    findContours(image.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    vector<Point> grootste_blob = contours[0];
    for(int i = 0; i < contours.size(); i++){
        if (contourArea(contours[i]) > contourArea(grootste_blob)){
            grootste_blob = contours[i];
        }
    }

    vector<vector<Point>> temp;
    temp.push_back(grootste_blob);
    drawContours(mask_1, temp, -1, 255, -1);

    blauwe_pixels = B & mask_1;
    groene_pixels = G & mask_1;
    rode_pixels = R & mask_1;

    vector<Mat> channels_mix_2;

    channels_mix_2.push_back(blauwe_pixels);
    channels_mix_2.push_back(groene_pixels);
    channels_mix_2.push_back(rode_pixels);

    merge(channels_mix_2, finaal);

    imshow("masker met hullen", finaal);
    waitKey(0);

    ///_________________________________OPDRACHT 4_____________________________________
    namedWindow("HSV_trackbar");
    createTrackbar("Low H", "HSV_trackbar", &low_H, max_value_H, on_low_H_thresh_trackbar);
    createTrackbar("High H", "HSV_trackbar", &high_H, max_value_H, on_high_H_thresh_trackbar);
    createTrackbar("Low S", "HSV_trackbar", &low_S, max_value, on_low_S_thresh_trackbar);
    createTrackbar("High S", "HSV_trackbar", &high_S, max_value, on_high_S_thresh_trackbar);
    createTrackbar("Low V", "HSV_trackbar", &low_V, max_value, on_low_V_thresh_trackbar);
    createTrackbar("High V", "HSV_trackbar", &high_V, max_value, on_high_V_thresh_trackbar);

    imshow("HSV_trackbar", image);
    while (true) {

        cvtColor(image, image_HSV, COLOR_BGR2HSV);
        inRange(image_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), threshold_HSV);
        imshow("HSV_trackbar", threshold_HSV);

        char key = (char) waitKey(30);
        if (key == 'q' || key == 27)
        {
            break;
        }
    }
    return 0;
}

