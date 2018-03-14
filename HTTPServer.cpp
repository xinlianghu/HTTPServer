#include "HTTPServer.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
HTTPServer::HTTPServer(string RootPath)
{
	this->serverAddr.sin_family = AF_INET;
	this->serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	this->serverAddr.sin_port = htons(80);
	this->maxConnect = 8;
	this->serverSocket = INVALID_SOCKET;
	this->timeout = 60000;
	for (int i = 0; i < MAXCONNECT; i++)
	{
		this->threadHandle[i] = NULL;
	}
	this->RootPath = RootPath;
	InitMime(this->MimeType1, this->MimeType2);
}


HTTPServer::~HTTPServer()
{
	if (this->serverSocket != INVALID_SOCKET && this->serverSocket != NULL)
		closesocket(this->serverSocket);
	WSACleanup();
	this->serverSocket = INVALID_SOCKET;
	MimeType1.clear();
	MimeType2.clear();
}


bool HTTPServer::setPort(int port)
{
	if (port > 0 && port < 65536)
	{
		this->serverAddr.sin_port = htons(port);
		return true;
	}
	return false;
}


bool HTTPServer::setMaxConnect(int num)
{
	if (num>0 && num <= MAXCONNECT)
	{
		this->maxConnect = num;
		return true;
	}
	return false;
}


bool HTTPServer::setAddr(char * addr)
{
	ULONG iaddr = inet_addr(addr);
	if (iaddr == INADDR_NONE)
		return false;
	this->serverAddr.sin_addr.S_un.S_addr = iaddr;
	return true;
}


bool HTTPServer::startServer()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return false;
	}
	this->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->serverSocket == INVALID_SOCKET)
	{
		printf("socket error !");
		return false;
	}
	if (bind(this->serverSocket, (LPSOCKADDR)&this->serverAddr, sizeof(this->serverAddr)) == SOCKET_ERROR)
	{
		printf("bind error !");
		return false;
	}
	if (listen(this->serverSocket, this->maxConnect) == SOCKET_ERROR)
	{
		printf("listen error !");
		return false;
	}
	return true;
}





void HTTPServer::runServer()
{
	if (!this->startServer())
	{
		return;
	}
	printf("start server success!\n");
	int index;
	while (true)
	{
		index = 0;
		printf("wait client connect!\n");
		PHTTPContext pcontext = new HTTPContext();
		memset(pcontext, 0, sizeof(HTTPContext));
		pcontext->httpServer = this;
		pcontext->len = sizeof(pcontext->addr);
		pcontext->client = accept(this->serverSocket, (LPSOCKADDR)&pcontext->addr, &pcontext->len);
		if (pcontext->client == INVALID_SOCKET)
		{
			printf("accept server failed!\n");
			printf("Error Code:%d\n", WSAGetLastError());
			delete(pcontext);
			continue;
		}
		setsockopt(pcontext->client, SOL_SOCKET, SO_RCVTIMEO, (char *)&(this->timeout), sizeof(timeout));
		for (int i = 0; i < this->maxConnect; i++)
		{
			if (this->threadHandle[i]==NULL)
			{
				index = i;
				pcontext->threadId = i;
				printf("create threadID:%d\n", pcontext->threadId);
				this->threadHandle[i] =(HANDLE) _beginthreadex(NULL, 0, &ThreadFun, (LPVOID)pcontext, 0, NULL);
				if (this->threadHandle[i] == NULL)
				{
					printf("create Thread Failed\n");
				}
				else
				{
					printf("create Thread Success!\n");
					break;
				}
			}
		}

		if (index = 0 || this->threadHandle[index] == NULL)
		{
			delete(pcontext);
		}
	}
}


 unsigned int WINAPI  ThreadFun(LPVOID parm)
{
	PHTTPContext pcontext = (PHTTPContext)parm;
	char buf[1025];
	memset(buf, 0, 1025);
	int ret = 0;
	Request request;
	while ((ret = recv(pcontext->client, buf, 1024, 0)) > 0)
	{
		printf("Ret:%d\n", ret);
		printf("-------------------ParseRequest---------------\n");
		request.parseRequest(buf, strlen(buf));
		printf("-------------------ParseRequest---------------\n");
//		printf("method:%s\n", request.requestLine.method.c_str());
		pcontext->httpServer->processRequest(request, pcontext);
//		printf(":%s\n", request.getParm("Connection").c_str());
		if (request.getParm("Connection") != " keep-alive")
		{
	//		printf("4156\n");
			break;
	}
		memset(buf, 0, 1025);
	}
	printf("CloseSocket\n");
	closesocket(pcontext->client);
	CloseHandle(pcontext->httpServer->threadHandle[pcontext->threadId]);
	pcontext->httpServer->threadHandle[pcontext->threadId] = NULL;
	delete pcontext;
	_endthreadex(0);
	return 0;
}


int HTTPServer::processRequest(Request request, PHTTPContext context)
{
	int method = request.getMethod();
	switch (method)
	{
	case 1:
		doGet(request, context);
		break;

	default:
		doGet(request, context);
		break;
	}
	return 0;
}





bool HTTPServer::doGet(Request request, PHTTPContext context)
{

	string path, url,subPath;
	url = request.getURl();
	int index = 0;
	for (int i = 0; i < url.length(); i++)
	{
		if (url[i] == '/')
		{
			subPath.append( 2,'\\');
		}
		else
		{
			subPath.append(1,url[i]);

		}
	}
	path = this->RootPath + subPath;
	PDoGetContext pctx = new DoGetContext();
	context->httpServer->_doGet(context->client, path, context->httpServer);
	return true;
}
bool HTTPServer::_doGet(SOCKET client, string path, HTTPServer* httpServer)
{
	ifstream file;
	file.open(path, ios::in);
	printf("---%s\n", httpServer->MimeType1[".ppt"].c_str());
	if (!file.is_open())
	{
		Responce res;
		res.setStatusCode("404");
		res.setStatusMsg("Request Resource Not Found£¡");
		string err;
		err.append("<html><head><title>404 Not Found</title></head><body><h1>404 Not Found!</h1>");
		err.append("</body></html>");
		res.setParm("Content-length", std::to_string(err.length()));
		res.setParm("Content-type","text/html");
		res.setData(err.c_str(), err.length());
		char * buf;
		int len = res.getResponce(buf);
		printf("Res:\n%s\n",buf);
		int ret = send(client, buf, len, 0);
		printf("send:%d\n", ret);
		delete[] buf;
	}
	else
	{
		Responce res;
		int index = 0;
		index = path.rfind('.');
		string type;
		type.assign(path, index, path.length() - index);
		printf("Type:%s\n", type.c_str());
		bool flag = false;
		if ( httpServer->MimeType1.count(type) == 1 )
		{
			res.setParm("Content-type", httpServer->MimeType1[type]);
			flag = true;
		}
		else if (httpServer->MimeType2.count(type) == 1)
		{
			res.setParm("Content-type", httpServer->MimeType2[type]);
			flag = false;
		}
		char * data;
		int len = 0;
		readFile(path,data, len,flag);
		res.setStatusCode("200");
		res.setStatusMsg("Request Resource Success£¡");
		res.setParm("Content-length", std::to_string(len));
		res.setData(data,len);
		char * buf;
		len = res.getResponce(buf);
//		printf("Res:\n%s\n", buf);
		int ret = send(client, buf, len, 0);
		printf("send:%d\n", ret);
		delete[] buf;
		delete[] data;
	}
	return true;
}
 bool readFile(string path,char *&data,int &len,bool flag)
{
	 ifstream file;
	 if (flag)
		 file.open(path, ios::in| ios::binary);
	 else
	 {
		 file.open(path, ios::in);
	 }
	 if (!file.is_open())
	 {
		 return false;
	 }
	 file.seekg(0, ios::end);
	 int length = file.tellg();
	 printf("length:%d\n", length);
	 file.seekg(0, ios::beg);
	 data = new char[length];
	 memset(data, 0, length);
	 file.read(data, length);
	 length = file.gcount();
	 len = length;
	 printf("len:%d\n", len);
	 file.close();
	 return true;

}
 void HTTPServer::InitMime(map<string, string> &mimeType1, map<string, string> &mimeType2)
 {
	 
	 mimeType1.insert(pair<string, string>(".doc", "application/msword"));
	 mimeType1.insert(pair<string, string>(".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
	 mimeType1.insert(pair<string, string>(".rtf", "application/rtf"));
	 mimeType1.insert(pair<string, string>(".xls", "application/vnd.ms-excel"));
	 mimeType1.insert(pair<string, string>(".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
	 mimeType1.insert(pair<string, string>(".ppt", "application/vnd.ms-powerpoint"));
	 mimeType1.insert(pair<string, string>(".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
	 mimeType1.insert(pair<string, string>(".pdf", "application/pdf"));
	 mimeType1.insert(pair<string, string>(".swf", "application/x-shockwave-flash"));
	 mimeType1.insert(pair<string, string>(".dll", "application/x-msdownload"));
	 mimeType1.insert(pair<string, string>(".exe", "application/octet-stream"));
	 mimeType1.insert(pair<string, string>(".msi", "application/octet-stream"));
	 mimeType1.insert(pair<string, string>(".chm", "application/octet-stream"));
	 mimeType1.insert(pair<string, string>(".cab", "application/octet-stream"));
	 mimeType1.insert(pair<string, string>(".ocx", "application/octet-stream"));
	 mimeType1.insert(pair<string, string>(".rar", "application/octet-stream"));
	 mimeType1.insert(pair<string, string>(".tar", "application/x-tar"));
	 mimeType1.insert(pair<string, string>(".zip", "application/x-zip-compressed"));
	 mimeType1.insert(pair<string, string>(".wav", "audio/wav"));
	 mimeType1.insert(pair<string, string>(".wmv", "video/x-ms-wmv"));
	 mimeType1.insert(pair<string, string>(".mp3", "audio/mpeg"));
	 mimeType1.insert(pair<string, string>(".mp2", "audio/mpeg"));
	 mimeType1.insert(pair<string, string>(".mpe", "audio/mpeg"));
	 mimeType1.insert(pair<string, string>(".mpeg", "audio/mpeg"));
	 mimeType1.insert(pair<string, string>(".mpg", "audio/mpeg"));
	 mimeType1.insert(pair<string, string>(".rm", "application/vnd.rn-realmedia"));
	 mimeType1.insert(pair<string, string>(".mid", "audio/mid"));
	 mimeType1.insert(pair<string, string>(".midi", "audio/mid"));
	 mimeType1.insert(pair<string, string>(".rmi", "audio/mid"));
	 mimeType1.insert(pair<string, string>(".bmp", "image/bmp"));
	 mimeType1.insert(pair<string, string>(".gif", "image/gif"));
	 mimeType1.insert(pair<string, string>(".png", "image/png"));
	 mimeType1.insert(pair<string, string>(".tiff", "image/tiff"));
	 mimeType1.insert(pair<string, string>(".tif", "application/msword"));
	 mimeType1.insert(pair<string, string>(".jpe", "image/jpeg"));
	 mimeType1.insert(pair<string, string>(".jpeg", "image/jpeg"));
	 mimeType1.insert(pair<string, string>(".jpg", "image/jpeg"));
	 mimeType1.insert(pair<string, string>(".mp4", "video/mp4"));
	 mimeType2.insert(pair<string, string>(".txt", "text/plain"));
	 mimeType2.insert(pair<string, string>(".xml", "text/xml"));
	 mimeType2.insert(pair<string, string>(".html", "text/html"));
	 mimeType2.insert(pair<string, string>(".css", "text/css"));
	 mimeType2.insert(pair<string, string>(".js", "text/javascript"));
 }
