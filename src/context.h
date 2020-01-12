//@author Liu Yukang
#pragma once
#include "utils.h"
#include "parameter.h"
#include <ucontext.h>

namespace netco
{

    class Processor;
	class Context
	{
	public:
		Context(size_t stackSize);
		~Context();

		Context(const Context& otherCtx) 
			: ctx_(otherCtx.ctx_), pStack_(otherCtx.pStack_)
		{ }

		Context(Context&& otherCtx)
			: ctx_(otherCtx.ctx_), pStack_(otherCtx.pStack_)
		{ }

		Context& operator=(const Context& otherCtx) = delete;

		//用函数指针设置当前context的上下文入口
		void makeContext(void (*func)(), Processor*, Context*);

		//直接用当前程序状态设置当前context的上下文
		void makeCurContext();

		//将当前上下文保存到oldCtx中，然后切换到当前上下文，若oldCtx为空，则直接运行
		void swapToMe(Context* pOldCtx);

		//获取当前上下文的ucontext_t指针
		inline struct ucontext_t* getUCtx() { return &ctx_; };

	private:

		struct ucontext_t ctx_;

		void* pStack_;

		size_t stackSize_;

	};

}