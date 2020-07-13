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

		//��idx���̴߳�����Э��
		inline void createNewCo(std::function<void()>&& func);
		inline void createNewCo(std::function<void()>& func);

		inline Processor* getProcessor(int);

		inline int getProCnt();

		inline void join();

	private:
		//��ʼ��Scheduler��threadCntΪ���������߳�
		bool startScheduler(int threadCnt);

		//��־������ʵ��
		static Scheduler* pScher_;

		//���ڱ���������Ϊ�˷�����ִ��Ч�ʣ�ԭ���ϲ�������ռ�д���
		static std::mutex scherMtx_;

		std::vector<Processor*> processors_;

		ProcessorSelector proSelector_;
	};

	inline void Scheduler::createNewCo(std::function<void()>&& func)
	{
		proSelector_.next()->goNewCo(std::move(func));
	}

	inline void Scheduler::createNewCo(std::function<void()>& func)
	{
		proSelector_.next()->goNewCo(func);
	}

	inline void Scheduler::join()
	{
		for (auto pP : processors_)
		{
			pP->join();
		}
	}

	inline Processor* Scheduler::getProcessor(int id)
	{
		return processors_[id];
	}

	inline int Scheduler::getProCnt()
	{
		return static_cast<int>(processors_.size());
	}

}