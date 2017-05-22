/*****************************************************************************/
/**
 * @file    gePlatformUtility.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/10
 * @brief   Provides access to various operating system specific utility
 *          functions.
 *
 * Provides access to various operating system specific utility functions.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

namespace geEngineSDK {

  /**
   * @brief Represents a MAC (Ethernet) address.
   */
  struct MACAddress
  {
    uint8 Value[6];
  };

  /**
   * @brief Possible type of platform file dialogs.
   */
  namespace FILE_DIALOG_TYPE {
    enum E {
      kOpenFile = 0x0,
      kOpenFolder = 0x1,
      kSave = 0x2,
      kMultiselect = 0x10000,
      kTypeMask = 0xFFFF
    };
  }

  /**
   * @brief Provides access to various operating system specific utility functions.
   */
  class GE_UTILITY_EXPORT PlatformUtility
  {
   public:
    /**
     * @brief Terminates the current process.
     * @param[in] force True if the process should be forcefully terminated with no cleanup.
     */
    static void
    terminate(bool force = false);

    /**
     * @brief Adds a string to the clipboard.
     * @note  Thread safe.
     */
    static void
    copyToClipboard(const WString& string);

    /**
     * @brief Reads a string from the clipboard and returns it. If there is no
     *        string in the clipboard it returns an empty string.
     * @note  Both wide and normal strings will be read, but normal strings
     *        will be converted to a wide string before returning.
     * @note  Thread safe.
     */
    static WString
    copyFromClipboard();

    /**
     * @brief Converts a keyboard key-code to a Unicode character.
     * @note  Normally this will output a single character, but it can happen
     *        it outputs multiple in case a accent/diacritic character could
     *        not be combined with the virtual key into a single character.
     */
    static WString
    keyCodeToUnicode(uint32 keyCode);

    /**
     * @brief Populates the provided buffer with a MAC address of the first available
     *        adapter, if one exists. If no adapters exist, returns false.
     */
    static bool
    getMACAddress(MACAddress& address);

    /**
     * @brief Creates a new universally unique identifier and returns it as a string.
     */
    static String
    generateUUID();

    /**
     * @brief Opens the provided file or folder using the default application
     *        for that file type, as specified by the operating system.
     * @param[in] path  Absolute path to the file or folder to open.
     */
    static void
    open(const Path& path);
  };
}
