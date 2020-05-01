### 实验目的

对DNS查询消息跟响应消息进行抓包



### 实验内容

#### 熟悉nslookup

> nslookup是一种网络管理 命令行工具,可在许多计算机操作系统中使用,用于查询域名系统(DNS)以获得域名或IP地址映射或其他DNS记录。名称“ nslookup”表示"名称服务器查找"。（维基百科）



##### 第一个命令：

表示查询某个域名的IP地址

```bash
nslookup www.szu.edu.cn
nslookup www.mit.edu.cn
```

![1.png](https://i.loli.net/2020/04/28/sABKF6UNdybmolz.png)

- 黄色框

  - 本地DNS服务器名称：`127.0.0.53`
  - 本地DNS服务器地址和端口号：`127.0.0.53#53`

- 绿色框

  - 主机名：`www.szu.edu.cn`

  - IPV4地址：`210.39.12.247`

  - IPV6地址：`2001:250:3c00:212::166`






##### 第二个命令：

DNS的资源记录格式：


![011.png](https://i.loli.net/2020/04/28/DcbqBl6swSEFyOr.png)

默认情况下不指定的话是`type=A`(看前面的查询结果也可以推断出这个结论)。`type=NS`换句话的意思就是`请给我发送mit.edu的权威DNS的主机名`。



命令行输入：

```bash
nslookup -type=NS mit.edu
```

![2.png](https://i.loli.net/2020/04/28/qyiVw5FQMG7AJ3h.png)

黄色框中表示`mit.edu`的权威域名服务器的主机名字。





##### 第三个命令：

这次是利用`eur5.akam.net`这个上个命令查询到的DNS服务器主机告诉我们`www.aiit.or.kr`这个主机的IP地址,因为之前都是向本地的DNS服务器（比如我的是`127.0.0.53`）查询某个主机的IP地址。

```bash
nslookup www.aiit.or.kr   eur5.akam.net
```

![3.png](https://i.loli.net/2020/04/28/KLQXbNnZlw3Afpm.png)

貌似查不到，可能是因为`qiang`的原因？

问题不大，让我们查查深大的

```bash
nslookup -type=NS szu.edu.cn 
nslookup www.szu.edu.cn bay.szu.edu.cn
nslookup www.szu.edu.cn sea.szu.edu.cn
```

![4.png](https://i.loli.net/2020/04/28/38XB1MtiSzQCyO6.png)

查到两个DNS服务器主机的名字

![044.png](https://i.loli.net/2020/04/28/pfQWjYJGlHmoM1a.png)

红框里面可以看到我们查询`www.szu.edu.cn`这个主机的IP地址的时候，用的域名服务器分别是指定的`bay.szu.edu.cn`，`sea.szu.edu.cn`。





##### 回答问题：

1.运行nslookup以获取一个亚洲的Web服务器的IP地址。该服务器的IP地址是什么?

![5.png](https://i.loli.net/2020/04/28/iwtGzEjBIKdTYfA.png)

```
查询的Web服务器是www.google.hk.com，IP地址是202.81.238.12
```



2.运行nslookup来确定一个欧洲的大学的权威DNS服务器。

![6.png](https://i.loli.net/2020/04/28/jMS2FIkDV4bKpvL.png)

```
查询的是英国牛津大学的权威DNS服务器
```



3.运行nslookup,使用问题2中一个已获得的DNS服务器,来查询Yahoo!邮箱的邮件服务器。它的IP
地址是什么?

![7.png](https://i.loli.net/2020/04/28/dozbwkUVpPKvNGr.png)

```
我输入的是nslookup mail.yahoo.com  dns1.ox.ac.uk，但是查不到，不知道是什么原因，如果是直接查的话是可以查到雅虎的邮箱地址是87.248.114.11或者是87.248.114.11
```

![8.png](https://i.loli.net/2020/04/28/fIcvtNqmg4PQnwb.png)





#### 熟悉ifconfig

之前[博客]([https://copyright1999.github.io/2020/03/28/%E7%86%9F%E6%82%89%E5%B8%B8%E7%94%A8%E7%9A%84%E7%BD%91%E7%BB%9C%E5%91%BD%E4%BB%A4/](https://copyright1999.github.io/2020/03/28/熟悉常用的网络命令/))有写过就不多说了



#### 清除dns缓存

##### ubuntu

在网上看到有很多说法，所以我选择这位[博主](https://blog.csdn.net/liangdsh/article/details/71131523)的说法试了一下

> DNS缓存可以加快网站IP的解析速度，所以大多数系统都支持DNS缓存。DNS的缓存时间是24小时，也就是每隔24小时DNS缓存将被自动清除。如果某些网站更新了DNS信息，而本机又没到自动清除的时间，这时，我们就要手动清除DNS缓存，最简单的方法如下。

```bash
$sudo /etc/init.d/dns-clean start
```

![9.png](https://i.loli.net/2020/04/29/WRx1Q7EJwk9XLi5.png)



##### 浏览器

> 1、打开Google Chrome浏览器，输入chrome://net-internals/#dns打开页面。
>
> 2、点击右边的Clear host cache按钮就可以删除谷歌Chrome浏览器DNS缓存了

![10.png](https://i.loli.net/2020/04/29/tULA5zihgXpb8vo.png)







#### wireshark追踪DNS

##### 追踪DNS

如上所述先清除主机跟浏览器的缓存后，打开浏览器，查询自己的IP地址，（当前我的IP地址是`192.168.1.103`），然后打开Wiresharks，在过滤设置那里输入`id.addr==192.168.1.103`，然后开始捕获，在浏览器输入深大的主页`www.szu.edu.cn`。

![11.png](https://i.loli.net/2020/04/29/ybZN4vtIRXqfiPV.png)





##### 回答问题：

4.找到DNS查询和响应消息，它们是否通过UDP或TCP发送?

![13.png](https://i.loli.net/2020/04/29/9Usgwuq6C8WAKbE.png)

```
在过滤器输入dns and ip.addr==192.168.1.103，可以看到DNS是通过UDP发送的。
```



5.DNS查询消息的目标端口是什么？DNS响应消息的源端口是什么？

![JIXqVe.png](https://s1.ax1x.com/2020/04/29/JIXqVe.png)



![013.png](https://i.loli.net/2020/04/29/iFvujR7qObmZfCU.png)

```
查询消息是query,目标端口是53
响应消息是response,源端口是53
```



6.DNS查询消息发送到哪个IP地址?使用ipconfig来确定本地DNS服务器的IP地址。这两个IP地址是是否相同？

![14.png](https://i.loli.net/2020/04/29/P6U4dJOAYWesB8H.png)



![15.png](https://i.loli.net/2020/04/29/Y7P5ueng14HETX9.png)

```
DNS发送查询消息发送到202.96.128.166这个IP地址，本地DNS服务器地址是127.0.0.53。这两个IP地址不相同。
```



7.检查DNS查询消息。DNS查询是什么 Type 的？查询消息是否包含任何 answers？

![16.png](https://i.loli.net/2020/04/29/wDJoqbSMIAcWZ5s.png)

```
查询消息的Type是A，不包含answer。
```



8.检查DNS响应消息，提供了多少个 answers ?这些答案具体包含什么?

![JIXHbD.png](https://s1.ax1x.com/2020/04/29/JIXHbD.png)

```
提供一个answer，内容是   www.szu.edu.cn: type A, class IN, addr 210.39.12.24
```





##### nslookup

重新捕获，使用nslookup在终端查询`www.mit.edu`的地址，查询完成后立即停止捕获。

![JIXTKK.png](https://s1.ax1x.com/2020/04/29/JIXTKK.png)



##### 回答问题：

9.DNS查询消息的目标端口是什么?DNS响应消息的源端口是什么?

![JIX7DO.png](https://s1.ax1x.com/2020/04/29/JIX7DO.png)

```
查询消息的目标端口号是53，响应消息的源端口号是53。
```



10.DNS查询消息的目标IP地址是什么?这是你的默认本地DNS服务器的IP地址吗?

![JIXIv6.png](https://s1.ax1x.com/2020/04/29/JIXIv6.png)

```
DNS查询消息的目标地址是202.96.28.166，不是本地的IP地址。
```



11.检查DNS查询消息。DNS查询是什么 Type 的?查询消息是否包含任何 answers ?

```
Type A，不提供任何answer。
```



12.检查DNS响应消息。提供了多少个 answers ?这些答案包含什么?

![JIXLUH.png](https://s1.ax1x.com/2020/04/29/JIXLUH.png)

```
提供了3个answer，答案包括
www.mit.edu: type CNAME, class IN, cname www.mit.edu.edgekey.net
www.mit.edu.edgekey.net: type CNAME, class IN, cname e9566.dscb.akamaiedge.net
e9566.dscb.akamaiedge.net: type A, class IN, addr 184.30.185.198
```





### 参考资料

在做实验的过程中，查到了一些资料，算是补充自己知识面的不足

- [链接1](https://zhuanlan.zhihu.com/p/101275725)
- [链接2](https://blog.csdn.net/evanxuhe/article/details/90229597)
- [链接3](https://www.feiqy.com/ubuntu-dns/)





















