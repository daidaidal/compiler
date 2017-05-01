#include "paser.h"
#include <iostream>
using namespace std;
void semantics(string guiyueshi, gloable_variable *l);
void xpush(int m[2]);
void gencode(code* c);
void mergel();
int duandian = 0;
int sum_offset = 0;
int temp_offset = 1000;//存放临时变量地址
stack<int> s;
val_attribute* x[10000];
int top = -1;
list<int> int_stack;
list<char> char_stack;
map<string, symbol*> symbol_map;
stack<symbol*> declare_stack;
int code_count = 0;
list<code*> sum_code;
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
				xpush(m);
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
				semantics(guiyueshi, l);

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

	mergel();
	cout << endl;
	return;
}

void xpush(int m[2])
{
	top++;
	val_attribute * val = new val_attribute();
	val->vall[0] = m[0];
	val->vall[1] = m[1];
	x[top] = val;
	return;
}

void enter(string name, int if_matrix, string type, matrix_develop * mdevelop, int width)
{
	symbol *t = new symbol(name, if_matrix, type, sum_offset, mdevelop);
	symbol_map.insert(pair<string, symbol*>(name, t));
	sum_offset = sum_offset + width;
	return;
}
void semantics(string guiyueshi, gloable_variable *l)
{
	//声明语句
	if (guiyueshi == "type->char_type")
	{
		x[top]->type = "char";
		x[top]->width = 1;
	}
	else if (guiyueshi == "type->int_type")
	{
		x[top]->type = "int";
		x[top]->width = 4;
	}
	else if (guiyueshi == "type->void_type")
	{
		x[top]->type = "void";
		x[top]->width = 0;
	}
	else if (guiyueshi == "multi_id->id" || guiyueshi == "multi_id->multi_id ',' id")
	{
		string name = l->list[x[top]->vall[1]];;
		symbol* s_temp = new symbol(name, 0, "", 1, NULL); //暂存总大小没有乘类型长度
		declare_stack.push(s_temp);
		if (guiyueshi == "multi_id->multi_id ',' id")
			top = top - 2;
	}
	else if (guiyueshi == "multi_id->matrix_id" || guiyueshi == "multi_id->multi_id ',' matrix_id")
	{
		string name = x[top]->name;
		int j = 1;
		//找了一年的for
		for (int i = 1; i <= x[top]->mdevelop->sum_weishu; i++)
			j = j*x[top]->mdevelop->weishu[i];
		symbol* s_temp = new symbol(name, 1, "", j, x[top]->mdevelop); //暂存数组总大小没有乘类型长度
		declare_stack.push(s_temp);
		if (guiyueshi == "multi_id->multi_id ',' matrix_id")
		{
			x[top - 2]->codel.splice(x[top - 2]->codel.end(),x[top]->codel);
			top = top - 2;
		}
	}
	else if (guiyueshi == "multi_id->assign_statement");
	else if (guiyueshi == "multi_id->multi_id ',' assign_statement")
	{
		x[top - 2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);
		top = top - 2;
	}
	else if (guiyueshi == "matrix_id->id multi_weishu")
	{
		//x[top-4].vall=?
		string name = l->list[x[top - 1]->vall[1]];
		map<string, symbol*>::iterator gg = symbol_map.find(name);

		x[top - 1]->if_matrix = 1;
		x[top - 1]->mdevelop = x[top]->mdevelop;
		x[top - 1]->name = l->list[x[top - 1]->vall[1]];
		x[top - 1]->type = ""; //声明语句type为""
		if (gg != symbol_map.end()) //已经在表里面了,单独的赋值语句
		{
			x[top - 1]->if_matrix = 0;
			x[top - 1]->type = gg->second->type; //单独赋值语句type有值
			int weishu = x[top]->mdevelop->sum_weishu;
			int *big = gg->second->matrix_attribute->weishu;
			int *small = x[top]->mdevelop->weishu;
			int sum = 0;
			for (int i = weishu; i >= 1; i--)
			{
				int behind = 1;
				for (int k = i + 1; k <= weishu + 1; k++)
				{
					if (k == weishu + 1)
						behind = behind * 1;
					else
						behind = behind*big[k];
				}
				sum = sum + small[i] * behind;
			}
			int single;
			string t_type;
			t_type = gg->second->type;
			if (t_type == "int")
				single = 4;
			else if (t_type == "char")
				single = 1;

			x[top - 1]->width = single*(sum - 1); //数组该位元素的首地址
			x[top - 1]->type = gg->second->type;
		}
		top = top - 1;

	}
	else if (guiyueshi == "multi_weishu->'[' int ']'")
	{
		x[top - 2]->mdevelop = new matrix_develop();
		x[top - 2]->mdevelop->sum_weishu = 1;
		x[top - 2]->mdevelop->weishu[1] = x[top - 1]->vall[1];
		top = top - 2;
	}
	else if (guiyueshi == "multi_weishu->multi_weishu '[' int ']'")
	{
		int i = x[top - 3]->mdevelop->sum_weishu;
		x[top - 3]->mdevelop->sum_weishu = i + 1;
		x[top - 3]->mdevelop->weishu[i + 1] = x[top - 1]->vall[1];
		top = top - 3;
	}
	//assign_statement
	else if (guiyueshi == "assign_statement->id '=' char")//不知道是否在符号表中
	{
		string name = l->list[x[top - 2]->vall[1]];
		string char_name = l->char_list[x[top - 2]->vall[1]];
		map<string, symbol*>::iterator gg = symbol_map.find(name);
		if (gg != symbol_map.end())
		{
			x[top - 2]->addr = gg->second->offset;
			code * temp_code=new code(x[top - 2]->addr, ":=", char_name.at(1), 0);
			x[top - 2]->codel.push_back(temp_code);
			gencode(temp_code);
		}
		else
		{
			int offset0 = sum_offset;
			enter(name, 0, "char", NULL, 1);
			code * temp_code=new code(offset0, ":=", char_name.at(1), 0);
			x[top - 2]->codel.push_back(temp_code);
			gencode(temp_code);
		}
		top = top - 2;
	}
	else if (guiyueshi == "assign_statement->id '=' math_statement")//不知道是否在符号表中-----------------
	{
		string name = l->list[x[top - 2]->vall[1]];
		map<string, symbol*>::iterator gg = symbol_map.find(name);
		if (gg != symbol_map.end())
		{
			x[top - 2]->addr = gg->second->offset;
			code * temp_code=new code(x[top - 2]->addr, "=", x[top]->addr, 0);
			x[top - 2]->codel = x[top]->codel;
			x[top - 2]->codel.push_back(temp_code);
			gencode(temp_code);
		}
		else
		{
			int offset0 = sum_offset;
			enter(name, 0, "int", NULL, 4);
			code * temp_code=new code(offset0, "=", x[top]->addr, 0);
			x[top - 2]->codel.splice(x[top - 2]->codel.end(),x[top]->codel);
			x[top - 2]->codel.push_back(temp_code);
			gencode(temp_code);
		}
		top = top - 2;
	}
	else if (guiyueshi == "assign_statement->matrix_id '=' char") //仅为赋值语句,在符号表中
	{
		string name = x[top - 2]->name;
		string char_name = l->char_list[x[top - 2]->vall[1]];
		map<string, symbol*>::iterator gg = symbol_map.find(name);
		if (gg != symbol_map.end())
		{
			x[top - 2]->addr = gg->second->offset + x[top - 2]->width;
			code * temp_code=new code(x[top - 2]->addr, ":=", char_name.at(1), 0);
			x[top - 2]->codel.push_back(temp_code);
			gencode(temp_code);
		}
		else
			cout << "can't find id while assign_statement->id '=' math_statement";
		top = top - 2;
	}
	else if (guiyueshi == "assign_statement->matrix_id '=' math_statement")//仅为赋值语句,知道在符号表中
	{
		map<string, symbol*>::iterator gg = symbol_map.find(l->list[x[top - 2]->vall[1]]);
		if (gg != symbol_map.end())
		{
			x[top - 2]->addr = gg->second->offset + x[top - 2]->width;
			code * temp_code=new code(x[top - 2]->addr, "=", x[top]->addr, 0);
			x[top - 2]->codel.splice(x[top - 2]->codel.end(),x[top]->codel);
			x[top - 2]->codel.push_back(temp_code);
			gencode(temp_code);
		}
		else
			cout << "can't find id while assign_statement->id '=' math_statement";
		top = top - 2;
	}
	else if (guiyueshi == "assign_statement->matrix_id '=' '{' multi_int '}'") //仅为声明语句
	{
		string name = x[top - 4]->name;
		int j = 1;
		for (int i = 1; i <= x[top-4]->mdevelop->sum_weishu; i++)
			j = j*x[top-4]->mdevelop->weishu[i];
		j = j*4;
		//enter(string name, int if_matrix, string type, matrix_develop * mdevelop, int width)
		int offset0 = sum_offset;
		enter(name, 1, "int", x[top - 4]->mdevelop, j);
		for (list<int>::iterator gg = int_stack.begin(); gg != int_stack.end(); gg++)
		{
			code * temp_code=new code(offset0,":=",*gg,0);
			x[top - 4]->codel.splice(x[top - 4]->codel.end(), x[top - 1]->codel);
			x[top - 4]->codel.push_back(temp_code);
			offset0 = offset0 + 4;
			gencode(temp_code);
		}
		int_stack.clear();
		top = top - 4;
	}
	else if (guiyueshi == "assign_statement->matrix_id '=' '{' multi_char '}'") //仅为声明语句
	{
		string name = x[top - 4]->name;
		int j = 1;
		for (int i = 1; i <= x[top - 4]->mdevelop->sum_weishu; i++)
			j = j*x[top-4]->mdevelop->weishu[i];
		j = j * 1;
		//enter(string name, int if_matrix, string type, matrix_develop * mdevelop, int width)
		int offset0 = sum_offset;
		enter(name, 1, "char", x[top - 4]->mdevelop, j);
		for (list<char>::iterator gg = char_stack.begin(); gg != char_stack.end(); gg++)
		{
			code * temp_code=new code(offset0, ":=", *gg, 0);
			x[top - 4]->codel.splice(x[top - 4]->codel.end(), x[top - 1]->codel);
			x[top - 4]->codel.push_back(temp_code);
			offset0 = offset0 + 1;
			gencode(temp_code);
		}
		char_stack.clear();
		top = top - 4;
	}
	//multi_int multi_char
	else if (guiyueshi == "multi_int->int")
	{
		int_stack.push_back(x[top]->vall[1]);
	}
	else if (guiyueshi == "multi_int->multi_int ',' int")
	{
		int_stack.push_back(x[top]->vall[1]);
		top = top - 2;
	}
	else if (guiyueshi == "multi_char->char")
	{
		char_stack.push_back(l->char_list[x[top]->vall[1]][1]);
	}
	else if (guiyueshi == "multi_char->multi_char ',' char")
	{
		char_stack.push_back(l->char_list[x[top]->vall[1]][1]);
		top = top - 2;
	}
	else if (guiyueshi == "declare_statement->type multi_id")
	{
		string type = x[top - 1]->type;
		int single = x[top - 1]->width;
		while (!declare_stack.empty())
		{
			symbol * s_temp;
			s_temp = declare_stack.top();
			declare_stack.pop();
			enter(s_temp->name, s_temp->if_matrix, type, s_temp->matrix_attribute, s_temp->offset*single);
		}
		x[top - 1]->codel = x[top]->codel;
		top = top - 1;
	}
	//数学运算 math_basic
	else if (guiyueshi == "math_basic->id") //去符号表找到地址赋给math_basic
	{
		string name = l->list[x[top]->vall[1]];
		map<string, symbol*>::iterator gg = symbol_map.find(name);
		if (gg != symbol_map.end())
		{
			x[top]->addr = gg->second->offset;
			x[top]->name = gg->second->name;
		}
		else
			cout << "can't find id " + x[top]->name << endl;
	}
	else if (guiyueshi == "math_basic->matrix_id")//同上
	{
		string name = x[top]->name;
		map<string, symbol*>::iterator gg = symbol_map.find(name);
		if (gg != symbol_map.end())
		{
			x[top]->addr = gg->second->offset + x[top]->width;//基地址+偏移地址
			x[top]->name = gg->second->name;
		}
		else
			cout << "can't find matrix_id " + x[top]->name << endl;
	}
	else if (guiyueshi == "math_basic->int")//
	{
		//int的值存到offset中-------------------------------------??
		x[top]->addr = temp_offset;
		code * temp_code=new code(x[top]->addr, ":=", x[top]->vall[1], 0);
		x[top]->codel.push_back(temp_code);
		gencode(temp_code);
		temp_offset = temp_offset + 4;
	}
	else if (guiyueshi == "math_basic->'(' math_statement ')'") //code * temp_code=new code
	{
		x[top - 2] = x[top - 1];
		top = top - 2;
	}
	else if (guiyueshi == "math_basic->'-' math_basic")
	{
		x[top - 1] = x[top];
		code * temp_code=new code(x[top - 1]->addr, "-", x[top - 1]->addr, 0);//统一单操作符时仅调用arg1
		x[top]->codel.push_back(temp_code);
		x[top - 1]->codel = x[top]->codel;
		gencode(temp_code);
		top = top - 1;
	}
	else if (guiyueshi == "math_basic->'++' math_basic") //code * temp_code=new code -------------------先考虑不改变元变量值----------------------
	{
		x[top - 1] = x[top];
		code * temp_code=new code(x[top - 1]->addr, "++", x[top - 1]->addr, 0);//统一单操作符时仅调用arg1
		x[top]->codel.push_back(temp_code);
		x[top - 1]->codel = x[top]->codel;
		gencode(temp_code);
		top = top - 1;
	}
	else if (guiyueshi == "math_basic->'--' math_basic")
	{
		x[top - 1] = x[top];
		code * temp_code=new code(x[top - 1]->addr, "--", x[top - 1]->addr, 0);//统一单操作符时仅调用arg1
		x[top]->codel.push_back(temp_code);
		x[top - 1]->codel = x[top]->codel;
		gencode(temp_code);
		top = top - 1;
	}
	else if (guiyueshi == "math_item->math_basic");
	else if (guiyueshi == "math_item->math_item '*' math_basic")
	{
		code * temp_code=new code(x[top - 2]->addr, "*", x[top - 2]->addr, x[top]->addr);
		x[top-2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);
		x[top - 2]->codel.push_back(temp_code);
		gencode(temp_code);
		top = top - 2;
	}
	else if (guiyueshi == "math_item->math_item '/' math_basic")
	{
		code * temp_code=new code(x[top - 2]->addr, "/", x[top - 2]->addr, x[top]->addr);
		x[top - 2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);
		x[top - 2]->codel.push_back(temp_code);
		gencode(temp_code);
		top = top - 2;
	}
	//math_statement
	else if (guiyueshi == "math_statement->math_item");
	else if (guiyueshi == "math_statement->math_statement '+' math_item")
	{
		//x[top - 2]->addr = temp_offset;
		code * temp_code=new code(x[top - 2]->addr, "+", x[top - 2]->addr, x[top]->addr);
		x[top - 2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);
		x[top - 2]->codel.push_back(temp_code);
		gencode(temp_code);
		//temp_offset = temp_offset + 4;
		top = top - 2;
	}
	else if (guiyueshi == "math_statement->math_statement '-' math_item")
	{
		//x[top - 2]->addr = temp_offset;
		code * temp_code=new code(x[top - 2]->addr, "-", x[top - 2]->addr, x[top]->addr);
		x[top - 2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);
		x[top - 2]->codel.push_back(temp_code);
		gencode(temp_code);
		//temp_offset = temp_offset + 4;
		top = top - 2;
	}
	//logic_op
	else if (guiyueshi == "logic_op->'>='")
		x[top]->logic_op = "'>='";
	else if (guiyueshi == "logic_op->'<='")
		x[top]->logic_op = "'<='";
	else if (guiyueshi == "logic_op->'>'")
		x[top]->logic_op = "'>'";
	else if (guiyueshi == "logic_op->'<'")
		x[top]->logic_op = "'<'";
	else if (guiyueshi == "logic_op->'=='")
		x[top]->logic_op = "'=='";
	else if (guiyueshi == "logic_op->'!='")
		x[top]->logic_op = "'!='";
	//logic_statement
	else if (guiyueshi == "logic->logic_mid");
	else if (guiyueshi == "logic->logic '||' logic_mid")
	{
		code * temp_code = new code(x[top - 2]->addr, "||", x[top - 2]->addr, x[top]->addr);
		x[top - 2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);
		x[top - 2]->codel.push_back(temp_code);
		gencode(temp_code);
		top = top - 2;
	}
	else if (guiyueshi == "logic_mid->logic_basic");
	else if (guiyueshi == "logic_mid->logic_mid '&&' logic_basic")
	{
		code * temp_code = new code(x[top - 2]->addr, "&&", x[top - 2]->addr, x[top]->addr);
		x[top - 2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);
		x[top - 2]->codel.push_back(temp_code);
		gencode(temp_code);
		top = top - 2;
	}
	else if (guiyueshi == "logic_basic->'(' logic ')'")
	{
		x[top - 2] = x[top - 1];
		top = top - 2;
	}
	else if (guiyueshi == "logic_basic->'!' logic_basic")
	{
		x[top - 1]->addr = x[top]->addr;
		code * temp_code = new code(x[top - 1]->addr, "!", x[top-1]->addr, 0);
		x[top - 1]->codel = x[top]->codel;
		x[top - 1]->codel.push_back(temp_code);
		gencode(temp_code);
		top = top - 1;
	}
	else if (guiyueshi == "logic_basic->math_statement logic_op math_statement")
	{
		x[top - 2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);
		code * temp_code1=new code(-3, x[top - 1]->logic_op,x[top-2]->addr,x[top]->addr);//if a<b goto nextquad+3
		x[top - 2]->addr = temp_offset;
		x[top - 2]->codel.push_back(temp_code1);
		gencode(temp_code1);
		code * temp_code2=new code(x[top - 2]->addr, ":=", 0, 0);
		x[top - 2]->codel.push_back(temp_code2);
		gencode(temp_code2);
		code * temp_code3=new code(-2, "goto", 0, 0); //生成全部三地址码的时候-n表示去当前num的基础上加n的位置
		x[top - 2]->codel.push_back(temp_code3);
		gencode(temp_code3);
		code * temp_code4=new code(x[top - 2]->addr, ":=", 1, 0);
		x[top - 2]->codel.push_back(temp_code4);
		gencode(temp_code4);
		++temp_offset;
		top = top - 2;
	}
	//if_statement
	else if (guiyueshi == "if_statement->if logic codeblockplus")
	{
		code * temp_code0 = new code(-2, "==", x[top - 1]->addr, 1);   //0
		gencode(temp_code0);
		code * temp_code1 = new code(-(int)(x[top]->codel.size() + 1), "goto", 0, 0); //1
																	 //s.code.begin //2
		gencode(temp_code1);
		x[top - 2]->codel = x[top - 1]->codel;
		x[top - 2]->codel.push_back(temp_code0);
		x[top - 2]->codel.push_back(temp_code1);
		x[top - 2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);

		top = top - 2;	
	}
	else if (guiyueshi == "if_statement->if logic codeblockplus else codeblockplus")
	{
		code* temp_code0 = new code(-2, "==", x[top - 3]->addr, 1); 
		gencode(temp_code0);
		code * temp_code1 = new code(-(int)(2 + x[top - 2]->codel.size()), "goto", 0, 0);
		gencode(temp_code1);
		code * temp_code2 = new code(-(int)(1+x[top]->codel.size()), "goto", 0, 0);
		gencode(temp_code2);

		x[top - 4]->codel = x[top - 3]->codel;   //b.code
		x[top - 4]->codel.push_back(temp_code0); // if b==1 goto ture
		x[top - 4]->codel.push_back(temp_code1); // goto false
		x[top - 4]->codel.splice(x[top - 4]->codel.end(), x[top - 2]->codel);  // c1.code
		x[top - 4]->codel.push_back(temp_code2); //goto end
		x[top - 4]->codel.splice(x[top - 4]->codel.end(), x[top]->codel); //c2.code

		top = top - 4;
	}
	//while_statement
	else if (guiyueshi == "while_statement->while '(' logic ')' codeblockplus")
	{
		code * temp_code0 = new code(-2, "==", x[top - 2]->addr, 1);
		gencode(temp_code0);
		code * temp_code1 = new code(-(int)(x[top]->codel.size() + 2), "goto", 0, 0);
		gencode(temp_code1);
		code * temp_code2 = new code(-(int)(2 + x[top]->codel.size() + x[top - 2]->codel.size()), "rgoto", 0, 0);
		gencode(temp_code2);

		x[top - 4]->codel = x[top - 2]->codel;      //b.code
		x[top - 4]->codel.push_back(temp_code0);    //if b==true goto 
		x[top - 4]->codel.push_back(temp_code1);    //goto end
		x[top - 4]->codel.splice(x[top - 4]->codel.end(), x[top]->codel);     //s.code
		x[top - 4]->codel.push_back(temp_code2);	//goto begin

		top = top - 4;
	}
	else if (guiyueshi == "for_statement->for '(' assign_statement ';' logic ';' assign_statement ')' codeblockplus")
	{
		code * temp_code0 = new code(-2,"==",x[top-4]->addr,1);
		gencode(temp_code0);
		code * temp_code1 = new code(-(int)(x[top]->codel.size()+x[top-2]->codel.size() + 2), "goto", 0, 0);
		gencode(temp_code1);
		code * temp_code2 = new code(-(int)(x[top]->codel.size() + x[top - 2]->codel.size()+ x[top - 4]->codel.size() + 2), "rgoto", 0, 0);
		gencode(temp_code2);

		x[top - 8]->codel = x[top - 6]->codel;      // assign1
		x[top - 8]->codel.splice(x[top - 8]->codel.end(), x[top - 4]->codel); // logic
		x[top - 8]->codel.push_back(temp_code0);    // if b==1 goto true
		x[top - 8]->codel.push_back(temp_code1);    // goto end
		x[top - 8]->codel.splice(x[top - 8]->codel.end(), x[top]->codel);     // s
		x[top - 8]->codel.splice(x[top - 8]->codel.end(), x[top-2]->codel);   // assign2
		x[top - 8]->codel.push_back(temp_code2);	// goto begin

		top = top - 8;
	}
	else if (guiyueshi == "statement->assign_statement ';'")
		top = top - 1;
	else if (guiyueshi == "statement->declare_statement ';'")
		top = top - 1;
	else if (guiyueshi == "statement->control_statement");
	else if (guiyueshi == "statement->print_statement ';'")
		top = top - 1;
	else if (guiyueshi == "codeblock->statement");
	else if (guiyueshi == "codeblock->statement codeblock")
	{
		x[top - 1]->codel.splice(x[top - 1]->codel.end(), x[top]->codel);
		top = top - 1;
	}
	else if (guiyueshi == "codeblockplus->'{' codeblock '}'")
	{
		x[top - 2]->codel = x[top - 1]->codel;
		top = top - 2;
	}
	else if (guiyueshi == "s->type main '(' ')' codeblockplus")
	{
		x[top - 4]->codel = x[top]->codel;
		sum_code = x[top - 4]->codel;
		top = top - 4;
	}
	return;
}
void gencode(code* c)
{
	cout<<to_string(c->result) + " " + c->op + " " + to_string(c->arg1) + " " + to_string(c->arg2) << endl;
	code_count++;
}

void mergel()
{
	list<code*>::iterator gg;
	int num = 1;
	for (gg = sum_code.begin(); gg != sum_code.end();gg++)
	{
		code * l = *gg;
		l->num = num;
		if (l->result < 0)
		{
			if (l->op == "rgoto")
			{
				l->result = num + l->result;
			}
			else
				l->result = num - l->result;
		}
		cout << num<<":   "<<to_string(l->result) + " " + l->op + " " + to_string(l->arg1) + " " + to_string(l->arg2) << endl;
		num++;
	}
}
/*
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
int single;
string t_type;
t_type = gg->second->type;
if (t_type == "int")
single = 2;
else if (t_type == "char")
single = 1;
x[top]->width = single*sum;
x[top]->type = gg->second->type;
}
//logic
else if (guiyueshi == "logic->logic '||' logic")
{
code * temp_code=new code(x[top - 2]->addr, "||", x[top - 2]->addr, x[top]->addr);
x[top - 2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);
x[top - 2]->codel.push_back(temp_code);
gencode(temp_code);
top = top - 2;
}
else if (guiyueshi == "logic->logic '&&' logic")
{
code * temp_code=new code(x[top - 2]->addr, "&&", x[top - 2]->addr, x[top]->addr);
x[top - 2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);
x[top - 2]->codel.push_back(temp_code);
gencode(temp_code);
top = top - 2;
}
else if (guiyueshi == "logic->'!' logic")
{
x[top - 1]->addr = x[top]->addr;
code * temp_code=new code(x[top - 1]->addr, "!", x[top]->addr, 0);
x[top-1]->codel=x[top]->codel;
x[top - 1]->codel.push_back(temp_code);
gencode(temp_code);
top = top - 1;
}
else if (guiyueshi == "logic->'(' logic ')'")
{
x[top - 2]= x[top - 1];
top = top - 2;
}
*/