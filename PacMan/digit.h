#ifndef DIGIT_H_INCLUDED
#define DIGIT_H_INCLUDED

#include <vector>

using namespace std;

struct digit{
    int x_positie = 0;
    int waarde = 0;
};

class digit_number{
        vector<digit> digits;
    public:
        void addDigit(digit);
        void bubble();
        int getInt();
};

void digit_number::addDigit(digit d){
    //cout << "digit toegevoegd aan digit_getal" << endl;
    digits.push_back(d);
}

void digit_number::bubble(){ //bubblesort algorthme om getallen te sorteren volgens hun positie
    bool swapp = true;
    if(digits.size()>=2){
        while(swapp){
            swapp = false;
            for (size_t i = 0; i < digits.size()-1; i++) {
                if (digits[i].x_positie>digits[i+1].x_positie ){
                    swap(digits[i], digits[i+1]);
                    swapp = true;
                }
            }
        }
    }
}

int digit_number::getInt(){
    bubble();
    int getal = 0;
    string getalstring = "";
    for(int i = 0; i < digits.size(); i++){
        getalstring.append(to_string(digits[i].waarde));
    }
    if (getalstring.size() != 0){
        getal = stoi(getalstring);
    }else{
        getal = 0;
    }
    return getal;
}

#endif // DIGIT_H_INCLUDED
