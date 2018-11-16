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
        "{help h usage?         |    |show this message}"
        "{temp template         |    |(required) message template image}"
        "{img image             |    |(required) message testimage}"
        "{img_rot image_rotate  |    |(required) message testimage totation}"
    );

    if(parser.has("help")){
        parser.printMessage();
        cout << "TIP: gebruik voledige padnamen";
        return -1;
    }

    string t(parser.get<string>("temp"));
    string i(parser.get<string>("img"));
    string irot(parser.get<string>("img_rot"));


    if(t.empty() || i.empty() || irot.empty()){
        parser.printMessage();
        return -1;
    }


    ///_____________AANMAKEN AVARIABELEN____________________

    Mat templ;
    Mat image;
    Mat image_rotate;

    /// ____________INLEZEN IMAGES_____________________________

    cout << t << endl;
    cout << i << endl;
    cout << irot << endl;


    templ = imread(t);
    image = imread(i);
    image_rotate = imread(irot);



    ///_____________TESTEN OF IMAGES CORRECT INGELADEN ZIJN________________

    if(templ.empty()){
        cout <<  "Could not open or find the template image" << std::endl ;
        return -1;
    }

    if(image.empty()){
        cout <<  "Could not open or find the test image" << std::endl ;
        return -1;
    }

    if(image_rotate.empty()){
        cout <<  "Could not open or find the rotation test image" << std::endl ;
        return -1;
    }


    //imshow(t, templ);
    //imshow(i, image);
    //imshow(irot, image_rotate);

    Mat template_gray;
    Mat image_gray;
    Mat image_rotate_gray;

    cvtColor(templ, template_gray, COLOR_BGR2GRAY);    //bimodal afbeelding omzettten naar grijswaarde
    cvtColor(image, image_gray, COLOR_BGR2GRAY);    //bimodal afbeelding omzettten naar grijswaarde
    cvtColor(image_rotate, image_rotate_gray, COLOR_BGR2GRAY);    //bimodal afbeelding omzettten naar grijswaarde

    imshow("gray template", template_gray);
    imshow("gray testimage", image_gray);
    imshow("gray testimage rotated", image_rotate_gray);

    Mat result;

    /// Do the Matching and Normalize
    matchTemplate( image_gray, template_gray, result, TM_CCORR);
    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

    result = 1-result;

    double minVal; double maxVal; Point minLoc; Point maxLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    Mat canvas = image.clone();
    Mat canvas_2 = image.clone();
    rectangle( canvas, minLoc, Point( minLoc.x + templ.cols , minLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );

    imshow("matching result", result);
    imshow("vind 1 locatie met minMaxLoc", canvas);

    Mat result_threshold;
    result_threshold.create( result.rows, result.cols, CV_32SC1);
    threshold(result, result_threshold, 0.1, 255, THRESH_BINARY_INV);


    imshow("threshold", result_threshold);
    vector<vector<Point>> contouren;
    findContours(result_threshold.clone(), contouren, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    for(int i = 0; i < contouren.size(); i++){
        Rect region = boundingRect(contouren[i]);
        Mat temp = result(region);
        Point  maxLoc;
        minMaxLoc(temp, NULL, NULL, &minLoc, NULL);
        rectangle(canvas_2, Point(region.x + minLoc.x, region.y + minLoc.y), Point(minLoc.x +region.x + templ.cols, region.y + minLoc.y + templ.rows), Scalar::all(0), 2, 8, 0 );
    }

    imshow("threshold", result_threshold);

    waitKey(0);

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


    return 0;
}

