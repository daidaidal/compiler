#include "paser.h"
#include <iostream>
#define TEMP_INT 1100
#define TEMP_BOOL 1000
using namespace std;
ofstream o;;
void semantics(string guiyueshi, gloable_variable *l);
void xpush(int m[2]);
void gencode(code* c);
void mergel();
void make_pic();
void make_assem();
string make_op(string op);
int getres(code * l);
int duandian = 0;
int sum_offset = 0;
int temp_offset_bool = TEMP_INT;//存放临时变量地址
int temp_offset_int = TEMP_BOOL;//存放临时变量地址
stack<int> s;
val_attribute* x[10000];
int top = -1;
list<int> int_stack;
list<char> char_stack;
map<string, symbol*> symbol_map;//名字-符号表
map<int, sys*> symbol_map2;//汇编符号表地址-符号表 类型 后续引用 地址描述符
set<int> res[16];
stack<symbol*> declare_stack;
int code_count = 0;
list<code*> sum_code;
list<code*> big_code[100];
picnode * liutu[100];
map<int, int> find_node;//行号--所属基本块号
int node_count;//流图中有多少个基本快
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
	make_pic();
	make_assem();

	cout << endl;
	return;
}

//计算活跃变量
void make_pic()
{
	list<code*> codel;
	codel.splice(codel.begin(), sum_code);
	list<code*>::iterator gg;
	set<int> s;
	for (gg = codel.begin(); gg != codel.end(); gg++)
	{
		code * l = *gg;
		if (l->op == "goto"||l->op == "'>='" || l->op == "'<='" || l->op == "'>'" || l->op == "'<'" || l->op == "'=='" || l->op == "'!='")
			s.insert(l->result);
	}
	s.insert(1);
	list<code*> temp;
	node_count = 0;
	int hangcount = 1;
	for (gg = codel.begin(); gg != codel.end(); gg++)
	{
		code * l = *gg;
		int num = -1;
		if (s.size() != 0)
			num = *s.begin();
		if (l->num == num && num != -1)
		{
			liutu[node_count] = new picnode(node_count);
			s.erase(num);
			big_code[node_count] = temp;
			temp.clear();
			node_count++;
		}
		l->node_num = node_count;
		temp.push_back(l);
		find_node.insert(pair<int, int>(hangcount, node_count));
		hangcount++;
	}
	if (temp.size() != 0)
	{
		big_code[node_count] = temp;
		liutu[node_count] = new picnode(node_count);
	}
	//建立关系流图 liutu
	int i = 1;
	for (gg = codel.begin(); gg != codel.end(); gg++)
	{
		code *l = *gg;
		if (l->op == "goto" ||l->op == "'>='" || l->op == "'<='" || l->op == "'>'" || l->op == "'<'" || l->op == "'=='" || l->op == "'!='")
		{
			int this_node = find_node.find(l->num)->second;
			int that_node = find_node.find(l->result)->second;
			if (this_node == that_node)
				continue;
			liutu[this_node]->hou.insert(that_node);
			liutu[that_node]->qian.insert(this_node);
		}
		if (l->num == big_code[i].back()->num&&l->op!="goto")
		{
			//i和i+1建立关系
			liutu[i]->hou.insert(i + 1);
			liutu[i + 1]->qian.insert(i);
		}
	}
	//计算每个基本快的use和def
	for (int i = 1; i <= node_count; i++)
	{
		list<code*> l = big_code[i];
		list<code*>::iterator gg;
		for (gg = l.begin(); gg != l.end(); gg++)
		{
			code * ll = *gg;
			if (ll->op == ":=")
				liutu[i]->def.insert(ll->result);
			if (ll->op == "=" || ll->op == "++" || ll->op == "--" || ll->op == "+" || ll->op == "-" || ll->op == "*" || ll->op == "/" || ll->op == "&&" || ll->op == "||" || ll->op == "!")
			{
				liutu[i]->def.insert(ll->result);
				if (ll->arg1 != -1)
					liutu[i]->use.insert(ll->arg1);
				if (ll->arg2 != -1)
					liutu[i]->use.insert(ll->arg2);
			}
			if (ll->op == "'>'" || ll->op == "'<'" || ll->op == "'>='" || ll->op == "'<='" || ll->op == "'=='" || ll->op == "'!='")
			{
				if (ll->arg1 != -1)
					liutu[i]->use.insert(ll->arg1);
				if (ll->arg2 != -1)
					liutu[i]->use.insert(ll->arg2);
			}
		}
	}
	//计算活跃变量
	bool flag = true;
	while (flag)
	{
		flag = false;
		for (int i = node_count; i >=1; i--)
		{
			//计算out
			for (set<int>::iterator m = liutu[i]->hou.begin(); m != liutu[i]->hou.end(); m++)
			{
				int hou = *m;
				liutu[i]->out.insert(liutu[hou]->in.begin(), liutu[hou]->in.end());
			}
			//计算in
			int size1 = liutu[i]->in.size();
			//加入use
			liutu[i]->in.insert(liutu[i]->use.begin(), liutu[i]->use.end());
			//out-def
			for (set<int>::iterator a = liutu[i]->def.begin(); a != liutu[i]->def.end(); a++)
			{
				int b = *a;
				liutu[i]->out.erase(b);
			}
			//liutu[i]->out.erase(liutu[i]->def.begin(), liutu[i]->def.end());
			//加入out
			liutu[i]->in.insert(liutu[i]->out.begin(), liutu[i]->out.end());
			int size2 = liutu[i]->in.size();
			if (size2 != size1)
				flag = true;
		}
	}

}
void make_assem()
{
	o.open("C:\\Users\\daisf\\Documents\\cworkspace\\compiler\\compiler\\huibian.txt");
	list<code*>::iterator gg;
	for (int i = 1; i <= node_count; i++)//对每个基本块遍历两次
	{
		for (gg = big_code[i].begin(); gg != big_code[i].end(); gg++)//建立每个块的后续引用关系
		{
			code * ll = *gg;
			if (ll->op == ":=" || ll->op == "=" || ll->op == "++" || ll->op == "--" || ll->op == "+" || ll->op == "-" || ll->op == "*" || ll->op == "/" || ll->op == "&&" || ll->op == "||" || ll->op == "!")
			{
				if (symbol_map2.find(ll->result) == symbol_map2.end())//不在汇编符号表中
				{
					sys * s = new sys(ll->result);
					symbol_map2.insert(pair<int, sys*>(ll->result, s));
					//首次出现放到内存中
				}
				if (ll->arg1 != -1)
				{
					if (symbol_map2.find(ll->arg1) == symbol_map2.end())
						symbol_map2.insert(pair<int, sys*>(ll->arg1, new sys(ll->arg1)));
					symbol_map2.find(ll->arg1)->second->yinyong.insert(ll->num);
				}
				if (ll->arg2 != -1)
				{
					if (symbol_map2.find(ll->arg2) == symbol_map2.end())
						symbol_map2.insert(pair<int, sys*>(ll->arg2, new sys(ll->arg2)));
					symbol_map2.find(ll->arg2)->second->yinyong.insert(ll->num);
				}
			}
		}


		cout<< to_string(i) + ":";
		for (gg = big_code[i].begin(); gg != big_code[i].end(); gg++)//生成汇编代码
		{
			string s = "";
			code * ll = *gg;
			cout<< to_string(ll->num) + ":";
			o << to_string(ll->num) + ":";
			//立即数赋值
			string opl = make_op(ll->op);
			if (ll->op == ":=") 
			{
				//如果a一开始在寄存器中就互删一波
				int yuan;
				if(symbol_map2.find(ll->result)!=symbol_map2.end())
					yuan= *symbol_map2.find(ll->result)->second->address.begin();
				if (yuan < 0)
				{
					o << "MOV " + to_string(ll->result)+to_string(yuan) + '\n';
					res[-yuan].erase(ll->result);
					symbol_map2.find(ll->result)->second->address.erase(yuan);
					symbol_map2.find(ll->result)->second->address.insert(ll->result);
				}
				int resl = getres(ll);
				if (resl < 0)
				{
					cout<< "MOV " + to_string(resl) + ",#" + to_string(ll->arg1) + '\n';
					o << "MOV " + to_string(resl) + ",#" + to_string(ll->arg1) + '\n';//b->寄存器
					res[-resl].insert(ll->result);//寄+a
					symbol_map2.find(ll->result)->second->address.insert(resl);
				}
				else
					o << "error1" + '\n';
				symbol_map2.find(ll->result)->second->address.erase(ll->result);
			}
			//=赋值
			else if(ll->op == "=" )
			{
				int yuan;
				if (symbol_map2.find(ll->result) != symbol_map2.end())
					yuan = *symbol_map2.find(ll->result)->second->address.begin();
				if (yuan < 0)
				{
					o << "MOV " + to_string(ll->result) + to_string(yuan) + '\n';
					res[-yuan].erase(ll->result);
					symbol_map2.find(ll->result)->second->address.erase(yuan);
					symbol_map2.find(ll->result)->second->address.insert(ll->result);
				}

				int num = *symbol_map2.find(ll->arg1)->second->address.begin();//arg1所在地址
				if (num < 0)
				{
					res[-num].insert(ll->result);
					symbol_map2.find(ll->result)->second->address.insert(num);
					if (liutu[ll->node_num]->out.find(ll->arg1) == liutu[ll->node_num]->out.end() && symbol_map2.find(ll->arg1)->second->yinyong.size() == 1)
					{
						cout<< "MOV " + to_string(ll->arg1) + "," + to_string(num) + '\n';
						o << "MOV " + to_string(ll->arg1)+"," +to_string(num)+'\n';//b->寄存器
						res[-num].erase(ll->arg1);
						symbol_map2.find(ll->arg1)->second->address.erase(num);
						symbol_map2.find(ll->arg1)->second->address.insert(ll->arg1);
					}
				}
				else
				{
					int resl = getres(ll);
					if (resl < 0)
					{
						cout<< "MOV " + to_string(resl) + "," + to_string(ll->arg1) + '\n';
						o << "MOV " + to_string(resl) + "," + to_string(ll->arg1)+'\n';//b->寄存器
						res[-resl].insert(ll->result);//寄+a
						symbol_map2.find(ll->result)->second->address.insert(resl);
					}
					else
						o << "error1" + '\n';
				}
				symbol_map2.find(ll->result)->second->address.erase(ll->result);

			}
			//单操作数
			else if (ll->op == "++"|| ll->op == "--" || ll->op == "!")
			{
				int yuan;
				if (symbol_map2.find(ll->result) != symbol_map2.end())
					yuan = *symbol_map2.find(ll->result)->second->address.begin();
				if (yuan < 0)
				{
					o << "MOV " + to_string(ll->result) + to_string(yuan) + '\n';
					res[-yuan].erase(ll->result);
					symbol_map2.find(ll->result)->second->address.erase(yuan);
					symbol_map2.find(ll->result)->second->address.insert(ll->result);
				}

				int add;
				int resl = getres(ll);//L
				set<int> t = symbol_map2.find(ll->arg1)->second->address;
				add = *t.begin();//arg1
				cout << "MOV " + to_string(add) + "," + to_string(resl) + '\n';
				o << "MOV " + to_string(add) + "," + to_string(resl) + '\n';
				cout << opl + " " + to_string(resl) + '\n';
				o << opl + " " + to_string(resl) + '\n';
				//记录x在l中
				symbol_map2.find(ll->result)->second->address.insert(resl);
				//如果l为寄存器记录该寄存器存有x
				if (resl < 0)
					res[-resl].insert(ll->result);
				else
					o << "error2" + '\n';
				if (ll->arg1!=ll->result&&symbol_map2.find(ll->arg1)->second->yinyong.size() == 1 && liutu[ll->node_num]->out.find(ll->arg1) == liutu[ll->node_num]->out.end())
				{
					int jicunqi = *symbol_map2.find(ll->arg1)->second->address.begin();
					if (jicunqi < 0)
					{
						res[-jicunqi].erase(ll->arg1);
						symbol_map2.find(ll->arg1)->second->address.erase(jicunqi);
					}
				}

				symbol_map2.find(ll->result)->second->address.erase(ll->result);
			}
			//多操作数
			else if (ll->op == "+"|| ll->op == "-" || ll->op == "*" || ll->op == "/" || ll->op == "&&" || ll->op == "||")
			{
				int yuan;
				if (symbol_map2.find(ll->result) != symbol_map2.end())
					yuan = *symbol_map2.find(ll->result)->second->address.begin();
				if (yuan < 0)
				{
					if (ll->result != ll->arg1)
					{
						cout << "MOV " + to_string(ll->result) + "," + to_string(yuan) + '\n';
						o << "MOV " + to_string(ll->result) + "," + to_string(yuan) + '\n';
						res[-yuan].erase(ll->result);
						symbol_map2.find(ll->result)->second->address.erase(yuan);
						symbol_map2.find(ll->result)->second->address.insert(ll->result);
					}
				}

				int add;
				set<int> t;
				int resl = yuan;
				if (ll->result != ll->arg1&&yuan>=0)
				{
					resl = getres(ll);//L
					t = symbol_map2.find(ll->arg1)->second->address;
					add = *t.begin();//arg1
					if (add != resl)//找到地址不是arg1地址
					{
						cout << "MOV " + to_string(resl) + "," + to_string(add) + '\n';// mov 目的 源
						o << "MOV " + to_string(resl) + "," + to_string(add) + '\n';
					}
				}
				
				t = symbol_map2.find(ll->arg2)->second->address;
				add = *t.begin();
				cout << opl + " " + to_string(resl) + ","  + to_string(add) + '\n';
				o << opl + " " + to_string(resl) + "," + to_string(add) + '\n';
				//记录x在l中
				symbol_map2.find(ll->result)->second->address.insert(resl);
				//如果l为寄存器记录该寄存器存有x
				if (resl < 0)
					res[-resl].insert(ll->result);
				else
					o << "error3" + '\n';
				if (ll->arg1!=ll->result&&symbol_map2.find(ll->arg1)->second->yinyong.size() == 1&& liutu[ll->node_num]->out.find(ll->arg1) == liutu[ll->node_num]->out.end())
				{
					int jicunqi = *symbol_map2.find(ll->arg1)->second->address.begin();
					if (jicunqi < 0)
					{
						res[-jicunqi].erase(ll->arg1);
						symbol_map2.find(ll->arg1)->second->address.erase(jicunqi);
					}
				}

				if (ll->arg2!=ll->result&&symbol_map2.find(ll->arg2)->second->yinyong.size() == 1 && liutu[ll->node_num]->out.find(ll->arg2) == liutu[ll->node_num]->out.end())
				{
					int jicunqi = *symbol_map2.find(ll->arg2)->second->address.begin();
					if (jicunqi < 0)
					{
						res[-jicunqi].erase(ll->arg2);
						symbol_map2.find(ll->arg2)->second->address.erase(jicunqi);
					}
				}

				symbol_map2.find(ll->result)->second->address.erase(ll->result);
			}
			//goto
			else if (ll->op == "goto")
			{
				cout << "JMP " + to_string(ll->result) + '\n';
				o << "JMP " + to_string(ll->result) + '\n';
			}
			//logic_op
			else if (ll->op == "'>='"|| ll->op == "'<='"|| ll->op == "'>'"|| ll->op == "'<'"|| ll->op == "'=='"|| ll->op == "'!='")
			{
				int s1=0;
				int s2=0;
				s1 = *symbol_map2.find(ll->arg1)->second->address.begin();
				s2 = *symbol_map2.find(ll->arg2)->second->address.begin();

				cout << "CMP " + to_string(s1) + "," + to_string(s2) + '\n';
				o << "CMP " + to_string(s1) + "," + to_string(s2) + '\n';
				cout << "CJ " + to_string(ll->result) + '\n';
				o << "CJ "+to_string(ll->result)+'\n';
			}
			//删除引用链中的信息
			if (ll->op != ":=")
			{
				if (ll->arg1 != -1)
					symbol_map2.find(ll->arg1)->second->yinyong.erase(ll->num);
				if (ll->arg2 != -1)
					symbol_map2.find(ll->arg2)->second->yinyong.erase(ll->num);
			}

		}
	}
	o.close();
}
int getres(code * l)
{
	if (l->op != ":=")
		if (symbol_map2.find(l->arg1)->second->address.size() != 0)//1
		{
			int h = *symbol_map2.find(l->arg1)->second->address.begin();
			//if (h < 0 && res[-h].size()==1&&liutu[l->node_num]->out.find(l->arg1)==liutu[l->node_num]->out.end())//在寄存器里1并且y不活跃
			//	return h;
			if (h < 0 && res[-h].size() == 1 && liutu[l->node_num]->out.find(l->arg1) == liutu[l->node_num]->out.end() && symbol_map2.find(l->arg1)->second->yinyong.size() == 1)//在寄存器里1并且y不活跃
			{
				cout << "MOV " + to_string(l->arg1) + to_string(h) + '\n';
				o << "MOV " + to_string(l->arg1) + to_string(h) + '\n';
				symbol_map2.find(l->arg1)->second->address.erase(h);
				symbol_map2.find(l->arg1)->second->address.insert(l->arg1);
				res[-h].erase(l->arg1);
				return h;
			}
		}

	for (int i = 1; i <= 16; i++)//2
		if (res[i].size() == 0)
			return (-i);

	if (symbol_map2.find(l->result)->second->yinyong.size() != 0)//3
	{
		//int yb = find_node.find(*symbol_map2.find(l->result)->second->yinyong.begin())->second;
		int max = 0;
		int maxnum = 0;
		//if (yb == find_node.find(l->num)->second)//本块中还会被引用
		if (symbol_map2.find(l->result)->second->yinyong.size() != 0)
		{
			for (int j = 1; j <= 16; j++)
			{
				set<int> cl = symbol_map2.find(*res[j].begin())->second->yinyong;
				if (cl.size() == 0)//---------------------------------------------把他们都写回到内存中
				{
					cout<< "MOV " + to_string(*res[j].begin()) + to_string(j) + '\n';
					o << "MOV " + to_string(*res[j].begin()) + to_string(j) + '\n';
					symbol_map2.find(*res[j].begin())->second->address.erase(j);
					symbol_map2.find(*res[j].begin())->second->address.insert(*res[j].begin());
					res[j].erase(*res[j].begin());
					return (-j);
				}
				else if (*cl.begin() > maxnum)
				{
					maxnum = *cl.begin();
					max = j;
				}
			}
			set<int>::iterator mm;
			for (mm = res[max].begin(); mm != res[max].end(); mm++)
			{
				cout<< "MOV " + to_string(*mm) + to_string(max) + '\n';
				o << "MOV " + to_string(*mm) + to_string(max) + '\n';
				symbol_map2.find(*mm)->second->address.erase(max);
				symbol_map2.find(*mm)->second->address.insert(*mm);
			}
			res[max].clear();
			return (-max);//-----------------------------------写回
		}
	}

	return l->result;

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
			code * temp_code=new code(x[top - 2]->addr, ":=", char_name.at(1), -1);
			x[top - 2]->codel.push_back(temp_code);
			gencode(temp_code);
		}
		else
		{
			int offset0 = sum_offset;
			enter(name, 0, "char", NULL, 1);
			code * temp_code=new code(offset0, ":=", char_name.at(1), -1);
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
			code * temp_code=new code(x[top - 2]->addr, "=", x[top]->addr, -1);
			x[top - 2]->codel = x[top]->codel;
			x[top - 2]->codel.push_back(temp_code);
			gencode(temp_code);
		}
		else
		{
			int offset0 = sum_offset;
			enter(name, 0, "int", NULL, 4);
			code * temp_code=new code(offset0, "=", x[top]->addr, -1);
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
			code * temp_code=new code(x[top - 2]->addr, ":=", char_name.at(1), -1);
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
			code * temp_code=new code(x[top - 2]->addr, "=", x[top]->addr, -1);
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
			code * temp_code=new code(offset0,":=",*gg,-1);
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
			code * temp_code=new code(offset0, ":=", *gg, -1);
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
		x[top]->addr = temp_offset_int;
		code * temp_code=new code(x[top]->addr, ":=", x[top]->vall[1], -1);
		x[top]->codel.push_back(temp_code);
		gencode(temp_code);
		temp_offset_int = temp_offset_int + 4;
	}
	else if (guiyueshi == "math_basic->'(' math_statement ')'") //code * temp_code=new code
	{
		x[top - 2] = x[top - 1];
		top = top - 2;
	}
	else if (guiyueshi == "math_basic->'-' math_basic")
	{
		x[top - 1] = x[top];
		code * temp_code=new code(x[top - 1]->addr, "-", x[top - 1]->addr, -1);//统一单操作符时仅调用arg1
		x[top]->codel.push_back(temp_code);
		x[top - 1]->codel = x[top]->codel;
		gencode(temp_code);
		top = top - 1;
	}
	else if (guiyueshi == "math_basic->'++' math_basic") //code * temp_code=new code -------------------先考虑不改变元变量值----------------------
	{
		x[top - 1] = x[top];
		code * temp_code=new code(x[top - 1]->addr, "++", x[top - 1]->addr, -1);//统一单操作符时仅调用arg1
		x[top]->codel.push_back(temp_code);
		x[top - 1]->codel = x[top]->codel;
		gencode(temp_code);
		top = top - 1;
	}
	else if (guiyueshi == "math_basic->'--' math_basic")
	{
		x[top - 1] = x[top];
		code * temp_code=new code(x[top - 1]->addr, "--", x[top - 1]->addr, -1);//统一单操作符时仅调用arg1
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
		code * temp_code=new code(x[top - 2]->addr, "+", x[top - 2]->addr, x[top]->addr);
		x[top - 2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);
		x[top - 2]->codel.push_back(temp_code);
		gencode(temp_code);
		top = top - 2;
	}
	else if (guiyueshi == "math_statement->math_statement '-' math_item")
	{
		code * temp_code=new code(x[top - 2]->addr, "-", x[top - 2]->addr, x[top]->addr);
		x[top - 2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);
		x[top - 2]->codel.push_back(temp_code);
		gencode(temp_code);
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
		code * temp_code = new code(x[top - 1]->addr, "!", x[top-1]->addr, -1);
		x[top - 1]->codel = x[top]->codel;
		x[top - 1]->codel.push_back(temp_code);
		gencode(temp_code);
		top = top - 1;
	}
	else if (guiyueshi == "logic_basic->math_statement logic_op math_statement")
	{
		x[top - 2]->codel.splice(x[top - 2]->codel.end(), x[top]->codel);
		code * temp_code1=new code(-3, x[top - 1]->logic_op,x[top-2]->addr,x[top]->addr);//if a<b goto nextquad+3
		x[top - 2]->addr = temp_offset_bool;
		x[top - 2]->codel.push_back(temp_code1);
		gencode(temp_code1);
		code * temp_code2=new code(x[top - 2]->addr, ":=", 0, -1);
		x[top - 2]->codel.push_back(temp_code2);
		gencode(temp_code2);
		code * temp_code3=new code(-2, "goto", -1, -1); //生成全部三地址码的时候-n表示去当前num的基础上加n的位置
		x[top - 2]->codel.push_back(temp_code3);
		gencode(temp_code3);
		code * temp_code4=new code(x[top - 2]->addr, ":=", 1, -1);
		x[top - 2]->codel.push_back(temp_code4);
		gencode(temp_code4);
		++temp_offset_bool;
		top = top - 2;
	}
	//if_statement
	else if (guiyueshi == "if_statement->if logic codeblockplus")
	{
		code * temp_code0 = new code(-2, "'=='", x[top - 1]->addr, 1);   //0
		gencode(temp_code0);
		code * temp_code1 = new code(-(int)(x[top]->codel.size() + 1), "goto", -1, -1); //1
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
		code* temp_code0 = new code(-2, "'=='", x[top - 3]->addr, 1); 
		gencode(temp_code0);
		code * temp_code1 = new code(-(int)(2 + x[top - 2]->codel.size()), "goto", -1, -1);
		gencode(temp_code1);
		code * temp_code2 = new code(-(int)(1+x[top]->codel.size()), "goto", -1, -1);
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
		code * temp_code0 = new code(-2, "'=='", x[top - 2]->addr, 1);
		gencode(temp_code0);
		code * temp_code1 = new code(-(int)(x[top]->codel.size() + 2), "goto", -1, -1);
		gencode(temp_code1);
		code * temp_code2 = new code(-(int)(2 + x[top]->codel.size() + x[top - 2]->codel.size()), "rgoto", -1, -1);
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
		code * temp_code0 = new code(-2,"'=='",x[top-4]->addr,1);
		gencode(temp_code0);
		code * temp_code1 = new code(-(int)(x[top]->codel.size()+x[top-2]->codel.size() + 2), "goto", -1, -1);
		gencode(temp_code1);
		code * temp_code2 = new code(-(int)(x[top]->codel.size() + x[top - 2]->codel.size()+ x[top - 4]->codel.size() + 2), "rgoto", -1, -1);
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
	ofstream o;
	o.open("C:\\Users\\daisf\\Documents\\cworkspace\\compiler\\compiler\\quatriple.txt");
	for (gg = sum_code.begin(); gg != sum_code.end();gg++)
	{
		code * l = *gg;
		l->num = num;
		if (l->result < 0)
		{
			if (l->op == "rgoto")
			{
				l->result = num + l->result;
				l->op = "goto";
			}
			else
				l->result = num - l->result;
		}
		//cout << num<<":   "<<to_string(l->result) + " " + l->op + " " + to_string(l->arg1) + " " + to_string(l->arg2) << endl;
		string out = to_string(num) + " " + to_string(l->result) + " " + l->op + " " + to_string(l->arg1) + " " + to_string(l->arg2) + '\n';
		o << out;
		num++;
	}
	o.close();
}

string make_op(string op)
{
	string s;
	if (op == "+")
		s= "ADD";
	else if (op == "-")
		s= "SUB";
	else if (op == "*")
		s= "MUL";
	else if (op == "/")
		s= "DIV";
	else if (op == "++")
		s= "INC";
	else if (op == "--")
		s= "DEC";
	else if (op == "'>'")
		s= "GT";
	else if (op == "'<'")
		s= "LT";
	else if (op == "'>='")
		s= "GE";
	else if (op == "'<='")
		s= "LE";
	else if (op == "'=='")
		s= "EQ";
	else if (op == "'!='")
		s= "NE";
	else if (op == "||")
		s= "OR";
	else if (op == "&&")
		s= "AND";
	else if (op == "!")
		s= "NOT";
	return s;
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