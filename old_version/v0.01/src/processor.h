//@author Liu Yukang
#pragma once
#include <queue>
#include <set>
#include <mutex>
#include <thread>
#include <atomic>
#include "context.h"
#include "coroutine.h"
#include "epoller.h"
#include "timer.h"
#include "socket.h"

extern __thread int threadIdx;

namespace netco
{
	enum processerStatus
	{
		PRO_RUNNING = 0,
		PRO_STOPPING,
		PRO_STOPPED
	};

	enum newCoAddingStatus
	{
		NEWCO_ADDING = 0,
		NEWCO_ADDED
	};

	class Processor
	{
	public:
		Processor(int);
		~Processor();

		DISALLOW_COPY_MOVE_AND_ASSIGN(Processor);

		//运行一个新协程，该协程的函数是func
		inline void goNewCo(std::function<void()>&& func);
		inline void goNewCo(std::function<void()>& func);

		void yield();

		//当前协程等待time毫秒
		void wait(Time time);

		//清除当前正在运行的协程
		inline void killCurCo();

		bool loop();

		inline void stop();

		inline void join();

		ssize_t read(int fd,char* buf,size_t len);

		Socket accept(Socket& listener);

		//获取当前正在运行的协程
		inline Coroutine* getCurRunningCo() { return pCurCoroutine_; };

		inline Context* getMainCtx() { return &mainCtx_; }

		inline size_t getCoCnt() { return coSet_.size(); }

	private:

		//恢复运行指定协程
		void resume(Coroutine*);

		void goNewCo_aux(Coroutine*);

		inline void wakeUpEpoller();

		//该处理器的线程号
		int tid_;

		int status_;

		std::thread* pLoop_;

		//新任务队列，使用双缓存队列
		std::queue<Coroutine*> newCoroutines_[2];

		//新任务双缓存队列中正在运行的队列号，另一条用于添加任务
		volatile int runningNewQue_;

		std::atomic_int semaphore_;

		std::mutex newCoQueMtx_;

		//EventEpoller发现的活跃事件所放的列表
		std::vector<Coroutine*> actCoroutines_;

		std::set<Coroutine*> coSet_;

		//定时器任务列表
		std::vector<Coroutine*> timerExpiredCo_;

		//被移除的协程列表，要移除某一个事件会先放在该列表中，一次循环结束才会真正delete
		std::vector<Coroutine*> removedCo_;

		Epoller epoller_;

		Timer timer_;

		Coroutine* pCurCoroutine_;

		Context mainCtx_;
	};

	inline void Processor::stop()
	{
		status_ = PRO_STOPPING;
	}

	inline void Processor::join()
	{
		pLoop_->join();
	}

	inline void Processor::wakeUpEpoller()
	{
		timer_.wakeUp();
	}

	inline void Processor::goNewCo(std::function<void()>&& coFunc)
	{
		Coroutine* pCo = new Coroutine(this, std::move(coFunc));
		goNewCo_aux(pCo);
	}

	inline void Processor::goNewCo(std::function<void()>& coFunc)
	{
		Coroutine* pCo = new Coroutine(this, coFunc);
		goNewCo_aux(pCo);
	}

	inline void Processor::killCurCo()
	{
		removedCo_.push_back(pCurCoroutine_);
	}

}

