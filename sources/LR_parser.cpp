#include "LR_parser.h"
#include "BNF_parser.h"
#include "LALRTable.h"

LR_parser::LR_parser()
{
	
}

LR_parser::~LR_parser()
{

}


void LR_parser::ExtendBNF()
{
    mainbnf = new BNF();
    State* s = new State();
    s->state_class = _T("<Main>");
    s->state_type = statement;
    s->id = vmap.InsertVn(_T("<Main>"));
    mainbnf->setRoot(s);
    mainbnf->setID(-1);
    mainbnf->addBNFdata(bnflist[0]->getRoot());
}

void LR_parser::MakeID()
{
    // print debug message
    printf("========= print V* =========\n");
    vmap.printAll();
    printf("============================\n");
    for (auto p = bnflist.begin(); p != bnflist.end(); ++p) {
        BNF* bnf = *p;
        State* s = bnf->getRoot();
        s->id = vmap.getVn(s->state_class);
        const vector<State*>& vec = bnf->getBNFdata();
        for (auto q = vec.begin(); q != vec.end(); ++q) {
            s = *q;
            if (s->state_type == constant)
            s->id = vmap.getConst(s->state_const);
            else if (s->state_type == temporality || s->state_type == statement )
            s->id = vmap.getVn(s->state_class);
            else if (s->state_type == epsilon )
            s->id = 0;
            else if (s->state_type == terminal )
            s->id = vmap.getVt(s->state_class);
        }
    }
}

void print_ItemCollection(vector<ItemCollection*> vec) {
    for (auto p = vec.begin(); p != vec.end(); ++p) {
        ItemCollection* items = *p;
        printf("I%d:",items->getID());
        items->printSet();
    }
}

void LR_parser::print_GOTO(vector<ItemCollection*> vec) {
    printf("======= GOTO =======\n");
    for (int i = 0; i< vmap.constSize; ++i) {
        printf("\t%d",i);
    }
    printf("\n");
    for (auto p = vec.begin(); p != vec.end(); ++p) {
        ItemCollection* items = *p;
        printf("I%d:",items->getID());
        for (int i = 0; i< vmap.constSize; ++i) {
            ItemCollection* gotoitems = items->GOTO(i);
            int gotoid = -1;
            if (gotoitems != NULL) {
                gotoid = gotoitems->getID();
            }
            printf("\t%d",gotoid);
        }
        printf("\n");
    }


    printf("======= Spread =======\n");
    for (auto p = vec.begin(); p != vec.end(); ++p) {
        ItemCollection* items = *p;

        items->printSpread();
        printf("\n");
    }

}

void LR_parser::BuildParser()
{
    ExtendBNF();
    MakeID(); // for each state, make a ID for it

    // 创建LR0项集族
    vector<ItemCollection*> vec = ItemCollection::MakeLR0Items(&vmap, mainbnf, bnflist);
//    printf("======== print LR0 Collection ========\n");
//    print_ItemCollection(vec);
    // 构建LALR项集族
    ItemCollection::MakeLALRItems(vec,bnflist);
    printf("======== print LR1 Collection ========\n");
    print_ItemCollection(vec);
    print_GOTO(vec);
//    printf("test: \t %d %d %d\n",vmap.constMax+1,vec.size(),vmap.constSize);
    table = (LRTable*)new LALRTable(vmap.constMax+1,vec.size(),vmap.constSize);
    table->BuildTable(vec);
//    table->printTable();
}

void LR_parser::BuildParser(CHAR* data,int flen)
{
    AddBNF(data,flen);
    BuildParser();
    // 输出调试信息
//    printf("======== print bnf ========\n");
//    for (auto p = bnflist.begin(); p != bnflist.end(); ++p)
//    {
//        BNF* pBnf = *p;
//        pBnf->print_bnf();
//    }

}

void LR_parser::AddBNF(CHAR* data,int flen)
{
    // ask the ID name from the lex
    int size = lex->getRuleSize();
    vmap.constSize = size-1;
    for (int i = 1; i< size; ++i) {
        vmap.InsertVt(lex->getRule(i),i);
    }
    BNF_parser bnfparser;
    State* root = bnfparser.Analysis(data,flen);
    bnfparser.printTree();
    bnflist = BNF::BuildAllBNF(root,vmap);
}

int LR_parser::Parse(Grammer_Node* root)
{
    core.setLex(lex);
    core.setBnflist(&bnflist);
    core.setTable(table);
    core.setVMap(&vmap);
    core.setAst(root);
    root = core.Run();
    return 0;
}

void LR_parser::setLex(LexInterface* _lex)
{
    lex = _lex;
}
