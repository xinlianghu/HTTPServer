#include "HTTPType.h"
#include "map"
#include <time.h>
#pragma once
class Responce
{
public:
	StatusLine statusLine;
	map<string, string> option;
	char * data;
	int len;
public:
	bool setParm(string key, string value);
	int setData(const char *buf,int len);
	bool setStatusCode(string code);
	bool setStatusMsg(string msg);
	Responce();
	~Responce();
	int getResponce(char * &buf);
};

