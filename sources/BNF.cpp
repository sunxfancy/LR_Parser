/* 
* @Author: sxf
* @Date:   2014-12-31 18:41:35
* @Last Modified by:   sxf
* @Last Modified time: 2015-01-03 14:52:57
*/

#include "BNF.h"

void BNF::print_bnf(int point) const {
    Print(this->root->state_class);
//    printf("%d",this->root->id);
    Print(_T(" => "));

    int k = 0;
    for (auto p = BNFdata.begin(); p!= BNFdata.end();++p)
    {
        if (point == k) Print(_T(". "));
        ++k;
        State* s = *p;
        if (s->state_type == constant)
            Print(s->state_const);
        else
            Print(s->state_class);
//        printf("%d",s->id);
        Print(_T(" "));
    }
    if (point == k) Print(_T(". "));
    Print(_T("\n"));
}

void BNF::print_bnf() const {
    print_bnf(-1);
}

int BNF::temp_size;
vector<BNF*> BNF::bnfs;

vector<BNF*> BNF::BuildAllBNF(State* root,VMap& vmap)
{
    BNF::bnfs.clear();
    BNF::temp_size = 0;
    State* p = root->children;
    while (p != NULL){
        BuildFromNode(p,vmap);
        p = p->brother;
    }
    vmap.constMax = vmap.constSize;
    // 最后遍历BNF列表，将Vn插入
    for (auto i = bnfs.begin(); i != bnfs.end(); ++i) {
        BNF* b = *i;
        b->setID(i-bnfs.begin());
        vmap.InsertVn(b->getRoot()->state_class);// 插入Vn表中
    }
    return BNF::bnfs;
}

void BNF::BuildFromNode(State* node, VMap& vmap)
{
    State* p = node->children;
    while (p != NULL){
    if (node->isList == false) {
	    BNF* bnf = new BNF();
	    bnf->root = node; // 根节点设置为要归约的部分
        bnf->script = p->script;
	    vector<State*>& vec = bnf->BNFdata;
	    State* q = p->children;
	    while (q != NULL){
            if (p->isList == true) {
                if (q->state_type != temporality) {
                    if (q->state_type != epsilon) {
                        vec.push_back(q);
                        if (q->state_type == constant) {
                            vmap.InsertConst(q->state_const); // 统计Constant的符号表
                        }
                    }
                } else {
                    ++temp_size;
                    CHAR* classname = new CHAR[10];
                    swprintf(classname,10,_T("temp%d"),temp_size);
                    q->state_class = classname;
                    vec.push_back(q);
                    BuildFromNode(q,vmap);
                }
            }
            q = q->brother;
	    }
	    BNF::bnfs.push_back(bnf);
	}
	p = p->brother;
    }
    
}
