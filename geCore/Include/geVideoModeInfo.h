/*****************************************************************************/
/**
 * @file    geVideoModeInfo.h
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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"

namespace geEngineSDK {
  class GE_CORE_EXPORT VideoMode
  {
   public:
    VideoMode() = default;

    /**
     * @brief Creates a new video mode.
     * @param[in] width       Width of the frame buffer in pixels.
     * @param[in] height      Height of the frame buffer in pixels.
     * @param[in] refreshRate How often should the output device refresh
     *            the output image in hertz.
     * @param[in] outputIdx   Output index of the output device. Normally this
     *            means output monitor. 0th index always represents the primary
     *            device while order of others is undefined.
     */
    VideoMode(uint32 width,
              uint32 height,
              float refreshRate = 60.0f,
              uint32 outputIdx = 0);

    virtual ~VideoMode() = default;

    bool
    operator==(const VideoMode& other) const;

    /**
     * @brief Width of the front/back buffer in pixels.
     */
    uint32
    getWidth() const {
      return m_width;
    }

    /**
     * @brief Height of the front/back buffer in pixels.
     */
    uint32
    getHeight() const {
      return m_height;
    }

    /**
     * @brief Returns a refresh rate in hertz.
     */
    virtual float
    getRefreshRate() const {
      return m_refreshRate;
    }

    /**
     * @brief Returns information about the parent output.
     */
    uint32
    getOutputIdx() const {
      return m_outputIdx;
    }

    /**
     * @brief Determines was video mode user created or provided by the API/OS.
     *        API/OS created video modes can contain additional information
     *        that allows the video mode to be used more accurately and you
     *        should use them when possible.
     */
    bool
    isCustom() const {
      return m_isCustom;
    }

   protected:
    uint32 m_width = 1280;
    uint32 m_height = 720;
    float m_refreshRate = 60.0f;
    uint32 m_outputIdx = 0;
    bool m_isCustom = true;
  };

  /**
   * @brief Contains information about a video output device, including a list
   *        of all available video modes.
   */
  class GE_CORE_EXPORT VideoOutputInfo
  {
   public:
    VideoOutputInfo() = default;
    virtual ~VideoOutputInfo();

    VideoOutputInfo(const VideoOutputInfo&) = delete; //Make non-copyable
    
    VideoOutputInfo&
    operator=(const VideoOutputInfo&) = delete; //Make non-copyable

    /**
     * @brief Name of the output device.
     */
    const String&
    getName() const {
      return m_name;
    }

    /**
     * @brief Number of available video modes for this output.
     */
    uint32
    getNumVideoModes() const {
      return (uint32)m_videoModes.size();
    }

    /**
     * @brief Returns video mode at the specified index.
     */
    const VideoMode&
    getVideoMode(uint32 idx) const {
      return *m_videoModes.at(idx);
    }

    /**
     * @brief Returns the video mode currently used by the desktop.
     */
    const VideoMode&
    getDesktopVideoMode() const {
      return *m_desktopVideoMode;
    }

   protected:
    String m_name;
    Vector<VideoMode*> m_videoModes;
    VideoMode* m_desktopVideoMode = nullptr;
  };

  /**
   * @brief Contains information about available output devices (for example
   *        monitor) and their video modes.
   */
  class GE_CORE_EXPORT VideoModeInfo
  {
   public:
    VideoModeInfo() = default;
    virtual ~VideoModeInfo();

    VideoModeInfo(const VideoModeInfo&) = delete; //Make non-copyable

    VideoModeInfo&
    operator=(const VideoModeInfo&) = delete; // Make non-copyable

    /**
     * @brief Returns the number of available output devices.
     */
    uint32
    getNumOutputs() const {
      return (uint32)m_outputs.size();
    }

    /**
     * @brief Returns video mode information about a specific output device.
     *        0th index always represents the primary device while order of
     *        others is undefined.
     */
    const VideoOutputInfo&
    getOutputInfo(uint32 idx) const {
      return *m_outputs[idx];
    }

   protected:
    Vector<VideoOutputInfo*> m_outputs;
  };
}
