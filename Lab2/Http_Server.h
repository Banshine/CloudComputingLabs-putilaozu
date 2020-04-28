#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_
#include <iostream>
#include<cstring>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
using namespace std;


#define BUFFER_SIZE 4096
class  HttpServer{
    private:
        int socketfd; //套接字描述符
    public:
        HttpServer(){}
        HttpServer(int id):socketfd(id){}
        ~HttpServer(){}
        void processHttp();
        void not_effect(string method);//非post,get方法
        void not_found(string url,string method);//文件未找到
        void get(string url,string method);//处理get方法
        void post(string name,string id);//处理post方法
};

#endif