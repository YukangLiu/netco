//@author Liu Yukang
#include <iostream>
#include "processor.h"
#include "netco_api.h"
#include "socket.h"

using namespace netco;

//processor wait test
void test_func1(netco::Processor& p)
{
	p.goNewCo([&p]
		{
			std::cout << "111" << std::endl;
			p.goNewCo(
				[&p]
				{
					std::cout << "222" << std::endl;
					p.wait(10000);
					std::cout << "222222" << std::endl;
				}
				,parameter::coroutineStackSize);
			std::cout << "111111" << std::endl;
		},parameter::coroutineStackSize);

	p.goNewCo([&p]
		{
			std::cout << "333" << std::endl;
			p.goNewCo(
				[&p]
				{
					std::cout << "444" << std::endl;
					p.wait(10000);
					std::cout << "444444" << std::endl;
				}
				, parameter::coroutineStackSize);
			std::cout << "333333" << std::endl;
		}, parameter::coroutineStackSize);
}

// //processor http response test
// void test_func2(netco::Processor& p)
// {
// 	netco::Socket listener;
// 	if (listener.isUseful())
// 	{
// 		listener.setTcpNoDelay(true);
// 		listener.setReuseAddr(true);
// 		listener.setReusePort(true);
// 		listener.setBlockSocket();
// 		if (listener.bind(80) < 0)
// 		{
// 			return ;
// 		}
// 		listener.listen();
// 	}
// 	while (1)
// 	{
// 		netco::Socket conn(listener.accept());
// 		conn.setTcpNoDelay(true);
// 		p.goNewCo(
// 			[&p,conn]
// 			{
// 			    std::string hello("HTTP/1.0 200 OK\r\nServer: netco/0.1.0\r\nContent-Length: 72\r\nContent-Type: text/html\r\n\r\n<HTML><TITLE>hello</TITLE>\r\n<BODY><P>hello word!\r\n</BODY></HTML>\r\n");
// 				char buf[1024];
// 				if (p.read(conn.fd(), buf, 1024) > 0)
// 				{
// 				    send(conn.fd(),hello.c_str(),hello.size(),MSG_NOSIGNAL);
// 				    p.wait(50);//需要等一下，否则还没发送完毕就关闭了
// 				}
// 			}
// 			, parameter::coroutineStackSize);
// 	}
// }

//netco wait test
void test_func3()
{
	netco::co_go([]
		{
			std::cout << "111" << std::endl;
			netco::co_go(
				[]
				{
					std::cout << "222" << std::endl;
					netco::co_wait(10000);
					std::cout << "222222" << std::endl;
				}
				);
			std::cout << "111111" << std::endl;
		});

	netco::co_go([]
		{
			std::cout << "333" << std::endl;
			netco::co_go(
				[]
				{
					std::cout << "444" << std::endl;
					netco::co_wait(10000);
					std::cout << "444444" << std::endl;
				}
				);
			std::cout << "333333" << std::endl;
		});
}

//netco http response with one acceptor test 
void test_func4()
{
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
					netco::co_wait(50);//需要等一下，否则还没发送完毕就关闭了
				}
				delete conn;
			}
			);
	}
}

//netco http response with multi acceptor test 
void test_func5()
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
								netco::co_wait(50);//需要等一下，否则还没发送完毕就关闭了
							}
							delete conn;
						}
						);
				}
			}
			,parameter::coroutineStackSize, i);
	}
	
}

void test_func6(){
	netco::co_go(
				[]
				{
					char buf[1024];
					while(1){
						netco::co_wait(2000);
						netco::Socket s;
						s.connect("127.0.0.1", 8099);
						s.send("ping", 4);
						s.read(buf, 1024);
						std::cout << std::string(buf) << std::endl;
					}
				}
				);
}

int main()
{
	test_func5();
	test_func6();
	netco::sche_join();
	std::cout << "end" << std::endl;
	return 0;
}