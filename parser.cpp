#include "paser.h"
#include <fstream>
#define BUFFERSIZE 10000
using namespace std;
char *buffer = new char[BUFFERSIZE];
int colon_judge = 0;
node *start = new node("", "", "");
node *p = NULL;
//标准化处理
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
	buffer[sum_num] == EOF;

	//start
	int i = 0;
	while (1)
	{
		string s="";
		while (buffer[i] == ' ' || buffer[i] == '\t' || buffer[i] == '\n')
		{
			if (buffer[i] == EOF)
				return;
			i++;
		}
		while (isalnum(buffer[i]) || buffer[i] == '_')
		{
			if (buffer[i] == EOF)
				return;
			s = s + buffer[i];
			i++;
		}
		//string or char
		while (buffer[i] != ';')
		{
			string p = "";
			while (!isalnum(buffer[i]) || buffer[i] == '\'')
			{
				if (buffer[i] == EOF)
					return;
				i++;
			}
			while (buffer[i]!='\n')
			{
				if (buffer[i] == EOF)
					return;
				p = p + buffer[i];
				i++;
			}
			i++;
			//新建节点
			start->next=new node(s,p,"");
			start=start->next;
		}
		
			
	}
	node *c = p->next;
	while (c->next != NULL)
	{
		cout << c->s << ":" << c->p << endl;
		c = c->next;
	}
}
void closure()
{

}
void parsefunction()
{
	standardize();
	node *c = p->next;
	while (c->next != NULL)
	{
		cout << c->s << ":" << c->p << endl;
		c = c->next;
	}
	cout << endl;
	return;
}

