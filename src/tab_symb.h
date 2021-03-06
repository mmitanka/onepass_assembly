#ifndef TAB_SYMB_H
#define TAB_SYMB_H

#pragma once
#include <iostream>
#include <vector>
#include <stdint.h>
#include "token.h"
#include "tab_unresolved.h"
#include "tab_rel.h"
#include <iomanip>
#include <iostream>
#include <fstream>

using namespace std; 

const int UND_NB=0;

typedef enum Symbol_type {
      GLOBAL=1, LOCAL, EXTERN
}Symbol_type;

typedef struct ForwardTable{
    uint16_t patch;
    Rel_type type;
    string section;
    bool relocation; 
    ForwardTable() {
        section="";
    }
    ForwardTable(uint16_t p, string se, Rel_type t, bool rel=false) {
        patch=p; section=se; type=t; relocation=rel;
       
    }
}ForwardTable;

 typedef struct Entry{
    int nb;
    string name; 
    uint16_t value;
    Symbol_type type; 
    unsigned long section; 
    bool defined;
    uint16_t size;
    vector<ForwardTable> flink;  //vidi ovo mozda da se doradi 
    Entry() {}

    Entry(int rb, string n, uint16_t val,Symbol_type t, unsigned long sec,uint16_t sz,  ForwardTable fl=ForwardTable(), bool def=false) {
        nb=rb; name=n; 
        value=val; //equals to line in mach code where is simbol 
        type=t; 
        section=sec; 
        defined=def; //initial is supposed to be like this
        if(fl.section!="")
            flink.push_back(fl);
        size=sz; 
    }

} Entry; 

class TabSymb
{
   
private:
    vector<Entry> tab_symb;
    Entry und_entry;
public:
    TabSymb();
    ~TabSymb(){

    };
    void setNewEntry(string name, string type, unsigned long sec,uint16_t sz, uint16_t loc_cnt, bool def);
    
    bool isDefined(string name);
    void setDefined(string name, bool state);
    Entry getEntry(string name); 
    unsigned long getNbOfEntry();

    //nije impl
    void backpatching(); 

    void writeToTextFile(ofstream& out);
    void print();

    void setFlink(string name, uint16_t patch, string sect, Rel_type type, bool rel); 
    //za ispitivanje da li se simbol vec nalazi u tabeli 
    bool alreadyExists(string name);
    void redefineSymbol(string name,uint16_t val,Symbol_type t, unsigned long sec, uint16_t sz,  bool def, string sect="");

    void setSectionSize(unsigned long section, uint16_t);
    void redefineValue(string name,uint16_t val); //set val from equ dir and sets defined to true

    string findSectionName(unsigned long sec);
    unsigned long findSectionNB(string name);
    unsigned long findSectionForSpecificSymbol(string simb_name);
    //nije impl
    uint16_t createRelTable(string name,string section, uint16_t loc_cnt, Rel_type type);

   
};

#endif 
