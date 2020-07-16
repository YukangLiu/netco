//@author Liu Yukang
#pragma once
#include <functional>
#include "context.h"
#include "utils.h"

namespace netco
{

	enum coStatus
	{
		CO_READY = 0,
		CO_RUNNING,
		CO_WAITING,
		CO_DEAD
	};

	class Processor;

	class Coroutine
	{
	public:
		Coroutine(Processor*, size_t stackSize, std::function<void()>&&);
		Coroutine(Processor*, size_t stackSize, std::function<void()>&);
		~Coroutine();

		DISALLOW_COPY_MOVE_AND_ASSIGN(Coroutine);

		//恢复运行当前协程
		void resume();

		//暂停运行当前协程
		void yield();

		Processor* getMyProcessor(){return pMyProcessor_;}

		//运行该协程的函数
		inline void startFunc() { coFunc_(); };

		//获取该协程的上下文
		inline Context* getCtx() { return &ctx_; }

	private:

		std::function<void()> coFunc_;

		Processor* pMyProcessor_;

		int status_;

		Context ctx_;

	};

}