#include "tab_section_header.h"

/*init static members*/
vector<EntryTabHSectoion> TabSectionHeader::header_table = { EntryTabHSectoion("","null",0) };

TabSymb* TabSectionHeader::addNewEntrySymb(string type, string name){
    EntryTabHSectoion ent= EntryTabHSectoion(name, type,0);
    ent.tab_symb_ref= new TabSymb();
    header_table.push_back(ent); 

    return header_table.back().tab_symb_ref;
}

TabRellocation* TabSectionHeader::addNewEntryRel(string type, string name){
    EntryTabHSectoion ent= EntryTabHSectoion(name, type,0);
    ent.tab_rel_ref= new TabRellocation(name); 
    header_table.push_back(ent);

    return header_table.back().tab_rel_ref; 
}

TabSection* TabSectionHeader::addNewEntrySec(string type, string name){
    EntryTabHSectoion ent= EntryTabHSectoion(name, type,0);
    ent.tab_sec_ref= new TabSection(name); 
    header_table.push_back(ent);

    return header_table.back().tab_sec_ref;
}


TabSymb* TabSectionHeader::getTabSymb() {
    for(EntryTabHSectoion e:header_table) {
        if(e.type=="tabsymb")
            return e.tab_symb_ref;
    }
    //if not found create new table
    return addNewEntrySymb("tabsymb","tabsymb");
     
}

TabSection* TabSectionHeader::getTabSec(string sec_name){
    for(EntryTabHSectoion e:header_table) {
        if(e.name==sec_name) 
            return e.tab_sec_ref;
    }
    //if not found create new table
    
    return addNewEntrySec(sec_name,sec_name);
}

TabRellocation* TabSectionHeader::getTabRel(string rel_name){
    string name= ".rel";
    name.append(rel_name);

    for(EntryTabHSectoion e:header_table) {
        if(e.name==name)
            return e.tab_rel_ref;
    }
    //if not found create new table
    return addNewEntryRel(".rel",name);
}


void TabSectionHeader::printHeaderTable() {
    setSizes();
    cout<< "Header section table: "<<endl;
    cout <<"name | " << "type | " << "size | "<<endl; 

    for(EntryTabHSectoion e:header_table){
        cout<<e.name<< " | " << e.type << " | " << e.size << " | " <<endl;
    }
        cout<<endl;
        cout<<endl;

    for(EntryTabHSectoion e:header_table) {
        if(e.tab_rel_ref) 
            e.tab_rel_ref->print();
        if(e.tab_sec_ref)
            e.tab_sec_ref->print();
        if(e.tab_symb_ref)
            e.tab_symb_ref->print();
        
    }
    cout<<endl;
    cout<<endl;
}

void TabSectionHeader::writeTo(ofstream &out) {
        setSizes();
    out<<setw(20)<< "Header section table: ";
    out<<endl;
    out <<setw(20)<<"name ";
    out <<setw(20)<< "type " ;
    out <<setw(20)<< "size ";
    out <<endl; 

    for(EntryTabHSectoion e:header_table){
       out <<setw(20)<<e.name ;
       out <<setw(20)<< e.type ;
        out <<setw(20)<< e.size ;
        out <<endl;
    }
        out<<endl;
        out<<endl;

    for(EntryTabHSectoion e:header_table) {
        if(e.tab_rel_ref) 
            e.tab_rel_ref->writeTo(out);
        if(e.tab_sec_ref)
            e.tab_sec_ref->writeTo(out);
        if(e.tab_symb_ref)
            e.tab_symb_ref->writeTo(out);
        
    }
    out<<endl;
    out<<endl;
}


void TabSectionHeader::setSizes() {
    for(EntryTabHSectoion &e:header_table) {
        if(e.tab_rel_ref) 
         e.size=  static_cast<uint16_t>(e.tab_rel_ref->tab_rel.size()*sizeof(EntryTabRel));
        if(e.tab_sec_ref)
          e.size=  static_cast<uint16_t>(e.tab_sec_ref->getTab().size());
        if(e.tab_symb_ref)
          e.size= static_cast<uint16_t>( e.tab_symb_ref->getTab().size()*sizeof(Entry));
    }
}


