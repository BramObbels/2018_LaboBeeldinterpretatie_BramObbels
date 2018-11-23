#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

using namespace std;
using namespace cv;

const float inlier_threshold = 2.5f; // Distance threshold to identify inliers
const float nn_match_ratio = 0.8f;   // Nearest neighbor matching r

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

    //detecteren en bechrijven van keypoints
    vector<KeyPoint> object_keypoints, beeld_keypoints;
    Mat object_desc, beeld_desc;
    Ptr<AKAZE> akaze = AKAZE::create();
    akaze->detectAndCompute(object, noArray(), object_keypoints, object_desc);
    akaze->detectAndCompute(beeld, noArray(), beeld_keypoints, beeld_desc);
    Mat object_canvas = object.clone();
    Mat beeld_canvas = beeld.clone();
    drawKeypoints(beeld, beeld_keypoints, beeld_canvas, Scalar(255,0,0));
    drawKeypoints(object, object_keypoints, object_canvas, Scalar(255,0,0));
    imshow("Keypoints in testbeeld",beeld_canvas);
    imshow("Keypoints in object",object_canvas);
    // brute force matching
    BFMatcher matcher(NORM_HAMMING);
    vector<vector<DMatch>> nn_matches;
    matcher.knnMatch(object_desc, beeld_desc, nn_matches, 2);


    vector<KeyPoint> object_matched, beeld_matched, object_inliers, beeld_inliers;
    vector<DMatch> good_matches;
    for(size_t i = 0; i < nn_matches.size(); i++) {
        DMatch first = nn_matches[i][0];
        float dist1 = nn_matches[i][0].distance;
        float dist2 = nn_matches[i][1].distance;

        if(dist1 < nn_match_ratio * dist2) {
            beeld_matched.push_back(beeld_keypoints[first.queryIdx]);
            object_matched.push_back(object_keypoints[first.trainIdx]);
        }
    }

    //drawKeypoints(object, object_matched, object, Scalar(255,0,0));
    //imshow("good matches in testbeeld",object);
    vector<Point2f> object_keypoint_positie;
    vector<Point2f> beeld_keypoint_positie;
    for(size_t i = 0; i<good_matches.size(); i++){
        object_keypoint_positie.push_back(object_keypoints[good_matches[i].queryIdx].pt);
        beeld_keypoint_positie.push_back(beeld_keypoints[good_matches[i].queryIdx].pt);
    }
    //check het aantal punten, voor Akaze zijn er soms te weinig
    findHomography(object_keypoint_positie, beeld_keypoint_positie, RANSAC);
    /*
    for(unsigned i = 0; i < object_matched.size(); i++) {
        Mat col = Mat::ones(3, 1, CV_64F);
        col.at<double>(0) = object_matched[i].pt.x;
        col.at<double>(1) = object_matched[i].pt.y;

        col = homography * col;
        col /= col.at<double>(2);
        double dist = sqrt( pow(col.at<double>(0) - beeld_matched[i].pt.x, 2) +
                            pow(col.at<double>(1) - beeld_matched[i].pt.y, 2));

        if(dist < inlier_threshold) {
            int new_i = static_cast<int>(inliers1.size());
            inliers1.push_back(object_matched[i]);
            inliers2.push_back(beeld_matched[i]);
            good_matches.push_back(DMatch(new_i, new_i, 0));
        }
    }

    Mat res;
    drawMatches(img1, inliers1, img2, inliers2, good_matches, res);
    imwrite("res.png", res);

    double inlier_ratio = inliers1.size() * 1.0 / matched1.size();
    cout << "A-KAZE Matching Results" << endl;
    cout << "*******************************" << endl;
    cout << "# Keypoints 1:                        \t" << kpts1.size() << endl;
    cout << "# Keypoints 2:                        \t" << kpts2.size() << endl;
    cout << "# Matches:                            \t" << matched1.size() << endl;
    cout << "# Inliers:                            \t" << inliers1.size() << endl;
    cout << "# Inliers Ratio:                      \t" << inlier_ratio << endl;
    cout << endl;
    */

    waitKey(0);

    return 0;
    //https://docs.opencv.org/3.0-beta/doc/tutorials/features2d/akaze_matching/akaze_matching.html
}
