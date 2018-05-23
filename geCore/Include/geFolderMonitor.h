/*****************************************************************************/
/**
 * @file    geFolderMonitor.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/12
 * @brief   Allows monitoring a file system folder for changes.
 *
 * Allows monitoring a file system folder for changes. Depending on the flags
 * set this monitor can notify you when file is changed / moved / renamed and
 * similar.
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
  /**
   * @brief Types of notifications we would like to receive when we start a
   *        FolderMonitor on a certain folder.
   */
  namespace FOLDER_CHANGE_BIT {
    enum E {
      /**
       * Called when file is created, moved or removed.
       */
      kFileName = 1 << 0,

      /**
       * Called when directory is created, moved or removed.
       */
      kDirName = 1 << 1,

      /**
       * Called when file is written to.
       */
      kFileWrite = 1 << 2
    };
  }

  typedef Flags<FOLDER_CHANGE_BIT::E> FolderChangeBits;
  GE_FLAGS_OPERATORS(FOLDER_CHANGE_BIT::E);

  class GE_CORE_EXPORT FolderMonitor
  {
    class FileNotifyInfo;

   public:
    struct Pimpl;
    struct FolderWatchInfo;

    FolderMonitor();
    ~FolderMonitor();

    /**
     * @brief Starts monitoring a folder at the specified path.
     * @param[in] folderPath  Absolute path to the folder you want to monitor.
     * @param[in] subdirectories  If true, provided folder and all of its
     *            subdirectories will be monitored for changes. Otherwise only
     *            the provided folder will be monitored.
     * @param[in] changeFilter  A set of flags you may OR together. Different
     *            notification events will trigger depending on which flags you
     *            set.
     */
    void
    startMonitor(const Path& folderPath,
                 bool subdirectories,
                 FolderChangeBits changeFilter);

    /**
     * @brief Stops monitoring the folder at the specified path.
     */
    void
    stopMonitor(const Path& folderPath);

    /**
     * @brief Stops monitoring all folders that are currently being monitored.
     */
    void
    stopMonitorAll();

    /**
     * @brief Triggers callbacks depending on events that occurred.
     *        Expected to be called once per frame.
     */
    void
    _update();

    /**
     * @brief Triggers when a file in the monitored folder is modified.
     *        Provides absolute path to the file.
     */
    Event<void(const Path&)> onModified;

    /**
     * @brief Triggers when a file / folder is added in the monitored folder.
     *        Provides absolute path to the file/folder.
     */
    Event<void(const Path&)> onAdded;

    /**
     * @brief Triggers when a file/folder is removed from the monitored folder.
     *        Provides absolute path to the file/folder.
     */
    Event<void(const Path&)> onRemoved;

    /**
     * @brief Triggers when a file / folder is renamed in the monitored folder.
     *        Provides absolute path with old and new names.
     */
    Event<void(const Path&, const Path&)> onRenamed;

    /**
     * @brief Returns private data, for use by internal helper classes and
     *        methods.
     */
    Pimpl*
    _getPrivateData() const {
      return m_privateData;
    }

   private:
    /**
     * @brief Worker method that monitors the IO ports for any modification
     *        notifications.
     */
    void
    workerThreadMain();

    /**
     * @brief Called by the worker thread whenever a modification notification is received.
     */
    void
    handleNotifications(FileNotifyInfo& notifyInfo,
                        FolderWatchInfo& watchInfo);

    Pimpl* m_privateData;
  };
}
