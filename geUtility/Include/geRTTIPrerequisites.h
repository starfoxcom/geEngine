/*****************************************************************************/
/**
* @file    geRTTIPrerequisites.h
* @author  Samuel Prince (samuel.prince.quezada@gmail.com)
* @date    2015/02/09
* @brief   Run-time type information utilities
*
* Utilities and functions created for RTTI purposes
*
* @bug	    No known bugs.
*/
/*****************************************************************************/
#pragma once

namespace geEngineSDK {
  /************************************************************************************************************************/
  /**
  * @brief	Template that you may specialize with a class if you want to provide
  * 			simple serialization for it.
  *
  *			Any type that uses the "plain" field in the RTTI system must specialize this class.
  *
  * @note		Normally you will want to implement IReflectable interface if you want to provide serialization
  * 			as that interface properly handles versioning, nested objects, pointer handling and more.
  *
  *			This class is useful for types you can easily serialize using a memcpy (built-in types like int/float/etc), or
  *			types you cannot modify so they implement IReflectable interface (like std::string or std::vector).
  *
  * @see		RTTIType
  * @see		RTTIField
  */
  /************************************************************************************************************************/
  template<class T>
  struct RTTIPlainType
  {
    static_assert(std::is_pod<T>::value,
      "Provided type isn't plain-old-data. You need to specialize RTTIPlainType template in order to serialize this type. "\
      "(Or call GE_ALLOW_MEMCPY_SERIALIZATION(type) macro if you are sure the type can be properly serialized using just memcpy.)");

    enum
    {
      id = 0 /**< Unique id for the serializable type. */
    };
    enum
    {
      hasDynamicSize = 0 /**< 0 (Object has static size less than 255 bytes, e.g. int) or 1 (Dynamic size with no size restriction, e.g. string) */
    };

    /************************************************************************************************************************/
    /**
    * @brief	Serializes the provided object into the provided pre-allocated
    * 			memory buffer.
    */
    /************************************************************************************************************************/
    static void ToMemory(const T& data, char* memory)
    {
      memcpy(memory, &data, sizeof(T));
    }

    /************************************************************************************************************************/
    /**
    * @brief	Deserializes a previously allocated object from the provided
    * 			memory buffer. Return the number of bytes read from the memory buffer.
    */
    /************************************************************************************************************************/
    static uint32 FromMemory(T& data, char* memory)
    {
      memcpy(&data, memory, sizeof(T));
      return sizeof(T);
    }

    /**
    * @brief	Returns the size of the provided object. (Works for both
    * 			static and dynamic size types)
    */
    static uint32 GetDynamicSize(const T&)
    {
      return sizeof(T);
    }
  };

  /************************************************************************************************************************/
  /**
  * @brief	Helper method when serializing known data types that have valid RTTIPlainType specialization.
  *
  *			Returns the size of the element. If elements serializable type is specialized with hasDynamicSize == true,
  *			the dynamic size is calculated, otherwise sizeof() is used.
  */
  /************************************************************************************************************************/
  template<class ElemType>
  uint32 RTTIGetElementSize(const ElemType& data)
  {
#if GE_COMPILER == GE_COMPILER_MSVC
#	pragma warning( disable : 4127 )
#endif
    if (RTTIPlainType<ElemType>::hasDynamicSize == 1)
    {
      return RTTIPlainType<ElemType>::GetDynamicSize(data);
    }
    else
    {
      return sizeof(ElemType);
    }
#if GE_COMPILER == GE_COMPILER_MSVC
#	pragma warning( default: 4127 )
#endif
  }

  /************************************************************************************************************************/
  /**
  * @brief	Helper method when serializing known data types that have valid
  * 			RTTIPlainType specialization.
  *
  *			Writes the specified data into memory, advances the memory pointer by the
  *			bytes written and returns pointer to new memory.
  */
  /************************************************************************************************************************/
  template<class ElemType>
  char* RTTIWriteElement(const ElemType& data, char* memory)
  {
    RTTIPlainType<ElemType>::ToMemory(data, memory);
    return memory + RTTIGetElementSize(data);
  }

  /************************************************************************************************************************/
  /**
  * @brief	Helper method when serializing known data types that have valid
  * 			RTTIPlainType specialization.
  *
  *			Writes the specified data into memory, advances the memory pointer by the
  *			bytes written and returns pointer to new memory. Also increases the size
  *			value by the size of the written element.
  */
  /************************************************************************************************************************/
  template<class ElemType>
  char* RTTIWriteElement(const ElemType& data, char* memory, uint32& size)
  {
    RTTIPlainType<ElemType>::ToMemory(data, memory);

    uint32 elemSize = RTTIGetElementSize(data);
    size += elemSize;

    return memory + elemSize;
  }

  /************************************************************************************************************************/
  /**
  * @brief	Helper method when serializing known data types that have valid
  * 			RTTIPlainType specialization.
  *
  *			Reads the specified data into memory, advances the memory pointer by the
  *			bytes read and returns pointer to new memory.
  */
  /************************************************************************************************************************/
  template<class ElemType>
  char* RTTIReadElement(ElemType& data, char* memory)
  {
    RTTIPlainType<ElemType>::FromMemory(data, memory);
    return memory + RTTIGetElementSize(data);
  }

  /************************************************************************************************************************/
  /**
  * @brief	Helper method when serializing known data types that have valid
  * 			RTTIPlainType specialization.
  *
  *			Reads the specified data into memory, advances the memory pointer by the
  *			bytes read and returns pointer to new memory. Also increases the size
  *			value by the size of the read element.
  */
  /************************************************************************************************************************/
  template<class ElemType>
  char* RTTIReadElement(ElemType& data, char* memory, uint32& size)
  {
    RTTIPlainType<ElemType>::FromMemory(data, memory);

    uint32 elemSize = RTTIGetElementSize(data);
    size += elemSize;

    return memory + elemSize;
  }

  /************************************************************************************************************************/
  /**
  * @brief	Tell the RTTI system that the specified type may be serialized just by
  * 			using a memcpy.
  *
  * @note		Internally this creates a basic RTTIPlainType specialization for the type.
  *
  * @see		RTTIPlainType
  */
  /************************************************************************************************************************/
#define GE_ALLOW_MEMCPY_SERIALIZATION(type)										\
	template<> struct RTTIPlainType<type>{										\
		enum {id=0}; enum {hasDynamicSize = 0};									\
		static void ToMemory(const type& data, char* memory)					\
		{ memcpy(memory, &data, sizeof(type)); }								\
		static uint32 FromMemory(type& data, char* memory)						\
		{ memcpy(&data, memory, sizeof(type)); return sizeof(type); }			\
		static uint32 GetDynamicSize(const type&)								\
		{ return sizeof(type); }												\
	};

  /************************************************************************************************************************/
  /**
  * @brief	RTTIPlainType for std::vector.
  *
  * @see		RTTIPlainType
  */
  /************************************************************************************************************************/
  template<class T>
  struct RTTIPlainType<std::vector<T, StdAlloc<T>>>
  {
    enum { id = TYPEID_UTILITY::TID_Vector }; enum { hasDynamicSize = 1 };

    /************************************************************************************************************************/
    /**
    * @copydoc	RTTIPlainType::ToMemory
    */
    /************************************************************************************************************************/
    static void ToMemory(const std::vector<T, StdAlloc<T>>& data, char* memory)
    {
      uint32 size = sizeof(uint32);
      char* memoryStart = memory;
      memory += sizeof(uint32);

      uint32 numElements = (uint32)data.size();
      memcpy(memory, &numElements, sizeof(uint32));
      memory += sizeof(uint32);
      size += sizeof(uint32);

      for (auto iter = data.begin(); iter != data.end(); ++iter)
      {
        uint32 elementSize = RTTIPlainType<T>::GetDynamicSize(*iter);
        RTTIPlainType<T>::ToMemory(*iter, memory);

        memory += elementSize;
        size += elementSize;
      }

      memcpy(memoryStart, &size, sizeof(uint32));
    }

    /************************************************************************************************************************/
    /**
    * @copydoc	RTTIPlainType::FromMemory
    */
    /************************************************************************************************************************/
    static uint32 FromMemory(std::vector<T, StdAlloc<T>>& data, char* memory)
    {
      uint32 size = 0;
      memcpy(&size, memory, sizeof(uint32));
      memory += sizeof(uint32);

      uint32 numElements;
      memcpy(&numElements, memory, sizeof(uint32));
      memory += sizeof(uint32);

      for (uint32 i = 0; i < numElements; i++)
      {
        T element;
        uint32 elementSize = RTTIPlainType<T>::FromMemory(element, memory);
        data.push_back(element);

        memory += elementSize;
      }

      return size;
    }

    /************************************************************************************************************************/
    /**
    * @copydoc	RTTIPlainType::GetDynamicSize
    */
    /************************************************************************************************************************/
    static uint32 GetDynamicSize(const std::vector<T, StdAlloc<T>>& data)
    {
      SIZE_T dataSize = sizeof(uint32) * 2;

      for (auto iter = data.begin(); iter != data.end(); ++iter)
      {
        dataSize += RTTIPlainType<T>::GetDynamicSize(*iter);
      }

      GE_ASSERT(dataSize <= std::numeric_limits<uint32>::max());

      return (uint32)dataSize;
    }
  };

  /************************************************************************************************************************/
  /**
  * @brief	RTTIPlainType for std::set.
  *
  * @see		RTTIPlainType
  */
  /************************************************************************************************************************/
  template<class T>
  struct RTTIPlainType<std::set<T, std::less<T>, StdAlloc<T>>>
  {
    enum { id = TYPEID_UTILITY::TID_Set }; enum { hasDynamicSize = 1 };

    /************************************************************************************************************************/
    /**
    * @copydoc	RTTIPlainType::ToMemory
    */
    /************************************************************************************************************************/
    static void ToMemory(const std::vector<T, StdAlloc<T>>& data, char* memory)
    {
      uint32 size = sizeof(uint32);
      char* memoryStart = memory;
      memory += sizeof(uint32);

      uint32 numElements = (uint32)data.size();
      memcpy(memory, &numElements, sizeof(uint32));
      memory += sizeof(uint32);
      size += sizeof(uint32);

      for (auto iter = data.begin(); iter != data.end(); ++iter)
      {
        uint32 elementSize = RTTIPlainType<T>::GetDynamicSize(*iter);
        RTTIPlainType<T>::ToMemory(*iter, memory);

        memory += elementSize;
        size += elementSize;
      }

      memcpy(memoryStart, &size, sizeof(uint32));
    }

    /************************************************************************************************************************/
    /**
    * @copydoc	RTTIPlainType::FromMemory
    */
    /************************************************************************************************************************/
    static uint32 FromMemory(std::vector<T, StdAlloc<T>>& data, char* memory)
    {
      uint32 size = 0;
      memcpy(&size, memory, sizeof(uint32));
      memory += sizeof(uint32);

      uint32 numElements;
      memcpy(&numElements, memory, sizeof(uint32));
      memory += sizeof(uint32);

      for (uint32 i = 0; i < numElements; i++)
      {
        T element;
        uint32 elementSize = RTTIPlainType<T>::FromMemory(element, memory);
        data.insert(element);

        memory += elementSize;
      }

      return size;
    }

    /************************************************************************************************************************/
    /**
    * @copydoc	RTTIPlainType::GetDynamicSize
    */
    /************************************************************************************************************************/
    static uint32 GetDynamicSize(const std::vector<T, StdAlloc<T>>& data)
    {
      SIZE_T dataSize = sizeof(uint32) * 2;

      for (auto iter = data.begin(); iter != data.end(); ++iter)
      {
        dataSize += RTTIPlainType<T>::GetDynamicSize(*iter);
      }

      GE_ASSERT(dataSize <= std::numeric_limits<uint32>::max());

      return (uint32)dataSize;
    }
  };

  /************************************************************************************************************************/
  /**
  * @brief	RTTIPlainType for std::map.
  *
  * @see		RTTIPlainType
  */
  /************************************************************************************************************************/
  template<class Key, class Value>
  struct RTTIPlainType<std::map<Key, Value, std::less<Key>, StdAlloc<std::pair<const Key, Value>>>>
  {
    enum { id = TYPEID_UTILITY::TID_Map }; enum { hasDynamicSize = 1 };

    /************************************************************************************************************************/
    /**
    * @copydoc		RTTIPlainType::ToMemory
    */
    /************************************************************************************************************************/
    static void ToMemory(const std::map<Key, Value, std::less<Key>, StdAlloc<std::pair<const Key, Value>>>& data, char* memory)
    {
      uint32 size = sizeof(uint32);
      char* memoryStart = memory;
      memory += sizeof(uint32);

      uint32 numElements = (uint32)data.size();
      memcpy(memory, &numElements, sizeof(uint32));
      memory += sizeof(uint32);
      size += sizeof(uint32);

      for (auto iter = data.begin(); iter != data.end(); ++iter)
      {
        uint32 keySize = RTTIPlainType<Key>::GetDynamicSize(iter->first);
        RTTIPlainType<Key>::ToMemory(iter->first, memory);

        memory += keySize;
        size += keySize;

        uint32 valueSize = RTTIPlainType<Value>::GetDynamicSize(iter->second);
        RTTIPlainType<Value>::ToMemory(iter->second, memory);

        memory += valueSize;
        size += valueSize;
      }

      memcpy(memoryStart, &size, sizeof(uint32));
    }

    /************************************************************************************************************************/
    /**
    * @copydoc		RTTIPlainType::FromMemory
    */
    /************************************************************************************************************************/
    static uint32 FromMemory(std::map<Key, Value, std::less<Key>, StdAlloc<std::pair<const Key, Value>>>& data, char* memory)
    {
      uint32 size = 0;
      memcpy(&size, memory, sizeof(uint32));
      memory += sizeof(uint32);

      uint32 numElements;
      memcpy(&numElements, memory, sizeof(uint32));
      memory += sizeof(uint32);

      for (uint32 i = 0; i < numElements; i++)
      {
        Key key;
        uint32 keySize = RTTIPlainType<Key>::FromMemory(key, memory);
        memory += keySize;

        Value value;
        uint32 valueSize = RTTIPlainType<Value>::FromMemory(value, memory);
        memory += valueSize;

        data[key] = value;
      }

      return size;
    }

    /************************************************************************************************************************/
    /**
    * @copydoc		RTTIPlainType::GetDynamicSize
    */
    /************************************************************************************************************************/
    static uint32 GetDynamicSize(const std::map<Key, Value, std::less<Key>, StdAlloc<std::pair<const Key, Value>>>& data)
    {
      SIZE_T dataSize = sizeof(uint32) * 2;

      for (auto iter = data.begin(); iter != data.end(); ++iter)
      {
        dataSize += RTTIPlainType<Key>::GetDynamicSize(iter->first);
        dataSize += RTTIPlainType<Value>::GetDynamicSize(iter->second);
      }

      GE_ASSERT(dataSize <= std::numeric_limits<uint32>::max());

      return (uint32)dataSize;
    }
  };

  /************************************************************************************************************************/
  /**
  * @brief	RTTIPlainType for std::unordered_map.
  *
  * @see		RTTIPlainType
  */
  /************************************************************************************************************************/
  template<class Key, class Value>
  struct RTTIPlainType<std::unordered_map<Key, Value, std::hash<Key>, std::equal_to<Key>, StdAlloc<std::pair<const Key, Value>>>>
  {
    enum { id = TYPEID_UTILITY::TID_UnorderedMap }; enum { hasDynamicSize = 1 };

    typedef std::unordered_map<Key, Value, std::hash<Key>, std::equal_to<Key>, StdAlloc<std::pair<const Key, Value>>> MapType;

    /************************************************************************************************************************/
    /**
    * @copydoc		RTTIPlainType::ToMemory
    */
    /************************************************************************************************************************/
    static void ToMemory(MapType& data, char* memory)
    {
      uint32 size = sizeof(uint32);
      char* memoryStart = memory;
      memory += sizeof(uint32);

      uint32 numElements = (uint32)data.size();
      memcpy(memory, &numElements, sizeof(uint32));
      memory += sizeof(uint32);
      size += sizeof(uint32);

      for (auto iter = data.begin(); iter != data.end(); ++iter)
      {
        uint32 keySize = RTTIPlainType<Key>::GetDynamicSize(iter->first);
        RTTIPlainType<Key>::ToMemory(iter->first, memory);

        memory += keySize;
        size += keySize;

        uint32 valueSize = RTTIPlainType<Value>::GetDynamicSize(iter->second);
        RTTIPlainType<Value>::ToMemory(iter->second, memory);

        memory += valueSize;
        size += valueSize;
      }

      memcpy(memoryStart, &size, sizeof(uint32));
    }

    /************************************************************************************************************************/
    /**
    * @copydoc		RTTIPlainType::FromMemory
    */
    /************************************************************************************************************************/
    static uint32 FromMemory(MapType& data, char* memory)
    {
      uint32 size = 0;
      memcpy(&size, memory, sizeof(uint32));
      memory += sizeof(uint32);

      uint32 numElements;
      memcpy(&numElements, memory, sizeof(uint32));
      memory += sizeof(uint32);

      for (uint32 i = 0; i < numElements; i++)
      {
        Key key;
        uint32 keySize = RTTIPlainType<Key>::FromMemory(key, memory);
        memory += keySize;

        Value value;
        uint32 valueSize = RTTIPlainType<Value>::FromMemory(value, memory);
        memory += valueSize;

        data[key] = value;
      }

      return size;
    }

    /************************************************************************************************************************/
    /**
    * @copydoc		RTTIPlainType::GetDynamicSize
    */
    /************************************************************************************************************************/
    static uint32 GetDynamicSize(const MapType& data)
    {
      SIZE_T dataSize = sizeof(uint32) * 2;

      for (auto iter = data.begin(); iter != data.end(); ++iter)
      {
        dataSize += RTTIPlainType<Key>::GetDynamicSize(iter->first);
        dataSize += RTTIPlainType<Value>::GetDynamicSize(iter->second);
      }

      GE_ASSERT(dataSize <= std::numeric_limits<uint32>::max());

      return (uint32)dataSize;
    }
  };

  /************************************************************************************************************************/
  /**
  * @brief	RTTIPlainType for std::unordered_set.
  *
  * @see		RTTIPlainType
  */
  /************************************************************************************************************************/
  template<class Key>
  struct RTTIPlainType<std::unordered_set<Key, std::hash<Key>, std::equal_to<Key>, StdAlloc<Key>>>
  {
    enum { id = TYPEID_UTILITY::TID_UnorderedSet }; enum { hasDynamicSize = 1 };

    typedef std::unordered_set<Key, std::hash<Key>, std::equal_to<Key>, StdAlloc<Key>> MapType;

    /************************************************************************************************************************/
    /**
    * @copydoc		RTTIPlainType::ToMemory
    */
    /************************************************************************************************************************/
    static void ToMemory(MapType& data, char* memory)
    {
      uint32 size = sizeof(uint32);
      char* memoryStart = memory;
      memory += sizeof(uint32);

      uint32 numElements = (uint32)data.size();
      memcpy(memory, &numElements, sizeof(uint32));
      memory += sizeof(uint32);
      size += sizeof(uint32);

      for (auto iter = data.begin(); iter != data.end(); ++iter)
      {
        uint32 keySize = RTTIPlainType<Key>::GetDynamicSize(*iter);
        RTTIPlainType<Key>::ToMemory(*iter, memory);

        memory += keySize;
        size += keySize;
      }

      memcpy(memoryStart, &size, sizeof(uint32));
    }

    /************************************************************************************************************************/
    /**
    * @copydoc		RTTIPlainType::FromMemory
    */
    /************************************************************************************************************************/
    static uint32 FromMemory(MapType& data, char* memory)
    {
      uint32 size = 0;
      memcpy(&size, memory, sizeof(uint32));
      memory += sizeof(uint32);

      uint32 numElements;
      memcpy(&numElements, memory, sizeof(uint32));
      memory += sizeof(uint32);

      for (uint32 i = 0; i<numElements; i++)
      {
        Key key;
        uint32 keySize = RTTIPlainType<Key>::FromMemory(key, memory);
        memory += keySize;

        data.insert(key);
      }

      return size;
    }

    /************************************************************************************************************************/
    /**
    * @copydoc		RTTIPlainType::GetDynamicSize
    */
    /************************************************************************************************************************/
    static uint32 GetDynamicSize(const MapType& data)
    {
      SIZE_T dataSize = sizeof(uint32) * 2;

      for (auto iter = data.begin(); iter != data.end(); ++iter)
      {
        dataSize += RTTIPlainType<Key>::GetDynamicSize(*iter);
      }

      GE_ASSERT(dataSize <= std::numeric_limits<uint32>::max());

      return (uint32)dataSize;
    }
  };

  /************************************************************************************************************************/
  /**
  * @brief	RTTIPlainType for std::pair.
  *
  * @see		RTTIPlainType
  */
  /************************************************************************************************************************/
  template<class A, class B>
  struct RTTIPlainType<std::pair<A, B>>
  {
    enum { id = TYPEID_UTILITY::TID_Pair }; enum { hasDynamicSize = 1 };

    /************************************************************************************************************************/
    /**
    * @copydoc		RTTIPlainType::ToMemory
    */
    /************************************************************************************************************************/
    static void ToMemory(const std::pair<A, B>& data, char* memory)
    {
      uint32 size = sizeof(uint32);
      char* memoryStart = memory;
      memory += sizeof(uint32);

      uint32 firstSize = RTTIPlainType<A>::GetDynamicSize(data.first);
      RTTIPlainType<A>::ToMemory(data.first, memory);

      memory += firstSize;
      size += firstSize;

      uint32 secondSize = RTTIPlainType<B>::GetDynamicSize(data.second);
      RTTIPlainType<B>::ToMemory(data.second, memory);

      memory += secondSize;
      size += secondSize;

      memcpy(memoryStart, &size, sizeof(uint32));
    }

    /************************************************************************************************************************/
    /**
    * @copydoc		RTTIPlainType::FromMemory
    */
    /************************************************************************************************************************/
    static uint32 FromMemory(std::pair<A, B>& data, char* memory)
    {
      uint32 size = 0;
      memcpy(&size, memory, sizeof(uint32));
      memory += sizeof(uint32);

      uint32 firstSize = RTTIPlainType<A>::FromMemory(data.first, memory);
      memory += firstSize;

      uint32 secondSize = RTTIPlainType<B>::FromMemory(data.second, memory);
      memory += secondSize;

      return size;
    }

    /************************************************************************************************************************/
    /**
    * @copydoc		RTTIPlainType::GetDynamicSize
    */
    /************************************************************************************************************************/
    static uint32 GetDynamicSize(const std::pair<A, B>& data)
    {
      SIZE_T dataSize = sizeof(uint32);

      dataSize += RTTIPlainType<A>::GetDynamicSize(data.first);
      dataSize += RTTIPlainType<B>::GetDynamicSize(data.second);

      GE_ASSERT(dataSize <= std::numeric_limits<uint32>::max());

      return (uint32)dataSize;
    }
  };
}
