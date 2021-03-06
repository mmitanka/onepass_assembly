#include "tab_unresolved.h"
#include "tab_symb.h"
#include <stdint.h> 
#include "token.h"


Expression TabUnresolved::exp;

TabUnresolved::TabUnresolved() {
        TabSymb *tab1=nullptr;
        tab1=TabSectionHeader::getTabSymb();
        setTabSymb(tab1);
    }

void TabUnresolved::insertUnresolvedSymbol(string simb_name, vector<Token> expression) {
    bool have=false; 
    if(tab_unresolved.size()!=0) {
        for(EntryUnresolved ent:tab_unresolved) {
            if(simb_name==ent.symbol)
                have=true; 
        }
    }
    if(!have) {
        EntryUnresolved ent=EntryUnresolved(simb_name,expression); 
        tab_unresolved.push_back(ent);
    }
 
}

void TabUnresolved::setTabSymb(TabSymb *tab){
    tab_symb=tab;
}

void TabUnresolved::resolveSymbols() {
    int to_resolve=tab_unresolved.size();
    int to_resolve_control=tab_unresolved.size();
    while(to_resolve>0) {
        bool resolved=false;
        for(auto &ent:tab_unresolved) {
            if(!ent.resolved){

            //func with wich i will calculte val of simbol from entry
            //if it is calculated proceed with chanching symbols data in main tab_symb
            unsigned long sec;
                if(exp.indexClass(ent.expression,sec)) {
                    uint16_t value; 
                    bool proceed=false;
                    value= exp.calculateExp(ent.expression, ent.symbol,proceed); 
                    if(proceed) {
                        //need to insert ent symbol in tab symb 
                        //tab_symb->redefineValue(ent.symbol, value);
                        if(sec!=-1) {
                            tab_symb->setNewEntry(ent.symbol,".equ",sec,0,value,true);
                        }else {
                            tab_symb->setNewEntry("ABS",".section",tab_symb->tab_symb.size(),0, 0, true);
                            tab_symb->setNewEntry(ent.symbol,".equ",tab_symb->tab_symb.size()-1,0,value, true);
                        }
                        resolved=true;
                        to_resolve--;
                        ent.resolved=true;
                    }
                }

            }

        }
        if(!resolved) {
            //u ovoj rundi nismo nista razresili, simboli zavise jedan d drugog 
            throw "Symbols defined in loop";
           // break; //ili trow videcemo kako funkcionise
        }
    }

}


void TabUnresolved::print() {
    cout <<endl << "Table of unresolved symbols from .equ:" <<endl;
    cout <<"Symbol | " <<".equ expression | " <<endl;

    for(auto e:tab_unresolved) {
        cout <<e.symbol <<" | ";
        vector<Token> v=e.expression;
        unsigned long sz=v.size();
        for(unsigned long i=0;i<sz;i++) {  
            Token t=v.front();
            cout<<t.getVal();
            vector<Token>::iterator it= v.begin();
            v.erase(it);
        }
        cout<< " | " <<endl;
    }
}