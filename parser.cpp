#include "paser.h"
#include <fstream>
#include <map>
#include <stack>
#include <set>
#include <list>
#include <iostream>
#define BUFFERSIZE 10000

using namespace std;
typedef multimap<string, node*>::iterator sn_it;
typedef map<string, set<string>>::iterator ss_it;
typedef set<string>::iterator set_it;
typedef map<string, set<node*>>::iterator sset_it;
multimap<string, node*> a; //最开始的状态集
map<string, set<string>> first_set; //first集合
set<string> already_in_first;//已经用调用过make_first的string
set<string> in_recalculate; //判断是否加入过recalculate
list<string> recalculate;
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
//string file_name="temp.txt";  //postfix_expression

string init_word = "s";   //postfix_expression
string file_name = "mywenfa.txt";  //postfix_expression

//string init_word = "S";  
//string file_name = "222.txt";  

multimap<string, node*> state_map[1000]; //有可能不是拷贝而是引用地址，需验证:就是引用地址
set<string> check_in_state[1000];
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
		while (!(isalnum(buffer[buffer_count]) || buffer[buffer_count] == '\'' || buffer[buffer_count] == '\"'))
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
		int_string_count++;
		q_string_int.insert(pair<string,int>(s + "->" + p, int_string_count));
		q_int_string.insert(pair<int, string>(int_string_count, s + "->" + p));

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
int closure(set<node*> input_set)
{
	//闭包map
	multimap<string, node*> temp_map;
	list<node*> stackl;
	set<string> setl; //用来检验是否已经求过闭包

	for (set<node*>::iterator it_temp = input_set.begin(); it_temp != input_set.end(); it_temp++)
	{
		node * temp = *it_temp;
		node * new_temp = new node(temp->s, temp->p);
		new_temp->insert_symbol(temp->symbol);
		stackl.push_back(*it_temp);
		temp_map.insert(pair<string, node*>(new_temp->s, new_temp));
	}
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
				node * new_temp_n=new node(temp_n->s, temp_n->p);
				new_temp_n->insert_symbol(temp_n->symbol);
				new_temp_n->insert_symbol(insert_symbol);//把insert_symbol和temp_n->symbol合并即可
				stackl.push_back(new_temp_n);
				temp_map.insert(pair<string, node*>(new_temp_n->s,new_temp_n));
			}
		}
		else //如果temp_map中有first
		{
			int before;
			pair<sn_it, sn_it> find_first = temp_map.equal_range(first);
			sn_it judge = find_first.first;
			set<string> tryl;
			tryl = judge->second->symbol;
			before = tryl.size();

			tryl.insert(insert_symbol.begin(), insert_symbol.end());

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
	state_count++;
	state_map[state_count] = temp_map;
	return state_count;
}

int find(string s)
{
	int i1=-1;
	for (int i = 1; i <= state_count; i++)
		if (check_in_state[i].find(s) != check_in_state[i].end())
		{
			i1 = i;
			if (check_in_state[i].size() == 1)
				return i;
		}
	return i1;
}

void make_list(set<node*>input_set)
{
	list<set<node*>> listl;
	listl.push_back(input_set);
	//
	while (1)
	{
		if (listl.size()==0)
			break;
		cout << listl.size() << endl;
		set<node*> init_set = listl.front();
		listl.pop_front();
		set<node*> setll;
		setll.clear();
		for (set<node*>::iterator lgg = init_set.begin(); lgg != init_set.end(); lgg++)
		{
			node * l = *lgg;
			string sl = l->s + "->" + l->p;
			for (set<string>::iterator lll = l->symbol.begin(); lll != l->symbol.end(); lll++)
				sl = sl + *lll;
			int findll = find(sl);
			if (findll == -1 || (findll != -1 && l->p.at(l->p.length()-1)=='@' && state_map[findll].size() != 1))
				setll.insert(l);
		}
		//
		if (setll.empty())
			continue;

		map<string, set<node*>> zhuanyibiao;
		zhuanyibiao.clear();
		int state_countl = closure(setll);

		for (sn_it temp_it = state_map[state_countl].begin(); temp_it != state_map[state_countl].end(); temp_it++)
		{
			node * temp = temp_it->second;
			string temps = "";
			for (set<string>::iterator temp2 = temp->symbol.begin(); temp2 != temp->symbol.end(); temp2++)
				temps = temps + *temp2;
			check_in_state[state_countl].insert(temp->s + "->" + temp->p + temps);

			string s = temp->p;
			int at = s.find("@");
			s.at(at) = ' ';
			int check = 0;
			string zhuanyifu = "";
			for (int i = at + 1; i < s.length(); i++)
				if (s.at(i) == ' ')
				{
					s.at(i) = '@';
					check = 1;
					break;
				}
				else
					zhuanyifu = zhuanyifu + s.at(i);
			if (at == 0)
				s = s.substr(1);
			if (zhuanyifu == "")
				continue;
			if (check == 0)
				s = s + '@';

			//
			int findl = find(temp->s + "->" + s + temps);
			if (findl == -1 || (findl!=-1 && check == 0 && state_map[findl].size() != 1))
			{
				sset_it diedai = zhuanyibiao.find(zhuanyifu);
				node * temp_node = new node(temp->s, s);
				temp_node->insert_symbol(temp->symbol);
				if (diedai == zhuanyibiao.end())
				{
					set<node*> temp_set;
					temp_set.insert(temp_node);
					zhuanyibiao.insert(pair<string, set<node*>>(zhuanyifu, temp_set));
				}
				else
					diedai->second.insert(temp_node);
			}
			

		}
		//
		for (sset_it gg = zhuanyibiao.begin(); gg != zhuanyibiao.end(); gg++)
			listl.push_back(gg->second);

	}
	return;
}

int find_all(set<node*> ll)
{
	map<int, int> who_is_more;
	for (set<node*>::iterator gg = ll.begin(); gg != ll.end(); gg++)
	{
		node * temp = *gg;
		string temps = "";
		for (set<string>::iterator temp2 = temp->symbol.begin(); temp2 != temp->symbol.end(); temp2++)
			temps = temps + *temp2;
		if (ll.size() == 1)
			return find(temp->s + "->" + temp->p + temps);
		for (int i = 1; i <= state_count; i++)
			if (check_in_state[i].find(temp->s + "->" + temp->p + temps) != check_in_state[i].end())
			{
				map<int, int>::iterator findll = who_is_more.find(i);
				if (findll == who_is_more.end())
					who_is_more.insert(pair<int, int>(i, 1));
				else
				{
					findll->second++;
					if (findll->second == ll.size())
						return findll->first;
				}
			}
	}
	return 1;
}

void make_list3()
{
	int state_count_temp = 1;
	while (state_count_temp <= state_count)
	{
		map<string, set<node*>> zhuanyibiao;
		zhuanyibiao.clear();
		string zhuanyifu = "";
		int findl;
		for (sn_it temp_it = state_map[state_count_temp].begin(); temp_it != state_map[state_count_temp].end(); temp_it++)
		{
			node * temp = temp_it->second;
			string temps = "";
			for (set<string>::iterator temp2 = temp->symbol.begin(); temp2 != temp->symbol.end(); temp2++)
				temps = temps + *temp2;

			string s = temp->p;
			int at = s.find("@");
			s.at(at) = ' ';
			int check = 0;
			zhuanyifu = "";
			for (int i = at + 1; i < s.length(); i++)
				if (s.at(i) == ' ')
				{
					s.at(i) = '@';
					check = 1;
					break;
				}
				else
					zhuanyifu = zhuanyifu + s.at(i);
			if (at == 0)
				s = s.substr(1);

			if (zhuanyifu == "")//规约
			{
				s.replace(s.length() - 1, 1, "");
				int ll = q_string_int[temp->s + "->" + s];
				for (set<string>::iterator lg = temp->symbol.begin(); lg != temp->symbol.end(); lg++)
				{
					map<string, string>::iterator judgegg;
					judgegg = go.find("" + to_string(state_count_temp) + " " + *lg);
					if (judgegg != go.end())
						if (judgegg->second != "r" + to_string(ll))
							judgecount++;
					go.insert(pair<string, string>("" + to_string(state_count_temp) + " " + *lg, "r" + to_string(ll)));
				}

			}
			else
			{
				if (check == 0)
					s = s + '@';
				findl = find(temp->s + "->" + s + temps);

				map<string, string>::iterator judgegg;
				judgegg = go.find("" + to_string(state_count_temp) + " " + zhuanyifu);
				if (judgegg != go.end())
					if (judgegg->second != "" + to_string(findl))
						judgecount++;


				sset_it diedai = zhuanyibiao.find(zhuanyifu);
				node * temp_node = new node(temp->s, s);
				temp_node->insert_symbol(temp->symbol);
				if (diedai == zhuanyibiao.end())
				{
					set<node*> temp_set;
					temp_set.insert(temp_node);
					zhuanyibiao.insert(pair<string, set<node*>>(zhuanyifu, temp_set));
				}
				else
					diedai->second.insert(temp_node);

			}
		}
		for (sset_it gg = zhuanyibiao.begin(); gg != zhuanyibiao.end(); gg++)
		{
			int all = find_all(gg->second);
			go.insert(pair<string, string>("" + to_string(state_count_temp) + " " + gg->first, "" + to_string(all)));
		}
		state_count_temp++;
	}
}

void make_list2()
{
	int state_count_temp = 1;
	while (state_count_temp <= state_count)
	{
		for (sn_it temp_it = state_map[state_count_temp].begin(); temp_it != state_map[state_count_temp].end(); temp_it++)
		{
			node * temp = temp_it->second;
			string temps = "";
			for (set<string>::iterator temp2 = temp->symbol.begin(); temp2 != temp->symbol.end(); temp2++)
				temps = temps + *temp2;

			string s = temp->p;
			int at = s.find("@");
			s.at(at) = ' ';
			int check = 0;
			string zhuanyifu = "";
			for (int i = at + 1; i < s.length(); i++)
				if (s.at(i) == ' ')
				{
					s.at(i) = '@';
					check = 1;
					break;
				}
				else
					zhuanyifu = zhuanyifu + s.at(i);
			if (at == 0)
				s = s.substr(1);
			


			if (zhuanyifu == "")//规约
			{
				s.replace(s.length() - 1, 1, "");
				int ll = q_string_int[temp->s + "->" + s];
				for (set<string>::iterator lg = temp->symbol.begin(); lg != temp->symbol.end(); lg++)
				{
					map<string, string>::iterator judgegg;
					judgegg = go.find("" + to_string(state_count_temp) + " " + *lg);
					if (judgegg != go.end())
						if(judgegg->second!= "r" + to_string(ll))
							judgecount++;
					go.insert(pair<string, string>("" + to_string(state_count_temp) + " " + *lg, "r" + to_string(ll)));
				}

			}
			else
			{
				if (check == 0)
					s = s + '@';
				int findl = find(temp->s + "->" + s + temps);

				map<string, string>::iterator judgegg;
				judgegg = go.find("" + to_string(state_count_temp) + " " + zhuanyifu);
				if (judgegg != go.end())
					if (judgegg->second != "" + to_string(findl))
						judgecount++;
					
				go.insert(pair<string, string>(""+to_string(state_count_temp) + " " + zhuanyifu, "" + to_string(findl)));
			}
		}
		state_count_temp++;
	}
}

void main()
{
	set<node*>input_set_init;
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
		input_set_init.insert(temp->second);
	make_list(input_set_init);
	make_list3();
	cout << endl << "closure" << endl;
	for (int i = 1; i <= state_count; i++)
	{
		cout << "closure:" << i << endl;
		for (sn_it ii = state_map[i].begin(); ii != state_map[i].end(); ii++)
		{
			string sss="///";
			set<string>::iterator mmm;
			for (mmm = ii->second->symbol.begin(); mmm != ii->second->symbol.end(); mmm++)
				sss = sss + " " + *mmm;
			cout << ii->second->s << "->" << ii->second->p << "  " << sss << endl;
		}
		cout << endl;
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


