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
            margin: 20px;
            line-height: 1.3;
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
            <div id="experiment">路由交换实验2——OSPF</div>
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
                    <span class="text">2023-11-09</span>
                </span>
            </div>
        </div>
    </div>
</body>
</html>

<center><b><h1>实验报告目录</h1></b></center>
[TOC]


# 01 实验概述

**实验目的**

1. 掌握单区域 OSPF 的配置；
2. 理解链路状态路由协议的工作过程；
3. 掌握实验环境中环回接口的配置；

**实验环境**
- 软件版本
  - Cisco Packet Tracer 6.0.0.0045
- 设备型号即数量
  - Router 2911 (3个)
  - Switches 2960-24TT （2个）
  - PC-PT（2个）
  - 网线若干


# 02 实验拓扑
本实验拓扑如下图所示。

![Alt text](image-2.png)

路由器端口地址配置如下表。

|       |        R0        |       |        R1        |       |        R2        |
| :---: | :--------------: | :---: | :--------------: | :---: | :--------------: |
| gi0/0 | 192.168.1.1 / 24 | gi0/0 | 192.168.2.2 / 24 | gi0/0 | 192.168.3.2 / 24 |
| gi0/1 | 192.168.2.1 / 24 | gi0/1 | 192.168.3.1 / 24 | gi0/1 | 192.168.4.1 / 24 |

# 03 实验步骤

## 3.1 路由器接口配置
**分别进入R0、R1、R1的命令行，配置各自的接口IP地址。**

R0的配置命令如下：

```shell
interface GigabitEthernet 0/0
ip address 192.168.1.1 255.255.255.0
no shutdown
exit
interface GigabitEthernet 0/1
ip address 192.168.2.1 255.255.255.0
no shutdown 
exit
```

R1的配置命令如下：

```shell
interface GigabitEthernet 0/0
ip address 192.168.2.2 255.255.255.0
no shutdown
exit
interface GigabitEthernet 0/1
ip address 192.168.3.1 255.255.255.0
no shutdown 
exit
```

R2的配置命令如下：

```shell
interface GigabitEthernet 0/0
ip address 192.168.3.2 255.255.255.0
no shutdown
exit
interface GigabitEthernet 0/1
ip address 192.168.4.1 255.255.255.0
no shutdown 
exit
```

**检验各接口是否配置正确**

R0接口IP检验：

```shell
Router#show ip interface brief 
Interface              IP-Address      OK? Method Status                Protocol
 
GigabitEthernet0/0     192.168.1.1     YES manual up                    up
 
GigabitEthernet0/1     192.168.2.1     YES manual up                    up
 
GigabitEthernet0/2     unassigned      YES unset  administratively down down
 
Vlan1                  unassigned      YES unset  administratively down down
```

R1接口IP检验：

```shell
Router#show ip interface brief 
Interface              IP-Address      OK? Method Status                Protocol
 
GigabitEthernet0/0     192.168.2.2     YES manual up                    up
 
GigabitEthernet0/1     192.168.3.1     YES manual up                    up
 
GigabitEthernet0/2     unassigned      YES unset  administratively down down
 
Vlan1                  unassigned      YES unset  administratively down down
```

R2接口IP检验：

```shell
Router#show ip interface brief 
Interface              IP-Address      OK? Method Status                Protocol
 
GigabitEthernet0/0     192.168.3.2     YES manual up                    up
 
GigabitEthernet0/1     192.168.4.1     YES manual up                    up
 
GigabitEthernet0/2     unassigned      YES unset  administratively down down
 
Vlan1                  unassigned      YES unset  administratively down down
```

## 3.2 PC机配置
配置PC0的IP如下图所示：
![Alt text](image.png)

配置PC1的IP如下图所示：
![Alt text](image-1.png)

## 3.3 OSPF配置
### 3.3.1 配置路由器loopback接口

进入R0的命令行，配置loopback接口。

```shell
interface loopback 0
ip address 1.1.1.1 255.255.255.255
no shutdown
```

进入R1的命令行，配置loopback接口。

```shell
interface loopback 0
ip address 2.2.2.2 255.255.255.255
no shutdown
```

进入R2的命令行，配置loopback接口。

```shell
interface loopback 0
ip address 3.3.3.3 255.255.255.255
no shutdown
```

检验loopback接口配置是否正确，以R0为例：

```shell
Router#show ip interface brief 
Interface              IP-Address      OK? Method Status                Protocol
 
GigabitEthernet0/0     192.168.1.1     YES manual up                    up
 
GigabitEthernet0/1     192.168.2.1     YES manual up                    up
 
GigabitEthernet0/2     unassigned      YES unset  administratively down down
 
Loopback0              1.1.1.1         YES manual up                    up
 
Vlan1                  unassigned      YES unset  administratively down down
```

### 3.3.2 配置OSPF

进入R0的命令行，配置OSPF。

```shell
router ospf 1
network 192.168.1.0 255.255.255.0 area 0
network 192.168.2.0 255.255.255.0 area 0
network 1.1.1.1 0.0.0.0 area 0  # 非必须，使得路由器暴露在外部可被访问

```

进入R1的命令行，配置OSPF。

```shell
router ospf 1
network 192.168.2.0 255.255.255.0 area 0
network 192.168.3.0 255.255.255.0 area 0
network 2.2.2.2 0.0.0.0 area 0  # 非必须，使得路由器暴露在外部可被访问
```

进入R2的命令行，配置OSPF。

```shell
router ospf 1
network 192.168.3.0 255.255.255.0 area 0
network 192.168.4.0 255.255.255.0 area 0
network 3.3.3.3 0.0.0.0 area 0  # 非必须，使得路由器暴露在外部可被访问
```

### 3.3.3 检验OSPF配置是否正确
进入R0的命令行，检验OSPF配置是否正确。

```shell
Router#show ip route
Codes: L - local, C - connected, S - static, R - RIP, M - mobile, B - BGP
       D - EIGRP, EX - EIGRP external, O - OSPF, IA - OSPF inter area
       N1 - OSPF NSSA external type 1, N2 - OSPF NSSA external type 2
       E1 - OSPF external type 1, E2 - OSPF external type 2, E - EGP
       i - IS-IS, L1 - IS-IS level-1, L2 - IS-IS level-2, ia - IS-IS inter area
       * - candidate default, U - per-user static route, o - ODR
       P - periodic downloaded static route

Gateway of last resort is not set

     1.0.0.0/32 is subnetted, 1 subnets
C       1.1.1.1/32 is directly connected, Loopback0
     2.0.0.0/32 is subnetted, 1 subnets
O       2.2.2.2/32 [110/2] via 192.168.2.2, 00:12:39, GigabitEthernet0/1
     192.168.1.0/24 is variably subnetted, 2 subnets, 2 masks
C       192.168.1.0/24 is directly connected, GigabitEthernet0/0
L       192.168.1.1/32 is directly connected, GigabitEthernet0/0
     192.168.2.0/24 is variably subnetted, 2 subnets, 2 masks
C       192.168.2.0/24 is directly connected, GigabitEthernet0/1
L       192.168.2.1/32 is directly connected, GigabitEthernet0/1
O    192.168.3.0/24 [110/2] via 192.168.2.2, 00:12:39, GigabitEthernet0
```

进入R1的命令行，检验OSPF配置是否正确。

```shell
Router#show ip route 
Codes: L - local, C - connected, S - static, R - RIP, M - mobile, B - BGP
       D - EIGRP, EX - EIGRP external, O - OSPF, IA - OSPF inter area
       N1 - OSPF NSSA external type 1, N2 - OSPF NSSA external type 2
       E1 - OSPF external type 1, E2 - OSPF external type 2, E - EGP
       i - IS-IS, L1 - IS-IS level-1, L2 - IS-IS level-2, ia - IS-IS inter area
       * - candidate default, U - per-user static route, o - ODR
       P - periodic downloaded static route

Gateway of last resort is not set

     1.0.0.0/32 is subnetted, 1 subnets
O       1.1.1.1/32 [110/2] via 192.168.2.1, 00:09:55, GigabitEthernet0/0
     2.0.0.0/32 is subnetted, 1 subnets
C       2.2.2.2/32 is directly connected, Loopback0
     3.0.0.0/32 is subnetted, 1 subnets
O       3.3.3.3/32 [110/2] via 192.168.3.2, 00:07:53, GigabitEthernet0/1
O    192.168.1.0/24 [110/2] via 192.168.2.1, 00:19:06, GigabitEthernet0/0
     192.168.2.0/24 is variably subnetted, 2 subnets, 2 masks
C       192.168.2.0/24 is directly connected, GigabitEthernet0/0
L       192.168.2.2/32 is directly connected, GigabitEthernet0/0
     192.168.3.0/24 is variably subnetted, 2 subnets, 2 masks
C       192.168.3.0/24 is directly connected, GigabitEthernet0/1
L       192.168.3.1/32 is directly connected, GigabitEthernet0/1
O    192.168.4.0/24 [110/2] via 192.168.3.2, 00:19:45, GigabitEthernet0/1
```

进入R2的命令行，检验OSPF配置是否正确。

```shell
Router#show ip route 
Codes: L - local, C - connected, S - static, R - RIP, M - mobile, B - BGP
       D - EIGRP, EX - EIGRP external, O - OSPF, IA - OSPF inter area
       N1 - OSPF NSSA external type 1, N2 - OSPF NSSA external type 2
       E1 - OSPF external type 1, E2 - OSPF external type 2, E - EGP
       i - IS-IS, L1 - IS-IS level-1, L2 - IS-IS level-2, ia - IS-IS inter area
       * - candidate default, U - per-user static route, o - ODR
       P - periodic downloaded static route

Gateway of last resort is not set

     1.0.0.0/32 is subnetted, 1 subnets
O       1.1.1.1/32 [110/3] via 192.168.3.1, 00:10:25, GigabitEthernet0/0
     2.0.0.0/32 is subnetted, 1 subnets
O       2.2.2.2/32 [110/2] via 192.168.3.1, 00:10:05, GigabitEthernet0/0
     3.0.0.0/32 is subnetted, 1 subnets
C       3.3.3.3/32 is directly connected, Loopback0
O    192.168.1.0/24 [110/3] via 192.168.3.1, 00:16:26, GigabitEthernet0/0
O    192.168.2.0/24 [110/2] via 192.168.3.1, 00:16:26, GigabitEthernet0/0
     192.168.3.0/24 is variably subnetted, 2 subnets, 2 masks
C       192.168.3.0/24 is directly connected, GigabitEthernet0/0
L       192.168.3.2/32 is directly connected, GigabitEthernet0/0
     192.168.4.0/24 is variably subnetted, 2 subnets, 2 masks
C       192.168.4.0/24 is directly connected, GigabitEthernet0/1
L       192.168.4.1/32 is directly connected, GigabitEthernet0/1
```

## 3.4 ping测试

### 3.4.1 PC0 ping PC1

PC0 ping PC1的结果如下。
```shell
PC>ping 192.168.4.100

Pinging 192.168.4.100 with 32 bytes of data:

Request timed out.
Reply from 192.168.4.100: bytes=32 time=1ms TTL=125
Reply from 192.168.4.100: bytes=32 time=0ms TTL=125
Reply from 192.168.4.100: bytes=32 time=0ms TTL=125

Ping statistics for 192.168.4.100:
    Packets: Sent = 4, Received = 3, Lost = 1 (25% loss),
Approximate round trip times in milli-seconds:
    Minimum = 0ms, Maximum = 1ms, Average = 0ms

PC>ping 192.168.4.100

Pinging 192.168.4.100 with 32 bytes of data:

Reply from 192.168.4.100: bytes=32 time=0ms TTL=125
Reply from 192.168.4.100: bytes=32 time=1ms TTL=125
Reply from 192.168.4.100: bytes=32 time=1ms TTL=125
Reply from 192.168.4.100: bytes=32 time=1ms TTL=125

Ping statistics for 192.168.4.100:
    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss),
Approximate round trip times in milli-seconds:
    Minimum = 0ms, Maximum = 1ms, Average = 0ms
```

### 3.4.2 PC1 ping PC0

PC1 ping PC0的结果如下。
```shell
PC>ping 192.168.1.100

Pinging 192.168.1.100 with 32 bytes of data:

Reply from 192.168.1.100: bytes=32 time=2ms TTL=125
Reply from 192.168.1.100: bytes=32 time=0ms TTL=125
Reply from 192.168.1.100: bytes=32 time=1ms TTL=125
Reply from 192.168.1.100: bytes=32 time=0ms TTL=125

Ping statistics for 192.168.1.100:
    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss),
Approximate round trip times in milli-seconds:
    Minimum = 0ms, Maximum = 2ms, Average = 0ms
```

### 3.4.3 PC0 ping R2

PC0 ping R2的结果如下。
```shell
PC>ping 3.3.3.3

Pinging 3.3.3.3 with 32 bytes of data:

Reply from 3.3.3.3: bytes=32 time=1ms TTL=253
Reply from 3.3.3.3: bytes=32 time=0ms TTL=253
Reply from 3.3.3.3: bytes=32 time=0ms TTL=253
Reply from 3.3.3.3: bytes=32 time=1ms TTL=253

Ping statistics for 3.3.3.3:
    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss),
Approximate round trip times in milli-seconds:
    Minimum = 0ms, Maximum = 1ms, Average = 0ms

PC>
```

# 04 实验总结及思考

## 4.1 实验总结
本次实验主要是配置单区域OSPF，配置后经测试，PC0可以ping通PC1，PC1可以ping通PC0，PC0可以ping通R2，PC1可以ping通R2，说明配置成功。

## 4.2 实验思考题
*动态路由配置命令解析：开启命令，配置哪些网络段，路由器间 ospf 进程 id 可以一样吗？*

答：

在OSPF协议中，不同的路由器可以具有相同的进程ID。这个进程ID主要用于在OSPF区域内标识和区分不同的路由器。ID是一个32位的数字，通常在1到65535之间选择。

对于同一个区域内的路由器来说，OSPF进程ID必须相同，这样才能保证它们正常地交换路由信息。而对于不同区域的路由器OSPF进程ID可以不同。这使得可以方便地在不同的区域之间进行路由信息的汇总和聚合。

# 05 配置文件

R0的配置如下：

```shell
Router#show running-config 
Building configuration...

Current configuration : 876 bytes
!
version 15.1
no service timestamps log datetime msec
no service timestamps debug datetime msec
no service password-encryption
!
hostname Router
!
!
!
!
!
!
!
!
!
!
!
!
license udi pid CISCO2911/K9 sn FTX1524EMFR
!
!
!
!
!
spanning-tree mode pvst
!
!
!
!
interface Loopback0
 ip address 1.1.1.1 255.255.255.255
!
interface GigabitEthernet0/0
 ip address 192.168.1.1 255.255.255.0
 duplex auto
 speed auto
!
interface GigabitEthernet0/1
 ip address 192.168.2.1 255.255.255.0
 duplex auto
 speed auto
!
interface GigabitEthernet0/2
 no ip address
 duplex auto
 speed auto
 shutdown
!
interface Vlan1
 no ip address
 shutdown
!
router ospf 1
 router-id 1.1.1.1
 log-adjacency-changes
 network 192.168.1.0 0.0.0.255 area 0
 network 192.168.2.0 0.0.0.255 area 0
 network 1.1.1.1 0.0.0.0 area 0
!
ip classless
!
!
!
!
!
!
!
line con 0
!
line aux 0
!
line vty 0 4
 login
!
!
!
end
```

R1的配置如下：

```shell
Router#show running-config 
Building configuration...

Current configuration : 876 bytes
!
version 15.1
no service timestamps log datetime msec
no service timestamps debug datetime msec
no service password-encryption
!
hostname Router
!
!
!
!
!
!
!
!
!
!
!
!
license udi pid CISCO2911/K9 sn FTX15248S87
!
!
!
!
!
spanning-tree mode pvst
!
!
!
!
interface Loopback0
 ip address 2.2.2.2 255.255.255.255
!
interface GigabitEthernet0/0
 ip address 192.168.2.2 255.255.255.0
 duplex auto
 speed auto
!
interface GigabitEthernet0/1
 ip address 192.168.3.1 255.255.255.0
 duplex auto
 speed auto
!
interface GigabitEthernet0/2
 no ip address
 duplex auto
 speed auto
 shutdown
!
interface Vlan1
 no ip address
 shutdown
!
router ospf 1
 router-id 2.2.2.2
 log-adjacency-changes
 network 192.168.2.0 0.0.0.255 area 0
 network 192.168.3.0 0.0.0.255 area 0
 network 2.2.2.2 0.0.0.0 area 0
!
ip classless
!
!
!
!
!
!
!
line con 0
!
line aux 0
!
line vty 0 4
 login
!
!
!
end
```

R2的配置如下：

```shell
Router#show running-config 
Building configuration...

Current configuration : 876 bytes
!
version 15.1
no service timestamps log datetime msec
no service timestamps debug datetime msec
no service password-encryption
!
hostname Router
!
!
!
!
!
!
!
!
!
!
!
!
license udi pid CISCO2911/K9 sn FTX15240BCT
!
!
!
!
!
spanning-tree mode pvst
!
!
!
!
interface Loopback0
 ip address 3.3.3.3 255.255.255.255
!
interface GigabitEthernet0/0
 ip address 192.168.3.2 255.255.255.0
 duplex auto
 speed auto
!
interface GigabitEthernet0/1
 ip address 192.168.4.1 255.255.255.0
 duplex auto
 speed auto
!
interface GigabitEthernet0/2
 no ip address
 duplex auto
 speed auto
 shutdown
!
interface Vlan1
 no ip address
 shutdown
!
router ospf 1
 router-id 3.3.3.3
 log-adjacency-changes
 network 192.168.3.0 0.0.0.255 area 0
 network 192.168.4.0 0.0.0.255 area 0
 network 3.3.3.3 0.0.0.0 area 0
!
ip classless
!
!
!
!
!
!
!
line con 0
!
line aux 0
!
line vty 0 4
 login
!
!
!
end

```