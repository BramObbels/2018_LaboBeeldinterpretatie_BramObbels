#include <opencv2/opencv.hpp>
//#include <opencv2/features2d/features2d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/ml.hpp>
#include <iostream>
#include "pacman.h"
#include "TitleTabs.h"

using namespace std;
using namespace cv;

void init(string);
Rect getRect(Mat);
int countContours(Mat);
vector<Rect> getDigitsRects(Mat);
vector<Point> selectPoins(Mat);
Mat getExampleFromStream(VideoCapture);
void getDigits(VideoCapture);
void showInit(Mat);



TitleTabs TAB;


void fillPointVector(int event, int x, int y, int flags, void* data)
{
    vector<Point>* selectie = (vector<Point>*)data;
    if  ( event == EVENT_LBUTTONDOWN )
    {
        selectie->push_back(Point(x,y));
        cout << TAB.getShort() << "nieuw punt toegevoegd" << endl;

    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
        if(selectie->size()>0){
            selectie->pop_back();
            cout << TAB.getShort() << "laatste punt is verwijdert" << endl;
        }
    }
    else if  ( event == EVENT_MBUTTONDOWN )
    {
        cout << TAB.getShort() << "alle punten:" << endl;
        for(int i = 0; i<selectie->size(); i++){
            cout << TAB.getShort()<< "Punt"<< i << ": "<< selectie->at(i).x <<","<< selectie->at(i).y << endl;
        }
    }
}

void SelectRectPoints(int event, int x, int y, int flags, void* data)
{
    selectRectParams* selectie = (selectRectParams*)data;
    if  ( event == EVENT_LBUTTONDOWN )
    {
        if(selectie->selectie_punten.size()>=2){
            cout << TAB.getShort()  << "U hebt al reeds twee hoekpuntenpunten gekozen: druk op de rechter muisknop om het laatste toegevoegde punt te annuleren" << endl;
        }else{
            selectie->selectie_punten.push_back(Point(x,y));
            cout << TAB.getShort() << "Nieuw hoekpuntpunt toegevoegd" << endl;
        }
    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
        if(selectie->selectie_punten.size()>0){
            selectie->selectie_punten.pop_back();
            cout << TAB.getShort() << "   Er zijn geen geselecteerde punten meer" << endl;
        }
    }
    else if (event == EVENT_MOUSEMOVE){
        selectie->current_mouse_location = Point(x,y);
    }
}

int main(int argc, const char ** argv){

    //////////////////////////////////////////////////////////////////////////////////////
    ////                                 SETUP                                         ///
    //////////////////////////////////////////////////////////////////////////////////////
    TAB.addTitle("PACMAN");

    ///______________INLEZEN VAN ARGUMENTEN_______________
    CommandLineParser parser(argc, argv,
        "{help h usage?               |    |show this message}"
        "{video pacman_video          |    |(required) message pacman_video}"
    );

    if(parser.has("help")){
        parser.printMessage();
        cout << TAB.getShort() << "TIP: gebruik voledige padnamen";
        return -1;
    }

    string video(parser.get<string>("video"));


    if(video.empty()){
        parser.printMessage();
        return -1;
    }

    /// ____________INLEZEN IMAGES_____________________________

    cout << TAB.getShort() << "given sourcefile for pacman video:" << video << std::endl;

    VideoCapture frame_capture(video); //videocapture

    Mat frame;

    frame_capture >> frame; //eerste frame


    ///_____________TESTEN OF IMAGES CORRECT INGELADEN ZIJN________________
    if(!frame_capture.isOpened()){
        cout << TAB.getShort() << "Error opening video stream" << endl;
        return -1;
    }

    if(frame.empty()){
        cout << TAB.getShort() <<  "Could not get frame from stream" << std::endl ;
        return -1;
    }

    namedWindow( "frame", WINDOW_AUTOSIZE );     // Create a window for display.
    imshow("frame", frame);

    TAB.addTitle("INITIALISATIE");
    cout << TAB.getShort() << "Opnieuw Initialiseren van Gebieden en Nodige Kleurwaarden?: YES = y,  NO = n" << endl;
    showInit(frame);
    char c=(char)waitKey(0);
    if(c=='y'){
        TAB.addTitle("GEBIEDEN_SELECTEREN");
        init(video);
        TAB.removeTitle();
    }
    cout << TAB.getShort() << "Opnieuw Selecteren van templates voor de getallen?: YES = y,  NO = n" << endl;
    c=(char)waitKey(0);
    if(c=='y'){
        TAB.addTitle("GETAL_TEMPLATES");
        getDigits(frame_capture);
        TAB.removeTitle();
    }

    getDigits(frame_capture);
    int aantal_levens;
    int aantal_sleutels;
    Mat score_frame = Mat(frame.clone(),SCORE_GEBIED);
    Mat top_score_frame = Mat(frame.clone(),TOP_SCORE_GEBIED);
    Mat levens_frame = Mat(frame.clone(),LEVENS_GEBIED);
    Mat sleutels_frame = Mat(frame.clone(),SLEUTELS_GEBIED);
    Mat speelveld_frame = Mat(frame.clone(),SPEELVELD_GEBIED);


    TAB.addTitle("RUNNING GAME");


    //goede_punten = selectPoins(frame.clone());
    //slechte_punten = selectPoins(frame.clone());

    while(1){
        // Capture frame-by-frame
        frame_capture >> frame;

        // If the frame is empty, break immediately
        if (frame.empty())
            break;

        score_frame = Mat(frame.clone(),SCORE_GEBIED);
        top_score_frame = Mat(frame.clone(),TOP_SCORE_GEBIED);
        levens_frame = Mat(frame.clone(),LEVENS_GEBIED);
        sleutels_frame = Mat(frame.clone(),SLEUTELS_GEBIED);
        speelveld_frame = Mat(frame.clone(),SPEELVELD_GEBIED);

        aantal_levens = countContours(levens_frame);
        aantal_sleutels = countContours(sleutels_frame);
        cout << TAB.getShort()<< "LEVENS: " << aantal_levens << endl;
        cout << TAB.getShort()<< "SLEUTELS: " << aantal_sleutels << endl;


        Mat canvas = frame.clone(); //canvas om op te tekenen
        imshow("frame", frame);

        // Press  ESC on keyboard to exit
        char c=(char)waitKey(25);
        if(c==27)
            break;
    }
    return 0;
}

void init(string videopath){
    VideoCapture capture(videopath); //videocapture
    Mat initialisatie_frame;
    capture >> initialisatie_frame; //eerste frame
    TAB.addTitle("STAP_1");
    cout << TAB.getFull() <<"SELECTEER HET SCOREGEBIED" << endl;
    SCORE_GEBIED = getRect(initialisatie_frame);
    TAB.removeTitle();
    TAB.addTitle("STAP_2");
    cout << TAB.getFull() << "SELECTEER HET TOPSCOREGEBIED"<<endl;
    TOP_SCORE_GEBIED = getRect(initialisatie_frame);
    TAB.removeTitle();
    TAB.addTitle("STAP_3");
    cout << TAB.getFull() << "SELECTEER HET AANTAL SLEUTELS GEBIED"<<endl;
    SLEUTELS_GEBIED = getRect(initialisatie_frame);
    TAB.removeTitle();
    TAB.addTitle("STAP_4");
    cout << TAB.getFull() << "SELECTEER HET AANTAL LEVENS GEBIED"<<endl;
    LEVENS_GEBIED = getRect(initialisatie_frame);
    TAB.removeTitle();
    TAB.addTitle("STAP_5");
    cout << TAB.getFull() << "SELECTEER HET SPEELVELD"<<endl;
    SPEELVELD_GEBIED = getRect(initialisatie_frame);
    //TAB.removeTitle();
    /*
    TAB.addTitle("STAP_6");
    cout << TAB.getFull() << "SELECTEER EEN VOORBEELD SPOOKJE"<<endl;
    GHOST_EXAMPLE = getExampleFromStream(capture);
    imwrite(GHOST_EXAMPLE_PATH, GHOST_EXAMPLE);
    */
    TAB.removeTitle();
    TAB.addTitle("STAP_6");
    cout << TAB.getFull() << "SELECTEER KLEURPIXEL VAN HET RODE SPOOKJE"<<endl;
    //vector<Point> spookje_1_kleur = selectPoins(initialisatie_frame);

    TAB = TitleTabs();


    //TAB.removeTitle();
    //TAB.removeTitle();
}

void filterBlue(Mat frame){
    vector<Mat> kanalen;        //vector aanmaken met van 3 kannalen
    split(frame, kanalen);      //beeld opsplitsen en de kanalen opslaan in de vector

    ///____________________stap2: ELKE PIXEL TESTEN OP HUIDSKLEUR (mbv de 3 kanalen)
    Mat masker = Mat::zeros(frame.rows, frame.cols, CV_8UC1);       //nieuw maskker aanmaken van juiste grote
    Mat B = kanalen[0];                  // Blauw kanaal
    Mat G = kanalen[1];                  // Groen kanaal
    Mat R = kanalen[2];                  // Rood kanaal
    masker = (B>200); // 2de methode: efficiente manier
}

Rect getRect(Mat frame){
    cout << TAB.getShort() << "selecteer twee punten en druk vervolgens op esc" << endl;
    Mat canvas = frame.clone();
    namedWindow("selectie boundingbox", WINDOW_AUTOSIZE);
    selectRectParams hoekpunten;
    setMouseCallback("selectie boundingbox", SelectRectPoints, &hoekpunten);
    while(true){

        canvas = frame.clone();
        if(hoekpunten.selectie_punten.size() == 1){
                cout <<"tot hier"<<endl;

            Point p1 = hoekpunten.selectie_punten[0];
            Point p2 = hoekpunten.current_mouse_location;
            rectangle(canvas, Point(min(p1.x, p2.x), min(p1.y, p2.y)), Point(max(p1.x, p2.x), max(p1.y, p2.y)), Scalar(255,255,0), 1, 8, 0);
        }
        if(hoekpunten.selectie_punten.size() == 2){
            Point p1 = hoekpunten.selectie_punten[0];
            Point p2 = hoekpunten.selectie_punten[1];
            rectangle(canvas, Point(min(p1.x, p2.x), min(p1.y, p2.y)), Point(max(p1.x, p2.x), max(p1.y, p2.y)), Scalar(255,255,0), 1, 8, 0);
        }
        imshow("selectie boundingbox", canvas);

        char c=(char)waitKey(25);
        if(c==27){
            if (hoekpunten.selectie_punten.size() == 2){
                cout << TAB.getShort() <<"SUCCES: U hebt een gebied geselecteerd" << endl;
                break;
            }else{
                cout << TAB.getShort() <<"FAIL: U moet 2 punten selecteren om een correct gebied aan te duiden, Probeer opnieuw" << endl;
            }
        }
    }
    Point p1 = hoekpunten.selectie_punten[0];
    Point p2 = hoekpunten.selectie_punten[1];
    Rect selectie = Rect(min(p1.x, p2.x), min(p1.y, p2.y), abs(p1.x-p2.x), abs(p1.y-p2.y));
    cout << " X:" << selectie.x << " Y:" << selectie.y << "Width: " << selectie.width << "Height: "<< selectie.height << endl;
    destroyWindow("selectie boundingbox");
    return selectie;
}

int countContours(Mat beeld){
    Mat beeld_gijs;
    cvtColor(beeld,beeld_gijs,COLOR_BGR2GRAY);
    Mat gebied_grijs_thresh = (beeld_gijs>10)*255;

    vector<vector<Point>> contours;
    findContours(gebied_grijs_thresh.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    return contours.size();
}

vector<Rect> getDigitsRects(Mat beeld){
    Mat gebied_grijs;
    Mat gebied_grijs_thresh;

    cvtColor(beeld,gebied_grijs,COLOR_BGR2GRAY);
    gebied_grijs_thresh = (gebied_grijs>100)*255;

    vector<vector<Point>> contours;
    findContours(gebied_grijs_thresh.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size());
    for( int i = 0; i < contours.size(); i++ ){
        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
    }
    return boundRect;
}
/*
vector<Point> selectPoins(Mat frame){
    namedWindow("selectie_punten", WINDOW_AUTOSIZE);
    vector<Point> punten;
    setMouseCallback("selectie_punten", fillPointVector, &punten);
    imshow("selectie_punten", frame);
    cout << TAB.getShort() << "linker muisknop om een punt toe te voegen, " << endl;
    cout << TAB.getShort() << "rechter muisknop om het laatste punt te verwijderen, " << endl;
    cout << TAB.getShort() << "middel muisknop om alle punten op te sommen, " << endl;
    waitKey(0);
    return punten;
}
*/
Mat getExampleFromStream(VideoCapture stream){
    Mat frame;
    Mat gevonden;
    cout << TAB.getShort() << "selecteer twee punten en druk vervolgens op esc" << endl;
    cout << TAB.getShort() << "druk om de (a) toets voor het volgende frame" << endl;
    stream >> frame;
    Mat canvas = frame.clone();
    namedWindow("selectie boundingbox video", WINDOW_AUTOSIZE);
    selectRectParams hoekpunten;
    setMouseCallback("selectie boundingbox video", SelectRectPoints, &hoekpunten);
    while(true){
        canvas = frame.clone();
        if(hoekpunten.selectie_punten.size() == 1){
            Point p1 = hoekpunten.selectie_punten[0];
            Point p2 = hoekpunten.current_mouse_location;
            rectangle(canvas, Point(min(p1.x, p2.x), min(p1.y, p2.y)), Point(max(p1.x, p2.x), max(p1.y, p2.y)), Scalar(255,255,0), 1, 8, 0);
        }
        if(hoekpunten.selectie_punten.size() == 2){
            Point p1 = hoekpunten.selectie_punten[0];
            Point p2 = hoekpunten.selectie_punten[1];
            int max_x = max(p1.x, p2.x);
            int min_x = min(p1.x, p2.x);
            int max_y = max(p1.y, p2.y);
            int min_y = min(p1.y, p2.y);
            rectangle(canvas, Point(min(p1.x, p2.x), min(p1.y, p2.y)), Point(max(p1.x, p2.x), max(p1.y, p2.y)), Scalar(255,255,0), 1, 8, 0);
        }
        imshow("selectie boundingbox video", canvas);

        char c=(char)waitKey(25);
        if(c==27){
            if (hoekpunten.selectie_punten.size() == 2){
                cout << TAB.getShort() <<"SUCCES: U hebt een gebied geselecteerd" << endl;
                break;
            }else{
                cout << TAB.getShort() <<"FAIL: U moet 2 punten selecteren om een correct gebied aan te duiden, Probeer opnieuw" << endl;
            }
        }
        if(c=='a'){
                cout << TAB.getShort() <<"Next Frame" << endl;
                stream >> frame;
        }
    }
    Point p1 = hoekpunten.selectie_punten[0];
    Point p2 = hoekpunten.selectie_punten[1];
    Rect selectie = Rect(min(p1.x, p2.x), min(p1.y, p2.y), abs(p1.x-p2.x), abs(p1.y-p2.y));
    gevonden = Mat(frame, selectie);
    cout << " X:" << selectie.x << " Y:" << selectie.y << "Width: " << selectie.width << "Height: "<< selectie.height << endl;
    destroyWindow("selectie boundingbox video");
    return gevonden;
}

void showInit(Mat frame){
    Mat canvas = frame.clone();
    namedWindow("init settings", WINDOW_AUTOSIZE);
    rectangle(canvas, SCORE_GEBIED, Scalar(255,255,0), 1, 8, 0);
    rectangle(canvas, TOP_SCORE_GEBIED, Scalar(255,255,0), 1, 8, 0);
    rectangle(canvas, SPEELVELD_GEBIED, Scalar(255,255,0), 1, 8, 0);
    rectangle(canvas, LEVENS_GEBIED, Scalar(255,255,0), 1, 8, 0);
    rectangle(canvas, SLEUTELS_GEBIED, Scalar(255,255,0), 1, 8, 0);
    imshow("init settings",canvas);
    waitKey(0);
}

void getDigits(VideoCapture stream){
    Mat frame;
    Mat gevonden;
    cout << TAB.getShort() << "druk op de (a) toets voor het volgende frame" << endl;
    cout << TAB.getShort() << "druk op de (s) om een template van een getal op te slaan" << endl;
    cout << TAB.getShort() << "druk op de (esc) om verder te gaan" << endl;
    stream >> frame;
    Mat canvas = frame.clone();
    namedWindow("getalen opslaan", WINDOW_AUTOSIZE);
    while(true){
        canvas = frame.clone();
        Mat score_frame = Mat(frame, SCORE_GEBIED);
        string indexnummer = "0";
        vector<Rect> digit_boxes = getDigitsRects(score_frame);
        for(int i = 0; i < digit_boxes.size(); i++){
            digit_boxes[i].x += SCORE_GEBIED.x;
            digit_boxes[i].y += SCORE_GEBIED.y;
            rectangle(canvas, digit_boxes[i], Scalar(255,255,0), 1, 8, 0);
            putText(canvas,to_string(i),Point(digit_boxes[i].x,digit_boxes[i].y + digit_boxes[i].height*2+3),FONT_HERSHEY_PLAIN, 1,  Scalar(0,255,0));
        }

        imshow("getalen opslaan", canvas);

        char c=(char)waitKey(25);
        if(c==27){
            break;
        }
        if(c=='a'){
            cout << TAB.getShort() <<"Next Frame" << endl;
            stream >> frame;
        }
        if(c =='s'){
            int index;
            int nummer;
            Mat selectie;
            string path = "";
            cout << TAB.getShort() <<"welke decimaal wil je opslaan?: geef het index nummer" << endl;
            cin >> index;
            cout << TAB.getShort() <<"wat is het getal?(1-9)" << endl;
            cin >> nummer;
            selectie = Mat(frame, digit_boxes[index]);
            path.append(VOORBEELDEN_MAP);
            path.append("/");
            path.append(to_string(nummer));
            path.append(".jpg");
            imwrite(path, selectie);
            cout << TAB.getShort() <<"saved at: " << path << endl;
        }
    }
    destroyWindow("getalen opslaan");
}

