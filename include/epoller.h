//@author Liu Yukang
#pragma once
#include "utils.h"

#include <vector>

struct epoll_event;

namespace netco
{
	class Coroutine;

	class Epoller
	{
	public:
		Epoller();
		~Epoller();

		DISALLOW_COPY_MOVE_AND_ASSIGN(Epoller);

		//Ҫʹ��EventEpoller������øú�����ʼ����ʧ���򷵻�false
		bool init();

		//�޸�Epoller�е��¼�
		bool modifyEv(Coroutine* pCo, int fd, int interesEv);

		//��Epoller������¼�
		bool addEv(Coroutine* pCo, int fd, int interesEv);

		//��Epoller���Ƴ��¼�
		bool removeEv(Coroutine* pCo, int fd, int interesEv);

		//��ȡ��������¼�����,����errno
		int getActEvServ(int timeOutMs, std::vector<Coroutine*>& activeEvServs);

	private:

		inline bool isEpollFdUseful() { return epollFd_ < 0 ? false : true; };

		int epollFd_;

		std::vector<struct epoll_event> activeEpollEvents_;

	};

}
