#ifndef PACMAN_H
#define PACMAN_H
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
using namespace std;
using namespace cv;

struct selectRectParams{
    Point current_mouse_location = Point(0,0);
    vector<Point> selectie_punten;
};


Rect SCORE_GEBIED = Rect(109, 9, 63, 14);
Rect TOP_SCORE_GEBIED = Rect(198, 9, 84, 14);
Rect SPEELVELD_GEBIED = Rect(99, 26, 280, 313);
Rect LEVENS_GEBIED = Rect(121, 340, 99, 20);
Rect SLEUTELS_GEBIED = Rect(220, 340, 150, 20);

Mat GHOST_EXAMPLE;

string VOORBEELDEN_MAP = "img";
string GHOST_EXAMPLE_PATH = "spookje_voorbeeld.jpg";
#endif // PACMAN_H
