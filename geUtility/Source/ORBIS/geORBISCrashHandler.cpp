/*****************************************************************************/
/**
 * @file    geWin32CrashHandler.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/10
 * @brief   Saves crash data and notifies when a crash occurs
 *
 * Saves crash data and notifies the user when a crash occurs
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtil.h"

namespace geEngineSDK {
  //static const char* s_MiniDumpName = "MiniDump.dmp";

  CrashHandler::CrashHandler() {}

  CrashHandler::~CrashHandler() {}

  void
  CrashHandler::reportCrash(const String& type,
                            const String& strDescription,
                            const String& strFunction,
                            const String& strFile,
                            uint32 nline) const {
    logErrorAndStackTrace(type, strDescription, strFunction, strFile, nline);
    saveCrashLog();
  }

  String
  CrashHandler::getCrashTimestamp() {
    return StringUtil::BLANK;
  }

  String
  CrashHandler::getStackTrace() {
    return StringUtil::BLANK;
  }
}
