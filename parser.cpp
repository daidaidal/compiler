#include "paser.h"

using namespace std;
typedef multimap<string, node*>::iterator sn_it;
typedef map<string, set<string>>::iterator ss_it;
typedef set<string>::iterator set_it;
typedef map<string, set<node*>>::iterator sset_it;
gloable_variablep *p = new gloable_variablep();
//标准化处理
int parser::s()
{
	string s = "";
	while (p->buffer[p->buffer_count] == ' ' || p->buffer[p->buffer_count] == '\t' || p->buffer[p->buffer_count] == '\n')
	{
		if (p->buffer[p->buffer_count] == EOF)
			return 0;
		p->buffer_count++;
	}
	while (isalnum(p->buffer[p->buffer_count]) || p->buffer[p->buffer_count] == '_')
	{
		if (p->buffer[p->buffer_count] == EOF) //
			return 0;
		s = s + p->buffer[p->buffer_count];
		p->buffer_count++;
	}
	//string or char
	while (1)
	{
		string ps = "";
		while (!(isalnum(p->buffer[p->buffer_count]) || p->buffer[p->buffer_count] == '\'' || p->buffer[p->buffer_count] == '\"'))
		{
			if (p->buffer[p->buffer_count] == EOF)
				return 0;
			if (p->buffer[p->buffer_count] == ';')
			{
				p->buffer_count++;
				return 1;
			}
			p->buffer_count++;
		}
		while (p->buffer[p->buffer_count] != '\n')
		{
			if (p->buffer[p->buffer_count] == EOF)
				return 0;
			ps = ps + p->buffer[p->buffer_count];
			p->buffer_count++;
		}
		p->buffer_count++;
		//新建节点
		p->int_string_count++;
		p->q_string_int.insert(pair<string,int>(s + "->" + ps, p->int_string_count));
		p->q_int_string.insert(pair<int, string>(p->int_string_count, s + "->" + ps));
		if (ps == "'$'")//-------------------------------------------------------------
			p->null_set.insert(s);
		ps = '@' + ps;
		p->start->next = new node(s, ps);
		p->start = p->start->next;
		p->a.insert(pair<string, node*>(s, p->start));
		set<string> y;
		p->first_set.insert(pair<string,set<string>>(s, y)); //有可能连接着同一个y
	}
	return 1;
}
void parser::standardize()
{
	int count = 0;//记录第多少个节点
	p->pn = p->start;
	ifstream in;
	in.open(p->file_name);
	if (!in)
	{
		cout << "open error" << endl;
	}
	int sum_num = 0;
	while (!in.eof())
	{
		in.read(p->buffer + sum_num, 1);
		sum_num++;
	}
	sum_num--;
	p->buffer[sum_num] = EOF;

	//start
	while (s());
	return;
}
//递归版本
set<string> parser::make_first(string s)
{
	//终结符号表
	//如果找不到别的以他为首字母的就是终结符
	if (p->in_recalculate.find(s) == p->in_recalculate.end()) //如果没有加入过recalculate
	{
		p->in_recalculate.insert(s);
		p->recalculate.push_front(s);
	}
	p->already_in_first.insert(s);
	ss_it f_s = p->first_set.find(s);
	//if (p->null_set.find(s) != p->null_set.end())
		//f_s->second.insert("'$'");
	pair<sn_it, sn_it> f = p->a.equal_range(s);
	for (sn_it k = f.first; k != f.second; k++)
	{
		//把每个的第一个加进去,如果可能出现空的话就把后面的加进去 while(1) if 通过string 剔除第一个 第二个 。。第 n个
		int at = k->second->p.find("@");
		int at2 = at;
		string first = "";

		list<string>firsts;

		for (int lll = at + 1; lll < k->second->p.length(); lll++)
		{
			if (k->second->p.at(lll) == ' ')
			{
				if (p->null_set.find(first) != p->null_set.end())
				{
					firsts.push_front(first);
					first = "";
				}
				else
					break;
			}
			first = first + k->second->p.at(lll);
		}
		if (p->null_set.find(first) != p->null_set.end())
			f_s->second.insert("'$'");
		firsts.push_front(first);

		for (list<string>::iterator uu = firsts.begin(); uu != firsts.end(); uu++)
		{
			first = *uu;
			set<string> temp;
			if (p->first_set.find(first) == p->first_set.end())
			{
				f_s->second.insert(first);
				continue;
			}
			if (p->already_in_first.find(first) != p->already_in_first.end()) //如果已经算过make_first(first)
			{
				//需要重新进行计算的
				if (p->judge_first_count == 0)
				{
					if (p->in_recalculate.find(first) != p->in_recalculate.end())//如果加入过recalculate
						continue;
					p->in_recalculate.insert(first);
					p->recalculate.push_front(first);
				}
				else
				{
					ss_it temp_it = p->first_set.find(first);
					int before = f_s->second.size();
					f_s->second.insert(temp_it->second.begin(), temp_it->second.end());
					if (f_s->second.size() > before)
						p->judge_end = 1;
				}
				continue;
			}
			temp = make_first(first);
			set<string>::iterator tt = temp.find("'$'");
			if (tt != temp.end())
			{
				p->null_set.insert(first);
				temp.erase(tt);
			}
			f_s->second.insert(temp.begin(), temp.end());
		}
	}
	return f_s->second;
}
void parser::first()
{

	//make_first(init_word); //初始化
	ss_it l;
	for (l = p->first_set.begin(); l != p->first_set.end(); l++)
		make_first(l->first);
	p->judge_first_count = 1; //跳过内部形成栈代码
	while (1)
	{
		p->judge_end = 0;
		for (list<string>::iterator ttt = p->recalculate.begin(); ttt != p->recalculate.end(); ttt++)
			make_first(*ttt);
		if (p->judge_end == 0)
			break;
	}

}

int parser::closure(set<node*> input_set)
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
		stackl.push_back(new_temp);
		temp_map.insert(pair<string, node*>(new_temp->s, new_temp));
	}
	set<string> insert_symbol;
	while (!stackl.empty())
	{
		//cout << stackl.size() << endl;
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
		if (first=="")
			continue;
		if (p->first_set.find(first) == p->first_set.end())
			continue;

		string second = "";
		if (judge_cc == 0) //如果后面没有非终结符
			insert_symbol.insert(n->symbol.begin(), n->symbol.end());
		else
		{
			list<string> seconds;
			at = n->p.find(" ", n->p.find("@"));
			for (int i = at + 1; i < n->p.size(); i++)
			{
				if (n->p.at(i) == ' ')
				{
					if (p->null_set.find(second) != p->null_set.end())
					{
						seconds.push_front(second);
						second = "";
					}
					else
						break;
				}
				second = second + n->p.at(i);
			}

			seconds.push_front(second);
			for (list<string>::iterator uu = seconds.begin(); uu != seconds.end(); uu++)
			{
				second = *uu;
				ss_it first_second = p->first_set.find(second);
				if (first_second != p->first_set.end()) //如果是非终结符
				{
					set<string>::iterator set_it;
					insert_symbol.insert(first_second->second.begin(), first_second->second.end());
				}
				else //如果是终结符
					insert_symbol.insert(second);
			}
		}


		if (setl.find(first) == setl.end())
		{
			//cout << "first://" << first << endl;
			pair<sn_it, sn_it> ret = p->a.equal_range(first);
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
					//if (j->second->p.at(0) != '@') //----------------------
						//continue;
					j->second->symbol.insert(tryl.begin(), tryl.end());
					stackl.push_back(j->second);
					//judge_count++;
					//cout << judge_count << endl;
				}
		}
	}
	p->state_count++;
	p->state_map[p->state_count] = temp_map;
	return p->state_count;
}


int parser::find_all2(set<node*> ll) //将新生成的闭包和以前的闭包进行对比
{
	int judge = 0;
	for (int i = 1; i <= p->state_count; i++)
	{
		judge = 0;
		if (p->check_in_state[i].size() != ll.size())
			continue;
		for (set<node*>::iterator gg = ll.begin(); gg != ll.end(); gg++)
		{
			node * temp = *gg;
			string temps = "";
			for (set<string>::iterator temp2 = temp->symbol.begin(); temp2 != temp->symbol.end(); temp2++)
				temps = temps + *temp2;
			if (p->check_in_state[i].find(temp->s + "->" + temp->p + temps) != p->check_in_state[i].end())
				continue;
			else
			{
				judge = -1;
				break;
			}
		}
		if (judge == -1)
			continue;
		else
			return i;
	}
	return -1;
}

void parser::make_list(set<node*>input_set)
{
	list<set<node*>> listl;
	listl.push_back(input_set);
	//
	while (1)
	{
		if (listl.size()==0)
			break;
		int lsize = listl.size();
		//cout << lsize << endl;
		set<node*> init_set = listl.front();
		set<node*> after_set;
		listl.pop_front();
		//int gggg = find_all1(init_set);

		map<string, set<node*>> zhuanyibiao;
		zhuanyibiao.clear();
		int state_countl = closure(init_set);
		for (map<string, node*>::iterator xx = p->state_map[state_countl].begin(); xx != p->state_map[state_countl].end(); xx++)
			after_set.insert(xx->second);

		int gggg = find_all2(after_set);
		//if (gggg != -1 && gggg != -2)
		if(gggg!=-1 && gggg!=state_countl)
		{
			//if (p->state_map[gggg].size() == p->state_map[state_countl].size())
			//{
				p->state_count--;
				continue;
			//}
		}

		for (sn_it temp_it = p->state_map[state_countl].begin(); temp_it != p->state_map[state_countl].end(); temp_it++)
		{
			node * temp = temp_it->second;
			string temps = "";
			for (set<string>::iterator temp2 = temp->symbol.begin(); temp2 != temp->symbol.end(); temp2++)
				temps = temps + *temp2;
			p->check_in_state[state_countl].insert(temp->s + "->" + temp->p + temps);

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

void parser::make_list3()
{
	int state_count_temp = 1;
	while (state_count_temp <= p->state_count)
	{
		map<string, set<node*>> zhuanyibiao;
		zhuanyibiao.clear();
		string zhuanyifu = "";
		int findl;
		for (sn_it temp_it = p->state_map[state_count_temp].begin(); temp_it != p->state_map[state_count_temp].end(); temp_it++)
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
				map<string,int>::iterator lol = p->q_string_int.find(temp->s + "->" + s);
				if (lol == p->q_string_int.end())
					cout << "baozhala" << endl;
				int ll = lol->second;
				for (set<string>::iterator lg = temp->symbol.begin(); lg != temp->symbol.end(); lg++)
				{
					map<string, string>::iterator judgegg;
					judgegg = p->go.find(to_string(state_count_temp) + " " + *lg);
					if (judgegg != p->go.end())
						if (judgegg->second != "r" + to_string(ll))
							p->judgecount++;
					map<string, int>::iterator find_hong = p->duizhao.find(*lg);
					if(find_hong!= p->duizhao.end())
						p->go.insert(pair<string, string>(to_string(state_count_temp) + " " + to_string(find_hong->second), "r" + to_string(ll)));
					else
						p->go.insert(pair<string, string>(to_string(state_count_temp) + " " + *lg, "r" + to_string(ll)));
				}

			}
			else
			{
				if (check == 0)
					s = s + '@';

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
			set<node*> after_set;
			map<string, set<node*>> zhuanyibiao;
			zhuanyibiao.clear();
			int state_countl = closure(gg->second);
			for (map<string, node*>::iterator xx = p->state_map[state_countl].begin(); xx != p->state_map[state_countl].end(); xx++)
				after_set.insert(xx->second);

			int all = find_all2(after_set);
			if (all != -1 && all != state_countl)
				p->state_count--;
			else
				p->judgecount++;
			if (all == -1)
				p->judgecount++;
			
			map<string, string>::iterator judgegg;
			judgegg = p->go.find(to_string(state_count_temp) + " " + gg->first);
			if (judgegg != p->go.end())
				if (judgegg->second != to_string(all))
					p->judgecount++;
			map<string, int>::iterator find_hong = p->duizhao.find(gg->first);
			if (find_hong != p->duizhao.end())
				p->go.insert(pair<string, string>(to_string(state_count_temp) + " " + to_string(find_hong->second),to_string(all)));
			else
				p->go.insert(pair<string, string>(to_string(state_count_temp) + " " + gg->first, to_string(all)));
		}
		state_count_temp++;
	}
}

gloable_variablep * parser::mainfunction()
{
	set<node*>input_set_init;
	standardize();
	first();
	make_duizhao();
	pair<sn_it, sn_it> g = p->a.equal_range(p->init_word);
	cout<<"标准化："<<endl;
	//for (sn_it lll = p->a.begin(); lll != p->a.end(); lll++)
		//cout << lll->first << "->" << lll->second->p << endl;
	//给最初表达式初始化搜索符号
	for (sn_it temp = g.first; temp != g.second; temp++)
		temp->second->symbol.insert("#");
	for (sn_it temp = g.first; temp != g.second; temp++)
		input_set_init.insert(temp->second);
	make_list(input_set_init);
	make_list3();
	/*
	cout << endl << "closure" << endl;
	for (int i = 1; i <= p->state_count; i++)
	{
		cout << "closure:" << i << endl;
		for (sn_it ii = p->state_map[i].begin(); ii != p->state_map[i].end(); ii++)
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
	for (ss_it itt = p->first_set.begin(); itt != p->first_set.end(); itt++)
	{
		cout << endl<<itt->first << ":";
		set<string>::iterator si;
		for (si = itt->second.begin();si != itt->second.end();si++)
			cout << *si << " ";
	}
	*/
	cout << endl;
	return p;
}
void parser::make_duizhao()
{
	p->duizhao.insert(pair<string, int>("if", IF));
	p->duizhao.insert(pair<string, int>("else", ELSE));
	//p->duizhao.insert(pair<string, int>("elseif", ELSEIF));
	p->duizhao.insert(pair<string, int>("for", FOR));
	p->duizhao.insert(pair<string, int>("while", WHILE));
	p->duizhao.insert(pair<string, int>("break",BREAK));
	p->duizhao.insert(pair<string, int>("continue", CONTINUE));
	p->duizhao.insert(pair<string, int>("return", RETURN));
	p->duizhao.insert(pair<string, int>("main", MAIN));
	p->duizhao.insert(pair<string, int>("int_type", INTEGER_TYPE));
	p->duizhao.insert(pair<string, int>("char_type", CHAR_TYPE));
	p->duizhao.insert(pair<string, int>("void_type", VOID_TYPE));
	p->duizhao.insert(pair<string, int>("include", INCLUDE));
	//标识符
	p->duizhao.insert(pair<string, int>("id", ID));
																  //整常数 字符串常数
	p->duizhao.insert(pair<string, int>("int", INT));
	p->duizhao.insert(pair<string, int>("string", STRING));
	p->duizhao.insert(pair<string, int>("char", CHAR));
	//运算符
	//算术运算符
	p->duizhao.insert(pair<string, int>("'+'", ADD));  //  +
	p->duizhao.insert(pair<string, int>("'-'", SUB));  //  -
	p->duizhao.insert(pair<string, int>("'*'", MULTI));  //  *
	p->duizhao.insert(pair<string, int>("'/'", DIV));  // /
	p->duizhao.insert(pair<string, int>("'%'", MOD));  //  %
	p->duizhao.insert(pair<string, int>("'++'", ADDD));  //  ++
	p->duizhao.insert(pair<string, int>("'--'", SUBB));  //  --
														 //关系运算符
	p->duizhao.insert(pair<string, int>("'<'", SMALL));  //  >
	p->duizhao.insert(pair<string, int>("'>'", BIG));  //  <
	p->duizhao.insert(pair<string, int>("'>='", NSMALL));  //  >=
	p->duizhao.insert(pair<string, int>("'<='", NBIG));  //  <=
	p->duizhao.insert(pair<string, int>("'=='", EQUAL));  // ==
	p->duizhao.insert(pair<string, int>("'!='", NEQUAL));  //  !=
														   //逻辑运算符
	p->duizhao.insert(pair<string, int>("'&&'", AND));  //  &&
	p->duizhao.insert(pair<string, int>("'||'", OR));  //  ||
	p->duizhao.insert(pair<string, int>("'!'", NOT));  //  !
													   //赋值
	p->duizhao.insert(pair<string, int>("'='", ASSIGN));  // =
														  //分界符
	p->duizhao.insert(pair<string, int>("'('", LS));  //  (
	p->duizhao.insert(pair<string, int>("')'", RS));  //  )
	p->duizhao.insert(pair<string, int>("'{'", LB));  //  {
	p->duizhao.insert(pair<string, int>("'}'", RB));  //  }
	p->duizhao.insert(pair<string, int>("'['", LM));  //  [
	p->duizhao.insert(pair<string, int>("']'", RM));  //  ]
	p->duizhao.insert(pair<string, int>("','", COMMA));  //  ,
	p->duizhao.insert(pair<string, int>("':'", COLON));  //  :
	p->duizhao.insert(pair<string, int>("';'", SEMIC));  //  \;
	
	p->duizhao.insert(pair<string, int>("#", OIL));  //  #
	//p->duizhao.insert(pair<string, int>("\"", QUO));  //  "
	//p->duizhao.insert(pair<string, int>("\'", SQUO));  //  '
}