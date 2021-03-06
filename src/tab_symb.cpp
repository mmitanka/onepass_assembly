#include "tab_symb.h"
#include "token.h"
#include <vector>


TabSymb::TabSymb() {
    ForwardTable fl=ForwardTable();
    und_entry=Entry(UND_NB,"UND", 0, Symbol_type::LOCAL,0,0,fl, true); 
    setNewEntry(und_entry.name,".und", und_entry.section,und_entry.size,und_entry.value,und_entry.defined); 
}

vector<Entry> TabSymb::getTab(){
    return tab_symb;
}


void TabSymb::setDefined(string name, bool st) {

    for(Entry& ent:tab_symb)
        if(ent.name==name){
            ent.defined=st;
            break;
        }
}

bool TabSymb::isDefined(string name){
    bool ret=false; 
    for(Entry ent: tab_symb)
        if(ent.name==name){
            ret=ent.defined;
            break;
        }
    return ret; 
}

Entry TabSymb::getEntry(string name) {


    Entry e;
    for(Entry ent: tab_symb)
        if(ent.name==name){
            e=ent;
            break;
        }


    return e; 
}

bool TabSymb::alreadyExists(string name) {
    bool ret=false; 
    if(tab_symb.size()!=0) {
        for(Entry ent: tab_symb){
            if(ent.name==name){
                ret= true; 
                break;
            }
        }
    }
    return ret;
}

/*poziva se kada je token simbol ili labela+moze i za ako se ubacuje simbol sekcije
regulisacu to sa ifom u asembleru; razlika samo sta cu slati kao entry parametre */   

void TabSymb::setNewEntry(string name, string type, unsigned long sec, uint16_t sz,uint16_t loc_cnt, bool def) {

    Symbol_type novitip;
    if(type =="label"||type==".und"||type==".section" || type==".equ")
        novitip =Symbol_type::LOCAL;
    else if(type==".global" )
        novitip = Symbol_type::GLOBAL;
    else if ( type==".extern")
        novitip= Symbol_type::EXTERN;


    if(!alreadyExists(name)) {

        cout<<"symbol "<<name<<" ne postoji!"<<endl;

        ForwardTable flink= ForwardTable();
        int newNB=tab_symb.size(); //cause of UND is nult section
        Entry newE=Entry(newNB, name,loc_cnt,novitip,sec,sz,flink,def); 
        tab_symb.push_back(newE);

    }else {

        Entry ent=getEntry(name);

        if(ent.type==Symbol_type::GLOBAL){
            novitip=Symbol_type::GLOBAL;
        
        }
        redefineSymbol(name, loc_cnt, novitip, sec,sz, def,type); 

    }

}

unsigned long TabSymb::getNbOfEntry() {
    return tab_symb.size();
}

void TabSymb::setNewValues(string name,uint16_t val,Symbol_type t, unsigned long sec,uint16_t sz) {

    for(Entry &ent:tab_symb) {
        if(ent.name==name) {
            ent.value=val;
            ent.section=sec;
            ent.size=sz;
            ent.type = (t==Symbol_type::GLOBAL) ? Symbol_type::GLOBAL: Symbol_type::LOCAL; 


        }

    }
}

/*ako vec postoji u tabeli simbola izmenimo to sto treba, paziti defined da se setuje na true*/ 
void TabSymb::redefineSymbol(string name,uint16_t val,Symbol_type t, unsigned long sec,uint16_t sz, bool def, string sect) {
    Entry ent=getEntry(name);
    string str;

    if(ent.name=="UND"){
        cout<<"ne diramo nista sa undom"<<endl;
    }
    else if(!isDefined(ent.name) ||(sect!="" && (sect==".section" || sect=="label"))) {
        cout<<"usli smo da redefinisemo simbol"<<name<<endl;
        if(ent.type==Symbol_type::EXTERN) {
            //greska ne sme da se stavi define za extern, to radi linker 
            str="Forbiden defining .extern symbol";
            throw str;
        }
       /* ent.value=val;
        ent.section=sec; 
        cout << ent.section << " nova vrednost"<<endl;
        ent.size=sz;
        ent.type = (t==Symbol_type::GLOBAL) ? Symbol_type::GLOBAL: Symbol_type::LOCAL;*/

        setNewValues(name,val,t,sec,sz);

        setDefined(ent.name, def); // isDefined(e.name); 

    }else {
        //ne moze redef nekog simb koji je vec def 
        str="This symbol is already defined";
        throw str; 
    }
}


void TabSymb::redefineValue(string name, uint16_t val){
    if(alreadyExists(name)) {
        //Entry e=getEntry(name); 
        for(Entry& e:tab_symb) {
            e.value=val;

        }
        setDefined(name,true);//when value is set then symbol is defined
    }
}

void TabSymb::setFlink(string name, uint16_t patch, string sect, Rel_type type, bool rel) {

    for(Entry &e:tab_symb) {
       if(e.name==name) {
           cout<<"nasli simbol u fliku"<<endl;
           ForwardTable fl=ForwardTable(patch,sect,type,rel);
           e.flink.push_back(fl);
           cout<<"novi patch je"<<e.flink.back().patch<<endl;
       }
    }
    cout<<"provera da li se sacuvalo : ";
    for(Entry e:tab_symb) 
    {
        if(e.name==name)
           cout<<"sacuvani patch je"<<e.flink.back().patch<<endl; 
    }
}

unsigned long TabSymb::findSectionForSpecificSymbol(string simb_name){
    for(Entry e:tab_symb) {
        if(e.name==simb_name)
            return e.section;
    }
    return -1;   

}


void TabSymb::setSectionSize(unsigned long section, uint16_t loc_cnt) {
    if(section <0 ) return; 

    if(tab_symb.size()!=0) {
       for(Entry &e:tab_symb) {
           if(e.nb==section){
               e.size=loc_cnt;
           }
       }
    } 
}

string TabSymb::findSectionName(unsigned long sec) {
    for(Entry e:tab_symb)
        if(e.nb==sec) {
            return e.name; 
        }
    return "UND"; 
}

unsigned long TabSymb::findSectionNB(string name) {
    
    for(Entry e:tab_symb) {
        if(e.name==name) {
            return e.nb;
        }

    }
    return und_entry.nb;
}

/*unchecked*/
uint16_t TabSymb::createRelTable(string name,string section, uint16_t loc_cnt, Rel_type type) {
    TabRellocation *tab_rel=nullptr;
    tab_rel=TabSectionHeader::getTabRel(section); 
    cout<<"prosledili smo loc count : "<<loc_cnt<<endl;

    if(alreadyExists(name)) {
        Entry e=getEntry(name);
        if(!isDefined(name) && e.type!=Symbol_type::EXTERN) {
            cout<<"usli ovde nije extern"<<endl;
            setFlink(name,loc_cnt,section,type,true);

            return 0;
        }
        if(e.type==Symbol_type::EXTERN) {
            cout<<"extern je"<<endl;
            tab_rel->setNewEntry(EntryTabRel(loc_cnt,type,e.nb));
            cout<<"trebalo bi da se izadje iz create tab rel"<<endl;

            return 0; 

        } else if(e.type==Symbol_type::LOCAL){
            if(type==Rel_type::R_16) {
                             
                tab_rel->setNewEntry(EntryTabRel(loc_cnt,type,e.section));
                return e.value;   
            }else {
                if(e.section!=findSectionNB(section)) {
                     tab_rel->setNewEntry(EntryTabRel(loc_cnt,type,e.section));

                }
                return e.value;
            }
        } else {//global
            if(type==Rel_type::R_16) {
                
                tab_rel->setNewEntry(EntryTabRel(loc_cnt,type,e.nb));
                return 0;
            }else {
                if(e.section!=findSectionNB(section)) {
                    tab_rel->setNewEntry(EntryTabRel(loc_cnt,type,e.nb));
                    return 0;

                }else{
                    return e.value;

                }
            }
        }
    }else {//symbol not found 
        ForwardTable fl=ForwardTable(loc_cnt,section,type,true);
        unsigned long sz=tab_symb.size();
        tab_symb.push_back(Entry(sz,name,0,Symbol_type::LOCAL,und_entry.nb,0,fl,false));
        return 0;
    }

}

/*unchecked*/
void TabSymb::backpatching() {
    
    for(Entry &e:tab_symb) {
        if(e.flink.size()!=0) {
            if(isDefined(e.name)) {
                for(ForwardTable &fwt:e.flink) {
                    if(e.type==Symbol_type::GLOBAL) {

                        if(fwt.type==Rel_type::R_16) {
                            if(fwt.relocation) {
                                TabSectionHeader::getTabRel(fwt.section)->setNewEntry(EntryTabRel(fwt.patch,fwt.type,e.nb));
                            }
                        }else if(fwt.type==Rel_type::R_PC16) {
                            if(e.section!=findSectionNB(fwt.section)) {
                                if(fwt.relocation) {
                                    TabSectionHeader::getTabRel(fwt.section)->setNewEntry(EntryTabRel(fwt.patch,fwt.type,e.nb));
                                }
                            }else {
                                TabSectionHeader::getTabSec(fwt.section)->addWord(fwt.patch,e.value-fwt.patch);
                            }
                        }

                    }else if(e.type==Symbol_type::LOCAL) {

                        if(fwt.relocation) {

                            if(fwt.type==Rel_type::R_16) {
                                TabSectionHeader::getTabRel(fwt.section)->setNewEntry(EntryTabRel(fwt.patch,fwt.type,e.section));
                                TabSectionHeader::getTabSec(fwt.section)->addWord(fwt.patch,e.value);

                            }else if(fwt.type==Rel_type::R_PC16) {
                                if(e.section!=findSectionNB(fwt.section)) {
                                    TabSectionHeader::getTabSec(fwt.section)->addWord(fwt.patch,e.value);
                                    TabSectionHeader::getTabRel(fwt.section)->setNewEntry(EntryTabRel(fwt.patch,fwt.type,e.section));
                                
                                }else {
                                    TabSectionHeader::getTabSec(fwt.section)->addWord(fwt.patch,e.value-fwt.patch);
                                }
                            }
                        }
                    }
                }
            }else { //not defined
                if(e.type==Symbol_type::EXTERN) {
                    for(ForwardTable &fwt:e.flink) {
                        if(fwt.relocation) {
                            TabSectionHeader::getTabRel(fwt.section)->setNewEntry(EntryTabRel(fwt.patch,fwt.type,e.nb));
                        }
                    }
                } else {
                    string str="Symbol is not defined";
                    throw str;
                }


            }

        }
    }
}

void TabSymb::print() {
    cout<< "Symbol Table: "<<endl;
    cout << "index \t|\t " <<"name \t|\t "<<"value \t|\t "<< "section\t|\t " << "type \t|\t " << "size \t|\t "<<"defined \t|\t " <<"flink \t|\t "<<endl; 

    for(auto e:tab_symb) {
        cout << e.nb << " \t|\t " << e.name << " \t|\t " <<e.value << " \t|\t ";
        cout<< e.section << " \t|\t " ;
        if(e.type==Symbol_type::EXTERN)
            cout << "extern"<<" \t|\t " ;
        else if(e.type==Symbol_type::GLOBAL)
            cout << "global"<<" \t|\t " ;
        else if(e.type==Symbol_type::LOCAL)
            cout << "local"<<" \t|\t " ;

        cout<< " \t|\t " << e.size << " \t|\t " ;

        if(e.defined)
            cout<<"true"<< " \t|\t ";
        else 
            cout<<"false" <<" \t|\t ";
         
        
        for(ForwardTable fwt:e.flink){
            cout<<fwt.patch <<"  ";
        
        }
        cout<< " \t|\t " <<endl;

    }
    cout<<endl;
    cout<<endl;
}

void TabSymb::writeTo(ofstream& out) {
    out<<endl <<"Symbol Table: ";
    out<<endl;
    out<<std::left;
    out << setw(10)<<"index " ;
    out<< setw(10)<<"name  ";
    out<< setw(10)<<"value  ";
    out<< setw(10)<< "section " ;
    out<< setw(10)<< "type  " ;
    out<< setw(10)<< "size ";
    out<< setw(10)<<"defined  " ;
    out<< setw(10)<<"flink  "; 
    out<<endl;
  for(auto e:tab_symb) {
    out<< setw(10) << e.nb ; 
    out<< setw(10)<< e.name ;
    out << setw(10) <<e.value;
        out<< setw(10)<< e.section ;
        if(e.type==Symbol_type::EXTERN)
            out << setw(10)<< "extern" ;
        else if(e.type==Symbol_type::GLOBAL)
            out << setw(10)<< "global";
        else if(e.type==Symbol_type::LOCAL)
            out << setw(10)<< "local";

        out<< setw(10)<< e.size ;

        if(e.defined)
            out<< setw(10)<<"true";
        else 
            out<< setw(10)<<"false" ;
         
        
        for(ForwardTable fwt:e.flink){
            out<<fwt.patch <<"  ";
        
        }
        out<< endl <<endl;

    }
}