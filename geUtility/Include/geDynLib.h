/********************************************************************/
/**
 * @file   geDynLib.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/09/25
 * @brief  Class that holds data about a dynamic library.
 *
 * Class that holds data about a dynamic library.
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#pragma once

#include "gePrerequisitesUtil.h"

#if GE_PLATFORM == GE_PLATFORM_WIN32
#    define DYNLIB_HANDLE hInstance
#    define DYNLIB_LOAD(a) LoadLibraryEx(a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)
#    define DYNLIB_GETSYM(a, b) GetProcAddress(a, b)
#    define DYNLIB_UNLOAD(a) !FreeLibrary(a)

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

#elif GE_PLATFORM == GE_PLATFORM_PS4
#    define DYNLIB_HANDLE SceKernelModule
#    define DYNLIB_LOAD(a, b, c, d, e, f) sceKernelLoadStartModule(a, b, c, d, e, f)
#    define DYNLIB_GETSYM(a, b, c) sceKernelDlsym(a, b, c);
#    define DYNLIB_UNLOAD(a) sceKernelStopUnloadModule(a, b, c, d, e, f)

#elif GE_PLATFORM == GE_PLATFORM_LINUX
#    define DYNLIB_HANDLE void*
#    define DYNLIB_LOAD(a) dlopen(a, RTLD_LAZY | RTLD_GLOBAL)
#    define DYNLIB_GETSYM(a, b) dlsym(a, b)
#    define DYNLIB_UNLOAD(a) dlclose(a)

#elif GE_PLATFORM == GE_PLATFORM_OSX
#    define DYNLIB_HANDLE void*
#    define DYNLIB_LOAD(a) mac_loadDylib(a)
#    define DYNLIB_GETSYM(a, b) dlsym(a, b)
#    define DYNLIB_UNLOAD(a) dlclose(a)

#endif

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Class that holds data about a dynamic library.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT DynLib
	{
	public:
		/************************************************************************************************************************/
		/**
		* @brief	Constructs the dynamic library object and loads the library with the specified name.
		*/
		/************************************************************************************************************************/
		DynLib(const String& name);
		~DynLib();

		/************************************************************************************************************************/
		/**
		* @brief	Loads the library. Does nothing if library is already loaded.
		*/
		/************************************************************************************************************************/
		void Load();

		/************************************************************************************************************************/
		/**
		* @brief	Unloads the library. Does nothing if library is not loaded.
		*/
		/************************************************************************************************************************/
		void Unload();

		/************************************************************************************************************************/
		/**
		* @brief	Get the name of the library.
		*/
		/************************************************************************************************************************/
		const String& GetName() const { return m_Name; }

		/************************************************************************************************************************/
		/**
		* @brief	Returns the address of the given symbol from the loaded library.
		* @param[in] strName	The name of the symbol to search for.
		* @return	If the function succeeds, the returned value is a handle to the symbol. Otherwise null.
		*/
		/************************************************************************************************************************/
		void* GetSymbol(const String& strName) const;

	protected:
		friend class DynLibManager;

		/************************************************************************************************************************/
		/**
		* @brief	Gets the last loading error.
		*/
		/************************************************************************************************************************/
		String DynLibError();

	protected:
		String m_Name;
		DYNLIB_HANDLE m_hInst;	//Handle to the loaded library.
	};
}
