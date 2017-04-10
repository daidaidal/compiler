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
multimap<string, node*> a; //�ʼ��״̬��
map<string, set<string>> first_set; //first����
set<string> already_in_first;//�Ѿ��õ��ù�make_first��string
set<string> in_recalculate; //�ж��Ƿ�����recalculate
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
int judgecount = 0; //�����ķ���ɾ��

//string init_word = "s0";   //postfix_expression
//string file_name="temp.txt";  //postfix_expression

string init_word = "s";   //postfix_expression
string file_name = "mywenfa.txt";  //postfix_expression

//string init_word = "S";  
//string file_name = "222.txt";  

multimap<string, node*> state_map[10000]; //�п��ܲ��ǿ����������õ�ַ������֤:�������õ�ַ
set<string> check_in_state[10000];
node *start = new node("", "");
node *p = NULL;
//��׼������
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
		//�½��ڵ�
		int_string_count++;
		q_string_int.insert(pair<string,int>(s + "->" + p, int_string_count));
		q_int_string.insert(pair<int, string>(int_string_count, s + "->" + p));

		p = '@' + p;
		start->next = new node(s, p);
		start = start->next;
		a.insert(pair<string, node*>(s, start));
		set<string> y;
		first_set.insert(pair<string,set<string>>(s, y)); //�п���������ͬһ��y
	}
	return 1;
}
void standardize()
{
	int count = 0;//��¼�ڶ��ٸ��ڵ�
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
//�ݹ�汾
set<string> make_first(string s)
{
	//�ս���ű�
	//����Ҳ����������Ϊ����ĸ�ľ����ս��
	if (in_recalculate.find(s) == in_recalculate.end()) //��������recalculate
	{
		in_recalculate.insert(s);
		recalculate.push_front(s);
	}
	already_in_first.insert(s);
	ss_it f_s = first_set.find(s);
	pair<sn_it, sn_it> f = a.equal_range(s);
	for (sn_it k = f.first; k != f.second; k++)
	{
		//��ÿ���ĵ�һ���ӽ�ȥ,������ܳ��ֿյĻ��ͰѺ���ļӽ�ȥ while(1) if ͨ��string �޳���һ�� �ڶ��� ������ n��
		int at = k->second->p.find("@");
		//string first = k->second->p.substr(at + 1, k->second->p.find(" ", at) - 1);
		string first = "";
		for (int lll = at + 1; lll < k->second->p.length(); lll++)
		{
			if (k->second->p.at(lll) == ' ')
				break;
			first = first + k->second->p.at(lll);
		}
		set<string> temp;
		if (first_set.find(first) == first_set.end())
		{
			f_s->second.insert(first);
			continue;
		}
		if (already_in_first.find(first) != already_in_first.end()) //����Ѿ����make_first(first)
		{
			//��Ҫ���½��м����
			if (judge_first_count == 0)
			{
				if (in_recalculate.find(first) != in_recalculate.end())//��������recalculate
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
	//make_first(init_word); //��ʼ��
	ss_it l;
	for (l = first_set.begin(); l != first_set.end(); l++)
		make_first(l->first);
	judge_first_count = 1; //�����ڲ��γ�ջ����
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
	//�հ�map
	multimap<string, node*> temp_map;
	list<node*> stackl;
	set<string> setl; //���������Ƿ��Ѿ�����հ�

	for (set<node*>::iterator it_temp = input_set.begin(); it_temp != input_set.end(); it_temp++)
	{
		node * temp = *it_temp;
		node * new_temp = new node(temp->s, temp->p);
		new_temp->insert_symbol(temp->symbol);
		stackl.push_back(new_temp);
		temp_map.insert(pair<string, node*>(new_temp->s, new_temp));
	}
	set<string> insert_symbol;
	while (!stackl.empty())
	{
		node * n = stackl.front();
		//�����ǰ�����n->sΪ ͷ�ıհ�
		//���������ż�
		//��n�ӵ�map��
		insert_symbol.clear();
		stackl.pop_front();
	
		int at = 0;
		at = n->p.find("@");
		string first = "";
		int i = 0;
		int judge_cc = 0; //��������Ƿ����ս������ս��
		for (i = at + 1; i < n->p.size(); i++)
		{
			if (n->p.at(i) == ' ')
			{
				judge_cc = 1;
				break;
			}
			first = first + n->p.at(i); 
		}
		if (first=="")
			continue;
		if (first_set.find(first) == first_set.end())
			continue;

		string second = "";
		if (judge_cc == 0) //�������û�з��ս��
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
			if (first_second != first_set.end()) //����Ƿ��ս��
			{
				set<string>::iterator set_it;
				insert_symbol.insert(first_second->second.begin(), first_second->second.end());
			}
			else //������ս��
				insert_symbol.insert(second);
		}


		if (setl.find(first) == setl.end())
		{
			//cout << "first://" << first << endl;
			pair<sn_it, sn_it> ret = a.equal_range(first);
			setl.insert(first);
			for (sn_it k = ret.first; k != ret.second; k++)
			{
				//J=J��{[B��.��,b]|[A����.B��,a]��J, b��FIRST(��a)}	
				node * temp_n = k->second; //
				node * new_temp_n=new node(temp_n->s, temp_n->p);
				new_temp_n->insert_symbol(temp_n->symbol);
				new_temp_n->insert_symbol(insert_symbol);//��insert_symbol��temp_n->symbol�ϲ�����
				stackl.push_back(new_temp_n);
				temp_map.insert(pair<string, node*>(new_temp_n->s,new_temp_n));
			}
		}
		else //���temp_map����first
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
					if (j->second->p.at(0) != '@')
						continue;
					j->second->symbol.insert(tryl.begin(), tryl.end());
					stackl.push_back(j->second);
					//judge_count++;
					//cout << judge_count << endl;
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
		int lll = 0;
		for (int i = 1; i <= state_count; i++)
			if (check_in_state[i].find(temp->s + "->" + temp->p + temps) != check_in_state[i].end())
			{
				lll++;
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
		if (lll == 0)
			return -1;
	}
	return -2;
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
		set<node*> init_set = listl.front();
		listl.pop_front();
		int gggg = find_all(init_set);

		map<string, set<node*>> zhuanyibiao;
		zhuanyibiao.clear();
		int state_countl = closure(init_set);
		if (gggg != -1 && gggg != -2)
		{
			if (state_map[gggg].size() == state_map[state_countl].size())
			{
				state_count--;
				continue;
			}
		}

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
		//
		for (sset_it gg = zhuanyibiao.begin(); gg != zhuanyibiao.end(); gg++)
			listl.push_back(gg->second);

	}
	return;
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

			if (zhuanyifu == "")//��Լ
			{
				if (state_map[state_count_temp].size() != 1)
					continue;
				s.replace(s.length() - 1, 1, "");
				map<string,int>::iterator lol = q_string_int.find(temp->s + "->" + s);
				if (lol == q_string_int.end())
					cout << "baozhala" << endl;
				int ll = lol->second;
				for (set<string>::iterator lg = temp->symbol.begin(); lg != temp->symbol.end(); lg++)
				{
					map<string, string>::iterator judgegg;
					judgegg = go.find(to_string(state_count_temp) + " " + *lg);
					if (judgegg != go.end())
						//if (judgegg->second != "r" + to_string(ll))
							judgecount++;
					go.insert(pair<string, string>(to_string(state_count_temp) + " " + *lg, "r" + to_string(ll)));
				}

			}
			else
			{
				if (check == 0)
					s = s + '@';
				findl = find(temp->s + "->" + s + temps);

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
			int all = 0;
			all = find_all(gg->second);
			if (all == -1)
				judgecount++;
			
			map<string, string>::iterator judgegg;
			judgegg = go.find(to_string(state_count_temp) + " " + gg->first);
			if (judgegg != go.end())
				//if (judgegg->second != to_string(all))
					judgecount++;
			go.insert(pair<string, string>(to_string(state_count_temp) + " " + gg->first,to_string(all)));
		}
		state_count_temp++;
	}
}

void main_paser()
{
	set<node*>input_set_init;
	standardize();
	first();

	pair<sn_it, sn_it> g = a.equal_range(init_word);
	cout<<"��׼����"<<endl;
	for (sn_it lll = a.begin(); lll != a.end(); lll++)
		cout << lll->first << "->" << lll->second->p << endl;
	//��������ʽ��ʼ����������
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
	
	cout << endl;
	return;
}
