//@author Liu Yukang
#include "../include/processor.h"
#include "../include/parameter.h"
#include "../include/spinlock_guard.h"

#include <sys/epoll.h>
#include <unistd.h>

using namespace netco;

__thread int threadIdx = -1;

Processor::Processor(int tid)
	: tid_(tid), status_(PRO_STOPPED), pLoop_(nullptr), runningNewQue_(0), pCurCoroutine_(nullptr), mainCtx_(0)
{
	mainCtx_.makeCurContext();
}

Processor::~Processor()
{
	if (PRO_RUNNING == status_)
	{
		stop();
	}
	if (PRO_STOPPING == status_)
	{
		join();
	}
	if (nullptr != pLoop_)
	{
		delete pLoop_;
	}
	for (auto co : coSet_)
	{
		delete co;
	}
}

void Processor::resume(Coroutine* pCo)
{
	if (nullptr == pCo)
	{
		return;
	}

	if (coSet_.find(pCo) == coSet_.end())
	{
		return;
	}
	
	pCurCoroutine_ = pCo;
	pCo->resume();
}

void Processor::yield()
{
	pCurCoroutine_->yield();
	mainCtx_.swapToMe(pCurCoroutine_->getCtx());
}

void Processor::wait(Time time)
{
	pCurCoroutine_->yield();
	timer_.runAfter(time,pCurCoroutine_);
	mainCtx_.swapToMe(pCurCoroutine_->getCtx());
}

void Processor::goCo(Coroutine* pCo)
{
	{
		SpinlockGuard lock(newQueLock_);
		newCoroutines_[!runningNewQue_].push(pCo);
	}
	wakeUpEpoller();
}

void Processor::goCoBatch(std::vector<Coroutine*>& cos){
	{
		SpinlockGuard lock(newQueLock_);
		for(auto pCo : cos){
			newCoroutines_[!runningNewQue_].push(pCo);
		}
	}
	wakeUpEpoller();
}

bool Processor::loop()
{
	//初始化Epoller
	if (!epoller_.init())
	{
		return false;
	}

	//初始化Timer
	if (!timer_.init(&epoller_))
	{
		return false;
	}

	//初始化loop
	pLoop_ = new std::thread(
		[this]
		{
			threadIdx = tid_;
			status_ = PRO_RUNNING;
			while (PRO_RUNNING == status_)
			{
				//清空所有列表
				if (actCoroutines_.size())
				{
					actCoroutines_.clear();
				}
				if (timerExpiredCo_.size())
				{
					timerExpiredCo_.clear();
				}
				//获取活跃事件
				epoller_.getActEvServ(parameter::epollTimeOutMs, actCoroutines_);

				//处理超时协程
				timer_.getExpiredCoroutines(timerExpiredCo_);
				size_t timerCoCnt = timerExpiredCo_.size();
				for (size_t i = 0; i < timerCoCnt; ++i)
				{
					resume(timerExpiredCo_[i]);
				}

				//执行新来的协程
				Coroutine* pNewCo = nullptr;
				int runningQue = runningNewQue_;
				
				while (!newCoroutines_[runningQue].empty())
				{
					{
						pNewCo = newCoroutines_[runningQue].front();
						newCoroutines_[runningQue].pop();
						coSet_.insert(pNewCo);
					}
					resume(pNewCo);
				}

				{
					SpinlockGuard lock(newQueLock_);
					runningNewQue_ = !runningQue;
				}

				//执行被唤醒的协程
				size_t actCoCnt = actCoroutines_.size();
				for (size_t i = 0; i < actCoCnt; ++i)
				{
					resume(actCoroutines_[i]);
				}

				//清除已经执行完毕的协程
				for (auto deadCo : removedCo_)
				{
					coSet_.erase(deadCo);
					//delete deadCo;
					{
						SpinlockGuard lock(coPoolLock_);
						coPool_.delete_obj(deadCo);
					}
				}
				removedCo_.clear();
				
			}
			status_ = PRO_STOPPED;
		}
		);
	return true;
}

//等待fd上的ev事件返回
void Processor::waitEvent(int fd, int ev){
	epoller_.addEv(pCurCoroutine_, fd, ev);
	yield();
	epoller_.removeEv(pCurCoroutine_, fd, ev);
}

void Processor::stop()
{
	status_ = PRO_STOPPING;
}

void Processor::join()
{
	pLoop_->join();
}

void Processor::wakeUpEpoller()
{
	timer_.wakeUp();
}

void Processor::goNewCo(std::function<void()>&& coFunc, size_t stackSize)
{
	//Coroutine* pCo = new Coroutine(this, stackSize, std::move(coFunc));
	Coroutine* pCo = nullptr;

	{
		SpinlockGuard lock(coPoolLock_);
		pCo = coPool_.new_obj(this, stackSize, std::move(coFunc));
	}

	goCo(pCo);
}

void Processor::goNewCo(std::function<void()>& coFunc, size_t stackSize)
{
	//Coroutine* pCo = new Coroutine(this, stackSize, coFunc);
	Coroutine* pCo = nullptr;

	{
		SpinlockGuard lock(coPoolLock_);
		pCo = coPool_.new_obj(this, stackSize, coFunc);
	}
	
	goCo(pCo);
}

void Processor::killCurCo()
{
	removedCo_.push_back(pCurCoroutine_);
}
