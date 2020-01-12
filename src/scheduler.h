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
		void createNewCo(std::function<void()>&& func, size_t stackSize);
		void createNewCo(std::function<void()>& func, size_t stackSize);

		Processor* getProcessor(int);

		int getProCnt();

		void join();

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

}