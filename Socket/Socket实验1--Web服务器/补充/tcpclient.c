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

#define maxlen 1024

void modify_send_story(char *filename,char *buf,int bufsize)
{
    sprintf(buf,"%s%s%s","GET /",filename," HTTP/1.1");
}

int main(void)
{
    int sockfd;
    int len;
    struct sockaddr_in address;
    int result;
    char ch = 'A';

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(8080);
    len = sizeof(address);

    result = connect(sockfd, (struct sockaddr *)&address, len);

    if(result == -1)
    {
        perror("没有链接到服务器");
        exit(1);
    }

    char buf[maxlen];

    modify_send_story("word.html",buf,sizeof(buf));

    int senflag=send(sockfd,buf,sizeof(buf),0);
    if(senflag==-1)
    {
        perror("发送出错");
        exit(1);
    }
    
    int revflag=recv(sockfd,buf,sizeof(buf),0);
    if (revflag==-1)
    {
        perror("接收出错");
        exit(1);
    }
    printf("%s\n",buf);



    close(sockfd);
    exit(0);
}