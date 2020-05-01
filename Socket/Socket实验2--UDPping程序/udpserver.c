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
#include <sys/time.h>

/*
@变量声明
*/
int server_sockfd,server_len;
struct sockaddr_in server_address;

/*
@宏定义
*/
#define bufmaxlen 1024 //1KB
#define urllen 50

/*
@brief:生成服务器端的socket描述符
*/
int get_UDPsever_sockfd(void)
{
    return socket(AF_INET,SOCK_DGRAM,0);
}

/*
@brief:设置服务端的sockaddr_in结构，包括协议族，本机地址，端口号，地址长度
*/
void name_server_socker_addr(void)
{
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(8080);
    server_len = sizeof(server_address);
}

/*
@brief:信号异常处理
*/
void handle_signal(int sign){
    close(server_sockfd);
    fputs("\nsignal interrupt,safe exit\n",stdout);
    exit(0);
}


int main()
{
    signal(SIGINT,handle_signal);//按下Ctrl+C的时候的处理

    server_sockfd=get_UDPsever_sockfd();
    name_server_socker_addr();

    int bindsta=bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
    if (bindsta==-1)
    {
        printf("bind fail\n");
        return -1;
    }

    int  rannum;//生成的随机数
    while (1)
    {
        char buf[bufmaxlen];

        rannum=rand()%10;
        int revflag=recvfrom(server_sockfd,buf,sizeof(buf),0,(struct sockaddr*)&server_address,&server_len);
        if(revflag==-1)
        {
            printf("UDP 服务器接收错误\n");
            continue;
        }
        if(rannum>2 )
        {
            printf("%s",buf);
            sprintf(buf,"%s","From Server:ping success");
            sendto(server_sockfd,buf,sizeof(buf),0,(struct sockaddr*)&server_address,server_len);          
        }
    } 

    printf("abnormal exit\n");
    close(server_sockfd);
    return 0;
}
