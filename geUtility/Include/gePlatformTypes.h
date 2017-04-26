/*****************************************************************************/
/**
 * @file    gePlatformTypes.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/09
 * @brief   Define the basic platform types
 *
 * Define the basic platform type variables. For porting, this might
 * be the place to start
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePlatformDefines.h"

#if GE_PLATFORM == GE_PLATFORM_PS4
# include <scebase.h>
#endif

namespace geEngineSDK {
  /***************************************************************************/
  /**
   * Basic unsigned types
   */
  /***************************************************************************/
  typedef unsigned char     uint8;  //8-bit  unsigned.
  typedef unsigned short    uint16; //16-bit unsigned.
  typedef unsigned int      uint32; //32-bit unsigned.
#if GE_COMPILER == GE_COMPILER_MSVC
  typedef unsigned __int64  uint64; //64-bit unsigned (Visual Studio version).
#else
  typedef unsigned long     uint64; //64-bit unsigned (Generic Compiler version).
#endif

	/***************************************************************************/
  /**
   * Basic signed types
   */
  /***************************************************************************/
  typedef signed char       int8;   //8-bit  signed.
  typedef signed short      int16;  //16-bit signed.
  typedef signed int        int32;  //32-bit signed.
#if GE_COMPILER == GE_COMPILER_MSVC
  typedef signed __int64    int64;  //64-bit signed (Visual Studio version).
#else
  typedef signed long       int64;  //64-bit signed (Generic Compiler version).
#endif

  /***************************************************************************/
  /**
   * @class   QWord
   * @brief   128 bits variable type
   */
  /***************************************************************************/
  MS_ALIGN(16) class QWord
  {
    /**
     * Constructor
     */
  public:
    QWord() : m_lower(0), m_upper(0) {}
    QWord(bool from)   : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    QWord(uint8 from)  : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    QWord(int8 from)   : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    QWord(uint16 from) : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    QWord(int16 from)  : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    QWord(uint32 from) : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    QWord(int32 from)  : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    QWord(uint64 from) : m_lower(from),                      m_upper(0) {}
    QWord(int64 from)  : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    QWord(float from)  : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    QWord(double from) : m_lower(static_cast<uint64>(from)), m_upper(0) {}

    /**
     * @brief	  The cast operator for casting/truncating to 64 bits.
     * @returns The lower 64 bits of the value.
     */
    operator int64() const { return static_cast<uint64>(m_lower); }

   public:
    uint64 m_lower; //The lower 64 bits of the 128 bit integer.
    int64	m_upper;  //The upper 64 bits of the 128 bit integer.
	}GCC_ALIGN(16);
	
  typedef QWord int128;   //Signed 128 bit integer.
  typedef QWord uint128;  //Unsigned 128 bit integer.

  /***************************************************************************/
  /**
   * Character types
   */
  /***************************************************************************/
#if GE_COMPILER == GE_COMPILER_MSVC || GE_PLATFORM == GE_PLATFORM_PS4
  typedef wchar_t         WCHAR;    //Wide Character (used by Visual Studio)
#else
  typedef unsigned short  WCHAR;    //Wide Character (Any other compiler)
#endif
  typedef char            ANSICHAR; //ANSI character type
  typedef WCHAR           UNICHAR;  //UNICODE character type

  /***************************************************************************/
  /**
   * NULL data type
   */
  /***************************************************************************/
	typedef int32				TYPE_OF_NULL;

  /***************************************************************************/
  /**
   * SIZE_T is an architecture dependant data type
   */
  /***************************************************************************/
#if ((GE_ARCH_TYPE == GE_ARCHITECTURE_x86_64) && (GE_COMPILER == GE_COMPILER_MSVC))
	typedef unsigned __int64	SIZE_T;
#else
	typedef unsigned long		SIZE_T;
#endif
}
