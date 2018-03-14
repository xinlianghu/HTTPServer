#include <string>
#include <map>
#pragma once
using namespace std;
typedef struct
{
	string method;
	string requestURL;
	string httpVersion;
}RequestLine, *PRequestLine;

typedef struct
{
	string httpVersion;
	string statusCode;
	string reasonPhrase;
}StatusLine, *PStatusLine;

