#ifndef UNRESOLVED_H
#define UNRESOLVED_H
#pragma once

#include <vector>
#include <stdint.h>
#include <string> 
#include <iomanip>
#include <iostream>
#include <fstream>
#include "tab_symb.h"
#include "token.h"
#include "expression.h"
#include "tab_section_header.h"

using namespace std;
class Token;
class TabSymb;
class Expression; 

typedef struct EntryUnresolved {
    string symbol; //from equ
    bool resolved;
    vector<Token> expression; //symbol's value, how to calculate symbol from equ dir 
    
    EntryUnresolved() {};
    EntryUnresolved(string s, vector<Token> ex, bool r=false) {
        symbol=s; expression=ex; resolved=r; 
    };
}EntryUnresolved; 

class TabSectionHeader;

class TabUnresolved{
private:
    vector<EntryUnresolved> tab_unresolved; 
    TabSymb *tab_symb;
    static Expression exp;
public:
    TabUnresolved() ;
    void setTabSymb(TabSymb *tab);
    /*unfinished*/
    void resolveSymbols();//iterative function peeks to tab_symb looking for new symbols 
    void insertUnresolvedSymbol(string simb_name, vector<Token> expression);

    void print();

};
#endif 
