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

		//�ú���ָ�����õ�ǰcontext�����������
		void makeContext(void (*func)(), Processor*, Context*);

		//ֱ���õ�ǰ����״̬���õ�ǰcontext��������
		void makeCurContext();

		//����ǰ�����ı��浽oldCtx�У�Ȼ���л�����ǰ�����ģ���oldCtxΪ�գ���ֱ������
		void swapToMe(Context* pOldCtx);

		//��ȡ��ǰ�����ĵ�ucontext_tָ��
		inline struct ucontext_t* getUCtx() { return &ctx_; };

	private:

		struct ucontext_t ctx_;

		void* pStack_;

		size_t stackSize_;

	};

}