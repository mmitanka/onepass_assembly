#ifndef SECTION_HEADER_H
#define SECTION_HEADER_H
#pragma once
using namespace std; 
#include <string>
#include <vector>
#include <stdint.h>

#include <iomanip>
#include <iostream>
#include <fstream>
#include "tab_section.h"
#include "tab_symb.h"
#include "tab_rel.h"

class TabSymb;
class TabRellocation;
class TabSection;

typedef struct EntryTabHSectoion{
    string name;
    string type; //if its simb_tab or rel_tab, section_tab bss, text, rodata...
    uint16_t size;
    //uint16_t entry_size; add if needed

    TabSymb *tab_symb_ref;
    TabSection *tab_sec_ref;
    TabRellocation *tab_rel_ref;

    EntryTabHSectoion(string nm, string tp, uint16_t sz) {
        tab_rel_ref=nullptr;
        tab_sec_ref=nullptr;
        tab_symb_ref=nullptr;
        name=nm; type=tp; size=sz;
    }
}EntryTabHSectoion;

class TabSectionHeader {
private:
    static vector<EntryTabHSectoion> header_table;

public: 
    static TabSymb* addNewEntrySymb(string type, string name="");
    static TabRellocation* addNewEntryRel(string type, string name);
    static TabSection* addNewEntrySec(string type, string name);

    static TabSymb* getTabSymb();
    static TabSection* getTabSec(string sec_name);
    static TabRellocation* getTabRel(string rel_name);

    //probably should have some print func
    static void printHeaderTable(); 
};

#endif 