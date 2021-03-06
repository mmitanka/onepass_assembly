#include "token.h" 
#include <string> 
#include <vector> 
#include "tab_symb.h"
#include <stdint.h>
#include <string>         
#include <cstddef> 
#include "tab_section_header.h"



/*costructurs*/ 

Token::Token(tokk t) {
    token=t;
}

Token::Token(Token_type t, string v ) {
    token=tokk(v,t); 
}

/*getters */ 
string Token::getVal(){
    return token.val; 
}

Token_type Token::getToken_type() {
    return token.type;
}

/*erase coments from one line*/

string Token::eraseComments(string line) {

    size_t pos=line.find_first_of("#"); 

    if(pos!=string::npos) {
        line.erase(pos);

    }
    //newLine= line.substr(0, pos-1);

    return line; //consider maybe return value should be refence & 
}

/*vec dobijam sredjenu liniju::SAMO JEDNU SA VISE TOKENA, znaci izbrisani komentari i gluposti blanko*/
vector<string> Token::reshapeToTokens(string input) {

    vector<string> tokens; 

    string newInput=""; 


    size_t pos_char = input.find_first_not_of(", \r\v\t\n\f",0);
    size_t pos_del=input.find_first_of(", \r\v\t\n\f",pos_char);

    while(pos_char!=string::npos || pos_del!=string::npos) {
        newInput=input.substr(pos_char,pos_del-pos_char);

        if(newInput.size()>0){
            tokens.push_back(newInput);
            //cout<<"nasao neki delimiter; token je  "<<" "<<newInput <<endl;
            newInput="";
        }
        pos_char = input.find_first_not_of(", \r\v\t\n\f",pos_del);
        pos_del=input.find_first_of(", \r\v\t\n\f",pos_char);
    }
    //cout<<input.at(11)<<input.at(12)<<endl;
 /*   for(char in:input) {
        if(in=='\r' || in==' ' || in==',' || in=='\f' || in=='\t'|| in=='\n' || in=='\v'){
            //newInput+=in;
            //kada nadjem neki delimiter ja tek tad push u token
           if(newInput.size()!=0){
               tokens.push_back(newInput);
               cout<<"nasao neki delimiter; token je  "<<" "<<newInput <<endl;
               newInput=""; 
           }
           

        }else if(in==':') { //for labels 
            /*if(newInput.size()!=0) {
                //tokens.push_back(newInput); 
            } else 
                tokens.at(tokens.size()-1)+=in;
            newInput="";
            newInput+=in; 
            tokens.push_back(newInput);
            cout<<"nasao : ; token je   "<<" "<<newInput <<endl;

            newInput="";

        } else if(in=='+' || in=='-') {
             if(newInput.size()!=0) {
                tokens.push_back(newInput); //first push operand 
                newInput="";
            }
            newInput+=in; /then push + or - in regulating string 
        } else {
            //cout<<"nadjeno nesto drugo " <<in <<endl;
            newInput+=in;
            //if(newInput=="+" || newInput=="-") continue; //waiting for next operand 
           // if(newInput.size()!=0)
                //tokens.push_back(newInput); 
            //newInput="";

        }


    }
*/
    if(newInput.size()!=0)
        tokens.push_back(newInput); 

    return tokens;

}



/*recognize what is token type of argument using regex */
/*needs more constructing*/
Token Token::resolveToken(string t) {

    if(regex_match(t, regex("^([a-zA-Z_][a-zA-Z0-9_]*_{0,}):$"))) {
        //is label
        return Token(Token_type::LABEL, t.substr(0,t.size()-1)); //-1 cause of ':' at the end 

    }else if (regex_match(t, regex("^\\.(extern|global|EXTERN|GLOBAL)$"))){
        //is extern or global 
        return Token(Token_type::SCOPE_DIRECTIVE, t);

    } else if(regex_match(t, regex("^\\.(equ|EQU)$"))){
        //is equ 
        return Token(Token_type::EQU_DIRECTIVE, t);

    } else if(regex_match(t, regex("^\\.(end|END)$"))) {
        //is end
        return Token(Token_type::END_DIRECTIVE, t);

    } else if(regex_match(t, regex("^\\.(skip|word|byte|SKIP|WORD|BYTE)$"))){
        //is .skip .word .byte 
        return Token(Token_type::MEMORY_DIRECTIVE, t);
    //N NEEDS TO BE updated because of user sections 
    }else if(regex_match(t, regex("^\\.(section|SECTION|text|TEXT|bss|BSS|rodata|RODATA|data|DATA|([a-zA-Z_][a-zA-Z0-9_]*_{0,}))$"))) {
       //predefined and user defined sections
        return Token(Token_type::SECTION_DIRECTIVE, t);

    } else if(regex_match(t,
        regex("^(((push|PUSH|pop|POP|xchg|XCHG|mov|MOV|add|ADD|sub|SUB|mul|MUL|div|DIV|cmp|CMP|not|NOT|and|AND|or|OR|xor|XOR|test|TEST|shl|SHL|shr|SHR)(b|B|w|W){0,1})|(halt|HALT|iret|IRET|ret|RET|int|INT|call|CALL|jmp|JMP|jeq|JEQ|jne|JNE|jgt|JGT))$"))) {
        t=regex_replace(t,regex("B"),"b");
        t=regex_replace(t,regex("W"),"w");
        
        return Token(Token_type::INSTRUCTION, t);
    
    } else if(regex_match(t,regex("^(\\-|\\+){0,1}[0-9]{1,}$"))) {
        return Token(Token_type::DEC_NB,t);

    } else if (regex_match(t,regex("^0x[0-9a-fA-F]{1,}$"))){
        return Token(Token_type::HEX_NB,t);
    } else if(regex_match(t,regex("^([a-zA-Z_][a-zA-Z0-9_]*_{0,})$"))) {
        //symbol
        return Token(Token_type::SYMBOL,t);

    }else if (regex_match(t, regex("^(\\+|\\-){0,1}(([a-zA-Z_][a-zA-Z0-9_]*_{0,})|([0-9]{1,})|(0x[0-9a-fA-F]{1,})|(\\.(section|SECTION|text|TEXT|bss|BSS|rodata|RODATA|data|DATA|([a-zA-Z_][a-zA-Z0-9_]*_{0,}))))((\\+|\\-)(([a-zA-Z_][a-zA-Z0-9_]*_{0,})|([0-9]{1,})|(0x[0-9a-fA-F]{1,})|(\\.(section|SECTION|text|TEXT|bss|BSS|rodata|RODATA|data|DATA|([a-zA-Z_][a-zA-Z0-9_]*_{0,})))))*(\\+|\\-){0,1}$"))) {
        //expression
        return Token(Token_type::EXPRESSION,t);

    }else if(regex_match(t,regex("^(\\+|\\-)$"))) {
        return Token(Token_type::PLUS_MINUS, t);
    }else if(regex_match(t,regex("^\\$0x[0-9a-fA-F]{1,}$"))) {
        return Token(Token_type::OPERAND_IMMEDIATE_HEX,t); 

    }else if(regex_match(t,regex("^\\$(\\+|\\-){0,1}[0-9]{1,}$"))) {
        return Token(Token_type::OPERAND_IMMEDIATE_DEC,t); 

    }else if(regex_match(t,regex("^\\$([a-zA-Z_][a-zA-Z0-9_]*_{0,})$"))) {
        return Token(Token_type::OPERAND_IMMEDIATE_SYMBOL,t);

    }else if(regex_match(t,regex("^%((r[0-7])|psw|PSW|pc|PC|sp|SP)(h|H|L|l){0,1}$"))) {
        t=regex_replace(t,regex("sp"),"r6");
        t=regex_replace(t,regex("SP"),"r6");
        t=regex_replace(t,regex("psw"),"r15");
        t=regex_replace(t,regex("PSW"),"r15");
        t=regex_replace(t,regex("PC"),"r7");
        t=regex_replace(t,regex("pc"),"r7");
        t=regex_replace(t,regex("H"),"h");
        t=regex_replace(t,regex("L"),"l");

        return Token(Token_type::OPERAND_REG_DIRECT_ADDR,t);

    }else if(regex_match(t,regex("^\\(%(((r|R)[0-7])|psw|PSW|pc|PC|sp|SP)\\)$"))) {
        t=regex_replace(t,regex("sp"),"r6");
        t=regex_replace(t,regex("SP"),"r6");
        t=regex_replace(t,regex("psw"),"r15");
        t=regex_replace(t,regex("PSW"),"r15");
        t=regex_replace(t,regex("PC"),"r7");
        t=regex_replace(t,regex("pc"),"r7");

        return Token(Token_type::OPERAND_REG_INDIRECT_ADDR,t);

    }else if(regex_match(t,regex("^\\*%((r[0-7])|psw|PSW|pc|PC|sp|SP)(h|H|L|l){0,1}$"))) {
        t=regex_replace(t,regex("sp"),"r6");
        t=regex_replace(t,regex("SP"),"r6");
        t=regex_replace(t,regex("psw"),"r15");
        t=regex_replace(t,regex("PSW"),"r15");
        t=regex_replace(t,regex("PC"),"r7");
        t=regex_replace(t,regex("pc"),"r7");
        t=regex_replace(t,regex("H"),"h");
        t=regex_replace(t,regex("L"),"l");

        return Token(Token_type::OPERAND_REG_DIRECT_ADDR_JMP,t);
    }
    else if(regex_match(t,regex("^(\\-|\\+){0,1}[a-zA-z0-9_]{1,}\\(%((r[0-7])|psw|PSW|pc|PC|sp|SP)\\)$"))) {
        t=regex_replace(t,regex("sp"),"r6");
        t=regex_replace(t,regex("SP"),"r6");
        t=regex_replace(t,regex("psw"),"r15");
        t=regex_replace(t,regex("PSW"),"r15");
        t=regex_replace(t,regex("PC"),"r7");
        t=regex_replace(t,regex("pc"),"r7");

        return Token(Token_type::OPERAND_REG_INDIRECT_ADDR_OFFSET,t);

    }else if(regex_match(t,regex("^\\*0x[0-9a-fA-F]{1,}$"))) {
        return Token(Token_type::OPERAND_ABS_HEX_JMP,t);

    }else if(regex_match(t,regex("^\\*(\\+|\\-){0,1}[0-9]{1,}$"))) {
        return Token(Token_type::OPERAND_ABS_DEC_JMP,t);
        
    }else if(regex_match(t,regex("^\\*([a-zA-Z_][a-zA-Z0-9_]*_{0,})$"))) {
        return Token(Token_type::OPERAND_ABS_SYMBOL_JMP,t);
        
    }else if(regex_match(t,regex("^\\*\\(%((r[0-7])|psw|PSW|pc|PC|sp|SP)\\)$"))) {
        t=regex_replace(t,regex("sp"),"r6");
        t=regex_replace(t,regex("SP"),"r6");
        t=regex_replace(t,regex("psw"),"r15");
        t=regex_replace(t,regex("PSW"),"r15");
        t=regex_replace(t,regex("PC"),"r7");
        t=regex_replace(t,regex("pc"),"r7");

        return Token(Token_type::OPERAND_REG_INDIRECT_ADDR_JMP,t);

    }else if(regex_match(t,regex("^\\*(\\-|\\+){0,1}[a-zA-z0-9_]{1,}\\(%((r[0-7])|psw|PSW|pc|PC|sp|SP)\\)$"))) {
        t=regex_replace(t,regex("sp"),"r6");
        t=regex_replace(t,regex("SP"),"r6");
        t=regex_replace(t,regex("psw"),"r15");
        t=regex_replace(t,regex("PSW"),"r15");
        t=regex_replace(t,regex("PC"),"r7");
        t=regex_replace(t,regex("pc"),"r7");

        return Token(Token_type::OPERAND_REG_INDIRECT_ADDR_OFFSET_JMP,t);
    } else {
        return Token(Token_type::DEFAULT,t); 
    }

    //if doesnt match anything then is invalid type; DEFAULT 

}



/*calling this one from assembler function in which i am going through tokens vector;one line at the time*/ 

void Token::dealWithLabel(TabSymb* tab,Token t,unsigned long section, unsigned long line, uint16_t loc_cnt) {
    if(section==0) { //und section 
        //error; unatorized label in no section 
        cout<<"kaze mi da mi je sekcija poslata 0";
        return; 
    }
    //this tab will be static pointer to one globaly simb tab from section header table 
    tab->setNewEntry(t.getVal(),"label" ,section, 0,loc_cnt, true);
}


/*calling this one from assembler function in which i am going through tokens vector;one line at the time*/ 
void Token::dealWithScope(TabSymb* tab,Token t,string scope,unsigned long section, uint16_t loc_cnt) {
    //for u fji iz koje pozivam ovo 
    if(section==-1)
        section=0;
    tab->setNewEntry(t.getVal(),scope,section,0,loc_cnt,false);//proveri za ovo false 

}

/*calling this one from assembler function in which i am going through tokens vector;one line at the time*/ 
void Token::dealWithEnd(TabSymb* tab, unsigned long section, uint16_t loc_cnt) {
    tab->setSectionSize(section, loc_cnt); 
}

/*not sure if works really properly*/
/*calling this one from assembler function in which i am going through tokens vector;one line at the time*/ 
uint16_t Token::dealWithSection(TabSymb* tab, string old_section_name, uint16_t loc_cnt, string new_symb_name) {
    
    if(old_section_name==""){
        cout<<"u undu smo"<<endl;
        old_section_name="UND";
    }
    Entry ent= tab->getEntry(old_section_name);
    
    tab->redefineSymbol(ent.name,ent.value,ent.type,ent.section,loc_cnt, true,".section");

    //new symbol insert
    unsigned long sz=tab->getNbOfEntry();
    tab->setNewEntry(new_symb_name,".section",sz,0,0,true);
    return 0; 
}

/*not sure if works really properly*/
/*calling this one from assembler function in which i am going through tokens vector;one line at the time*/ 
unsigned long Token::dealWithMemory(TabSection* tab, Token t,string memory_type, long bytes,string sect_cur, 
        uint16_t loc_cnt, Rel_type type) {
    string str;
    if(memory_type==".skip") {
        if(bytes==-1)
            return bytes;//da vratimo 0

        for(unsigned long i=0; i<bytes; i++)
            tab->generateByte(0);

        return bytes; 
    } else if(memory_type==".byte") {

        long par1;
        long par2=0;

        if(t.getToken_type()==Token_type::HEX_NB){
            par1=hex_byte;
            if(bytes > par1 ) {
            //baci error
            str="hex  .byte directive must be in specified range";
             throw str;

            }
        }else if(t.getToken_type()==Token_type::DEC_NB){
            par1=dec_byte;
            par2=-dec_byte;
            if(bytes > par1|| bytes <par2) {
            //baci error
            str=" decimal number in .byte directive must be in specified range";
             throw str;

            }
        }

        int8_t pod= bytes & MASK_BYTE; //proveri da li je uint ili int 
        if(t.getToken_type()==Token_type::SYMBOL ||t.getToken_type()==Token_type::SECTION_DIRECTIVE) {
            if(t.getVal()==".section") {
                str="If token type is section it must contain name of specific section :.byte";
                throw str; 
                return 0; 
            }

            TabSymb *tab1= nullptr;
            tab1= TabSectionHeader::getTabSymb();
            pod= (tab1->createRelTable(t.getVal(),sect_cur,loc_cnt,type)) & MASK_BYTE;
            tab1=nullptr;
        }
        tab->generateByte(pod); 
        long rrrr=1;
        return rrrr; 
    }else if(memory_type==".word"){
        long par1;
        long par2=0;

        if(t.getToken_type()==Token_type::HEX_NB)
            par1=hex_word;
        else if(t.getToken_type()==Token_type::DEC_NB){
            par1=dec_word;
            par2=-dec_word;
        }
        if(bytes > par1 || bytes <par2) {
            //baci error
            str="hex or decimal number in .word directive must be in specified range";
             throw str;

        }
        int16_t pod= bytes & MASK_WORD; 
        if(t.getToken_type()==Token_type::SYMBOL ||t.getToken_type()==Token_type::SECTION_DIRECTIVE) {
            if(t.getVal()==".section") {
                //error 
                str="If token type is section it must contain name of specific section :.word";
                throw str; 
                return 0; 
            }
            TabSymb *tab1=nullptr;
            tab1= TabSectionHeader::getTabSymb();
            pod= (tab1->createRelTable(t.getVal(),sect_cur,loc_cnt,type));
            tab1=nullptr;
            
        }
        tab->generateWord(pod);
        long rr=2;
        return rr; 
    }
    return 0;
}