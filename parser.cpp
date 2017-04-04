#include "paser.h"
#include <fstream>
#include <map>
#include <stack>
#include <set>
#include <list>
#define BUFFERSIZE 10000

using namespace std;
multimap<string, node*> a; //�ʼ��״̬��
map<string, set<string>> first_set; //first����
set<string> already_in_first;//�Ѿ��õ��ù�make_first��string
typedef multimap<string, node*>::iterator sn_it;
typedef map<string, set<string>>::iterator ss_it;
typedef set<string>::iterator set_it;
set<string> in_recalculate; //�ж��Ƿ�����recalculate
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
		string first = k->second->p.substr(k->second->p.find("@") + 1, k->second->p.find(" ", k->second->p.find("@")) - 1);
		//cout << "first_word:" << first << 111 << endl;
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
void closure(node* n)
{
	//�հ�map
	multimap<string, node*> temp_map;
	list<node*> stackl;
	set<string> setl; //���������Ƿ��Ѿ�����հ�
	stackl.push_back(n);
	//setl.insert(n->s);
	temp_map.insert(pair<string, node*>(n->s,n));
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
		if (first == "")
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
		//if(temp_map.find(first)==temp_map.end())//�����û����հ� =====������ǰ��֤��������
		{
			cout << "first://" << first << endl;
			pair<sn_it, sn_it> ret = a.equal_range(first);
			setl.insert(first);
			for (sn_it k = ret.first; k != ret.second; k++)
			{
				//J=J��{[B��.��,b]|[A����.B��,a]��J, b��FIRST(��a)}	
				node * temp_n = k->second; //
				temp_n->symbol.insert(insert_symbol.begin(), insert_symbol.end());//��insert_symbol��temp_n->symbol�ϲ�����
				stackl.push_back(temp_n);
				temp_map.insert(pair<string, node*>(temp_n->s, temp_n));
			}
		}
		else //���temp_map����first
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
	cout<<"��׼����"<<endl;
	for (sn_it lll = a.begin(); lll != a.end(); lll++)
		cout << lll->first << "->" << lll->second->p << endl;
	//��������ʽ��ʼ����������
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
/*����
1.state_count
2.state_map���ս��������first_set���ս��������һ��
3.������p187��������*/


