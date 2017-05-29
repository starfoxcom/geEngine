/*****************************************************************************/
/**
 * @file    geThreadDefines.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/09
 * @brief   Defines for mutex and thread utilities
 *
 * Here are the defines of the mutex and threads for the engine
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

#define GE_AUTO_MUTEX_NAME mutex      /** Define the auto mutex name */

 /****************************************************************************/
 /**
  * Includes
  */
 /****************************************************************************/
# include <thread>
# include <chrono>
# include <mutex>
# include <condition_variable>
# include "geSpinLock.h"

/** Returns the number of logical CPU cores. */
# define GE_THREAD_HARDWARE_CONCURRENCY std::thread::hardware_concurrency()

/** Returns the ThreadId of the current thread. */
# define GE_THREAD_CURRENT_ID std::this_thread::get_id()

/** Causes the current thread to sleep for the provided amount of milliseconds. */
# define GE_THREAD_SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms));

/** Wrapper for the C++ std::mutex. */
using Mutex = std::mutex;

/** Wrapper for the C++ std::recursive_mutex. */
using RecursiveMutex = std::recursive_mutex;

/** Wrapper for the C++ std::condition_variable. */
using Signal = std::condition_variable;

/** Wrapper for the C++ std::thread. */
using Thread = std::thread;

/** Wrapper for the C++ std::thread::id. */
using ThreadId = std::thread::id;

/** Wrapper for the C++ std::unique_lock<std::mutex>. */
using Lock = std::unique_lock<Mutex>;

/** Wrapper for the C++ std::unique_lock<std::recursive_mutex>. */
using RecursiveLock = std::unique_lock<RecursiveMutex>;
