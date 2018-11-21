#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void meerdere_detectie(Mat beeld, Mat templ);
void de_eerste_de_beste(Mat image, Mat templ);

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



    waitKey(0);

    ///////////////////////////////////////////////////////////////////////////////////
    ////                                OPDRACHTEN                                  ///
    ///////////////////////////////////////////////////////////////////////////////////
    /*
    Sessie 3 - template based matching

    Kleur segmentatie en connected components brengen je maar tot een beperkte oplossing bij heel wat problemen. Daarom gaan we deze week een stap verder. Op een transportband in een industriele setup komen zowel metalen plaatjes als metalen ringen voorbij (zie input foto). De metalen plaatjes moeten gedetecteerd worden en gelabeld als in de output foto, zodanig dat een autonome robot deze van de band kan plukken. Hiervoor zullen we gebruik maken van template matching.

    Opdracht 1: Gebruik template matching om een object te vinden in een inputbeeld
    Opdracht 2: Pas de template matching aan om lokaal naar maxima te zoeken, zodanig dat je alle matches vind
    Extra: Pas de template matching aan geroteerde objecten te vinden (roteren van beeld, rotatedRect, warpPerspective op hoekpunten)
*/
    //__________________________________Opdracht1_____________________________________
    de_eerste_de_beste(image,templ);

    //__________________________________Opdracht2_____________________________________
    meerdere_detectie(image,templ);
    return 0;
}


void de_eerste_de_beste(Mat image, Mat templ){
    Mat template_gray;
    Mat image_gray;

    cvtColor(templ, template_gray, COLOR_BGR2GRAY);    //bimodal afbeelding omzettten naar grijswaarde
    cvtColor(image, image_gray, COLOR_BGR2GRAY);    //bimodal afbeelding omzettten naar grijswaarde

    //imshow("gray template", template_gray);
    //imshow("gray testimage", image_gray);

    Mat result;

    // vind een heatmap dat de beste corelaties met het template weergeeft
    matchTemplate( image_gray, template_gray, result, TM_CCORR);
    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() ); //normalizeer

    //inverteer ()
    result = 1-result;

    //zoek het minimum, max en de coresponderende punten
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    //teken boundingbox op canvas
    Mat canvas = image.clone();
    rectangle( canvas, minLoc, Point( minLoc.x + templ.cols , minLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );

    //imshow("matching result", result);
    imshow("opdracht_2", canvas);
    waitKey(0);
}


void meerdere_detectie(Mat beeld, Mat templ){
    Mat canvas = beeld.clone();
    Mat template_gray;
    Mat image_gray;
    Mat match_result;

    double minimum;
    double maximum;

    double thresholdwaarde = 0.85;

    cvtColor(templ, template_gray, COLOR_BGR2GRAY);    // afbeelding omzettten naar grijswaarde
    cvtColor(beeld, image_gray, COLOR_BGR2GRAY);    // afbeelding omzettten naar grijswaarde

    // vind een heatmap dat de beste corelaties met het template weergeeft
    matchTemplate(image_gray, template_gray, match_result, TM_CCORR_NORMED);
    normalize(match_result, match_result, 0, 1, NORM_MINMAX, -1, Mat()); //normalizeer
    //vind de minimum en en maximum waarde maximum in deze heatmap
    minMaxLoc(match_result, &minimum, &maximum);
    //trhreshold aan de hand van dit maximum
    Mat match_result_threshold = Mat::zeros(Size(beeld.cols, beeld.rows), CV_32FC1);
    inRange(match_result, maximum * thresholdwaarde, maximum, match_result_threshold);
    match_result_threshold.convertTo(match_result_threshold, CV_8UC1);

    vector<vector<Point>> contouren;
    findContours(match_result_threshold, contouren, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    for(int i = 0; i < contouren.size(); i++){
        //vind een vierkant dat rond de contour past
        vector<Point> hulls;
        convexHull(contouren[i], hulls);
        Rect rect = boundingRect(hulls);
        //vind het locale maximum van het template resultaat maar dan binnen deze vierkant
        Point locatie;
        minMaxLoc(match_result(rect), NULL, NULL, NULL, &locatie);
        //teken rond punt een vrierkant even groot als het template beeld
        Point p(locatie.x + rect.x, locatie.y + rect.y);
        rectangle(canvas, p, Point(p.x + templ.cols, p.y + templ.rows), Scalar(0, 255, 0));

    }
    imshow("opdracht_2", canvas);
    waitKey(0);
}
