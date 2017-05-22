/*****************************************************************************/
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
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geDataStream.h"
#include "geDebug.h"
//#include "geException.h"
#include <codecvt>

namespace geEngineSDK {
  using std::codecvt_mode;
  using std::codecvt_utf8;
  using std::codecvt_utf16;
  using std::codecvt_utf8_utf16;
  using std::wstring_convert;
  using std::stringstream;

  using std::generate_header;
  using std::consume_header;
  using std::little_endian;

  const uint32 DataStream::streamTempSize = 128;

  /**
   * @brief	Checks does the provided buffer has an UTF32 byte order mark in
   *        little endian order.
   */
  bool
  isUTF32LE(const char* buffer) {
    return (buffer[0] == 0xFF &&
            buffer[1] == 0xFE &&
            buffer[2] == 0x00 &&
            buffer[3] == 0x00);
  }

  /**
   * @brief Checks does the provided buffer has an UTF32 byte order mark in
   *        big endian order.
   */
  bool
  isUTF32BE(const char* buffer) {
    return (buffer[0] == 0x00 &&
            buffer[1] == 0x00 &&
            buffer[2] == 0xFE &&
            buffer[3] == 0xFF);
  }

  /**
   * @brief Checks does the provided buffer has an UTF16 byte order mark in
   *        little endian order.
   */
  bool
  isUTF16LE(const char* buffer) {
    return (buffer[0] == 0xFF &&
            buffer[1] == 0xFE);
  }

  /**
   * @brief Checks does the provided buffer has an UTF16 byte order mark in
   *        big endian order.
   */
  bool
  isUTF16BE(const char* buffer) {
    return (buffer[0] == 0xFE &&
            buffer[1] == 0xFF);
  }

  /**
   * @brief Checks does the provided buffer has an UTF8 byte order mark.
   */
  bool
  isUTF8(const char* buffer) {
    return (buffer[0] == 0xEF && 
            buffer[1] == 0xBB && 
            buffer[2] == 0xBF);
  }

  template<typename T>
  DataStream&
  DataStream::operator>>(T& val) {
    read(static_cast<void*>(&val), sizeof(T));
    return *this;
  }

  void
  DataStream::writeString(const String& string, STRING_ENCODING::E encoding) {
    if (STRING_ENCODING::kUTF16 == encoding) {
      const codecvt_mode convMode = static_cast<codecvt_mode>(generate_header);
      typedef codecvt_utf8_utf16<char, 1114111, convMode> UTF8ToUTF16Conv;
      wstring_convert<UTF8ToUTF16Conv, char> conversion("?");

      std::string encodedString = conversion.from_bytes(string.c_str());
      write(encodedString.data(), encodedString.length());
    }
    else {
      write(string.data(), string.length());
    }
  }

  void
  DataStream::writeString(const WString& string, STRING_ENCODING::E encoding) {
    if (STRING_ENCODING::kUTF16 == encoding) {
      const codecvt_mode convMde = 
        static_cast<codecvt_mode>(generate_header | little_endian);
      typedef codecvt_utf16<wchar_t, 1114111, convMde> WCharToUTF16Conv;
      wstring_convert<WCharToUTF16Conv, wchar_t> conversion("?");

      std::string encodedString = conversion.to_bytes(string.c_str());
      write(encodedString.data(), encodedString.length());
    }
    else {
      const codecvt_mode convMode = static_cast<codecvt_mode>(generate_header);
      typedef std::codecvt_utf8<wchar_t, 1114111, convMode> WCharToUTF8Conv;
      wstring_convert<WCharToUTF8Conv, wchar_t> conversion("?");

      std::string encodedString = conversion.to_bytes(string.c_str());
      write(encodedString.data(), encodedString.length());
    }
  }

  String
  DataStream::getAsString() {
    //Read the entire buffer - ideally in one read, but if the size of the
    //buffer is unknown, do multiple fixed size reads.
    SIZE_T bufSize = (m_size > 0 ? m_size : 4096);
    stringstream::char_type* tempBuffer = 
      static_cast<stringstream::char_type*>(ge_alloc(bufSize));

    //Ensure read from begin of stream
    seek(0);

    stringstream result;
    while (!isEOF()) {
      SIZE_T numReadBytes = read(tempBuffer, bufSize);
      result.write(tempBuffer, numReadBytes);
    }

    free(tempBuffer);
    std::string string = result.str();

    SIZE_T readBytes = string.size();
    if (4 <= readBytes) {
      if (isUTF32LE(string.data())) {
        //Little Endian 32 bits
        const codecvt_mode convMode = 
          static_cast<codecvt_mode>(consume_header | little_endian);
        typedef codecvt_utf8<uint32, 1114111, convMode> utf8utf32;

        wstring_convert<utf8utf32, uint32> conversion("?");
        uint32* start = (uint32*)string.data();
        uint32* end = (start + (string.size() - 1) / 4);

        return conversion.to_bytes(start, end).c_str();
      }
      else if (isUTF32BE(string.data())) {
        //Big Endian 32 bits
        const codecvt_mode convMode = static_cast<codecvt_mode>(consume_header);
        typedef codecvt_utf8<uint32, 1114111, convMode> utf8utf32;

        wstring_convert<utf8utf32, uint32> conversion("?");
        uint32* start = (uint32*)string.data();
        uint32* end = (start + (string.size() - 1) / 4);

        return conversion.to_bytes(start, end).c_str();
      }
    }

    if (3 <= readBytes) {
      if (isUTF8(string.data())) {
        return string.c_str() + 3;
      }
    }

    if (2 <= readBytes) {
      if (isUTF16LE(string.data())) {
        //Little Endian 16 bits
        const codecvt_mode convMode = static_cast<codecvt_mode>(little_endian);
        typedef codecvt_utf8_utf16<uint16, 1114111, convMode> utf8utf16;

        wstring_convert<utf8utf16, uint16> conversion("?");

        //Bug?: std::consume_header seems to be ignored so I manually remove the header
        uint16* start = (uint16*)(string.c_str() + 2);

        return conversion.to_bytes(start).c_str();
      }
      else if (isUTF16BE(string.data())) {
        //Big Endian 16 bits
        const codecvt_mode convMode = static_cast<codecvt_mode>(0);
        typedef codecvt_utf8_utf16<uint16, 1114111, convMode> utf8utf16;

        //Bug?: Regardless of not providing the std::little_endian flag it seems
        //that is how the data is read so I manually flip it
        SIZE_T numChars = (string.size() - 2) / 2;
        for (SIZE_T i = 0; i<numChars; ++i) {
          std::swap(string[i * 2 + 0], string[i * 2 + 1]);
        }

        wstring_convert<utf8utf16, uint16> conversion("?");

        //Bug?: std::consume_header seems to be ignored so I manually remove the header
        uint16* start = (uint16*)(string.c_str() + 2);

        return conversion.to_bytes(start).c_str();
      }
    }

    return string.c_str();
  }

  WString
  DataStream::getAsWString() {
    //Read the entire buffer - ideally in one read, but if the size of the
    //buffer is unknown, do multiple fixed size reads.
    SIZE_T bufSize = (m_size > 0 ? m_size : 4096);
    stringstream::char_type* tempBuffer = 
      static_cast<stringstream::char_type*>(ge_alloc(bufSize));

    //Ensure read from begin of stream
    seek(0);

    stringstream result;
    while (!isEOF()) {
      SIZE_T numReadBytes = read(tempBuffer, bufSize);
      result.write(tempBuffer, numReadBytes);
    }

    free(tempBuffer);
    std::string string = result.str();

    uint32 readBytes = (uint32)string.size();
    if (4 <= readBytes) {
      if (isUTF32LE(string.data())) {
        //Little Endian 32 bits
        //Not supported, maybe throw an exception?
      }
      else if (isUTF32BE(string.data())) {
        //Big Endian 32 bits
        //Not supported, maybe throw an exception?
      }
    }

    if (3 <= readBytes) {
      if (isUTF8(string.data())) {
        const codecvt_mode convMode = static_cast<codecvt_mode>(consume_header);
        typedef codecvt_utf8<wchar_t, 1114111, convMode> wcharutf8;

        wstring_convert<wcharutf8> conversion("?");
        return conversion.from_bytes(string).c_str();
      }
    }

    if (2 <= readBytes) {
      if (isUTF16LE(string.data())) {
        //Little Endian 16 bits
        const codecvt_mode convMode = 
          static_cast<codecvt_mode>(consume_header | little_endian);
        typedef std::codecvt_utf16<wchar_t, 1114111, convMode> wcharutf16;

        wstring_convert<wcharutf16> conversion("?");
        return conversion.from_bytes(string).c_str();
      }
      else if (isUTF16BE(string.data())) {
        //Big Endian 32 bits
        const codecvt_mode convMode = static_cast<codecvt_mode>(consume_header);
        typedef codecvt_utf16<wchar_t, 1114111, convMode> wcharutf16;

        wstring_convert<wcharutf16> conversion("?");
        return conversion.from_bytes(string).c_str();
      }
    }

    {
      const codecvt_mode convMode = static_cast<codecvt_mode>(consume_header);
      typedef codecvt_utf8<wchar_t, 1114111, convMode> wcharutf8;

      wstring_convert<wcharutf8> conversion("?");
      return conversion.from_bytes(string).c_str();
    }
  }

  MemoryDataStream::MemoryDataStream(void* memory, SIZE_T inSize, bool freeOnClose)
    : DataStream(ACCESS_MODE::kREAD | ACCESS_MODE::kWRITE),
      m_data(nullptr),
      m_freeOnClose(freeOnClose) {
    m_data = m_pos = static_cast<uint8*>(memory);
    m_size = inSize;
    m_end = m_data + m_size;

    GE_ASSERT(m_end >= m_pos);
  }

  MemoryDataStream::MemoryDataStream(DataStream& sourceStream)
    : DataStream(ACCESS_MODE::kREAD | ACCESS_MODE::kWRITE),
      m_data(nullptr) {
    //Copy data from incoming stream
    m_size = sourceStream.size();

    m_data = reinterpret_cast<uint8*>(ge_alloc(m_size));
    m_pos = m_data;
    m_end = m_data + sourceStream.read(m_data, m_size);
    m_freeOnClose = true;

    GE_ASSERT(m_end >= m_pos);
  }

  MemoryDataStream::MemoryDataStream(const SPtr<DataStream>& sourceStream)
    : DataStream(ACCESS_MODE::kREAD | ACCESS_MODE::kWRITE),
      m_data(nullptr) {
    //Copy data from incoming stream
    m_size = sourceStream->size();

    m_data = reinterpret_cast<uint8*>(ge_alloc(m_size));
    m_pos = m_data;
    m_end = m_data + sourceStream->read(m_data, m_size);
    m_freeOnClose = true;

    GE_ASSERT(m_end >= m_pos);
  }

  MemoryDataStream::~MemoryDataStream() {
    close();
  }

  SIZE_T
  MemoryDataStream::read(void* buf, SIZE_T count) {
    SIZE_T cnt = count;

    if (m_pos + cnt > m_end) {
      cnt = m_end - m_pos;
    }
    if (0 == cnt) {
      return 0;
    }

    GE_ASSERT(cnt <= count);

    memcpy(buf, m_pos, cnt);
    m_pos += cnt;

    return cnt;
  }

  SIZE_T
  MemoryDataStream::write(const void* buf, SIZE_T count) {
    SIZE_T written = 0;
    if (isWriteable()) {
      written = count;

      if (m_pos + written > m_end) {
        written = m_end - m_pos;
      }
      if (0 == written) {
        return 0;
      }

      memcpy(m_pos, buf, written);
      m_pos += written;
    }

    return written;
  }

  void
  MemoryDataStream::skip(SIZE_T count) {
    SIZE_T newpos = static_cast<SIZE_T>((m_pos - m_data) + count);
    GE_ASSERT(m_data + newpos <= m_end);
    m_pos = m_data + newpos;
  }

  void
  MemoryDataStream::seek(SIZE_T pos) {
    GE_ASSERT(m_data + pos <= m_end);
    m_pos = m_data + pos;
  }

  SIZE_T
  MemoryDataStream::tell() const {
    return m_pos - m_data;
  }

  bool
  MemoryDataStream::isEOF() const {
    return m_pos >= m_end;
  }

  SPtr<DataStream>
  MemoryDataStream::clone(bool copyData) const {
    if (!copyData) {
      return ge_shared_ptr_new<MemoryDataStream>(m_data, m_size, false);
    }
    return ge_shared_ptr_new<MemoryDataStream>(*this);
  }

  void
  MemoryDataStream::close() {
    if (nullptr != m_data) {
      if (m_freeOnClose) {
        ge_free(m_data);
      }
      m_data = nullptr;
    }
  }

  FileDataStream::FileDataStream(const Path& filePath,
                                 ACCESS_MODE::E accessMode,
                                 bool freeOnClose)
    : DataStream(static_cast<uint16>(accessMode)),
      m_path(filePath),
      m_freeOnClose(freeOnClose) {
    //Always open in binary mode. Also, always include reading
    std::ios::openmode mode = std::ios::binary;

    if ((accessMode & ACCESS_MODE::kREAD) != 0) {
      mode |= std::ios::in;
    }

    if (((accessMode & ACCESS_MODE::kWRITE) != 0)) {
      mode |= std::ios::out;
      m_pFStream = ge_shared_ptr_new<std::fstream>();
      m_pFStream->open(filePath.toPlatformString().c_str(), mode);
      m_pInStream = m_pFStream;
    }
    else {
      m_pFStreamRO = ge_shared_ptr_new<std::ifstream>();
      m_pFStreamRO->open(filePath.toPlatformString().c_str(), mode);
      m_pInStream = m_pFStreamRO;
    }

    //Should check ensure open succeeded, in case fail for some reason.
    if (m_pInStream->fail()) {
      LOGWRN("Cannot open file: " + filePath.toString());
      return;
    }

    m_pInStream->seekg(0, std::ios_base::end);
    m_size = static_cast<SIZE_T>(m_pInStream->tellg());
    m_pInStream->seekg(0, std::ios_base::beg);
  }

  FileDataStream::~FileDataStream() {
    close();
  }

  SIZE_T
  FileDataStream::read(void* buf, SIZE_T count) {
    m_pInStream->read(static_cast<char*>(buf), static_cast<std::streamsize>(count));
    return static_cast<SIZE_T>(m_pInStream->gcount());
  }

  SIZE_T
  FileDataStream::write(const void* buf, SIZE_T count) {
    SIZE_T written = 0;
    if (isWriteable() && m_pFStream) {
      m_pFStream->write(static_cast<const char*>(buf), static_cast<std::streamsize>(count));
      written = count;
    }
    return written;
  }

  void
  FileDataStream::skip(SIZE_T count) {
    m_pInStream->clear(); //Clear fail status in case eof was set
    m_pInStream->seekg(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
  }

  void
  FileDataStream::seek(SIZE_T pos) {
    m_pInStream->clear();	//Clear fail status in case eof was set
    m_pInStream->seekg(static_cast<std::streamoff>(pos), std::ios::beg);
  }

  SIZE_T
  FileDataStream::tell() const {
    m_pInStream->clear(); //Clear fail status in case eof was set
    return static_cast<SIZE_T>(m_pInStream->tellg());
  }

  bool
  FileDataStream::isEOF() const {
    return m_pInStream->eof();
  }

  SPtr<DataStream>
  FileDataStream::clone(bool copyData) const {
    GE_UNREFERENCED_PARAMETER(copyData);
    return ge_shared_ptr_new<FileDataStream>(m_path,
                                             static_cast<ACCESS_MODE::E>(getAccessMode()),
                                             true);
  }

  void
  FileDataStream::close() {
    if (m_pInStream) {
      if (m_pFStreamRO) {
        m_pFStreamRO->close();
      }

      if (m_pFStream) {
        m_pFStream->flush();
        m_pFStream->close();
      }

      if (m_freeOnClose) {
        m_pInStream = nullptr;
        m_pFStreamRO = nullptr;
        m_pFStream = nullptr;
      }
    }
  }
}
