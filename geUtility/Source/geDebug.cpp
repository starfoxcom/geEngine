/*****************************************************************************/
/**
 * @file    geDebug.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/09/19
 * @brief   Utility class providing various debug functionality.
 *
 * Utility class providing various debug functionality.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geDebug.h"
#include "geLog.h"
#include "geException.h"
#include "geBitmapWriter.h"
#include "geFileSystem.h"
#include "geDataStream.h"

#if GE_PLATFORM == GE_PLATFORM_WIN32 && GE_COMPILER == GE_COMPILER_MSVC
#	include <windows.h>
#	include <iostream>

void
logToIDEConsole(const geEngineSDK::String& message) {
  OutputDebugString(message.c_str());
  OutputDebugString("\n");

  //Also default output in case we're running without debugger attached
  std::cout << message << std::endl;
}
#else
void
logToIDEConsole(const geEngineSDK::String& message) {
  // Do nothing
  GE_UNREFERENCED_PARAMETER(message);
}
#endif

namespace geEngineSDK {
  void
  Debug::logDebug(const String& msg) {
    m_log.logMsg(msg, (uint32)DEBUG_CHANNEL::kDebug);
    logToIDEConsole(msg);
  }

  void
  Debug::logWarning(const String& msg) {
    m_log.logMsg(msg, (uint32)DEBUG_CHANNEL::kWarning);
    logToIDEConsole(msg);
  }

  void
  Debug::logError(const String& msg) {
    m_log.logMsg(msg, (uint32)DEBUG_CHANNEL::kError);
    logToIDEConsole(msg);
  }

  void
  Debug::logMessage(const String& msg, uint32 channel) {
    m_log.logMsg(msg, channel);
    logToIDEConsole(msg);
  }

  void
  Debug::writeAsBMP(uint8* rawPixels,
                    uint32 bytesPerPixel,
                    uint32 width,
                    uint32 height,
                    const Path& filePath,
                    bool overwrite) const {
    if (FileSystem::isFile(filePath)) {
      if (overwrite) {
        FileSystem::remove(filePath);
      }
      else {
        GE_EXCEPT(FileNotFoundException,
                  "File already exists at specified location: " + filePath.toString());
      }
    }

    DataStreamPtr ds = FileSystem::createAndOpenFile(filePath);

    uint32 bmpDataSize = BitmapWriter::getBMPSize(width, height, bytesPerPixel);
    uint8* bmpBuffer = ge_newN<uint8>(bmpDataSize);

    BitmapWriter::rawPixelsToBMP(rawPixels, bmpBuffer, width, height, bytesPerPixel);

    ds->write(bmpBuffer, bmpDataSize);
    ds->close();

    ge_deleteN(bmpBuffer, bmpDataSize);
  }

  void
  Debug::_triggerCallbacks() {
    LogEntry entry;
    while (m_log.getUnreadEntry(entry)) {
      onLogEntryAdded(entry);
    }

    uint64 hash = m_log.getHash();
    if (m_logHash != hash) {
      onLogModified();
      m_logHash = hash;
    }
  }

  void
  Debug::saveLog(const Path& path) const {
    static const char* style =
      R"(<link rel="stylesheet" type="text/css" href="../css/debug.css">)";

    static const char* htmlPreStyleHeader = 
R"(<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0 Transitional//EN'>
<html>
  <head>
    <title>geEngine: Information Log</title>
      <link rel='shortcut icon' href=''>)";

    static const char* htmlPostStyleHeader = R"(
  </head>
  <body>
    <h1>geEngine Log</h1>
    <div class="wrapper">
      <div class="table">
        <div class="row header blue">
          <div class="cell"> Type </div>
          <div class="cell"> Description </div>
        </div>)";

    static const char* htmlFooter = R"(
        </div>
      </div>
    </body>
</html>)";

    StringStream stream;
    stream << htmlPreStyleHeader;
    stream << style;
    stream << htmlPostStyleHeader;

    Vector<LogEntry> entries = m_log.getAllEntries();
    for (auto& entry : entries) {
      String channelName;
      uint32 iLogChannel = entry.getLogChannel();
      if (iLogChannel == static_cast<uint32>(DEBUG_CHANNEL::kError) ||
          iLogChannel == static_cast<uint32>(DEBUG_CHANNEL::kCompilerError)) {
        stream << R"(<div class="row red">)" << std::endl;
        stream << R"(<div class="cell">Error</div>)" << std::endl;
      }
      else if (iLogChannel == static_cast<uint32>(DEBUG_CHANNEL::kWarning) ||
               iLogChannel == static_cast<uint32>(DEBUG_CHANNEL::kCompilerWarning)) {  
        stream << R"(<div class="row yellow">)" << std::endl;
        stream << R"(<div class="cell">Warning</div>)" << std::endl;
      }
      else
      {
        stream << R"(<div class="row green">)" << std::endl;
        stream << R"(<div class="cell">Debug</div>)" << std::endl;
      }

      String parsedMessage = StringUtil::replaceAll(entry.getLogMessage(), "\n", "<BR>\n");

      stream << R"(<div class="cell">)" << parsedMessage << "</div>" << std::endl;
      stream << R"(</div>)" << std::endl;
    }

    stream << htmlFooter;

    DataStreamPtr fileStream = FileSystem::createAndOpenFile(path);
    fileStream->writeString(stream.str());
  }

  GE_UTILITY_EXPORT Debug& g_Debug() {
    static Debug debug;
    return debug;
  }
}
