/*****************************************************************************/
/**
 * @file    geUUID.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/29
 * @brief   Class that represents a universally unique identifier.
 *
 * Class that represents a universally unique identifier.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtil.h"
#include "geUUID.h"
#include "gePlatformUtility.h"
#include <chrono>

namespace geEngineSDK {

  std::array<uint8, 256>
  literalToHex() {
    std::array<uint8, 256> output;
    output.fill(255);

    output['0'] = 0;
    output['1'] = 1;
    output['2'] = 2;
    output['3'] = 3;
    output['4'] = 4;
    output['5'] = 5;
    output['6'] = 6;
    output['7'] = 7;
    output['8'] = 8;
    output['9'] = 9;
    output['a'] = 10;
    output['b'] = 11;
    output['c'] = 12;
    output['d'] = 13;
    output['e'] = 14;
    output['f'] = 15;

    return output;
  }

  static const std::array<char, 16> HEX_TO_LITERAL = {{'0', '1', '2', '3',
                                                       '4', '5', '6', '7',
                                                       '8', '9', 'a', 'b',
                                                       'c', 'd', 'e', 'f' }};
  static const std::array<uint8, 256> LITERAL_TO_HEX = literalToHex();
  UUID UUID::EMPTY;

  UUID::UUID(const String& uuid) {
    memset(m_data, 0, sizeof(m_data));

    if (uuid.size() < 36) {
      return;
    }

    uint32 idx = 0;

    //First group: 8 digits
    for (int32 i = 7; i >= 0; --i) {
      char charVal = static_cast<char>(tolower(uuid[idx++]));
      uint8 hexVal = LITERAL_TO_HEX[charVal];
      m_data[0] |= hexVal << (i * 4);
    }

    idx++;

    //Second group: 4 digits
    for (int32 i = 7; i >= 4; --i) {
      char charVal = static_cast<char>(tolower(uuid[idx++]));
      uint8 hexVal = LITERAL_TO_HEX[charVal];
      m_data[1] |= hexVal << (i * 4);
    }

    idx++;

    //Third group: 4 digits
    for (int32 i = 3; i >= 0; --i)
    {
      char charVal = static_cast<char>(tolower(uuid[idx++]));
      uint8 hexVal = LITERAL_TO_HEX[charVal];
      m_data[1] |= hexVal << (i * 4);
    }

    idx++;

    //Fourth group: 4 digits
    for (int32 i = 7; i >= 4; --i)
    {
      char charVal = static_cast<char>(tolower(uuid[idx++]));
      uint8 hexVal = LITERAL_TO_HEX[charVal];
      m_data[2] |= hexVal << (i * 4);
    }

    idx++;

    //Fifth group: 12 digits
    for (int32 i = 3; i >= 0; --i)
    {
      char charVal = static_cast<char>(tolower(uuid[idx++]));
      uint8 hexVal = LITERAL_TO_HEX[charVal];
      m_data[2] |= hexVal << (i * 4);
    }

    for (int32 i = 7; i >= 0; --i)
    {
      char charVal = static_cast<char>(tolower(uuid[idx++]));
      uint8 hexVal = LITERAL_TO_HEX[charVal];
      m_data[3] |= hexVal << (i * 4);
    }
  }

  String
  UUID::toString() const {
    uint8 output[36];
    uint32 idx = 0;

    //First group: 8 digits
    for (int32 i = 7; i >= 0; --i) {
      uint32 hexVal = (m_data[0] >> (i * 4)) & 0xF;
      output[idx++] = HEX_TO_LITERAL[hexVal];
    }

    output[idx++] = '-';

    //Second group: 4 digits
    for (int32 i = 7; i >= 4; --i) {
      uint32 hexVal = (m_data[1] >> (i * 4)) & 0xF;
      output[idx++] = HEX_TO_LITERAL[hexVal];
    }

    output[idx++] = '-';

    //Third group: 4 digits
    for (int32 i = 3; i >= 0; --i) {
      uint32 hexVal = (m_data[1] >> (i * 4)) & 0xF;
      output[idx++] = HEX_TO_LITERAL[hexVal];
    }

    output[idx++] = '-';

    //Fourth group: 4 digits
    for (int32 i = 7; i >= 4; --i) {
      uint32 hexVal = (m_data[2] >> (i * 4)) & 0xF;
      output[idx++] = HEX_TO_LITERAL[hexVal];
    }

    output[idx++] = '-';

    //Fifth group: 12 digits
    for (int32 i = 3; i >= 0; --i) {
      uint32 hexVal = (m_data[2] >> (i * 4)) & 0xF;
      output[idx++] = HEX_TO_LITERAL[hexVal];
    }

    for (int32 i = 7; i >= 0; --i) {
      uint32 hexVal = (m_data[3] >> (i * 4)) & 0xF;
      output[idx++] = HEX_TO_LITERAL[hexVal];
    }

    return String((const char*)output, 36);
  }

  UUID UUIDGenerator::generateRandom() {
    return PlatformUtility::generateUUID();
  }
}
