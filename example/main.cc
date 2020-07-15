//@author Liu Yukang
#include <iostream>

#include "processor.h"
#include "netco_api.h"
#include "socket.h"
#include "mutex.h"

using namespace netco;

//netco http response with one acceptor test 
//只有一个acceptor的服务
void single_acceptor_server_test()
{
	netco::co_go(
		[]{
			netco::Socket listener;
			if (listener.isUseful())
			{
				listener.setTcpNoDelay(true);
				listener.setReuseAddr(true);
				listener.setReusePort(true);
				listener.setBlockSocket();
				if (listener.bind(8099) < 0)
				{
					return;
				}
				listener.listen();
			}
			while (1){
				netco::Socket* conn = new netco::Socket(listener.accept());
				conn->setTcpNoDelay(true);
				netco::co_go(
					[conn]
					{
						std::string hello("HTTP/1.0 200 OK\r\nServer: netco/0.1.0\r\nContent-Length: 72\r\nContent-Type: text/html\r\n\r\n<HTML><TITLE>hello</TITLE>\r\n<BODY><P>hello word!\r\n</BODY></HTML>\r\n");
						//std::string body("<HTML><TITLE>hello</TITLE>\r\n<BODY><P>hello word!\r\n</BODY></HTML>\r\n");
						char buf[1024];
						if (conn->read((void*)buf, 1024) > 0)
						{
							conn->send(hello.c_str(), hello.size());
							netco::co_sleep(50);//需要等一下，否则还没发送完毕就关闭了
						}
						delete conn;
					}
					);
			}
		}
	);
}

//netco http response with multi acceptor test 
//每条线程一个acceptor的服务
void multi_acceptor_server_test()
{
	for (int i = 0; i < 4; ++i)
	{
		netco::co_go(
			[]
			{
				netco::Socket listener;
				if (listener.isUseful())
				{
					listener.setTcpNoDelay(true);
					listener.setReuseAddr(true);
					listener.setReusePort(true);
					if (listener.bind(8099) < 0)
					{
						return;
					}
					listener.listen();
				}
				while (1)
				{
					netco::Socket* conn = new netco::Socket(listener.accept());
					conn->setTcpNoDelay(true);
					netco::co_go(
						[conn]
						{
							std::string hello("HTTP/1.0 200 OK\r\nServer: netco/0.1.0\r\nContent-Length: 72\r\nContent-Type: text/html\r\n\r\n<HTML><TITLE>hello</TITLE>\r\n<BODY><P>hello word!\r\n</BODY></HTML>\r\n");
							//std::string body("<HTML><TITLE>hello</TITLE>\r\n<BODY><P>hello word!\r\n</BODY></HTML>\r\n");
							char buf[1024];
							if (conn->read((void*)buf, 1024) > 0)
							{
								conn->send(hello.c_str(), hello.size());
								netco::co_sleep(50);//需要等一下，否则还没发送完毕就关闭了
							}
							delete conn;
						}
						);
				}
			}
			,parameter::coroutineStackSize, i);
	}
	
}

//作为客户端的测试，可配合上述server测试
void client_test(){
	netco::co_go(
				[]
				{
					char buf[1024];
					while(1){
						netco::co_sleep(2000);
						netco::Socket s;
						s.connect("127.0.0.1", 8099);
						s.send("ping", 4);
						s.read(buf, 1024);
						std::cout << std::string(buf) << std::endl;
					}
				}
				);
}

//读写锁测试
void mutex_test(netco::RWMutex& mu){
	for(int i = 0; i < 10; ++i)
	if(i < 5){
		netco::co_go(
		[&mu, i]{
			mu.rlock();
			std::cout << i << " : start reading" << std::endl;
			netco::co_sleep(100);
			std::cout << i << " : finish reading" << std::endl;
			mu.runlock();
			mu.wlock();
			std::cout << i << " : start writing" << std::endl;
			netco::co_sleep(100);
			std::cout << i << " : finish writing" << std::endl;
			mu.wunlock();
		}
		);
	}else{
		netco::co_go(
		[&mu, i]{
			mu.wlock();
			std::cout << i << " : start writing" << std::endl;
			netco::co_sleep(100);
			std::cout << i << " : finish writing" << std::endl;
			mu.wunlock();
			mu.rlock();
			std::cout << i << " : start reading" << std::endl;
			netco::co_sleep(100);
			std::cout << i << " : finish reading" << std::endl;
			mu.runlock();
		}
	);
	}
	
}

int main()
{
	netco::RWMutex mu;
	test_mutex(mu);
	//test_func5();
	//test_func6();
	netco::sche_join();
	std::cout << "end" << std::endl;
	return 0;
}