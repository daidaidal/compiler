#include "hong.h"
#include<iostream>
#include<string>
#include<tuple>
#include <map>
#include <fstream> 
#include <math.h>
using namespace std;
class gloable_variable {
public:
	char * buffer = new char[8192];
	ifstream in;
	int qian = 8191;
	int hou = 0;
	char * list[1000]; //标识符
	char * string_list[1000]; //string常量表
	int count_sl = 0;
	char * char_list[1000]; // char常量表
	int count_cl = 0;
	int countl = 0;
	int boundary = 0; //是否回退到边界线过
	class errorl *e[100]; //错误数组 行数，错误原因
	int count_e = 0;
	int hang = 1; //处理到多少行 错误提示使用
	int error_hang; //错误行数
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
	//各种函数
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
	int sum_weishu; //从一开始
	int weishu[10];
	matrix_develop()
	{
		this->sum_weishu = 0;
	}
};
class val_attribute {
public:
	int vall[2]; //val
	string name;
	string type;
	int if_matrix;
	int width;
	matrix_develop * mdevelop;

};
class symbol {
public:
	string name;
	int if_matrix; //1为数组，0为变量
	string type;
	int offset;
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

