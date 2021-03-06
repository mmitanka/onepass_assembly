#ifndef REL_H
#define REL_H
#pragma once
using namespace std; 
#include <stdint.h> 
#include <string> 
#include <vector>
#include <iomanip>
#include <iostream>
#include <fstream>

enum Rel_type {
    R_16,
    R_PC16
};

typedef struct EntryTabRel {
    uint16_t offset; 
    Rel_type type; 
    unsigned long nb; //where linker to find this 

    EntryTabRel(uint16_t off, Rel_type tp, unsigned long n) {
        offset=off; type=tp; nb=n; 
    }
}EntryTabRel;

class TabRellocation{
private:
    string name; 
    
public:
    vector<EntryTabRel> tab_rel; 
    TabRellocation() {}
    TabRellocation(string nm);

    void setNewEntry(EntryTabRel e);

    uint16_t calculateSize();
    //some functions to write or print this table->later
    void writeTo(ofstream& out);
    void print();
};

#endif 