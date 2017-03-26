#include "paser.h"
#include <fstream>
#include <map>
#include <stack>
#include <set>
#define BUFFERSIZE 10000

using namespace std;
multimap<string, node*> a; //最开始的状态集
map<string, set<string>> first_set; //first集合
typedef multimap<string, node*>::iterator sn_it;
typedef map<string, set<string>>::iterator ss_it;
typedef set<string>::iterator set_it;
char *buffer = new char[BUFFERSIZE];
int colon_judge = 0;
int buffer_count = 0;
int state_count = 0;
map<string, node*> state_map[100]; //有可能不是拷贝而是引用地址，需验证
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
		set<string> y;
		first_set.insert(pair<string,set<string>>(s, y)); //有可能连接着同一个y
	}
	return 1;
}
void standardize()
{
	int count = 0;//记录第多少个节点
	p = start;
	ifstream in;
	string filename;
	in.open("temp.txt");
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
/*
set<string> first(string beita, set<string>aerfa, set<string>*last)
{

}
*/
//递归版本
set<string> make_first(string s)
{
	//终结符号表
	//如果找不到别的以他为首字母的就是终结符
	//first=本身就是终结符
	//递归函数
	ss_it f_s = first_set.find(s);
	if (f_s == first_set.end())
	{
		set<string> ret;
		ret.insert(s);
		return ret;
	}
	pair<sn_it, sn_it> f = a.equal_range(s);
	for (sn_it k = f.first; k != f.second; k++)
	{
		//把每个的第一个加进去,如果可能出现空的话就把后面的加进去 while(1) if 通过string 剔除第一个 第二个 。。第 n个
		string first = k->second->p.substr(k->second->p.find("@") + 1, k->second->p.find(" ", k->second->p.find("@"))-1);
		set<string> temp = make_first(first);
		for (set_it m = temp.begin(); m != temp.end(); m++)
			f_s->second.insert(*m);
	}
	return f_s->second;
}
//非递归版本
void make_firstset()
{
	string start_string = "postfix_expression";
	string s;
	ss_it f_s = first_set.find(s);
	if (f_s == first_set.end())
	{
		set<string> ret;
		ret.insert(s);
		return ret;
	}
	pair<sn_it, sn_it> f = a.equal_range(s);
	for (sn_it k = f.first; k != f.second; k++)
	{
		//把每个的第一个加进去,如果可能出现空的话就把后面的加进去 while(1) if 通过string 剔除第一个 第二个 。。第 n个
		string first = k->second->p.substr(k->second->p.find("@") + 1, k->second->p.find(" ", k->second->p.find("@")) - 1);
		set<string> temp = make_first(first);
		for (set_it m = temp.begin(); m != temp.end(); m++)
			f_s->second.insert(*m);
	}
	return f_s->second;
}
/*
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
		string second;
		if (n->p.find("", n->p.find("@")) == -1)
			insert_symbol = n->symbol;
		else
		{
			second = n->p.substr(n->p.find(" ", n->p.find("@")) + 1, n->p.find(" ", n->p.find(" ", n->p.find("@"))));
			first(second, n->symbol, &insert->symbol);
		}

		if (first == "")
			continue;
		if (setl.find(first) != setl.end())
		{
			pair<sn_it, sn_it> ret = a.equal_range(first);
			setl.insert(first);
			for (sn_it k = ret.first; k != ret.second; k++)
			{
				//J=J∪{[B→.η,b]|[A→α.Bβ,a]∈J, b∈FIRST(βa)}	
				node * temp_n = k->second;
				temp_n->symbol.//把insert_symbol和temp_n->symbol合并即可
					stackl.push(k->second);
			}
		}
	}

}
*/

void main()
{
	standardize();
	make_first("postfix_expression");
	
	node *c = p->next;
	while (c != NULL)
	{
		cout << c->s << ":" << c->p << endl;
		c = c->next;
	}
	cout << endl << "first:" << endl;
	for (ss_it itt = first_set.begin(); itt != first_set.end(); itt++)
	{
		cout << endl<<itt->first << ":";
		set<string>::iterator si;
		for (si = itt->second.begin();si != itt->second.end();si++)
			cout << *si << " ";
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

