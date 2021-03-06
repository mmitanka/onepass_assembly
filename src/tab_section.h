#ifndef SECTION_H
#define SECTION_H
#pragma once
#include <stdint.h>
#include <vector>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class TabSection {
private:
    unsigned long section; //seq number of section
    vector<int8_t> tab_section; //8 cause of byte 

public: 
    TabSection(string name);
    void generateByte(int8_t b);
    void generateWord(int16_t w);

    //at specific addres
    void insertWord(uint16_t off, int16_t data);
    void addWord(uint16_t off, int16_t data);

    string getSectionName(); 
    void print(); 
    unsigned long findIndex(uint16_t off);
    void writeTo(ofstream &out);
    vector<int8_t> getTab() {
        return tab_section;
    }
    string toHex(const int8_t s);
};



#endif 
