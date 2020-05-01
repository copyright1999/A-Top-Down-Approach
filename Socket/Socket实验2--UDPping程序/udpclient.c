#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

#define maxlen 1024

/*
@变量声明
*/
int sockfd,len;
struct sockaddr_in address;

/*
@brief:将ping命令放到buf里面
@para:buf bufsize seqnum
@return:none
*/
void ping_cmd(char *buf,int bufsize,int seqnum)
{
    time_t timeval;
    (void)time(&timeval);
    char seq[5];
    sprintf(seq,"%d%s",seqnum," ");
    sprintf(buf,"%s%s%s","ping seq=",seq,ctime(&timeval));//ctime会自动换行,不用加换行符
}


int main(void)
{
/*配置套接字*/
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");//inet函数已经实现了字节转换的功能了
    address.sin_port = htons(8080);
    len = sizeof(address);

/*配置发送跟接收的超时时间，避免阻塞*/
    struct timeval timeout = {1,0}; //该结构体在<sys/time.h>头文件里面
    setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(struct timeval));
    setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));

    struct timeval tv;//计算RTT用到的结构体
    char buf[maxlen];
    int seq=0;
    while (seq!=10)
    {
       seq++;
       printf("seq=%d   ",seq);

/*发送ping命令，并且获取发送的时间，微秒表示*/
       ping_cmd(buf,sizeof(buf),seq);

        int sentflag=sendto(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&address,len);
        if(sentflag==-1)
        {
            printf("发送超时\n");
            continue;
        }
        gettimeofday(&tv,NULL);
        unsigned long int pingtime=tv.tv_sec*1000000 + tv.tv_usec;

/*接收服务器命令，若超时则打印出请求超时，反之获取接收的时间，微妙表示*/
        int revflag=recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&address,&len);
        if(revflag==-1)
        {
            printf("请求超时\n");
            continue;
        }
        gettimeofday(&tv,NULL);
        unsigned long   int sucess=tv.tv_sec*1000000 + tv.tv_usec;

/*计算RTT*/
        unsigned long   int RTT=sucess-pingtime;

/*打印结果*/
        printf("%s  rtt:%ldus\n",buf,RTT);
    }
    
    close(sockfd);
    exit(0);
}

