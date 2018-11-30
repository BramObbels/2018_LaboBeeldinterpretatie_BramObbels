#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void CallBackFunc(int event, int x, int y, int flags, void* data)
{
    vector<Point>* selectie = (vector<Point>*)data;
    if  ( event == EVENT_LBUTTONDOWN )
    {
        selectie->push_back(Point(x,y));
        cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
        cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if  ( event == EVENT_MBUTTONDOWN )
    {
        cout << "alle punten:" << endl;
        for(int i = 0; i<selectie->size(); i++){
            cout << "Punt"<< i << ": "<< selectie->at(i).x <<","<< selectie->at(i).y << endl;
        }
    }
}


int main(int argc, const char ** argv){

    //////////////////////////////////////////////////////////////////////////////////////
    ////                                 SETUP                                         ///
    //////////////////////////////////////////////////////////////////////////////////////

    ///______________INLEZEN VAN ARGUMENTEN_______________
    CommandLineParser parser(argc, argv,
        "{help h usage? |    |show this message}"
        "{aardbei1 aarbei_voorbeeld_1   |    |(required) message aarbei_voorbeeld_1}"
        "{aardbei2 aarbei_voorbeeld_2   |    |(required) message aarbei_voorbeeld_2}"
    );

    if(parser.has("help")){
        parser.printMessage();
        cout << "TIP: gebruik voledige padnamen";
        return -1;
    }

    string aardbei_1(parser.get<string>("aardbei1"));
    string aardbei_2(parser.get<string>("aardbei2"));



    if(aardbei_1.empty() || aardbei_2.empty()){
        parser.printMessage();
        return -1;
    }


    ///_____________AANMAKEN AVARIABELEN____________________

    Mat aarbei_voorbeels_1;
    Mat aarbei_voorbeels_2;

    /// ____________INLEZEN IMAGES_____________________________

    cout << aardbei_1 << std::endl;
    cout << aardbei_2 << std::endl;


    aarbei_voorbeels_1 = imread(aardbei_1);
    aarbei_voorbeels_2 = imread(aardbei_2);


    ///_____________TESTEN OF IMAGES CORRECT INGELADEN ZIJN________________

    if(aarbei_voorbeels_1.empty()){
        cout <<  "Could not open or find the image of aarbei_voorbeeld_1" << std::endl ;
        return -1;
    }

    if(aarbei_voorbeels_2.empty()){
        cout <<  "Could not open or find the image of aarbei_voorbeeld_2" << std::endl ;
        return -1;
    }

    namedWindow( "aardbei_voorbeeld_1", WINDOW_AUTOSIZE );     // Create a window for display.
    namedWindow( "aardbei_voorbeeld_2", WINDOW_AUTOSIZE );    // Create a window for display.

    imshow("aardbei_voorbeeld_1", aarbei_voorbeels_1);
    imshow("aardbei_voorbeeld_2", aarbei_voorbeels_2);


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




    ///_________________________________OPDRACHT 1_____________________________________
    //vectoren van keypoints, voor elk detectietype 1



    namedWindow("My Window", 1);
    vector<Point> punten;
    setMouseCallback("My Window", CallBackFunc, &punten);

    imshow("aardbei_voorbeeld_1", aarbei_voorbeels_1);
    //niet vergeten gausianblur toevoegen
    //zerken in hsvrui;te

    waitKey(0);

    return 0;
    //voor akaze zie deze link obbels
    //https://docs.opencv.org/3.0-beta/doc/tutorials/features2d/akaze_matching/akaze_matching.html
}
