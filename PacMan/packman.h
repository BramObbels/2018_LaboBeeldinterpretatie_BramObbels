#ifndef MAIN_H
#define MAIN_H


class Texttab{
        string spaces = "";
        vector<string> titles;

    public:
        void updateSpaces();
        void addTitle(string);
        void removeTitle();
        string getFull();
        string getShort();
};
void Texttab::updateSpaces(){
    spaces = "";
    for(int i = 0; i < titles.size()-1; i++){
        for(int l = 0; l < titles[i].length(); l++){
            spaces.append(" ");
        }
        spaces.append(" | ");
    }
}
void Texttab::addTitle(string t){
    titles.push_back(t);
    updateSpaces();
}
void Texttab::removeTitle(){
    cout << "" << endl;
    titles.pop_back();
    updateSpaces();
}

string Texttab::getFull(){
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
string Texttab::getShort(){
    string txt = spaces;
    txt.append(titles[titles.size()-1]);
    txt.append(" : ");
    return txt;
}

#endif // MAIN_H
