#include<stdio.h>
#include "HTTPServer.h"
#include <time.h>
int  main(int argc, char *argv[])
{
	HTTPServer httpserver("C:\\\\HTTP");
	httpserver.runServer();
	httpserver.~HTTPServer();
}