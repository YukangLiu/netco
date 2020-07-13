//@author Liu Yukang
#include "scheduler.h"

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