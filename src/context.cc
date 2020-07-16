//@author Liu Yukang
#include "../include/context.h"
#include "../include/parameter.h"
#include <stdlib.h>

using namespace netco;

Context::Context(size_t stackSize)
	:pStack_(nullptr), stackSize_(stackSize)
{ }

Context::~Context()
{
	if (pStack_)
	{
		free(pStack_);
	}
}

void Context::makeContext(void (*func)(), Processor* pP, Context* pLink)
{
	if (nullptr == pStack_)
	{
		pStack_ = malloc(stackSize_);
	}
	::getcontext(&ctx_);
    ctx_.uc_stack.ss_sp = pStack_;
	ctx_.uc_stack.ss_size = parameter::coroutineStackSize;
	ctx_.uc_link = pLink->getUCtx();
	makecontext(&ctx_, func, 1, pP);
}

void Context::makeCurContext()
{
	::getcontext(&ctx_);
}

void Context::swapToMe(Context* pOldCtx)
{
	if (nullptr == pOldCtx)
	{
		setcontext(&ctx_);
	}
	else
	{
		swapcontext(pOldCtx->getUCtx(), &ctx_);
	}
}