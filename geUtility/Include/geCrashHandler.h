/********************************************************************/
/**
 * @file   geCrashHandler.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/03/10
 * @brief  Saves crash data and notifies when a crash occurs
 *
 * Saves crash data and notifies the user when a crash occurs
 *
 * @bug	   Crashes are reported in the same process as the main
 *		   application. This can be a problem if the crash was caused
 *		   by heap. Any further use of the heap by the reporting
 *		   methods will cause a silent crash, failing to log it.
 *		   A more appropriate way of doing it should be to resume
 *		   another process to actually handle the crash.
 */
/********************************************************************/
#pragma once

#define GE_MAX_STACKTRACE_DEPTH 200
#define GE_MAX_STACKTRACE_NAME_BYTES 1024

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Saves crash data and notifies the user when a crash occurs.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT CrashHandler
	{
	public:
		CrashHandler();
		~CrashHandler();

		/************************************************************************************************************************/
		/**
		* @brief	Constructs and starts the module.
		*/
		/************************************************************************************************************************/
		static void StartUp()
		{
			_Instance() = ge_new<CrashHandler>();
		}

		/************************************************************************************************************************/
		/**
		* @brief	Shuts down this module and frees any resources it is using.
		*/
		/************************************************************************************************************************/
		static void ShutDown()
		{
			ge_delete( _Instance() );
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns a reference to the module instance.
		*/
		/************************************************************************************************************************/
		static CrashHandler& Instance()
		{
			return *_Instance();
		}

		/************************************************************************************************************************/
		/**
		* @brief	Records a crash with a custom error message.
		* @param[in]	type		Type of the crash that occurred. e.g. "InvalidParameter".
		* @param[in]	description	More detailed description of the issue that caused the crash.
		* @param[in]	function	Optional name of the function where the error occurred.
		* @param[in]	file		Optional name of the source code file in which the code that crashed the program exists.
		* @param[in]	line		Optional source code line at which the crash was triggered at.
		*/
		/************************************************************************************************************************/
		void ReportCrash(const String& type, const String& description, const String& function=StringUtil::BLANK, const String& file=StringUtil::BLANK, uint32 line=0) const;

#if GE_PLATFORM == GE_PLATFORM_WIN32
		/************************************************************************************************************************/
		/**
		* @brief	Records a crash resulting from a Windows-specific SEH exception.
		* @param[in]	exceptionData	Exception data returned from GetExceptionInformation()
		* @return	Code that signals the __except exception handler on how to proceed.
		* @note		Available in Windows builds only.
		*/
		/************************************************************************************************************************/
		int ReportCrash(void* exceptionData) const;
#endif

		/************************************************************************************************************************/
		/**
		* @brief	Returns a string containing a current stack trace. If function can be found in the symbol table its readable
		*			name will be present in the stack trace, otherwise just its address.
		* @return	String containing the call stack with each function on its own line.
		*/
		/************************************************************************************************************************/
		static String GetStackTrace();
	private:
		/************************************************************************************************************************/
		/**
		* @brief	Returns path to the folder into which to store the crash reports.
		*/
		/************************************************************************************************************************/
		Path GetCrashFolder() const;

		/************************************************************************************************************************/
		/**
		* @brief	Returns a singleton instance of this module.
		*/
		/************************************************************************************************************************/
		static CrashHandler*& _Instance()
		{
			static CrashHandler* inst = nullptr;
			return inst;
		}

		static const UNICHAR* CrashReportFolder;
		static const UNICHAR* CrashLogName;

		struct Data;
		Data* m_CrashData;
	};

	GE_UTILITY_EXPORT CrashHandler& g_CrashHandler();
}
