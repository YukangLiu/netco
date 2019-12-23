# 版本说明

旧版本都放在old_version文件夹中<br>
<br>
<br>
V0.01:<br>
1、Scheduler->Processor->Coroutine层次结构<br>
2、对ucontext封装了上下文类。<br>
3、对称协程切换方法。<br>
4、epoller和定时器实现。<br>
5、无锁主循环。<br>
6、co_go(func)的执行接口，co_wait(time)的等待接口，co_read(),co_accept的网络编程接口。<br>
<br>
<br>
V0.02:(当前版本，持续更新，打√的项为已完成):<br>
1、实现更多的网络编程接口。<br>
2、对网络编程系统调用进行hook。<br>