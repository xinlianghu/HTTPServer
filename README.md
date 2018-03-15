# HTTPServer
用C++实现了一个简单的HttpServer,其可以处理Http中的get请求，并允许多客户端连接。
* 函数说明 

文件:HTTPServer.cpp
```C++<br>
bool HTTPServer::setPort(int port)
设置服务器监听端口

bool HTTPServer::setAddr(char * addr)
设置服务器监听地址

bool HTTPServer::setMaxConnect(int num)
设置服务器最大连接数

HTTPServer::HTTPServer(string RootPath)
初始化HTTPServer并设置网站根目录。

void HTTPServer::runServer()
启动HTTPServer,侦听相应端口。

int HTTPServer::processRequest(Request request, PHTTPContext context)
处理来自客户端的请求

bool HTTPServer::doGet(Request request, PHTTPContext context)
处理get请求，主要处理url路径，并调用_doGet处理。

bool HTTPServer::_doGet(SOCKET client, string path, HTTPServer* httpServer)
实际处理get请求

void HTTPServer::InitMime(map<string, string> &mimeType1, map<string, string> &mimeType2)
初始化MimeType类型

unsigned int WINAPI  ThreadFun(LPVOID parm)
每次有新的连接时，就创建一个线程函数，处理连接请求。
```

文件:Request.cpp
``` C++
bool Request::parseRequest(char res[], int len);
解析请求，具体工作在parseRequestLine，parseOption中完成

bool Request::parseRequestLine(string str);
解析请求行

bool Request::parseOption(string str);
解析请求选项

string Request::getURl();
获取Request中的URL

string Request::getRequestParm();
获取Request中的请求参数

string Request::getParm(string key);
获取Request中的请求参数的值

int Request::getMethod();
获取Request中的请求方法
其相应取值为：
	GET      1
	POST     2
        HEAD     3
	PUT      4
	DELETE   5
	OPTION   6
	TRACE    7
	CONNECT  8

```
* 数据结构说明<br>
文件：HttpType.h
```C++
请求行
typedef struct
{
	string method;                //请求方法
	string requestURL;            //请求URL
	string httpVersion;           //Http协议版本
}RequestLine, *PRequestLine;

状态行
typedef struct
{
	string httpVersion;          //Http协议版本
	string statusCode;           //状态码
	string reasonPhrase;         //错误原因
}StatusLine, *PStatusLine;
```
* 例子
```C++
int  main(int argc, char *argv[])
{
	HTTPServer httpserver("C:\\\\HTTP");
	httpserver.runServer();
	httpserver.~HTTPServer();
}
```
