#include "Request.h"
Request::Request()
{
	this->data = NULL;
	this->methodMap.insert(pair<string,int>("GET",1));
	this->methodMap.insert(pair<string, int>("POST", 2));
	this->methodMap.insert(pair<string, int>("HEAD", 3));
	this->methodMap.insert(pair<string, int>("PUT", 4));
	this->methodMap.insert(pair<string, int>("DELETE", 5));
	this->methodMap.insert(pair<string, int>("OPTION", 6));
	this->methodMap.insert(pair<string, int>("TRACE", 7));
	this->methodMap.insert(pair<string, int>("CONNECT", 8));

}
Request::~Request()
{
	if (this->data != NULL)
	{
		delete[] this->data;
	}
	this->data = NULL;
	memset(&this->requestLine, 0, sizeof(RequestLine));
	this->option.clear();
}
bool Request::parseRequest(char res[], int len)
{
	string str(res);
	string requestLine;
	int index = str.find_first_of('\r\n',0);
	requestLine.assign(str,0, index+2);
	str.erase(0, index+2);
	index = str.find("\r\n\r\n");
	string option;
	option.assign(str,0,index+2);
	str.erase(0,index+4);
	parseRequestLine(requestLine);
	parseOption(option);
	if (str.length() == 0)
	{
		this->data = NULL;
	}
	else
	{
		this->data = new char[str.length()];
		memset(this->data,0,str.length());
		memcpy(this->data, str.c_str(), strlen(str.c_str()));
	}
	return true;
}
bool Request::parseRequestLine(string str)
{
	int start=0, num=0,index=0;
	index = str.find_first_of(' ', start);
	num = index;
	this->requestLine.method.assign(str,start,num);
	start = index + 1;
	index = str.find_first_of(' ', start);
	num = index - start;
	this->requestLine.requestURL.assign(str, start, num);
	start = index+1;
	index = str.find_first_of('\r\n', start);
	num = index - start;
	this->requestLine.httpVersion.assign(str, start,num);
	str.clear();
	printf("requestLine.method:%s\n", this->requestLine.method.c_str());
	printf("requestLine.URL:%s\n", this->requestLine.requestURL.c_str());
	printf("requestLine.HTTPVersion:%s\n", this->requestLine.httpVersion.c_str());
	return true;
}
bool Request::parseOption(string str)
{
	int start = 0, num = 0, index = 0,temp = 0;
	string key, value,content;
	do 
	{
		index = str.find_first_of('\r\n', start);

		num = index - start;
		content.assign(str,start,num);
		temp = content.find_first_of(':', 0);
		key.assign(content,0,temp);
		value.assign(content,temp+1,content.length()-temp-1);
		printf("%s:", key.c_str());
		printf("%s\n", value.c_str());
		this->option.push_back(pair<string,string>(key,value));
		start = index + 2;
	} while (index != string::npos && index != str.length()-2);
	return true;
}
string Request::getParm(string key)
{
	vector<pair<string, string>>::iterator iter;
	for (iter = this->option.begin(); iter != this->option.end(); iter++)
	{
		if (iter->first == key)
		{
			return iter->second;
		}
	}
	return NULL;
}
int Request::getMethod()
{
	string str = this->requestLine.method;
	return this->methodMap[str];
}
string Request::getURl()
{
	int index = 0;
	string Url;
	index = this->requestLine.requestURL.find_first_of('?',0);
	Url.assign(this->requestLine.requestURL,0, index);
	return Url;
}
string Request::getRequestParm()
{
	int start = 0,index = 0;
	string Url;
	index = this->requestLine.requestURL.find_first_of('?', 0);
	start = index+1;
	Url.assign(this->requestLine.requestURL, start, this->requestLine.requestURL.length()-start);
	return Url;
}