//@author Liu Yukang
#pragma once
#include <queue>
#include <set>
#include <mutex>
#include <thread>
#include <atomic>
#include "context.h"
#include "coroutine.h"
#include "epoller.h"
#include "timer.h"
#include "socket.h"

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
		inline void goNewCo(std::function<void()>&& func);
		inline void goNewCo(std::function<void()>& func);

		void yield();

		//��ǰЭ�̵ȴ�time����
		void wait(Time time);

		//�����ǰ�������е�Э��
		inline void killCurCo();

		bool loop();

		inline void stop();

		inline void join();

		ssize_t read(int fd,char* buf,size_t len);

		Socket accept(Socket& listener);

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

		std::atomic_int semaphore_;

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

		Coroutine* pCurCoroutine_;

		Context mainCtx_;
	};

	inline void Processor::stop()
	{
		status_ = PRO_STOPPING;
	}

	inline void Processor::join()
	{
		pLoop_->join();
	}

	inline void Processor::wakeUpEpoller()
	{
		timer_.wakeUp();
	}

	inline void Processor::goNewCo(std::function<void()>&& coFunc)
	{
		Coroutine* pCo = new Coroutine(this, std::move(coFunc));
		goNewCo_aux(pCo);
	}

	inline void Processor::goNewCo(std::function<void()>& coFunc)
	{
		Coroutine* pCo = new Coroutine(this, coFunc);
		goNewCo_aux(pCo);
	}

	inline void Processor::killCurCo()
	{
		removedCo_.push_back(pCurCoroutine_);
	}

}

