//@Author Liu Yukang 
#pragma once
#include "mstime.h"
#include "utils.h"

#include <map>
#include <queue>
#include <vector>
#include <mutex>
#include <functional>

#define TIMER_DUMMYBUF_SIZE 1024

namespace netco
{
	class Coroutine;
	class Epoller;

	//定时器
	class Timer
	{
	public:
		using TimerHeap = typename std::priority_queue<std::pair<Time, Coroutine*>, std::vector<std::pair<Time, Coroutine*>>, std::greater<std::pair<Time, Coroutine*>>>;

		Timer();
		~Timer();

		bool init(Epoller*);

		DISALLOW_COPY_MOVE_AND_ASSIGN(Timer);

		//获取所有已经超时的需要执行的函数
		void getExpiredCoroutines(std::vector<Coroutine*>& expiredCoroutines);

		//在time时刻需要恢复协程co
		void runAt(Time time, Coroutine* pCo);

		//经过time毫秒恢复协程co
		void runAfter(Time time, Coroutine* pCo);

		void wakeUp();

	private:
		//给timefd重新设置时间，time是绝对时间
		bool resetTimeOfTimefd(Time time);

		inline bool isTimeFdUseful() { return timeFd_ < 0 ? false : true; };

		int timeFd_;

		//用于read timefd上数据的
		char dummyBuf_[TIMER_DUMMYBUF_SIZE];

		//定时器协程集合
		//std::multimap<Time, Coroutine*> timerCoMap_;
		TimerHeap timerCoHeap_;
	};

}
