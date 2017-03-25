#include "paser.h"
#include <fstream>
#include <map>
#include <stack>
#include <set>
#define BUFFERSIZE 10000

using namespace std;
multimap<string, node*> a;
typedef multimap<string, node*>::iterator it;
char *buffer = new char[BUFFERSIZE];
int colon_judge = 0;
int buffer_count = 0;
int state_count = 0;
map<string, node*> state_map[100];
node *start = new node("", "");
node *p = NULL;
//标准化处理
int s()
{
	string s = "";
	while (buffer[buffer_count] == ' ' || buffer[buffer_count] == '\t' || buffer[buffer_count] == '\n')
	{
		if (buffer[buffer_count] == EOF)
			return 0;
		buffer_count++;
	}
	while (isalnum(buffer[buffer_count]) || buffer[buffer_count] == '_')
	{
		if (buffer[buffer_count] == EOF) //
			return 0;
		s = s + buffer[buffer_count];
		buffer_count++;
	}
	//string or char
	while (1)
	{
		string p = "";
		while (!(isalnum(buffer[buffer_count]) || buffer[buffer_count] == '\''))
		{
			if (buffer[buffer_count] == EOF)
				return 0;
			if (buffer[buffer_count] == ';')
			{
				buffer_count++;
				return 1;
			}
			buffer_count++;
		}
		while (buffer[buffer_count] != '\n')
		{
			if (buffer[buffer_count] == EOF)
				return 0;
			p = p + buffer[buffer_count];
			buffer_count++;
		}
		buffer_count++;
		//新建节点
		p = '@' + p;
		start->next = new node(s, p);
		start = start->next;
		a.insert(pair<string, node*>(s, start));
	}
	return 1;
}
void standardize()
{
	int count = 0;//记录第多少个节点
	p = start;
	ifstream in;
	string filename;
	in.open("222.txt");
	if (!in)
	{
		cout << "open error" << endl;
	}
	int sum_num = 0;
	while (!in.eof())
	{
		in.read(buffer + sum_num, 1);
		sum_num++;
	}
	sum_num--;
	buffer[sum_num] = EOF;

	//start
	while (s());
	return;
}
void first(string n){}
void closure(node* n)
{
	map<string, node*> temp_map;
	stack<node*> stackl;
	set<string> setl;
	stackl.push(n);
	setl.insert(n->s);
	while (!stackl.empty())
	{
		node * n = stackl.top();
		set<string> insert_symbol;
		//将n加到map中
		temp_map.insert(pair<string, node*>(n->s, n));
		stackl.pop();
		string first = n->p.substr(n->p.find("@") + 1, n->p.find(" ", n->p.find("@")));

		if (n->p.find("", n->p.find("@")) == -1)
		{
			insert_symbol = n->symbol;
		}
		else
			insert_symbol = first(second, a);

		if (first == "")
			continue;
		if (setl.find(first) != setl.end())
		{
			pair<it, it> ret = a.equal_range(first);
			setl.insert(first);
			for (it k = ret.first; k != ret.second; k++)
			{
				//J=J∪{[B→.η,b]|[A→α.Bβ,a]∈J, b∈FIRST(βa)}	
				node * temp_n = k->second;
				temp_n->symbol.//把insert_symbol和temp_n->symbol合并即可
				stackl.push(k->second);
			}
		}
	}
	
}

void main()
{
	standardize();
	node *c = p->next;
	while (c != NULL)
	{
		cout << c->s << ":" << c->p << endl;
		c = c->next;
	}
	/*
	pair<it, it> ret = n->p.equal_range("primary_expression");
	for (it k = ret.first; k != ret.second; k++)
	{
		cout << k->second->p << endl;
		k->second->p = "hahahaha";
	}
	*/
	cout << endl;
	return;
}

