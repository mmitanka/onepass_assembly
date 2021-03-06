#include "tab_rel.h"

TabRellocation::TabRellocation(string nm){
    name=nm;
}

void TabRellocation::setNewEntry(EntryTabRel e) {
    cout<<"usla u set entry u tab rel"<<endl;
    tab_rel.push_back(e); 
    cout<<"izlazim iz set entry u tab rel"<<endl;

}

uint16_t TabRellocation::calculateSize() {
    return static_cast<uint16_t>(tab_rel.size()*sizeof(EntryTabRel));

}

void TabRellocation::print() {
    cout<<"Relocation table: "<<name<<endl;
    cout<< "offset | " << "type | " << "nb" <<endl;
    for(auto e:tab_rel) {
        cout<<e.offset <<" | "<<e.type << " | "<<e.nb<< " | " <<endl;
    }
}
void TabRellocation::writeTo(ofstream& out){
        out<<endl;
        out<<setw(10)<<"Relocation table: "<<name<<endl;
        out<<setw(10)<< "offset  " ;
        out<<setw(10)<< "type  " ;
        out<<setw(10)<< "nb" <<endl;

        for(auto e:tab_rel) {
        out<<setw(10)<<e.offset;
        if(e.type==Rel_type::R_16)
            out<<setw(10)<<"R_16";
        else 
            out<<setw(10)<<"R_PC16";

        out<<setw(10)<<e.nb; 
        out <<endl;
    }
    }
