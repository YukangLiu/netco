//@author Liu Yukang
#pragma once
#include <stddef.h>

namespace netco
{
	namespace parameter
	{
		//Э��ջ��С
		const static size_t coroutineStackSize = 32 * 1024;

		//��ȡ��Ծ��epoll_event������ĳ�ʼ����
		static constexpr int epollerListFirstSize = 16;

		//epoll_wait������ʱ��
		static constexpr int epollTimeOutMs = 10000;

		//�������еĳ���
		constexpr static unsigned backLog = 1024;
	}
	
}