#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/ml.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, const char ** argv){

    //////////////////////////////////////////////////////////////////////////////////////
    ////                                 SETUP                                         ///
    //////////////////////////////////////////////////////////////////////////////////////

    ///______________INLEZEN VAN ARGUMENTEN_______________
    CommandLineParser parser(argc, argv,
        "{help h usage?               |    |show this message}"
        "{face face_video             |    |(required) message face_video}"
        "{person person_video         |    |(required) message person_video}"
        "{HAAR_model haar             |    |(required) message haar model}"
        "{LBP_model haar              |    |(required) message lbp model}"
    );

    if(parser.has("help")){
        parser.printMessage();
        cout << "TIP: gebruik voledige padnamen";
        return -1;
    }

    string face(parser.get<string>("face"));
    string person(parser.get<string>("person"));
    string HAAR_model(parser.get<string>("HAAR_model"));
    string LBP_model(parser.get<string>("LBP_model"));

    if(face.empty() || person.empty() || HAAR_model.empty() || LBP_model.empty()){
        parser.printMessage();
        return -1;
    }


    ///_____________AANMAKEN AVARIABELEN____________________




    /// ____________INLEZEN IMAGES_____________________________

    cout << face << std::endl;
    cout << person << std::endl;

    VideoCapture gezicht_cap(face);
    VideoCapture persoon_cap(person);

    Mat gezicht;
    Mat persoon;

    gezicht_cap >> gezicht;
    persoon_cap >> persoon;

    ///_____________TESTEN OF IMAGES CORRECT INGELADEN ZIJN________________
    if(!gezicht_cap.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return -1;
    }
    if(!persoon_cap.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    if(gezicht.empty()){
        cout <<  "Could not open or find gezicht video" << std::endl ;
        return -1;
    }

    if(persoon.empty()){
        cout <<  "Could not open or find persoon video" << std::endl ;
        return -1;
    }

    namedWindow( "face_capture", WINDOW_AUTOSIZE );     // Create a window for display.
    namedWindow( "walking_capture", WINDOW_AUTOSIZE );    // Create a window for display.

    imshow("face_capture", gezicht);
    imshow("walking_capture", persoon);


    ///////////////////////////////////////////////////////////////////////////////////
    ////                                OPDRACHTEN                                  ///
    ///////////////////////////////////////////////////////////////////////////////////
    /*

    Sessie 5 - Machine Learning

    Een aardbei plukker wil graag mee gaan met de tijd en wil zijn manuele arbeid reduceren. Hiervoor heeft hij aan ons gevraag om een automatische aarbei-pluk machine te bouwen, die in staat is rijpe aardbeien te onderscheiden van onrijpe aardbeien. In dit labo gaan jullie proberen om met behulp van machine learning rijpe aarbeien te detecteren.

    Opdracht 1: Maak een interface waarop je op basis van mouse callbacks pixels kan aanklikken. Bij links klikken, sla je een extra punt op. Rechts klikken verwijder je het laatst toegevoegde punt. En waarbij de middelste knop een lijst toont met alle opgeslagen punten.

    Opdracht 2: Op basis van de geselecteerd punten bepaalje een descriptor, in dit geval de HSV representatie van de pixel.

    Opdracht 3: Train met deze data een K-Nearest-Neighbor classifier, een Normal Bayes Classifier en een Support Vector Machine

    Opdracht 4: Classificeer de pixels van de afbeelding met deze technieken en visualiseer met een masker de resterende pixels.

    EXTRA: Hoe kan je de segmentatie verbeteren? Denk na over het feit dat een rijpe aarbei meer rood en minder groen componenten geven.

    */

    CascadeClassifier detectorHAAR(HAAR_model);
    CascadeClassifier detectorLBP(LBP_model);


    ///_________________________________OPDRACHT 1_____________________________________
    while(1){
        // Capture frame-by-frame
        gezicht_cap >> gezicht;

        // If the frame is empty, break immediately
        if (gezicht.empty())
          break;




        Mat canvas = gezicht.clone();

        Mat tempHaar = gezicht.clone();
        Mat tempLBP = gezicht.clone();

        vector<Rect> objectHAAR, objectLBP;
        vector<int> scoreHAAR, scoreLBP;

        detectorHAAR.detectMultiScale(tempHaar, objectHAAR, scoreHAAR, 1.05, 3);
        detectorLBP.detectMultiScale(tempLBP, objectLBP, scoreLBP, 1.05, 3);

        for(int i = 0; i<objectHAAR.size(); i++){
            string score = to_string(scoreHAAR[i]);
            putText(canvas,score,Point(objectHAAR[i].x, objectHAAR[i].y),FONT_HERSHEY_PLAIN, 3,  Scalar(255,0,0));
            circle(canvas, Point(objectHAAR[i].x+objectHAAR[i].height/2, objectHAAR[i].y+objectHAAR[i].height/2), objectHAAR[i].height/2, Scalar(255,0,0), 2);
        }
        for(int i = 0; i<objectLBP.size(); i++){
            string score = to_string(scoreLBP[i]);
            putText(canvas,score,Point(objectLBP[i].x+objectLBP[i].width, objectLBP[i].y+objectLBP[i].height),FONT_HERSHEY_PLAIN, 3,  Scalar(0,255,0));
            rectangle(canvas,objectLBP[i],Scalar(0,255,0),2);
        }

        // Display the resulting frame
        imshow( "Frame", canvas );
        // Press  ESC on keyboard to exit
        char c=(char)waitKey(25);
        if(c==27)
            break;
    }
    return 0;
}
