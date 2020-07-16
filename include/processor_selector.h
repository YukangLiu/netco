//@Author Liu Yukang 
#pragma once
#include <vector>

namespace netco
{
	class Processor;

	enum scheduleStrategy
	{
		MIN_EVENT_FIRST = 0 , //最少事件优先
		ROUND_ROBIN			  //轮流分发
	};

	//事件管理器选择器，决定下一个事件应该放入哪个事件管理器中
	class ProcessorSelector
	{
	public:
		ProcessorSelector(std::vector<Processor*>& processors, int strategy = MIN_EVENT_FIRST) :  curPro_(-1) , strategy_(strategy) , processors_(processors) {}
		~ProcessorSelector() {}

		//设置分发任务的策略
		//MIN_EVENT_FIRST则每次挑选EventService最少的EventManager接收新连接
		//ROUND_ROBIN则每次轮流挑选EventManager接收新连接
		inline void setStrategy(int strategy) { strategy_ = strategy; };

		Processor* next();

	private:
		int curPro_;

		int strategy_;

		std::vector<Processor*>& processors_;

	};

}