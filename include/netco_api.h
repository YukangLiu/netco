//@author Liu Yukang
#pragma once
#include "scheduler.h"
#include "mstime.h"

namespace netco 
{
	////////////////////协程调用相关接口///////////////////

	//并发运行一个协程，协程将运行func函数
	//可以指定协程栈大小stackSize，默认为2k
	//tid：协程要运行在哪个线程上
	//		-1： 使用netco调度器选择协程运行在哪个线程
	//		other：线程号
	//netco会根据处理器个数创建等数量的线程用于运行协程，
	//若有4个处理器，则tid只有0 ~ 3是有效的 
	void co_go(std::function<void()>& func, size_t stackSize = parameter::coroutineStackSize, int tid = -1);
	void co_go(std::function<void()>&& func, size_t stackSize = parameter::coroutineStackSize, int tid = -1);

	//当前协程等待t毫秒后再继续执行
	void co_sleep(Time t);

	//等待调度器的结束
	void sche_join();

	//////////////网络相关系统调用使用Socket.h头中的封装//////////
}


