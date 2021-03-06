#ifndef TOKEN_H
#define TOKEN_H
#pragma once
#include <regex>
#include <vector> 
#include <iostream>
#include "tab_symb.h"
#include <stdint.h>
#include "tab_section.h"
#include "tab_section_header.h"
#include "tab_rel.h"

using namespace std; 


enum Token_type {
    DEFAULT=-1,
    LABEL=0, 
    SCOPE_DIRECTIVE=2, //.global .extern 
    EQU_DIRECTIVE=3, 
    END_DIRECTIVE, 
    MEMORY_DIRECTIVE, //.skip, .word, .byte 
    SECTION_DIRECTIVE,
    DEC_NB,
    HEX_NB,

    SYMBOL,
    EXPRESSION,
    PLUS_MINUS,
    INSTRUCTION,

    ASM,
    EXP_CLASS,
    INSTR_CLASS,

    OPERAND_IMMEDIATE_HEX,
	OPERAND_IMMEDIATE_DEC,
	OPERAND_IMMEDIATE_SYMBOL,
	OPERAND_REG_DIRECT_ADDR,
	OPERAND_REG_INDIRECT_ADDR,
	OPERAND_REG_INDIRECT_ADDR_OFFSET,


	OPERAND_ABS_HEX_JMP,
	OPERAND_ABS_DEC_JMP,
	OPERAND_ABS_SYMBOL_JMP,
    OPERAND_REG_DIRECT_ADDR_JMP,
	OPERAND_REG_INDIRECT_ADDR_JMP,
	OPERAND_REG_INDIRECT_ADDR_OFFSET_JMP

    
};

typedef struct tokk {
    Token_type type;
    string val;
    tokk(string v="",Token_type t=DEFAULT) {
        type=t; val=v; 
    }
}tokk;


#define MASK_BYTE 0x00FF
#define MASK_WORD 0xFFFF
const long dec_byte=127;
const long dec_word=32767;
const long hex_byte=255;
const long hex_word=65535;


class Assembler; 
class TabSymb;

class Token {
private: 
    tokk token;
    //za sada  ovde, mozda ga pomerim 
    //vector<Token> tokens;

public: 

    Token() {};
    Token(tokk t);
    Token(Token_type tip, string v);

    string getVal();
    Token_type getToken_type();

    string eraseComments(string line);
    vector<string> reshapeToTokens(string input);
    Token resolveToken(string t);

    void dealWithLabel(TabSymb* tab, Token t,unsigned long section, unsigned long line, uint16_t loc_cnt); 
    void dealWithScope(TabSymb* tab, Token t,string scope, unsigned long section, uint16_t loc_cnt);
    void dealWithEnd(TabSymb* tab,unsigned long section,uint16_t loc_cnt);

    unsigned long dealWithMemory(TabSection* tab, Token t,string mem_type, long bytes=0,string sect_cur="", 
        uint16_t loc_cnt=0, Rel_type type=Rel_type::R_16);

    uint16_t dealWithSection(TabSymb* tab, string old_section_name,uint16_t loc_cnt, string new_symb_name);

    //nije impl
    

    friend class Assembler;
};



#endif 

