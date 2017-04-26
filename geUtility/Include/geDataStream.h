/********************************************************************/
/**
 * @file   geDataStream.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/09/19
 * @brief  General purpose class used for encapsulating the reading
 *			and writing of data
 *
 * General purpose class used for encapsulating the reading and writing
 * of data from and to various sources using a common interface.
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#pragma once

#include "gePrerequisitesUtil.h"
#include <istream>

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Supported encoding types for strings.
	*/
	/************************************************************************************************************************/
	namespace STRING_ENCODING
	{
		enum E
		{
			UTF8 = 1,
			UTF16 = 2
		};
	}

	namespace ACCESS_MODE
	{
		enum E
		{
			READ = 1,
			WRITE = 2
		};
	}

	/************************************************************************************************************************/
	/**
	* @brief	General purpose class used for encapsulating the reading and writing of data from and to various sources
	*			using a common interface.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT DataStream
	{
	public:
		/************************************************************************************************************************/
		/**
		* @brief	Creates an unnamed stream.
		*/
		/************************************************************************************************************************/
		DataStream(uint16 accessMode = ACCESS_MODE::READ) : m_Size(0), m_Access(accessMode)
		{}

		/************************************************************************************************************************/
		/**
		* @brief	Creates a named stream.
		*/
		/************************************************************************************************************************/
		DataStream(const String& name, uint16 accessMode = ACCESS_MODE::READ) : m_Name(name), m_Size(0), m_Access(accessMode)
		{}

		virtual ~DataStream()
		{}

		const String& GetName(void) { return m_Name; }
		uint16 GetAccessMode() const { return m_Access; }

		virtual bool IsReadable() const { return (m_Access & ACCESS_MODE::READ) != 0; }
		virtual bool IsWriteable() const { return (m_Access & ACCESS_MODE::WRITE) != 0; }

		/************************************************************************************************************************/
		/**
		* @brief	Reads data from the buffer and copies it to the specified value.
		*/
		/************************************************************************************************************************/
		template<typename T> DataStream& operator >> (T& val);

		/************************************************************************************************************************/
		/**
		* @brief	Read the requisite number of bytes from the stream, stopping at the end of the file.
		* @param[in]	buf		Pre-allocated buffer to read the data into.
		* @param[in]	count	Number of bytes to read.
		* @return				Number of bytes actually read.
		* @note		Stream must be created with READ access mode.
		*/
		/************************************************************************************************************************/
		virtual SIZE_T Read(void* buf, SIZE_T count) = 0;

		/************************************************************************************************************************/
		/**
		* @brief	Write the requisite number of bytes to the stream.
		* @param[in]	buf		Buffer containing bytes to write.
		* @param[in]	count	Number of bytes to write.
		* @return				Number of bytes actually written.
		* @note		Stream must be created with WRITE access mode.
		*/
		/************************************************************************************************************************/
		virtual SIZE_T Write(const void* buf, SIZE_T count)
		{
			GE_UNREFERENCED_PARAMETER(buf);
			GE_UNREFERENCED_PARAMETER(count);
			return 0;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Writes the provided narrow string to the steam. String is converted to the required encoding before being written.
		* @param[in]	string		String containing narrow characters to write, encoded as UTF8.
		* @param[in]	encoding	Encoding to convert the string to before writing.
		*/
		/************************************************************************************************************************/
		virtual void WriteString(const String& string, STRING_ENCODING::E encoding = STRING_ENCODING::UTF8);

		/************************************************************************************************************************/
		/**
		* @brief	Writes the provided wide string to the steam. String is converted to the required encoding before being written.
		* @param[in]	string		String containing wide characters to write, encoded as specified by platform for
		* 							wide characters.
		* @param[in]	encoding	Encoding to convert the string to before writing.
		*/
		/************************************************************************************************************************/
		virtual void WriteString(const WString& string, STRING_ENCODING::E encoding = STRING_ENCODING::UTF16);

		/************************************************************************************************************************/
		/**
		* @brief	Returns a string containing the entire stream.
		* @return	String data encoded as UTF-8.
		* @note		This is a convenience method for text streams only, allowing you to retrieve a String object containing
		*			all the data in the stream.
		*/
		/************************************************************************************************************************/
		virtual String GetAsString();

		/************************************************************************************************************************/
		/**
		* @brief	Returns a wide string containing the entire stream.
		* @return	Wide string encoded as specified by current platform.
		* @note		This is a convenience method for text streams only, allowing you to retrieve a WString object containing
		*			all the data in the stream.
		*/
		/************************************************************************************************************************/
		virtual WString GetAsWString();

		/************************************************************************************************************************/
		/**
		* @brief	Skip a defined number of bytes. This can also be a negative value, in which case the file pointer rewinds a
		*			defined number of bytes.
		*/
		/************************************************************************************************************************/
		virtual void Skip(SIZE_T count) = 0;

		/************************************************************************************************************************/
		/**
		* @brief	Repositions the read point to a specified byte.
		*/
		/************************************************************************************************************************/
		virtual void Seek(SIZE_T pos) = 0;

		/************************************************************************************************************************/
		/**
		* @brief	Returns the current byte offset from beginning.
		*/
		/************************************************************************************************************************/
		virtual SIZE_T Tell() const = 0;

		/************************************************************************************************************************/
		/**
		* @brief	Returns true if the stream has reached the end.
		*/
		/************************************************************************************************************************/
		virtual bool IsEOF() const = 0;

		/************************************************************************************************************************/
		/**
		* @brief	Returns the total size of the data to be read from the stream, or 0 if this is indeterminate for this stream.
		*/
		/************************************************************************************************************************/
		SIZE_T Size() const { return m_Size; }

		/************************************************************************************************************************/
		/**
		* @brief	Close the stream. This makes further operations invalid.
		*/
		/************************************************************************************************************************/
		virtual void Close() = 0;

	protected:
		static const uint32 StreamTempSize;

		String m_Name;
		SIZE_T m_Size;
		uint16 m_Access;
	};

	/************************************************************************************************************************/
	/**
	* @brief	Data stream for handling data from memory.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT MemoryDataStream : public DataStream
	{
	public:
		/************************************************************************************************************************/
		/**
		* @brief	Wrap an existing memory chunk in a stream.
		* @param[in] 	memory		Memory to wrap the data stream around.
		* @param[in]	size		Size of the memory chunk in bytes.
		*/
		/************************************************************************************************************************/
		MemoryDataStream(void* memory, SIZE_T size);

		/************************************************************************************************************************/
		/**
		* @brief	Create a stream which pre-buffers the contents of another stream. Data from the other buffer will be entirely
		*			read and stored in an internal buffer.
		* @param[in]	sourceStream		Stream to read data from.
		*/
		/************************************************************************************************************************/
		MemoryDataStream(DataStream& sourceStream);

		/************************************************************************************************************************/
		/**
		* @brief	Create a stream which pre-buffers the contents of another stream. Data from the other buffer will be entirely
		*			read and stored in an internal buffer.
		* @param[in]	sourceStream		Stream to read data from.
		*/
		/************************************************************************************************************************/
		MemoryDataStream(const DataStreamPtr& sourceStream);

		~MemoryDataStream();

		/************************************************************************************************************************/
		/**
		* @brief	Get a pointer to the start of the memory block this stream holds.
		*/
		/************************************************************************************************************************/
		uint8* GetPtr() const { return m_Data; }

		/************************************************************************************************************************/
		/**
		* @brief	Get a pointer to the current position in the memory block this stream holds.
		*/
		/************************************************************************************************************************/
		uint8* GetCurrentPtr() const { return m_Pos; }

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::Read
		*/
		/************************************************************************************************************************/
		SIZE_T Read(void* buf, SIZE_T count) override;

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::Write
		*/
		/************************************************************************************************************************/
		SIZE_T Write(const void* buf, SIZE_T count) override;

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::Skip
		*/
		/************************************************************************************************************************/
		void Skip(SIZE_T count) override;

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::Seek
		*/
		/************************************************************************************************************************/
		void Seek(SIZE_T pos) override;

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::Tell
		*/
		/************************************************************************************************************************/
		SIZE_T Tell() const override;

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::IsEOF
		*/
		/************************************************************************************************************************/
		bool IsEOF() const override;

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::Close
		*/
		/************************************************************************************************************************/
		void Close() override;

	protected:
		uint8* m_Data;
		uint8* m_Pos;
		uint8* m_End;

		bool m_FreeOnClose;
	};

	/************************************************************************************************************************/
	/**
	* @brief	Data stream for handling data from standard streams.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT FileDataStream : public DataStream
	{
	public:
		/************************************************************************************************************************/
		/**
		* @brief	Construct read-only stream from an standard stream.
		*
		* If @p freeOnClose is true, the STL stream will be freed once the data stream is closed.
		*/
		FileDataStream(std::shared_ptr<std::ifstream> s, bool freeOnClose = true);

		/************************************************************************************************************************/
		/**
		* @brief	Construct read-write stream from an standard stream.
		*
		* If @p freeOnClose is true, the STL stream will be freed once the data stream is closed.
		*/
		FileDataStream(std::shared_ptr<std::fstream> s, bool freeOnClose = true);

		/************************************************************************************************************************/
		/**
		* @brief	Construct read-only stream from an standard stream, and tell it the size.
		*
		* Size parameter allows you to specify the size without requiring us to seek to the end of the stream to find
		* the size.
		*
		* If @p freeOnClose is true, the STL stream will be freed once the data stream is closed.
		*/
		FileDataStream(std::shared_ptr<std::ifstream> s, SIZE_T size, bool freeOnClose = true);

		/************************************************************************************************************************/
		/**
		* @brief	Construct read-write stream from an standard stream, and tell it the size.
		*
		* Size parameter allows you to specify the size without requiring us to seek to the end of the stream to find
		* the size.
		*
		* If @p freeOnClose is true, the STL stream will be freed once the data stream is closed.
		*/
		FileDataStream(std::shared_ptr<std::fstream> s, SIZE_T size, bool freeOnClose = true);

		~FileDataStream();

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::Read
		*/
		/************************************************************************************************************************/
		SIZE_T Read(void* buf, SIZE_T count) override;

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::Write
		*/
		/************************************************************************************************************************/
		SIZE_T Write(const void* buf, SIZE_T count) override;

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::Skip
		*/
		/************************************************************************************************************************/
		void Skip(SIZE_T count) override;

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::Seek
		*/
		/************************************************************************************************************************/
		void Seek(SIZE_T pos) override;

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::Tell
		*/
		/************************************************************************************************************************/
		SIZE_T Tell() const override;

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::IsEOF
		*/
		/************************************************************************************************************************/
		bool IsEOF() const override;

		/************************************************************************************************************************/
		/**
		* @brief	@copydoc DataStream::Close
		*/
		/************************************************************************************************************************/
		void Close() override;

	protected:
		std::shared_ptr<std::istream> m_pInStream;
		std::shared_ptr<std::ifstream> m_pFStreamRO;
		std::shared_ptr<std::fstream> m_pFStream;
		bool m_FreeOnClose;

		void DetermineAccess();
	};
}
