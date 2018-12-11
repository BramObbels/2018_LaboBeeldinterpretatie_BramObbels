#include <opencv2/opencv.hpp>
//#include <opencv2/features2d/features2d.hpp>
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

    VideoCapture gezicht_cap(face); //videocapture
    VideoCapture persoon_cap(person); //videocapture

    Mat gezicht;
    Mat persoon;

    gezicht_cap >> gezicht; //eerste frame
    persoon_cap >> persoon; //eerste frame

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

   Sessie 6 - Object Detection

    In dit labo gaan we 2 verschillende soorten object detectors gebruiken om eerst gezichten te vinden. Daarna gebruiken we een andere detector om personen te vinden.

    Opdracht 1: Maak een interface waarin je de fases.mp4 video kan afspelen, daarna gebruik je het Viola en Jones detectie algoritme om met het HAAR face model gezichten in elk frame te vinden. Duid deze aan met een cirkel en een detectie score.

    Opdracht 2: Detecteer nu niet enkel met het HAAR face model, maar ook het LBP model. Wat merk je van verschillen?

    Opdracht 3: Maak nu een ander project waarin we dezelfde interface hebben, maar met de people.mp4 video. Hierin moet je personen vinden gebruikmakende van de HOG+SVM pedestrian detector.

    EXTRA: Kan je ook per persoon zijn/haar traject in de video bijhouden?

    */

    CascadeClassifier detectorHAAR(HAAR_model);
    CascadeClassifier detectorLBP(LBP_model);


    ///_________________________________OPDRACHT 1 en 2_____________________________________
    ///antwoord op de vraag: Haarfeatures zijn preciezer dan LBP maar LBP is sneller;
    while(1){
        // Capture frame-by-frame
        gezicht_cap >> gezicht;

        // If the frame is empty, break immediately
        if (gezicht.empty())
          break;




        Mat canvas = gezicht.clone(); //canvas om op te tekenen

        Mat tempHaar = gezicht.clone(); //clone voor detectie later
        Mat tempLBP = gezicht.clone(); //clone voor detectie later

        vector<Rect> objectHAAR, objectLBP; // vector waar boundingboxes uit detectie worden bewaard
        vector<int> scoreHAAR, scoreLBP; // vector waar boundingboxes uit detectie worden bewaard

        detectorHAAR.detectMultiScale(tempHaar, objectHAAR, scoreHAAR, 1.05, 3); //detectie uitvoeren
        detectorLBP.detectMultiScale(tempLBP, objectLBP, scoreLBP, 1.05, 3); //detectie uitvoeren

        for(int i = 0; i<objectHAAR.size(); i++){ // voor elk gevonden object tekenen we de boundingbox/cirkel en de score
            string score = to_string(scoreHAAR[i]);
            putText(canvas,score,Point(objectHAAR[i].x, objectHAAR[i].y),FONT_HERSHEY_PLAIN, 3,  Scalar(255,0,0));
            circle(canvas, Point(objectHAAR[i].x+objectHAAR[i].height/2, objectHAAR[i].y+objectHAAR[i].height/2), objectHAAR[i].height/2, Scalar(255,0,0), 2);
        }
        for(int i = 0; i<objectLBP.size(); i++){ // voor elk gevonden object tekenen we de boundingbox/cirkel en de score
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

    ///_________________________________OPDRACHT 3_____________________________________
    HOGDescriptor hogdesc; //hogdetector
    hogdesc.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector()); //standaard hogdescriptor voor personen inladen

    while(1){
        // Capture frame-by-frame
        persoon_cap >> persoon;

        // If the frame is empty, break immediately
        if (persoon.empty())
            break;
        //we herschalen het frame omdat in het begin de persoon te klein is voor detectie;
        resize(persoon, persoon, Size(1.5*persoon.cols, 1.5*persoon.rows));

        Mat persooncl = persoon.clone(); // clone voor detectiefuncitie
        Mat persooncavas = persoon.clone(); // canvas om op te tekenen

        vector<Rect> personen; //gevonden boundingboxen van person
        vector<double> gewichten; //bijbehorende scores

        hogdesc.detectMultiScale(persooncl, personen, gewichten); //detectie uitvoeren in verschillende grotes
        for(int i=0; i<personen.size(); i++){ //tekenen van boundingboxen en score
            if(gewichten[i] >= 1.0){
                string score = to_string(gewichten[i]);
                putText(persooncavas,score,Point(personen[i].x, personen[i].y),FONT_HERSHEY_PLAIN, 3,  Scalar(0,255,0));
                rectangle(persooncavas, Rect(Point(personen[i].x, personen[i].y),Point(personen[i].x + personen[i].width, personen[i].y + personen[i].height)), Scalar(0,255, 0));
            }
        }
        imshow("walking_capture", persooncavas ); //toon canvas
        // Press  ESC on keyboard to exit
        char c=(char)waitKey(25);
        if(c==27)
            break;
    }
    return 0;
}
