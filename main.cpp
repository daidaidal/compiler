#include "paser.h"
#include <iostream>
using namespace std;
int duandian = 0;
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
		if (gg != p->go.end())
		{
			string st = gg->second;
			if (st.at(0) != 'r') //转移
			{
				int num0 = 0;
				for (int j = 0; j < st.size(); j++)
					num0 = num0 * 10 + (st.at(j) - '0');
				s.push(num0);
				x.push(to_string(m[0]));
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

				if (guiyue == "s")
				{
					cout << "success" << endl;
					break;
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
					num1 = num1 * 10 + (zhuanyi.at(j) - '0');
				s.push(num1);
				x.push(guiyue);
				i = i - 1;


			}
		}
		else
			continue;

	}



	return;
}

