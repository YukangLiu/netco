# netco协程库
 <br>
 blog: http://www.liuyukang.com/archives/netco <br>
<br>
1、定位：<br>
	netco协程库是纯c++11实现的轻量级对称协程库，有着极简的协程创建接口设计和用于并发网络编程的接口。<br>
	若不需要用于网络编程，可使用我另一个更轻量的协程库tinyco：https://github.com/YukangLiu/tinyco <br>
<br>
<br>
2、使用：<br>
	编译src文件夹源码，使用时包含netco_api.h，即可使用netco协程库接口：<br>
	co_go(func)执行一条协程；<br>
	co_sleep(time)等待time毫秒时间后继续执行当前协程；<br>
	<br>
	包含socket.h，即可使用socket族函数接口：<br>
	Socket s; //创建一个socket类<br>
	s.bind(80); //绑定端口号80<br>
	s.listen(); //开始监听<br>
	Socket conSock(s.accept()); //accept一个连接<br>
	conSock.read(buf, 1024); //读连接上的数据<br>
	conSock.send(buf, 1024); //往对端写数据<br>
	Socket clientSock;<br>
	clientSock.connect("127.0.0.1", 80); //建立一个连接,之后即可如上述read和send<br>
	<br>
	包含mutex.h，即可使用读写锁，因为读写锁包含了普通的互斥锁功能，不对互斥锁做单独实现<br>
	rlock()读锁，runlock()读解锁，wlock()写锁，wunlock()写解锁；<br>
	<br>
	具体用法可见example<br>
<br>
<br>
3、文件：<br>
	src：netco协程库的源代码。<br>
	old_version：旧版本源码。<br>
	example:测试示例。<br>
	<br>
	netco类图如下：<br>
	
![uml](https://github.com/YukangLiu/netco/raw/master/pic/netcouml.png)
<br>
<br>
4、编译：<br>
	src：make即可,会生成libnetco.so。<br>
	example：make了netco之后进入该文件夹make即可。<br>
<br>
<br>
5、并发度<br>
	测试环境：4核CPU3.70GHz，8G内存3200MHz<br>
	使用webbench对回复hello world的功能进行了简单的压力测试，QPS四万多：<br>

![qps](https://github.com/YukangLiu/netco/raw/master/pic/webbench-c13000-t10.png)
<br>
<br>
附：有什么需求或者bug，建议，问题，都可以邮件 390161495@qq.com 讨论交流，谢谢。<br>
