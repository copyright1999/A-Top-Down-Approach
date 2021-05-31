### 实验内容

利用`SMTP`协议来发送电子邮件，连接的端口采用非`SSL`端口，端口号为`25`。这次的实验完成发送邮件文本内容以及附件（图片以及文件）的任务。后面的实验再来增加`SSL`协议。本质上就是实现一个邮件客户端。



### 实验过程

#### 基本思路

创建客户端套接字-->与收件人的`SMTP`服务器进行连接-->发送`HELO`命令-->发送`AUTH LOGIN`命令-->发送发件人的邮箱账号的`base64编码`-->发送发件人的邮箱`SMTP`授权码的`base64编码`-->发送`MAIL FROM`命令-->发送`RCPT TO`命令--> 发送`DATA`命令-->发送`SMTP`消息格式的头部行 -->发送`SMTP`的消息内容（也即邮件内容）-->发送正文结束命令`\r\n.\r\n`  -->发送`QUIT`命令-->关闭套接字



#### 消息格式

SMTP中Email的消息格式由头部行与消息体两部分组成，具体是这么规定的。头部行与消息行中间要隔出来一行（`\r\n`）。

- 头部行
  - To
  - From
  - Subject



- 消息体
  - 消息本身

![2mdFj1.png](https://z3.ax1x.com/2021/05/31/2mdFj1.png)



如果发送内容中有包含其他音频图片之类的其他多媒体文件，就需要在邮件头部增加额外的行以声明MIME的内容类型，具体实现在代码中也有体现。

<details>
    <summary>点击查看MIME头部信息</summary>
    <pre><code>
    string title ="From: " + from + "\r\nTo: " + to + "\r\nSubject: " + subject +"\r\n"; 
    string mime_header1 = "MIME-Version: 1.0\r\n";
    string mime_header3 = "Content-Type: multipart/mixed;boundary=@boundary@\r\n\r\n";
    string total = title + mime_header1  + mime_header3;
    send(sockfd,total.c_str(),strlen(total.c_str()),0);
    </code></pre>
</details>






#### 准备工作

- 获得邮箱授权码

  具体获得授权码的过程可以谷歌查询，不过要注意的有三点

  - 要开启授权码的邮箱是发件人的邮箱，不是收件人的邮箱（如果是收件人的邮箱那我还发个*）
  - 授权码只会显示一次，记得拿纸笔或拍照记录
  - 授权码要保密，别让不法分子利用了

- base64编码

  因为发送用户名，授权码以及邮件内容的时候会用到，有两种方法，一种是直接[在线编码](https://tool.oschina.net/encrypt?type=3)，另一种则是自己在代码里面加上`base64`编码的函数。我的用的是第二种方法。

  
  
  

#### 代码

详细的代码见`notssl.cpp`，对于每个函数我都有写注释。重点看`main`函数。

```c++
int main(void)
{
    string sender_addr = "发件人邮箱@qq.com"; //发件人邮箱

    string receiver_addr = "收件人邮箱@163.com"; //收件人邮箱

    string subject = "发送jpg png txt docx等文件";//邮件的主题

    string authrization_code = "原始授权码";//发件人的邮箱的授权码

    string story="没有使用ssl的邮箱客户端\r\n";//邮箱正文内容

    smtp example("smtp.qq.com",25,32);//发件人的邮箱smtp服务器

    example.start(sender_addr,receiver_addr,subject,story,authrization_code);

    example.attachment(5,"readme.txt","photo.png","photo.jpg","测试文档.docx","hello.cpp");

    example.closesocket();

    return 0;
}
```



- 只要填写好发件人邮箱，收件人邮箱，邮件主题，发件人邮箱的授权码，就可以基本实现想要的功能。附件的发送参照`attachment`这个函数的注释说明即可。

- 需要注意的是，声明一个`smtp`对象的时候，你的主机名字要对应你的发件人的`smtp`邮箱服务器。
- 其中的`authrization_code`要填写发件人邮箱的授权码，这里为了隐私我就不把我的授权码挂上去了。



### 实验结果

终端编译，然后执行可执行文件

![2mdVHK.md.png](https://z3.ax1x.com/2021/05/31/2mdVHK.md.png)



邮箱中收到的消息

![2mdAnx.md.png](https://z3.ax1x.com/2021/05/31/2mdAnx.md.png)



点开邮件

![2mdeAO.md.png](https://z3.ax1x.com/2021/05/31/2mdeAO.md.png)





### 参考链接

- [Markdown中实现内容折叠操作](https://blog.csdn.net/djzhao627/article/details/89977411)
- [fread函数用法](https://blog.csdn.net/Li_Ning_/article/details/51038925)
- [base64编解码用于图片传输](https://blog.csdn.net/qq_38289815/article/details/89280863)
- [va_arg可变参数用法](https://www.cnblogs.com/pengdonglin137/p/3345911.html)
- [MIME格式参考](https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Basics_of_HTTP/MIME_types)
- [char* 与string 的互换](https://blog.csdn.net/u013925378/article/details/88899115)
- [读取文件大小](https://blog.csdn.net/qq_38289815/article/details/89280863)
- [代码编写参考链接一](https://www.jb51.net/article/140979.htm)





### 其他细节

- `POP3`跟` IMAP`的端口与`SMTP`的端口也不一样 ，如果是写邮件服务端的话就可能需要了解跟打开`POP3`跟` IMAP`的服务吧

  

- telnet的命令

  在命令行用telnet命令也可以实现连接邮箱的`smtp`服务器并发送邮件的功能

  

- 卡在服务器验证阶段

  这里巨坑来着。。。之前也写过邮箱客户端的代码，但是就是因为发送函数`send`第三个参数的原因一直卡在服务器验证那里。正确的写法是

  ```c++
  send(sockfd, photo64.c_str(), photo64.length(), 0); 
  ```

  之前一直没有完成就是发送的数据大小的原因，也即第三个参数，那个时候我以为是自己定义的发送区的大小。但是很奇怪发送其他的命令这样子并不会出现问题，只有发送服务器验证命令的时候才会出问题。也是这个原因，巨坑，导致卡在服务器验证阶段很久。



- va_arg函数

  见函数`void smtp::attachment(int size,...)`以及前面的参考链接

  其中`va_arg`函数的第二个参数如果是想表示字符串的话没办用`string`，只能用`char*`，因为这个是c语言库的函数。



- 读取文件的大小

  见函数`string smtp::base64_openfile(string filename)`

  ```c++
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
  ```

  因为要读取文件时会用到`fread()`函数，需要知道文件的大小。用`fseek`将指针移动到文件末尾，再用`ftell`算出文件的大小。这时候算出之后，一定要记得将指针移动到文件开头，不然`fread()`函数就会从末尾开始读，读到的将会是错误的数据。

  

- fread函数

  前面的参考链接也有对`fread`函数的介绍，再这也不多加赘述。只是想说明，求出文件的大小再用`fread`函数会降低对资源的损耗。



- string的加法

  > C++中的string重载了+,+=,但是在进行连加的时候有一个限制，就是连加的时候前两项中必须有一项是string类型。

  所以可以看到代码中有这段看似有点zz的操作

  ```c++
  sendstring = "--@boundary@\r\nContent-Type: text/plain; name=";
  sendstring = sendstring +"\"" +filename +"\"" + "\r\n";  
  ```

  



### 待完成

- `c++`与`python`有邮件客户端的库，以后有机会可以尝试用一下，肯定比自己造的轮子好用。
- 打算传个音频的，但是不知道什么原因传送失败，也没太多时间纠结于这上面，以后有空了再回头找找问题，函数为`void smtp::send_attachment_music(string filename)`。

