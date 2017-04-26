/********************************************************************/
/**
* @file   geStringID.h
* @author Samuel Prince (samuel.prince.quezada@gmail.com)
* @date   2016/03/07
* @brief  A string identifier for very fast comparisons
*
* A string identifier that provides very fast comparisons to other
* string ids.
*
* @bug	   No known bugs.
*/
/********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Includes																												*/
/************************************************************************************************************************/
#include "gePrerequisitesUtil.h"
#include "geSpinLock.h"

namespace geEngineSDK
{
  /************************************************************************************************************************/
  /**
  * @brief	A string identifier that provides very fast comparisons to other string ids.
  * @note		Essentially a unique ID is generated for each string and then the ID is used for comparisons as if you were
  *			using an integer or an enum.
  * @note		Thread safe.
  */
  /************************************************************************************************************************/
  class GE_UTILITY_EXPORT StringID
  {
  public:
    static const StringID NONE;

  private:
    static const int32 HASH_TABLE_SIZE = 4096;
    static const int32 MAX_CHUNK_COUNT = 50;
    static const int32 ELEMENTS_PER_CHUNK = 256;
    static const int32 STRING_SIZE = 256;

  private:
    /************************************************************************************************************************/
    /**
    * @brief	Helper class that performs string actions on both null terminated character arrays and standard strings.
    */
    /************************************************************************************************************************/
    template<class T>
    class StringIDUtil
    {
    public:
      static uint32 Size(T const&) { return 0; }
      static void Copy(T const&, ANSICHAR*) {}
      static bool Compare(T const&, ANSICHAR*) { return 0; }
    };

    /**	Internal data that is shared by all instances for a specific string. */
    struct InternalData
    {
      uint32 Id;
      InternalData* Next;
      ANSICHAR Chars[STRING_SIZE];
    };

    /************************************************************************************************************************/
    /**
    * @brief	Performs initialization of static members as soon as the library is loaded.
    */
    /************************************************************************************************************************/
    struct InitStatics
    {
      InitStatics();
    };

  private:
    static volatile InitStatics m_InitStatics;
    static InternalData* m_StringHashTable[HASH_TABLE_SIZE];
    static InternalData* m_Chunks[MAX_CHUNK_COUNT];

    static uint32 m_NextId;
    static uint32 m_NumChunks;
    static SpinLock m_Sync;

    InternalData* m_Data;

  public:
    StringID();

    StringID(const ANSICHAR* name) : m_Data(nullptr) {
      Construct(name);
    }

    StringID(const String& name) : m_Data(nullptr) {
      Construct(name);
    }

    /*
    template<int N>
    StringID(const ANSICHAR name[N]) : m_Data(nullptr)
    {
    Construct((const ANSICHAR*)name);
    }
    */

    /************************************************************************************************************************/
    /**
    * @brief	Compare to string ids for equality. Uses fast integer comparison.
    */
    /************************************************************************************************************************/
    bool operator== (const StringID& rhs) const {
      return m_Data == rhs.m_Data;
    }

    /************************************************************************************************************************/
    /**
    * @brief	Compare to string ids for inequality. Uses fast integer comparison.
    */
    /************************************************************************************************************************/
    bool operator!= (const StringID& rhs) const {
      return m_Data != rhs.m_Data;
    }

    /************************************************************************************************************************/
    /**
    * @brief	Returns true if the string id has no value assigned.
    */
    /************************************************************************************************************************/
    bool Empty() const {
      return m_Data == nullptr;
    }

    /************************************************************************************************************************/
    /**
    * @brief	Returns the null-terminated name of the string id.
    */
    /************************************************************************************************************************/
    const ANSICHAR* c_str() const {
      if (m_Data == nullptr)
      {
        return nullptr;
      }

      return m_Data->Chars;
    }

  private:
    /************************************************************************************************************************/
    /**
    * @brief	Constructs a StringID object in a way that works for pointers to character arrays and standard strings.
    */
    /************************************************************************************************************************/
    template<class T>
    void Construct(T const& name);

    /************************************************************************************************************************/
    /**
    * @brief	Calculates a hash value for the provided null-terminated string.
    */
    /************************************************************************************************************************/
    template<class T>
    uint32 CalcHash(T const& input);

    /************************************************************************************************************************/
    /**
    * @brief	Allocates a new string entry and assigns it a unique ID. Optionally expands the chunks buffer if the new entry
    * doesn't fit.
    */
    /************************************************************************************************************************/
    InternalData* AllocEntry();
  };

  template<> struct RTTIPlainType<StringID>
  {
    enum { kID = TYPEID_UTILITY::kID_StringID }; enum { kHasDynamicSize = 1 };

    static void
    toMemory(const StringID& data, char* memory) {
      uint64 size = getDynamicSize(data);

      uint64 curSize = sizeof(uint64);
      memcpy(memory, &size, curSize);
      memory += curSize;

      bool isEmpty = data.Empty();
      memory = rttiWriteElement(isEmpty, memory);

      if (!isEmpty) {
        uint64 length = static_cast<uint64>(strlen(data.c_str()));
        memcpy(memory, data.c_str(), length * sizeof(ANSICHAR));
      }
    }

    static uint64
    fromMemory(StringID& data, char* memory) {
      uint64 size;
      memcpy(&size, memory, sizeof(uint64));
      memory += sizeof(uint64);

      bool empty = false;
      memory = rttiReadElement(empty, memory);

      if (!empty) {
        uint64 length = (size - sizeof(uint64) - sizeof(bool)) / sizeof(ANSICHAR);

        ANSICHAR* name = reinterpret_cast<ANSICHAR*>(ge_alloc(length + 1));
        memcpy(name, memory, length);
        name[length] = '\0';

        data = StringID(name);
      }

      return size;
    }

    static uint64
    getDynamicSize(const StringID& data) {
      uint64 dataSize = sizeof(bool) + sizeof(uint64);

      bool isEmpty = data.Empty();
      if (!isEmpty) {
        uint64 length = static_cast<uint64>(strlen(data.c_str()));
        dataSize += length * sizeof(ANSICHAR);
      }

      return dataSize;
    }
  };
}
