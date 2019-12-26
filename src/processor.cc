//@author Liu Yukang
#include "processor.h"
#include "parameter.h"
#include "spinlock_guard.h"

#include <sys/epoll.h>
#include <unistd.h>

using namespace netco;

__thread int threadIdx = -1;

Processor::Processor(int tid)
	: tid_(tid), status_(PRO_STOPPED), pLoop_(nullptr), runningNewQue_(0), semaphore_(1), pCurCoroutine_(nullptr)
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

void Processor::goNewCo_aux(Coroutine* pCo)
{
	{
		SpinlockGuard lock(semaphore_);
		newCoroutines_[!runningNewQue_].push(pCo);
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
					SpinlockGuard lock(semaphore_);
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
					delete deadCo;
				}
				removedCo_.clear();
				
			}
			status_ = PRO_STOPPED;
		}
		);
	return true;
}

ssize_t Processor::read(int fd, char* buf, size_t len)
{
	epoller_.addEv(pCurCoroutine_, fd, EPOLLIN | EPOLLPRI | EPOLLRDHUP);
	yield();
	return ::read(fd, buf, len);
}

Socket Processor::accept(Socket& listener)
{
	epoller_.addEv(pCurCoroutine_,listener.fd(), EPOLLIN | EPOLLPRI | EPOLLRDHUP);
	yield();
	return listener.accept();
}