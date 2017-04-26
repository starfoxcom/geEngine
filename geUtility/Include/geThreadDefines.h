/********************************************************************/
/**
 * @file   geThreadDefines.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2015/02/09
 * @brief  Defines for mutex and thread utilities
 *
 * Here are the defines of the mutex and threads for the engine
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Define the auto mutex name                                                                     						*/
/************************************************************************************************************************/
#define GE_AUTO_MUTEX_NAME mutex

/************************************************************************************************************************/
/* if GE_THREAD_SUPPORT set the defines to a valid configuration for the use of threads									*/
/************************************************************************************************************************/
#if GE_THREAD_SUPPORT

	/************************************************************************************************************************/
	/* Include thread files                                                                     							*/
	/************************************************************************************************************************/
	#include <thread>
	#include <chrono>
	#include <mutex>
	#include <condition_variable>
	#include "geSpinLock.h"

	#define GE_AUTO_MUTEX														mutable std::mutex GE_AUTO_MUTEX_NAME;
	#define GE_LOCK_AUTO_MUTEX													std::unique_lock<std::mutex> geAutoMutexLock(GE_AUTO_MUTEX_NAME);
	#define GE_MUTEX(name)														mutable std::mutex name;
	#define GE_STATIC_MUTEX(name)												static std::mutex name;
	#define GE_STATIC_MUTEX_INSTANCE(name)										std::mutex name;
	#define GE_STATIC_MUTEX_CLASS_INSTANCE(name, classTypeName)					std::mutex classTypeName##::name;
	#define GE_LOCK_MUTEX(name)													std::unique_lock<std::mutex> genameLock(name);
	#define GE_LOCK_MUTEX_NAMED(mutexName, lockName)							std::unique_lock<std::mutex> lockName(mutexName);
	#define GE_LOCK_TYPE														std::unique_lock<std::mutex>

	/************************************************************************************************************************/
	/* Like GE_AUTO_MUTEX but mutex held by pointer																			*/
	/************************************************************************************************************************/
	#define GE_AUTO_SHARED_MUTEX												mutable std::mutex *GE_AUTO_MUTEX_NAME;
	#define GE_LOCK_AUTO_SHARED_MUTEX											GE_ASSERT(GE_AUTO_MUTEX_NAME); std::lock_guard<std::mutex> geAutoMutexLock(*GE_AUTO_MUTEX_NAME);
	#define GE_COPY_AUTO_SHARED_MUTEX(from)										GE_ASSERT(!GE_AUTO_MUTEX_NAME); GE_AUTO_MUTEX_NAME = from;
	#define GE_SET_AUTO_SHARED_MUTEX_NULL										GE_AUTO_MUTEX_NAME = 0;
	#define GE_MUTEX_CONDITIONAL(mutex)											if (mutex)
	#define GE_THREAD_SYNCHRONISER(sync)										std::condition_variable sync;
	#define GE_STATIC_THREAD_SYNCHRONISER(sync)									static std::condition_variable sync;
	#define GE_STATIC_THREAD_SYNCHRONISER_CLASS_INSTANCE(sync, classTypeName)	std::condition_variable classTypeName##::sync;
	#define GE_THREAD_WAIT(sync, mutex, lock)									sync.wait(lock);
	#define GE_THREAD_WAIT_FOR(sync, mutex, lock, ms)							sync.wait_for(lock, std::chrono::milliseconds(ms));
	#define GE_THREAD_NOTIFY_ONE(sync)											sync.notify_one(); 
	#define GE_THREAD_NOTIFY_ALL(sync)											sync.notify_all(); 
	#define GE_THREAD_JOIN(thread)												thread.join();

	/************************************************************************************************************************/
	/* Recursive mutex                                                                     									*/
	/************************************************************************************************************************/
	#define GE_RECURSIVE_MUTEX(name)											mutable std::recursive_mutex name
	#define GE_LOCK_RECURSIVE_MUTEX(name)										std::unique_lock<std::recursive_mutex> cmnameLock(name);
	
	/************************************************************************************************************************/
	/* Read-write mutex                                                                     								*/
	/************************************************************************************************************************/
	#define GE_RW_MUTEX(name)													mutable std::mutex name
	#define GE_LOCK_RW_MUTEX_READ(name)											std::unique_lock<std::mutex> cmnameLock(name)
	#define GE_LOCK_RW_MUTEX_WRITE(name)										std::unique_lock<std::mutex> cmnameLock(name)
	
	/************************************************************************************************************************/
	/* Thread objects and related functions                                                                     			*/
	/************************************************************************************************************************/
	#define GE_THREAD_TYPE														std::thread
	#define GE_THREAD_CREATE(name, worker)										std::thread* name = new (geEngineSDK::MemoryAllocator<geEngineSDK::GenAlloc>::allocate(sizeof(std::thread))) std::thread(worker);
	#define GE_THREAD_DESTROY(name)												geEngineSDK::ge_delete<geEngineSDK::GenAlloc, std::thread>(name);
	#define GE_THREAD_HARDWARE_CONCURRENCY										std::thread::hardware_concurrency()
	#define GE_THREAD_CURRENT_ID												std::this_thread::get_id()
	#define GE_THREAD_ID_TYPE													std::thread::id
	#define GE_DEFER_LOCK														std::defer_lock
	#define GE_THREAD_WORKER_INHERIT
	
	/************************************************************************************************************************/
	/* Utility                                                                     											*/
	/************************************************************************************************************************/
	#define GE_THREAD_SLEEP(ms)													std::this_thread::sleep_for(std::chrono::milliseconds(ms));

	using Mutex				= std::mutex;
	using RecursiveMutex	= std::recursive_mutex;
	using Signal			= std::condition_variable;
	using Thread			= std::thread;

	template <typename T = Mutex>
	using Lock				= std::unique_lock<T>;

	template <typename T = RecursiveMutex>
	using RecursiveLock		= std::unique_lock<T>;
#endif

