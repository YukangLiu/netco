//@author Liu Yukang
#pragma once
#include <queue>
#include <set>
#include <mutex>
#include <thread>
#include "objpool.h"
#include "spinlock.h"
#include "context.h"
#include "coroutine.h"
#include "epoller.h"
#include "timer.h"

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

		//����һ����Э�̣���Э�̵ĺ�����func
		void goNewCo(std::function<void()>&& func, size_t stackSize);
		void goNewCo(std::function<void()>& func, size_t stackSize);

		void yield();

		//��ǰЭ�̵ȴ�time����
		void wait(Time time);

		//�����ǰ�������е�Э��
		void killCurCo();

		bool loop();

		void stop();

		void join();

		//�ȴ�fd�ϵ�ev�¼�����
		void waitEvent(int fd, int ev);

		//��ȡ��ǰ�������е�Э��
		inline Coroutine* getCurRunningCo() { return pCurCoroutine_; };

		inline Context* getMainCtx() { return &mainCtx_; }

		inline size_t getCoCnt() { return coSet_.size(); }

	private:

		//�ָ�����ָ��Э��
		void resume(Coroutine*);

		void goNewCo_aux(Coroutine*);

		inline void wakeUpEpoller();

		//�ô��������̺߳�
		int tid_;

		int status_;

		std::thread* pLoop_;

		//��������У�ʹ��˫�������
		std::queue<Coroutine*> newCoroutines_[2];

		//������˫����������������еĶ��кţ���һ�������������
		volatile int runningNewQue_;

		Spinlock newQueLock_;

		Spinlock coPoolLock_;

		std::mutex newCoQueMtx_;

		//EventEpoller���ֵĻ�Ծ�¼����ŵ��б�
		std::vector<Coroutine*> actCoroutines_;

		std::set<Coroutine*> coSet_;

		//��ʱ�������б�
		std::vector<Coroutine*> timerExpiredCo_;

		//���Ƴ���Э���б�Ҫ�Ƴ�ĳһ���¼����ȷ��ڸ��б��У�һ��ѭ�������Ż�����delete
		std::vector<Coroutine*> removedCo_;

		Epoller epoller_;

		Timer timer_;

		ObjPool<Coroutine> coPool_;

		Coroutine* pCurCoroutine_;

		Context mainCtx_;
	};

}

