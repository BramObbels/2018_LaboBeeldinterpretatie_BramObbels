#include <opencv2/opencv.hpp>
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
    namedWindow( "gray", WINDOW_AUTOSIZE );    // Create a window for display.
    namedWindow( "color", WINDOW_AUTOSIZE );    // Create a window for display.

    imshow( "gray", grayimage );                   // Show our image inside it.
    imshow( "color", colorimage );                   // Show our image inside it.


    ///_____________________OPSPLITSEN IN KLEUREN_______________
    vector<Mat> kanalen;
    split(colorimage, kanalen);
    imshow("blauw kanaal", kanalen[0]);
    imshow("groen kanaal", kanalen[1]);
    imshow("rood kanaal", kanalen[2]);
    waitKey(0);                                          // Wait for a keystroke in the window

    ///____________________OMZETTEN NAAR GRAY____________________

    Mat collortograyImage;
    //cvtColor();
    return 0;
}
