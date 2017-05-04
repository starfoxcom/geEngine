/********************************************************************/
/**
 * @file   geDebug.cpp
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/09/19
 * @brief  Utility class providing various debug functionality.
 *
 * Utility class providing various debug functionality.
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#include "geDebug.h"
#include "geLog.h"
#include "geException.h"
#include "geBitmapWriter.h"
#include "geFileSystem.h"
#include "geDataStream.h"

#if GE_PLATFORM == GE_PLATFORM_WIN32 && GE_COMPILER == GE_COMPILER_MSVC
#	include <windows.h>
#	include <iostream>

	void LogToIDEConsole(const geEngineSDK::String& message)
	{
		OutputDebugString( message.c_str() );
		OutputDebugString( "\n" );

		//Also default output in case we're running without debugger attached
		std::cout << message << std::endl;
	}
#else
	void LogToIDEConsole(const geEngineSDK::String& message)
	{
		// Do nothing
	}
#endif

namespace geEngineSDK
{
	void Debug::LogDebug(const String& msg)
	{
		m_Log.LogMsg(msg, (uint32)DEBUG_CHANNEL::kDebug);
		LogToIDEConsole(msg);
	}

	void Debug::LogWarning(const String& msg)
	{
		m_Log.LogMsg(msg, (uint32)DEBUG_CHANNEL::kWarning);
		LogToIDEConsole(msg);
	}

	void Debug::LogError(const String& msg)
	{
		m_Log.LogMsg(msg, (uint32)DEBUG_CHANNEL::kError);
		LogToIDEConsole(msg);
	}

	void Debug::LogMessage(const String& msg, uint32 channel)
	{
		m_Log.LogMsg(msg, channel);
		LogToIDEConsole(msg);
	}

	void Debug::WriteAsBMP(uint8* rawPixels, uint32 bytesPerPixel, uint32 width, uint32 height, const Path& filePath, bool overwrite) const
	{
		if( FileSystem::IsFile(filePath) )
		{
			if( overwrite )
			{
				FileSystem::Remove(filePath);
			}
			else
			{
				GE_EXCEPT(FileNotFoundException, "File already exists at specified location: " + filePath.toString());
			}
		}

		DataStreamPtr ds = FileSystem::CreateAndOpenFile(filePath);

		uint32 bmpDataSize = BitmapWriter::GetBMPSize(width, height, bytesPerPixel);
		uint8* bmpBuffer = ge_newN<uint8>(bmpDataSize);

		BitmapWriter::RawPixelsToBMP(rawPixels, bmpBuffer, width, height, bytesPerPixel);

		ds->Write(bmpBuffer, bmpDataSize);
		ds->Close();

		ge_deleteN(bmpBuffer, bmpDataSize);
	}

	void Debug::_TriggerCallbacks()
	{
		LogEntry entry;
		while( m_Log.GetUnreadEntry(entry) )
		{
			OnLogEntryAdded(entry);
		}

		uint64 hash = m_Log.GetHash();
		if( m_LogHash != hash )
		{
			OnLogModified();
			m_LogHash = hash;
		}
	}

	void Debug::SaveLog(const Path& path) const
	{
		static const char* style = 
R"(<LINK REL="stylesheet" TYPE="text/css" HREF="System/Styles/Debug.css">)";

		static const char* htmlPreStyleHeader =
R"(<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0 Transitional//EN'>
<HTML>
	<HEAD>
		<TITLE>geEngine: Information Log</TITLE>
		<LINK REL='SHORTCUT ICON' HREF=''>)";

		static const char* htmlPostStyleHeader =
R"(		<TITLE>geEngine Log</TITLE>
	</HEAD>
	<BODY>
		<H1>geEngine Log</H1>
		<TABLE BORDER="1" cellpadding="1" cellspacing="1">
			<THEAD>
				<TR>
					<TH SCOPE="col" STYLE="width:60px">Type</TH>
					<TH SCOPE="col">Description</TH>
				</TR>
			</THEAD>
			<TBODY>)";

		static const char* htmlFooter =
R"(			</TBODY>
		</TABLE>
	</BODY>
</HTML>)";

		StringStream stream;
		stream << htmlPreStyleHeader;
		stream << style;
		stream << htmlPostStyleHeader;

		bool alternate = false;
		Vector<LogEntry> entries = m_Log.GetAllEntries();
		for( auto& entry : entries )
		{
			String channelName;
			if( entry.GetLogChannel() == (uint32)DEBUG_CHANNEL::kError || entry.GetLogChannel() == (uint32)DEBUG_CHANNEL::kCompilerError )
			{
				if( !alternate )
				{
					stream << R"(				<TR CLASS="error-row">)" << std::endl;
				}
				else
				{
					stream << R"(				<TR CLASS="error-alt-row">)" << std::endl;
				}

				stream << R"(					<TD>Error</TD>)" << std::endl;
			}
			else if( entry.GetLogChannel() == (uint32)DEBUG_CHANNEL::kWarning || entry.GetLogChannel() == (uint32)DEBUG_CHANNEL::kCompilerWarning )
			{
				if (!alternate)
					stream << R"(				<TR CLASS="warn-row">)" << std::endl;
				else
					stream << R"(				<TR CLASS="warn-alt-row">)" << std::endl;

				stream << R"(					<TD>Warning</TD>)" << std::endl;
			}
			else
			{
				if (!alternate)
					stream << R"(				<TR CLASS="debug-row">)" << std::endl;
				else
					stream << R"(				<TR CLASS="debug-alt-row">)" << std::endl;

				stream << R"(					<TD>Debug</TD>)" << std::endl;
			}

			String parsedMessage = StringUtil::replaceAll(entry.GetLogMessage(), "\n", "<BR>\n");

			stream << R"(					<TD>)" << parsedMessage << "</TD>" << std::endl;
			stream << R"(				</TR>)" << std::endl;

			alternate = !alternate;
		}

		stream << htmlFooter;

		DataStreamPtr fileStream = FileSystem::CreateAndOpenFile(path);
		fileStream->WriteString(stream.str());
	}

	GE_UTILITY_EXPORT Debug& g_Debug()
	{
		static Debug debug;
		return debug;
	}
}
