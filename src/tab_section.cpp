#include "tab_section.h"
#include "tab_section_header.h"
#include "tab_symb.h"
#include <bitset>

TabSection::TabSection(string name) {
    section=TabSectionHeader::getTabSymb()->findSectionNB(name);
}

void TabSection::generateByte(int8_t b) {
    tab_section.push_back(b); 
}

void TabSection::generateWord(int16_t w) {
    int8_t low= (w & 0x00FF); //clear leading nulls
    tab_section.push_back(low); 
    int8_t high= (w & 0xFF00) >> 8;
    tab_section.push_back(high);
}
string TabSection::getSectionName() {
    /*TabSymb *tab=*/ 
    return TabSectionHeader::getTabSymb()->findSectionName(section);

}

/*unchecked*/
void TabSection::insertWord(uint16_t off, int16_t data) {
    vector<int8_t>::iterator it=tab_section.begin()+off;
    if(off<=tab_section.size()-2){
        int8_t w=data & 0x00FF;
        tab_section.insert(it,w);
        it++;
        w=(data & 0xFF00) >> 8;
        tab_section.insert(it,w);
    }
}
/*unchecked*/
void TabSection::addWord(uint16_t off, int16_t data) {
    vector<int8_t>::iterator it=tab_section.begin()+off;
    if(off<=tab_section.size()-2){
        it++;
        int16_t ex=*it;
        it--;
        ex<<=8;
        ex |= *it;
        ex +=data; 
        int8_t w=ex & 0x00FF;
        tab_section.insert(it,w);
        it++;
        w=(ex & 0xFF00) >> 8;
        tab_section.insert(it,w);
    }

}

void TabSection::print() {
    string name=getSectionName();
    cout<<"Section table : "<<name <<endl;
    
    for(int8_t e:tab_section) {
        bitset<8> b(e);
        cout << b<< "  ";
    }
    cout<<endl;

}