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
		void createNewCo(std::function<void()>&& func, size_t stackSize);
		void createNewCo(std::function<void()>& func, size_t stackSize);

		Processor* getProcessor(int);

		int getProCnt();

		void join();

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

}