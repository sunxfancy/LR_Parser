#include "BNF_parser.h"
#include "afx.h"
#include <VMap.h>
#include <stdio.h>
#include <ctype.h>

BNF_parser::BNF_parser()
{

}

BNF_parser::BNF_parser(const BNF_parser& other)
{

}

BNF_parser::~BNF_parser()
{

}

BNF_parser& BNF_parser::operator=(const BNF_parser& other)
{

}

bool BNF_parser::operator==(const BNF_parser& other)
{

}

State* BNF_parser::getState(){
    
}

int BNF_parser::getSize(){
    return state_size;
}

void BNF_parser::blank()
{
    while ((p<size)&&(!isgraph(data[p])))
	{
		++p;
        if (data[p]=='#') {
            while ((p<size) && (data[p]!='\n'))
            {
                printf("a%C",data[p]);
                ++p;
            }
        }
	}
}

CHAR BNF_parser::getNextalpha()
{
	blank();
	return data[p];
}

CHAR* BNF_parser::constword()
{
	int s,i,q;
	CHAR* word;
	blank();
	//记录下初始指针位置
	// println("constword");	

	q = p;
	while ((p<size) && (data[p]!='\"'))
	{
		++p;
	}
	if (p==size) return NULL;

	s = p - q;
	// printf("constword-len: %d\n", s);
	//拷贝单词
	word = new CHAR[s+1];
	for (i = 0; i < s; ++i)
	{
		word[i] = data[q+i];
	}
	word[s] = 0;

	return word;
}

CHAR* BNF_parser::script()
{
    int s,i,q;
    CHAR* word;
    //记录下初始指针位置
    q = p;
    while ((p<size-1) && !((data[p]=='}') && (data[p+1]=='}')) )
    {
        ++p;
    }
    if (p==size-1) return NULL;

    s = p - q;
    // printf("constword-len: %d\n", s);
    //拷贝单词
    word = new CHAR[s+1];
    for (i = 0; i < s; ++i)
    {
        word[i] = data[q+i];
    }
    word[s] = 0;

    return word;
}


CHAR* BNF_parser::keyword()
{
	int s,i,q;
	CHAR* word;
	blank();
	//记录下初始指针位置
	// println("keyword");	
	q = p;
	//初始特判只能为字母或'_'
	if (isalpha(data[p])||(data[p]=='_'))
		++p;
	else return NULL;
	while (isalnum(data[p]) || (data[p]=='_'))
	{
		++p;
	}

	s = p - q;
	// printf("keyword-len: %d\n", s);
	//拷贝单词
	word = new CHAR[s+1];
	// println("malloc!");
	for (i = 0; i < s; ++i)
	{
		word[i] = data[q+i];
	}
	word[s] = 0;
	// printf("Word:%S\n", word);
	return word;
}




bool BNF_parser::Analysis_word(const CHAR* word)
{
	int t,i;
	int word_len = STRLEN(word);
	// printf("word-len:%d %S\n",word_len,word);
	blank();

	if (word_len>size-p) return false;
	for (i = 0; i < word_len; ++i,++p)
	{
		if (word[i]!=data[p])
			return false;
	}
	// println("\nword");
	return true;
}

State* BNF_parser::Analysis_const()
{
	CHAR* state_const = NULL;
	State* s = new State();
	s->state_type = constant;
	blank();
    println(_T("const"));
	if (!Analysis_word(_T("\""))) goto Error_const;
	state_const = constword();
	
	printf("keyword: ");
    println(state_const);
	
	if (!Analysis_word(_T("\""))) goto Error_const;
	
    println(_T("const done"));
	s->state_const = state_const;
	return s;
Error_const:
    println(_T("Error_const"));
	return NULL;
	if (state_const!=NULL) delete[] state_const;
}

State* BNF_parser::Analysis_state()
{
	CHAR* state_class;
	CHAR* state_var;
	State* s;
	if (!Analysis_word(_T("<"))) goto Error_state;
	state_class = keyword();
	
	printf("state_var is ");
	println(state_class);
	
	if (getNextalpha()!=':')
	{
		if (!Analysis_word(_T(">"))) goto Error_state;
		s = new State();
		s->state_type = statement;
		s->state_class = state_class;
		return s;
	}
	++p;
	state_var = keyword();
	
	printf("state_var is ");
	println(state_var);
	
	if (!Analysis_word(_T(">"))) goto Error_state;
	s = new State();
	s->state_type = statement;
	s->state_class = state_class;
	s->state_var = state_var;
	return s;

Error_state:
    println(_T("Error for state"));
	delete[] state_class;
	delete[] state_var;
	return NULL;
}

State* BNF_parser::Analysis_temporality()
{
	State* root = new State();
	root->state_type = temporality;
	State* ans = NULL;
	State* subroot = new State();
	subroot->isList = false;
	subroot->state_type = temporality;
	// 这里subroot是为了解决一条语句中可能有'|'符号的问题
	// 通过subroot指针的可修改性,可以使得里面的元素能控制父层节点的增加
	root->AddChildrenState(subroot);
	if (!Analysis_word(_T("("))) goto Error_T;
	do
	{
        ans = Analysis_select(subroot,ans);
		if (ans==NULL) goto Error_T;
	} while (((long)ans)!=-1);
	if (!Analysis_word(_T(")"))) goto Error_T;

	return root;
Error_T:
    println(_T("Error_temporality"));
	delete root;
	return NULL;
}

State* BNF_parser::Analysis_terminal()
{
	CHAR* state_class;
	CHAR* state_var;
	State* s;
	if (!Analysis_word(_T("["))) goto Error_terminal;
	state_class = keyword();
	
	printf("terminal is ");
	println(state_class);
	
	if (getNextalpha()!=':')
	{
		if (!Analysis_word(_T("]"))) goto Error_terminal;
		s = new State();
        s->state_type = terminal;
		s->state_class = state_class;
		return s;
	}
	++p;
	state_var = keyword();
	
	printf("state_var is ");
	println(state_var);
	
	if (!Analysis_word(_T("]"))) goto Error_terminal;
	s = new State();
	s->state_type = terminal;
	s->state_class = state_class;
	s->state_var = state_var;
	return s;

Error_terminal:
    println(_T("Error for terminal"));
	delete[] state_class;
	delete[] state_var;
	return NULL;
}


void BNF_parser::Analysis_script(State* root) {
    CHAR* scriptdata = NULL;
    blank();
    if (!Analysis_word(_T("{{"))) goto Error_script;
    scriptdata = script();

    printf("script: ");
    println(scriptdata);

    if (!Analysis_word(_T("}}"))) goto Error_script;

    println(_T("script done"));
    root->script = scriptdata;
    return;
Error_script:
    println(_T("Error_script"));
}




State* BNF_parser::Analysis_select(State*& root,State* last)
{
	State* ans;
	CHAR next = getNextalpha();
	printf("Next:%C\n", next);
	switch(next)
	{
		case '\"' : ans = Analysis_const();
			break;
		case '(' : ans = Analysis_temporality();
			break;
		case ')' : ans = (State*)-1;
            break;
        case '{' : Analysis_script(root);
            break;
		case '<' : ans = Analysis_state();
            break;
		case 'e' : 
			ans = new State();
			ans->state_type = epsilon;
            ++p;
			break;
		case '[' : ans = Analysis_terminal();
			break;
		case '|' : 
			{
				State* pTemp = new State();
                pTemp->state_type = temporality;
                root->AddBrotherState(pTemp);
                root = pTemp;

				if (last==NULL) goto Error_select;
				++p;
                return last;
			}
        case ';' : {
                println(_T("It's a ';'"));
                ++p;
                ans = (State*)-1;
                break;
        }
		case '*' :
			{
				if (last==NULL) goto Error_select;
				last->Repeatable = 3;
				++p;
				break;
			}
		case '+' :
			{
				if (last==NULL) goto Error_select;
				last->Repeatable = 2;
				++p;
				break;
			}
		case '?' :
			{
				if (last==NULL) goto Error_select;
				last->Repeatable = 1;
				++p;
				break;
			}
		default:
			goto Error_select;
	}
	if (ans==NULL)
		goto Error_select;

	if ((ans!=NULL)&&(((long)ans)!=-1))
        root->AddChildrenState(ans);
	return ans;

Error_select:
    println(_T("Error_select"));
	if (ans!=NULL) delete ans;
	return NULL;
}

State* BNF_parser::Analysis_list()
{
	State* root = NULL;
	State* ans = NULL;
	State* subroot = new State();

	subroot->state_type = temporality;
    println(_T("list state"));
	root = Analysis_state();
    root->isList = false;
	// 这里ans是为了解决一条语句中可能有'|'符号的问题
	// 通过ans指针的可修改性,可以使得里面的元素能控制父层节点的增加
	root->AddChildrenState(subroot);
	
	if (root==NULL) goto Error;
	if (!Analysis_word(_T("="))) goto Error;
    do {
        ans = Analysis_select(subroot,ans);
		if (ans==NULL) goto Error;
	} while (((long)ans)!=-1);
	return root;
Error:
    println(_T("Error for list"));
	if (root!=NULL)	delete root;
	if (ans!=NULL) delete ans;
	return NULL;
}


State* BNF_parser::Analysis(const CHAR* _data,int flen)
{
	int i;
	State* root = new State();
	State* ans = NULL;
	data = _data;

	p = 0;
    println(_T("run"));
    size = flen;

	while (p < size)
	{
        println(_T("begin a statemnet"));
		ans = Analysis_list();
		if (ans==NULL) return root;
		root->AddChildrenState(ans);
        println(_T("A statement is done"));
		blank();
	}
	
	state_vector = root;
	return root;

Error_A:
	delete root;
	return NULL;
}



void BNF_parser::printNode(State* s,int d)
{
    if (s == NULL) return;
    
    for (int i = 0; i<d; ++i)
		printf("    ");
    if (s->state_type == statement || s->state_type == terminal)
        printf("%S %S",s->state_class,s->state_var);
    if (s->state_type == constant)
        printf("%S",s->state_const);
    if (s->state_type == temporality)
		printf("temp node");
    if (s->isList == false)
		printf(" (not list)");
    if (s->Repeatable >0 )
    {
		if (s->Repeatable == 1) printf(" ?");
		if (s->Repeatable == 2) printf(" +");
		if (s->Repeatable == 3) printf(" *");
    }
   
	
    printf("\n");
    printNode(s->children,d+1);
    printNode(s->brother,d);
}



void BNF_parser::printTree()
{
    printNode(state_vector,0);
}

