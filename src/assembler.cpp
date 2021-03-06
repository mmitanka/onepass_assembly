#include "assembler.h"
#include <stdint.h>
#include "tab_section_header.h"
#include "tab_symb.h"


vector<vector<string>> Assembler::inputFile;
Token Assembler::tok=Token(Token_type::ASM,"asm");;
TabUnresolved Assembler::tab; 
Expression Assembler::exp;
Instruction Assembler::instr;
Assembler::Assembler() {
   
}

int Assembler::readInputFile(string in_name, string out) {
    ifstream inn;
    ofstream oout;
    string newIn;
    string str;
    //cout<<"pre otvaranja in=a"<<endl;
    inn.open(in_name);

    //cout<<"otvorila fajl"<<endl;
    while(getline(inn,newIn)){
         //cout<<"usla u while"<<endl;
            if(newIn.empty()){
                //cout<<"ne ocitava lepo liniju" <<endl;
                continue;

            }
            string line=tok.eraseComments(newIn);
            //cout<<"pre reshape token" <<endl;

            vector<string> newVector=tok.reshapeToTokens(line);
            //cout<<"posle reshape token" <<endl;

            if(!newVector.empty()) {
                //cout<<"vrceni vektor nije prazan"<<endl;

                inputFile.push_back(newVector);
                //cout<<proba<<"new vector radi"<<endl;
                
                if(newVector.front()==".end") {
                    cout<<"found end"<<endl;
                    break;
                }

            }

    }  
        if(inputFile.back().back()!=".end") {
            str="File must end with .end directive";
            throw str;
        }

    /*}else {
        str="Unable to open input file";
        throw str; 
    }*/
        
    inn.close();
    cout<<"pre worka"<<endl;
    work();
    
    //backpaching();
    TabSymb *tab1=nullptr;
    tab1=TabSectionHeader::getTabSymb();
    //tab.resolveSymbols();

    tab1->backpatching();
    tab1=nullptr;

    //TabSectionHeader::printHeaderTable();
    oout.open(out);
    TabSectionHeader::writeTo(oout);
    oout.close();
    //tab.print();
    cout<< "done!"<<endl;

    return 0;
}

Token Assembler::findNextToken(vector<string>& str) {
    cout<<"proslo"<<endl;
    Token t=tok.resolveToken(str.front());
    vector<string>::iterator it= str.begin();
    str.erase(it);

    return t;
}


void Assembler::work() {

    string str;
    uint16_t loc_cnt=0; 
    string cur=""; 
    unsigned long cur_sec=-1;
    unsigned long count=inputFile.size();

    for(unsigned long i=0; i<count;i++) {
        cout<<"usao u for work"<<endl;
        vector<string> oneLineTokens=inputFile.at(i);
        //cout<<inputFile.at(i).front()<<endl;

        Token curT=tok.resolveToken(oneLineTokens.front());
        vector<string>::iterator it= oneLineTokens.begin();
        oneLineTokens.erase(it);

        if(curT.getToken_type()==Token_type::LABEL) {//keep that in mind
        cout<<"nasao sam labelu"<<curT.getVal()<<endl;
            TabSymb *tab1=nullptr;
            tab1=TabSectionHeader::getTabSymb();//nzm da li ce ovo sa ovim pokazivacima da radi
            tok.dealWithLabel(tab1,curT,cur_sec, i, loc_cnt);
            if(oneLineTokens.size()==0) {
                i++; 
                oneLineTokens=inputFile.at(i);
            }
            curT=tok.resolveToken(oneLineTokens.front());
            vector<string>::iterator it= oneLineTokens.begin();
            oneLineTokens.erase(it);   
        }
        cout<<"pronadjen string u liniji : "<<curT.getVal()<<"sa tokenom tipa : "<<curT.getToken_type()<<endl;
        switch(curT.getToken_type()) {
            case Token_type::LABEL: {
                //error neki baci ne moze labela posle labele 
                str="Label cannot come right after label";
                throw str; 
                break; 
            }

            case Token_type::SCOPE_DIRECTIVE:{
                unsigned long cntt=oneLineTokens.size();
                cout<<"nasla sam  token"<<curT.getVal()<<endl;
                if(cntt==0) {
                //error neki 
                    str="Scope directive must be followed by some symbols";
                    throw str;
                }
                string scope=curT.getVal(); 
                for(unsigned long j=0; j<cntt;j++) {
                    curT=tok.resolveToken(oneLineTokens.front());
        
                    cout<<"nasla sam sledeci token   "<<" "<<curT.getVal()<<endl;

                    vector<string>::iterator it= oneLineTokens.begin();
                    
                    oneLineTokens.erase(it);  

                    if(curT.getToken_type()==Token_type::SYMBOL) {
                        
                        TabSymb *tab=nullptr;
                        tab=TabSectionHeader::getTabSymb();

                        tok.dealWithScope(tab,curT, scope,cur_sec,loc_cnt); //hope this is doing the job 
                        tab=nullptr;
                    }else {
                        //some error
                        str="Scope directive must be followed  only by symbols";
                    throw str;

                    }
                }
                break;
            }

            case Token_type::MEMORY_DIRECTIVE:{
                if(cur_sec==-1 || oneLineTokens.size()==0){
                    //some error 
                    str="Memory directives cannot be defiend in und setion and must be followed by some parameters";
                    throw str;
                }
                cout<<"nasla sam sledeci token   "<<" "<<curT.getVal()<<endl;
                string mem=curT.getVal();
                if(curT.getVal()==".skip") {
                    unsigned long cnt=oneLineTokens.size();
                    for(unsigned j=0; j<cnt; j++) {
                        curT=tok.resolveToken(oneLineTokens.front());
                        vector<string>::iterator it= oneLineTokens.begin();
                        oneLineTokens.erase(it);  
                        cout<<"nasla sam sledeci token   "<<" "<<curT.getVal()<<endl;

                        if(curT.getToken_type()!=Token_type::HEX_NB && curT.getToken_type()!=Token_type::DEC_NB ){
                            //error
                            str=".skip directive  must be followed by hex or decimal number";
                            throw str;

                        }
                        unsigned long bytes= stol(curT.getVal(),nullptr,0);
                        if(bytes>Assembler::MAX_MEM || bytes<0){
                            //error
                            str="hex or decimal number in .skip directive must be in specified range";
                            throw str;

                        }
                        TabSection *tab=nullptr;
                         tab=TabSectionHeader::getTabSec(cur);
                       
                        loc_cnt+=static_cast<uint16_t>(tok.dealWithMemory(tab,curT,mem,bytes));
                         

                        tab=nullptr;

                    }
                }else if(cur==".bss"){
                    //error ne moze u bss 
                    str="Memory directives cannot be defiend in bss setion";
                    throw str;

                }else if(curT.getVal()==".byte" ||curT.getVal()==".word"){
                    unsigned long cnt2=oneLineTokens.size();
                    for(unsigned j=0; j<cnt2; j++) {

                        curT=tok.resolveToken(oneLineTokens.front());
                        vector<string>::iterator it= oneLineTokens.begin();
                        oneLineTokens.erase(it); 
                        cout<<"nasla sam sledeci token   "<<" "<<curT.getVal()<<endl;

                        long bytes=0;

                        if(curT.getToken_type()!=Token_type::SYMBOL && curT.getToken_type()!=Token_type::SECTION_DIRECTIVE){
                    
                            bytes= stol(curT.getVal(),NULL,0);
                        }

                        TabSection *tab2=nullptr;
                        tab2=TabSectionHeader::getTabSec(cur);
    
                         

                        loc_cnt+=(tok.dealWithMemory(tab2,curT,mem,bytes, cur,loc_cnt,Rel_type::R_16));
                         


                        tab2=nullptr;
                    }
                }else {
                    //some error 
                    str="Unrecognized memmory directive!";
                    throw str; 
                }
                break;
            }

            case Token_type::END_DIRECTIVE: {

                TabSymb *tab=nullptr;
                tab=TabSectionHeader::getTabSymb();
                tok.dealWithEnd(tab,cur_sec,loc_cnt);
                loc_cnt=0;
                tab=nullptr;

                break;
            }

            case Token_type::SECTION_DIRECTIVE: {
              

                if(curT.getVal()!=".section" || oneLineTokens.size()==0) {
                    //error kad je exception on ce sam izbaciti
                    str="Section directive must begin with keyword .section and must be followed by pramters";
                    throw str;
                }
                
                curT=tok.resolveToken(oneLineTokens.front());
                
                vector<string>::iterator it= oneLineTokens.begin();
                oneLineTokens.erase(it); 
                cout<<"nasla sam sledeci token   "<<" "<<curT.getVal()<<endl;

                string ss=curT.getVal();
                if(curT.getToken_type()!=Token_type::SECTION_DIRECTIVE || curT.getVal()==".section" || ss.at(0)!='.' ) {
                    //error
                    str="Section directive must be followed by other section parameter wich starts with .";
                    throw str;

                }
                
                TabSymb* tab=nullptr;
                tab=TabSectionHeader::getTabSymb();
                loc_cnt=tok.dealWithSection(tab, cur, loc_cnt,ss);
                
                cur=ss;
                cout<<ss<<" sada smo u sekciji toj"<<endl;
                
                cur_sec =tab->findSectionNB(cur);
                tab=nullptr;

                break;
            }

            case Token_type::EQU_DIRECTIVE: {
                if(cur_sec!=-1 || cur!="") { //maybe change this -1 and ""
                    str=".equ directive must be defined in und section ";
                    throw str;
                }
                if(oneLineTokens.size()<0) {
                    str=".equ directive must be followed at least with 2 symbols ";
                    throw str;                    
                }
                curT=tok.resolveToken(oneLineTokens.front());
                vector<string>::iterator it= oneLineTokens.begin();
                oneLineTokens.erase(it); 

                if(curT.getToken_type()!=Token_type::SYMBOL) {
                    str=".equ directive only can define new symbols ";
                    throw str;                    
                }
                cout<<"ovde sam"<<endl;

                string newExpLine =exp.expressionToString(oneLineTokens,curT.getVal()); 
                cout<<"vratila se iz expr to string"<<endl;

                vector<Token> exp_tokens= exp.reshapeToTokens(newExpLine);
                cout<<"vratila se iz reshape to tokens"<<endl;

                tab.insertUnresolvedSymbol(curT.getVal(),exp_tokens); 
                //inserted then in the end i will call from tab unresolved to clc index of classification 
                //and to calc expresion 
                //done with this, maybe it needs changing in index of classification and cacl of expression 
                break;
            }

            case Token_type::INSTRUCTION: {
                cout<<"usao u instruction"<<endl;
                //if not equal to .text section or some user section user section recognize with . at front 
                if(cur!=".text" || cur!=".TEXT") {
                    
                    if(cur==".rodata" || cur==".RODATA") {
                    string str="Can not use instruction in this sections "+cur;
                    throw str;
                    }
                    if(cur==".bss" || cur==".BSS") {
                    string str="Can not use instruction in this sections "+cur;
                    throw str;
                    }
                    if(cur=="" || cur=="UND" ){
                    string str="Can not use instruction in this sections "+cur;
                    throw str;
                    }
                        
                    if( cur==".data" || cur==".DATA" ){
                    string str="Can not use instruction in this sections "+cur;
                    throw str;

                    }
                }
                loc_cnt+=instr.resolveInstruction(oneLineTokens,curT.getVal(), cur, loc_cnt);
                break; 
            }

            default: {
                //some error 
                str="Token isnt right!";
                throw str; 
                break; 
            }

        }
    }
}

void backpatching() {
    TabSymb *tab=nullptr;
    tab=TabSectionHeader::getTabSymb();
    tab->backpatching();
    tab=nullptr;
}

void Assembler::printAll() {

}