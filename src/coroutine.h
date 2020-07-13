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

		//�ָ����е�ǰЭ��
		void resume();

		//��ͣ���е�ǰЭ��
		void yield();

		//���и�Э�̵ĺ���
		inline void startFunc() { coFunc_(); };

		//��ȡ��Э�̵�������
		inline Context* getCtx() { return &ctx_; }

	private:

		std::function<void()> coFunc_;

		Processor* pMyProcessor_;

		int status_;

		Context ctx_;

	};

}