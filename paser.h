#include<iostream>
#include<string>
#include <fstream> 
#include "hong.h"
using namespace std;
class node {
public:
	string s;
	string p;
	string symbol;
	node *next = NULL;
	node(string s, string p,string symbol)
	{
		this->s = s;
		this->p = p;
		this->symbol = symbol;
	}

};