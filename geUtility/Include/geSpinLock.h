/********************************************************************/
/**
 * @file   geSpinLock.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2015/02/09
 * @brief  Synchronization primitive with low overhead.
 *
 * However it will actively block the thread waiting for the lock
 * not allowing any other work to be done, so it is best used for
 * short locks.
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Includes                                                                     										*/
/************************************************************************************************************************/
#include <atomic>

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Synchronization primitive with low overhead.
	*
	* @note		However it will actively block the thread waiting for the lock, not allowing any other work to be done, 
	*			so it is best used for short locks.
	*/
	/************************************************************************************************************************/
	class SpinLock
	{
		/************************************************************************************************************************/
		/* Variables declaration                                                                     							*/
		/************************************************************************************************************************/
	private:
		std::atomic_flag m_Lock;	/*!< Lock flags */

		/************************************************************************************************************************/
		/* Functions declaration                                                                     							*/
		/************************************************************************************************************************/
	public:
		/************************************************************************************************************************/
		/**
		* @brief	Basic constructor
		*/
		/************************************************************************************************************************/
		SpinLock()
		{
			m_Lock.clear(std::memory_order_relaxed);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Lock any following operations with the spin lock, not allowing any other thread to access them.
		*/
		/************************************************************************************************************************/
		void Lock()
		{
			while (m_Lock.test_and_set(std::memory_order_acquire))
			{
			}
		}

		/************************************************************************************************************************/
		/**
		* @brief	Release the lock and allow other threads to acquire the lock.
		*/
		/************************************************************************************************************************/
		void Unlock()
		{
			m_Lock.clear(std::memory_order_release);
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Provides a safer method for locking a spin lock as the lock will get automatically locked when this objected
	*			is created and unlocked as soon as it goes out of scope.
	*/
	/************************************************************************************************************************/
	class ScopedSpinLock
	{
	private:
		SpinLock& m_SpinLock;

	public:
		ScopedSpinLock(SpinLock& spinLock) : m_SpinLock(spinLock)
		{
			m_SpinLock.Lock();
		}

		~ScopedSpinLock()
		{
			m_SpinLock.Unlock();
		}
	};
}
