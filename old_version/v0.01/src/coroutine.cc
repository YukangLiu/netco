//@author Liu Yukang
#include "coroutine.h"
#include "processor.h"
#include "parameter.h"

using namespace netco;

static void coWrapFunc(Processor* pP)
{
	pP->getCurRunningCo()->startFunc();
	pP->killCurCo();
}

Coroutine::Coroutine(Processor* pMyProcessor, std::function<void()>&& func)
	: coFunc_(std::move(func)), pMyProcessor_(pMyProcessor), status_(CO_DEAD)//, ctx_(stack_, parameter::coroutineStackSize)
{
	status_ = CO_READY;
}

Coroutine::Coroutine(Processor* pMyProcessor, std::function<void()>& func)
	: coFunc_(func), pMyProcessor_(pMyProcessor), status_(CO_DEAD)//, ctx_(stack_, parameter::coroutineStackSize)
{
	status_ = CO_READY;
}

Coroutine::~Coroutine()
{
}

void Coroutine::resume()
{
	Context* pMainCtx = pMyProcessor_->getMainCtx();
	switch (status_)
	{
	case CO_READY:
        status_ = CO_RUNNING;
		ctx_.makeContext((void (*)(void)) coWrapFunc, pMyProcessor_, pMainCtx);
		ctx_.swapToMe(pMainCtx);
		break;

	case CO_WAITING:
		status_ = CO_RUNNING;
		ctx_.swapToMe(pMainCtx);
		break;

	default:

		break;
	}
}

