/********************************************************************/
/**
 * @file   geWin32CrashHandler.cpp
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
#include "gePrerequisitesUtil.h"
#include "geDebug.h"
#include "geDynLib.h"
#include "geFileSystem.h"
#include "geMath.h"
#include "windows.h"
#include <psapi.h>

//Disable warning in VS2015 that's not under my control
#pragma warning(disable : 4091)
#	include "DbgHelp.h"
#pragma warning(default : 4091)

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Returns the raw stack trace using the provided context. Raw stack trace contains only function addresses.
	* @param	context		Processor context from which to start the stack trace.
	* @param	stackTrace	Output parameter that will contain the function addresses. First address is the deepest
	* 						called function and following address is its caller and so on.
	* @returns	Number of functions in the call stack.
	*/
	/************************************************************************************************************************/
	uint32 Win32_GetRawStackTrace(CONTEXT context, uint64 stackTrace[GE_MAX_STACKTRACE_DEPTH])
	{
		HANDLE hProcess = GetCurrentProcess();
		HANDLE hThread = GetCurrentThread();
		uint32 machineType;

		STACKFRAME64 stackFrame;
		memset(&stackFrame, 0, sizeof(stackFrame));

		stackFrame.AddrPC.Mode = AddrModeFlat;
		stackFrame.AddrStack.Mode = AddrModeFlat;
		stackFrame.AddrFrame.Mode = AddrModeFlat;

#if GE_ARCH_TYPE == GE_ARCHITECTURE_x86_64
		stackFrame.AddrPC.Offset = context.Rip;
		stackFrame.AddrStack.Offset = context.Rsp;
		stackFrame.AddrFrame.Offset = context.Rbp;
		machineType = IMAGE_FILE_MACHINE_AMD64;
#else
		stackFrame.AddrPC.Offset = context.Eip;
		stackFrame.AddrStack.Offset = context.Esp;
		stackFrame.AddrFrame.Offset = context.Ebp;
		machineType = IMAGE_FILE_MACHINE_I386;
#endif

		uint32 numEntries = 0;
		while( true )
		{
			if( !StackWalk64(machineType, hProcess, hThread, &stackFrame, &context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr) )
			{
				break;
			}

			if( numEntries < GE_MAX_STACKTRACE_DEPTH )
			{
				stackTrace[numEntries] = stackFrame.AddrPC.Offset;
			}

			numEntries++;

			if( stackFrame.AddrPC.Offset == 0 || stackFrame.AddrFrame.Offset == 0 )
			{
				break;
			}
		}

		return numEntries;
	}

	/************************************************************************************************************************/
	/**
	* @brief	Returns a string containing a stack trace using the provided context. If function can be found in the symbol
	* 			table its readable name will be present in the stack trace, otherwise just its address.
	*
	* @param	context		Processor context from which to start the stack trace.
	* @param	skip		Number of bottom-most call stack entries to skip.
	*
	* @returns	String containing the call stack with each function on its own line.
	*/
	/************************************************************************************************************************/
	String Win32_GetStackTrace(CONTEXT context, uint32 skip = 0)
	{
		uint64 rawStackTrace[GE_MAX_STACKTRACE_DEPTH];
		uint32 numEntries = Win32_GetRawStackTrace(context, rawStackTrace);

		numEntries = Math::Min((uint32)GE_MAX_STACKTRACE_DEPTH, numEntries);

		uint32 bufferSize = sizeof(PIMAGEHLP_SYMBOL64) + GE_MAX_STACKTRACE_NAME_BYTES;
		uint8* buffer = (uint8*)ge_alloc(bufferSize);

		PIMAGEHLP_SYMBOL64 symbol = (PIMAGEHLP_SYMBOL64)buffer;
		symbol->SizeOfStruct = bufferSize;
		symbol->MaxNameLength = GE_MAX_STACKTRACE_NAME_BYTES;

		HANDLE hProcess = GetCurrentProcess();

		StringStream outputStream;
		for( uint32 i=skip; i<numEntries; i++ )
		{
			if( i > skip )
			{
				outputStream << std::endl;
			}

			DWORD64 funcAddress = rawStackTrace[i];

			//Output function name
			DWORD64 dummy;
			if( SymGetSymFromAddr64(hProcess, funcAddress, &dummy, symbol) )
			{
				outputStream << StringUtil::format("{0}() - ", symbol->Name);
			}

			//Output file name and line
			IMAGEHLP_LINE64	lineData;
			lineData.SizeOfStruct = sizeof(lineData);

			String addressString = toString(funcAddress, 0, ' ', std::ios::hex);

			DWORD column;
			if( SymGetLineFromAddr64(hProcess, funcAddress, &column, &lineData) )
			{
				Path filePath = lineData.FileName;

				outputStream << StringUtil::format("0x{0} File[{1}:{2} ({3})]", addressString, filePath.getFilename(), lineData.LineNumber, column);
			}
			else
			{
				outputStream << StringUtil::format("0x{0}", addressString);
			}

			//Output module name
			IMAGEHLP_MODULE64 moduleData;
			moduleData.SizeOfStruct = sizeof(moduleData);

			if( SymGetModuleInfo64(hProcess, funcAddress, &moduleData) )
			{
				Path filePath = moduleData.ImageName;
				outputStream << StringUtil::format(" Module[{0}]", filePath.getFilename());
			}
		}

		ge_free(buffer);

		return outputStream.str();
	}

	typedef bool(WINAPI *EnumProcessModulesType)(HANDLE hProcess, HMODULE* lphModule, DWORD cb, LPDWORD lpcbNeeded);
	typedef DWORD(WINAPI *GetModuleBaseNameType)(HANDLE hProcess, HMODULE hModule, LPSTR lpBaseName, DWORD nSize);
	typedef DWORD(WINAPI *GetModuleFileNameExType)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize);
	typedef bool(WINAPI *GetModuleInformationType)(HANDLE hProcess, HMODULE hModule, LPMODULEINFO lpmodinfo, DWORD cb);

	static DynLib* g_PSAPILib = nullptr;

	static EnumProcessModulesType g_EnumProcessModules;
	static GetModuleBaseNameType g_GetModuleBaseName;
	static GetModuleFileNameExType g_GetModuleFileNameEx;
	static GetModuleInformationType g_GetModuleInformation;

	/************************************************************************************************************************/
	/**
	* @brief	Dynamically load the PSAPI.dll and the required symbols, if not already loaded.
	*/
	/************************************************************************************************************************/
	void Win32_InitPSAPI()
	{
		if( g_PSAPILib != nullptr)
		{
			return;
		}

		g_PSAPILib = ge_new<DynLib>("PSAPI.dll");
		g_EnumProcessModules = (EnumProcessModulesType)g_PSAPILib->GetSymbol("EnumProcessModules");
		g_GetModuleBaseName = (GetModuleBaseNameType)g_PSAPILib->GetSymbol("GetModuleFileNameExA");
		g_GetModuleFileNameEx = (GetModuleFileNameExType)g_PSAPILib->GetSymbol("GetModuleBaseNameA");
		g_GetModuleInformation = (GetModuleInformationType)g_PSAPILib->GetSymbol("GetModuleInformation");
	}

	/************************************************************************************************************************/
	/**
	* @brief	Unloads the PSAPI.dll if is loaded.
	*/
	/************************************************************************************************************************/
	void Win32_UnloadPSAPI()
	{
		if( g_PSAPILib == nullptr )
		{
			return;
		}

		g_PSAPILib->Unload();
		ge_delete(g_PSAPILib);
		g_PSAPILib = nullptr;
	}

	static bool g_SymbolsLoaded = false;

	/************************************************************************************************************************/
	/**
	* @brief	Loads symbols for all modules in the current process. Loaded symbols allow the stack walker to retrieve
	* 			human readable method, file, module names and other information.
	*/
	/************************************************************************************************************************/
	void Win32_LoadSymbols()
	{
		if( g_SymbolsLoaded )
		{
			return;
		}

		HANDLE hProcess = GetCurrentProcess();
		uint32 options = SymGetOptions();

		options |= SYMOPT_LOAD_LINES;
		options |= SYMOPT_EXACT_SYMBOLS;
		options |= SYMOPT_UNDNAME;
		options |= SYMOPT_FAIL_CRITICAL_ERRORS;
		options |= SYMOPT_NO_PROMPTS;

		SymSetOptions(options);
		if( !SymInitialize(hProcess, nullptr, false) )
		{
			LOGERR("SymInitialize failed. Error code: " + toString((uint32)GetLastError()));
			return;
		}

		DWORD bufferSize;
		g_EnumProcessModules(hProcess, nullptr, 0, &bufferSize);

		HMODULE* modules = (HMODULE*)ge_alloc(bufferSize);
		g_EnumProcessModules(hProcess, modules, bufferSize, &bufferSize);

		uint32 numModules = bufferSize / sizeof(HMODULE);
		for( uint32 i=0; i<numModules; ++i )
		{
			MODULEINFO moduleInfo;

			char moduleName[GE_MAX_STACKTRACE_NAME_BYTES];
			char imageName[GE_MAX_STACKTRACE_NAME_BYTES];

			g_GetModuleInformation(hProcess, modules[i], &moduleInfo, sizeof(moduleInfo));
			g_GetModuleFileNameEx(hProcess, modules[i], imageName, GE_MAX_STACKTRACE_NAME_BYTES);
			g_GetModuleBaseName(hProcess, modules[i], moduleName, GE_MAX_STACKTRACE_NAME_BYTES);

			char pdbSearchPath[GE_MAX_STACKTRACE_NAME_BYTES];
			char* fileName = nullptr;
			GetFullPathNameA(moduleName, GE_MAX_STACKTRACE_NAME_BYTES, pdbSearchPath, &fileName);
			*fileName = '\0';

			SymSetSearchPath(GetCurrentProcess(), pdbSearchPath);

			DWORD64 moduleAddress = SymLoadModule64(hProcess, modules[i], imageName, moduleName, (DWORD64)moduleInfo.lpBaseOfDll, (DWORD)moduleInfo.SizeOfImage);

			if( moduleAddress != 0 )
			{
				IMAGEHLP_MODULE64 imageInfo;
				memset(&imageInfo, 0, sizeof(imageInfo));
				imageInfo.SizeOfStruct = sizeof(imageInfo);

				if( !SymGetModuleInfo64(GetCurrentProcess(), moduleAddress, &imageInfo) )
				{
					LOGWRN("Failed retrieving module info for module: " + String(moduleName) + ". Error code: " + toString((uint32)GetLastError()));
				}
				else
				{
#if GE_DEBUG_DETAILED_SYMBOLS
					if( imageInfo.SymType == SymNone )
					{
						LOGWRN("Failed loading symbols for module: " + String(moduleName));
					}
#endif
				}
			}
			else
			{
				LOGWRN("Failed loading module " + String(moduleName) + ".Error code: " + toString((uint32)GetLastError()) + ". Search path: " + String(pdbSearchPath) + ". Image name: " + String(imageName));
			}
		}

		ge_free(modules);
		g_SymbolsLoaded = true;
	}

	/************************************************************************************************************************/
	/**
	* @brief	Converts an exception record into a human readable error message.
	*/
	/************************************************************************************************************************/
	String Win32_GetExceptionMessage(EXCEPTION_RECORD* record)
	{
		String exceptionAddress = toString((uint64)record->ExceptionAddress, 0, ' ', std::ios::hex);

		String format;
		switch( record->ExceptionCode )
		{
			case EXCEPTION_ACCESS_VIOLATION:
			{
				DWORD_PTR violatedAddress = 0;
				if( record->NumberParameters == 2 )
				{
					if( record->ExceptionInformation[0] == 0 )
					{
						format = "Unhandled exception at 0x{0}. Access violation reading 0x{1}.";
					}
					else if( record->ExceptionInformation[0] == 8 )
					{
						format = "Unhandled exception at 0x{0}. Access violation DEP 0x{1}.";
					}
					else
					{
						format = "Unhandled exception at 0x{0}. Access violation writing 0x{1}.";
					}

					violatedAddress = record->ExceptionInformation[1];
				}
				else
				{
					format = "Unhandled exception at 0x{0}. Access violation.";
				}

				String violatedAddressStr = toString((uint64)violatedAddress, 0, ' ', std::ios::hex);
				return StringUtil::format(format, exceptionAddress, violatedAddressStr);
			}
			case EXCEPTION_IN_PAGE_ERROR:
			{
				DWORD_PTR violatedAddress = 0;
				DWORD_PTR code = 0;
				if( record->NumberParameters == 3 )
				{
					if( record->ExceptionInformation[0] == 0 )
					{
						format = "Unhandled exception at 0x{0}. Page fault reading 0x{1} with code 0x{2}.";
					}
					else if( record->ExceptionInformation[0] == 8 )
					{
						format = "Unhandled exception at 0x{0}. Page fault DEP 0x{1} with code 0x{2}.";
					}
					else
					{
						format = "Unhandled exception at 0x{0}. Page fault writing 0x{1} with code 0x{2}.";
					}

					violatedAddress = record->ExceptionInformation[1];
					code = record->ExceptionInformation[3];
				}
				else
				{
					format = "Unhandled exception at 0x{0}. Page fault.";
				}

				String violatedAddressStr = toString((uint64)violatedAddress, 0, ' ', std::ios::hex);
				String codeStr = toString((uint64)code, 0, ' ', std::ios::hex);
				return StringUtil::format(format, exceptionAddress, violatedAddressStr, codeStr);
			}
			case STATUS_ARRAY_BOUNDS_EXCEEDED:
			{
				format = "Unhandled exception at 0x{0}. Attempting to access an out of range array element.";
				return StringUtil::format(format, exceptionAddress);
			}
			case EXCEPTION_DATATYPE_MISALIGNMENT:
			{
				format = "Unhandled exception at 0x{0}. Attempting to access missaligned data.";
				return StringUtil::format(format, exceptionAddress);
			}
			case EXCEPTION_FLT_DENORMAL_OPERAND:
			{
				format = "Unhandled exception at 0x{0}. Floating point operand too small.";
				return StringUtil::format(format, exceptionAddress);
			}
			case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			{
				format = "Unhandled exception at 0x{0}. Floating point operation attempted to divide by zero.";
				return StringUtil::format(format, exceptionAddress);
			}
			case EXCEPTION_FLT_INVALID_OPERATION:
			{
				format = "Unhandled exception at 0x{0}. Floating point invalid operation.";
				return StringUtil::format(format, exceptionAddress);
			}
			case EXCEPTION_FLT_OVERFLOW:
			{
				format = "Unhandled exception at 0x{0}. Floating point overflow.";
				return StringUtil::format(format, exceptionAddress);
			}
			case EXCEPTION_FLT_UNDERFLOW:
			{
				format = "Unhandled exception at 0x{0}. Floating point underflow.";
				return StringUtil::format(format, exceptionAddress);
			}
			case EXCEPTION_FLT_STACK_CHECK:
			{
				format = "Unhandled exception at 0x{0}. Floating point stack overflow/underflow.";
				return StringUtil::format(format, exceptionAddress);
			}
			case EXCEPTION_ILLEGAL_INSTRUCTION:
			{
				format = "Unhandled exception at 0x{0}. Attempting to execute an illegal instruction.";
				return StringUtil::format(format, exceptionAddress);
			}
			case EXCEPTION_PRIV_INSTRUCTION:
			{
				format = "Unhandled exception at 0x{0}. Attempting to execute a private instruction.";
				return StringUtil::format(format, exceptionAddress);
			}
			case EXCEPTION_INT_DIVIDE_BY_ZERO:
			{
				format = "Unhandled exception at 0x{0}. Integer operation attempted to divide by zero.";
				return StringUtil::format(format, exceptionAddress);
			}
			case EXCEPTION_INT_OVERFLOW:
			{
				format = "Unhandled exception at 0x{0}. Integer operation result has overflown.";
				return StringUtil::format(format, exceptionAddress);
			}
			case EXCEPTION_STACK_OVERFLOW:
			{
				format = "Unhandled exception at 0x{0}. Stack overflow.";
				return StringUtil::format(format, exceptionAddress);
			}
			default:
			{
				format = "Unhandled exception at 0x{0}. Code 0x{1}.";

				String exceptionCode = toString((uint32)record->ExceptionCode, 0, ' ', std::ios::hex);
				return StringUtil::format(format, exceptionAddress, exceptionCode);
			}
		}
	}

	struct MiniDumpParams
	{
		Path FilePath;
		EXCEPTION_POINTERS* ExceptionData;
	};

	DWORD CALLBACK Win32_WriteMiniDumpWorker(void* data)
	{
		MiniDumpParams* params = (MiniDumpParams*)data;

		HANDLE hFile = CreateFileW(params->FilePath.toWString().c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if( hFile != INVALID_HANDLE_VALUE )
		{
			MINIDUMP_EXCEPTION_INFORMATION DumpExceptionInfo;

			DumpExceptionInfo.ThreadId = GetCurrentThreadId();
			DumpExceptionInfo.ExceptionPointers = params->ExceptionData;
			DumpExceptionInfo.ClientPointers = false;

			MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &DumpExceptionInfo, nullptr, nullptr);
			CloseHandle(hFile);
		}

		return 0;
	}

	void Win32_WriteMiniDump(const Path& filePath, EXCEPTION_POINTERS* exceptionData)
	{
		MiniDumpParams param = { filePath, exceptionData };

		//Write mini dump on a second thread in order to preserve the current thread's call stack
		DWORD threadId = 0;
		HANDLE hThread = CreateThread(nullptr, 0, &Win32_WriteMiniDumpWorker, &param, 0, &threadId);

		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}

	static const UNICHAR* g_MiniDumpName = L"Minidump.dmp";
	const UNICHAR* CrashHandler::CrashReportFolder = L"Reports/{0}/";
	const UNICHAR* CrashHandler::CrashLogName = L"Log.html";

	struct CrashHandler::Data
	{
		Mutex mutex;
	};

	CrashHandler::CrashHandler()
	{
		m_CrashData = ge_new<Data>();
	}

	CrashHandler::~CrashHandler()
	{
		Win32_UnloadPSAPI();
		ge_delete(m_CrashData);
	}

	void CrashHandler::ReportCrash(const String& type, const String& description, const String& function, const String& file, uint32 line) const
	{
		//Win32 debug methods are not thread safe
		Lock<>(m_CrashData->mutex);

		String stackTrace = GetStackTrace();

		StringStream errorMessageStream;
		errorMessageStream << "Fatal error occurred. The program has to terminate!" << std::endl;
		errorMessageStream << "\t\t" << type << " - " << description << std::endl;
		errorMessageStream << "\t\t in " << function << " [" << file << ":" << line << "]" << std::endl;
		errorMessageStream << std::endl;
		errorMessageStream << "Stack trace: " << std::endl;
		errorMessageStream << stackTrace;

		String errorMessage = errorMessageStream.str();
		g_Debug().LogError(errorMessage);

		Path crashFolder = GetCrashFolder();
		FileSystem::CreateDir(crashFolder);

		g_Debug().SaveLog(crashFolder + WString(CrashLogName));
		Win32_WriteMiniDump(crashFolder + WString(g_MiniDumpName), nullptr);

		WString simpleErrorMessage = L"Fatal error occurred. The program has to terminate! \n\nFor more info check the report at:\n " + crashFolder.toWString();
		MessageBoxW(nullptr, simpleErrorMessage.c_str(), L"Fatal Error!", MB_OK);
		//TODO: Potentially also log Windows Error Report and/or send crash data to server
	}

	int CrashHandler::ReportCrash(void* exceptionDataPtr) const
	{
		EXCEPTION_POINTERS* exceptionData = (EXCEPTION_POINTERS*)exceptionDataPtr;

		//Win32 debug methods are not thread safe
		Lock<>(m_CrashData->mutex);

		Win32_InitPSAPI();
		Win32_LoadSymbols();
		String stackTrace = Win32_GetStackTrace(*exceptionData->ContextRecord, 0);

		StringStream errorMessageStream;
		errorMessageStream << "Fatal error occurred. The program has to terminate!" << std::endl;
		errorMessageStream << "\t\t" << Win32_GetExceptionMessage(exceptionData->ExceptionRecord) << std::endl;;
		errorMessageStream << std::endl;
		errorMessageStream << "Stack trace: " << std::endl;
		errorMessageStream << stackTrace;

		String errorMessage = errorMessageStream.str();
		g_Debug().LogError(errorMessage);

		Path crashFolder = GetCrashFolder();
		FileSystem::CreateDir(crashFolder);

		g_Debug().SaveLog(crashFolder + WString(CrashLogName));
		Win32_WriteMiniDump(crashFolder + WString(g_MiniDumpName), exceptionData);

		WString simpleErrorMessage = L"Fatal error occurred. The program has to terminate! \n\nFor more info check the report at:\n" + crashFolder.toWString();

		MessageBoxW(nullptr, simpleErrorMessage.c_str(), L"Fatal Error!", MB_OK);
		//TODO: Potentially also log Windows Error Report and/or send crash data to server

		return EXCEPTION_EXECUTE_HANDLER;
	}

	Path CrashHandler::GetCrashFolder() const
	{
		SYSTEMTIME systemTime;
		GetLocalTime(&systemTime);

		WString timeStamp = L"{0}{1}{2}_{3}{4}";
		WString strYear =	toWString(systemTime.wYear,	 4, '0');
		WString strMonth =	toWString(systemTime.wMonth, 2, '0');
		WString strDay =	toWString(systemTime.wDay,   2, '0');
		WString strHour =	toWString(systemTime.wHour,  2, '0');
		WString strMinute = toWString(systemTime.wMinute,2, '0');

		//timeStamp = StringUtil::format(timeStamp, strYear, strMonth, strDay, strHour, strMinute);
		WString folderName = StringUtil::format(CrashReportFolder, timeStamp);

		return FileSystem::GetWorkingDirectoryPath() + folderName;
	}

	String CrashHandler::GetStackTrace()
	{
		CONTEXT context;
		RtlCaptureContext(&context);

		Win32_InitPSAPI();
		Win32_LoadSymbols();
		return Win32_GetStackTrace(context, 2);
	}

	CrashHandler& g_CrashHandler()
	{
		return CrashHandler::Instance();
	}
}
