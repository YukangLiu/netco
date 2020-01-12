//@author Liu Yukang
#include "netco_api.h"

void netco::co_go(std::function<void()>&& func, size_t stackSize, int tid)
{
	if (tid < 0)
	{
		netco::Scheduler::getScheduler()->createNewCo(std::move(func), stackSize);
	}
	else
	{
		tid %= netco::Scheduler::getScheduler()->getProCnt();
		netco::Scheduler::getScheduler()->getProcessor(tid)->goNewCo(std::move(func), stackSize);
	}
}

void netco::co_go(std::function<void()>& func, size_t stackSize, int tid)
{
	if (tid < 0)
	{
		netco::Scheduler::getScheduler()->createNewCo(func, stackSize);
	}
	else
	{
		tid %= netco::Scheduler::getScheduler()->getProCnt();
		netco::Scheduler::getScheduler()->getProcessor(tid)->goNewCo(func, stackSize);
	}
}

void netco::co_wait(Time time)
{
	netco::Scheduler::getScheduler()->getProcessor(threadIdx)->wait(time);
}

void netco::sche_join()
{
	netco::Scheduler::getScheduler()->join();
}

ssize_t netco::co_read(int fd, char* buf, size_t len)
{
	return netco::Scheduler::getScheduler()->getProcessor(threadIdx)->read(fd, buf, len);
}

netco::Socket netco::co_accept(netco::Socket listener)
{
	return netco::Scheduler::getScheduler()->getProcessor(threadIdx)->accept(listener);
}
