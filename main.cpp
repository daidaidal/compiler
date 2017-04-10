#include "lex.h"
#include <iostream>
using namespace std;
void main()
{
	lex *cifa= new lex();
	cifa->mainfunction();
	cout << "词法分析到此结束" << endl;
	return;
}
