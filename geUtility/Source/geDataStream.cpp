/********************************************************************/
/**
 * @file   geDataStream.cpp
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/09/19
 * @brief  General purpose class used for encapsulating the reading
 *			and writing of data
 *
 * General purpose class used for encapsulating the reading and writing
 * of data from and to various sources using a common interface.
 *
 * @bug	  -std::consume_header seems to be ignored so I manually
 *		  remove the header.
 *
 *		  -Regardless of not providing the std::little_endian flag it
 *		  seems that is how the data is read so I manually flip it.
 */
/********************************************************************/
#include "geDataStream.h"
#include "geDebug.h"
#include "geException.h"
#include <codecvt>

namespace geEngineSDK
{
	const uint32 DataStream::StreamTempSize = 128;

	/************************************************************************************************************************/
	/**
	* @brief	Checks does the provided buffer has an UTF32 byte order mark in little endian order.
	*/
	/************************************************************************************************************************/
	bool IsUTF32LE(const uint8* buffer)
	{
		return buffer[0] == 0xFF && buffer[1] == 0xFE && buffer[2] == 0x00 && buffer[3] == 0x00;
	}

	/************************************************************************************************************************/
	/**
	* @brief	Checks does the provided buffer has an UTF32 byte order mark in big endian order.
	*/
	/************************************************************************************************************************/
	bool IsUTF32BE(const uint8* buffer)
	{
		return buffer[0] == 0x00 && buffer[1] == 0x00 && buffer[2] == 0xFE && buffer[3] == 0xFF;
	}

	/************************************************************************************************************************/
	/**
	* @brief	Checks does the provided buffer has an UTF16 byte order mark in little endian order.
	*/
	/************************************************************************************************************************/
	bool IsUTF16LE(const uint8* buffer)
	{
		return buffer[0] == 0xFF && buffer[1] == 0xFE;
	}

	/************************************************************************************************************************/
	/**
	* @brief	Checks does the provided buffer has an UTF16 byte order mark in big endian order.
	*/
	/************************************************************************************************************************/
	bool IsUTF16BE(const uint8* buffer)
	{
		return buffer[0] == 0xFE && buffer[1] == 0xFF;
	}

	/************************************************************************************************************************/
	/**
	* @brief	Checks does the provided buffer has an UTF8 byte order mark.
	*/
	/************************************************************************************************************************/
	bool IsUTF8(const uint8* buffer)
	{
		return (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF);
	}

	template <typename T> DataStream& DataStream::operator >> (T& val)
	{
		Read(static_cast<void*>(&val), sizeof(T));
		return *this;
	}

	void DataStream::WriteString(const String& string, STRING_ENCODING::E encoding)
	{
		if( encoding == STRING_ENCODING::UTF16 )
		{
			const std::codecvt_mode convMode = (std::codecvt_mode)(std::generate_header);
			typedef std::codecvt_utf8_utf16<char, 1114111, convMode> UTF8ToUTF16Conv;
			std::wstring_convert<UTF8ToUTF16Conv, char> conversion("?");

			std::string encodedString = conversion.from_bytes(string.c_str());
			Write(encodedString.data(), encodedString.length());
		}
		else
		{
			Write(string.data(), string.length());
		}
	}

	void DataStream::WriteString(const WString& string, STRING_ENCODING::E encoding)
	{
		if( encoding == STRING_ENCODING::UTF16 )
		{
			const std::codecvt_mode convMode = (std::codecvt_mode)(std::generate_header | std::little_endian);
			typedef std::codecvt_utf16<wchar_t, 1114111, convMode> WCharToUTF16Conv;
			std::wstring_convert<WCharToUTF16Conv, wchar_t> conversion("?");

			std::string encodedString = conversion.to_bytes(string.c_str());
			Write(encodedString.data(), encodedString.length());
		}
		else
		{
			const std::codecvt_mode convMode = (std::codecvt_mode)(std::generate_header);
			typedef std::codecvt_utf8<wchar_t, 1114111, convMode> WCharToUTF8Conv;
			std::wstring_convert<WCharToUTF8Conv, wchar_t> conversion("?");

			std::string encodedString = conversion.to_bytes(string.c_str());
			Write(encodedString.data(), encodedString.length());
		}
	}

	String DataStream::GetAsString()
	{
		//Read the entire buffer - ideally in one read, but if the size of the buffer is unknown, do multiple fixed size reads.
		SIZE_T bufSize = (m_Size > 0 ? m_Size : 4096);
		std::stringstream::char_type* tempBuffer = (std::stringstream::char_type*)ge_alloc((uint32)bufSize);

		//Ensure read from begin of stream
		Seek(0);

		std::stringstream result;
		while( !IsEOF() )
		{
			SIZE_T numReadBytes = Read(tempBuffer, bufSize);
			result.write(tempBuffer, numReadBytes);
		}

		free(tempBuffer);
		std::string string = result.str();

		uint32 readBytes = (uint32)string.size();
		if( readBytes >= 4 )
		{
			if( IsUTF32LE( (uint8*)string.data() ))
			{//Little Endian 32 bits
				const std::codecvt_mode convMode = (std::codecvt_mode)(std::consume_header | std::little_endian);
				typedef std::codecvt_utf8<uint32, 1114111, convMode> utf8utf32;

				std::wstring_convert<utf8utf32, uint32> conversion("?");
				uint32* start = (uint32*)string.data();
				uint32* end = (start + (string.size() - 1) / 4);

				return conversion.to_bytes(start, end).c_str();
			}
			else if( IsUTF32BE((uint8*)string.data() ))
			{//Big Endian 32 bits
				const std::codecvt_mode convMode = (std::codecvt_mode)(std::consume_header);
				typedef std::codecvt_utf8<uint32, 1114111, convMode> utf8utf32;

				std::wstring_convert<utf8utf32, uint32> conversion("?");
				uint32* start = (uint32*)string.data();
				uint32* end = (start + (string.size() - 1) / 4);

				return conversion.to_bytes(start, end).c_str();
			}
		}

		if( readBytes >= 3 )
		{
			if( IsUTF8((uint8*)string.data()) )
			{
				return string.c_str() + 3;
			}
		}

		if( readBytes >= 2 )
		{
			if( IsUTF16LE((uint8*)string.data() ))
			{//Little Endian 16 bits
				const std::codecvt_mode convMode = (std::codecvt_mode)(std::little_endian);
				typedef std::codecvt_utf8_utf16<uint16, 1114111, convMode> utf8utf16;

				std::wstring_convert<utf8utf16, uint16> conversion("?");
				uint16* start = (uint16*)(string.c_str() + 2); //Bug?: std::consume_header seems to be ignored so I manually remove the header

				return conversion.to_bytes(start).c_str();
			}
			else if( IsUTF16BE((uint8*)string.data() ))
			{//Big Endian 16 bits
				const std::codecvt_mode convMode = (std::codecvt_mode)(0);
				typedef std::codecvt_utf8_utf16<uint16, 1114111, convMode> utf8utf16;

				//Bug?: Regardless of not providing the std::little_endian flag it seems that is how the data is read so I manually flip it
				uint32 numChars = (uint32)(string.size() - 2) / 2;
				for(uint32 i=0; i<numChars; ++i)
				{
					std::swap(string[i*2+0], string[i*2+1]);
				}

				std::wstring_convert<utf8utf16, uint16> conversion("?");
				uint16* start = (uint16*)(string.c_str() + 2); //Bug?: std::consume_header seems to be ignored so I manually remove the header

				return conversion.to_bytes(start).c_str();
			}
		}

		return string.c_str();
	}

	WString DataStream::GetAsWString()
	{
		//Read the entire buffer - ideally in one read, but if the size of the buffer is unknown, do multiple fixed size reads.
		SIZE_T bufSize = (m_Size > 0 ? m_Size : 4096);
		std::stringstream::char_type* tempBuffer = (std::stringstream::char_type*)ge_alloc((uint32)bufSize);

		//Ensure read from begin of stream
		Seek(0);

		std::stringstream result;
		while( !IsEOF() )
		{
			SIZE_T numReadBytes = Read(tempBuffer, bufSize);
			result.write(tempBuffer, numReadBytes);
		}

		free(tempBuffer);
		std::string string = result.str();

		uint32 readBytes = (uint32)string.size();
		if( readBytes >= 4 )
		{
			if( IsUTF32LE((uint8*)string.data() ))
			{//Little Endian 32 bits
				//Not supported, maybe throw an exception?
			}
			else if( IsUTF32BE((uint8*)string.data() ))
			{//Big Endian 32 bits
				//Not supported, maybe throw an exception?
			}
		}

		if( readBytes >= 3 )
		{
			if( IsUTF8((uint8*)string.data() ))
			{
				const std::codecvt_mode convMode = (std::codecvt_mode)(std::consume_header);
				typedef std::codecvt_utf8<wchar_t, 1114111, convMode> wcharutf8;

				std::wstring_convert<wcharutf8> conversion("?");
				return conversion.from_bytes(string).c_str();
			}
		}

		if( readBytes >= 2 )
		{
			if( IsUTF16LE((uint8*)string.data() ))
			{//Little Endian 16 bits
				const std::codecvt_mode convMode = (std::codecvt_mode)(std::consume_header | std::little_endian);
				typedef std::codecvt_utf16<wchar_t, 1114111, convMode> wcharutf16;

				std::wstring_convert<wcharutf16> conversion("?");
				return conversion.from_bytes(string).c_str();
			}
			else if( IsUTF16BE((uint8*)string.data() ))
			{//Big Endian 32 bits
				const std::codecvt_mode convMode = (std::codecvt_mode)(std::consume_header);
				typedef std::codecvt_utf16<wchar_t, 1114111, convMode> wcharutf16;

				std::wstring_convert<wcharutf16> conversion("?");
				return conversion.from_bytes(string).c_str();
			}
		}

		{
			const std::codecvt_mode convMode = (std::codecvt_mode)(std::consume_header);
			typedef std::codecvt_utf8<wchar_t, 1114111, convMode> wcharutf8;

			std::wstring_convert<wcharutf8> conversion("?");
			return conversion.from_bytes(string).c_str();
		}
	}

	MemoryDataStream::MemoryDataStream(void* memory, SIZE_T inSize) : DataStream(ACCESS_MODE::READ | ACCESS_MODE::WRITE), m_Data(nullptr)
	{
		m_Data = m_Pos = static_cast<uint8*>(memory);
		m_Size = inSize;
		m_End = m_Data + m_Size;

		GE_ASSERT(m_End >= m_Pos);
	}

	MemoryDataStream::MemoryDataStream(DataStream& sourceStream) :  DataStream(ACCESS_MODE::READ | ACCESS_MODE::WRITE), m_Data(nullptr)
	{
		//Copy data from incoming stream
		m_Size = sourceStream.Size();

		m_Data = (uint8*)ge_alloc((uint32)m_Size);
		m_Pos = m_Data;
		m_End = m_Data + sourceStream.Read(m_Data, m_Size);

		GE_ASSERT(m_End >= m_Pos);
	}

	MemoryDataStream::MemoryDataStream(const DataStreamPtr& sourceStream) : DataStream(ACCESS_MODE::READ | ACCESS_MODE::WRITE), m_Data(nullptr)
	{
		//Copy data from incoming stream
		m_Size = sourceStream->Size();

		m_Data = (uint8*)ge_alloc((uint32)m_Size);
		m_Pos = m_Data;
		m_End = m_Data + sourceStream->Read(m_Data, m_Size);

		GE_ASSERT(m_End >= m_Pos);
	}

	MemoryDataStream::~MemoryDataStream()
	{
		Close();
	}

	SIZE_T MemoryDataStream::Read(void* buf, SIZE_T count)
	{
		SIZE_T cnt = count;

		if( m_Pos + cnt > m_End )
		{
			cnt = m_End - m_Pos;
		}
		if( cnt == 0 )
		{
			return 0;
		}

		GE_ASSERT(cnt <= count);

		memcpy(buf, m_Pos, cnt);
		m_Pos += cnt;

		return cnt;
	}

	SIZE_T MemoryDataStream::Write(const void* buf, SIZE_T count)
	{
		SIZE_T written = 0;
		if( IsWriteable() )
		{
			written = count;

			if( m_Pos + written > m_End )
			{
				written = m_End - m_Pos;
			}
			if( written == 0 )
			{
				return 0;
			}

			memcpy(m_Pos, buf, written);
			m_Pos += written;
		}

		return written;
	}

	void MemoryDataStream::Skip(SIZE_T count)
	{
		SIZE_T newpos = (SIZE_T)((m_Pos - m_Data) + count);
		GE_ASSERT(m_Data + newpos <= m_End);

		m_Pos = m_Data + newpos;
	}

	void MemoryDataStream::Seek(SIZE_T pos)
	{
		GE_ASSERT(m_Data + pos <= m_End);
		m_Pos = m_Data + pos;
	}

	SIZE_T MemoryDataStream::Tell() const
	{
		return m_Pos - m_Data;
	}

	bool MemoryDataStream::IsEOF() const
	{
		return m_Pos >= m_End;
	}

	void MemoryDataStream::Close()
	{
		if( m_Data != nullptr )
		{
			ge_free(m_Data);
			m_Data = nullptr;
		}
	}

	FileDataStream::FileDataStream(std::shared_ptr<std::ifstream> s, bool freeOnClose)
		: DataStream(ACCESS_MODE::READ), 
		 m_pInStream(s), 
		 m_pFStreamRO(s), 
		 m_pFStream(0), 
		 m_FreeOnClose(freeOnClose)
	{
		m_pInStream->seekg(0, std::ios_base::end);
		m_Size = (SIZE_T)m_pInStream->tellg();
		m_pInStream->seekg(0, std::ios_base::beg);

		DetermineAccess();
	}

	FileDataStream::FileDataStream(std::shared_ptr<std::ifstream> s, SIZE_T inSize, bool freeOnClose) 
		: DataStream(ACCESS_MODE::READ), 
		 m_pInStream(s), 
		 m_pFStreamRO(s), 
		 m_pFStream(0), 
		 m_FreeOnClose(freeOnClose)
	{
		m_Size = inSize;

		DetermineAccess();
	}

	FileDataStream::FileDataStream(std::shared_ptr<std::fstream> s, bool freeOnClose)
		: DataStream(ACCESS_MODE::READ | ACCESS_MODE::WRITE), 
		 m_pInStream(s), 
		 m_pFStreamRO(0), 
		 m_pFStream(s), 
		 m_FreeOnClose(freeOnClose)
	{
		m_pInStream->seekg(0, std::ios_base::end);
		m_Size = (SIZE_T)m_pInStream->tellg();
		m_pInStream->seekg(0, std::ios_base::beg);

		DetermineAccess();
	}

	FileDataStream::FileDataStream(std::shared_ptr<std::fstream> s, SIZE_T inSize, bool freeOnClose)
		: DataStream(ACCESS_MODE::READ | ACCESS_MODE::WRITE), 
		 m_pInStream(s), 
		 m_pFStreamRO(0), 
		 m_pFStream(s), 
		 m_FreeOnClose(freeOnClose)
	{
		m_Size = inSize;

		DetermineAccess();
	}

	void FileDataStream::DetermineAccess()
	{
		m_Access = 0;

		if( m_pInStream )
		{
			m_Access |= ACCESS_MODE::READ;
		}

		if( m_pFStream )
		{
			m_Access |= ACCESS_MODE::WRITE;
		}
	}

	FileDataStream::~FileDataStream()
	{
		Close();
	}

	SIZE_T FileDataStream::Read(void* buf, SIZE_T count)
	{
		m_pInStream->read(static_cast<char*>(buf), static_cast<std::streamsize>(count));
		return (SIZE_T)m_pInStream->gcount();
	}

	SIZE_T FileDataStream::Write(const void* buf, SIZE_T count)
	{
		SIZE_T written = 0;
		if( IsWriteable() && m_pFStream )
		{
			m_pFStream->write(static_cast<const char*>(buf), static_cast<std::streamsize>(count));
			written = count;
		}

		return written;
	}
	void FileDataStream::Skip(SIZE_T count)
	{
		m_pInStream->clear();	//Clear fail status in case eof was set
		m_pInStream->seekg(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
	}

	void FileDataStream::Seek(SIZE_T pos)
	{
		m_pInStream->clear();	//Clear fail status in case eof was set
		m_pInStream->seekg(static_cast<std::streamoff>(pos), std::ios::beg);
	}

	SIZE_T FileDataStream::Tell() const
	{
		m_pInStream->clear(); // Clear fail status in case eof was set
		return (SIZE_T)m_pInStream->tellg();
	}

	bool FileDataStream::IsEOF() const
	{
		return m_pInStream->eof();
	}

	void FileDataStream::Close()
	{
		if( m_pInStream )
		{
			if( m_pFStreamRO )
			{
				m_pFStreamRO->close();
			}

			if( m_pFStream )
			{
				m_pFStream->flush();
				m_pFStream->close();
			}

			if( m_FreeOnClose )
			{
				m_pInStream = nullptr;
				m_pFStreamRO = nullptr;
				m_pFStream = nullptr;
			}
		}
	}
}
