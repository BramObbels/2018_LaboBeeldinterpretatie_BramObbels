#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, const char ** argv){

    //////////////////////////////////////////////////////////////////////////////////////
    ////                                 SETUP                                         ///
    //////////////////////////////////////////////////////////////////////////////////////

    ///______________INLEZEN VAN ARGUMENTEN_______________
    CommandLineParser parser(argc, argv,
        "{help h usage? |    |show this message}"
        "{imgbimodal ImageBimodal   |    |(required) message grayimage}"
        "{imgcolor ImageColor   |    |(required) message colorimage}"
        "{imgcoloradapted ImageColorAdaptive   |    |(required) message colorimage}"
    );

    if(parser.has("help")){
        parser.printMessage();
        cout << "TIP: gebruik voledige padnamen" <<endl;
        return -1;
    }

    string imgbimodal(parser.get<string>("imgbimodal"));
    string imgcolor(parser.get<string>("imgcolor"));
    string imgcoloradapted(parser.get<string>("imgcoloradapted"));

    if(imgbimodal.empty() || imgcolor.empty() || imgcoloradapted.empty()){
        parser.printMessage();
        return -1;
    }


    ///_____________AANMAKEN AVARIABELEN____________________

    Mat bimodal;
    Mat color;
    Mat coloradapted;

    /// ____________INLEZEN IMAGES_____________________________

    cout << imgbimodal << std::endl;
    cout << imgcolor << std::endl;
    cout << imgcoloradapted << std::endl;

    bimodal = imread(imgbimodal);
    color = imread(imgcolor);
    coloradapted = imread(imgcoloradapted);

    ///_____________TESTEN OF IMAGES CORRECT INGELADEN ZIJN________________

    if(bimodal.empty()){
        cout <<  "Could not open or find the bimodal image" << std::endl ;
        return -1;
    }

    if(color.empty()){
        cout <<  "Could not open or find the color image" << std::endl ;
        return -1;
    }

    if(coloradapted.empty()){
        cout <<  "Could not open or find the coloradapted image" << std::endl ;
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////////////
    ////                                OPDRACHTEN                                  ///
    ///////////////////////////////////////////////////////////////////////////////////
    ///____________________AFBEELDINGEN TONEN___________________
    namedWindow( "bimodalimage", WINDOW_AUTOSIZE );    // Create a window for display.
    namedWindow( "colorimage", WINDOW_AUTOSIZE );    // Create a window for display.
    namedWindow( "coloradaptedimage", WINDOW_AUTOSIZE );    // Create a window for display.

    imshow( "bimodalimage", bimodal );                   // Show our image inside it.
    imshow( "colorimage", color );                   // Show our image inside it.
    imshow( "coloradaptedimage", coloradapted );                   // Show our image inside it.
    waitKey(0);

    ///____________________OPDRACHT 1.1
    //opLees afbeelding imageColor.png en segmenteer de skin pixels
    //Gebruik hiervoor een filter uit de literatuur in het RGB domein: (RED>95) && (GREEN>40) && (BLUE>20) && ((max(RED,max(GREEN,BLUE)) - min(RED,min(GREEN,BLUE)))>15) && (abs(RED-GREEN)>15) && (RED>GREEN) && (RED>BLUE);
    //Visualiseer naast het masker (binaire pixels die tot huid behoren) dat je bekomt ook de resulterende pixelwaarden.
    ///____________________DEEL 1. HUIDSKLEUR SEGMENTATIE
    ///____________________stap1: OPSPLITSEN IN KLEUREN_______________
    vector<Mat> kanalen;        //vector aanmaken met van 3 kannalen
    split(color, kanalen);      //beeld opsplitsen en de kanalen opslaan in de vector
    ///____________________stap2: ELKE PIXEL TESTEN OP HUIDSKLEUR (mbv de 3 kanalen)
    Mat mask_1 = Mat::zeros(color.rows, color.cols, CV_8UC1);       //nieuw maskker aanmaken van juiste grote
    Mat mask_2 = mask_1.clone();         // repeat(kopier nieuw masker)
    Mat B = kanalen[0];                  // Blauw kanaal
    Mat G = kanalen[1];                  // Groen kanaal
    Mat R = kanalen[2];                  // Rood kanaal

    for(int i=0; i<color.rows; i++){        // Methode 1: hantmatig gaaan testen of de pixewaarde in het interval
        for(int j=0; j<color.cols; j++){    //            ligt van nodig voor een huidskleur
            if((R.at<uchar>(i,j)>95) && (G.at<uchar>(i,j)>40) &&
            (B.at<uchar>(i,j)>20) && ((max(R.at<uchar>(i,j),max(G.at<uchar>(i,j),B.at<uchar>(i,j))) - min(R.at<uchar>(i,j),min(G.at<uchar>(i,j),B.at<uchar>(i,j))))>15) &&
            (abs(R.at<uchar>(i,j)-G.at<uchar>(i,j))>15) && (R.at<uchar>(i,j)>G.at<uchar>(i,j)) && (R.at<uchar>(i,j)>B.at<uchar>(i,j))){
                mask_1.at<uchar>(i,j) = 255;             //indien in het interval set masker pixelwaarde op 255
            }
        }
    }

    mask_2 = (R>95) & (G>40) & (B>20) & ((max(R,max(G,B)) - min(R,min(G,B)))>15) & (abs(R-G)>15) & (R>G) & (R>B); // 2de methode: efficiente manier

    mask_2 = mask_2 * 255; // 0-1 masker aanpassen naar 0-255
    imshow("huidskleur masker methode 1", mask_1); //tonen van resutaat(masker) van methode 1
    waitKey(0);
    imshow("huidskleur masker methode 2", mask_2);  // tonen van resutaat(masker) van methode 2
    waitKey(0);
    ///____________________stap3: De gemaskerde terug kleuren samenvoegen

    Mat finaal(color.rows, color.cols, CV_8UC3);  // nieuwe afbeelding om de de huidskleurpixels weer te geven
    Mat blauwe_pixels = B & mask_2; //blauw kannaal filteren met masker
    Mat groene_pixels = G & mask_2; //groen kannaal filteren met masker
    Mat rode_pixels = R & mask_2;   //rood kannaal filteren met masker
    vector<Mat> channels_mix;   // nieuwe vector om kleuren terug samen te voegen
    channels_mix.push_back(blauwe_pixels); // voeg blauw toe
    channels_mix.push_back(groene_pixels); // voeg groen toe
    channels_mix.push_back(rode_pixels); // voeg rood toe
    merge(channels_mix, finaal); //mix de kanalen
    imshow("huid", finaal); // toon resutlaat
    waitKey(0);
    ///____________________DEEL 2. THRESHOLDING
    Mat bimodal_gray;       // nieuwe image om grijswaarde van bimodal afbeelding op te slaan
    cvtColor(bimodal, bimodal_gray, COLOR_BGR2GRAY);    //bimodal afbeelding omzettten naar grijswaarde
    Mat thresholdImg;       // nieuwe image om gethresholde afbeelding in te bewaren
    threshold(bimodal_gray, thresholdImg, 0, 255, THRESH_BINARY | THRESH_OTSU);  //grijswaarde afbeelding thresholden met otsu methode
    imshow("otsu methode (not equalized)", thresholdImg); // toon resultaat
    waitKey(0);

    Mat bimodal_gelijk;
    equalizeHist(bimodal_gray, bimodal_gelijk);
    imshow("equalized van grijswaarde bimodal", bimodal_gelijk);
    waitKey(0);
    threshold(bimodal_gelijk, thresholdImg, 0, 255, THRESH_BINARY | THRESH_OTSU);  //grijswaarde afbeelding thresholden met otsu methode
    imshow("otsu methode (equalized)", thresholdImg); // toon resultaat
    waitKey(0);
    /// --->  equalize heeft geen verbetering in het resultaat

    Mat bimodal_CLAHE;

    Ptr<CLAHE> clahe_pointer = createCLAHE();
    clahe_pointer->setTilesGridSize(Size(15,15));
    clahe_pointer->setClipLimit(1);
    clahe_pointer->apply(bimodal_gray.clone(), bimodal_CLAHE);
    threshold(bimodal_CLAHE, thresholdImg, 0, 255, THRESH_BINARY | THRESH_OTSU);  //grijswaarde afbeelding thresholden met otsu methode
    imshow("CLAHE ticket grayscale", thresholdImg);
    waitKey(0);
    /// ---> ClAHE geeft al een beter resultaat maar is nog niet perfect



    ///____________________OPDRACHT 1.2
    ///____________________stap1: OPSPLITSEN IN KLEUREN_______________
    vector<Mat> kanalen_2;        //vector aanmaken met van 3 kannalen
    split(coloradapted, kanalen_2);      //beeld opsplitsen en de kanalen opslaan in de vector

    ///____________________stap2: ELKE PIXEL TESTEN OP HUIDSKLEUR (mbv de 3 kanalen)
    Mat masker = Mat::zeros(color.rows, color.cols, CV_8UC1);       //nieuw maskker aanmaken van juiste grote
    Mat B_2 = kanalen_2[0];                  // Blauw kanaal
    Mat G_2 = kanalen_2[1];                  // Groen kanaal
    Mat R_2 = kanalen_2[2];                  // Rood kanaal
    masker = (R_2>95) & (G_2>40) & (B_2>20) & ((max(R_2,max(G_2,B_2)) - min(R_2,min(G_2,B_2)))>15) & (abs(R_2-G_2)>15) & (R_2>G_2) & (R_2>B_2); // 2de methode: efficiente manier

    ///____________________stap3: EROSIE EN DILATIE VOOR PIXELFOUTEN WEG TE WERKEN____________
    erode(masker, masker, Mat(), Point(-1, -1), 2);
    dilate(masker, masker, Mat(), Point(-1, -1), 2);

    imshow("masker geërodeerd en gedilateerd (pixels)", masker);
    waitKey(0);
    ///____________________stap4: EROSIE EN DILATIE VOOR HAND TERUG BIJ ARM TE VOEGEN____________
    dilate(masker, masker, Mat(), Point(-1, -1), 5);
    erode(masker, masker, Mat(), Point(-1, -1), 5);

    imshow("masker geërodeerd en gedilateerd (hand arm)", masker);
    waitKey(0);
    ///____________________stap5: HET VINDEN VAN CONTOUREN EN HULLS________________________
    vector<vector<Point>> contouren;
    findContours(masker.clone(), contouren, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    vector<vector<Point>> hullen;
    for (size_t i = 0; i < contouren.size(); i++){
        vector<Point> hull;
        convexHull(contouren[i], hull);
        hullen.push_back(hull);
    }

    drawContours(masker, hullen, -1, 255, -1);
    imshow("masker met hullen", masker);
    waitKey(0);
    ///___________________stap6: TERUG SAMENVOEGEN VAN KLEUREN NA FILTEREN MET MASKER_______
    Mat eindresultaat(coloradapted.rows, coloradapted.cols, CV_8UC3);  // nieuwe afbeelding om de de huidskleurpixels weer te geven
    Mat blauwe_pixels_2 = B_2 & masker; //blauw kannaal filteren met masker
    Mat groene_pixels_2 = G_2 & masker; //groen kannaal filteren met masker
    Mat rode_pixels_2 = R_2 & masker;   //rood kannaal filteren met masker
    vector<Mat> channels_mix_2;   // nieuwe vector om kleuren terug samen te voegen
    channels_mix_2.push_back(blauwe_pixels_2); // voeg blauw toe
    channels_mix_2.push_back(groene_pixels_2); // voeg groen toe
    channels_mix_2.push_back(rode_pixels_2); // voeg rood toe
    merge(channels_mix_2, eindresultaat); //mix de kanalen
    imshow("eindresultaat", eindresultaat); // toon resutlaat
    waitKey(0);
    return 0;
}
