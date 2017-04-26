/********************************************************************/
/**
 * @file   geDebug.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/09/19
 * @brief  Utility class providing various debug functionality.
 *
 * Utility class providing various debug functionality.
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#pragma once

#include "gePrerequisitesUtil.h"
#include "geLog.h"

namespace geEngineSDK
{
	class Log;

	/************************************************************************************************************************/
	/**
	* @brief	Available types of channels that debug messages can be logged to.
	*/
	/************************************************************************************************************************/
	namespace DEBUG_CHANNEL
	{
		enum E
		{
			kDebug,
			kWarning,
			kError,
			kCompilerWarning,
			kCompilerError
		};
	}

	/************************************************************************************************************************/
	/**
	* @brief	Utility class providing various debug functionality.
	* @note		Thread safe.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT Debug
	{
	public:
		/************************************************************************************************************************/
		/**
		* @brief	Adds a log entry in the "Debug" channel.
		*/
		/************************************************************************************************************************/
		void LogDebug(const String& msg);

		/************************************************************************************************************************/
		/**
		* @brief	Adds a log entry in the "Warning" channel.
		*/
		/************************************************************************************************************************/
		void LogWarning(const String& msg);

		/************************************************************************************************************************/
		/**
		* @brief	Adds a log entry in the "Error" channel.
		*/
		/************************************************************************************************************************/
		void LogError(const String& msg);

		/************************************************************************************************************************/
		/**
		* @brief	Adds a log entry in the specified channel. You may specify custom channels as needed.
		*/
		/************************************************************************************************************************/
		void LogMessage(const String& msg, uint32 channel);

		/************************************************************************************************************************/
		/**
		* @brief	Retrieves the Log used by the Debug instance.
		*/
		/************************************************************************************************************************/
		Log& GetLog() { return m_Log; }

		/************************************************************************************************************************/
		/**
		* @brief	Converts raw pixels into a BMP image. See BitmapWriter for more information.
		*/
		/************************************************************************************************************************/
		void WriteAsBMP(uint8* rawPixels, uint32 bytesPerPixel, uint32 width, uint32 height, const Path& filePath, bool overwrite = true) const;

		/************************************************************************************************************************/
		/**
		* @brief	Saves a log about the current state of the application to the specified location.
		* @param	path	Absolute path to the log filename.
		*/
		/************************************************************************************************************************/
		void SaveLog(const Path& path) const;

		/************************************************************************************************************************/
		/**
		* @brief	Triggered when a new entry in the log is added.
		* @note	Sim thread only.
		*/
		/************************************************************************************************************************/
		Event<void(const LogEntry&)> OnLogEntryAdded;

		/************************************************************************************************************************/
		/**
		* @brief	Triggered whenever one or multiple log entries were added or removed.
		*			Triggers only once per frame.
		* @note	Sim thread only.
		*/
		/************************************************************************************************************************/
		Event<void()> OnLogModified;

		/************************************************************************************************************************/
		/**
		* @brief	Triggers callbacks that notify external code that a log entry was added.
		* @note	Internal method. Sim thread only.
		*/
		/************************************************************************************************************************/
		void _TriggerCallbacks();

		Debug() : m_LogHash(0) {}

	private:
		uint64 m_LogHash;
		Log m_Log;
	};

	/************************************************************************************************************************/
	/**
	* @brief	A simpler way of accessing the Debug module.
	*/
	/************************************************************************************************************************/
	GE_UTILITY_EXPORT Debug& g_Debug();

	/************************************************************************************************************************/
	/**
	* @brief	Shortcut for logging a message in the debug channel.
	*/
	/************************************************************************************************************************/
#define LOGDBG(x) geEngineSDK::g_Debug().LogDebug((x));

	/************************************************************************************************************************/
	/**
	* @brief	Shortcut for logging a message in the warning channel.
	*/
	/************************************************************************************************************************/
#define LOGWRN(x) geEngineSDK::g_Debug().LogWarning((x));

	/************************************************************************************************************************/
	/**
	* @brief	Shortcut for logging a message in the error channel.
	*/
	/************************************************************************************************************************/
#define LOGERR(x) geEngineSDK::g_Debug().LogError((x));

	/************************************************************************************************************************/
	/**
	* @brief	Shortcut for logging a verbose message in the debug channel. Verbose messages can be ignored unlike other
	*			log messages.
	*/
	/************************************************************************************************************************/
#define LOGDBG_VERBOSE(x)

	/************************************************************************************************************************/
	/**
	* @brief	Shortcut for logging a verbose message in the warning channel. Verbose messages can be ignored unlike other
	*			log messages.
	*/
	/************************************************************************************************************************/
#define LOGWRN_VERBOSE(x)
}
