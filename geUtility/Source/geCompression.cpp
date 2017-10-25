/*****************************************************************************/
/**
 * @file    geCompression.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/15
 * @brief   Performs generic compression and decompression on raw data
 *
 * Performs generic compression and decompression on raw data
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geCompression.h"
#include "geDataStream.h"
#include "geDebug.h"

#include "snappy.h"
#include "snappy-sinksource.h"

namespace geEngineSDK {
  using std::static_pointer_cast;

  /**
   * @brief Source accepting a data stream. Used for Snappy compression lib.
   */
  class DataStreamSource : public snappy::Source
  {
   public:
    explicit DataStreamSource(const SPtr<DataStream>& stream)
      : m_stream(stream),
        m_readBuffer(nullptr),
        m_readBufferContentSize(0),
        m_bufferOffset(0) {
      m_remaining = m_stream->size() - m_stream->tell();

      if (m_stream->isFile()) {
        m_readBuffer = reinterpret_cast<char*>(ge_alloc(2048));
      }
    }

    virtual ~DataStreamSource() {
      if (nullptr != m_readBuffer) {
        ge_free(m_readBuffer);
      }
    }

    size_t
    Available() const override {
      return m_remaining;
    }

    const char*
    Peek(size_t* len) override {
      if (!m_stream->isFile()) {
        SPtr<MemoryDataStream> memStream = static_pointer_cast<MemoryDataStream>(m_stream);
        *len = Available();
        return reinterpret_cast<char*>(memStream->getPtr() + m_bufferOffset);
      }
      else {
        while (m_bufferOffset >= m_readBufferContentSize) {
          m_bufferOffset -= m_readBufferContentSize;
          m_readBufferContentSize = m_stream->read(m_readBuffer, 2048);
          if (0 == m_readBufferContentSize) {
            break;
          }
        }
        
        *len = m_readBufferContentSize - m_bufferOffset;
        return reinterpret_cast<char*>(m_readBuffer + m_bufferOffset);
      }
    }

    void
    Skip(size_t n) override {
      m_bufferOffset += n;
      m_remaining -= n;
    }

   private:
    SPtr<DataStream> m_stream;

    //File streams only
    char* m_readBuffer;
    size_t m_readBufferContentSize;
    size_t m_remaining;
    size_t m_bufferOffset;
  };

  /**
   * @brief Sink (destination) accepting a data stream. Used for Snappy compression lib.
   */
  class DataStreamSink : public snappy::Sink
  {
    struct BufferPiece
    {
      char* buffer;
      size_t size;
    };

   public:
    DataStreamSink() {}

    virtual ~DataStreamSink() {
      for (auto& entry : m_bufferPieces) {
        ge_free(entry.buffer);
      }
    }

    void
    Append(const char* data, size_t n) override {
      if (0 == m_bufferPieces.size() || m_bufferPieces.back().buffer != data) {
        BufferPiece piece;
        piece.buffer = reinterpret_cast<char*>(ge_alloc(n));
        piece.size = n;

        memcpy(piece.buffer, data, n);
        m_bufferPieces.push_back(piece);
      }
      else {
        BufferPiece& piece = m_bufferPieces.back();
        GE_ASSERT(piece.buffer == data);
        piece.size = n;
      }
    }

    char*
    GetAppendBuffer(size_t len, char* scratch) override {
      GE_UNREFERENCED_PARAMETER(scratch);
      BufferPiece piece;
      piece.buffer = reinterpret_cast<char*>(ge_alloc(len));
      piece.size = 0;
      m_bufferPieces.push_back(piece);
      return piece.buffer;
    }

    char*
    GetAppendBufferVariable(size_t min_size,
                            size_t desired_size_hint,
                            char* scratch,
                            size_t scratch_size,
                            size_t* allocated_size) override {
      GE_UNREFERENCED_PARAMETER(min_size);
      GE_UNREFERENCED_PARAMETER(scratch);
      GE_UNREFERENCED_PARAMETER(scratch_size);
      BufferPiece piece;
      piece.buffer = reinterpret_cast<char*>(ge_alloc(desired_size_hint));
      piece.size = 0;

      m_bufferPieces.push_back(piece);

      *allocated_size = desired_size_hint;
      return piece.buffer;
    }

    void
    AppendAndTakeOwnership(char* bytes,
                           size_t n,
                           void(*deleter)(void*, const char*, size_t),
                           void *deleter_arg) override {
      BufferPiece& piece = m_bufferPieces.back();

      if (piece.buffer != bytes) {
        memcpy(piece.buffer, bytes, n);
        (*deleter)(deleter_arg, bytes, n);
      }

      piece.size = n;
    }

    SPtr<MemoryDataStream>
    GetOutput() {
      size_t totalSize = 0;
      for (auto& entry : m_bufferPieces) {
        totalSize += entry.size;
      }

      SPtr<MemoryDataStream> ds = ge_shared_ptr_new<MemoryDataStream>(totalSize);
      for (auto& entry : m_bufferPieces) {
        ds->write(entry.buffer, entry.size);
      }
      ds->seek(0);
      return ds;
    }

   private:
    Vector<BufferPiece> m_bufferPieces;
  };

  SPtr<MemoryDataStream>
  Compression::compress(SPtr<DataStream>& input) {
    DataStreamSource src(input);
    DataStreamSink dst;

    size_t bytesWritten = snappy::Compress(&src, &dst);
    SPtr<MemoryDataStream> output = dst.GetOutput();
    GE_ASSERT(output->size() == bytesWritten);
    GE_UNREFERENCED_PARAMETER(bytesWritten);

    return output;
  }

  SPtr<MemoryDataStream>
  Compression::decompress(SPtr<DataStream>& input) {
    DataStreamSource src(input);
    DataStreamSink dst;

    if (!snappy::Uncompress(&src, &dst)) {
      LOGERR("Decompression failed, corrupt data.");
      return nullptr;
    }

    return dst.GetOutput();
  }
}
