#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, const char ** argv){

    //////////////////////////////////////////////////////////////////////////////////////
    ////                                 SETUP                                         ///
    //////////////////////////////////////////////////////////////////////////////////////

    ///______________INLEZEN VAN ARGUMENTEN_______________
    CommandLineParser parser(argc, argv,
        "{help h usage? |    |show this message}"
        "{img1 grayimage   |    |(required) message grayimage}"
        "{img2 colorimage   |    |(required) message colorimage}"
    );

    if(parser.has("help")){
        parser.printMessage();
        return -1;
    }

    string img1(parser.get<string>("img1"));
    string img2(parser.get<string>("img2"));

    if(img1.empty() || img1.empty()){
        parser.printMessage();
        return -1;
    }


    ///_____________AANMAKEN AVARIABELEN____________________

    Mat grayimage;
    Mat colorimage;

    /// ____________INLEZEN IMAGES_____________________________

    cout << img1 << std::endl;
    cout << img2 << std::endl;
    grayimage = imread(img1);
    colorimage = imread(img2);

    ///_____________TESTEN OF IMAGES CORRECT INGELADEN ZIJN________________

    if(grayimage.empty()){
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    if(colorimage.empty()){
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }


    ///////////////////////////////////////////////////////////////////////////////////
    ////                                OPDRACHTEN                                  ///
    ///////////////////////////////////////////////////////////////////////////////////
    ///____________________AFBEELDINGEN TONEN___________________
    namedWindow( "gray image", WINDOW_AUTOSIZE );    // Create a window for display.
    namedWindow( "color image", WINDOW_AUTOSIZE );    // Create a window for display.

    imshow( "gray image", grayimage );                   // Show our image inside it.
    imshow( "color image", colorimage );                   // Show our image inside it.


    ///____________________OPSPLITSEN IN KLEUREN_______________
    vector<Mat> kanalen;
    split(colorimage, kanalen);
    imshow("blauw kanaal", kanalen[0]);
    imshow("groen kanaal", kanalen[1]);
    imshow("rood kanaal", kanalen[2]);

    ///____________________OMZETTEN NAAR GRAY____________________
    Mat collortograyImage;
    cvtColor(colorimage,collortograyImage,COLOR_BGR2GRAY);
    imshow("collor to gray image", collortograyImage );

    ///____________________PIXELWAARDE UITLEZEN__________________
    for(int i=0; i<collortograyImage.rows; i++){
        for(int j=0; j<collortograyImage.cols; j++){
            cout << (int)collortograyImage.at<uchar>(i,j)<< endl;
        }
    }
    ///____________________TEKENING__________________
    Mat tekening = Mat::zeros( 500, 500, CV_8UC3 );
    line( tekening, Point( 15, 20 ), Point( 70, 50), Scalar( 110, 220, 0 ),  2, 8 );
    circle( tekening, Point( 200, 200 ), 32.0, Scalar( 0, 0, 255 ), 1, 8 );
    ellipse( tekening, Point( 200, 200 ), Size( 100.0, 160.0 ), 45, 0, 360, Scalar( 255, 0, 0 ), 1, 8 );
    rectangle( tekening, Point( 15, 20 ), Point( 70, 50), Scalar( 0, 55, 255 ), +1, 4 );
    imshow("tekening", tekening);
    waitKey(0);
    return 0;
}
