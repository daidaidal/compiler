#include "paser.h"
#include <iostream>
using namespace std;
void main()
{
	lex *cifa = new lex();
	parser *yufa = new parser();
	gloable_variable *l = cifa->mainfunction();
	gloable_variablep * p = yufa->mainfunction();
	stack<int> s;
	stack<string> x;
	s.push(1);
	x.push("#");
	int judge = 0;
	while (judge != 1)
	{
		for (int i = 0;; i++)
		{
			int * m = l->save[i];
			if (m[0] == -1)
				break;
			string come = "";
			come = s.top() + " " + to_string(m[1]);
			map<string, string>::iterator gg = p->go.find(come);
			if (gg != p->go.end())
			{
				string st = gg->second;
				if (st.at(0) != 'r') //转移
				{
					int num0 = 0;
					for (int j = 0; j < st.size(); j++)
						num0 = num0 + (st.at(j) - '0');
					s.push(num0);
					x.push(to_string(m[0]));
				}
				else //规约
				{
					int num = 0;
					for (int j = 1; j < st.size(); j++)
						num = num + (st.at(j) - '0');
					if (num == 1)
					{
						judge = 1;
						cout << "success" << endl;
						break;
					}
					map<int, string>::iterator find_guiyue = p->q_int_string.find(num);

					string guiyueshi = find_guiyue->second;
					int flag = 0;
					i = 1;
					string guiyue = "";
					int count = 0;
					while(i<guiyueshi.size())
					{
						if(flag==0)
							guiyue = guiyue + guiyueshi.at(i);
						if (guiyueshi.at(i) == '-' && guiyueshi.at(i + 1) == '>')
						{
							i = i + 2;
							flag = 1;
							count++;
							continue;
						}
						if (guiyueshi.at(i) == ' ')
							count++;
					}

					//上面获取了s和count
					while (count != 0)
					{
						s.pop();
						x.pop();
						count--;
					}

					//移进
					string zhuanyi = p->go.find(to_string(s.top()) + ' ' + guiyue)->second;
					int num1 = 0;
					for (int j = 0; j < zhuanyi.size(); j++)
						num1 = num1 + (zhuanyi.at(j) - '0');
					s.push(num1);
					x.push(zhuanyi);


				}
			}

		}
	}


	return;
}

