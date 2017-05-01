#include "lex.h"
#include<iostream>
#include<string>
#include <fstream> 
#include <set>
#include <fstream>
#include <map>
#include <stack>
#include <set>
#include <iostream>
#define BUFFERSIZE 10000
using namespace std;
class node {
public:
	string s;
	string p;
	set<string>symbol;
	node *next = NULL;
	node(string s, string p)
	{
		this->s = s;
		this->p = p;
	}
	void insert_symbol(set<string>symbol)
	{
		this->symbol.insert(symbol.begin(), symbol.end());
	}

};
class gloable_variablep {
public:
	map<string, int>duizhao;
	multimap<string, node*> a; //最开始的状态集
	map<string, set<string>> first_set; //first集合
	set<string> already_in_first;//已经用调用过make_first的string
	set<string> in_recalculate; //判断是否加入过recalculate
	list<string> recalculate;
	set<string>null_set;
	map<string, int> q_string_int;
	map<int, string> q_int_string;
	int int_string_count = 0;
	map<string, string>go;
	char *buffer = new char[BUFFERSIZE];
	int colon_judge = 0;
	int buffer_count = 0;
	int state_count = 0;
	int judge_first_count = 0;
	int judge_end = 0;
	int judge_count = 0;
	int judgecount = 0; //试验文法后删除

	//string init_word = "s0";   //postfix_expression
	//string file_name="bookwenfa.txt";  //postfix_expression

	string init_word = "s";   //postfix_expression
	string file_name = "mywenfa.txt";  //postfix_expression

	//string file_name = "test_first.txt";

    //string init_word = "S";  
	//string file_name = "yuanshiwenfa.txt";  

	multimap<string, node*> state_map[10000]; //有可能不是拷贝而是引用地址，需验证:就是引用地址
	set<string> check_in_state[10000];
	node *start = new node("", "");
	node *pn = NULL;
};

class parser {
public:
	void make_duizhao();
	int s();
	void standardize();
	set<string> make_first(string s);
	void first();
	int closure(set<node*> input_set);
	//int find(string s);
	//int find_all1(set<node*> ll);
	int find_all2(set<node*> ll);
	void make_list(set<node*>input_set);
	void make_list3();
	gloable_variablep * mainfunction();
};


