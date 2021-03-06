#include "tab_rel.h"

TabRellocation::TabRellocation(string nm){
    name=nm;
}

void TabRellocation::setNewEntry(EntryTabRel e) {
    tab_rel.push_back(e); 
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