#ifndef EXPR_H
#define EXPR_H
#pragma once
using namespace std; 
#include <vector>
#include <string>
#include <stdint.h>
#include "tab_section_header.h"
#include "token.h"
#include "tab_symb.h"

typedef struct indexClassEntry {
    unsigned long section;
    int val;

    indexClassEntry(unsigned long s, int v) {
        section=s;
        val=v;
    }
}indexClassEntry;
class Token;

class Expression {
private:
    vector<indexClassEntry> indClassf;
    static Token tok;
public:
    string expressionToString(vector<string> tokens, string symbol);

    vector<Token> reshapeToTokens(string line);
    uint16_t calculateExp(vector<Token> tok_exp, string symbol, bool& proceed);
    bool indexClass(vector<Token> tok_exp, unsigned long sec);
    bool sectionExsists(unsigned long section);
    indexClassEntry* getEntry(unsigned long section);


};

#endif 
