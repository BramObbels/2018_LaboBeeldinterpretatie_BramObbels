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
using namespace cv::ml;

void CallBackFunc(int event, int x, int y, int flags, void* data)
{
    vector<Point>* selectie = (vector<Point>*)data;
    if  ( event == EVENT_LBUTTONDOWN )
    {
        selectie->push_back(Point(x,y));
        cout << "nieuw punt toegevoegd" << endl;

    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
        if(selectie->size()>0){
            selectie->pop_back();
            cout << "laatste punt is verwijdert" << endl;
        }
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



    Mat img = aarbei_voorbeels_1;
    ///_________________________________OPDRACHT 1_____________________________________


    namedWindow("selectie", WINDOW_AUTOSIZE);
    vector<Point> goede_punten;
    vector<Point> slechte_punten;
    setMouseCallback("selectie", CallBackFunc, &goede_punten);
    imshow("selectie", img);
    cout<< "maak een selectie van goede aarbei punten" << endl;
    cout<< "druk vervolgens op een toets naar keuze" << endl;
    waitKey(0);
    setMouseCallback("selectie", CallBackFunc, &slechte_punten);
    cout<< "maak een selectie van punten die geen rijpe aarbij zijn" << endl;
    cout<< "druk vervolgens op een toets naar keuze" << endl;

    imshow("selectie", img);
    waitKey(0);
    ///_________________________________OPDRACHT 2_____________________________________

    Mat img_hsv;
    cvtColor(img, img_hsv, CV_BGR2HSV);

    Mat trainingDataForeg(goede_punten.size(),3,CV_32FC1);
    Mat labels_fg = Mat::ones(goede_punten.size(),1,CV_32SC1);
    for (int i = 0; i < goede_punten.size(); i++)
    {
        Vec3b descriptor = img_hsv.at<Vec3b>(goede_punten[i].y,goede_punten[i].x);
        trainingDataForeg.at<float>(i,0) = descriptor[0];
        trainingDataForeg.at<float>(i,1) = descriptor[1];
        trainingDataForeg.at<float>(i,2) = descriptor[2];
    }

    Mat trainingsDataBackg(slechte_punten.size(), 3, CV_32FC1);
    Mat labels_bg = Mat::zeros(slechte_punten.size(), 1, CV_32SC1);
    for( int i = 0; i < slechte_punten.size(); i++ ){
        Vec3b descriptor = img_hsv.at<Vec3b>(slechte_punten[i].y, slechte_punten[i].x);
        trainingsDataBackg.at<float>(i, 0) = descriptor[0];
        trainingsDataBackg.at<float>(i, 1) = descriptor[1];
        trainingsDataBackg.at<float>(i, 2) = descriptor[2];
    }

    ///Samenvoegen
    Mat trainingsData, labels;
    vconcat(trainingDataForeg, trainingsDataBackg, trainingsData);
    vconcat(labels_fg, labels_bg, labels);
    cerr << trainingsData << endl;
    cerr << labels << endl;
    ///_________________________________OPDRACHT 3_____________________________________

    ///Classifier trainen
    cerr << "Training a normal Bayes classifier" << endl;
    Ptr<NormalBayesClassifier> normalBayes = NormalBayesClassifier::create();
    normalBayes->train(trainingsData, ROW_SAMPLE, labels);

    cerr << "Training a kNearest classifier" << endl;
    Ptr<KNearest> kNN = KNearest::create();
    Ptr<TrainData> trainingDatakNN = TrainData::create(trainingsData, ROW_SAMPLE, labels);
    kNN->setIsClassifier(true);
    kNN->setAlgorithmType(KNearest::BRUTE_FORCE);
    kNN->setDefaultK(3);
    kNN->train(trainingDatakNN);

    cerr << "Training a SVM classifier" << endl;
    Ptr<SVM> svm = SVM::create();
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
    svm->train(trainingsData, ROW_SAMPLE, labels);
    ///_________________________________OPDRACHT 4_____________________________________

    ///Mask maken
    Mat mask_svm = Mat::zeros(img.rows, img.cols, CV_8UC1), mask_kNN = Mat::zeros(img.rows, img.cols, CV_8UC1), mask_bayes = Mat::zeros(img.rows, img.cols, CV_8UC1);
    Mat labels_svm, labels_kNN, labels_bayes;
    for( int r = 0; r < img.rows; r++ ){
        for( int c = 0; c < img.cols; c++ ){
            Vec3b pixelvalue = img_hsv.at<Vec3b>(r,c);
            Mat data_test(1,3,CV_32FC1);
            data_test.at<float>(0,0) = pixelvalue[0];
            data_test.at<float>(0,1) = pixelvalue[1];
            data_test.at<float>(0,2) = pixelvalue[2];

            ///Running classifiers
            svm->predict(data_test, labels_svm);
            kNN->findNearest(data_test, kNN->getDefaultK(), labels_kNN);
            normalBayes->predict(data_test, labels_bayes);

            ///Making masks
            mask_svm.at<uchar>(r,c) = labels_svm.at<float>(0,0);
            mask_kNN.at<uchar>(r,c) = labels_kNN.at<float>(0,0);
            mask_bayes.at<uchar>(r,c) = labels_bayes.at<float>(0,0);


        }
    }

    mask_svm *= 255;
    mask_kNN *= 255;
    mask_bayes *= 255;

    //erode(mask_svm, mask_svm, Mat(), Point(-1,-1), 2);
    //dilate(mask_svm, mask_svm, Mat(), Point(-1,-1), 2);

    imshow("Mask SVM", mask_svm);
    imshow("Mask kNN", mask_kNN);
    imshow("Mask Bayes", mask_bayes);

    waitKey(0);

    //niet vergeten gausianblur toevoegen
    //zerken in hsvrui;te



    return 0;
    //voor akaze zie deze link obbels
    //https://docs.opencv.org/3.0-beta/doc/tutorials/features2d/akaze_matching/akaze_matching.html
}
