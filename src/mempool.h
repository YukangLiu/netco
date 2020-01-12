//@Author Liu Yukang 
#pragma once
#include "parameter.h"
#include "utils.h"

namespace netco
{
	struct MemBlockNode
	{
		union
		{
			MemBlockNode* next;
			char data;
		};
	};

	//ÿ�ο��Դ��ڴ���л�ȡobjSize��С���ڴ��
	template<size_t objSize>
	class MemPool
	{
	public:
		MemPool()
			:freeListHead_(nullptr), mallocListHead_(nullptr), mallocTimes_(0)
		{ };

		~MemPool();

		DISALLOW_COPY_MOVE_AND_ASSIGN(MemPool);

		void* allocAMemBlock();
		void freeAMemBlock(void* block);

	private:
		//��������
		MemBlockNode* freeListHead_;
		//malloc�Ĵ��ڴ������
		MemBlockNode* mallocListHead_;
		//ʵ��malloc�Ĵ���
		size_t mallocTimes_;
	};

	template<size_t objSize>
	MemPool<objSize>::~MemPool()
	{
		while (mallocListHead_)
		{
			MemBlockNode* mallocNode = mallocListHead_;
			mallocListHead_ = mallocNode->next;
			free(static_cast<void*>(mallocNode));
		}
	}

	template<size_t objSize>
	void* MemPool<objSize>::allocAMemBlock()
	{
		void* ret;
		if (nullptr == freeListHead_)
		{
			size_t mallocCnt = parameter::memPoolMallocObjCnt + mallocTimes_;
			void* newMallocBlk = malloc(mallocCnt * objSize + sizeof(MemBlockNode));
			MemBlockNode* mallocNode = static_cast<MemBlockNode*>(newMallocBlk);
			mallocNode->next = mallocListHead_;
			mallocListHead_ = mallocNode;
			newMallocBlk = static_cast<char*>(newMallocBlk) + sizeof(MemBlockNode);
			for (size_t i = 0; i < mallocCnt; ++i)
			{
				MemBlockNode* newNode = static_cast<MemBlockNode*>(newMallocBlk);
				newNode->next = freeListHead_;
				freeListHead_ = newNode;
				newMallocBlk = static_cast<char*>(newMallocBlk) + objSize;
			}
			++mallocTimes_;
		}
		ret = &(freeListHead_->data);
		freeListHead_ = freeListHead_->next;
		return ret;
	}

	template<size_t objSize>
	void MemPool<objSize>::freeAMemBlock(void* block)
	{
		if (nullptr == block)
		{
			return;
		}
		MemBlockNode* newNode = static_cast<MemBlockNode*>(block);
		newNode->next = freeListHead_;
		freeListHead_ = newNode;
	}
}
