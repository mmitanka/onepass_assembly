#ifndef INSTR_H
#define INSTR_H
#pragma once
#include <unordered_map>
#include <string> 
#include <stdint.h>
#include <vector>
#include "token.h"
#include "tab_section_header.h"
#include "tab_symb.h"
#include "tab_section.h"

using namespace std; 

enum Adressing_type {
    IMM=1, REG_DIR, REG_INDIR, 
    REG_INDIR_OFF, MEM_DIR
};


class Instruction {
private:
    static Token tok;
public:
    Instruction();

    unordered_map <string,int8_t> instrustions; 
    unordered_map <Adressing_type, int8_t> adressings; 

    uint16_t resolveInstruction(vector<string> operands, string token, string cur_section, uint16_t loc_cnt);
    uint16_t findOperandSize(string& mn, bool& proceed);
    uint16_t findOperandCnt(string mn);

    int8_t findRegNbDir(string op); 
    int8_t findRegNbIndir(string op);
 
    int8_t findOpCode(int8_t code);

    long dealWithIndrOff(char find, string operand, int8_t reg_nb, string cur_section, bool first_op, uint16_t loc_cnt, Token tt, unsigned long vec_instr_size);
    long dealWithSymbolIndrOff(string new_symb,int8_t reg_nb, string cur_section, bool first_op, uint16_t loc_cnt, Token tt, unsigned long vec_instr_size); 
    string getMNType(string mn);

    void insertInSection(vector<int8_t> instr_code, TabSection* tab);


    long dealWithIndrOffJMP(char find, string operand, int8_t reg_nb, string cur_section, uint16_t loc_cnt, Token tt);
    long dealWithSymbolIndrOffJMP(string new_symb,int8_t reg_nb, string cur_section, uint16_t loc_cnt, Token tt); 

};

#endif 