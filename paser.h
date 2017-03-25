#include<iostream>
#include<string>
#include <fstream> 
#include <set>
#include "hong.h"
using namespace std;
class node {
public:
	string s;
	string p;
	set<string>symbol;
	node *next = NULL;
	node(string s, string p)
	{
		this->s = s;
		this->p = p;
	}

};