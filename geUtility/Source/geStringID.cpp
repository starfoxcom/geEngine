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

/************************************************************************************************************************/
/* Includes																												*/
/************************************************************************************************************************/
#include "geStringID.h"

namespace geEngineSDK
{
	const StringID StringID::NONE = StringID();
	
	volatile StringID::InitStatics StringID::m_InitStatics = StringID::InitStatics();
	StringID::InternalData* StringID::m_StringHashTable[HASH_TABLE_SIZE];
	StringID::InternalData* StringID::m_Chunks[MAX_CHUNK_COUNT];

	uint32 StringID::m_NextId = 0;
	uint32 StringID::m_NumChunks = 0;
	SpinLock StringID::m_Sync;

	StringID::InitStatics::InitStatics()
	{
		ScopedSpinLock lock(m_Sync);

		memset(m_StringHashTable, 0, sizeof(m_StringHashTable));
		memset(m_Chunks, 0, sizeof(m_Chunks));

		m_Chunks[0] = (InternalData*)ge_alloc(sizeof(InternalData) * ELEMENTS_PER_CHUNK);
		memset(m_Chunks[0], 0, sizeof(InternalData) * ELEMENTS_PER_CHUNK);

		m_NumChunks++;
	}

	StringID::StringID() : m_Data(nullptr)
	{
	}

	template<class T>
	void StringID::Construct(T const& name)
	{
		GE_ASSERT(StringIDUtil<T>::Size(name) <= STRING_SIZE);

		uint32 hash = CalcHash(name) & (sizeof(m_StringHashTable) / sizeof(m_StringHashTable[0]) - 1);
		InternalData* existingEntry = m_StringHashTable[hash];

		while( existingEntry != nullptr )
		{
			if( StringIDUtil<T>::Compare(name, existingEntry->Chars) )
			{
				m_Data = existingEntry;
				return;
			}

			existingEntry = existingEntry->Next;
		}

		ScopedSpinLock lock(m_Sync);

		//Search for the value again in case other thread just added it
		existingEntry = m_StringHashTable[hash];
		InternalData* lastEntry = nullptr;
		while( existingEntry != nullptr )
		{
			if( StringIDUtil<T>::Compare(name, existingEntry->Chars) )
			{
				m_Data = existingEntry;
				return;
			}

			lastEntry = existingEntry;
			existingEntry = existingEntry->Next;
		}

		m_Data = AllocEntry();
		StringIDUtil<T>::Copy(name, m_Data->Chars);

		if( lastEntry == nullptr )
		{
			m_StringHashTable[hash] = m_Data;
		}
		else
		{
			lastEntry->Next = m_Data;
		}
	}

	template<class T>
	uint32 StringID::CalcHash(T const& input)
	{
		uint32 size = StringIDUtil<T>::Size(input);

		uint32 hash = 0;
		for( uint32 i=0; i<size; i++ )
		{
			hash = hash * 101 + input[i];
		}

		return hash;
	}

	StringID::InternalData* StringID::AllocEntry()
	{
		uint32 chunkIdx = m_NextId / ELEMENTS_PER_CHUNK;

		GE_ASSERT(chunkIdx < MAX_CHUNK_COUNT);
		GE_ASSERT(chunkIdx <= m_NumChunks); //Can only increment sequentially

		if( chunkIdx >= m_NumChunks )
		{
			m_Chunks[chunkIdx] = (InternalData*)ge_alloc(sizeof(InternalData) * ELEMENTS_PER_CHUNK);
			memset(m_Chunks[chunkIdx], 0, sizeof(InternalData) * ELEMENTS_PER_CHUNK);
			m_NumChunks++;
		}

		InternalData* chunk = m_Chunks[chunkIdx];
		uint32 chunkSpecificIndex = m_NextId % ELEMENTS_PER_CHUNK;

		InternalData* newEntry = &chunk[chunkSpecificIndex];
		newEntry->Id = m_NextId++;
		newEntry->Next = nullptr;

		return newEntry;
	}

	template<>
	class StringID::StringIDUtil<const ANSICHAR*>
	{
	public:
		static uint32 Size(const ANSICHAR* const& input){ return (uint32)strlen(input); }
		static void Copy(const ANSICHAR* const& input, ANSICHAR* dest) { memcpy(dest, input, strlen(input) + 1); }
		static bool Compare(const ANSICHAR* const& a, ANSICHAR* b) { return strcmp(a, b) == 0; }
	};

	template<>
	class StringID::StringIDUtil<String>
	{
	public:
		static uint32 Size(String const& input) { return (uint32)input.length(); }
		static void Copy(String const& input, ANSICHAR* dest)
		{
			uint32 len = (uint32)input.length();
			input.copy(dest, len);
			dest[len] = '\0';
		}
		static bool Compare(String const& a, ANSICHAR* b) { return a.compare(b) == 0; }
	};

	template class StringID::StringIDUtil<const ANSICHAR*>;
	template class StringID::StringIDUtil<String>;

	template GE_UTILITY_EXPORT void StringID::Construct(const ANSICHAR* const&);
	template GE_UTILITY_EXPORT void StringID::Construct(String const&);

	template GE_UTILITY_EXPORT uint32 StringID::CalcHash(const ANSICHAR* const&);
	template GE_UTILITY_EXPORT uint32 StringID::CalcHash(String const&);
}
