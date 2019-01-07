
#ifndef TITLETABS_H
#define TITLETABS_H
#include <string>
#include <vector>

using namespace std;

class TitleTabs{
        string spaces = "";
        vector<string> titles;

    public:
        void updateSpaces();
        void addTitle(string);
        void removeTitle();
        string getFull();
        string getShort();
};
void TitleTabs::updateSpaces(){
    spaces = "";
    for(int i = 0; i < titles.size()-1; i++){
        for(int l = 0; l < titles[i].length(); l++){
            spaces.append(" ");
        }
        spaces.append(" | ");
    }
}
void TitleTabs::addTitle(string t){
    titles.push_back(t);
    updateSpaces();
}
void TitleTabs::removeTitle(){
    cout << "" << endl;
    titles.pop_back();
    updateSpaces();
}

string TitleTabs::getFull(){
    string txt = "";
    for(int i = 0; i < titles.size(); i++){
        txt.append(titles[i]);
        if(i == titles.size()-1){
            txt.append(" : ");
        }else{
            txt.append(" | ");
        }
    }
    return txt;
}

string TitleTabs::getShort(){
    string txt = spaces;
    txt.append(titles[titles.size()-1]);
    txt.append(" : ");
    return txt;
}


#endif // TITLETABS_H
