//@Author Liu Yukang 
#pragma once
#include "mstime.h"
#include "utils.h"

#include <map>
#include <queue>
#include <vector>
#include <mutex>
#include <functional>

namespace netco
{
	class Coroutine;
	class Epoller;

	//��ʱ��
	class Timer
	{
	public:
		using TimerHeap = typename std::priority_queue<std::pair<Time, Coroutine*>, std::vector<std::pair<Time, Coroutine*>>, std::greater<std::pair<Time, Coroutine*>>>;

		Timer();
		~Timer();

		bool init(Epoller*);

		DISALLOW_COPY_MOVE_AND_ASSIGN(Timer);

		//��ȡ�����Ѿ���ʱ����Ҫִ�еĺ���
		void getExpiredCoroutines(std::vector<Coroutine*>& expiredCoroutines);

		//��timeʱ����Ҫ�ָ�Э��co
		void runAt(Time time, Coroutine* pCo);

		//����time����ָ�Э��co
		inline void runAfter(Time time, Coroutine* pCo);

		inline void wakeUp();

	private:
		//��timefd��������ʱ�䣬time�Ǿ���ʱ��
		bool resetTimeOfTimefd(Time time);

		inline bool isTimeFdUseful() { return timeFd_ < 0 ? false : true; };

		int timeFd_;

		//��ʱ��Э�̼���
		//std::multimap<Time, Coroutine*> timerCoMap_;
		TimerHeap timerCoHeap_;
	};

	inline void Timer::runAfter(Time time, Coroutine* pCo)
	{
		Time runTime(Time::now().getTimeVal() + time.getTimeVal());
		runAt(runTime, pCo);
	}

	inline void Timer::wakeUp()
	{
		resetTimeOfTimefd(Time::now());
	}

}
