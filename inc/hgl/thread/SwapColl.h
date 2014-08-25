﻿#ifndef HGL_THREAD_SWAP_LIST_INCLUDE
#define HGL_THREAD_SWAP_LIST_INCLUDE

#include<hgl/thread/ThreadMutex.h>
#include<hgl/thread/Semaphore.h>
#include<hgl/type/List.h>
#include<hgl/type/Set.h>
namespace hgl
{
	template<typename T,typename S> class SwapColl
	{
	protected:

		S join_list;
		S proc_list;

		ThreadMutex lock;

	public:

		virtual ~SwapColl(){}

		bool Add(T &item)
		{
			lock.Lock();
			int result=join_list.Add(item);
			lock.Unlock();

			return(result!=-1);
		}

		int Add(T *item_list,const int count)
		{
			lock.Lock();
			int result=join_list.Add(item_list,count);
			lock.Unlock();

			return result;
		}

		S &GetProcList()
		{
			lock.Lock();
				proc_list.Add(join_list);
				join_list.ClearData();
			lock.Unlock();

			return proc_list;
		}

		S *operator ->(){return &proc_list;}
		operator S &(){return proc_list;}
	};//template<typename T> class SwapColl

	template<typename T> using SwapList=SwapColl<T,List<T>>;		///<安全交换列表
	template<typename T> using SwapSet=SwapColl<T,Set<T>>;			///<安全交换合集

	template<typename T,typename S> class SemSwapColl
	{
	protected:

		S join_list;
		S proc_list;

		ThreadMutex lock;
		Semaphore sem;

	public:

		virtual ~SemSwapColl(){}

		bool Add(T &item)
		{
			lock.Lock();
			int result=join_list.Add(item);
			lock.Unlock();

			if(result!=-1)
				sem.Release(1);

			return(result!=-1);
		}

		int Add(T *item_list,const int count)
		{
			lock.Lock();
			int result=join_list.Add(item_list,count);
			lock.Unlock();

			if(result>0)
				sem.Release(1);

			return result;
		}

		bool WaitProc(const double time_out=HGL_TIME_ONE_MINUTE)
		{
			if(sem.Acquire(time_out))
				return(false);

			lock.Lock();
				proc_list.Add(join_list);
				join_list.ClearData();
			lock.Unlock();

			return(true);
		}

		S *operator ->(){return &proc_list;}
		operator S &(){return proc_list;}
	};//template<typename T> class SemSwapColl

	template<typename T> using SemSwapList=SemSwapColl<T,List<T>>;		///<安全信号交换列表
	template<typename T> using SemSwapSet=SemSwapColl<T,Set<T>>;		///<安全信号交换合集
}//namespace hgl
#endif//HGL_THREAD_SWAP_LIST_INCLUDE
