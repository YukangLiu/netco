//@author Liu Yukang
#pragma once
#include "scheduler.h"
#include "mstime.h"

namespace netco 
{
	////////////////////Э�̵�����ؽӿ�///////////////////

	//��������һ��Э�̣�Э�̽�����func����
	//tid��Э��Ҫ�������ĸ��߳���
	//		-1�� ʹ��netco������ѡ��Э���������ĸ��߳�
	//		other���̺߳�
	//netco����ݴ����������������������߳���������Э�̣�
	//����4������������tidֻ��0 ~ 3����Ч�� 
	void co_go(std::function<void()>& func, int tid = -1);
	void co_go(std::function<void()>&& func, int tid = -1);

	//��ǰЭ�̵ȴ�t������ټ���ִ��
	void co_wait(Time t);

	//�ȴ��������Ľ���
	void sche_join();

	//////////////�������ϵͳ���ã�δ������hook//////////
	ssize_t co_read(int fd, char* buf, size_t len);

	Socket co_accept(Socket listener);
}


