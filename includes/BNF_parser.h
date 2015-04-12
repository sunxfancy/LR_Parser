/* 
* @Author: sxf
* @Date:   2015-01-03 18:43:13
* @Last Modified by:   sxf
* @Last Modified time: 2015-01-03 18:43:33
*/
#ifndef BNF_PARSER_H
#define BNF_PARSER_H

#include "afx.h"
#include "State.h"

class BNF_parser
{
public:
    BNF_parser();
    BNF_parser(const BNF_parser& other);
    ~BNF_parser();
    BNF_parser& operator=(const BNF_parser& other);
    bool operator==(const BNF_parser& other);
    State* Analysis_select(State*& root,State* last);
    State* Analysis(const CHAR*,int);
    State* getState();
    int getSize();
    void printTree();
private:
    // 元数据列表
    int size;
    const CHAR* data;
    int p;
    
    // BNF语句列表
    int state_size;
    State* state_vector;
    
    void blank();
    CHAR getNextalpha();
    CHAR* constword();
    CHAR* keyword();
    CHAR* script();
    bool Analysis_word(const CHAR* word);
    State* Analysis_const();
    State* Analysis_state();
    State* Analysis_temporality();
    State* Analysis_list();
    State* Analysis_terminal();
    void Analysis_script(State* root);
    void printNode(State* s,int d);
};

#endif // BNF_PARSER_H
