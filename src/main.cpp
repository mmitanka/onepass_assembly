
#include <iostream>
#include "assembler.h"
#include <exception>
using namespace std;

int main(int argc, char** argv){

    int ret=-1;
    cout<<"krece"<<endl;
    
    try {
        ret= Assembler::readInputFile(argv[1], argv[2]);
    }catch(string str) {
        cout<< str<<endl;

    }catch(const char* c) {
        cout <<c<<endl;
    }
    catch(exception &e) {
        cout<<"Unkown error "<<e.what()<<endl;
    }
    return ret; 

}