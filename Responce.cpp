#include "Responce.h"
Responce::Responce()
{
	this->data = NULL;
	this->len = 0;
	this->statusLine.httpVersion = "HTTP/1.1";
	this->setParm("Server", "HXL-HTTPServer/1.1(Window)");
	this->setParm("Content-type","application/octet-stream");
	time_t timep;
	time(&timep);
	string str = asctime(gmtime(&timep));
	str.erase(str.length() - 1, 1);
	this->setParm("Date",str);
}
bool Responce::setParm(string key, string value)
{
	this->option[key] = value;
	return true;
}
int Responce::setData(const char *buf, int len)
{
	if (len <= 0)
		return 0;
	this->data = new char[len];
	this->len = len;
	memset(this->data, 0, len);
	memcpy(this->data, buf, len);
	return len;
}
bool Responce::setStatusCode(string code)
{
	this->statusLine.statusCode = code;
	return true;
}
bool Responce::setStatusMsg(string msg)
{
	this->statusLine.reasonPhrase = msg;
	return true;
}
Responce::~Responce()
{
	if (this->data != NULL)
	{
		delete[] this->data;
	}
	this->option.clear();
	this->len = 0;
}
int Responce::getResponce(char * &buf)
{
	string str("");
	str = str + this->statusLine.httpVersion;
	str = str + " " + this->statusLine.statusCode;
	str = str + " " + this->statusLine.reasonPhrase + "\r\n";
	map<string, string>::iterator iter;
	for (iter = this->option.begin(); iter != this->option.end(); iter++)
	{
		str = str + iter->first;
		str = str + ":";
		str = str + iter->second + "\r\n";
	}
	str = str + "\r\n";
	int len = str.length() + this->len;
	buf = new char[len+1];
	memset(buf, 0, len+1);
	memcpy(buf, str.c_str(), str.length());
	memcpy(buf + str.length(), this->data, this->len);
	return len;
}
