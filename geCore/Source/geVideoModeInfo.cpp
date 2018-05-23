/*****************************************************************************/
/**
 * @file    geVideoModeInfo.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/11
 * @brief   Contains information about available output devices
 *
 * Contains information about available output devices (for example monitor)
 * and their video modes.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geVideoModeInfo.h"

namespace geEngineSDK {
  VideoMode::VideoMode(uint32 width,
                       uint32 height,
                       float refreshRate,
                       uint32 outputIdx)
    : m_width(width),
      m_height(height),
      m_refreshRate(refreshRate),
      m_outputIdx(outputIdx),
      m_isCustom(true)
  {}

  bool
  VideoMode::operator==(const VideoMode& other) const {
    return m_width == other.m_width &&
           m_height == other.m_height &&
           m_outputIdx == other.m_outputIdx &&
           m_refreshRate == other.m_refreshRate;
  }

  VideoOutputInfo::~VideoOutputInfo() {
    for (auto& videoMode : m_videoModes) {
      ge_delete(videoMode);
    }

    if (nullptr != m_desktopVideoMode) {
      ge_delete(m_desktopVideoMode);
    }
  }

  VideoModeInfo::~VideoModeInfo() {
    for (auto& output : m_outputs) {
      ge_delete(output);
    }
  }
}
