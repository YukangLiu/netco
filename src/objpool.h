//@Author Liu Yukang 
#pragma once
#include <type_traits>
#include "mempool.h"

namespace netco
{

	template<class T>
	class ObjPool
	{
	public:
		ObjPool() {};
		~ObjPool() {};

		DISALLOW_COPY_MOVE_AND_ASSIGN(ObjPool);

		template<typename... Args>
		inline T* new_obj(Args... args);

		inline void delete_obj(void* obj);

	private:
		template<typename... Args>
		inline T* new_aux(std::true_type, Args... args);

		template<typename... Args>
		inline T* new_aux(std::false_type, Args... args);

		inline void delete_aux(std::true_type, void* obj);

		inline void delete_aux(std::false_type, void* obj);

		MemPool<sizeof(T)> _memPool;

	};

	template<class T>
	template<typename... Args>
	inline T* ObjPool<T>::new_obj(Args... args)
	{
		return new_aux(std::integral_constant<bool, std::is_trivially_constructible<T>::value>(), args...);
	}

	template<class T>
	template<typename... Args>
	inline T* ObjPool<T>::new_aux(std::true_type, Args... args)
	{
		return static_cast<T*>(_memPool.AllocAMemBlock());
	}

	template<class T>
	template<typename... Args>
	inline T* ObjPool<T>::new_aux(std::false_type, Args... args)
	{
		void* newPos = _memPool.AllocAMemBlock();
		return new(newPos) T(args...);
	}

	template<class T>
	inline void ObjPool<T>::delete_obj(void* obj)
	{
		if (!obj)
		{
			return;
		}
		delete_aux(std::integral_constant<bool, std::is_trivially_destructible<T>::value>(), obj);
	}

	template<class T>
	inline void ObjPool<T>::delete_aux(std::true_type, void* obj)
	{
		_memPool.FreeAMemBlock(obj);
	}

	template<class T>
	inline void ObjPool<T>::delete_aux(std::false_type, void* obj)
	{
		(static_cast<T*>(obj))->~T();
		_memPool.FreeAMemBlock(obj);
	}

}
