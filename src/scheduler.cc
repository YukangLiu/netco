//@author Liu Yukang
#include "../include/scheduler.h"

#include <sys/sysinfo.h>

using namespace netco;

Scheduler* Scheduler::pScher_ = nullptr;
std::mutex Scheduler::scherMtx_;

Scheduler::Scheduler()
	:proSelector_(processors_)
{ }

Scheduler::~Scheduler()
{
	for (auto pP : processors_)
	{
		pP->stop();
	}
	for (auto pP : processors_)
	{
		pP->join();
		delete pP;
	}
}

bool Scheduler::startScheduler(int threadCnt)
{
	for (int i = 0; i < threadCnt; ++i)
	{
		processors_.emplace_back(new Processor(i));
		processors_[i]->loop();
	}
	return true;
}

Scheduler* Scheduler::getScheduler()
{
	if (nullptr == pScher_)
	{
		std::lock_guard<std::mutex> lock(scherMtx_);
		if (nullptr == pScher_)
		{
			pScher_ = new Scheduler();
			pScher_->startScheduler(::get_nprocs_conf());
		}
	}
	return pScher_;
}

void Scheduler::createNewCo(std::function<void()>&& func, size_t stackSize)
{
	proSelector_.next()->goNewCo(std::move(func), stackSize);
}

void Scheduler::createNewCo(std::function<void()>& func, size_t stackSize)
{
	proSelector_.next()->goNewCo(func, stackSize);
}

void Scheduler::join()
{
	for (auto pP : processors_)
	{
		pP->join();
	}
}

Processor* Scheduler::getProcessor(int id)
{
	return processors_[id];
}

int Scheduler::getProCnt()
{
	return static_cast<int>(processors_.size());
}