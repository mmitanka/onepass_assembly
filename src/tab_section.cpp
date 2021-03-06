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
    cout<<"proba generisem 2 bajta"<<endl;
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
   unsigned long index=findIndex(off);
    if(off<=tab_section.size()-2){
        int8_t w=data & 0x00FF;
        tab_section[index]=w;
        //it++;
        w=(data & 0xFF00) >> 8;
        tab_section[index+1]=w;
    }



}
unsigned long TabSection::findIndex(uint16_t off){
    unsigned long index=0;
   for(unsigned long i=0; i<tab_section.size();++i)
        if(i==off) {
            index=i;
            break;
        }
    
    return index;
}
void TabSection::addWord(uint16_t off, int16_t data) {
   vector<int8_t>::iterator it=tab_section.begin()+off;
   unsigned long index=findIndex(off);
    if(off<=tab_section.size()-2){
        //it++;
        int16_t ex=*(it+1);
        //it--;
        ex<<=8;
        ex |= *it;
        ex +=data; 
        int8_t w=ex & 0x00FF;
        tab_section[index]=w;
        //it++;
        w=(ex & 0xFF00) >> 8;
        tab_section[index+1]=w;
       
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

void TabSection::writeTo(ofstream& out) {
    string name=getSectionName();
    if(name!=".bss") {
        out<<endl;
        out<<setw(10)<<"Section table : "<<name <<endl;
        for(int8_t e:tab_section) {
        
        out<<setw(2) << toHex(e)<< " ";
        }
    }
    out<<endl;
}

string TabSection::toHex(const int8_t b) {
	stringstream s;
	uint8_t by = b;
	s << hex << ((by >> 4) & 0xF);
	s << hex << (by & 0xF);
	return s.str();
}