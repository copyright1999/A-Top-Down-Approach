#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>

using namespace std;

class smtp
{
private:
    int maxlen;
    int sockfd,socklen;
    uint16_t smtpcom;
    struct sockaddr_in address;
    struct hostent *host;
    string hostname;//服务器名字 比如 "smtp.163.com"
    string from_mail_addr;
    string to_mail_addr;
    char *recvbuf;

    const string base64Char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; 

public:
    smtp();
    smtp(string name);
    smtp(string name,int receive_buf_max_kb);
    smtp(string name,uint16_t com,int receive_buf_max_kb);
    smtp(string name,uint16_t com);
    ~smtp();

public:
    int send_string_cmd(string buf);
    void name_client_socket(void);
    void connect_server(void);
    void send_HELO_cmd(void);
    void send_AUTH_LOGIN_cmd(void);
    void send_base64UserName_cmd(string from);
    void send_base64UserPassword_cmd(string authroization_code);
    void send_MAIL_FROM_cmd(string mailaddr);
    void send_RCPT_TO_cmd(string mailaddr);
    void send_QUIT_cmd(void);
    void send_BodyEndSymbol(void);
    void recv_status_code(string exceptcode);
    char* base64Encode(char const* origSigned, unsigned origLength);
    void closesocket(void);

    void attachment(int size,...);
    void send_attachment(string filename);
    string base64_openfile(string filename);
    void send_attachment_text(string filename);
    void send_attachment_image(string filename);

    void send_MailStory(string from,string to,string subject,string story);
    void start(string from,string to,string subject,string story,string authroization_code);

    void send_attachment_music(string filename);
};

/**
  * @brief  构造函数
  * @param  
  *        name:主机名字，发件人的smtp服务器,默认为"smtp.163.com"
  *        receive_buf_max_kb:服务器传过来的数据，接收区大小，单位kb，默认为32kb
  *        com:端口，默认为25，不使用SSL，如果使用SSL，端口自己查询对应邮箱的smtp ssl端口
  * @retval void
  */
smtp::smtp()
{
    hostname = "smtp.163.com";
    maxlen = 1024 * 32;//32kb
    smtpcom = 25;
    recvbuf = new char [maxlen];
}

smtp::smtp(string name)
{
    hostname = name;
    maxlen = 1024 * 32;//32kb
    smtpcom = 25;
    recvbuf = new char [maxlen];
}

smtp::smtp(string name,int receive_buf_max_kb)
{
    hostname = name;
    maxlen = receive_buf_max_kb * 1024;
    smtpcom = 25;
    recvbuf = new char [maxlen];
}

smtp::smtp(string name,uint16_t com,int receive_buf_max_kb)
{
    hostname = name;
    maxlen = receive_buf_max_kb * 1024;
    smtpcom = com;
    recvbuf = new char [maxlen];
}

smtp::smtp(string name,uint16_t com)
{
    hostname = name;
    maxlen = 32 * 1024;
    smtpcom = com;
    recvbuf = new char [maxlen];
}

/**
  * @brief  析构函数
  * @param  前面用new开辟了一个数组
  * @retval void
  */
smtp::~smtp()
{
    delete recvbuf;
}

/**
  * @brief  关闭套接字
  * @param  void
  * @retval void
  */
void smtp::closesocket(void)
{
    send_BodyEndSymbol();
    send_QUIT_cmd();
    close(sockfd);
}

/**
  * @brief  base64编码
  * @param  void
  * @retval void
  */
char* smtp::base64Encode(char const* origSigned, unsigned origLength) 
{ 
    unsigned char const* orig = (unsigned char const*)origSigned; // in case any input bytes have the MSB set 
    if (orig == NULL) return NULL; 

    unsigned const numOrig24BitValues = origLength / 3; 
    bool havePadding = origLength > numOrig24BitValues * 3; 
    bool havePadding2 = origLength == numOrig24BitValues * 3 + 2; 
    unsigned const numResultBytes = 4 * (numOrig24BitValues + havePadding); 
    char* result = new char[numResultBytes + 3]; // allow for trailing '/0' 

    // Map each full group of 3 input bytes into 4 output base-64 characters: 
    unsigned i; 
    for (i = 0; i < numOrig24BitValues; ++i) 
    { 
        result[4 * i + 0] = base64Char[(orig[3 * i] >> 2) & 0x3F]; 
        result[4 * i + 1] = base64Char[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F]; 
        result[4 * i + 2] = base64Char[((orig[3 * i + 1] << 2) | (orig[3 * i + 2] >> 6)) & 0x3F]; 
        result[4 * i + 3] = base64Char[orig[3 * i + 2] & 0x3F]; 
    } 

    // Now, take padding into account. (Note: i == numOrig24BitValues) 
    if (havePadding) 
    { 
        result[4 * i + 0] = base64Char[(orig[3 * i] >> 2) & 0x3F]; 
        if (havePadding2) 
        { 
            result[4 * i + 1] = base64Char[(((orig[3 * i] & 0x3) << 4) | (orig[3 * i + 1] >> 4)) & 0x3F]; 
            result[4 * i + 2] = base64Char[(orig[3 * i + 1] << 2) & 0x3F]; 
        } 
        else
        { 
            result[4 * i + 1] = base64Char[((orig[3 * i] & 0x3) << 4) & 0x3F]; 
            result[4 * i + 2] = '='; 
        } 
        result[4 * i + 3] = '='; 
    } 
    result[numResultBytes] = '\0'; 
    return result; 
} 

/**
  * @brief  socket发送字符串
  * @param  字符串
  * @retval 成功发送返回0 失败-1
  */
int smtp::send_string_cmd(string buf)
{
    return send(sockfd,buf.c_str(),strlen(buf.c_str()),0);
}

/**
  * @brief  创建套接字
  * @param  void
  * @retval void
  */
void smtp::name_client_socket(void)
{
    if((host=gethostbyname(hostname.c_str()))==NULL)/*取得主机IP地址*/
    {
        cout<<"Gethostname error"<<strerror(errno)<<endl;
        exit(1);
    }

    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)/*建立SOCKET连接*/
    {
        cout<<"Socket Error:"<<strerror(errno)<<endl;
        exit(1);
    }

    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr =*((struct in_addr *)host->h_addr);
    address.sin_port = htons(smtpcom);
    socklen = sizeof(address); 
}

/**
  * @brief  与服务器创建连接
  * @param  void
  * @retval void
  */
void smtp::connect_server(void)
{
    if(connect(sockfd, (struct sockaddr *)&address, socklen)==-1)/*连接服务器*/
    {
        cout<<"Connect Error:"<<strerror(errno)<<endl;
        exit(1);
    }
    recv_status_code("220");
}

/**
  * @brief  发送邮件文本消息的末尾
  * @param  void
  * @retval void
  */
void smtp::send_BodyEndSymbol(void)
{
    //发送邮件结束
    send(sockfd,"--@boundary@--\r\n.\r\n", strlen("--@boundary@--\r\n.\r\n"),0);
    recv_status_code("354");
}

/**
  * @brief  发送HELO命令
  * @param  void
  * @retval void
  */
void smtp::send_HELO_cmd(void)
{
    if(send_string_cmd("HELO smtp.163.com\r\n") == -1)cout<<"send HELO cmd timeout"<<endl;
    recv_status_code("250");
}

/**
  * @brief  发送QUIT命令
  * @param  void
  * @retval void
  */
void smtp::send_QUIT_cmd(void)
{
    if(send_string_cmd("QUIT\r\n") == -1)cout<<"send QUIT cmd timeout"<<endl;
    recv_status_code("250");
}

/**
  * @brief  发送AUTH LOGIN命令
  * @param  void
  * @retval void
  */
void smtp::send_AUTH_LOGIN_cmd(void)
{
    if(send_string_cmd("AUTH LOGIN\r\n") == -1)cout<<"send AUTH LOGIN cmd timeout"<<endl;
    recv_status_code("334");
}

/**
  * @brief  发送MAIL FROM命令
  * @param  void
  * @retval void
  */
void smtp::send_MAIL_FROM_cmd(string mailaddr)
{
    from_mail_addr = mailaddr;
    string temp = "MAIL FROM: <"+mailaddr+">\r\n";
    if(send_string_cmd(temp) == -1)cout<<"send MAIL FROM cmd timeout"<<endl;
    recv_status_code("250");
}

/**
  * @brief  发送RCPT TO命令
  * @param  void
  * @retval void
  */
void smtp::send_RCPT_TO_cmd(string mailaddr)
{
    to_mail_addr = mailaddr;
    string temp = "RCPT TO: <"+mailaddr+">\r\n";
    if(send_string_cmd(temp) == -1)cout<<"send RCPT TO cmd timeout"<<endl;
    recv_status_code("250");
}

/**
  * @brief  发送base64编码过的发件人用户名
  * @param  void
  * @retval void
  */
void smtp::send_base64UserName_cmd(string from)
{
    string tmp = base64Encode(from.c_str(),strlen(from.c_str()));
    tmp = tmp +"\r\n";
    if(send_string_cmd(tmp) == -1)cout<<"send base64Username cmd timeout"<<endl;
    recv_status_code("334");
}

/**
  * @brief  发送base64编码过的发件人的邮箱授权码
  * @param  void
  * @retval void
  */
void smtp::send_base64UserPassword_cmd(string authroization_code)
{
    string tmp = base64Encode(authroization_code.c_str(),strlen(authroization_code.c_str()));
    tmp = tmp +"\r\n";
    if(send_string_cmd(tmp) == -1)cout<<"send base64Password cmd timeout"<<endl;
    recv_status_code("235"); 
}

/**
  * @brief  打印服务器发回来的信息
  * @param  期待收到的应答状态码 字符串
  * @retval void
  */
void smtp::recv_status_code(string exceptcode)
{
    if (recv(sockfd,recvbuf,maxlen,0) == -1)
    {
        cout<<"receive error"<<endl;
    }
    if(!strstr(recvbuf,exceptcode.c_str()))
    {
        cout<<"client accept servers'status code error"<<endl;
    }
    cout<<recvbuf<<endl;
    memset(recvbuf,0,maxlen);
}

/**
  * @brief  附件信息
  * @param  附件个数  附件的名字
  * @retval void
  */
void smtp::attachment(int size,...)
{
    va_list pointer;
    va_start(pointer,size);//一定要“...”之前的那个参数
    string tmp="";
    for (int i = 0; i < size; i++)
    {
        tmp = va_arg(pointer,char*);//没有string类型！！！
        send_attachment(tmp);
    }
    va_end(pointer);
}

/**
  * @brief  只发送邮件正文内容
  * @param  发件人 收件人 主题 邮件内容
  * @retval void
  */
void smtp::send_MailStory(string from,string to,string subject,string story)
{
//发送DATA命令
    send(sockfd,"DATA\r\n", strlen("DATA\r\n"),0);

//发送邮件的头 
    string title ="From: " + from + "\r\nTo: " + to + "\r\nSubject: " + subject +"\r\n"; 
    string mime_header1 = "MIME-Version: 1.0\r\n";
    string mime_header3 = "Content-Type: multipart/mixed;boundary=@boundary@\r\n\r\n";//多加一个换行符
    string total = title + mime_header1  + mime_header3;
    send(sockfd,total.c_str(),strlen(total.c_str()),0);

//发送邮件正文内容
    string text="--@boundary@\r\nContent-Type: text/plain;charset=\"UTF-8\"\r\n\r\n";//gb2312
    story = text + story  + "\r\n\r\n"; 
    send(sockfd,story.c_str(), strlen(story.c_str()),0);
}

/**
  * @brief  发送文本文件 后面可能要支持c文件之类的
  * @param  文件名字
  * @retval void
  */
void smtp::send_attachment_text(string filename)
{
    string sendstring; 
    sendstring = "--@boundary@\r\nContent-Type: text/plain; name=";
    sendstring = sendstring +"\"" +filename +"\"" + "\r\n";  
    sendstring = sendstring + "Content-Disposition: attachment; filename="+"\""+filename +"\"" + "\r\n"; ; 
    sendstring += "Content-Transfer-Encoding: base64\r\n\r\n"; 
    send(sockfd, sendstring.c_str(), sendstring.length(), 0); 

    string textbase64=base64_openfile(filename);
    textbase64 += "\r\n";
    send(sockfd, textbase64.c_str(), textbase64.length(), 0);
}

/**
  * @brief  返回Base64编码后文件的内容
  * @param  文件名字
  * @retval base64编码后的数据
  */
string smtp::base64_openfile(string filename)
{
    FILE *fp = fopen(filename.c_str(), "rb+");//二进制方式打开文件
    char *ar ;

    if(fp == NULL)
    {
        cout<<"openfile fail"<<endl;
        fclose(fp);
        return "";
    }

    //求得文件的大小
	fseek(fp, 0, SEEK_END);//这里将指针移动到文件末尾
	int size = ftell(fp);
	rewind(fp);//将fp指针设置到文件开头
 
	//申请一块能装下整个文件的空间
	ar = new char [(sizeof(char)*size)];
 
	//读文件
	fread(ar,1,size,fp);//每次读一个，共读size次
    fclose(fp);
	string buf = base64Encode(ar,(sizeof(char)*size));
    delete ar;
    return buf;
}

/**
  * @brief  开始发送
  * @param  发件地址 收件地址 主题 邮件正文内容 授权码密码
  * @retval void
  */
void smtp::start(string from,string to,string subject,string story,string authroization_code)
{
    name_client_socket();

    connect_server();

    send_HELO_cmd();
    
    send_AUTH_LOGIN_cmd();
   
    send_base64UserName_cmd(from);
 
    send_base64UserPassword_cmd(authroization_code); 

    send_MAIL_FROM_cmd(from);

    send_RCPT_TO_cmd(to);

    send_MailStory(from,to,subject,story);  
}

/**
  * @brief  发送附件图片
  * @param  附件的名字 字符串
  * @retval void
  */
void smtp::send_attachment_image(string filename)
{
    string photo; 
    photo = "--@boundary@\r\nContent-Type: image/png; name="; 
    photo = photo + "\""+filename+"\""+"\r\n";

    photo += "Content-Disposition: attachment; filename="; 
    photo = photo +"\""+filename+"\""+"\r\n";
    photo += "Content-Transfer-Encoding: base64\r\n\r\n"; 
    send(sockfd, photo.c_str(), photo.length(), 0); 

    string photo64 = base64_openfile(filename);
    photo64 += "\r\n";
    send(sockfd, photo64.c_str(), photo64.length(), 0); 
}

/**
  * @brief  发送附件音乐
  * @param  附件的名字 字符串
  * @retval void
  */
void smtp::send_attachment_music(string filename)
{
    string music; 
    music = "--@boundary@\r\nContent-Type: audio/mpeg; name="; 
    music = music + "\""+filename+"\""+"\r\n";

    music += "Content-Disposition: attachment; filename="; 
    music = music +"\""+filename+"\""+"\r\n";
    music += "Content-Transfer-Encoding: base64\r\n\r\n"; 
    send(sockfd, music.c_str(), music.length(), 0); 

    string music64 = base64_openfile(filename);
    music64 += "\r\n";
    send(sockfd, music64.c_str(), music64.length(), 0); 
}


/**
  * @brief  发送附件
  * @param  附件的名字 字符串
  * @retval void
  */
void smtp::send_attachment(string filename)
{
    //匹配文件名字后缀
    int pos = filename.find_last_of('.');
    if(pos < 0)
    {
        cout<<"attachment dose not match the normal style"<<endl;
        return ;
    }

//匹配后 打开 并且  发送
    if(filename.substr(pos+1) == "txt" || filename.substr(pos+1) == "c"|| filename.substr(pos+1) == "cpp" || filename.substr(pos+1) == "docx")//奇怪的考虑的东西增加了
    {
        send_attachment_text(filename);
    }
    else if(filename.substr(pos+1) == "png" || filename.substr(pos+1) == "jpg")
    {
        send_attachment_image(filename);
    }
    else if(filename.substr(pos+1) == "mp4" || filename.substr(pos+1) == "m4a")
    {
        send_attachment_music(filename);
    }
    else
    {
        cout<<filename<<" can not match the type! "<<endl;
    }
}


int main(void)
{
    string sender_addr = "发件人@qq.com"; //发件人邮箱

    string receiver_addr = "收件人@163.com"; //收件人邮箱

    string subject = "发送jpg png txt docx等文件";//邮件的主题

    string authrization_code = "原始授权码";//发件人的邮箱的授权码

    string story="没有使用ssl的邮箱客户端\r\n";//邮箱正文内容

    smtp example("smtp.qq.com",25,32);//发件人的邮箱smtp服务器

    example.start(sender_addr,receiver_addr,subject,story,authrization_code);

    example.attachment(5,"readme.txt","photo.png","photo.jpg","测试文档.docx","hello.cpp");

    example.closesocket();

    return 0;
}

