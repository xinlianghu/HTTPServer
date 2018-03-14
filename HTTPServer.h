#include <winsock2.h>
#include <process.h>
#include "Request.h"
#include "Responce.h"
#include "HTTPType.h"
#pragma comment(lib, "ws2_32.lib")
#pragma once
#define MAXCONNECT 20

class HTTPServer;
typedef struct _HTTPContext
{
	SOCKET client;
	sockaddr_in addr;
	int len;
	int threadId;
	HTTPServer * httpServer;

}HTTPContext, *PHTTPContext;

typedef struct
{
	string path;
	SOCKET client;
	string RootPath;
	HTTPServer * httpServer;
}DoGetContext, *PDoGetContext;
class HTTPServer
{
public:
	SOCKET serverSocket;
	int maxConnect;
	sockaddr_in serverAddr;
	string RootPath;
	HANDLE threadHandle[MAXCONNECT];
	int timeout;
	map<string, string> MimeType1, MimeType2;
public:
	HTTPServer(string RootPath);
	~HTTPServer();
	bool setPort(int port);
	bool setMaxConnect(int num);
	bool setAddr(char * addr);
	bool startServer();

	void runServer();
	
	int processRequest(Request request, PHTTPContext context);
	bool doGet(Request request, PHTTPContext context);
	void InitMime(map<string, string> &mimeType1, map<string, string> &mimeType2);
	bool HTTPServer::_doGet(SOCKET client, string path, HTTPServer* httpServer);
};
unsigned int WINAPI  ThreadFun(LPVOID parm);
bool readFile(string path, char *&data, int &len,bool flag);
