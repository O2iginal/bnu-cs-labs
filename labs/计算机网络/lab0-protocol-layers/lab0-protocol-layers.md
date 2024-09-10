<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>实验报告封面</title>
    <style>
        .cover {
            font-family: 华文楷体, sans-serif;
            font-size: 18pt;
            margin-top: 0px;
            padding: 0;
            display: flex;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            /* page-break-before: auto;  或者直接删除这行 */
        }
        .cover #cover {
            text-align: center;
            position: relative;
        }
        .cover #logo {
            max-width: 100%;
            margin: 40px auto;
            display: block;
        }
        .cover #course {
            font-family: "华文行楷", sans-serif;
            font-size: 64px;
            line-height: 1.3;
            margin: 20px;
        }
        .cover #experiment {
            font-family: "华文楷体", sans-serif;
            font-size: 40px;
            margin: 30px 0;
        }
        .cover #info {
            text-align: center;
            margin: 20px;
            margin-top: 120px;
            position: relative;
        }
        .cover #info span {
            display: inline-block;
            text-align: left;
        }
        .cover #info span.label {
            font-family: "华文楷体", sans-serif;
            font-size: 22px;
            width: 100px;
        }
        .cover #info span.line {
            border-bottom: 1px solid #000;
            width: 150px;
            margin-bottom: -2pt;
            margin-left: -50px;
            display: inline-block;
            position: relative;
        }
        .cover #info span.text {
            position: absolute;
            font-family: "华文楷体", sans-serif;
            font-size: 22px;
            top: -16pt;
            left: 0;
            right: 0;
            text-align: center;
        }
    </style>
</head>
<body>
    <div class="cover">
        <div id="cover">
                        <div id="course">计算机网络<br/>实验报告</div>
            <div id="experiment">Lab1  Protocol Layers</div>
            <div id="info">
                <span class="label">姓名：</span>
                <span class="line">
                    <span class="text">O2iginal</span>
                </span><br>
                <span class="label">学号：</span>
                <span class="line">
                    <span class="text">O2iginal ID</span>
                </span><br>
                <span class="label">日期：</span>
                <span class="line">
                    <span class="text">2023-10-01</span>
                </span>
            </div>
        </div>
    </div>
</body>
</html>




# 01 实验步骤与结果

## 1.1 选择监听网卡

观察Wireshark界面中每个网卡的流量波形，选择有流量的网卡进行监听。
 这里选择无线网卡WLAN进行监听。如下图所示。

![image-20231001130428106](./lab0-protocol-layers.assets/image-20231001130428106.png)

## 1.2 设置Wireshark过滤器

设置过滤器，过滤规则 “tcp.port == 80”。仅抓取tcp协议、端口号为80的包。如下图所示。

![image-20231001130456367](./lab0-protocol-layers.assets/image-20231001130456367.png)

## 1.3 开始Wireshark抓包

如下图所示，Wireshark下方窗口已显示当前捕获的包，并不断增加。
 （可点击左上角绿色按钮，清空下方信息，重新开始当前捕获）

![image-20231001130517420](./lab0-protocol-layers.assets/image-20231001130517420.png)

##  1.4  wget www.baidu.com

使用`wegt`工具下载www.baidu.com。
 在下载过程中，Wireshark会捕获客户端（本机）与服务器（百度）的通信过程，而后我们将对捕获内容进行分析。

![image-20231001130608447](./lab0-protocol-layers.assets/image-20231001130608447.png)

## 1.5 检查抓包结果

查看Wireshark窗口，可发现已经捕获到一些内容。
其中，可找到两条Protocol为HTTP的trace。其中第一条为客户端向服务器发送的GET请求，而第二条HTTP协议的trace为服务器向客户端发送的包含200OK的响应。

![image-20231001130638717](./lab0-protocol-layers.assets/image-20231001130638717.png)

# 02 问题分析与回答

## 2.1 Turn-in 1：Hand in your packet drawing.

***In Step3：Packet Structure。Draw a figure of an HTTP GET packet that shows the position and size in bytes of the TCP, IP and Ethernet protocol headers.***

操作：选中HTTP GET的 trace，查看Wireshark下方窗口显示的相关结构信息。

![image-20231001130729046](./lab0-protocol-layers.assets/image-20231001130729046.png)

结果：绘制结构如下图所示。

![image-20231001130741664](./lab0-protocol-layers.assets/image-20231001130741664.png)

## 2.2 Turn-in 2

***Your estimate of download protocol overhead as defined above. Tell us whether you find this overhead to be significant.***

*（In Step4：Estimate the download protocol overhead, or percentage of the download bytes taken up by protocol overhead. To do this, consider HTTP data (headers and message) to be useful data for the network to carry, and lower layer headers (TCP, IP, and Ethernet) to be the overhead. ）*

![image-20231001130809135](./lab0-protocol-layers.assets/image-20231001130809135.png)

如上图所示，找到HTTP GET的条目，向上找到包含[SYN,ACK]条目，向下扎到HTTP GET下面的第一个 TCP [ACK] 的条目，对这段区域进行计算。

由Turn-in 1表格HTTP GET结构图可知，HTTP有效数据为128bits，其余为协议开销。

因而协议开销为
$$
66+54+(182-128)+60=234bits
$$
协议开销占比为
$$
\frac{234}{234+128}\times100\%=64.6\%
$$

## 2.3 Turn-in 3: Hand in your answers to the two questions.

*（In Step5）*

***1.**  **Which Ethernet header field is the demultiplexing key that tells it the next higher layer is IP? What value is used in this field to indicate “IP”?*** 

***2.**  **Which IP header field is the demultiplexing key that tells it the next higher layer is TCP? What value is used in this field to indicate “TCP”?***

Demultiplexing Keys，即解复用键。当数据包从交给上层进行处理时，需要找到正确的上层协议，这个过程就叫做解复用；解复用键即在协议头中来指明上层协议的信息。

**1. Ethernet**：查看Wireshark的Ethernet字段，得知报头字段为 Type ，其值为 0x0800 。

![image-20231001131023485](./lab0-protocol-layers.assets/image-20231001131023485.png)

**2. IP**：IP报头字段为Protocol，其值为0x06 。

![image-20231001131041679](./lab0-protocol-layers.assets/image-20231001131041679.png)

# 03  Explore on your own

*1)*   ***Look at a short TCP packet that carries no higher-layer data. To what entity is this packet des-tined? After all, if it carries no higher-layer data then it does not seem very useful to a higher layer protocol such as HTTP!***

查看Wireshark中TCP协议包，如下图所示。

![image-20231001132008227](./lab0-protocol-layers.assets/image-20231001132008227.png)

建立TCP连接需要三次握手，在三次握手协议中，需要客户端向服务器建立连接请求、服务器向客户端确认请求、客户端向服务器确认收到，这些数据包都是不含更高层的短TCP。

*2)*   ***In a classic layered model, one message from a higher layer has a header appended by the lower layer and becomes one new message. But this is not always the case. Above, we saw a trace in which the web response (one HTTP message comprised of an HTTP header and an HTTP pay-load) was converted into multiple lower layer messages (being multiple TCP packets). Imagine that you have drawn the packet structure (as in step 2) for the first and last TCP packet carrying the web response. How will the drawings differ?*** 

![image-20231001132034181](./lab0-protocol-layers.assets/image-20231001132034181.png)

Web响应数据包如上图所示。其中第一个数据包为TCP协议，可以发现其中负载长度为0，仅包含通信控制信息；而最后一个显示为HTTP协议，其中包含web的200 ok响应。
