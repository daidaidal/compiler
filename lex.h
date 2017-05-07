#include "hong.h"
#include<iostream>
#include<string>
#include<tuple>
#include <map>
#include <fstream> 
#include <math.h>
#include <list>
#include <set>
using namespace std;
class gloable_variable {
public:
	char * buffer = new char[8192];
	ifstream in;
	int qian = 8191;
	int hou = 0;
	char * list[1000]; //��ʶ��
	char * string_list[1000]; //string������
	int count_sl = 0;
	char * char_list[1000]; // char������
	int count_cl = 0;
	int countl = 0;
	int boundary = 0; //�Ƿ���˵��߽��߹�
	class errorl *e[100]; //�������� ����������ԭ��
	int count_e = 0;
	int hang = 1; //���������� ������ʾʹ��
	int error_hang; //��������
	int S[50][2];
	int count_s = 0;
	int M[50][2];
	int count_m = 0;
	int B[50][2];
	int count_b = 0;
	int save[10000][2];
};
class lex {
public:
	//���ֺ���
	void read();
	void retract();
	char getcharl();
	char * copytoken();
	int gettoken(char * token);
	void error_handle(int num);
	int install_id(char * token);
	int install_string(char * token);
	int install_char(char * token);
	int change_num(char * token);
	void left(int a[][2], int &count);
	void right(int a[][2], int &count);
	int * token_scan();
	gloable_variable * mainfunction();
};
class matrix_develop {
public:
	int sum_weishu; //��һ��ʼ
	int weishu[10];
	matrix_develop()
	{
		this->sum_weishu =0;
	}
};

class code {
public:
	int num;
	int result;
	string op;
	int arg1;
	int arg2;
	code(int result, string op, int arg1, int arg2)
	{
		this->result = result;
		this->op = op;
		this->arg1 = arg1;
		this->arg2 = arg2;
	}
};

class val_attribute {
public:
	int vall[2]; //val
	string name;
	string type;
	int if_matrix;
	int width;
	string logic_op;
	int addr;
	matrix_develop * mdevelop;
	list<code*> codel;
};

class symbol {
public:
	string name;
	int if_matrix; //1Ϊ���飬0Ϊ����
	string type;
	int offset;//��ʼ��ַ
	matrix_develop *matrix_attribute;
	symbol(string name,int if_matrix,string type,int offset,matrix_develop *matrix_attribute)
	{
		this->name = name;
		this->if_matrix = if_matrix;
		this->type = type;
		this->offset = offset;
		this->matrix_attribute = matrix_attribute;
	}
};

class picnode {
public:
	int num;
	set<int> qian;
	set<int> hou;
	set<int> use;
	set<int> def;
	set<int> in;
	set<int> out;
	picnode(int num)
	{
		this->num = num;
	}
};

