#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, const char ** argv){
    CommandLineParser parser(argc, argv,
        "{help h usage? |    |show this message}"
        "{parameter p   |    |(required) message parameter}"
    );

    if(parser.has("help")){
        parser.printMessage();
        return -1;
    }

    string parameter(parser.get<string>("parameter"));
    if(parameter.empty()){
        parser.printMessage();
        return -1;
    }

    Mat image;
    image = imread(parameter, CV_LOAD_IMAGE_COLOR);

    if(! image.data){
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    namedWindow( "Display window", WINDOW_AUTOSIZE );    // Create a window for display.
    imshow( "Display window", image );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
