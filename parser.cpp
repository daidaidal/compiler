#include "paser.h"
#include <fstream>
#include <map>
#include <stack>
#include <set>
#include <list>
#define BUFFERSIZE 10000

using namespace std;
multimap<string, node*> a; //最开始的状态集
map<string, set<string>> first_set; //first集合
set<string> already_in_first;//已经用调用过make_first的string
typedef multimap<string, node*>::iterator sn_it;
typedef map<string, set<string>>::iterator ss_it;
typedef set<string>::iterator set_it;
set<string> in_recalculate; //判断是否加入过recalculate
list<string> recalculate;
char *buffer = new char[BUFFERSIZE];
int colon_judge = 0;
int buffer_count = 0;
int state_count = 0;
int judge_first_count = 0;
int judge_end = 0;
int judge_count = 0;
//string init_word = "s0";   //postfix_expression
//string file_name="temp.txt";  //postfix_expression

//string init_word = "S";  
string init_word = "declaration_list";  
string file_name = "222.txt";  

multimap<string, node*> state_map[100]; //有可能不是拷贝而是引用地址，需验证
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
	in.open(file_name);
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
//递归版本
set<string> make_first(string s)
{
	//终结符号表
	//如果找不到别的以他为首字母的就是终结符
	if (in_recalculate.find(s) == in_recalculate.end()) //如果加入过recalculate
	{
		in_recalculate.insert(s);
		recalculate.push_front(s);
	}
	already_in_first.insert(s);
	ss_it f_s = first_set.find(s);
	pair<sn_it, sn_it> f = a.equal_range(s);
	for (sn_it k = f.first; k != f.second; k++)
	{
		//把每个的第一个加进去,如果可能出现空的话就把后面的加进去 while(1) if 通过string 剔除第一个 第二个 。。第 n个
		string first = k->second->p.substr(k->second->p.find("@") + 1, k->second->p.find(" ", k->second->p.find("@")) - 1);
		//cout << "first_word:" << first << 111 << endl;
		set<string> temp;
		if (first_set.find(first) == first_set.end())
		{
			f_s->second.insert(first);
			continue;
		}
		if (already_in_first.find(first) != already_in_first.end()) //如果已经算过make_first(first)
		{
			//需要重新进行计算的
			if (judge_first_count == 0)
			{
				if (in_recalculate.find(first) != in_recalculate.end())//如果加入过recalculate
					continue;
				in_recalculate.insert(first);
				recalculate.push_front(first);
			}
			else
			{
				ss_it temp_it = first_set.find(first);
				int before = f_s->second.size();
				f_s->second.insert(temp_it->second.begin(), temp_it->second.end());
				if (f_s->second.size() > before)
					judge_end = 1;
			}
			continue;
		}
		temp = make_first(first);
		f_s->second.insert(temp.begin(), temp.end());
	}
	return f_s->second;
}
void first()
{
	//make_first(init_word); //初始化
	ss_it l;
	for (l = first_set.begin(); l != first_set.end(); l++)
		make_first(l->first);
	judge_first_count = 1; //跳过内部形成栈代码
	while (1)
	{
		judge_end = 0;
		for (list<string>::iterator ttt = recalculate.begin(); ttt != recalculate.end(); ttt++)
			make_first(*ttt);
		if (judge_end == 0)
			break;
	}
}
void closure(node* n)
{
	//闭包map
	multimap<string, node*> temp_map;
	list<node*> stackl;
	set<string> setl; //用来检验是否已经求过闭包
	stackl.push_back(n);
	//setl.insert(n->s);
	temp_map.insert(pair<string, node*>(n->s,n));
	set<string> insert_symbol;
	while (!stackl.empty())
	{
		node * n = stackl.front();
		//如果以前求过以n->s为 头的闭包
		//求搜索符号集
		//将n加到map中
		insert_symbol.clear();
		stackl.pop_front();
		int at = 0;
		at = n->p.find("@");
		string first = "";
		int i = 0;
		int judge_cc = 0; //检验后面是否有终结符或非终结符
		for (i = at + 1; i < n->p.size(); i++)
		{
			if (n->p.at(i) == ' ')
			{
				judge_cc = 1;
				break;
			}
			first = first + n->p.at(i);
		}
		if (first == "")
			continue;
		if (first_set.find(first) == first_set.end())
			continue;

		string second = "";
		if (judge_cc == 0) //如果后面没有非终结符
			insert_symbol.insert(n->symbol.begin(), n->symbol.end());
		else
		{
			at = n->p.find(" ", n->p.find("@"));
			for (int i = at + 1; i < n->p.size(); i++)
			{
				if (n->p.at(i) == ' ')
					break;
				second = second + n->p.at(i);
			}
			ss_it first_second = first_set.find(second);
			if (first_second != first_set.end()) //如果是非终结符
			{
				set<string>::iterator set_it;
				insert_symbol.insert(first_second->second.begin(), first_second->second.end());
			}
			else //如果是终结符
				insert_symbol.insert(second);
		}


		if (setl.find(first) == setl.end()) 
		//if(temp_map.find(first)==temp_map.end())//如果还没求过闭包 =====可以提前验证减少运算
		{
			cout << "first://" << first << endl;
			pair<sn_it, sn_it> ret = a.equal_range(first);
			setl.insert(first);
			for (sn_it k = ret.first; k != ret.second; k++)
			{
				//J=J∪{[B→.η,b]|[A→α.Bβ,a]∈J, b∈FIRST(βa)}	
				node * temp_n = k->second; //
				temp_n->symbol.insert(insert_symbol.begin(), insert_symbol.end());//把insert_symbol和temp_n->symbol合并即可
				stackl.push_back(temp_n);
				temp_map.insert(pair<string, node*>(temp_n->s, temp_n));
			}
		}
		else //如果temp_map中有first
		{
			int before;
			pair<sn_it,sn_it> find_first=temp_map.equal_range(first);
			sn_it judge = find_first.first;
			set<string> tryl;
			tryl = judge->second->symbol;
			before = tryl.size();

			tryl.insert(insert_symbol.begin(),insert_symbol.end());

			if (tryl.size() > before)
				for (sn_it j = find_first.first; j != find_first.second; j++)
				{
					j->second->symbol.insert(tryl.begin(), tryl.end());
					stackl.push_back(j->second);
					judge_count++;
					cout << judge_count << endl;
				}
		}
	}
	state_map[state_count] = temp_map;
	state_count++;

}

void main()
{
	standardize();
	first();
	/*
	for (ss_it lll = first_set.begin(); lll != first_set.end(); lll++)
		if (lll->second.size() == 0)
			make_first(lll->first);
	*/
	pair<sn_it, sn_it> g = a.equal_range(init_word);
	cout<<"标准化："<<endl;
	for (sn_it lll = a.begin(); lll != a.end(); lll++)
		cout << lll->first << "->" << lll->second->p << endl;
	//给最初表达式初始化搜索符号
	for (sn_it temp = g.first; temp != g.second; temp++)
		temp->second->symbol.insert("#");
	for (sn_it temp = g.first; temp != g.second; temp++)
		closure(temp->second);
	
	cout << endl << "closure" << endl;
	for (int i = 0; i < state_count; i++)
	{
		state_map[i];
		for (sn_it ii = state_map[0].begin(); ii != state_map[0].end(); ii++)
		{
			string sss="///";
			set<string>::iterator mmm;
			for (mmm = ii->second->symbol.begin(); mmm != ii->second->symbol.end(); mmm++)
				sss = sss + " " + *mmm;
			cout << ii->second->s << "->" << ii->second->p << "  " << sss << endl;
		}

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
/*问题
1.state_count
2.state_map非终结符数量和first_set非终结符数量不一致
3.与书上p187用例不符*/


