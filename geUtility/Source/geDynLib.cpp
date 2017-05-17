/********************************************************************/
/**
 * @file   geDynLib.cpp
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/09/25
 * @brief  Class that holds data about a dynamic library.
 *
 * Class that holds data about a dynamic library.
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#include "geDynLib.h"
#include "geException.h"

#if GE_PLATFORM == GE_PLATFORM_WIN32
#	define WIN32_LEAN_AND_MEAN
#	if !defined(NOMINMAX) && defined(_MSC_VER)
#		define NOMINMAX // required to stop windows.h messing up std::min
#	endif
#	include <windows.h>
#endif

#if GE_PLATFORM == GE_PLATFORM_APPLE
#	include "macUtils.h"
#	include <dlfcn.h>
#endif

namespace geEngineSDK
{
	DynLib::DynLib(const String& name)
	{
		m_Name = name;
		m_hInst = nullptr;
		Load();
	}

	DynLib::~DynLib()
	{
	}

	void DynLib::Load()
	{
		if( m_hInst )
		{
			return;
		}

		String name = m_Name;
#if GE_PLATFORM == GE_PLATFORM_LINUX
		//dlopen() does not add .so to the filename, like windows does for .dll
		if( name.substr(name.length() - 3, 3) != ".so" )
		{
			name += ".so";
		}
#elif GE_PLATFORM == GE_PLATFORM_OSX
		//dlopen() does not add .dylib to the filename, like windows does for .dll
		if( name.substr(name.length() - 6, 6) != ".dylib" )
		{
			name += ".dylib";
		}
#elif GE_PLATFORM == GE_PLATFORM_WIN32
		//Although LoadLibraryEx will add .dll itself when you only specify the library name, if you include a relative path then it does not. So, add it to be sure.
		if( name.substr(name.length() - 4, 4) != ".dll" )
		{
			name += ".dll";
		}
#endif
		m_hInst = (DYNLIB_HANDLE)DYNLIB_LOAD(name.c_str());
		if( !m_hInst )
		{
			GE_EXCEPT(InternalErrorException, "Could not load dynamic library " + m_Name + ".  System Error: " + DynLibError());
		}
	}

	void DynLib::Unload()
	{
		if( !m_hInst )
		{
			return;
		}

		if( DYNLIB_UNLOAD(m_hInst) )
		{
			GE_EXCEPT(InternalErrorException, "Could not unload dynamic library " + m_Name + ".  System Error: " + DynLibError());
		}
	}

	void* DynLib::GetSymbol(const String& strName) const
	{
		if( !m_hInst )
		{
			return nullptr;
		}

		return (void*)DYNLIB_GETSYM(m_hInst, strName.c_str());
	}

	String DynLib::DynLibError()
	{
#if GE_PLATFORM == GE_PLATFORM_WIN32
		LPVOID lpMsgBuf;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
		String ret = (char*)lpMsgBuf;
		LocalFree(lpMsgBuf);	//Free the buffer.
		return ret;
#elif GE_PLATFORM == GE_PLATFORM_LINUX || GE_PLATFORM == GE_PLATFORM_OSX
		return String(dlerror());
#else
		return String("");
#endif
	}
}
