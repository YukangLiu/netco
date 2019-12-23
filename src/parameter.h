//@author Liu Yukang
#pragma once
#include <stddef.h>

namespace netco
{
	namespace parameter
	{
		//协程栈大小
		const static size_t coroutineStackSize = 32 * 1024;

		//获取活跃的epoll_event的数组的初始长度
		static constexpr int epollerListFirstSize = 16;

		//epoll_wait的阻塞时长
		static constexpr int epollTimeOutMs = 10000;

		//监听队列的长度
		constexpr static unsigned backLog = 1024;
	}
	
}