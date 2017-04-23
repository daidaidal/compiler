#include "paser.h"
#include <iostream>
using namespace std;
void semantics(string guiyueshi);
void xpush(int m[2], string name);
int duandian = 0;
int sum_offset = 0;
stack<int> s;
val_attribute* x[10000];
int top = 0;
map<string, symbol*> symbol_map;
void main()
{
	lex *cifa = new lex();
	parser *yufa = new parser();
	gloable_variable *l = cifa->mainfunction();
	gloable_variablep * p = yufa->mainfunction();
	s.push(1);

	int judge = 0;

	for (int i = 0;; i++)
	{
		int * m = l->save[i];
		//if (judge == 1)
			//break;
		if (m[0] == -1)
		{
			m[0] = OIL;
			judge = 1;
		}
		string come = "";
		come = to_string(s.top()) + " " + to_string(m[0]);
		map<string, string>::iterator gg = p->go.find(come);
		map<string, string>::iterator gg_null = p->go.find(to_string(s.top()) + " " + "'$'");
		if (gg_null != p->go.end())
			gg = gg_null;
		if (gg != p->go.end())
		{
			string st = gg->second;
			if (st.at(0) != 'r') //移进
			{
				int num0 = 0;
				for (int j = 0; j < st.size(); j++)
					num0 = num0 * 10 + (st.at(j) - '0');
				s.push(num0);
				xpush(m,"");
			}
			else //规约
			{
				int num = 0;
				for (int j = 1; j < st.size(); j++)
					num = num * 10 + (st.at(j) - '0');
				map<int, string>::iterator find_guiyue = p->q_int_string.find(num);

				string guiyueshi = find_guiyue->second;
				cout << guiyueshi << endl;
				int flag = 0;
				int ii = 0;
				string guiyue = "";
				int count = 0;
				while (ii < guiyueshi.size())
				{
					if (guiyueshi.at(ii) == '-' && guiyueshi.at(ii + 1) == '>')
					{
						ii = ii + 2;
						flag = 1;
						count++;
						continue;
					}
					if (flag == 0)
						guiyue = guiyue + guiyueshi.at(ii);
					if (guiyueshi.at(ii) == ' ')
						count++;
					ii++;
				}

				//获得s和count
				//语义分析
				//semantics(guiyueshi,l);

				if (guiyue == "s")
				{
					cout << "success" << endl;
					break;
				}
				while (count != 0)
				{
					s.pop();
					count--;
				}

				string zhuanyi = p->go.find(to_string(s.top()) + ' ' + guiyue)->second;
				int num1 = 0;
				for (int j = 0; j < zhuanyi.size(); j++)
					num1 = num1 * 10 + (zhuanyi.at(j) - '0');
				s.push(num1);
				i = i - 1;


			}
		}
		else
			continue;

	}


	cout << endl;
	return;
}

void xpush(int m[2], string name)
{
	val_attribute * val = new val_attribute();
	if (name != "")
	{
		val->name = name;
		x[top] = val;
		top++;
		return;
	}
	val->vall[0] = m[0];
	val->vall[1] = m[1];
	x[top] = val;
	top++;
	return;
}
void enter(string name, int if_matrix, string type, matrix_develop * mdevelop, int width)
{
	symbol *t = new symbol(name, if_matrix, type, sum_offset, mdevelop);
	symbol_map.insert(pair<string, symbol*>(name, t));
	sum_offset = sum_offset + width;
	return;
}
void semantics(string guiyueshi,gloable_variable *l)
{
	if (guiyueshi == "type->char_type")
	{
		x[top]->type = "char";
		x[top]->width = 1;
	}
	else if (guiyueshi == "type->int_type")
	{
		x[top]->type = "int";
		x[top]->width = 2;
	}
	else if (guiyueshi == "type->void_type")
	{
		x[top]->type = "void";
		x[top]->width = 0;
	}
	else if (guiyueshi == "multi_id->id")
	{
		x[top]->if_matrix = 0;
		x[top]->name = l->list[x[top]->vall[1]];
		x[top]->width = 1;
	}
	else if (guiyueshi == "multi_id->matrix_id")
	{
		//如果符号表里没有视为声明，如果有视为调用某个值
		string name = l->list[x[top]->vall[2]];
		int j = 1;
		map<string, symbol*>::iterator gg;
		gg = symbol_map.find(name);
		if (gg== symbol_map.end()) //声明
		{
			for (int i = 1; i <= x[top]->mdevelop->sum_weishu; i++)
				j = j*x[top]->mdevelop->weishu[i];
			x[top]->width = j;
			x[top]->type = "";
		}
		else //调取某个值，返回值offset即可
		{
			int weishu = x[top]->mdevelop->sum_weishu;
			int *big= gg->second->matrix_attribute->weishu;
			int *small= x[top]->mdevelop->weishu;
			int sum = 0;
			for (int i = weishu; i >= 1; i--)
			{
				int behind = 1;
				for (int k = i+1; k <= weishu+1; k++)
				{
					if (k == weishu + 1)
						behind = behind * 1;
					else
						behind = behind*big[k];
				}
				sum = sum + small[i] * behind;
			}
			x[top]->width = sum;
			x[top]->type = gg->second->type;
		}
	}
	else if (guiyueshi == "matrix_id->id '[' int ']' multi_weishu")
	{
		//x[top-4].vall=?
		x[top-4]->if_matrix = 1;
		x[top - 4]->mdevelop = x[top]->mdevelop;
		x[top - 4]->name = l->list[x[top - 4]->vall[1]];
		int i = ++x[top - 4]->mdevelop->sum_weishu;
		x[top - 4]->mdevelop->weishu[i] = x[top - 2]->vall[1];
		top = top - 4;

	}
	else if (guiyueshi == "multi_weishu->'$'")
	{
		x[top]->mdevelop = new matrix_develop();
	}
	else if (guiyueshi == "multi_weishu->multi_weishu '[' int ']'")
	{
		int i = ++x[top - 3]->mdevelop->sum_weishu;
		x[top - 3]->mdevelop->weishu[i] =x[top - 1]->vall[1];
		top = top - 3;
	}
	else if (guiyueshi == "declare_statement->type multi_id")
	{
		if(x[top]->type=="")
			enter(x[top]->name,x[top]->if_matrix, x[top - 1]->type, x[top]->mdevelop, x[top - 1]->width*x[top]->width);
	}
	return;
}




