### 实验内容

- Ubuntu18.10安装`wireshark`

- 运行wireshark并且进行抓包



### 实验步骤

#### 安装wireshark

- 环境：Ubuntu18.10
- 版本：Stable Wireshark
- 参考：[链接](https://www.linuxidc.com/Linux/2017-01/139104.html)



命令行安装

![GuHIsJ.png](https://s1.ax1x.com/2020/03/31/GuHIsJ.png)

安装`wireshark-common`选择`yes`

![GuHoL9.png](https://s1.ax1x.com/2020/03/31/GuHoL9.png)

添加到`$USER`变量，以后运行时不用加`sudo`权限

![GuHbIx.png](https://s1.ax1x.com/2020/03/31/GuHbIx.png)





#### 运行wireshark

命令行启动软件，因为现在是WiFi环境，选择无线网卡`wlps20`，点击捕获->开始。

![GuH7ZR.png](https://s1.ax1x.com/2020/03/31/GuH7ZR.png)

在浏览器输入一个`HTTP`协议的网页（最好是之前没浏览过的，好像本地缓存过的地址不会被wireshark抓到？）。输入完成后，点击`wireshark`中的捕获->停止，在过滤器列表输入`http`(小写)，并点击右边的箭头。

![GuHhzF.png](https://s1.ax1x.com/2020/03/31/GuHhzF.png)

捕获的`HTTP `的请求消息

![GuHLi6.png](https://s1.ax1x.com/2020/03/31/GuHLi6.png)

捕获的`HTTP`响应消息（紫色箭头的方向可以区分是请求消息还是响应消息）

![GuHOJK.png](https://s1.ax1x.com/2020/03/31/GuHOJK.png)
图中的响应状态码是204 

![GuHXRO.png](https://s1.ax1x.com/2020/03/31/GuHXRO.png)



### 实验结果

- 列出上述出现在未过滤的分组列表窗口的协议列中的3种不同的协议。

  在捕获的过程中还出现`TCP` ，`UDP`，`TLSV1.2`，`TLSV1.3`这几种协议
  
  ![GuHjzD.png](https://s1.ax1x.com/2020/03/31/GuHjzD.png)
  ![GuHxQe.png](https://s1.ax1x.com/2020/03/31/GuHxQe.png)



- 从HTTP GET消息发送到HTTP OK回复需要多长时间？ 

  

  `GET`消息发送时间

  ```
  Arrival Time: Mar 30, 2020 23:50:04.019483996 CST
  ```

  ![GuHzsH.png](https://s1.ax1x.com/2020/03/31/GuHzsH.png)

​     

这次实验收到的是`204 No Content`响应状态码	

```
Arrival Time: Mar 30, 2020 23:50:04.325039445 CST
```

![GubSLd.png](https://s1.ax1x.com/2020/03/31/GubSLd.png)



所以从`HTTP GET`消息发送到`HTTP No Content`回复需要的时间为`0.3055555449`





### 参考资料

- [安装wireshark](https://www.linuxidc.com/Linux/2017-01/139104.html)
- [实验文档翻译](https://github.com/moranzcw/Computer-Networking-A-Top-Down-Approach-NOTES)







