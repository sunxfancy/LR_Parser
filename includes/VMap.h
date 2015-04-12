#include "afx.h"
#include <map>

using namespace std;

#ifndef VMAP_H
#define VMAP_H

struct VMap {
    map<String, int> VnMap; // 用来处理非终结符
    map<String, int> VtMap; // 用来处理终结符
    map<String, int> ConstMap; // 用来处理静态终结符
    int constSize = 0;
    int constMax = 0;
    int InsertVn(const CHAR* s) {
        auto i = VnMap.find(s);
        if (i != VnMap.end()) return i->second;
        VnMap[s] = ++constSize;
        return constSize;
    }
    int getVn(const CHAR* s){
        return VnMap[s];
    }
    
    void InsertVt(const CHAR* s, int id) {
        VtMap[s] = id;
    }
    int getVt(CHAR* s){
        return VtMap[s];
    }
    
    int InsertConst(const CHAR* s) {
        auto i = ConstMap.find(s);
        if (i != ConstMap.end()) return i->second;
        ConstMap[s] = ++constSize;
        return constSize;
    }
    int getConst(CHAR* s){
        if (ConstMap.find(s) == ConstMap.end()) return -1;
        return ConstMap[s];
    }
    
    void print(map<String,int>& V){
        for (auto p = V.begin(); p != V.end(); ++p)
        {
            pair<String, int> t = *p;
            Print(t.first.c_str());
            Print(_T(":"));
            printf("%d\n",t.second);
        }
    }
    void printAll() {
        println(_T("==== Vn ===="));
        print(VnMap);
        println(_T("==== Vt ===="));
        print(VtMap);
        println(_T("==== Const ===="));
        print(ConstMap);
    }
    

    // from Vn id to Vn index in the vector
    int VnIDtoIndex(int p) { if (p<=constMax) return -1;  return p-constMax-1; }

};

#endif
