/********************************************************************/
/**
 * @file   geLog.h
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
#pragma once

#include "gePrerequisitesUtil.h"

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	A single log entry, containing a message and a channel the message was recorded on.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT LogEntry
	{
	public:
		LogEntry(){}
		LogEntry(const String& msg, uint32 channel);

		/************************************************************************************************************************/
		/**
		* @brief	Channel the message was recorded on.
		*/
		/************************************************************************************************************************/
		uint32 GetLogChannel() const { return m_Channel; }

		/************************************************************************************************************************/
		/**
		* @brief	Text of the message.
		*/
		/************************************************************************************************************************/
		const String& GetLogMessage() const { return m_Msg; }

	private:
		String m_Msg;
		uint32 m_Channel;
	};

	/************************************************************************************************************************/
	/**
	* @brief	Used for logging messages. Can categorize messages according to channels, save the log to a file
	*			and send out callbacks when a new message is added.
	* @note		Thread safe.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT Log
	{
	public:
		Log();
		~Log();

		/************************************************************************************************************************/
		/**
		* @brief	Logs a new message.
		* @param[in]	message	The message describing the log entry.
		* @param[in]	channel Channel in which to store the log entry.
		*/
		/************************************************************************************************************************/
		void LogMsg(const String& message, uint32 channel);

		/************************************************************************************************************************/
		/**
		* @brief	Removes all log entries.
		*/
		/************************************************************************************************************************/
		void Clear();

		/************************************************************************************************************************/
		/**
		* @brief	Removes all log entries in a specific channel.
		*/
		/************************************************************************************************************************/
		void Clear(uint32 channel);

		/************************************************************************************************************************/
		/**
		* @brief	Returns all existing log entries.
		*/
		/************************************************************************************************************************/
		Vector<LogEntry> GetEntries() const;

		/************************************************************************************************************************/
		/**
		* @brief	Returns the latest unread entry from the log queue, and removes the entry from the unread entries list.
		* @param[out]	entry	Entry that was retrieved, or undefined if no entries exist.
		* @return				True if an unread entry was retrieved, false otherwise.
		*/
		/************************************************************************************************************************/
		bool GetUnreadEntry(LogEntry& entry);

		/************************************************************************************************************************/
		/**
		* @brief	Returns the last available log entry.
		* @param[out]	entry	Entry that was retrieved, or undefined if no entries exist.
		* @return				True if an entry was retrieved, false otherwise.
		*/
		/************************************************************************************************************************/
		bool GetLastEntry(LogEntry& entry);

		/************************************************************************************************************************/
		/**
		* @brief	Returns a hash value that is modified whenever entries in the log change. This can be used for checking for
		*			changes by external systems.
		*/
		/************************************************************************************************************************/
		uint64 GetHash() const { return m_Hash; }

	private:
		friend class Debug;

		/************************************************************************************************************************/
		/**
		* @brief	Returns all log entries, including those marked as unread.
		*/
		/************************************************************************************************************************/
		Vector<LogEntry> GetAllEntries() const;

		Vector<LogEntry> m_Entries;
		Queue<LogEntry> m_UnreadEntries;
		uint64 m_Hash;
		GE_RECURSIVE_MUTEX(m_Mutex);
	};
}
