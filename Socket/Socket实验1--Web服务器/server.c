#include <unistd.h> 
#include <sys/types.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>

/*
@变量声明
*/
int server_sockfd, client_sockfd;
int server_len, client_len;
struct sockaddr_in server_address;
struct sockaddr_in client_address;

/*
@HTTP响应消息格式
*/
char *http_res_tmpl = "HTTP/1.1 200 OK\r\n"
    "Server: my localhost 127.0.0.1\r\n"
    "Accept-Ranges: bytes\r\n"
    "Content-Length: %d\r\n"
    "Connection: close\r\n"
    "Content-Type: %s\r\n\r\n";


/*
@宏定义
*/
#define bufmaxlen 1024 //1KB
#define urllen 50


/*
@brief:生成服务器端的socket描述符
@para:void
@return:int
*/
int get_sever_sockfd(void)
{
    return socket(AF_INET,SOCK_STREAM,0);
}


/*
@brief:设置服务端的sockaddr_in结构，包括协议族，本机地址，端口号，地址长度
@para:void
@return:none
*/
void name_server_socker_addr(void)
{
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(8080);
    server_len = sizeof(server_address);
}


/*
@brief:HTTP发送文件
@para:客户端套接字 发送的内容(文件等)
@return:none
*/
void http_send(int client_sockfd,char *content)
{
        char header[bufmaxlen],text[bufmaxlen];
        int len = strlen(content);
        sprintf(header,http_res_tmpl,len,"text/html");
        len = sprintf(text,"%s%s",header,content);
        send(client_sockfd,text,len,0);
}


/*
@brief:信号异常处理
@para:sign
@return:none
*/
void handle_signal(int sign){
    close(server_sockfd);
    fputs("\nsignal interrupt,safe exit\n",stdout);
    exit(0);
}


/*
@brief:将要发送的文件放进缓冲区
@para:文件名 发送缓冲区 发送缓冲区大小
@return:0成功转换  -1无法打开指定文件
*/
int convert_whatfileToSend_buf(char *filename,char *buf,int size)
{
        FILE *t ;
        t=fopen(filename,"rb");
        if (!t)
        {
           printf("open file fail\n");
           return -1;
        }
        memset(buf,0,size);
        while (fread(buf,1,size,t));//一个循环一个字节一个字节的读
        fread(buf,size,1,t);
        fclose(t);
        return 0;
}


int main()
{
    signal(SIGINT,handle_signal);//按下Ctrl+C的时候的处理

/*获取服务器的套接字描述符*/
    server_sockfd=get_sever_sockfd();

    name_server_socker_addr();

/*绑定*/    
    int bindsta=bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
    if (bindsta==-1)
    {
        printf("bind fail\n");
        return -1;
    }

/*监听*/      
    int listensta=listen(server_sockfd,1);
    if(listensta==-1)
    {
        printf("listen error\n");
        return -3;
    }

    while (1)
    {
        client_len=sizeof(client_address);
        client_sockfd=accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);//这里天坑
        if(client_sockfd<0)
        {
            printf("accept error\n");
            return -4;
        }

        char buf[bufmaxlen];
        memset(buf,0,sizeof(buf));
        
/*读取客户端的消息并打印出来*/
        read(client_sockfd, buf, bufmaxlen);
        printf("%s\n",buf);

/*从接受到的客户端的请求报文中截取URL，暂时都是GET请求*/    
    char* urlbegpos=strstr(buf,"GET /")+sizeof("GET /")-1;
    char* urlendpos=strstr(buf,"HTTP")-1;
    char urlbuf[urllen];
    memset(urlbuf,'0',urllen);
    for (int i=0; urlbegpos!= urlendpos; urlbegpos++,i++)
    {
       urlbuf[i]=*urlbegpos;
    }

/*匹配URL*/       
    if (strstr(urlbuf,"404.html"))
    {
        int filesta=convert_whatfileToSend_buf("404.html",buf,bufmaxlen);
        if(filesta==-1)return -6;
    }
    else
    {
        int filesta=convert_whatfileToSend_buf("HelloWorld.html",buf,bufmaxlen);
        if(filesta==-1)return -6;
    }

/*根据不同请求发送不同的内容*/ 
        http_send(client_sockfd,buf);

/*关闭客户端的套接字*/
        close(client_sockfd);
    }
 
    printf("abnormal exit\n");

/*关闭服务器的套接字*/
    close(server_sockfd);

    return 0;
}
