#include "pacman.h" //bevat de globale variabelen, verwijzingen en nodige structuren


using namespace std;
using namespace cv;

/// GEBRUIKTE FUNCTIES, DE UITLEG VAN ELKE FUNCTIE IS HIERONDER TERUG TE VINDEN IN DE
/// DE DEFINITIE VAN ELKE FUNCTIE
void initGebieden(string);
Rect getRect(Mat);
int countContours(Mat);
vector<Rect> getDigitsRects(Mat);
vector<Point> selectPoins(Mat);
Mat getExampleFromStream(VideoCapture);
void getDigits(VideoCapture);
void showInit(Mat);
int getNumber(Mat);
vector<int> meerdere_detectie(Mat, Mat);
Vec3b getColor(VideoCapture);
void initKleuren(string);
Point getPosOfBiggestColourBlob(Mat frame, Vec3b s, int range);


///deze functie gebruik ik als callback functie voor het sort() van een vector met contouren
///op deze manier worden de contouren gesorteerd volgens hun oppervlakte groote
bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
    double i = fabs( contourArea(cv::Mat(contour1)) );
    double j = fabs( contourArea(cv::Mat(contour2)) );
    return ( i < j );
}

///calback functie voor het achterhalen van de coordinaten waar in het venster werdt geklikt
void getPoint(int event, int x, int y, int flags, void* data){
    Point* p = (Point*)data; //datapointer naar punt variabele
    if  ( event == EVENT_LBUTTONDOWN ) //click event van de linker muisknop
    {
        p->x = x;
        p->y = y;
        cout << TAB.getShort() << "nieuw punt geselecteerd" << endl;
    }

}

///vullen van een vector die een selectie van punten bevat waar geklickt is geweest in het venster
void fillPointVector(int event, int x, int y, int flags, void* data)
{
    vector<Point>* selectie = (vector<Point>*)data;  //datapointer naar vector met punten
    if  ( event == EVENT_LBUTTONDOWN )  //linker muisknop event
    {
        selectie->push_back(Point(x,y)); //toevoegen aan punt in de lijst
        cout << TAB.getShort() << "nieuw punt toegevoegd" << endl;

    }
    else if  ( event == EVENT_RBUTTONDOWN )  //rechter muisknop event
    {
        if(selectie->size()>0){
            selectie->pop_back(); //laatste punt verwijderen in de lijst
            cout << TAB.getShort() << "laatste punt is verwijdert" << endl;
        }
    }
    else if  ( event == EVENT_MBUTTONDOWN ) //midelste muisknop event
    {
        cout << TAB.getShort() << "alle punten:" << endl;
        for(int i = 0; i<selectie->size(); i++){ //loop die al de punten afprint
            cout << TAB.getShort()<< "Punt"<< i << ": "<< selectie->at(i).x <<","<< selectie->at(i).y << endl;
        }
    }
}

/// een callback funcite die de huidige muispositie teruggeeft en een lijst van punten,
/// deze lijst kan maximum 2 punten bevatten, de structuur van "selectRectParams" is
/// terug te vinden in de header file
void SelectRectPoints(int event, int x, int y, int flags, void* data)
{
    selectRectParams* selectie = (selectRectParams*)data;
    if  ( event == EVENT_LBUTTONDOWN ) // linker muisbutten event
    {
        if(selectie->selectie_punten.size()>=2){ //als er al 2 of meer punten zijn
            // voeg geen extra punt toe
            cout << TAB.getShort()  << "U hebt al reeds twee hoekpuntenpunten gekozen: druk op de rechter muisknop om het laatste toegevoegde punt te annuleren" << endl;
        }else{ // anders
            selectie->selectie_punten.push_back(Point(x,y)); // voeg de huidige muislocatie toe aan de puntenlijst
            cout << TAB.getShort() << "Nieuw hoekpuntpunt toegevoegd" << endl;
        }
    }
    else if  ( event == EVENT_RBUTTONDOWN ) // linker muisbutten event
    {
        if(selectie->selectie_punten.size()>0){ // als punten in de lijst staan
            selectie->selectie_punten.pop_back(); // verwijder laatst toegevoegde punt
            cout << TAB.getShort() << "   Er zijn geen geselecteerde punten meer" << endl;
        }
    }
    else if (event == EVENT_MOUSEMOVE){ // de muis beweegt in het venster
        selectie->current_mouse_location = Point(x,y); // huidige x en y positie worden waard in pointerdata
    }
}

int main(int argc, const char ** argv){
    //om een of andere rede kon ik dit niet in de header doen... dus doe ik het maar hier
    //hier definieer ik dus de op voorhand gekende kleuren van de figuurtjes
    //deze kunnen nog gewijzigd worden in de loop van het programma
    //door bepaalde pixels aan te duiden in een videostream
    KLEUR_PACMAN[0] = (char)8;
    KLEUR_PACMAN[1] = (char)254;
    KLEUR_PACMAN[2] = (char)251;

    KLEUR_SPOOK_ORANJE[0] = (char)87;
    KLEUR_SPOOK_ORANJE[1] = (char)183;
    KLEUR_SPOOK_ORANJE[2] = (char)247;

    KLEUR_SPOOK_ROOD[0] = (char)1;
    KLEUR_SPOOK_ROOD[1] = (char)1;
    KLEUR_SPOOK_ROOD[2] = (char)148;

    KLEUR_SPOOK_GROEN[0] = (char)249;
    KLEUR_SPOOK_GROEN[1] = (char)255;
    KLEUR_SPOOK_GROEN[2] = (char)23;

    KLEUR_SPOOK_ROZE[0] = (char)251;
    KLEUR_SPOOK_ROZE[1] = (char)169;
    KLEUR_SPOOK_ROZE[2] = (char)244;
    //////////////////////////////////////////////////////////////////////////////////////
    ////                                 SETUP                                         ///
    //////////////////////////////////////////////////////////////////////////////////////
    TAB.addTitle("PACMAN"); // voeg eertste tittel toe

    ///______________INLEZEN VAN ARGUMENTEN VIA PARSER_______________
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

    /// _____________________INLEZEN IMAGES/AANMAKEN VAN VIDEO CAPTURE_____________________________

    cout << TAB.getShort() << "given sourcefile for pacman video:" << video << std::endl;

    VideoCapture frame_capture(video); //videocapture

    Mat frame;

    frame_capture >> frame; //eerste frame


    ///_____________TESTEN OF IMAGES EN VIDEOCAPTURE CORRECT INGELADEN ZIJN________________
    if(!frame_capture.isOpened()){
        cout << TAB.getShort() << "Error opening video stream" << endl;
        return -1;
    }

    if(frame.empty()){
        cout << TAB.getShort() <<  "Could not get frame from stream" << std::endl ;
        return -1;
    }

    namedWindow( "frame", WINDOW_AUTOSIZE );        // weergave van eerste frame
    imshow("frame", frame);         // tonen van eerste frame

    TAB.addTitle("INITIALISATIE"); //Toevoegen volgende titel
    cout << TAB.getShort() << "Opnieuw Initialiseren van Gebieden?: y/n" << endl;
    showInit(frame); // geef de huidige gebieden weer
    char c=(char)waitKey(0);
    if(c=='y'){ // als "y" initialiseer de gebieden opnieuw
        TAB.addTitle("GEBIEDEN_SELECTEREN");//Toevoegen volgende titel
        initGebieden(video); // intitaliseren van globale gebieden (vierkanten)
        TAB.removeTitle(); //verwijderen van laatste titel
    }
    cout << TAB.getShort() << "Opnieuw Initialiseren van Nodige Kleurwaarden?: YES = y,  NO = n" << endl;
    c=(char)waitKey(0);
    if(c=='y'){ // als "y" initialiseer de kleur waarden opnieuw
        TAB.addTitle("KLEUREN_SELECTEREN"); //Toevoegen volgende titel
        initKleuren(video); // initialiseren van de kleurwaarde van pacman en de sppokjes
        TAB.removeTitle(); //verwijderen van laatste titel
    }
    cout << TAB.getShort() << "Opnieuw Selecteren van templates voor de getallen?: YES = y,  NO = n" << endl;
    c=(char)waitKey(0);
    if(c=='y'){ // als "y" initialiseer de kleur waarden opnieuw
        TAB.addTitle("GETAL_TEMPLATES"); //Toevoegen volgende titel
        getDigits(frame_capture); //maak opnieuw de templates voor de getallen
        TAB.removeTitle(); //verwijderen van laatste titel
    }
    TAB.removeTitle(); //verwijder de intitlaiseer titel

    //de variabelen die we wensen te achterhalen
    int aantal_levens;
    int aantal_sleutels;
    int score;
    int top_score;
    Point pacman_positie;
    Point oranje_spook_positie;
    Point groen_spook_positie;
    Point roze_spook_positie;
    Point rood_spook_positie;

    //voor elk frame de selectie die we bekomen door de gebieden
    Mat score_frame = Mat(frame.clone(),SCORE_GEBIED);
    Mat top_score_frame = Mat(frame.clone(),TOP_SCORE_GEBIED);
    Mat levens_frame = Mat(frame.clone(),LEVENS_GEBIED);
    Mat sleutels_frame = Mat(frame.clone(),SLEUTELS_GEBIED);
    Mat speelveld_frame = Mat(frame.clone(),SPEELVELD_GEBIED);


    TAB.addTitle("RUNNING GAME"); //toevoegen titel

    while(1){ //gameloop word afgebroken als alle frames zijn geweest of er op escape wordt gedrukt
        // Capture frame-by-frame
        frame_capture >> frame; // volgende frame

        // If the frame is empty, break immediately
        if (frame.empty())
            break;

        //voor elk frame de selectie die we bekomen door de gebieden
        score_frame = Mat(frame.clone(),SCORE_GEBIED);
        top_score_frame = Mat(frame.clone(),TOP_SCORE_GEBIED);
        levens_frame = Mat(frame.clone(),LEVENS_GEBIED);
        sleutels_frame = Mat(frame.clone(),SLEUTELS_GEBIED);
        speelveld_frame = Mat(frame.clone(),SPEELVELD_GEBIED);

        //achterhalen van de nodige gegevens
        aantal_levens = countContours(levens_frame);
        aantal_sleutels = countContours(sleutels_frame);
        score = getNumber(score_frame.clone());
        top_score = getNumber(top_score_frame.clone());
        pacman_positie = getPosOfBiggestColourBlob(speelveld_frame, KLEUR_PACMAN, KLEUREN_RANGE);
        oranje_spook_positie = getPosOfBiggestColourBlob(speelveld_frame, KLEUR_SPOOK_ORANJE, KLEUREN_RANGE);
        groen_spook_positie = getPosOfBiggestColourBlob(speelveld_frame, KLEUR_SPOOK_GROEN, KLEUREN_RANGE);
        rood_spook_positie = getPosOfBiggestColourBlob(speelveld_frame, KLEUR_SPOOK_ROZE, KLEUREN_RANGE);
        roze_spook_positie = getPosOfBiggestColourBlob(speelveld_frame, KLEUR_SPOOK_ROZE, KLEUREN_RANGE);

        //afprinten van de gevonden waarden
        cout << TAB.getShort()<< "============================================" << aantal_levens << endl;
        cout << TAB.getShort()<< "==  LEVENS: " << aantal_levens << endl;
        cout << TAB.getShort()<< "==  SLEUTELS: " << aantal_sleutels << endl;
        cout << TAB.getShort()<< "==  SCORE: " << score << endl;
        cout << TAB.getShort()<< "==  TOPSCORE: " << top_score << endl;
        cout << TAB.getShort()<< "==  PACMAN POSITIE: " <<  "x: "<< pacman_positie.x << "y: " << pacman_positie.y << endl;
        cout << TAB.getShort()<< "==  ROOD SPOOK POSITIE: " <<  "x: "<< rood_spook_positie.x << "y: " << rood_spook_positie.y << endl;
        cout << TAB.getShort()<< "==  GROEN SPOOK POSITIE: " <<  "x: "<< groen_spook_positie.x << "y: " << groen_spook_positie.y << endl;
        cout << TAB.getShort()<< "==  ORANJE SPOOK POSITIE: " <<  "x: "<< oranje_spook_positie.x << "y: " << oranje_spook_positie.y << endl;
        cout << TAB.getShort()<< "==  ROZE SPOOK POSITIE: " <<  "x: "<< roze_spook_positie.x << "y: " << roze_spook_positie.y << endl;
        cout << TAB.getShort()<< "============================================" << aantal_levens << endl;

        //Mat canvas = frame.clone(); //canvas om op te tekenen eventueel(nog ni gebruikt)
        imshow("frame", frame); // toon frame

        // Press  ESC on keyboard to exit
        char c=(char)waitKey(25);
        if(c==27)
            break;
    }
    return 0;
}

/// functie die de globale kleuren voor het detecteren van pacman en de spookjes kan wijzigen
/// door in een video een punt te selecteren en de kleur te achterhalen
void initKleuren(string videopath){
    VideoCapture capture(videopath); //videocapture
    TAB.addTitle("STAP_1"); //toevoegen titel
    cout << TAB.getFull() << "SELECTEER EEN KLEURENPIXEL VAN PACMAN"<<endl;
    Vec3b kleur = getColor(capture); //gevonden kleur (bgr)
    cout << TAB.getFull() << "KLEUR B:" << (int)kleur[0] << "  G:" << (int)kleur[1] << "  R:" << (int)kleur[2] << endl;
    KLEUR_PACMAN = kleur; //toekennen kleurwaarde
    TAB.removeTitle(); //verwijderen titel

    TAB.addTitle("STAP_2"); //toevoegen titel
    cout << TAB.getFull() << "SELECTEER EEN KLEURENPIXEL VAN ROOD SPOOK"<<endl;
    kleur = getColor(capture); //gevonden kleur (bgr)
    cout << TAB.getFull() << "KLEUR B:" << (int)kleur[0] << "  G:" << (int)kleur[1] << "  R:" << (int)kleur[2] << endl;
    KLEUR_SPOOK_ROOD = kleur; //toekennen kleurwaarde
    TAB.removeTitle(); //verwijderen titel

    TAB.addTitle("STAP_3"); //toevoegen titel
    cout << TAB.getFull() << "SELECTEER EEN KLEURENPIXEL VAN ORANJE SPOOK"<<endl;
    kleur = getColor(capture); //gevonden kleur (bgr)
    cout << TAB.getFull() << "KLEUR B:" << (int)kleur[0] << "  G:" << (int)kleur[1] << "  R:" << (int)kleur[2] << endl;
    KLEUR_SPOOK_ORANJE = kleur; //toekennen kleurwaarde
    TAB.removeTitle(); //verwijderen titel

    TAB.addTitle("STAP_4"); //toevoegen titel
    cout << TAB.getFull() << "SELECTEER EEN KLEURENPIXEL VAN ROZE SPOOK"<<endl;
    kleur = getColor(capture); //gevonden kleur (bgr)
    cout << TAB.getFull() << "KLEUR B:" << (int)kleur[0] << "  G:" << (int)kleur[1] << "  R:" << (int)kleur[2] << endl;
    KLEUR_SPOOK_ROZE = kleur; //toekennen kleurwaarde
    TAB.removeTitle(); //verwijderen titel

    TAB.addTitle("STAP_5"); //toevoegen titel
    cout << TAB.getFull() << "SELECTEER EEN KLEURENPIXEL VAN GROEN SPOOK"<<endl;
    kleur = getColor(capture); //gevonden kleur (bgr)
    cout << TAB.getFull() << "KLEUR B:" << (int)kleur[0] << "  G:" << (int)kleur[1] << "  R:" << (int)kleur[2] << endl;
    KLEUR_SPOOK_GROEN = kleur; //toekennen kleurwaarde
    TAB.removeTitle(); //verwijderen titel
}

/// deze functie geeft de positie van de grootste blob van een specifieke kleur terug
Point getPosOfBiggestColourBlob(Mat frame, Vec3b s, int range){
    // te vinden rgb waarde
    int B = (int)s[0];
    int G = (int)s[1];
    int R = (int)s[2];

    // maxima en minema van te vinden rgb waarde na op en aftellen van de range waarde
    int max_B = min(B+range, 255);
    int max_G = min(G+range, 255);
    int max_R = min(R+range, 255);

    int min_B = max(0,B-range);
    int min_G = max(0,G-range);
    int min_R = max(0,R-range);

    Mat masker;// masker

    inRange(frame, Scalar(min_B, min_G, min_R), Scalar(max_B, max_G, max_R), masker); //segmentatie
    //imshow("masker", masker); toon het gevonden masker
    //dilate(masker, masker, Mat(), Point(-1, -1), 3, 1, 1); // dilatie (maar werkt ni om een of andere rede)
    vector<vector<Point>> contours; //vector met alle contouren
    findContours(masker, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE); // vinden van alle contouren in het masker
    Point p = Point(0,0); //gevonden punt
    if(contours.size()>0){ // als er een contour gevonden is
        sort(contours.begin(), contours.end(), compareContourAreas); // sorteer de gevondne contouren volgens oppervlake groote
                                                                     // de callback funcitie is bovenaan gedefinieerd
        vector<Point> bigest = contours[contours.size()-1]; // grootste contour
        vector<Point> contours_poly; // poligoon voor boundingbox
        Rect boundRect; //boundingbox van grootste countour
        approxPolyDP( Mat(bigest), contours_poly, 3, true ); // contoer omsetten naar poligoon
        boundRect = boundingRect( Mat(contours_poly) ); //poligoon naar vierhoek
        p = Point(boundRect.x + boundRect.width/2, boundRect.y + boundRect.height/2); // punt is midden van gevonden boundingbox
    }
    return p;
}

///init_gebieden geeft de mogelijkheid om voor elk gebied
///een selectie te maken en de globale standaard gebieden opnieuw in te stellen
void initGebieden(string videopath){
    VideoCapture capture(videopath); //videocapture maken
    Mat initialisatie_frame; // frame waarvan we de nodige gebieden selecteren
    capture >> initialisatie_frame; //eerste frame
    TAB.addTitle("STAP_1"); // titel toevoegen in stappenplan
    cout << TAB.getFull() <<"SELECTEER HET SCOREGEBIED" << endl;
    SCORE_GEBIED = getRect(initialisatie_frame); // vierkant selecteren voor nodige gebied
    TAB.removeTitle(); // titel verwijderen uit stappenplan
    TAB.addTitle("STAP_2"); // titel toevoegen in stappenplan
    cout << TAB.getFull() << "SELECTEER HET TOPSCOREGEBIED"<<endl;
    TOP_SCORE_GEBIED = getRect(initialisatie_frame); // vierkant selecteren voor nodige gebied
    TAB.removeTitle(); // titel verwijderen uit stappenplan
    TAB.addTitle("STAP_3"); // titel toevoegen in stappenplan
    cout << TAB.getFull() << "SELECTEER HET AANTAL SLEUTELS GEBIED"<<endl;
    SLEUTELS_GEBIED = getRect(initialisatie_frame); // vierkant selecteren voor nodige gebied
    TAB.removeTitle(); // titel verwijderen uit stappenplan
    TAB.addTitle("STAP_4"); // titel toevoegen in stappenplan
    cout << TAB.getFull() << "SELECTEER HET AANTAL LEVENS GEBIED"<<endl;
    LEVENS_GEBIED = getRect(initialisatie_frame); // vierkant selecteren voor nodige gebied
    TAB.removeTitle(); // titel verwijderen uit stappenplan
    TAB.addTitle("STAP_5"); // titel toevoegen in stappenplan
    cout << TAB.getFull() << "SELECTEER HET SPEELVELD"<<endl;
    SPEELVELD_GEBIED = getRect(initialisatie_frame); // vierkant selecteren voor nodige gebied
    TAB.removeTitle(); // titel verwijderen uit stappenplan

    ///onderstaande was experimenteel om template van spookje te bekomen.
    ///Maar door overlapping van spookjes was dit minder efficient dan zoeken op kleur.

    /*
    TAB.addTitle("STAP_6");
    cout << TAB.getFull() << "SELECTEER EEN VOORBEELD SPOOKJE"<<endl;
    GHOST_EXAMPLE = getExampleFromStream(capture);
    imwrite(GHOST_EXAMPLE_PATH, GHOST_EXAMPLE);
    */
}

///functie waarin je een vierkant kan selecteren op een venster
///terwijl je een selectie maakt krijg je fiedback van waar het gevonden gebied zich bevind
Rect getRect(Mat frame){
    cout << TAB.getShort() << "selecteer twee punten en druk vervolgens op esc" << endl;
    Mat canvas = frame.clone(); //copie van het frame om op te tekenen
    namedWindow("selectie boundingbox", WINDOW_AUTOSIZE); //maken van venster
    selectRectParams hoekpunten; //hokpunten en huidige muispositie
    setMouseCallback("selectie boundingbox", SelectRectPoints, &hoekpunten); //toevoegen van callback funcite en datapointer(hoekpuntne)
    while(true){ //loop die word beindigt als er op "esc" wordt gedrukt en er twee punten zijn geselecteerd
        canvas = frame.clone(); //nieuw canvas
        if(hoekpunten.selectie_punten.size() == 1){ // als er maar een punt geseleteerd is
                                                    // teken dan het vierkant bepaald door dit punt
                                                    // en de huidige muispositie
            Point p1 = hoekpunten.selectie_punten[0];
            Point p2 = hoekpunten.current_mouse_location;
            rectangle(canvas, Point(min(p1.x, p2.x), min(p1.y, p2.y)), Point(max(p1.x, p2.x), max(p1.y, p2.y)), Scalar(255,255,0), 1, 8, 0);
        }
        if(hoekpunten.selectie_punten.size() == 2){ // als er twee punten zijn geselecteerd teken dan
                                                    // de rechthoek die hierdoor wordt bepaald
            Point p1 = hoekpunten.selectie_punten[0];
            Point p2 = hoekpunten.selectie_punten[1];
            rectangle(canvas, Point(min(p1.x, p2.x), min(p1.y, p2.y)), Point(max(p1.x, p2.x), max(p1.y, p2.y)), Scalar(255,255,0), 1, 8, 0);
        }
        imshow("selectie boundingbox", canvas); // toon het canvas

        char c=(char)waitKey(25);
        if(c==27){// enkel sluiten als er twee punten zijn geselecteerd en er op "esc" wordt gedrukt
            if (hoekpunten.selectie_punten.size() == 2){
                cout << TAB.getShort() <<"SUCCES: U hebt een gebied geselecteerd" << endl;
                break;
            }else{
                cout << TAB.getShort() <<"FAIL: U moet 2 punten selecteren om een correct gebied aan te duiden, Probeer opnieuw" << endl;
            }
        }
    }
    Point p1 = hoekpunten.selectie_punten[0]; //eerste hoekpunt
    Point p2 = hoekpunten.selectie_punten[1]; //tweede hoekpunt
    Rect selectie = Rect(min(p1.x, p2.x), min(p1.y, p2.y), abs(p1.x-p2.x), abs(p1.y-p2.y)); //maken van Rect door min en max van gevondne punten
    cout << TAB.getShort() << " X:" << selectie.x << " Y:" << selectie.y << "Width: " << selectie.width << "Height: "<< selectie.height << endl;
    destroyWindow("selectie boundingbox");
    return selectie;
}

/// funcite die het aantal contouren telt dat kan gevonden worden in een beeld na thresholding van grijswaarde beeld
int countContours(Mat beeld){
    Mat beeld_gijs; // grijswaarden beeld
    cvtColor(beeld,beeld_gijs,COLOR_BGR2GRAY); //kleuren beeld naar grijswaarden
    Mat gebied_grijs_thresh = (beeld_gijs>10)*255; //10 (lage waarde) want achtergrond is altijd zwart

    vector<vector<Point>> contours; // contouren
    findContours(gebied_grijs_thresh.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_NONE); //vind de contouren
    return contours.size(); //geef aantal contouren terug
}

///geeft een vector van boundingboxen terug voor het vinden van getallen
vector<Rect> getDigitsRects(Mat beeld){
    Mat gebied_grijs; // grijswaarden beeld
    Mat gebied_grijs_thresh; // masker

    cvtColor(beeld,gebied_grijs,COLOR_BGR2GRAY); //kleurenbeeld naar grijswaarde
    gebied_grijs_thresh = (gebied_grijs>100)*255; // grenswaarde is een beetje experimenteel maar werkt goed

    vector<vector<Point>> contours; //contouren
    findContours(gebied_grijs_thresh.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_NONE); // vinden van contouren threshold

    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size());
    for( int i = 0; i < contours.size(); i++ ){// elke contour omzetten naar een boundingbox
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


/// Een functie voor het  selecteren van een kleine afbeelding (uit boundingbox) uit een video;
/// (werd oorspronkelijk gebruikt voor het selecteren van een template van het spookeje, maar kleur werkte beter voor
/// het detecteren van een spookje en pacman)
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

/// Tekenen van al de gebieden
void showInit(Mat frame){
    Mat canvas = frame.clone(); //canvas om viekhoeken op te tekenen
    //tekenen van de gebieden op het canvas
    rectangle(canvas, SCORE_GEBIED, Scalar(255,255,0), 1, 8, 0);
    rectangle(canvas, TOP_SCORE_GEBIED, Scalar(255,255,0), 1, 8, 0);
    rectangle(canvas, SPEELVELD_GEBIED, Scalar(255,255,0), 1, 8, 0);
    rectangle(canvas, LEVENS_GEBIED, Scalar(255,255,0), 1, 8, 0);
    rectangle(canvas, SLEUTELS_GEBIED, Scalar(255,255,0), 1, 8, 0);
    imshow("gebieden",canvas); //tonen van canvas
    waitKey(0);
}

/// Deze functie geeft een kleurenvector(BGR) terug door een punt te selecteren in een video
Vec3b getColor(VideoCapture stream){
    Mat frame;//huidige frame
    cout << TAB.getShort() << "druk op de (a) toets voor het volgende frame" << endl;
    cout << TAB.getShort() << "selecteer een punt door te klikken" << endl;
    cout << TAB.getShort() << "druk op de (esc) om verder te gaan" << endl;
    stream >> frame; //volgend frame

    namedWindow("selecteer kleur in video");
    Point punt; //huidige punt in venster
    setMouseCallback("selecteer kleur in video", getPoint, &punt); //calllbackfuncie aan frame toevoegen
                                                                   //deze funcit is hierboven gedefinieerd
                                                                   //datapointer naar punt wordt meegegeven

    while(true){ //

        imshow("selecteer kleur in video", frame); //toon frame

        char c=(char)waitKey(25);
        if(c==27){ // escape => exit loop
            break;
        }
        if(c=='a'){ // volgend frame
            cout << TAB.getShort() <<"Next Frame" << endl;
            stream >> frame;
        }
    }
    return frame.at<Vec3b>(punt); // return kleur in punt
}

/// opslaan van templates voor van getallen uit een video
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

/// geeft de getalwaarde in een bepaald beeld op basis van templates
int getNumber(Mat beeld){
    int getalwaarde; //de gevonden getalwaarde
    string path; //huidige path van de template
    Mat templ; //template beeld
    vector<int> digit_positions; //lijst van huidige digits xposites (zie digits.h)
    digit_number dn; //digits bevatte een getalwaarde en een xpos.
                    //kan gesorteerd worden op basi van hun xlocatie
                    //vervolgens kan je de integerwaarde hieruit afleiden
    for(int i = 0; i < 10; i++){ // van 0 tot 9
        // path samenstellen van template locatie
        path = "";
        path.append(VOORBEELDEN_MAP);
        path.append("/");
        path.append(to_string(i));
        path.append(".jpg");
        templ = imread(path); //lees tempate in voor huidig getal
        //imshow(path,templ);
        digit_positions = meerdere_detectie(beeld, templ); // vind de x_posities van het template nummer
        //cout << i << ": X" << digit_positions.size() << endl;

        for(int j = 0; j < digit_positions.size(); j++){//toevoegen van de gevonden digts aan digit nummer
            digit d;
            d.waarde = i;
            d.x_positie = digit_positions[j];
            dn.addDigit(d);
        }
    }
    dn.bubble(); //sorteren van digit nummer
    getalwaarde = dn.getInt(); //achterhalen van integerwaarde uit gesorteerde digitnummer
    return getalwaarde;
    //cout << getalwaarde << endl;
}

///functie die voor een bepaalde template de een lijst van x_posities teruggeeft
vector<int> meerdere_detectie(Mat beeld, Mat templ){
    vector<int> digit_posities;
    Mat canvas = beeld.clone();
    Mat template_gray;
    Mat image_gray;
    Mat match_result;

    //double thresholdwaarde = 0.8;

    cvtColor(templ, template_gray, COLOR_BGR2GRAY);    // afbeelding omzettten naar grijswaarde
    cvtColor(beeld, image_gray, COLOR_BGR2GRAY);    // afbeelding omzettten naar grijswaarde

    // vind een heatmap dat de beste corelaties met het template weergeeft
    matchTemplate(image_gray, template_gray, match_result, TM_CCORR_NORMED); //template matching
    Mat match_result_thres = match_result.clone();
    match_result_thres = (match_result_thres>0.95)*255; //95 procent is experimenteel maar werk goed
    ///imshow("x", match_result_thres);


    //voor elke contour het middelpunt van de daarbijhordende RECT
    vector<vector<Point>> contouren;
    findContours(match_result_thres, contouren, RETR_EXTERNAL, CHAIN_APPROX_NONE);
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
        digit_posities.push_back(p.x);
        //rectangle(canvas, p, Point(p.x + templ.cols, p.y + templ.rows), Scalar(0, 255, 0));
    }
    //imshow("matches", canvas);

    return digit_posities;
}


