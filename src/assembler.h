#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#pragma once
using namespace std; 
#include <vector>
#include "token.h"
#include <string> 
#include "tab_unresolved.h"
#include <iostream>
#include <fstream>
#include "expression.h"
#include "instruction.h"

class Assembler {
private:

    static const long MAX_MEM=65536; 

    static vector<vector<string>> inputFile;
    static Token tok; //name asm type ASM 
    static TabUnresolved tab;
    static Expression exp;
    static Instruction instr;
    static void backpaching();
    static void work();

    static Token findNextToken(vector<string> &str);
public:
    Assembler();
    static int readInputFile(string in_name, string out_name); 
    
    static void printAll(); 
};


#endif 
