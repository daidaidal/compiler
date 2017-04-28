#include "lex.h"

using namespace std;
gloable_variable *g = new gloable_variable();
void lex::read()
{
	if (g->boundary == 1)
	{
		g->boundary = 0;
		return;
	}
	if (g->qian == 4095)
	{
		for (int i = 4096; i < 8192; i++) // 读取4095个
		{
			if (!g->in.eof())
				g->in.read(g->buffer + i, 1);
			else
			{
				g->buffer[i-1] = EOF;
				break;
			}
		}
	}
	else if (g->qian == 8191)
	{
		for (int i = 0; i < 4096; i++) // 读取4095个
		{
			if (!g->in.eof())
				g->in.read(g->buffer + i, 1);
			else
			{
				g->buffer[i-1] = EOF;
				break;
			}
		}
	}
}
void lex::retract()
{
	g->qian--;
	if (g->qian == -1)
	{
		g->boundary = 1;
		g->qian = 8190;
	}
	else if (g->qian == 4095)
	{
		g->boundary = 1;
		g->qian = 4094;
	}
}
char lex::getcharl()
{
	char a = g->buffer[g->qian];
	if (a == EOF)
	{
		if (g->qian == 4095 || g->qian == 8191)
		{
			read();
			g->qian = (g->qian + 2) % 8192;
			return g->buffer[g->qian - 1];
		}
		else
		{
			g->qian++;
			return EOF; //程序结束
		}
	}
	g->qian++;
	return g->buffer[g->qian - 1]; //返回当前位并且指针向后移一位
}
char * lex::copytoken()
{
	int len = g->qian - g->hou;
	char * a = new char[len + 1];
	for (int i = 0; i<len; i++)
	{
		a[i] = g->buffer[g->hou + i];
	}
	a[len] = '\0';
	g->hou = g->qian;
	cout << a;
	return a;
}
int lex::gettoken(char * token)
{
	if (!strcmp(token, "if"))
		return IF;
	else if (!strcmp(token, "else"))
		return ELSE;
	else if (!strcmp(token, "for"))
		return FOR;
	else if (!strcmp(token, "while"))
		return WHILE;
	else if (!strcmp(token, "break"))
		return BREAK;
	else if (!strcmp(token, "continue"))
		return CONTINUE;
	else if (!strcmp(token, "return"))
		return RETURN;
	else if (!strcmp(token, "main"))
		return MAIN;
	else if (!strcmp(token, "int"))
		return INTEGER_TYPE;
	else if (!strcmp(token, "char"))
		return CHAR_TYPE;
	else if (!strcmp(token, "include"))
		return INCLUDE;
	else if (!strcmp(token, "void"))
		return VOID_TYPE;
	else if (!strcmp(token, "printf"))
		return PRINTF;
	else return ID;
}
void lex::error_handle(int num)
{
	errorl *temp = new errorl(g->error_hang, num);
	g->e[g->count_e] = temp;
	g->count_e++;
}
int lex::install_id(char * token)
{
	for (int i = 0; i < g->countl; i++)
		if (!strcmp(g->list[i], token))
			return i;
	g->list[g->countl] = token;
	g->countl++;
	return (g->countl - 1);
}
int lex::install_string(char * token)
{
	g->string_list[g->count_sl] = token;
	g->count_sl++;
	return (g->count_sl - 1);
}
int lex::install_char(char * token)
{
	g->char_list[g->count_cl] = token;
	g->count_cl++;
	return (g->count_cl - 1);
}
int lex::change_num(char * token)
{
	int sum = 0;
	int i = strlen(token);
	for (int j = 0; j < i; j++)
	{
		sum = sum + (token[j] - '0')*pow(10, i - 1 - j);
	}
	return sum;
}
void lex::left(int a[][2], int &count)
{
	a[count][0] = g->hang;
	a[count][1] = -1;
	count++;
	return;
}
void lex::right(int a[][2], int &count)
{
	if (count > 0)
		if (a[count - 1][1] == -1)
		{
			count--;
			return;
		}

	a[count][0] = g->hang;
	a[count][1] = 1;
	count++;
	return;
}
int * lex::token_scan()
{
	int * ret = new int[2];
	char ch;
	char * token = NULL;
	ch = getcharl();
	if (ch == EOF)
	{
		ret[0] = EOF;
		ret[1] = EOF;
		return ret;
	}
	while (ch == ' ' || ch == '\n' || ch == '\t')
	{
		if (ch == '\n')
			g->hang++;
		g->hou = g->qian;
		ch = getcharl();
		if (ch == EOF)
		{
			ret[0] = EOF;
			ret[1] = EOF;
			return ret;
		}
	}
	if (isalpha(ch))
	{
		ch = getcharl();
		while (isalnum(ch) || ch == '_')
		{
			ch = getcharl();
		}
		retract();
		token = copytoken();
		int num = gettoken(token);
		if (num == ID)
		{
			ret[0] = gettoken(token);
			ret[1] = install_id(token);
			return ret; //标识符
		}
		else
		{
			//关键字
			ret[0] = num;
			ret[1] = 0;
			return ret;
		}
	}
	else
		if (isdigit(ch))
		{
			ch = getcharl();
			while (isdigit(ch))
			{
				ch = getcharl();
			}
			retract();
			token = copytoken();
			ret[0] = INT;
			ret[1] = change_num(token);
			return ret;
		}
		else
			switch (ch)
			{
			case '+':ch = getcharl();
				if (ch == '+')
				{
					ret[0] = ADDD;
					ret[1] = 0;
					return ret;
				}
				else
				{
					retract();
					ret[0] = ADD;
					ret[1] = 0;
					return ret;
				}break;
			case '-':ch = getcharl();
				if (ch == '-')
					return (SUBB, 0);
				else
				{
					retract();
					ret[0] = SUB;
					ret[1] = 0;
					return ret;
				}break;
			case '*':
				ret[0] = MULTI;
				ret[1] = 0;
				return ret;
				break;
			case '/':ch = getcharl();
				if (ch == '/')
				{
					ch = getcharl();
					while (ch != '\n')
					{
						ch = getcharl();
					}
					g->hang++;
					ret[0] = NULL;
					ret[1] = NULL;
					return ret;
				}
				else if (ch == '*')
				{
					g->error_hang = g->hang;
					ch = getcharl();
					while (1)
					{
						if (ch == EOF)
						{
							error_handle(9);
							ret[0] = EOF;
							ret[1] = EOF;
							return ret;
						}
						if (ch == '*')
						{
							ch = getcharl();
							if (ch == '/')
							{
								ret[0] = NULL;
								ret[1] = NULL;
								return ret;
							}
						}
						ch = getcharl();
					}
				}
				else
				{
					retract();
					ret[0] = DIV;
					ret[1] = 0;
					return ret;
				}break;
			case '%':
				ret[0] = MOD;
				ret[1] = 0;
				return ret;
				break;
			case '>':ch = getcharl();
				if (ch == '=')
				{
					ret[0] = NSMALL;
					ret[1] = 0;
					return ret;
				}
				else
				{
					retract();
					ret[0] = BIG;
					ret[1] = 0;
					return ret;
				}break;
			case '<':ch = getcharl();
				if (ch == '=')
				{
					ret[0] = NBIG;
					ret[1] = 0;
					return ret;
				}
				else
				{
					retract();
					ret[0] = SMALL;
					ret[1] = 0;
					return ret;
				}break;
			case '=':ch = getcharl();
				if (ch == '=')
				{
					ret[0] = EQUAL;
					ret[1] = 0;
					return ret;
				}
				else
				{
					retract();
					ret[0] = ASSIGN;
					ret[1] = 0;
					return ret;
				}break;
			case '!':ch = getcharl();
				if (ch == '=')
				{
					ret[0] = NEQUAL;
					ret[1] = 0;
					return ret;
				}
				else
				{
					retract();
					ret[0] = NOT;
					ret[1] = 0;
					return ret;
				}break;
			case '&':ch = getcharl();
				if (ch == '&')
				{
					ret[0] = ADD;
					ret[1] = 0;
					return ret;
				}
				else
				{
					retract();
					g->error_hang = g->hang;
					error_handle(0);
				}
				break;
			case '|':ch = getcharl();
				if (ch == '|')
				{
					ret[0] = OR;
					ret[1] = 0;
					return ret;
				}
				else
				{
					retract();
					g->error_hang = g->hang;
					error_handle(0);
				}
				break;
			case '(':
				left(g->S, g->count_s);
				ret[0] = LS;
				ret[1] = 0;
				return ret;
				break;
			case ')':
				right(g->S, g->count_s);
				ret[0] = RS;
				ret[1] = 0;
				return ret;;
				break;
			case '{':
				left(g->B, g->count_b);
				ret[0] = LB;
				ret[1] = 0;
				return ret;;
				break;
			case '}':
				right(g->B, g->count_b);
				ret[0] = RB;
				ret[1] = 0;
				return ret;;
				break;
			case '[':
				left(g->M, g->count_m);
				ret[0] = LM;
				ret[1] = 0;
				return ret;;
				break;
			case ']':
				right(g->M, g->count_m);
				ret[0] = RM;
				ret[1] = 0;
				return ret;
				break;
			case ',':ret[0] = COMMA;
				ret[1] = 0;
				return ret;
				break;
			case ':':ret[0] = COLON;
				ret[1] = 0;
				return ret;
				break;
			case ';':ret[0] = SEMIC;
				ret[1] = 0;
				return ret;
				break;
			case '#':ret[0] = OIL;
				ret[1] = 0;
				return ret;
				break;
			case '\'':ch = getcharl();
				if (ch == '\n')
				{
					g->error_hang = g->hang;
					g->hang++;
					error_handle(1);
					break;
				}
				while (ch != '\'')
				{
					ch = getcharl();
					if (ch == '\n')
					{
						g->error_hang = g->hang;
						g->hang++;
						error_handle(1);
						ret[0] = NULL;
						ret[1] = NULL;
						return ret;
					}
				}
				token = copytoken();
				ret[0] = CHAR;
				ret[1] = install_char(token);
				return ret;
				break;
			case '\"':ch = getcharl();
				if (ch == '\n')
				{
					g->error_hang = g->hang;
					g->hang++;
					error_handle(2);
					break;
				}
				while (ch != '\"')
				{
					ch = getcharl();
					if (ch == '\n')
					{
						g->error_hang = g->hang;
						g->hang++;
						error_handle(2);
						ret[0] = NULL;
						ret[1] = NULL;
						return ret;
					}
				}
				token = copytoken();
				ret[0] = STRING;
				ret[1] = install_string(token);
				return ret;
			default:g->error_hang = g->hang;
				error_handle(0); break;
			}
	ret[0] = NULL;
	ret[1] = NULL;
	return ret;
}

gloable_variable * lex::mainfunction()
{
	//缓冲区的设置 读取文件
	//错误处理 错误恢复
	//
	string file_name;
	int i = 0;
	//cout << "请输入文件名：";
	//getline(cin, file_name, '\n');
	g->in.open("input.txt");
	if (!g->in)
	{
		cout << "open errorl" << endl;
	}
	g->buffer[4095] = EOF;
	g->buffer[8191] = EOF;
	int save_count = 0;
	while (1)
	{
		int *ret;
		ret = token_scan();
		g->hou = g->qian;
		if (ret[0] == EOF)
		{
			g->save[save_count][0] = -1;
			break;
		}
		else if (ret[0] == NULL)
			continue;
		cout << '(' << ret[0] << "," << ret[1] << ")" << endl;
		g->save[save_count][0] = ret[0];
		g->save[save_count][1] = ret[1];
		save_count++;
	}
	g->in.close();
	int temp = 0;
	for (int j = 0; j < g->count_s; j++)
	{
		if(g->S[j][1] == -1)
			temp = 3;
		else
			temp = 4;
		errorl *feng = new errorl(g->S[j][0], temp);
		g->e[g->count_e] = feng;
		g->count_e++;
	}
	for (int j = 0; j < g->count_m; j++)
	{
		if (g->M[j][1] == -1)
			temp = 5;
		else
			temp = 6;
		errorl *feng = new errorl(g->M[j][0], temp);
		g->e[g->count_e] = feng;
		g->count_e++;
	}
	for (int j = 0; j < g->count_b; j++)
	{
		if (g->B[j][1] == -1)
			temp = 7;
		else
			temp = 8;
		errorl *feng = new errorl(g->B[j][0], temp);
		g->e[g->count_e] = feng;
		g->count_e++;
	}

	cout << endl;
	map<string, int> duizhao;
	duizhao.insert(pair<string, int>("lala", IF));
	//delete g;
	cout << "delete" << endl;
	return g;
}
