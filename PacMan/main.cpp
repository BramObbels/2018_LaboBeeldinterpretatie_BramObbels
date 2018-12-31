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

struct selectRectParams{
    Point current_mouse_location = Point(0,0);
    vector<Point> selectie_punten;
};

void init(Mat);
Rect getRect(Mat);


void SelectRectPoints(int event, int x, int y, int flags, void* data)
{
    selectRectParams* selectie = (selectRectParams*)data;
    if  ( event == EVENT_LBUTTONDOWN )
    {
        if(selectie->selectie_punten.size()>=2){
            cout << "U hebt al reeds twee punten gekozen: druk op de rechter muisknop om het laatste toegevoegde punt te annuleren" << endl;
        }else{
            selectie->selectie_punten.push_back(Point(x,y));
            cout << "Nieuw punt toegevoegd" << endl;
        }
    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
        if(selectie->selectie_punten.size()>0){
            selectie->selectie_punten.pop_back();
            cout << "Er zijn geen geselecteerde punten meer" << endl;
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

    ///______________INLEZEN VAN ARGUMENTEN_______________
    CommandLineParser parser(argc, argv,
        "{help h usage?               |    |show this message}"
        "{video pacman_video          |    |(required) message pacman_video}"
    );

    if(parser.has("help")){
        parser.printMessage();
        cout << "TIP: gebruik voledige padnamen";
        return -1;
    }

    string video(parser.get<string>("video"));


    if(video.empty()){
        parser.printMessage();
        return -1;
    }

    /// ____________INLEZEN IMAGES_____________________________

    cout << "given sourcefile for pacman video:" << video << std::endl;

    VideoCapture frame_capture(video); //videocapture

    Mat frame;

    frame_capture >> frame; //eerste frame
    init(frame);

    ///_____________TESTEN OF IMAGES CORRECT INGELADEN ZIJN________________
    if(!frame_capture.isOpened()){
        cout << "Error opening video stream" << endl;
        return -1;
    }

    if(frame.empty()){
        cout <<  "Could not get frame from stream" << std::endl ;
        return -1;
    }

    namedWindow( "frame", WINDOW_AUTOSIZE );     // Create a window for display.
    imshow("frame", frame);

    waitKey(0);
    while(1){
        // Capture frame-by-frame
        frame_capture >> frame;

        // If the frame is empty, break immediately
        if (frame.empty())
            break;


        Mat canvas = frame.clone(); //canvas om op te tekenen
        imshow( "Frame", canvas );
        // Press  ESC on keyboard to exit
        char c=(char)waitKey(25);
        if(c==27)
            break;
    }
    return 0;
}
void init(Mat initialisatie_frame){
    cout << "INITIALISATIE STAP:" << endl;
    cout << "STAP 1: selecteer het scoregebied" << endl;
    Rect scoregebied;
    scoregebied = getRect(initialisatie_frame);
    Mat gebied = Mat(initialisatie_frame, scoregebied);
    imshow("gebied", gebied);
    waitKey(0);
}
Rect getRect(Mat frame){
    cout << "selecteer twee punten en druk vervolgens op esc" << endl;

    Mat canvas = frame.clone();
    namedWindow("selectie", WINDOW_AUTOSIZE);
    selectRectParams hoekpunten;
    setMouseCallback("selectie", SelectRectPoints, &hoekpunten);
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
        imshow("selectie", canvas);

        char c=(char)waitKey(25);
        if(c==27){
            if (hoekpunten.selectie_punten.size() != 2){
                cout <<"FAIL: U moet 2 punten selecteren om een correct gebied aan te duiden, Probeer opnieuw" << endl;
            }else{
                cout <<"SUCCES: u hebt een gebied geselecteerd" << endl;
                break;
            }
        }
    }
    Point p1 = hoekpunten.selectie_punten[0];
    Point p2 = hoekpunten.selectie_punten[1];
    Rect selectie = Rect(min(p1.x, p2.x), min(p1.y, p2.y), abs(p1.x-p2.x), abs(p1.y-p2.y));
    return selectie;
}
