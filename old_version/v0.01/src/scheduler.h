//@author Liu Yukang
#pragma once
#include <vector>
#include <functional>

#include "processor.h"
#include "processor_selector.h"

namespace netco
{

	class Scheduler
	{
	protected:
		Scheduler();
		~Scheduler();

	public:

		DISALLOW_COPY_MOVE_AND_ASSIGN(Scheduler);

		static Scheduler* getScheduler();

		//在idx号线程创建新协程
		inline void createNewCo(std::function<void()>&& func);
		inline void createNewCo(std::function<void()>& func);

		inline Processor* getProcessor(int);

		inline int getProCnt();

		inline void join();

	private:
		//初始化Scheduler，threadCnt为开启几个线程
		bool startScheduler(int threadCnt);

		//日志管理器实例
		static Scheduler* pScher_;

		//用于保护的锁，为了服务器执行效率，原则上不允许长久占有此锁
		static std::mutex scherMtx_;

		std::vector<Processor*> processors_;

		ProcessorSelector proSelector_;
	};

	inline void Scheduler::createNewCo(std::function<void()>&& func)
	{
		proSelector_.next()->goNewCo(std::move(func));
	}

	inline void Scheduler::createNewCo(std::function<void()>& func)
	{
		proSelector_.next()->goNewCo(func);
	}

	inline void Scheduler::join()
	{
		for (auto pP : processors_)
		{
			pP->join();
		}
	}

	inline Processor* Scheduler::getProcessor(int id)
	{
		return processors_[id];
	}

	inline int Scheduler::getProCnt()
	{
		return static_cast<int>(processors_.size());
	}

}