#include "Http_Server.h"
#include "Threadpool.h"

using namespace std;

static int thread_num=20;
int main(int argc, char *argv[])
{
	if(argc <5){
		printf("usage : %s --ip   --port  [--number-thread]\n", argv[0]);
		return 1;
    }//未按照要求输入参数
    string ip=argv[2];
    int port = atoi(argv[4]);  //获取端口
    if(argc==7&&strcmp(argv[5],"--number-thread")==0)thread_num=atoi(argv[6]);
    int sockfd, connfd; //欢迎套接字和连接套接字
   	struct sockaddr_in servaddr, client;
    //设置服务端的sockaddr_in
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    //servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    //创建socket,TCP,IPv4网络
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
		printf("socket error\n");
		return 1;
    }
   
    int option=1;
     socklen_t optlen=sizeof(option);
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(void*)&option,optlen);
    //绑定
    int ret = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if(ret < 0){
		printf("bind error\n");
		return 1;
    }

    //监听
    ret = listen(sockfd, 20);
    if(ret < 0){
		printf("listen error\n");
		return 1;
    }

    //创建线程池,30个线程
    Threadpool pool(30);
    pool.start();  //线程池开始运行

    while(1){
      socklen_t len = sizeof(client);
      //接受连接
      connfd = accept(sockfd, (struct sockaddr *)&client, &len);
      if(connfd!=-1){
        HttpServer task (connfd);
        //向线程池添加任务
        pool.queue_append(task);
      }
    }
    close(sockfd);
    return 0;
}
