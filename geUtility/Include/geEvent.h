/********************************************************************/
/**
 * @file   geEvent.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2015/02/22
 * @brief  Templates and Classes for the creating on Event objects
 *
 * Thread safe Event object with callbacks for disconnection
 *
 * @bug	   No known bugs.
 */
 /********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Includes                                                                     										*/
/************************************************************************************************************************/
#include "gePrerequisitesUtil.h"

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Data common to all event connections.
	*/
	/************************************************************************************************************************/
	class BaseConnectionData
	{
	public:
		BaseConnectionData* m_Prev;
		BaseConnectionData* m_Next;
		bool m_IsActive;
		uint32 m_HandleLinks;

	public:
		BaseConnectionData() : m_Prev(nullptr), m_Next(nullptr), m_IsActive(true), m_HandleLinks(0)
		{

		}

		virtual ~BaseConnectionData()
		{
			GE_ASSERT(!m_HandleLinks && !m_IsActive);
		}

		virtual void Deactivate()
		{
			m_IsActive = false;
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Internal data for an Event, storing all connections.
	*/
	/************************************************************************************************************************/
	struct EventInternalData
	{
		BaseConnectionData* m_Connections;
		BaseConnectionData* m_FreeConnections;
		GE_RECURSIVE_MUTEX(m_Mutex);

		EventInternalData() : m_Connections(nullptr), m_FreeConnections(nullptr)
		{
		}

		~EventInternalData()
		{
			BaseConnectionData* conn = m_Connections;
			while( conn != nullptr )
			{
				BaseConnectionData* next = conn->m_Next;
				ge_free(conn);
				conn = next;
			}

			conn = m_FreeConnections;
			while( conn != nullptr )
			{
				BaseConnectionData* next = conn->m_Next;
				ge_free(conn);
				conn = next;
			}
		}

		/************************************************************************************************************************/
		/**
		* @brief	Disconnects the connection with the specified data, ensuring the event doesn't call its callback again.
		* @note		Only call this once.
		*/
		/************************************************************************************************************************/
		void Disconnect(BaseConnectionData* conn)
		{
			GE_LOCK_RECURSIVE_MUTEX(m_Mutex);

			conn->Deactivate();
			conn->m_HandleLinks--;

			if( conn->m_HandleLinks == 0 )
			{
				free(conn);
			}
		}

		/************************************************************************************************************************/
		/**
		* @brief	Disconnects all connections in the event.
		*/
		/************************************************************************************************************************/
		void Clear()
		{
			GE_LOCK_RECURSIVE_MUTEX(m_Mutex);

			BaseConnectionData* conn = m_Connections;
			while( conn != nullptr )
			{
				BaseConnectionData* next = conn->m_Next;
				conn->Deactivate();

				if( conn->m_HandleLinks == 0 )
				{
					free(conn);
				}

				conn = next;
			}
		}

		/************************************************************************************************************************/
		/**
		* @brief	Called when the event handle no longer keeps a reference to the connection data. This means we might be able
		*			to free (and reuse) its memory if the event is done with it too.
		*/
		/************************************************************************************************************************/
		void FreeHandle(BaseConnectionData* conn)
		{
			GE_LOCK_RECURSIVE_MUTEX(m_Mutex);

			conn->m_HandleLinks--;

			if(conn->m_HandleLinks == 0 && !conn->m_IsActive)
			{
				free(conn);
			}
		}

		/************************************************************************************************************************/
		/**
		* @brief	Releases connection data and makes it available for re-use when next connection is formed.
		*/
		/************************************************************************************************************************/
		void Free(BaseConnectionData* conn)
		{
			if( conn->m_Prev != nullptr )
			{
				conn->m_Prev->m_Next = conn->m_Next;
			}
			else
			{
				m_Connections = conn->m_Next;
			}

			if( conn->m_Next != nullptr )
			{
				conn->m_Next->m_Prev = conn->m_Prev;
			}

			conn->m_Prev = nullptr;
			conn->m_Next = nullptr;

			if( m_FreeConnections != nullptr )
			{
				conn->m_Next = m_FreeConnections;
				m_FreeConnections->m_Prev = conn;
			}

			m_FreeConnections = conn;
			m_FreeConnections->~BaseConnectionData();
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Event handle. Allows you to track to which events you subscribed to and disconnect from them when needed.
	*/
	/************************************************************************************************************************/
	class HEvent
	{
	private:
		BaseConnectionData* m_Connection;
		SPtr<EventInternalData> m_EventData;

	public:
		HEvent() : m_Connection(nullptr)
		{
		}

		explicit HEvent(const SPtr<EventInternalData>& eventData, BaseConnectionData* connection)
			: m_Connection(connection), m_EventData(eventData)
		{
			connection->m_HandleLinks++;
		}

		~HEvent()
		{
			if( m_Connection != nullptr )
			{
				m_EventData->FreeHandle(m_Connection);
			}
		}

		/************************************************************************************************************************/
		/**
		* @brief	Disconnect from the event you are subscribed to.
		*/
		/************************************************************************************************************************/
		void Disconnect()
		{
			if( m_Connection != nullptr )
			{
				m_EventData->Disconnect(m_Connection);
				m_Connection = nullptr;
				m_EventData = nullptr;
			}
		}

		struct Bool_struct
		{
			int _Member;
		};

		/************************************************************************************************************************/
		/**
		* @brief	Allows direct conversion of a handle to bool.
		* @note		Additional struct is needed because we can't directly convert to bool since then we can assign pointer
		*			to bool and that's wrong.
		*/
		/************************************************************************************************************************/
		operator int Bool_struct::*() const
		{
			return (m_Connection != nullptr ? &Bool_struct::_Member : 0);
		}

		HEvent& operator=(const HEvent& rhs)
		{
			m_Connection = rhs.m_Connection;
			m_EventData = rhs.m_EventData;

			if( m_Connection != nullptr )
			{
				m_Connection->m_HandleLinks++;
			}

			return *this;
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Events allows you to register method callbacks that get notified when the event is triggered.
	*
	* @note		Callback method return value is ignored.
	*/
	/************************************************************************************************************************/
	template <class RetType, class... Args>
	class TEvent
	{
	private:
		SPtr<EventInternalData> m_InternalData;

	private:
		struct ConnectionData : BaseConnectionData
		{
		public:
			void Deactivate() override
			{
				m_func = nullptr;
				BaseConnectionData::Deactivate();
			}

			std::function<RetType(Args...)> m_func;
		};

	public:
		TEvent() : m_InternalData( ge_shared_ptr_new<EventInternalData>() )
		{
		
		}

		~TEvent()
		{
			Clear();
		}

		/************************************************************************************************************************/
		/**
		* @brief	Register a new callback that will get notified once the event is triggered.
		*/
		/************************************************************************************************************************/
		HEvent Connect(std::function<RetType(Args...)> func)
		{
			GE_LOCK_RECURSIVE_MUTEX(m_InternalData->m_Mutex);

			ConnectionData* connData = nullptr;
			if(m_InternalData->m_FreeConnections != nullptr )
			{
				connData = static_cast<ConnectionData*>(m_InternalData->m_FreeConnections);
				m_InternalData->m_FreeConnections = connData->m_Next;

				new (connData)ConnectionData();
				if( connData->m_Next != nullptr )
				{
					connData->m_Next->m_Prev = nullptr;
				}

				connData->m_IsActive = true;
			}

			if( connData == nullptr )
			{
				connData = ge_new<ConnectionData>();
			}

			connData->m_Next = m_InternalData->m_Connections;

			if( m_InternalData->m_Connections != nullptr )
			{
				m_InternalData->m_Connections->m_Prev = connData;
			}

			m_InternalData->m_Connections = connData;
			connData->m_func = func;

			return HEvent(m_InternalData, connData);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Trigger the event, notifying all register callback methods.
		*/
		/************************************************************************************************************************/
		void operator() (Args... args)
		{
			//Increase ref count to ensure this event data isn't destroyed if one of the callbacks deletes the event itself.
			SPtr<EventInternalData> internalData = m_InternalData;
			
			GE_LOCK_RECURSIVE_MUTEX(internalData->m_Mutex);

			//Hidden dependency: If any new connections are made during these callbacks they must be inserted at the start of
			//the linked list so that we don't trigger them here.
			ConnectionData* conn = static_cast<ConnectionData*>(internalData->m_Connections);
			while( conn != nullptr )
			{
				//Save next here in case the callback itself disconnects this connection
				ConnectionData* next = static_cast<ConnectionData*>(conn->m_Next);

				if( conn->m_func != nullptr )
				{
					conn->m_func(std::forward<Args>(args)...);
				}

				conn = next;
			}
		}

		/************************************************************************************************************************/
		/**
		* @brief	Clear all callbacks from the event.
		*/
		/************************************************************************************************************************/
		void Clear()
		{
			m_InternalData->Clear();
		}

		/************************************************************************************************************************/
		/**
		* @brief	Check if event has any callbacks registered.
		*
		* @note		It is safe to trigger an event even if no callbacks are registered.
		*/
		/************************************************************************************************************************/
		bool Empty()
		{
			GE_LOCK_RECURSIVE_MUTEX(m_InternalData->m_Mutex);
			return m_InternalData->m_Connections == nullptr;
		}
	};

	/************************************************************************************************************************/
	/* 													SPECIALIZATIONS                      								*/
	/* 							SO YOU MAY USE FUNCTION LIKE SYNTAX FOR DECLARING EVENT SIGNATURE							*/
	/************************************************************************************************************************/

	/************************************************************************************************************************/
	/**
	* @copydoc	TEvent
	*/
	/************************************************************************************************************************/
	template <typename Signature>
	class Event;

	/************************************************************************************************************************/
	/**
	* @copydoc	TEvent
	*/
	/************************************************************************************************************************/
	template <class RetType, class... Args>
	class Event<RetType(Args...) > : public TEvent<RetType, Args...>
	{
	};
}
