/*****************************************************************************/
/**
 * @file    geDynLib.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/09/25
 * @brief   Class that holds data about a dynamic library.
 *
 * Class that holds data about a dynamic library.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geDynLib.h"
#include "geException.h"

#if GE_PLATFORM == GE_PLATFORM_WIN32
#	define WIN32_LEAN_AND_MEAN
#	if !defined(NOMINMAX) && defined(_MSC_VER)
#		define NOMINMAX // required to stop windows.h messing up std::min
#	endif
#	include <Windows.h>
#endif

#if GE_PLATFORM == GE_PLATFORM_OSX
# include <dlfcn.h>
#endif

namespace geEngineSDK {

#if GE_PLATFORM == GE_PLATFORM_LINUX
  const char* DynLib::EXTENSION = "so";
#elif GE_PLATFORM == GE_PLATFORM_OSX
  const char* DynLib::EXTENSION = "dylib";
#elif GE_PLATFORM == GE_PLATFORM_WIN32
  const char* DynLib::EXTENSION = "dll";
#elif GE_PLATFORM == GE_PLATFORM_PS4
  const char* DynLib::EXTENSION = "prx";
#else
#  error Unhandled platform
#endif

  DynLib::DynLib(const String& name) {
    m_name = name;
    m_hInst = nullptr;
    load();
  }

  DynLib::~DynLib() {}

  void
  DynLib::load() {
    if (m_hInst) {
      return;
    }

    m_hInst = (DYNLIB_HANDLE)DYNLIB_LOAD(m_name.c_str());

    if (!m_hInst) {
      GE_EXCEPT(InternalErrorException,
                "Could not load dynamic library " +
                m_name +
                ".  System Error: " +
                dynlibError());
    }
  }

  void DynLib::unload() {
    if (!m_hInst) {
      return;
    }

    if (DYNLIB_UNLOAD(m_hInst)) {
      GE_EXCEPT(InternalErrorException,
                "Could not unload dynamic library " +
                m_name +
                ".  System Error: " +
                dynlibError());
    }
  }

  void* DynLib::getSymbol(const String& strName) const {
    if (!m_hInst) {
      return nullptr;
    }

    return (void*)DYNLIB_GETSYM(m_hInst, strName.c_str());
  }

  String DynLib::dynlibError() {
#if GE_PLATFORM == GE_PLATFORM_WIN32
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_FROM_SYSTEM | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&lpMsgBuf,
                  0,
                  NULL);

    String ret = static_cast<char*>(lpMsgBuf);
    LocalFree(lpMsgBuf);  //Free the buffer.
    return ret;
#elif GE_PLATFORM == GE_PLATFORM_LINUX || GE_PLATFORM == GE_PLATFORM_OSX
    return String(dlerror());
#else
    return String("");
#endif
  }
}
