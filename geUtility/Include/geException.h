/********************************************************************/
/**
 * @file   geException.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2015/02/22
 * @brief  Exceptions Base Classes
 *
 * This file contains the basic declaration of Exception objects
 * and classes for the use on the library.
 *
 * @bug	   warning C4275: non dll-interface class 'std::exception'
 * used as base for dll-interface class 'geEngineSDK::Exception'
 */
/********************************************************************/
#pragma once

#include "gePrerequisitesUtil.h"

#if GE_COMPILER == GE_COMPILER_MSVC
	#undef __PRETTY_FUNCTION__
	#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Base class for all geEngine exceptions.
	*/
	/************************************************************************************************************************/
#if GE_COMPILER == GE_COMPILER_MSVC
	#pragma warning( push )
	#pragma warning( disable : 4275 )
#endif

	class Exception : public std::exception
	{
	public:
		Exception(const char* type, const String& description, const String& source)
			: m_Line(0), m_TypeName(type), m_Description(description), m_Source(source)
		{
		}

		Exception(const char* type, const String& description, const String& source, const char* file, long line)
			: m_Line(line), m_TypeName(type), m_Description(description), m_Source(source), m_File(file)
		{
		}

		Exception(const Exception& rhs)
			: m_Line(rhs.m_Line), m_TypeName(rhs.m_TypeName), m_Description(rhs.m_Description), m_Source(rhs.m_Source), m_File(rhs.m_File)
		{
		}

		~Exception() _NOEXCEPT {}

		void operator = (const Exception& rhs)
		{
			m_Description = rhs.m_Description;
			m_Source = rhs.m_Source;
			m_File = rhs.m_File;
			m_Line = rhs.m_Line;
			m_TypeName = rhs.m_TypeName;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns a string with the full description of the exception.
		*
		* @note		The description contains the error number, the description supplied by the thrower, what routine threw the
		*			exception, and will also supply extra platform-specific information where applicable.
		*/
		/************************************************************************************************************************/
		virtual const String& GetFullDescription() const
		{
			if( m_FullDesc.empty() )
			{
				StringStream desc;
				desc << "GEENGINE EXCEPTION(" << m_TypeName << "): " << m_Description << " in " << m_Source;

				if( m_Line > 0 )
				{
					desc << " at " << m_File << " (line " << m_Line << ")";
				}

				m_FullDesc = desc.str();
			}

			return m_FullDesc;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Gets the source function that threw the exception.
		*/
		/************************************************************************************************************************/
		virtual const String& GetSource() const
		{
			return m_Source;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Gets the source file name in which the exception was thrown.
		*/
		/************************************************************************************************************************/
		virtual const String& GetFile() const
		{
			return m_File;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Gets line number on which the exception was thrown.
		*/
		/************************************************************************************************************************/
		virtual long GetLine() const
		{
			return m_Line;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Gets a short description about the exception.
		*/
		/************************************************************************************************************************/
		virtual const String& GetDescription(void) const
		{
			return m_Description;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Overriden std::exception::what. Returns the same value as "GetFullDescription".
		*/
		/************************************************************************************************************************/
		const char* what() const override
		{
			return GetFullDescription().c_str();
		}

	protected:
		long m_Line;
		String m_TypeName;
		String m_Description;
		String m_Source;
		String m_File;
		mutable String m_FullDesc;
	};

	/************************************************************************************************************************/
	/**
	* @brief	Exception for signaling not implemented parts of the code.
	*/
	/************************************************************************************************************************/
	class NotImplementedException : public Exception
	{
	public:
		NotImplementedException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("NotImplementedException", inDescription, inSource, inFile, inLine)
		{
		
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Exception for signaling file system errors when file could not be found.
	*/
	/************************************************************************************************************************/
	class FileNotFoundException : public Exception
	{
	public:
		FileNotFoundException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("FileNotFoundException", inDescription, inSource, inFile, inLine)
		{
		
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Exception for signaling general IO errors.
	*
	* @note		An example being failed to open a file or a network connection.
	*/
	/************************************************************************************************************************/
	class IOException : public Exception
	{
	public:
		IOException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("IOException", inDescription, inSource, inFile, inLine)
		{
		
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Exception for signaling not currently executing code in not in a valid state.
	*/
	/************************************************************************************************************************/
	class InvalidStateException : public Exception
	{
	public:
		InvalidStateException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("InvalidStateException", inDescription, inSource, inFile, inLine)
		{
		
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Exception for signaling not some parameters you have provided are not valid.
	*/
	/************************************************************************************************************************/
	class InvalidParametersException : public Exception
	{
	public:
		InvalidParametersException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("InvalidParametersException", inDescription, inSource, inFile, inLine)
		{
		
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Exception for signaling an internal error, normally something that shouldn't have happened or
	* 			wasn't anticipated by the programmers of that system.
	*/
	/************************************************************************************************************************/
	class InternalErrorException : public Exception
	{
	public:
		InternalErrorException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("InternalErrorException", inDescription, inSource, inFile, inLine)
		{
		
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Exception for signaling an error in a rendering API.
	*/
	/************************************************************************************************************************/
	class RenderingAPIException : public Exception
	{
	public:
		RenderingAPIException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("RenderingAPIException", inDescription, inSource, inFile, inLine)
		{
		
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Exception for signaling an error in an unit test.
	*/
	/************************************************************************************************************************/
	class UnitTestException : public Exception
	{
	public:
		UnitTestException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("UnitTestException", inDescription, inSource, inFile, inLine) {}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Macro for throwing exceptions that will automatically fill out function name, file name and line number of
	*			the exception.
	*/
	/************************************************************************************************************************/
	//TODO: Add Crash handler functionality and the platform terminate data
#ifndef GE_EXCEPT
	#define GE_EXCEPT(type, desc)	\
	{								\
		static_assert((std::is_base_of<geEngineSDK::Exception, type>::value), "Invalid exception type (" #type ") for GE_EXCEPT macro. It needs to derive from geEngineSDK::Exception."); \
		throw type(desc, __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	}
#endif

#if GE_COMPILER == GE_COMPILER_MSVC
	#pragma warning( pop )
#endif
}
