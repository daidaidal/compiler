#include "paser.h"
#include <fstream>
#include <map>
#include <stack>
#include <set>
#define BUFFERSIZE 10000

using namespace std;
multimap<string, node*> a; //�ʼ��״̬��
map<string, set<string>> first_set; //first����
set<string> already_in_first;//�Ѿ��õ��ù�make_first��string
typedef multimap<string, node*>::iterator sn_it;
typedef map<string, set<string>>::iterator ss_it;
typedef set<string>::iterator set_it;
char *buffer = new char[BUFFERSIZE];
int colon_judge = 0;
int buffer_count = 0;
int state_count = 0;
multimap<string, node*> state_map[100]; //�п��ܲ��ǿ����������õ�ַ������֤
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
		//�½��ڵ�
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
//�ݹ�汾
set<string> make_first(string s)
{
	//�ս���ű�
	//����Ҳ����������Ϊ����ĸ�ľ����ս��
	already_in_first.insert(s);
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
		//��ÿ���ĵ�һ���ӽ�ȥ,������ܳ��ֿյĻ��ͰѺ���ļӽ�ȥ while(1) if ͨ��string �޳���һ�� �ڶ��� ������ n��
		string first = k->second->p.substr(k->second->p.find("@") + 1, k->second->p.find(" ", k->second->p.find("@"))-1);
		//cout << "first_word:" << first << 111 << endl;
		if (already_in_first.find(first) != already_in_first.end())
			continue;
		set<string> temp = make_first(first);
		for (set_it m = temp.begin(); m != temp.end(); m++)
			f_s->second.insert(*m);
	}
	return f_s->second;
}

void closure(node* n)
{
	//�հ�map
	multimap<string, node*> temp_map;
	stack<node*> stackl;
	set<string> setl; //���������Ƿ��Ѿ�����հ�
	stackl.push(n); 
	//setl.insert(n->s);
	while (!stackl.empty())
	{
		node * n = stackl.top();
		//�����ǰ�����n->sΪ ͷ�ıհ�

		//���������ż�
		set<string> insert_symbol;
		//��n�ӵ�map��
		temp_map.insert(pair<string, node*>(n->s, n));
		stackl.pop();
		string first = n->p.substr(n->p.find("@") + 1, n->p.find(" ", n->p.find("@"))-1);
		string second;
		if (n->p.find(" ", n->p.find("@")) == -1)
			insert_symbol = n->symbol;
		else
		{
			second = n->p.substr(n->p.find(" ", n->p.find("@")) + 1, n->p.find(" ", n->p.find(" ", n->p.find("@"))));
			ss_it first_second = first_set.find(second);
			if (first_second != first_set.end())
			{
				set<string>::iterator set_it;
				for (set_it = first_second->second.begin(); set_it != first_second->second.end(); set_it++)
					insert_symbol.insert(*set_it);
			}
			else
				insert_symbol.insert(second);
		}

		if (first == "")
			continue;
		if (setl.find(first) == setl.end())
		{
			pair<sn_it, sn_it> ret = a.equal_range(first);
			setl.insert(first);
			for (sn_it k = ret.first; k != ret.second; k++)
			{
				//J=J��{[B��.��,b]|[A����.B��,a]��J, b��FIRST(��a)}	
				node * temp_n = k->second;
				temp_n->symbol=insert_symbol;//��insert_symbol��temp_n->symbol�ϲ�����
				stackl.push(temp_n);
			}
		}
	}
	state_map[state_count] = temp_map;
	state_count++;
}


void main()
{
	standardize();
	make_first("A");
	for (ss_it lll = first_set.begin(); lll != first_set.end(); lll++)
		if (lll->second.size() == 0)
			make_first(lll->first);
	pair<sn_it, sn_it> g = a.equal_range("A");
	cout<<"��׼����"<<endl;
	for (sn_it lll = a.begin(); lll != a.end(); lll++)
		cout << lll->first << "->" << lll->second->p << endl;
	//��������ʽ��ʼ����������
	for (sn_it temp = g.first; temp != g.second; temp++)
	{
		temp->second->symbol.insert("#");
		closure(temp->second);
	}
	
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
/*����
1.state_count
2.state_map���ս��������first_set���ս��������һ��*/

