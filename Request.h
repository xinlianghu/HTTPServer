#include<string>
#include<vector>
#include<map>
#include "HTTPType.h"
#pragma once
using namespace std;

class Request
{
public:
	map<string, int> methodMap;
	RequestLine requestLine;
	vector<pair<string, string>> option;	
	char *data;
public:
	Request();
	~Request();
	bool parseRequest(char res[], int len);
	bool parseRequestLine(string str);
	bool parseOption(string str);
	string getURl();
	string getRequestParm();
	string getParm(string key);
	int getMethod();
};


