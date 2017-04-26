/********************************************************************/
/**
 * @file   geLog.cpp
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/09/19
 * @brief  Used for logging messages
 *
 * Can categorize messages according to channels, save the log to a
 * file and send out callbacks when a new message is added
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#include "geLog.h"
#include "geException.h"

namespace geEngineSDK
{
	LogEntry::LogEntry(const String& msg, uint32 channel) : m_Msg(msg), m_Channel(channel)
	{
	}

	Log::Log() : m_Hash(0)
	{
	}

	Log::~Log()
	{
		Clear();
	}

	void Log::LogMsg(const String& message, uint32 channel)
	{
		GE_LOCK_RECURSIVE_MUTEX(m_Mutex);
		m_UnreadEntries.push(LogEntry(message, channel));
	}

	void Log::Clear()
	{
		GE_LOCK_RECURSIVE_MUTEX(m_Mutex);
		m_Entries.clear();

		while( !m_UnreadEntries.empty() )
		{
			m_UnreadEntries.pop();
		}

		m_Hash++;
	}

	void Log::Clear(uint32 channel)
	{
		GE_LOCK_RECURSIVE_MUTEX(m_Mutex);
		
		Vector<LogEntry> newEntries;
		for(auto& entry : m_Entries)
		{
			if( entry.GetLogChannel() == channel )
			{
				continue;
			}

			newEntries.push_back(entry);
		}

		m_Entries = newEntries;

		Queue<LogEntry> newUnreadEntries;
		while( !m_UnreadEntries.empty() )
		{
			LogEntry entry = m_UnreadEntries.front();
			m_UnreadEntries.pop();

			if( entry.GetLogChannel() == channel )
			{
				continue;
			}

			newUnreadEntries.push(entry);
		}

		m_UnreadEntries = newUnreadEntries;
		m_Hash++;
	}

	bool Log::GetUnreadEntry(LogEntry& entry)
	{
		GE_LOCK_RECURSIVE_MUTEX(m_Mutex);

		if( m_UnreadEntries.empty() )
		{
			return false;
		}

		entry = m_UnreadEntries.front();
		m_UnreadEntries.pop();
		m_Entries.push_back(entry);
		m_Hash++;

		return true;
	}

	bool Log::GetLastEntry(LogEntry& entry)
	{
		if( m_Entries.size() == 0 )
		{
			return false;
		}

		entry = m_Entries.back();
		
		return true;
	}

	Vector<LogEntry> Log::GetEntries() const
	{
		GE_LOCK_RECURSIVE_MUTEX(m_Mutex);
		return m_Entries;
	}

	Vector<LogEntry> Log::GetAllEntries() const
	{
		Vector<LogEntry> entries;
		{
			GE_LOCK_RECURSIVE_MUTEX(m_Mutex);

			for(auto& entry : m_Entries)
			{
				entries.push_back(entry);
			}

			Queue<LogEntry> unreadEntries = m_UnreadEntries;
			while( !unreadEntries.empty() )
			{
				entries.push_back(unreadEntries.front());
				unreadEntries.pop();
			}
		}
		return entries;
	}
}
