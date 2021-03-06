#include "instruction.h"


Token Instruction::tok=Token(Token_type::INSTR_CLASS,"instr");

Instruction::Instruction() {
    instrustions= unordered_map<string, int>();
    adressings = unordered_map<Adressing_type,int>();
    instrustions={
        {"halt",0},
        {"iret",1},
        {"ret",2},
        {"int",3},
        {"call",4},
        {"jmp",5},
        {"jeq",6},
        {"jne",7},
        {"jgt",8},
        {"push",9},
        {"pop",10},
        {"xchg",11},
        {"mov",12},
        {"add",13},
        {"sub",14},
        {"mul",15},
        {"div",16},
        {"cmp",17},
        {"not",18},
        {"and",19},
        {"or",20},
        {"xor",21},
        {"test",22},
        {"shl",23},
        {"shr",24}
    };
  /*  adressings = {
        {Adressing_type::IMM,0x0},
        {Adressing_type::REG_DIR,0x1},
        {Adressing_type::REG_INDIR,0x2},
        {Adressing_type::REG_INDIR_OFF,0x3},
        {Adressing_type::MEM_DIR,0x4}
    };*/
    adressings[Adressing_type::IMM]=0;
    //adressings[Adressing_type::REG_DIR]=0x1;
    adressings.insert({Adressing_type::REG_DIR,1});
    adressings[Adressing_type::REG_INDIR]=2;
    adressings[Adressing_type::REG_INDIR_OFF]=3;
    adressings[Adressing_type::MEM_DIR]=4;

}

uint16_t Instruction::findOperandSize(string& mn, bool& proceed) {
    uint16_t op_size=2;

    if(mn.at(mn.size()-1)=='w') {
        op_size=2;
        proceed=true;
        mn= mn.substr(0,mn.size()-1);



    }else if(mn.at(mn.size()-1)=='b' && mn!="sub") {
        op_size=1;
        proceed=false;

        mn= mn.substr(0,mn.size()-1);

    }
    return op_size;
}

uint16_t Instruction::findOperandCnt(string mn) {
    uint16_t cnt;
    if(mn=="halt" || mn=="iret" || mn=="ret") {
        cnt=0;
    }else if(mn=="int" ||mn=="call" ||mn=="jmp" ||mn=="jeq" ||mn=="jne" || mn=="jgt" ||mn=="push" ||mn=="pop" ) {
        cnt=1;
    } else {
        cnt=2;
    }
    return cnt;
}

string Instruction::getMNType(string mn) {
    string str;
    cout <<"prosledjen string u mn fju je "<<mn<<endl;
    if(mn=="int" ||mn=="call" ||mn=="jmp" ||mn=="jeq" ||mn=="jne" || mn=="jgt") {
        str="jump";
    } else 
        str="other";
    return str;
}

int8_t Instruction::findRegNbDir(string op) {
    string nb;
    cout<<"poslat string je  "<<op<<endl;

    if(op=="%r15")
        nb=op.substr(op.size()-2, 2);
    else 
        nb=op.substr(op.size()-1, 1);

    cout<<"broj registra je "<<nb<<endl;
    int8_t regnb= stoi(nb, NULL,0);
    return regnb;
}

int8_t Instruction::findRegNbIndir(string op) {
    string nb;
    //cout<<"prosledjen string je "<<op<<endl;

    if(op=="(%r15)")
        nb=op.substr(op.size()-3, 2);
    else 
        nb=op.substr(op.size()-2, 1);

    //cout<<"broj registra je "<<nb<<endl;

    int8_t regnb= stoi(nb, NULL,0);
    return regnb;
}

int8_t Instruction::findOpCode(int code) {
    cout<<"op code koji smo prosledili je " <<code<<endl;
    int8_t b2=0;
    if(code==1) {
        
    }
    int8_t newcode=static_cast<int8_t>(code);
    cout << "posle casta new code " <<+newcode<<endl;
    b2|=newcode<<5;

    return b2;
}
                                                                //9-4; simb(%r15) 10-4 
long Instruction::dealWithIndrOff(char ffind, string operand,int8_t reg_nb, string cur_section, bool first_op, uint16_t loc_cnt, Token tt, unsigned long vec_instr_size) { 
    //simb(%r7)  size=9 numeracija 0 do 8  positin sam doila 4 ->8-4
    size_t position_zagrada= operand.find(ffind,0);

    string newOperand = operand.substr(0,operand.size()-(operand.size()-position_zagrada));
     long vv;
    //cout<<newOperand<<"a pozicija zagrade je "<<position_zagrada<<endl;
    Token curT = tok.resolveToken(newOperand); 
    if(curT.getToken_type()!=Token_type::SYMBOL) {
     vv=stol(curT.getVal(),NULL,0); 

    }
    //cout<<"tip trenutnog tokena je "<<curT.getToken_type()<<endl;
    if((vv>32767 || vv<-32767) && curT.getToken_type()==Token_type::DEC_NB) {
        string str="Dec nb must be in specific range in instruction;2 ";
        throw str;
    }else if(vv>65535 && curT.getToken_type()==Token_type::HEX_NB) {
        string str="Hex nb must be in specific range in instruction;2 ";
        throw str;
    }else if(curT.getToken_type()==Token_type::SYMBOL){
        vv=0; 
        vv=dealWithSymbolIndrOff(curT.getVal(),reg_nb,  cur_section,first_op,loc_cnt,tt, vec_instr_size);
    } else {
        if(curT.getToken_type()!=Token_type::SYMBOL && curT.getToken_type()!=Token_type::HEX_NB && curT.getToken_type()!=Token_type::DEC_NB){
        string str= "Invalid sytax at indirect with data 1";
        throw str;
        }

    }

    return vv;

}

long Instruction::dealWithSymbolIndrOff(string new_symb,int8_t reg_nb, string cur_section, bool first_op, uint16_t loc_cnt, Token tt, unsigned long vec_instr_size) {
    long offset=0; 
    cout<<"usla u deal with section indr" <<endl;
    TabSymb* tab=nullptr;
    tab= TabSectionHeader::getTabSymb();
    int size=0;

    if(reg_nb==7) {
        if(tab->findSectionNB(cur_section)==tab->findSectionForSpecificSymbol(new_symb)) {
            size=0;
            if(first_op) {
                size+=1; 
                size+=3;
                if(tt.getToken_type()==Token_type::OPERAND_REG_DIRECT_ADDR ||tt.getToken_type()==Token_type::OPERAND_REG_INDIRECT_ADDR ) {
                    size+=1;

                }else if(tt.getToken_type()==Token_type::SYMBOL ||tt.getToken_type()==Token_type::HEX_NB || tt.getToken_type()==Token_type::DEC_NB
                ||tt.getToken_type()==Token_type::OPERAND_REG_INDIRECT_ADDR_OFFSET || tt.getToken_type()==Token_type::OPERAND_IMMEDIATE_SYMBOL) {
                    
                    size+=3;

                }else if(tt.getToken_type()==Token_type::OPERAND_IMMEDIATE_HEX||tt.getToken_type()==Token_type::OPERAND_IMMEDIATE_DEC) {
                    string val_str=tt.getVal().substr(1, tt.getVal().size()-1); //bez $
                    int16_t vv= stoi(val_str, NULL,0);

                    if(vv<=127 || vv>=-127){
                        size+=2;
                    } else {
                        size+=3;
                    }

                }else {
                    string str="Invalid next operand for simbol op in indr adr w off";
                    throw str;
                }

            }else {
                size+=vec_instr_size +2;
            }
            offset =(loc_cnt+size)*(-1);

        } else { //not in same section
            size=0;
            if(first_op) {
                 if(tt.getToken_type()==Token_type::OPERAND_REG_DIRECT_ADDR ||tt.getToken_type()==Token_type::OPERAND_REG_INDIRECT_ADDR ) {
                    size+=1;

                }else if(tt.getToken_type()==Token_type::SYMBOL ||tt.getToken_type()==Token_type::HEX_NB || tt.getToken_type()==Token_type::DEC_NB
                ||tt.getToken_type()==Token_type::OPERAND_REG_INDIRECT_ADDR_OFFSET || tt.getToken_type()==Token_type::OPERAND_IMMEDIATE_SYMBOL) {
                    
                    size+=3;

                }else if(tt.getToken_type()==Token_type::OPERAND_IMMEDIATE_HEX||tt.getToken_type()==Token_type::OPERAND_IMMEDIATE_DEC) {
                    string val_str=tt.getVal().substr(1, tt.getVal().size()-1); //bez $
                    int16_t vv= stoi(val_str, NULL,0);

                    if(vv<=127 || vv>=-127){
                        size+=2;
                    } else {
                        size+=3;
                    }

                }else {
                    string str="Invalid next operand for simbol op in indr adr w off";
                    throw str;
                }
                offset-= (size+2);
            }else {
                offset-=2; 
            }
        }
    }

    Rel_type rel_type=(reg_nb==7) ? Rel_type::R_PC16 : Rel_type::R_16;
    offset+= tab->createRelTable(new_symb, cur_section, static_cast<uint16_t>(loc_cnt +static_cast<uint16_t>(vec_instr_size)),rel_type);
    cout<<"posle povratka opet sam u instruction.cpp"<<endl;

    //this needs to be when is word about symbol
    /*
    //name of simbol
    size_t position_zagrada= curT.getVal().find('(',0);
    string newOperand = curT.getVal().substr(0,curT.getVal().size()-1-position_zagrada);

    */
   return offset;
}

long Instruction::dealWithIndrOffJMP(char ffind, string operand,int8_t reg_nb, string cur_section, uint16_t loc_cnt, Token tt) { 
    //simb(%r7)  size=9 numeracija 0 do 8  positin sam doila 4 ->8-4
    size_t position_zagrada= operand.find(ffind,0);

    string newOperand = operand.substr(0,operand.size()-(operand.size()-position_zagrada));
    cout<<newOperand<<"a pozicija zagrade je "<<position_zagrada<<endl;

    Token curT = tok.resolveToken(newOperand); 
    long vv;
    if(curT.getToken_type()!=Token_type::SYMBOL){
        vv=stol(curT.getVal(),NULL,0); 

    }

    if((vv>32767 || vv<-32767) && curT.getToken_type()==Token_type::DEC_NB) {
        string str="Dec nb must be in specific range in instruction;2 jump ";
        throw str;
    }else if(vv>65535 && curT.getToken_type()==Token_type::HEX_NB) {
        string str="Hex nb must be in specific range in instruction;2 jump ";
        throw str;
    }else if(curT.getToken_type()==Token_type::SYMBOL){
        vv=0; 
        vv=dealWithSymbolIndrOffJMP(curT.getVal(),reg_nb,  cur_section,loc_cnt,tt);
    } else {
        if(curT.getToken_type()!=Token_type::SYMBOL && curT.getToken_type()!=Token_type::HEX_NB && curT.getToken_type()!=Token_type::DEC_NB){

        string str= "Invalid sytax at indirect with jump";
        throw str;
        }
    }

    return vv;

}


long Instruction::dealWithSymbolIndrOffJMP(string new_symb,int8_t reg_nb, string cur_section, uint16_t loc_cnt, Token tt) {
    long offset=0; 

    TabSymb* tab=nullptr;
    tab= TabSectionHeader::getTabSymb();


    if(reg_nb==7) {
        if(tab->findSectionNB(cur_section)==tab->findSectionForSpecificSymbol(new_symb)) {
            offset= (loc_cnt +2 +2)*(-1);

        }else {
            offset-=2;
        }
    }
    Rel_type rel_type=(reg_nb==7) ? Rel_type::R_PC16 : Rel_type::R_16;
    offset+= tab->createRelTable(new_symb, cur_section, static_cast<uint16_t>(loc_cnt +static_cast<uint16_t>(2)),rel_type);
    return offset;
}

uint16_t Instruction::resolveInstruction(vector<string> operands, string token, string cur_section, uint16_t loc_cnt) {
    bool proceed;
    uint16_t op_size=findOperandSize(token, proceed);

    auto op_code= instrustions.find(token);
    cout<<op_code->second<< " code istrukcije je"<<endl;
    if(op_code==instrustions.end()) {
        string str="Unknown instruction";
        throw str;

    }
    uint16_t op_cnt=findOperandCnt(token); 
    if(operands.size()!=op_cnt) {
        string str="Invalid number of ooperands in instruction";
        throw str;
    }
    vector<int8_t> instr_code; 


    int8_t b1 = op_code->second <<3; 
    int8_t b2=0x00;
    b1= (op_size==1) ? (b1|0X00) :(b1|0x04);

    instr_code.push_back(b1);
    bool first_op=false;
    for(uint16_t i=0; i<op_cnt;i++) {
        first_op=(i==0) ? true:false;

        Token curT=tok.resolveToken(operands.front());

        
        vector<string>::iterator it= operands.begin();
        operands.erase(it);

        switch(curT.getToken_type()) {
            case Token_type::OPERAND_IMMEDIATE_DEC:
            case Token_type::OPERAND_IMMEDIATE_HEX:
            case Token_type::OPERAND_IMMEDIATE_SYMBOL:{
                string ret=getMNType(token);
                
                if(ret!="other") {
                    string str="Invalid syntax of operand in this mnemonik";
                    throw str;
                }

                if(i==1 && curT.getVal()!="cmp" && curT.getVal()!="test" && op_cnt==2) {
                    string str="Not allowed dst operand in immediate";
                    throw str;
                }

                auto adr_code=adressings.find(Adressing_type::IMM);
               /* b2=0;
                b2|=adr_code->second <<5;*/
                b2=findOpCode(adr_code->second);

                instr_code.push_back(b2);

                string val=curT.getVal();
                val= val.substr(1,val.size()-1);

                if(curT.getToken_type()==Token_type::OPERAND_IMMEDIATE_SYMBOL) {
                    if(op_size==1) {
                        string str="With this mnemonik and symbols as operand op size must be 2B";
                        throw str;
                    }
                    TabSymb* tab=nullptr;
                    tab= TabSectionHeader::getTabSymb();
                    uint16_t symb_val=tab->createRelTable(val,cur_section,static_cast<uint16_t>(loc_cnt + static_cast<uint16_t>(instr_code.size())) ,Rel_type::R_16);
                    tab=nullptr;

                    int8_t low= (symb_val & 0x00FF); 
                    instr_code.push_back(low); 
                    int8_t high= (symb_val & 0xFF00) >> 8;
                    instr_code.push_back(high);
                }else {
                    long  vv=stol(val, NULL,0);

                    if((vv>32767 || vv<-32767) && curT.getToken_type()==Token_type::OPERAND_IMMEDIATE_DEC) {
                        string str="Dec nb must be in specific range in instruction ";
                        throw str;
                    }
                    if(vv>65535 && curT.getToken_type()==Token_type::OPERAND_IMMEDIATE_HEX) {
                        string str="Hex nb must be in specific range in instruction ";
                        throw str;
                    }

                    if((curT.getToken_type()==Token_type::OPERAND_IMMEDIATE_HEX && vv<=255 ) || ((vv<=127 || vv>=-127) && curT.getToken_type()==Token_type::OPERAND_IMMEDIATE_DEC)) {
                        //1B
                        int8_t low= (vv & 0x00FF); 
                        instr_code.push_back(low);

                        if(proceed) {
                            int8_t high= (vv & 0xFF00) >> 8;
                            instr_code.push_back(high);
                        }


                    } else {
                        if(op_size==1) {
                            string str="Number is too big"; 
                            throw str;
                        }
                        
                        int8_t low= (vv & 0x00FF); 
                        instr_code.push_back(low); 
                        int8_t high= (vv & 0xFF00) >> 8;
                        instr_code.push_back(high);
                    }

                }

                break;
            }

            case Token_type::OPERAND_REG_DIRECT_ADDR :{
                string ret=getMNType(token);
                
                if(ret!="other") {
                    string str="Invalid syntax of operand in this mnemonik";
                    throw str;
                }

                auto adr_code=adressings.find(Adressing_type::REG_DIR);
                if(adressings.find(Adressing_type::REG_DIR)==adressings.end()) {
                    cout<<"uopste mi ne nalazi adr kode ZASTOOOOOOOOOOOOOO"<<endl;
                }
                

               /* b2=0;
                b2|=adr_code->second <<5;*/
                cout<<"zasto nece ovo second  : "<<adr_code->second<<endl;
                b2=findOpCode(adr_code->second);
                

                string curT_str=curT.getVal();

                if(curT_str.at(curT_str.size()-1)=='l') {
                    if(op_size!=1) {
                        string str="Not enough operands to access only lower byte";
                        throw str;
                    }
                    b2|=0x00;
                    curT_str= curT_str.substr(0,curT_str.size()-1); 
                } else if(curT_str.at(curT_str.size()-1)=='h') {
                    if(op_size!=1) {
                        string str="Not enough operands to access only higher byte";
                        throw str;
                    }
                    b2|=0x01;
                    curT_str= curT_str.substr(0,curT_str.size()-1);
                }

                int8_t reg_nb= findRegNbDir(curT_str); //maybe to move this shif in func
                b2 |= reg_nb <<1;
                cout<<"guramo b2 nam je "<<+b2<<endl;

                instr_code.push_back(b2);

                break;
            }

            case Token_type::OPERAND_REG_INDIRECT_ADDR: {
                string ret=getMNType(token);
                
                if(ret!="other") {
                    string str="Invalid syntax of operand in this mnemonik;indirect addr";
                    throw str;
                }

                auto adr_code=adressings.find(Adressing_type::REG_INDIR);

               /* b2=0;
                b2|=adr_code->second <<5;*/
                b2=findOpCode(adr_code->second);
                

                int8_t reg_nb =findRegNbIndir(curT.getVal()); 

                b2 |= reg_nb <<1;
                instr_code.push_back(b2);

                break;
            }

            case Token_type::OPERAND_REG_INDIRECT_ADDR_OFFSET: {
                string ret=getMNType(token);
                if(ret!="other") {
                    string str="Invalid syntax of operand in this mnemonik;indirect addr off";
                    throw str;
                }

                auto adr_code=adressings.find(Adressing_type::REG_INDIR_OFF);
                b2=findOpCode(adr_code->second);

                size_t position_zagrada= curT.getVal().find('(',0);
                string opernew= curT.getVal().substr(position_zagrada, curT.getVal().size() -position_zagrada);
                int8_t reg_nb =findRegNbIndir(opernew); 
                
                b2 |= reg_nb <<1;
                instr_code.push_back(b2);
                Token next_token= tok.resolveToken(operands.front());
                //cout<<"pre nego sto odem u deal with indr da vidim koi je token ovo kad mi usao u case za pc rel data : "<<next_token.getVal()<<endl;
                long offset=dealWithIndrOff('(',curT.getVal(),reg_nb,cur_section,first_op,loc_cnt, next_token,instr_code.size());
                
                //cout<<"vratila sam se u glavnu fju instr.cpp" <<endl;
                int8_t low= (offset & 0x00FF); 
                instr_code.push_back(low); 
                int8_t high= (offset & 0xFF00) >> 8;
                instr_code.push_back(high);


                break;
            }

            case Token_type::DEC_NB:
            case Token_type::HEX_NB:
            case Token_type::SYMBOL: {

                string ret=getMNType(token);
                
                if(ret=="other") {
                    auto adr_code=adressings.find(Adressing_type::MEM_DIR);
                    /* b2=0;
                    b2|=adr_code->second <<5;*/
                    b2=findOpCode(adr_code->second);
                    instr_code.push_back(b2);

                    long  vv;

                    if(curT.getToken_type()==Token_type::SYMBOL) {
                        TabSymb* tab=nullptr;
                        tab= TabSectionHeader::getTabSymb();
                        vv=tab->createRelTable(curT.getVal(),cur_section,static_cast<uint16_t>(loc_cnt + static_cast<uint16_t>(instr_code.size())) ,Rel_type::R_16);
                        tab=nullptr;

                    }else {
                        vv=stol(curT.getVal(),NULL,0);
                        if(curT.getToken_type()==Token_type::DEC_NB && (vv>65535 || vv<0)) {
                            string str="Dec nb must be in specific range in instruction ";
                            throw str;
                        } else if(curT.getToken_type()==Token_type::HEX_NB && (vv>65535)) {
                            string str="Hex nb must be in specific range in instruction ";
                            throw str;
                        }

                    }
                    int8_t low= (vv & 0x00FF); 
                    instr_code.push_back(low); 
                    int8_t high= (vv & 0xFF00) >> 8;
                    instr_code.push_back(high);

                } else if(ret=="jump") {
                    auto adr_code=adressings.find(Adressing_type::IMM);
                    /* b2=0;
                    b2|=adr_code->second <<5;*/
                    if(token=="jne")
                    cout<<"TU SAM! simbol!!!" <<endl;
                    b2=findOpCode(adr_code->second);
                    instr_code.push_back(b2);

                    long  vv;

                    if(curT.getToken_type()==Token_type::SYMBOL) {
                        TabSymb* tab=nullptr;
                        tab= TabSectionHeader::getTabSymb();
                        vv=tab->createRelTable(curT.getVal(),cur_section,static_cast<uint16_t>(loc_cnt + static_cast<uint16_t>(2)) ,Rel_type::R_16);
                        tab=nullptr;

                        int8_t low= (vv & 0x00FF); 
                        instr_code.push_back(low); 
                        int8_t high= (vv & 0xFF00) >> 8;
                        instr_code.push_back(high);

                    }else {
                        vv=stol(curT.getVal(),NULL,0);
                        if(curT.getToken_type()==Token_type::DEC_NB && (vv>65535 || vv<0)) {
                            string str="Dec nb must be in specific range in instruction ";
                            throw str;
                        } else if(curT.getToken_type()==Token_type::HEX_NB && (vv>65535)) {
                            string str="Hex nb must be in specific range in instruction ";
                            throw str;
                        }
                        if(vv<=127 || vv>=-127) {//1b
                            int8_t low= (vv & 0x00FF); 
                            instr_code.push_back(low);

                        }else {//2b
                            int8_t low= (vv & 0x00FF); 
                            instr_code.push_back(low); 
                            int8_t high= (vv & 0xFF00) >> 8;
                            instr_code.push_back(high);
                        }

                    }
                }
                break;
            }

            case Token_type::OPERAND_REG_DIRECT_ADDR_JMP :{
                string ret=getMNType(token);
                
                if(ret!="jump") {
                    string str="Invalid syntax of operand in this mnemonik jump";
                    throw str;
                }
                auto adr_code=adressings.find(Adressing_type::REG_DIR);

               /* b2=0;
                b2|=adr_code->second <<5;*/

                b2=findOpCode(adr_code->second);

                string cur_string= curT.getVal().substr(1,curT.getVal().size()-1); //bez *  *%r15

                int8_t reg_nb= findRegNbDir(cur_string); //maybe to move this shif in func
                b2 |= reg_nb <<1;
                instr_code.push_back(b2);      

                break;
            }

            case Token_type::OPERAND_REG_INDIRECT_ADDR_JMP:{
                string ret=getMNType(token);
                
                if(ret!="jump") {
                    string str="Invalid syntax of operand in this mnemonik;indirect addr jump";
                    throw str;
                }

                auto adr_code=adressings.find(Adressing_type::REG_INDIR);

               /* b2=0;
                b2|=adr_code->second <<5;*/
                b2=findOpCode(adr_code->second);


                string cur_string= curT.getVal().substr(1,curT.getVal().size()-1); //bez *  *(%r15)
                int8_t reg_nb= findRegNbIndir(cur_string);
                b2 |= reg_nb <<1;
                instr_code.push_back(b2);


                break;
            }

            case Token_type::OPERAND_REG_INDIRECT_ADDR_OFFSET_JMP : {
                string ret=getMNType(token);
                //cout << ret<< " to mi vratija fja get mn type"<<endl;
                if(ret!="jump") {
                    string str="Invalid syntax of operand in this mnemonik;indirect addr off jump";
                    throw str;
                }

                auto adr_code=adressings.find(Adressing_type::REG_INDIR_OFF);
                b2=findOpCode(adr_code->second);


                size_t position_zagrada= curT.getVal().find('(',0);  //nasla zagradu *simb(%r15)
                string opernew= curT.getVal().substr(position_zagrada, curT.getVal().size() -position_zagrada);
                int8_t reg_nb =findRegNbIndir(opernew); 
                
                b2 |= reg_nb <<1;
                instr_code.push_back(b2);
                Token next_token= tok.resolveToken(operands.front());
                string operand= curT.getVal().substr(1,curT.getVal().size()-1); 
                //cout<<"pre nego sto odem u deal with indr da vidim koi je token ovo kad mi usao u case za pc rel data : "<<next_token.getVal()<<endl;

                long offset=dealWithIndrOffJMP('(',operand, reg_nb,cur_section,loc_cnt,next_token);
                

                int8_t low= (offset & 0x00FF); 
                instr_code.push_back(low); 
                int8_t high= (offset & 0xFF00) >> 8;
                instr_code.push_back(high);
                break;
            }

            case Token_type::OPERAND_ABS_DEC_JMP:
            case Token_type::OPERAND_ABS_HEX_JMP:
            case Token_type::OPERAND_ABS_SYMBOL_JMP: {
                string ret=getMNType(token);
                if(token=="jne")
                    cout<<"TU SAM!!!!" <<endl;
                //cout << ret<< " to mi vratija fja get mn type"<<endl;
                if(ret!="jump") {
                    string str="Invalid syntax of operand in this mnemonik;op absolute jumb";
                    throw str;
                }

                auto adr_code=adressings.find(Adressing_type::MEM_DIR);
                /* b2=0;
                b2|=adr_code->second <<5;*/
                b2=findOpCode(adr_code->second);
                instr_code.push_back(b2);

                long  vv;
                string new_val= curT.getVal().substr(1, curT.getVal().size()-1);

                if(curT.getToken_type()==Token_type::OPERAND_ABS_SYMBOL_JMP) {
                        TabSymb* tab=nullptr;
                        tab= TabSectionHeader::getTabSymb();
                        vv=tab->createRelTable(new_val,cur_section,static_cast<uint16_t>(loc_cnt + static_cast<uint16_t>(2)) ,Rel_type::R_16);
                        tab=nullptr;

                }else {
                    vv=stol(new_val,NULL,0);
                    if(curT.getToken_type()==Token_type::OPERAND_ABS_DEC_JMP && (vv>65535 || vv<0)) {
                        string str="Dec nb must be in specific range in instruction ";
                        throw str;

                    } else if(curT.getToken_type()==Token_type::OPERAND_ABS_HEX_JMP && (vv>65535)) {
                        string str="Hex nb must be in specific range in instruction ";
                        throw str;
                    }

                }
                int8_t low= (vv & 0x00FF); 
                instr_code.push_back(low); 
                int8_t high= (vv & 0xFF00) >> 8;
                instr_code.push_back(high);
                
                break;
            }

            default : {
                string str="Unknow operand in instr" +token;
                throw str;
                break;
            }

        }
    }
    TabSection* tab_s=nullptr;
    tab_s=TabSectionHeader::getTabSec(cur_section);
    insertInSection(instr_code, tab_s);
    tab_s=nullptr;
    return static_cast<uint16_t>(instr_code.size());

}

void Instruction::insertInSection(vector<int8_t> instr_code, TabSection* tab) {

    for(auto inst:instr_code) {
        tab->generateByte(inst);
    }
}