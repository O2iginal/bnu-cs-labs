- 示例代码需要修改（有bug）
- 关于 [siginal](https://www.runoob.com/cprogramming/c-function-signal.html) （这里似乎是用于程序中断后的收尾工作）
- 消息队列在一次运行结束时可能没有被删除，从而影响下次运行
	- 查看系统消息队列命令 `ipcs`
	- 删除消息队列 `ipcrm`
	- 参考[ipcs & ipcrm](https://www.cnblogs.com/machangwei-8/p/10388824.html)
	