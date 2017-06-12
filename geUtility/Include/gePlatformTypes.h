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
#include <stdint.h>
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
  typedef uint8_t   uint8;  //8-bit  unsigned.
  typedef uint16_t  uint16; //16-bit unsigned.
  typedef uint32_t  uint32; //32-bit unsigned.
  typedef uint64_t  uint64; //64-bit unsigned.

	/***************************************************************************/
  /**
   * Basic signed types
   */
  /***************************************************************************/
  typedef int8_t    int8;   //8-bit  signed.
  typedef int16_t   int16;  //16-bit signed.
  typedef int32_t   int32;  //32-bit signed.
  typedef int64_t   int64;  //64-bit signed.

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
    explicit QWord(bool from)   : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    explicit QWord(uint8 from)  : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    explicit QWord(int8 from)   : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    explicit QWord(uint16 from) : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    explicit QWord(int16 from)  : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    explicit QWord(uint32 from) : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    explicit QWord(int32 from)  : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    explicit QWord(uint64 from) : m_lower(from),                      m_upper(0) {}
    explicit QWord(int64 from)  : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    explicit QWord(float from)  : m_lower(static_cast<uint64>(from)), m_upper(0) {}
    explicit QWord(double from) : m_lower(static_cast<uint64>(from)), m_upper(0) {}

    /**
     * @brief   The cast operator for casting/truncating to 64 bits.
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
  typedef int32           TYPE_OF_NULL;

  /***************************************************************************/
  /**
   * SIZE_T is an architecture dependant data type
   */
  /***************************************************************************/
  typedef size_t SIZE_T;
}
