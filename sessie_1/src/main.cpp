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
        "{imgbimodal ImageBimodal   |    |(required) message grayimage}"
        "{imgcolor ImageColor   |    |(required) message colorimage}"
        "{imgcoloradapted ImageColorAdaptive   |    |(required) message colorimage}"
    );

    if(parser.has("help")){
        parser.printMessage();
        cout << "TIP: gebruik voledige padnamen";
        return -1;
    }

    string imgbimodal(parser.get<string>("imgbimodal"));
    string imgcolor(parser.get<string>("imgcolor"));
    string imgcoloradapted(parser.get<string>("imgcoloradapted"));

    if(imgbimodal.empty() || imgcolor.empty() || imgcoloradapted.empty()){
        parser.printMessage();
        return -1;
    }


    ///_____________AANMAKEN AVARIABELEN____________________

    Mat bimodal;
    Mat color;
    Mat coloradapted;

    /// ____________INLEZEN IMAGES_____________________________

    cout << imgbimodal << std::endl;
    cout << imgcolor << std::endl;
    cout << imgcoloradapted << std::endl;

    bimodal = imread(imgbimodal);
    color = imread(imgcoloradapted);
    coloradapted = imread(imgcoloradapted);

    ///_____________TESTEN OF IMAGES CORRECT INGELADEN ZIJN________________

    if(bimodal.empty()){
        cout <<  "Could not open or find the bimodal image" << std::endl ;
        return -1;
    }

    if(color.empty()){
        cout <<  "Could not open or find the color image" << std::endl ;
        return -1;
    }

    if(coloradapted.empty()){
        cout <<  "Could not open or find the coloradapted image" << std::endl ;
        return -1;
    }


    ///////////////////////////////////////////////////////////////////////////////////
    ////                                OPDRACHTEN                                  ///
    ///////////////////////////////////////////////////////////////////////////////////
    ///____________________AFBEELDINGEN TONEN___________________
    namedWindow( "bimodalimage", WINDOW_AUTOSIZE );    // Create a window for display.
    namedWindow( "colorimage", WINDOW_AUTOSIZE );    // Create a window for display.
    namedWindow( "coloradaptedimage", WINDOW_AUTOSIZE );    // Create a window for display.

    imshow( "bimodalimage", bimodal );                   // Show our image inside it.
    imshow( "colorimage", color );                   // Show our image inside it.
    imshow( "coloradaptedimage", coloradapted );                   // Show our image inside it.

    ///____________________OPDRACHT 1.1
    //opLees afbeelding imageColor.png en segmenteer de skin pixels
    //Gebruik hiervoor een filter uit de literatuur in het RGB domein: (RED>95) && (GREEN>40) && (BLUE>20) && ((max(RED,max(GREEN,BLUE)) - min(RED,min(GREEN,BLUE)))>15) && (abs(RED-GREEN)>15) && (RED>GREEN) && (RED>BLUE);
    //Visualiseer naast het masker (binaire pixels die tot huid behoren) dat je bekomt ook de resulterende pixelwaarden.

    ///____________________stap1: OPSPLITSEN IN KLEUREN_______________
    vector<Mat> kanalen;
    split(color, kanalen);
    ///____________________stap2: ELKE PIXEL TESTEN OP HUIDSKLEUR (mbv de 3 kanalen)
    Mat mask_1 = Mat::zeros(color.rows, color.cols, CV_8UC1);
    Mat mask_2 = mask_1.clone();
    Mat B = kanalen[0];
    Mat G = kanalen[1];
    Mat R = kanalen[2];

    for(int i=0; i<color.rows; i++){
        for(int j=0; j<color.cols; j++){
            if((R.at<uchar>(i,j)>95) && (G.at<uchar>(i,j)>40) &&
            (B.at<uchar>(i,j)>20) && ((max(R.at<uchar>(i,j),max(G.at<uchar>(i,j),B.at<uchar>(i,j))) - min(R.at<uchar>(i,j),min(G.at<uchar>(i,j),B.at<uchar>(i,j))))>15) &&
            (abs(R.at<uchar>(i,j)-G.at<uchar>(i,j))>15) && (R.at<uchar>(i,j)>G.at<uchar>(i,j)) && (R.at<uchar>(i,j)>B.at<uchar>(i,j))){
                mask_1.at<uchar>(i,j) = 255;
            }
        }
    }

    mask_2 = (R>95) & (G>40) & (B>20) & ((max(R,max(G,B)) - min(R,min(G,B)))>15) & (abs(R-G)>15) & (R>G) & (R>B);

    mask_2 = mask_2 * 255;
    imshow("huidskleur masker methode 1", mask_1);
    imshow("huidskleur masker methode 2", mask_2);

    Mat finaal(color.rows, color.cols, CV_8UC3);
    Mat blauwe_pixels = B & mask_2;
    Mat groene_pixels = G & mask_2;
    Mat rode_pixels = R & mask_2;
    vector<Mat> channels_mix;
    channels_mix.push_back(blauwe_pixels);
    channels_mix.push_back(groene_pixels);
    channels_mix.push_back(rode_pixels);
    merge(channels_mix, finaal);
    imshow("huid", finaal);


    Mat maskA;


    Mat result_CLAHE;
    Ptr<CLAHE> clahe_pointer = createCLAHE();
    clahe_pointer->setTilesGridSize(Size(15,15));
    clahe_pointer->setClipLimit(1);
    clahe_pointer->apply(imgbimodal.clone(), result_CLAHE);
    imshow(CLAHE ticket grayscale, result_CLAHE);

    Mat mask;
    threshold(result_CLAHE, mask, 0, 255, THRESH_OTSU | THRESH_BINARY);
    imshow("ticket segment", mask);

    waitKey(0);
    return 0;
}
