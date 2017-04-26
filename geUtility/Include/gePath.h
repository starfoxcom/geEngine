/********************************************************************/
/**
 * @file   gePath.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/03/06
 * @brief  Class for storing and manipulating file paths
 *
 * Class for storing and manipulating file paths. Paths may be
 * parsed from and to raw strings according to various platform
 * specific path types.
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Includes																												*/
/************************************************************************************************************************/
namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Class for storing and manipulating file paths. Paths may be parsed from and to raw strings according to
	*			various platform specific path types.
	*
	* @note		In order to allow the system to easily distinguish between file and directory paths, try to ensure that all
	*			directory paths end with a separator (\ or / depending on platform).
	*
	*			System won't fail if you don't but it will be easier to misuse.
	*/
	/************************************************************************************************************************/
	namespace PATH_TYPE
	{
		enum E
		{
			Windows,
			Unix,
			Default
		};
	}
	
	class GE_UTILITY_EXPORT Path
	{
	public:

		static const Path BLANK;

	private:
		friend struct RTTIPlainType<Path>;		//For serialization
		friend struct ::std::hash<geEngineSDK::Path>;

		Vector<WString> m_Directories;
		WString m_Device;
		WString m_Filename;
		WString m_Node;
		bool m_IsAbsolute;

	public:
		Path();

		/************************************************************************************************************************/
		/**
		* @brief	Constructs a path by parsing the provided path string. Throws exception if provided path is not valid.
		* @param[in]	type	If set to default path will be parsed according to the rules of the platform the application
		*						is being compiled to. Otherwise it will be parsed according to provided type.
		*/
		/************************************************************************************************************************/
		Path(const WString& pathStr, PATH_TYPE::E type = PATH_TYPE::Default);
		Path(const String& pathStr, PATH_TYPE::E type = PATH_TYPE::Default);
		Path(UNICHAR* pathStr, PATH_TYPE::E type = PATH_TYPE::Default);
		Path(const ANSICHAR* pathStr, PATH_TYPE::E type = PATH_TYPE::Default);
		
		/************************************************************************************************************************/
		/**
		* @brief	Copy constructor
		*/
		/************************************************************************************************************************/
		Path(const Path& other);

		/************************************************************************************************************************/
		/**
		* @brief	Assigns a path by parsing the provided path string. Path will be parsed according to the rules of the platform
		*			the application is being compiled to.
		*/
		/************************************************************************************************************************/
		Path& operator= (const WString& pathStr);
		Path& operator= (const String& pathStr);
		Path& operator= (const UNICHAR* pathStr);
		Path& operator= (const ANSICHAR* pathStr);
		Path& operator= (const Path& path);

		/************************************************************************************************************************/
		/**
		* @brief	Compares two paths and returns true if they match. Comparison is case insensitive and paths will be compared
		*			as-is, without canonization.
		*/
		/************************************************************************************************************************/
		bool operator== (const Path& path) const
		{
			return Equals(path);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Compares two paths and returns true if they don't match. Comparison is case insensitive and paths will be
		*			compared as-is, without canonization.
		*/
		/************************************************************************************************************************/
		bool operator!= (const Path& path) const
		{
			return !Equals(path);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Gets a directory name with the specified index from the path.
		*/
		/************************************************************************************************************************/
		const WString& operator[] (uint32 idx) const
		{
			return GetWDirectory(idx);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Swap internal data with another Path object.
		*/
		/************************************************************************************************************************/
		void Swap(Path& path);

		/************************************************************************************************************************/
		/**
		* @brief	Create a path from another Path object.
		*/
		/************************************************************************************************************************/
		void Assign(const Path& path);

		/************************************************************************************************************************/
		/**
		* @brief	Constructs a path by parsing the provided path string. Throws exception if provided path is not valid.
		* @param[in]	type	If set to default path will be parsed according to the rules of the platform the application
		*						is being compiled to. Otherwise it will be parsed according to provided type.
		*/
		/************************************************************************************************************************/
		void Assign(const WString& pathStr, PATH_TYPE::E type = PATH_TYPE::Default);
		void Assign(const String& pathStr, PATH_TYPE::E type = PATH_TYPE::Default);
		void Assign(const UNICHAR* pathStr, PATH_TYPE::E type = PATH_TYPE::Default);
		void Assign(const ANSICHAR* pathStr, PATH_TYPE::E type = PATH_TYPE::Default);

		/************************************************************************************************************************/
		/**
		* @brief	Converts the path in a string according to platform path rules.
		* @param[in] type	If set to default path will be parsed according to the rules of the platform the application is
		*					being compiled to. Otherwise it will be parsed according to provided type.
		*/
		/************************************************************************************************************************/
		WString ToWString(PATH_TYPE::E type = PATH_TYPE::Default) const;
		String ToString(PATH_TYPE::E type = PATH_TYPE::Default) const;

		/************************************************************************************************************************/
		/**
		* @brief	Checks is the path a directory (contains no file-name).
		*/
		/************************************************************************************************************************/
		bool IsDirectory() const
		{
			return m_Filename.empty();
		}

		/************************************************************************************************************************/
		/**
		* @brief	Checks does the path point to a file.
		*/
		/************************************************************************************************************************/
		bool IsFile() const
		{
			return !m_Filename.empty();
		}

		/************************************************************************************************************************/
		/**
		* @brief	Checks is the contained path absolute.
		*/
		/************************************************************************************************************************/
		bool IsAbsolute() const
		{
			return m_IsAbsolute;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns parent path. If current path points to a file the parent path will be the folder where the file is
		*			located. Or if it contains a directory the parent will be the parent directory. If no parent exists, same
		*			path will be returned.
		*/
		/************************************************************************************************************************/
		Path GetParent() const;

		/************************************************************************************************************************/
		/**
		* @brief	Returns an absolute path by appending the current path to the provided base. If path was already absolute no
		*			changes are made and copy of current path is returned. If base is not absolute, then the returned path will
		*			be made relative to base, but will not be absolute.
		*/
		/************************************************************************************************************************/
		Path GetAbsolute(const Path& base) const;

		/************************************************************************************************************************/
		/**
		* @brief	Returns a relative path by making the current path relative to the provided base. Base must be a part of the
		*			current path. If base isn't a part of the path no changes are made and a copy of the current path is returned
		*/
		/************************************************************************************************************************/
		Path GetRelative(const Path& base) const;

		/************************************************************************************************************************/
		/**
		* @brief	Returns the path as a path to directory. If path was pointing to a file, the filename is removed, otherwise
		*			no changes are made and exact copy is returned.
		*/
		/************************************************************************************************************************/
		Path GetDirectory() const;

		/************************************************************************************************************************/
		/**
		* @brief	Makes the path the parent of the current path. If current path points to a file the parent path will be the
		*			folder where the file is located. Or if it contains a directory the parent will be the parent directory.
		*			If no parent exists, same path will be returned.
		*/
		/************************************************************************************************************************/
		Path& MakeParent();

		/************************************************************************************************************************/
		/**
		* @brief	Makes the current path absolute by appending it to base. If path was already absolute no changes are made and
		*			copy of current path is returned. If base is not absolute, then the returned path will be made relative to
		*			base, but will not be absolute.
		*/
		/************************************************************************************************************************/
		Path& MakeAbsolute(const Path& base);

		/************************************************************************************************************************/
		/**
		* @brief	Makes the current path relative to the provided base. Base must be a part of the current path. If base is not
		*			a part of the path no changes are made and a copy of the current path is returned.
		*/
		/************************************************************************************************************************/
		Path& MakeRelative(const Path& base);

		/************************************************************************************************************************/
		/**
		* @brief	Appends another path to the end of this path.
		*/
		/************************************************************************************************************************/
		Path& Append(const Path& path);

		/************************************************************************************************************************/
		/**
		* @brief	Checks if the current path contains the provided path. Comparison is case insensitive and paths will be
		*			compared as-is, without canonization.
		*/
		/************************************************************************************************************************/
		bool Includes(const Path& child) const;

		/************************************************************************************************************************/
		/**
		* @brief	Compares two paths and returns true if they match. Comparison is case insensitive and paths will be compared
		*			as-is, without canonization.
		*/
		/************************************************************************************************************************/
		bool Equals(const Path& other) const;

		/************************************************************************************************************************/
		/**
		* @brief	Change or set the filename in the path.
		*/
		/************************************************************************************************************************/
		void SetFilename(const WString& filename)
		{
			m_Filename = filename;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Change or set the filename in the path.
		*/
		/************************************************************************************************************************/
		void SetFilename(const String& filename)
		{
			m_Filename = geEngineSDK::ToWString(filename);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Change or set the base name in the path. Base name changes the filename by changing its base to the provided
		*			value but keeping extension intact.
		*/
		/************************************************************************************************************************/
		void SetBasename(const WString& basename);

		/************************************************************************************************************************/
		/**
		* @brief	Change or set the base name in the path. Base name changes the filename by changing its base to the provided
		*			value but keeping extension intact.
		*/
		/************************************************************************************************************************/
		void SetBasename(const String& basename);

		/************************************************************************************************************************/
		/**
		* @brief	Change or set the extension of the filename in the path.
		* @param[in]	extension	Extension with a leading ".".
		*/
		/************************************************************************************************************************/
		void SetExtension(const WString& extension);

		/************************************************************************************************************************/
		/**
		* @brief	Change or set the extension of the filename in the path.
		* @param[in]	extension	Extension with a leading ".".
		*/
		/************************************************************************************************************************/
		void SetExtension(const String& extension);

		void SetNode(const WString& node)
		{
			m_Node = node;
		}

		void SetNode(const String& node)
		{
			m_Node = geEngineSDK::ToWString(node);
		}

		void SetDevice(const WString& device)
		{
			m_Device = device;
		}

		void SetDevice(const String& device)
		{
			m_Device = geEngineSDK::ToWString(device);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns a filename in the path.
		* @param[in]	extension	If true, returned filename will contain an extension.
		*/
		/************************************************************************************************************************/
		WString GetWFilename(bool extension = true) const;

		/************************************************************************************************************************/
		/**
		* @brief	Returns a filename in the path.
		* @param[in]	extension	If true, returned filename will contain an extension.
		*/
		/************************************************************************************************************************/
		String GetFilename(bool extension = true) const;

		/************************************************************************************************************************/
		/**
		* @brief	Returns file extension with the leading "."
		*/
		/************************************************************************************************************************/
		WString GetWExtension() const;

		/************************************************************************************************************************/
		/**
		* @brief	Returns file extension with the leading "."
		*/
		/************************************************************************************************************************/
		String GetExtension() const;

		/************************************************************************************************************************/
		/**
		* @brief	Gets the number of directories in the path.
		*/
		/************************************************************************************************************************/
		uint32 GetNumDirectories() const
		{
			return (uint32)m_Directories.size();
		}

		/************************************************************************************************************************/
		/**
		* @brief	Gets a directory name with the specified index from the path.
		*/
		/************************************************************************************************************************/
		const WString& GetWDirectory(uint32 idx) const;

		/************************************************************************************************************************/
		/**
		* @brief	Gets a directory name with the specified index from the path.
		*/
		/************************************************************************************************************************/
		String GetDirectory(uint32 idx) const;

		/************************************************************************************************************************/
		/**
		* @brief	Returns path device (e.g. drive, volume, etc.) if one exists in the path.
		*/
		/************************************************************************************************************************/
		const WString& GetWDevice() const
		{
			return m_Device;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns path device (e.g. drive, volume, etc.) if one exists in the path.
		*/
		/************************************************************************************************************************/
		String GetDevice() const
		{
			return geEngineSDK::ToString(m_Device);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns path node (e.g. network name) if one exists in the path.
		*/
		/************************************************************************************************************************/
		const WString& GetWNode() const
		{
			return m_Node;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns path node (e.g. network name) if one exists in the path.
		*/
		/************************************************************************************************************************/
		String GetNode() const
		{
			return geEngineSDK::ToString(m_Node);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Gets last element in the path, filename if it exists, otherwise the last directory. If no directories exist
		*			returns device or node.
		* @param[in]	type	Determines format of node or device, in case they are returned. When default, format for
		*						the active platform will be used, otherwise the format defined by the parameter will be used.
		*/
		/************************************************************************************************************************/
		WString GetWTail(PATH_TYPE::E type = PATH_TYPE::Default) const;

		/************************************************************************************************************************/
		/**
		* @brief	Gets last element in the path, filename if it exists, otherwise the last directory. If no directories exist
		*			returns device or node.
		* @param[in]	type	Determines format of node or device, in case they are returned. When default, format for the
		*						active platform will be used, otherwise the format defined by the parameter will be used.
		*/
		/************************************************************************************************************************/
		String GetTail(PATH_TYPE::E type = PATH_TYPE::Default) const;

		/************************************************************************************************************************/
		/**
		* @brief	Clears the path to nothing.
		*/
		/************************************************************************************************************************/
		void Clear();

		/************************************************************************************************************************/
		/**
		* @brief	Returns true if no path has been set.
		*/
		/************************************************************************************************************************/
		bool IsEmpty() const
		{
			return m_Directories.empty() && m_Filename.empty() && m_Device.empty() && m_Node.empty();
		}

		/************************************************************************************************************************/
		/**
		* @brief	Concatenates two paths.
		*/
		/************************************************************************************************************************/
		Path operator+ (const Path& rhs) const;
		Path& operator+= (const Path& rhs);

		/************************************************************************************************************************/
		/**
		* @brief	Compares two path elements (i.e. filenames, directory names, etc.)
		*/
		/************************************************************************************************************************/
		static bool ComparePathElem(const WString& left, const WString& right);

		/************************************************************************************************************************/
		/**
		* @brief	Combines two paths and returns the result. Right path should be relative.
		*/
		/************************************************************************************************************************/
		static Path Combine(const Path& left, const Path& right);

	private:
		/************************************************************************************************************************/
		/**
		* @brief	Constructs a path by parsing the provided raw string data. Throws exception if provided path is not valid.
		* @param[in]	type	If set to default path will be parsed according to the rules of the platform the application
		*						is being compiled to. Otherwise it will be parsed according to provided type.
		*/
		/************************************************************************************************************************/
		void Assign(const UNICHAR* pathStr, uint32 numChars, PATH_TYPE::E type = PATH_TYPE::Default);
		void Assign(const ANSICHAR* pathStr, uint32 numChars, PATH_TYPE::E type = PATH_TYPE::Default);

		/************************************************************************************************************************/
		/**
		* @brief	Parses a Windows path and stores the parsed data internally. Throws an exception if parsing fails.
		*/
		/************************************************************************************************************************/
		template<class T>
		void ParseWindows(const T* pathStr, uint32 numChars)
		{
			Clear();

			uint32 idx = 0;
			BasicStringStream<T> tempStream;

			if( idx < numChars )
			{
				if( pathStr[idx] == '\\' || pathStr[idx] == '/' )
				{
					m_IsAbsolute = true;
					idx++;
				}
			}

			if( idx < numChars )
			{
				//Path starts with a node, a drive letter or is relative
				if( m_IsAbsolute && (pathStr[idx] == '\\' || pathStr[idx] == '/') )
				{//Node
					idx++;
					tempStream.str( BasicString<T>() );
					tempStream.clear();
					
					while( idx < numChars && pathStr[idx] != '\\' && pathStr[idx] != '/' )
					{
						tempStream << pathStr[idx++];
					}

					SetNode( tempStream.str() );

					if( idx < numChars )
					{
						idx++;
					}
				}
				else
				{//A drive letter or not absolute
					T drive = pathStr[idx];
					idx++;

					if( idx < numChars && pathStr[idx] == ':' )
					{
						if( m_IsAbsolute || !((drive >= 'a' && drive <= 'z') || (drive >= 'A' && drive <= 'Z')) )
						{//The drive letter is not valid
							throwInvalidPathException(BasicString<T>(pathStr, numChars));
						}

						m_IsAbsolute = true;
						SetDevice( geEngineSDK::ToWString(drive) );

						idx++;
						if( idx >= numChars || (pathStr[idx] != '\\' && pathStr[idx] != '/') )
						{//The path does not end with a trailing slash
							throwInvalidPathException(BasicString<T>(pathStr, numChars));
						}

						idx++;
					}
					else
					{
						idx--;
					}
				}

				while( idx < numChars )
				{
					tempStream.str( BasicString<T>() );
					tempStream.clear();
					while( idx < numChars && pathStr[idx] != '\\' && pathStr[idx] != '/' )
					{
						tempStream << pathStr[idx];
						idx++;
					}

					if( idx < numChars )
					{
						PushDirectory( tempStream.str() );
					}
					else
					{
						SetFilename(tempStream.str());
					}

					idx++;
				}
			}
		}

		/************************************************************************************************************************/
		/**
		* @brief	Parses a Unix path and stores the parsed data internally. Throws an exception if parsing fails.
		*/
		/************************************************************************************************************************/
		template<class T>
		void ParseUnix(const T* pathStr, uint32 numChars)
		{
			Clear();

			uint32 idx = 0;
			BasicStringStream<T> tempStream;

			if( idx < numChars )
			{
				if( pathStr[idx] == '/' )
				{
					m_IsAbsolute = true;
					idx++;
				}
				else if( pathStr[idx] == '~' )
				{
					idx++;
					if( idx >= numChars || pathStr[idx] == '/' )
					{
						PushDirectory(geEngineSDK::ToWString('~'));
						m_IsAbsolute = true;
					}
					else
					{
						idx--;
					}
				}

				while( idx < numChars )
				{
					tempStream.str( BasicString<T>() );
					tempStream.clear();

					while( idx < numChars && pathStr[idx] != '/' )
					{
						tempStream << pathStr[idx];
						idx++;
					}

					if( idx < numChars )
					{
						if( m_Directories.empty() )
						{
							BasicString<T> deviceStr = tempStream.str();
							if( !deviceStr.empty() && *(deviceStr.rbegin()) == ':' )
							{
								SetDevice(deviceStr.substr(0, deviceStr.length() - 1));
								m_IsAbsolute = true;
							}
							else
							{
								PushDirectory(deviceStr);
							}
						}
						else
						{
							PushDirectory(tempStream.str());
						}
					}
					else
					{
						SetFilename(tempStream.str());
					}

					idx++;
				}
			}
		}

		/************************************************************************************************************************/
		/**
		* @brief	Build a Windows path string from internal path data.
		*/
		/************************************************************************************************************************/
		WString BuildWindows() const;

		/************************************************************************************************************************/
		/**
		* @brief	Build a Unix path string from internal path data.
		*/
		/************************************************************************************************************************/
		WString BuildUnix() const;

		/************************************************************************************************************************/
		/**
		* @brief	Add new directory to the end of the path.
		*/
		/************************************************************************************************************************/
		void PushDirectory(const WString& dir);
		void PushDirectory(const String& dir);

		/************************************************************************************************************************/
		/**
		* @brief	Helper method that throws invalid path exception.
		*/
		/************************************************************************************************************************/
		void throwInvalidPathException(const WString& path) const;
		void throwInvalidPathException(const String& path) const;
	};

	/************************************************************************************************************************/
	/**
	* @brief	RTTIPlainType specialization for Path that allows paths be serialized as value types.
	* @see		RTTIPlainType
	*/
	/************************************************************************************************************************/
	template<> struct RTTIPlainType<Path>
	{
		enum { id = TYPEID_UTILITY::TID_Path }; enum { hasDynamicSize = 1 };

		static void ToMemory(const Path& data, char* memory)
		{
			uint32 size = GetDynamicSize(data);
			memcpy(memory, &size, sizeof(uint32));
			memory += sizeof(uint32);

			memory = RTTIWriteElement(data.m_Device, memory);
			memory = RTTIWriteElement(data.m_Node, memory);
			memory = RTTIWriteElement(data.m_Filename, memory);
			memory = RTTIWriteElement(data.m_IsAbsolute, memory);
			memory = RTTIWriteElement(data.m_Directories, memory);
		}

		static uint32 FromMemory(Path& data, char* memory)
		{
			uint32 size;
			memcpy(&size, memory, sizeof(uint32));
			memory += sizeof(uint32);

			memory = RTTIReadElement(data.m_Device, memory);
			memory = RTTIReadElement(data.m_Node, memory);
			memory = RTTIReadElement(data.m_Filename, memory);
			memory = RTTIReadElement(data.m_IsAbsolute, memory);
			memory = RTTIReadElement(data.m_Directories, memory);

			return size;
		}

		static uint32 GetDynamicSize(const Path& data)
		{
			SIZE_T dataSize =	RTTIGetElementSize(data.m_Device) + 
								RTTIGetElementSize(data.m_Node) + 
								RTTIGetElementSize(data.m_Filename) + 
								RTTIGetElementSize(data.m_IsAbsolute) + 
								RTTIGetElementSize(data.m_Directories) + sizeof(uint32);

#if GE_DEBUG_MODE
			if( dataSize > std::numeric_limits<uint32>::max() )
			{
				__string_throwDataOverflowException();
			}
#endif
			return (uint32)dataSize;
		}
	};
}

/************************************************************************************************************************/
/**
* @brief	Hash value generator for Path.
*/
/************************************************************************************************************************/
template<>
struct std::hash<geEngineSDK::Path>
{
	size_t operator()(const geEngineSDK::Path& path) const
	{
		size_t hash = 0;
		geEngineSDK::Hash_Combine(hash, path.m_Filename);
		geEngineSDK::Hash_Combine(hash, path.m_Device);
		geEngineSDK::Hash_Combine(hash, path.m_Node);

		for( auto& dir : path.m_Directories )
		{
			geEngineSDK::Hash_Combine(hash, dir);
		}

		return hash;
	}
};
