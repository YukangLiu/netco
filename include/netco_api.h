//@author Liu Yukang
#pragma once
#include "scheduler.h"
#include "mstime.h"

namespace netco 
{
	////////////////////Э�̵�����ؽӿ�///////////////////

	//��������һ��Э�̣�Э�̽�����func����
	//����ָ��Э��ջ��СstackSize��Ĭ��Ϊ2k
	//tid��Э��Ҫ�������ĸ��߳���
	//		-1�� ʹ��netco������ѡ��Э���������ĸ��߳�
	//		other���̺߳�
	//netco����ݴ����������������������߳���������Э�̣�
	//����4������������tidֻ��0 ~ 3����Ч�� 
	void co_go(std::function<void()>& func, size_t stackSize = parameter::coroutineStackSize, int tid = -1);
	void co_go(std::function<void()>&& func, size_t stackSize = parameter::coroutineStackSize, int tid = -1);

	//��ǰЭ�̵ȴ�t������ټ���ִ��
	void co_sleep(Time t);

	//�ȴ��������Ľ���
	void sche_join();

	//////////////�������ϵͳ����ʹ��Socket.hͷ�еķ�װ//////////
}

