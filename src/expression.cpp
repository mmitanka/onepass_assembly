#include "expression.h"
Token Expression::tok=Token(Token_type::EXP_CLASS,"exp");

string Expression::expressionToString(vector<string> tokens, string symbol) {
    
    string expression="";
    unsigned long count=tokens.size();
    for(unsigned long i=0; i<count;i++) {

        Token curT=tok.resolveToken(tokens.front());
        vector<string>::iterator it= tokens.begin();
        tokens.erase(it);

        switch(curT.getToken_type()) {
            case Token_type::SYMBOL:
            case Token_type::SECTION_DIRECTIVE: {
                    if(curT.getVal()==".section" || (expression.at(expression.size()-1)!='+' && expression.at(expression.size()-1)!='-')) {
                        string str="Invalid operand .1. or syntax in .equ";
                        throw str;
                    }

                    expression.append(curT.getVal()); 
                break;
            }
            case Token_type::PLUS_MINUS: {
                if((expression.at(expression.size()-1)=='+' || expression.at(expression.size()-1)=='-') && expression.size()!=0) {
                    string str="plus minus can not come after + -";
                    throw str;
                }
                expression.append(curT.getVal()); 

                break;
            }
            case Token_type::HEX_NB:{
                long vv=stol(curT.getVal(), NULL,0);
                if(vv>65535) {
                    string str="Hex nb must be in specific range in .equ";
                    throw str;
                }
                if(expression.at(expression.size()-1)!='+' && expression.at(expression.size()-1)!='-'){
                    string str="Invalid operand .2. or syntax in .equ";
                    throw str;
                }
                expression.append(curT.getVal()); 

                break;
            }
            case Token_type::DEC_NB: {
                long  vv=stol(curT.getVal(), NULL,0);
                if(vv>32767 || vv<-32767) {
                    string str="Dec nb must be in specific range in .equ";
                    throw str;
                }
            }
            case Token_type::EXPRESSION: {
                if (curT.getVal().at(0) == '+' || curT.getVal().at(0) == '-') {
                    if((expression.at(expression.size()-1)=='+' || expression.at(expression.size()-1)=='-') && expression.size()!=0) {
                        string str="plus minus can not come after + -";
                        throw str;
                    }
                }else if(expression.at(expression.size()-1)!='+' && expression.at(expression.size()-1)!='-'){
                    string str="Invalid operand .3. or syntax in .equ";
                    throw str;

                }
                expression.append(curT.getVal()); 
                break;
            }

            default:{
                    string str="Invalid operand .4. or syntax in .equ";
                    throw str;
            }
        }

    }

    if((expression.at(expression.size()-1)=='+' || expression.at(expression.size()-1)=='-')) {
        string str="Invalid expression in .equ";
        throw str;
    }

    return expression;
}

/*unchecked*/
vector<Token> Expression::reshapeToTokens(string line) {
    
    vector<Token> retToken;
    string t="";
    for(char ch:line) {
        if(ch=='+' || ch=='-') {
            if(t!="")
                retToken.push_back(tok.resolveToken(t));
            t="";
            t+=ch;
            retToken.push_back(tok.resolveToken(t));
        }else {
            t+=ch;
        }
    }
    if(t!="")
        retToken.push_back(tok.resolveToken(t));

    return retToken;

}

/*if returned false it means that symbol from tab_symb isnt defined
then add this new simbol from equ in unresolved table;
true only insert to symbtable*/
/*PITAJ UROSA SUTRA STA SE RADI TACNO ZNACI AKO PRODJE KLASSIFIKACIONU PROVERU 
NE IZBCI NIJEDAN EXCEPTION ONDA AKO JE EXTERN PRAVIM TAB RELOKACIJE I KOJ JE TIP TU ONDA PC16 ILI 16 SAMO
ILI PRAVIM TAB RELOKACIJE ZA STA?? ZA NOVI SIMBOL ILI -A+B+C OVDE ZA C?HM ???? Z*/
/*nema tab rel, ovo pozivamo tek na kraju iz tab unresolved
simbol ako ima samo konstante onda je nova sekcija ABS moramo da je ubacimo
ako je extern onda je u und sekciji a inace je u sekciji iz koje je ovaj sto ima jedini index relokacije 1*/
uint16_t Expression::calculateExp(vector<Token> tok_exp, string symbol, bool& proceed) {
    uint16_t res;
    char ch;
    unsigned long count=tok_exp.size();
    for(unsigned long i=0; i<count;i++){
        Token curT=tok_exp.front();
        vector<Token>::iterator it= tok_exp.begin();
        tok_exp.erase(it);

        if(curT.getToken_type()==Token_type::PLUS_MINUS) {
            ch=(curT.getVal()=="+")? '+':'-';
        }else if(curT.getToken_type()==Token_type::HEX_NB ){
            long vv=stol(curT.getVal(), NULL,0);
            if(vv>65535) {
                string str="Hex nb must be in specific range in .equ";
                throw str;
            }
            if(ch=='+') {
                res+= static_cast<uint16_t>(vv);
            } else {
                res-= static_cast<uint16_t>(vv);
            }

        }else if(curT.getToken_type()==Token_type::DEC_NB) {
            long  vv=stol(curT.getVal(), NULL,0);
            if(vv>32767 || vv<-32767) {
                string str="Dec nb must be in specific range in .equ";
                throw str;
            }
            if(ch=='+') {
                res+= static_cast<uint16_t>(vv);
            } else {
                res-= static_cast<uint16_t>(vv);
            }

        }else if(curT.getToken_type()==Token_type::SECTION_DIRECTIVE||curT.getToken_type()==Token_type::SYMBOL) {
            if(curT.getVal()==".section"){
                string str="Invalid syntax in .equ calculate";
                throw str;
            }
            TabSymb *tab=nullptr;
            tab=TabSectionHeader::getTabSymb();
            if(tab->alreadyExists(curT.getVal())) {
                if(tab->isDefined(curT.getVal()))
                    proceed=true;
                Entry entry_symb= tab->getEntry(curT.getVal());

                if(proceed) {
                    if(ch=='+') {
                         res+=entry_symb.value ;
                    } else {
                        res-= entry_symb.value;
                    }
                }

            }
        }

    }
    return res;
}

bool Expression::sectionExsists(unsigned long section) {
    if(indClassf.size()!=0){
        for(indexClassEntry ent:indClassf) {
            if(ent.section==section)
                return true;
        }

    }
    return false; 

}

indexClassEntry* Expression::getEntry(unsigned long section) {
    indexClassEntry* newEnt=nullptr;
        for(indexClassEntry ent:indClassf) {
            if(ent.section==section)
                newEnt= &ent;
        }
    return newEnt;
}

/*if index of classification returns true then all symbols are known and its possible to calculate equ expression
then call calculateExp; if returned false you may add this new to unresolved table*/
//prosiriti da prima jos jedan &unsigned long koji ce mi vracati 0 kad je extern 
//-2 kad treba da dodam abs sekciju i inace broj sekcije gde je index klas 1
bool Expression::indexClass(vector<Token> tok_exp) {
    char ch;
    unsigned long count=tok_exp.size();
    for(unsigned long i=0; i<count;i++) {
        Token curT=tok_exp.front();
        vector<Token>::iterator it= tok_exp.begin();
        tok_exp.erase(it);

        if(curT.getToken_type()==Token_type::PLUS_MINUS) {
            ch=(curT.getVal()=="+")? '+':'-';
        }else if(curT.getToken_type()==Token_type::SECTION_DIRECTIVE||curT.getToken_type()==Token_type::SYMBOL) {
            if(curT.getVal()==".section"){
                string str="Invalid syntax in .equ calculate";
                throw str;
            }
            TabSymb *tab=nullptr;
            tab=TabSectionHeader::getTabSymb();
            if(tab->alreadyExists(curT.getVal())){
                Entry entry_symb= tab->getEntry(curT.getVal());
                if(entry_symb.type==Symbol_type::EXTERN) {
                    if(sectionExsists(0)) {
                        indexClassEntry* ent= getEntry(0);
                        if(ent!=nullptr){
                            ent->val++;
                        }

                    } else {
                        indexClassEntry e1=indexClassEntry(0,1);
                        indClassf.push_back(e1);
                    }
                }else {
                    if(sectionExsists(entry_symb.section)) {
                        indexClassEntry* ent= getEntry(entry_symb.section);
                        if(ch=='+') {
                            ent->val++;
                        } else {
                           ent->val--;
                        }
                    } else {
                        int v; 
                        if(ch=='+') {
                            v=1; 
                        } else {
                            v=-1;
                        }
                        indexClassEntry e1=indexClassEntry(entry_symb.section,v);
                        indClassf.push_back(e1);
                    }
                }
            }else {
                //symbol from equ dont exists in symb table
                //return false to know to put this to tns table
                return false;
            }
        }
    }
    //done with inserting into index of classification table
    //time to check if all values are 0 and possibly one value is 
    //1
    int cnt_one=0; 
    for(indexClassEntry ind:indClassf) {
        if(ind.val==1){
            cnt_one++;
        }else if(ind.val<=-1 || ind.val>=2) {
            string str="Invalid equ expression, index of classification isnt right";
            throw str;
        }else if(ind.val==0)
            continue;
    }
    if(cnt_one>1) {
        string str="Invalid equ expression, index of classification isnt right: multiple index 1";
        throw str;
    }
    return true;
}