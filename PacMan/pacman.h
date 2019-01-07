#ifndef PACMAN_H
#define PACMAN_H
#include <opencv2/opencv.hpp> //standaard opencv lib
#include "opencv2/highgui/highgui.hpp" // opencv lib voor gui callbacks
#include <vector>
#include <iostream>
#include <string>
#include "digit.h" //bevat digits, dit is een vector van getallen en hun x positie die geordend kan worden
#include "TitleTabs.h" //gemaakt voor eenvoudig een stappenplan te kunnen afprinten in cout

using namespace std;
using namespace cv;

struct selectRectParams{ // struct nodig voor tekeken van rect terwijl je de muis beweegd
    Point current_mouse_location = Point(0,0); //huidige muislocatie in het venster
    vector<Point> selectie_punten; // geselecteerde punten
};

TitleTabs TAB; //globale variabele die wordt gebruikt om de huidige fase van het programma in te bewaren.
               //dit wordt gebruikt voor de text output

// gebieden die aanduiden waar in het beeld wat te vinden is
Rect SCORE_GEBIED = Rect(109, 9, 63, 14);
Rect TOP_SCORE_GEBIED = Rect(198, 9, 84, 14);
Rect SPEELVELD_GEBIED = Rect(99, 26, 280, 313);
Rect LEVENS_GEBIED = Rect(121, 340, 99, 20);
Rect SLEUTELS_GEBIED = Rect(220, 340, 150, 20);

// welke afwijking er is toegestaan op de kleurwaarde voor segmentatie
// bij het vindne van pacman en de spookjes
int KLEUREN_RANGE = 15;

// globale variabele die de kleuren van de spookjes en pacman beschrijven in rgb waarde
///de echte waarde worden in het begin van main() opgegeven omdat dat hier om een of andere reden
///niet mocht van de compiler
Vec3b KLEUR_PACMAN;
Vec3b KLEUR_SPOOK_ROOD;
Vec3b KLEUR_SPOOK_GROEN;
Vec3b KLEUR_SPOOK_ORANJE;
Vec3b KLEUR_SPOOK_ROZE;

//Mat GHOST_EXAMPLE;

// path naar de map waar de templates worden bewaard
string VOORBEELDEN_MAP = "img";


//string GHOST_EXAMPLE_PATH = "spookje_voorbeeld.jpg";
#endif // PACMAN_H
