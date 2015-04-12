#include "afx.h"
#include "Lex.h"
#include <iostream>  
#include <fstream>  
#include <vector>  
#include <string>  
#include "Parser.h"
#include <unistd.h>
#define maxpath 1000
using namespace std;  

CHAR* fileReader(const char* path, int& flen) {
    wfstream file;
    locale::global(locale("zh_CN.UTF-8"));
    file.open(path);//打开文件
    if(!file.is_open())
    {
        printf("can not open BNF file!\n");
        return NULL;
    }
    file.seekg(0,ios::end);
    flen = file.tellg();
    file.seekg(0,ios::beg);
    CHAR* data = new CHAR[flen+1];
    file.read(data,flen);
    file.close();
    data[flen] = 0;
    printf("========= file reader ========\n");
    printf("file length:%d\n",flen);
//     分配数据空间
    printf("%S",data);
//    for (int i = 0; i< flen; ++i)
//        printf("%C",data[i]);
    printf("==============================\n");
    return data;
}

int main(int argc, char **argv) {
    char buffer[maxpath]; int flen;
    getcwd(buffer, sizeof(buffer));
    // Initialize the Lex Parser
    Lex* lexParser = new Lex();
//    bool b = lexParser->ReadConfig("/mnt/data/workspace/Complier/LR_parser/build/lex.txt");
    bool b = lexParser->ReadConfig("lex.txt");
    // file for test
    CHAR* data;
//    CHAR* data = fileReader("/mnt/data/workspace/Complier/LR_parser/build/test.txt",flen);
    if (argc>0)
        data = fileReader(argv[1],flen);
    if (data == NULL) {
        printf(argv[1]);
        printf("找不到程序源文件");
        return -1;
    }
    lexParser->Init(data);

    // Initialize the LR Parser
//    CHAR* lrcfg = fileReader("/mnt/data/workspace/Complier/LR_parser/build/lrparser.txt",flen);
    CHAR* lrcfg = fileReader("lrparser.txt",flen);
    if (data == NULL) {
        printf("找不到语法配置文件");
        return -1;
    }
    Parser* parser = Parser::NewLRParser();
    parser->setLex(lexParser);
    parser->BuildParser(lrcfg,flen);
    Grammer_Node* root = new Grammer_Node();
    parser->Parse(root);
    return 0;
}

