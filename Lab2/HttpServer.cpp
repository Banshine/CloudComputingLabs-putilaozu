#include "Http_Server.h"
#include <cstring>
#include <linux/tcp.h>

void HttpServer::Not_Implemented(string method){//非GET/POST方法
    string entity1="<html><title>501 Not Implemented</title><body bgcolor=ffffff>\n Not Implemented\n";
    string entity2="<p>Does not implement this method: "+method+"\n<hr><em>HTTP Web Server</em>\n</body></html>\n";
    string entity=entity1+entity2;
    string str="HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\nContent-Length: "+to_string(entity.length())+"\r\n\r\n";
    string total=str+entity;
    char buf[512];
    sprintf(buf,"%s",total.c_str());
    write(socketfd, buf, strlen(buf));
}

void HttpServer::Not_Found(string url,string method){//404错误
    string entity1="<html><title>404 Not Found</title><body bgcolor=ffffff>\n Not Found\n";
    string file="<p>Could not find this file: "+url+"\n";
    string entity2="<hr><em>HTTP Web Server</em>\n</body></html>\n";
    string entity=entity1+file+entity2;
    if(method=="POST"){
        entity=entity1+entity2;
    }
    string str="HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: "+to_string(entity.length())+"\r\n\r\n";
    string total=str+entity;
    char buf[512];
    sprintf(buf,"%s",total.c_str());
    write(socketfd, buf, strlen(buf));
}

void HttpServer::response_get(string url,string method){
    string s="./html";
    if(url.find(".")==string::npos){
        if(url.length()==0||url[url.length()-1]=='/')
            s=s+url+"index.html";
        else  s=s+url+"/index.html";
    }
    else s=s+url;
    
    int filefd=open(s.c_str(),O_RDONLY);
    if(filefd<0){//不存在该文件
       Not_Found(url,method);
    }
    else{
        struct stat filestat;
        stat(s.c_str(), &filestat);
        char buf[128];
		sprintf(buf, "HTTP/1.1 202 OK\r\ncontent-length:%d\r\n\r\n", (int)filestat.st_size);
		write(socketfd, buf, strlen(buf));
		//使用“零拷贝”发送文件
		sendfile(socketfd, filefd, 0, filestat.st_size);
    }
}

void HttpServer::response_post(string name,string id){
    string entity1="<html><title>POST Method</title><body bgcolor=ffffff>\n";
    string str2="Your name: "+name+"\nYour id: "+id+"\n";
    string entity2="<hr><em>HTTP Web Server</em>\n</body></html>\n";
    string entity=entity1+str2+entity2;
    string str="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "+to_string(entity.length())+"\r\n\r\n";
    string total=str+entity;
    char buf[512];
    sprintf(buf,"%s",total.c_str());
    write(socketfd, buf, strlen(buf));
}

void HttpServer::processHttp(){
    string strbuf;
    while(1){
        bool is_keepalive=true;//持久连接标志
        char buf[BUFFER_SIZE];
        int size=0;
        //size=read(socketfd,buf,BUFFER_SIZE-1);
        size=recv(socketfd,buf,BUFFER_SIZE-1,0);
        //cout<<"size = "<<size<<endl;
        if(size>0){
            buf[size]='\0';
            strbuf+=string(buf);
            //cout<<strbuf<<endl;
            //cout<<buf<<endl;
            //处理所有的http请求报文
            while(strbuf.find("HTTP/")!=string::npos){
                //首先判断strbuf中的有没有完整报文
                int pos=0;//http请求报文首部行
                int postPos=0;//请求体的起始位置
                //存在完整的首部行和请求行
                if((pos=strbuf.find("\r\n\r\n"))!=-1){
                    string httprequest="";//http请求报文
                    //接下来判断是否有请求体，以及请求体是否完整
                    pos+=4;
                    httprequest=strbuf.substr(0,pos);
                    postPos=httprequest.length();
                    int entitypos=httprequest.find("Content-Length:");

                    if(entitypos!=-1){//存在请求体
                        string num;
                        entitypos+=16;
                        while(httprequest[entitypos]!='\r'){
                            num+=httprequest[entitypos++];
                        }
                        int entityLen=atoi(num.c_str());//实体长度

                        if((int)(strbuf.length()-httprequest.length())>=entityLen){//存在完整实体
                            httprequest+=strbuf.substr(httprequest.length(),entityLen);
                            pos+=entityLen;          
                        }
                        else continue;
                    }
                    //得到完整http请求报文后,以上代码解决拆包粘包问题

                    strbuf=strbuf.substr(pos);//从strbuf中删除该报文,并处理该报文
                    string method,url;
                    pos=0;
                    //cout<<httprequest<<endl;
                   
                    while(httprequest[pos]!=' '){
                        method+=httprequest[pos++];
                    }//提取方法

                    if(method!="GET"&&method!="POST"){
                        Not_Implemented(method);
                        continue;
                    }//若既不是GET也不是POST，返回501

                    ++pos;
                    while(httprequest[pos]!=' '){
                        url+=httprequest[pos++];
                    }
                    ++pos;//提取URL

                    if(method=="GET"){
                        response_get(url,method);
                    }
                    else if(method=="POST"){
                        
                        if(url!="/Post_show"){
                            Not_Found(url,method);
                            continue;
                        }
                        string entity=httprequest.substr(postPos,httprequest.length()-postPos);
                        //cout<<entity<<endl;

                        //请求体按照Name=xxx&ID=xxx排列时才处理
                        int namepos=entity.find("Name="),idpos=entity.find("&ID=");
                        if(namepos==-1||idpos==-1||idpos<=namepos){//请求体中存在Name和ID并且按照Name、ID排列
                            Not_Found(url,method);
                            continue;
                        }
                        if(entity.find("=",idpos+4)!=string::npos){
                            Not_Found(url,method);
                            continue;
                        }
                        
                        string name,id;
                        
                        name=entity.substr(namepos+5,idpos-namepos-5);
                        id=entity.substr(idpos+4);
                        response_post(name,id);
                    }
                    if(httprequest.find("Connection: close")!=string::npos){//判断是否是持久连接
                        is_keepalive=false;
                        break;
                    }
                }
            }
            if(!is_keepalive)break;//非持久连接，可以关闭tcp套接字了
        }
        else{
            if(size<=0&&errno!=EINTR){//检测到客户端tcp连接关闭
                break;
            }
        }
    }
    sleep(2);
    close(socketfd);
}