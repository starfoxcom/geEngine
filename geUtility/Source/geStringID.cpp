/*****************************************************************************/
/**
 * @file    geStringID.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/07
 * @brief   A string identifier for very fast comparisons
 *
 * A string identifier that provides very fast comparisons to other string ids.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
* Includes
*/
/*****************************************************************************/
#include "geStringID.h"

namespace geEngineSDK {
  const StringID StringID::NONE = StringID();

  volatile StringID::InitStatics StringID::m_InitStatics = StringID::InitStatics();
  StringID::InternalData* StringID::m_StringHashTable[HASH_TABLE_SIZE];
  StringID::InternalData* StringID::m_Chunks[MAX_CHUNK_COUNT];

  uint32 StringID::m_NextId = 0;
  uint32 StringID::m_NumChunks = 0;
  SpinLock StringID::m_Sync;

  StringID::InitStatics::InitStatics() {
    ScopedSpinLock lock(m_Sync);
    memset(m_StringHashTable, 0, sizeof(m_StringHashTable));
    memset(m_Chunks, 0, sizeof(m_Chunks));
    m_Chunks[0] = reinterpret_cast<InternalData*>
                    (ge_alloc(sizeof(InternalData) * ELEMENTS_PER_CHUNK));
    memset(m_Chunks[0], 0, sizeof(InternalData) * ELEMENTS_PER_CHUNK);
    m_NumChunks++;
  }

  StringID::StringID() : m_Data(nullptr) {}

  template<class T>
  void
  StringID::construct(T const& name) {
    GE_ASSERT(StringIDUtil<T>::size(name) <= STRING_SIZE);

    uint32 hash = calcHash(name)
                  & (sizeof(m_StringHashTable) / sizeof(m_StringHashTable[0]) - 1);
    InternalData* existingEntry = m_StringHashTable[hash];

    while (nullptr != existingEntry) {
      if (StringIDUtil<T>::compare(name, existingEntry->m_chars)) {
        m_Data = existingEntry;
        return;
      }

      existingEntry = existingEntry->m_next;
    }

    ScopedSpinLock lock(m_Sync);

    //Search for the value again in case other thread just added it
    existingEntry = m_StringHashTable[hash];
    InternalData* lastEntry = nullptr;
    while (nullptr != existingEntry) {
      if (StringIDUtil<T>::compare(name, existingEntry->m_chars)) {
        m_Data = existingEntry;
        return;
      }

      lastEntry = existingEntry;
      existingEntry = existingEntry->m_next;
    }

    m_Data = allocEntry();
    StringIDUtil<T>::copy(name, m_Data->m_chars);
    if (nullptr == lastEntry) {
      m_StringHashTable[hash] = m_Data;
    }
    else {
      lastEntry->m_next = m_Data;
    }
  }

  template<class T>
  uint32
  StringID::calcHash(T const& input) {
    uint32 size = StringIDUtil<T>::size(input);

    uint32 hash = 0;
    for (uint32 i = 0; i < size; ++i) {
      hash = hash * 101 + input[i];
    }

    return hash;
  }

  StringID::InternalData*
  StringID::allocEntry() {
    uint32 chunkIdx = m_NextId / ELEMENTS_PER_CHUNK;

    GE_ASSERT(chunkIdx < MAX_CHUNK_COUNT);
    GE_ASSERT(chunkIdx <= m_NumChunks); //Can only increment sequentially

    if (chunkIdx >= m_NumChunks) {
      m_Chunks[chunkIdx] = reinterpret_cast<InternalData*>
                             (ge_alloc(sizeof(InternalData) * ELEMENTS_PER_CHUNK));
      memset(m_Chunks[chunkIdx], 0, sizeof(InternalData) * ELEMENTS_PER_CHUNK);
      m_NumChunks++;
    }

    InternalData* chunk = m_Chunks[chunkIdx];
    uint32 chunkSpecificIndex = m_NextId % ELEMENTS_PER_CHUNK;

    InternalData* newEntry = &chunk[chunkSpecificIndex];
    newEntry->m_id = m_NextId++;
    newEntry->m_next = nullptr;

    return newEntry;
  }

  template<>
  class StringID::StringIDUtil<const ANSICHAR*>
  {
  public:
    static uint32
    size(const ANSICHAR* const& input) {
      return (uint32)strlen(input);
    }

    static void
    copy(const ANSICHAR* const& input, ANSICHAR* dest) {
      memcpy(dest, input, strlen(input) + 1);
    }

    static bool
    compare(const ANSICHAR* const& a,
            ANSICHAR* b) {
      return strcmp(a, b) == 0;
    }
  };

  template<>
  class StringID::StringIDUtil<String>
  {
  public:
    static uint32
    size(String const& input) {
        return(uint32)input.length();
    }

    static void
    copy(String const& input, ANSICHAR* dest) {
      uint32 len = (uint32)input.length();
      input.copy(dest, len);
      dest[len] = '\0';
    }

    static bool
    compare(String const& a, ANSICHAR* b) {
      return a.compare(b) == 0;
    }
  };

  template
  class StringID::StringIDUtil<const ANSICHAR*>;
  
  template
  class StringID::StringIDUtil<String>;

  template
  GE_UTILITY_EXPORT void
  StringID::construct(const ANSICHAR* const&);
  
  template
  GE_UTILITY_EXPORT void
  StringID::construct(String const&);

  template
  GE_UTILITY_EXPORT uint32 StringID::calcHash(const ANSICHAR* const&);
  
  template
  GE_UTILITY_EXPORT uint32 StringID::calcHash(String const&);
}
