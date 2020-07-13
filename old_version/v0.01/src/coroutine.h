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
		Coroutine(Processor*, std::function<void()>&&);
		Coroutine(Processor*, std::function<void()>&);
		~Coroutine();

		DISALLOW_COPY_MOVE_AND_ASSIGN(Coroutine);

		//�ָ����е�ǰЭ��
		void resume();

		//��ͣ���е�ǰЭ��
		inline void yield();

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


	inline void Coroutine::yield()
	{
		status_ = CO_WAITING;
	};

}