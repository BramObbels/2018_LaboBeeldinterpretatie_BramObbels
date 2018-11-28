#include <opencv2/opencv.hpp>
#include<opencv2/features2d/features2d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <iostream>

using namespace std;
using namespace cv;

const float inlier_threshold = 2.5f; // Distance threshold to identify inliers
const float nn_match_ratio = 0.9f;   // Nearest neighbor matching r

int main(int argc, const char ** argv){

    //////////////////////////////////////////////////////////////////////////////////////
    ////                                 SETUP                                         ///
    //////////////////////////////////////////////////////////////////////////////////////

    ///______________INLEZEN VAN ARGUMENTEN_______________
    CommandLineParser parser(argc, argv,
        "{help h usage? |    |show this message}"
        "{kinder_o kinder_object   |    |(required) message object_1}"
        "{kinder_b kinder_beeld   |    |(required) message beeld_1}"
        "{fitness_o fitness_object   |    |(required) message object_2}"
        "{fitness_b fitness_beeld   |    |(required) message beeld_2}"

    );

    if(parser.has("help")){
        parser.printMessage();
        cout << "TIP: gebruik voledige padnamen";
        return -1;
    }

    string kinder_o(parser.get<string>("kinder_o"));
    string kinder_b(parser.get<string>("kinder_b"));
    string fitness_o(parser.get<string>("fitness_o"));
    string fitness_b(parser.get<string>("fitness_b"));


    if(kinder_o.empty() || kinder_b.empty() || fitness_o.empty() || fitness_b.empty()){
        parser.printMessage();
        return -1;
    }


    ///_____________AANMAKEN AVARIABELEN____________________

    Mat kinder_voorbeeld;
    Mat kinder_testbeeld;
    Mat fitness_voorbeeld;
    Mat fitness_testbeeld;
    /// ____________INLEZEN IMAGES_____________________________

    cout << kinder_o << std::endl;
    cout << kinder_b << std::endl;
    cout << fitness_o << std::endl;
    cout << fitness_b << std::endl;

    kinder_voorbeeld = imread(kinder_o);
    kinder_testbeeld = imread(kinder_b);
    fitness_voorbeeld = imread(fitness_o);
    fitness_testbeeld = imread(fitness_b);


    ///_____________TESTEN OF IMAGES CORRECT INGELADEN ZIJN________________

    if(kinder_voorbeeld.empty()){
        cout <<  "Could not open or find the image of sign1" << std::endl ;
        return -1;
    }

    if(kinder_testbeeld.empty()){
        cout <<  "Could not open or find the image of sign2" << std::endl ;
        return -1;
    }

    if(fitness_voorbeeld.empty()){
        cout <<  "Could not open or find the image of sign3" << std::endl ;
        return -1;
    }

    if(fitness_testbeeld.empty()){
        cout <<  "Could not open or find the image of sign4" << std::endl ;
        return -1;
    }

    namedWindow( "kinder_voorbeeld", WINDOW_AUTOSIZE );     // Create a window for display.
    namedWindow( "kinder_testbeeld", WINDOW_AUTOSIZE );    // Create a window for display.
    namedWindow( "fitness_voorbeeld", WINDOW_AUTOSIZE );    // Create a window for display.
    namedWindow( "fitness_testbeeld", WINDOW_AUTOSIZE );    // Create a window for display.

    imshow("kinder_voorbeeld", kinder_voorbeeld);
    imshow("kinder_testbeeld", kinder_testbeeld);
    imshow("fitness_voorbeeld", fitness_voorbeeld);
    imshow("fitness_testbeeld", fitness_testbeeld);


    ///////////////////////////////////////////////////////////////////////////////////
    ////                                OPDRACHTEN                                  ///
    ///////////////////////////////////////////////////////////////////////////////////
    /*

    Sessie 3 - keypoint detection and matching

    Met template matching gaan we op zoek naar een exact object. Wanneer er echter een grote vervorming van het gekende object kan optreden, dan lijkt template matching al snel te falen. In dit labo voeren we een analyse uit van een eyetracker opname waarbij we op zoek gaan naar een vooraf bepaald object (kinderbueono and fitness cornflakes).

    Opdracht 1: Bepaal op de inputbeelden en op de te zoeken templates keypoints, gebaseerd op de volgende keypoint types: ORB, BRISK en AKAZE.
    Opdracht 2: Kies 1 van bovenstaande keypoint detectoren, bepaal descriptoren van de gegenereerde keypoints en doe een brute force matching tussen beide sets.
    Opdracht 3: Pas RANSAC toe om enkel de keypoint matches over te houden die op iets slaan. Gebruik de resulterende matches om een homography uit te rekenen en het object in uw doelbeeld te vinden.



    */

    Mat object = kinder_voorbeeld; // keuze van
    Mat beeld = kinder_testbeeld; // keuze van


    ///_________________________________OPDRACHT 1_____________________________________
    //vectoren van keypoints, voor elk detectietype 1
    vector<KeyPoint> object_keypoints_AKAZE, beeld_keypoints_AKAZE;
    vector<KeyPoint> object_keypoints_ORB, beeld_keypoints_ORB;
    vector<KeyPoint> object_keypoints_BRISK, beeld_keypoints_BRISK;

    //descriptoren van de keypoints, voor elk type 1
    Mat object_desc_AKAZE, beeld_desc_AKAZE;
    Mat object_desc_ORB, beeld_desc_ORB;
    Mat object_desc_BRISK, beeld_desc_BRISK;

    // objecten die in staat zijn om keypoint en descriptoren te vinden/genereren, van elk type 1
    Ptr<AKAZE> akaze = AKAZE::create();
    Ptr<ORB> orb = ORB::create();
    Ptr<BRISK> brisk = BRISK::create();

    // Vinden van keypoints en descriptoren voor beeld en object;
    akaze->detectAndCompute(object, noArray(), object_keypoints_AKAZE, object_desc_AKAZE);
    akaze->detectAndCompute(beeld, noArray(), beeld_keypoints_AKAZE, beeld_desc_AKAZE);
    orb->detectAndCompute(object, noArray(), object_keypoints_ORB, object_desc_ORB);
    orb->detectAndCompute(beeld, noArray(), beeld_keypoints_ORB, beeld_desc_ORB);
    brisk->detectAndCompute(object, noArray(), object_keypoints_BRISK, object_desc_BRISK);
    brisk->detectAndCompute(beeld, noArray(), beeld_keypoints_BRISK, beeld_desc_BRISK);

    // canvasen om gevonden keypoints op te tekenen
    Mat object_canvas_AKAZE, object_canvas_ORB, object_canvas_BRISK = object.clone();
    Mat beeld_canvas_AKAZE, beeld_canvas_ORB, beeld_canvas_BRISK = beeld.clone();

    // tekennen van de gevonden keypoints
    drawKeypoints(beeld, beeld_keypoints_AKAZE, beeld_canvas_AKAZE, Scalar(255,0,0));
    drawKeypoints(object, object_keypoints_AKAZE, object_canvas_AKAZE, Scalar(255,0,0));
    drawKeypoints(beeld, beeld_keypoints_ORB, beeld_canvas_ORB, Scalar(255,0,0));
    drawKeypoints(object, object_keypoints_ORB, object_canvas_ORB, Scalar(255,0,0));
    drawKeypoints(beeld, beeld_keypoints_BRISK, beeld_canvas_BRISK, Scalar(255,0,0));
    drawKeypoints(object, object_keypoints_BRISK, object_canvas_BRISK, Scalar(255,0,0));

    //resultaat tonen
    imshow("Keypoints in testbeeld (AKAZE)",beeld_canvas_AKAZE);
    imshow("Keypoints in object (AKAZE)",object_canvas_AKAZE);
    waitKey(0);
    imshow("Keypoints in testbeeld (ORB)",beeld_canvas_ORB);
    imshow("Keypoints in object (ORB)",object_canvas_ORB);
    waitKey(0);
    imshow("Keypoints in testbeeld (BRISK)",beeld_canvas_BRISK);
    imshow("Keypoints in object (BRISK)",object_canvas_BRISK);
    waitKey(0);






    ///_________________________________OPDRACHT 2_____________________________________
    // brute force matching
    //vector die de matches bewaart en met elkaar koppeld
    vector<DMatch> matches;

    //object dat in staat is om correcte matches te vinden
    BFMatcher matcher(NORM_L2); //euclidische afstand wordt gebruikt (werkt enkel voor ORB descriptoren)
    matcher.match(object_desc_ORB, beeld_desc_ORB, matches); // matches zoeken en bewaren in matches

    Mat matches_canvas; //canvas om de gevondne matches op te tekenen
    drawMatches(object, object_keypoints_ORB, beeld, beeld_keypoints_ORB, matches, matches_canvas); // geeft een mooie weergave terug van de gevondne matches tussen het object en het beeld

    sort(matches.begin(), matches.end()); //sorteer de gevonden matches. de beste matches worden de eerste
    const int beste_matches_procent = matches.size() * 0.2; // berenken wat de 20 procent beste matches zijn
    matches.erase(matches.begin()+beste_matches_procent, matches.end()); // verwijder al de rest


    ///_________________________________OPDRACHT 3_____________________________________
    //de locaties vand e beste matches bepalen
    vector<Point2f> object_match_posities, beeld_match_posities; //punten van de beste matches
    for(int i = 0; i < matches.size(); i++){
        object_match_posities.push_back(object_keypoints_ORB[matches[i].queryIdx].pt);
        beeld_match_posities.push_back(beeld_keypoints_ORB[matches[i].trainIdx].pt);
    }
    //ransac toepassen om de homography te bepalen tussen beeld en object
    Mat ransac = findHomography(object_match_posities, beeld_match_posities, RANSAC);
    //hoeken van object afbeelding in een vector steken
    vector<Point2f> object_hoeken(4);
    vector<Point2f> beeld_hoeken(4);
    object_hoeken[0] = cvPoint(0,0);
    object_hoeken[1] = cvPoint(object.cols,0);
    object_hoeken[2] = cvPoint(object.cols,object.rows);
    object_hoeken[3] = cvPoint(0,object.rows);
    // homografische transformatie toepassen op de hoekpunten van het objectbeeld op het beeld
    perspectiveTransform(object_hoeken, beeld_hoeken, ransac);
    //tekenen van gevonden kader na transformatie
    line(matches_canvas, beeld_hoeken[0] + Point2f(object.cols, 0), beeld_hoeken[1] +Point2f(object.cols,0), Scalar(255,0,0), 8);
    line(matches_canvas, beeld_hoeken[1] + Point2f(object.cols, 0), beeld_hoeken[2] +Point2f(object.cols,0), Scalar(255,0,0), 8);
    line(matches_canvas, beeld_hoeken[2] + Point2f(object.cols, 0), beeld_hoeken[3] +Point2f(object.cols,0), Scalar(255,0,0), 8);
    line(matches_canvas, beeld_hoeken[3] + Point2f(object.cols, 0), beeld_hoeken[0] +Point2f(object.cols,0), Scalar(255,0,0), 8);
    //toon resulaat
    imshow("matches", matches_canvas);


    waitKey(0);

    return 0;
    //voor akaze zie deze link obbels
    //https://docs.opencv.org/3.0-beta/doc/tutorials/features2d/akaze_matching/akaze_matching.html
}
