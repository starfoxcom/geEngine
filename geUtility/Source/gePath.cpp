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

/************************************************************************************************************************/
/* Includes																												*/
/************************************************************************************************************************/
#include "gePrerequisitesUtil.h"
#include "geException.h"

namespace geEngineSDK
{
	const Path Path::BLANK = Path();

	Path::Path() : m_IsAbsolute(false)
	{
	}

	Path::Path(const WString& pathStr, PATH_TYPE::E type)
	{
		Assign(pathStr, type);
	}

	Path::Path(const String& pathStr, PATH_TYPE::E type)
	{
		Assign(pathStr, type);
	}

	Path::Path(UNICHAR* pathStr, PATH_TYPE::E type)
	{
		Assign(pathStr, type);
	}

	Path::Path(const ANSICHAR* pathStr, PATH_TYPE::E type)
	{
		Assign(pathStr, type);
	}

	Path::Path(const Path& other)
	{
		Assign(other);
	}

	Path& Path::operator= (const Path& path)
	{
		Assign(path);
		return *this;
	}

	Path& Path::operator= (const WString& pathStr)
	{
		Assign(pathStr);
		return *this;
	}

	Path& Path::operator= (const String& pathStr)
	{
		Assign(pathStr);
		return *this;
	}

	Path& Path::operator= (const UNICHAR* pathStr)
	{
		Assign(pathStr);
		return *this;
	}

	Path& Path::operator= (const ANSICHAR* pathStr)
	{
		Assign(pathStr);
		return *this;
	}

	void Path::Swap(Path& path)
	{
		std::swap(m_Directories, path.m_Directories);
		std::swap(m_Filename, path.m_Filename);
		std::swap(m_Device, path.m_Device);
		std::swap(m_Node, path.m_Node);
		std::swap(m_IsAbsolute, path.m_IsAbsolute);
	}

	void Path::Assign(const Path& path)
	{
		m_Directories = path.m_Directories;
		m_Filename = path.m_Filename;
		m_Device = path.m_Device;
		m_Node = path.m_Node;
		m_IsAbsolute = path.m_IsAbsolute;
	}

	void Path::Assign(const WString& pathStr, PATH_TYPE::E type)
	{
		Assign(pathStr.data(), (uint32)pathStr.length(), type);
	}

	void Path::Assign(const String& pathStr, PATH_TYPE::E type)
	{
		Assign(pathStr.data(), (uint32)pathStr.length(), type);
	}

	void Path::Assign(const UNICHAR* pathStr, PATH_TYPE::E type)
	{
		Assign(pathStr, (uint32)wcslen(pathStr), type);
	}

	void Path::Assign(const ANSICHAR* pathStr, PATH_TYPE::E type)
	{
		Assign(pathStr, (uint32)strlen(pathStr), type);
	}

	void Path::Assign(const UNICHAR* pathStr, uint32 numChars, PATH_TYPE::E type)
	{
		switch(type)
		{
		case PATH_TYPE::Windows:
			ParseWindows(pathStr, numChars);
			break;
		case PATH_TYPE::Unix:
			ParseUnix(pathStr, numChars);
			break;
		case PATH_TYPE::Default:
		default:
#if GE_PLATFORM == GE_PLATFORM_WIN32
			ParseWindows(pathStr, numChars);
#elif GE_PLATFORM == GE_PLATFORM_APPLE || GE_PLATFORM == GE_PLATFORM_LINUX || GE_PLATFORM == GE_PLATFORM_PS4
			//TODO: Test parsing with PS4
			ParseUnix(pathStr, numChars);
#else
			static_assert(false, "Unsupported platform for path.");
#endif
			break;
		}
	}

	void Path::Assign(const ANSICHAR* pathStr, uint32 numChars, PATH_TYPE::E type)
	{
		switch(type)
		{
		case PATH_TYPE::Windows:
			ParseWindows(pathStr, numChars);
			break;
		case PATH_TYPE::Unix:
			ParseUnix(pathStr, numChars);
			break;
		case PATH_TYPE::Default:
		default:
#if GE_PLATFORM == GE_PLATFORM_WIN32
			ParseWindows(pathStr, numChars);
#elif GE_PLATFORM == GE_PLATFORM_APPLE || GE_PLATFORM == GE_PLATFORM_LINUX || GE_PLATFORM == GE_PLATFORM_PS4
			//TODO: Test parsing with PS4
			ParseUnix(pathStr, numChars);
#else
			static_assert(false, "Unsupported platform for path.");
#endif
			break;
		}
	}

	WString Path::ToWString(PATH_TYPE::E type) const
	{
		switch (type)
		{
		case PATH_TYPE::Windows:
			return BuildWindows();
		case PATH_TYPE::Unix:
			return BuildUnix();
		case PATH_TYPE::Default:
		default:
#if GE_PLATFORM == GE_PLATFORM_WIN32
			return BuildWindows();
#elif GE_PLATFORM == GE_PLATFORM_APPLE || GE_PLATFORM == GE_PLATFORM_LINUX || GE_PLATFORM == GE_PLATFORM_PS4
			return BuildUnix();
#else
			static_assert(false, "Unsupported platform for path.");
#endif
			break;
		}
	}

	String Path::ToString(PATH_TYPE::E type) const
	{
		switch (type)
		{
		case PATH_TYPE::Windows:
			return geEngineSDK::ToString(BuildWindows());
		case PATH_TYPE::Unix:
			return geEngineSDK::ToString(BuildUnix());
		case PATH_TYPE::Default:
		default:
#if GE_PLATFORM == GE_PLATFORM_WIN32
			return geEngineSDK::ToString(BuildWindows());
#elif GE_PLATFORM == GE_PLATFORM_APPLE || GE_PLATFORM == GE_PLATFORM_LINUX || GE_PLATFORM == GE_PLATFORM_PS4
			return geEngineSDK::ToString(BuildUnix());
#else
			static_assert(false, "Unsupported platform for path.");
#endif
			break;
		}
	}

	Path Path::GetParent() const
	{
		Path copy = *this;
		copy.MakeParent();
		return copy;
	}

	Path Path::GetAbsolute(const Path& base) const
	{
		Path copy = *this;
		copy.MakeAbsolute(base);
		return copy;
	}

	Path Path::GetRelative(const Path& base) const
	{
		Path copy = *this;
		copy.MakeRelative(base);
		return copy;
	}

	Path Path::GetDirectory() const
	{
		Path copy = *this;
		copy.m_Filename.clear();
		return copy;
	}

	Path& Path::MakeParent()
	{
		if( m_Filename.empty() )
		{
			if( m_Directories.empty() )
			{
				if( !m_IsAbsolute)
				{
					m_Directories.push_back(L"..");
				}
			}
			else
			{
				if( m_Directories.back() == L"..")
				{
					m_Directories.push_back(L"..");
				}
				else
				{
					m_Directories.pop_back();
				}
			}
		}
		else
		{
			m_Filename.clear();
		}

		return *this;
	}

	Path& Path::MakeAbsolute(const Path& base)
	{
		if( m_IsAbsolute )
		{
			return *this;
		}

		Path absDir = base.GetDirectory();
		if( base.IsFile() )
		{
			absDir.PushDirectory(base.m_Filename);
		}

		for( auto& dir : m_Directories )
		{
			absDir.PushDirectory(dir);
		}

		absDir.SetFilename(m_Filename);
		*this = absDir;

		return *this;
	}

	Path& Path::MakeRelative(const Path& base)
	{
		if( !base.Includes(*this) )
		{
			return *this;
		}

		m_Directories.erase(m_Directories.begin(), m_Directories.begin() + base.m_Directories.size());

		//Sometimes a directory name can be interpreted as a file and we're okay with that. Check for that special case.
		if( base.IsFile() )
		{
			if( m_Directories.size() > 0 )
			{
				m_Directories.erase(m_Directories.begin());
			}
			else
			{
				m_Filename = L"";
			}
		}

		m_Device = L"";
		m_Node = L"";
		m_IsAbsolute = false;

		return *this;
	}

	bool Path::Includes(const Path& child) const
	{
		if( m_Device != child.m_Device )
		{
			return false;
		}

		if( m_Node != child.m_Node )
		{
			return false;
		}

		auto iterParent = m_Directories.begin();
		auto iterChild = child.m_Directories.begin();

		for(; iterParent != m_Directories.end(); ++iterChild, ++iterParent)
		{
			if( iterChild == child.m_Directories.end() )
			{
				return false;
			}

			if( !ComparePathElem(*iterChild, *iterParent) )
			{
				return false;
			}
		}

		if( !m_Filename.empty() )
		{
			if( iterChild == child.m_Directories.end() )
			{
				if( child.m_Filename.empty() )
				{
					return false;
				}

				if( !ComparePathElem(child.m_Filename, m_Filename) )
				{
					return false;
				}
			}
			else
			{
				if( !ComparePathElem(*iterChild, m_Filename) )
				{
					return false;
				}
			}
		}

		return true;
	}

	bool Path::Equals(const Path& other) const
	{
		if( m_IsAbsolute != other.m_IsAbsolute )
		{
			return false;
		}

		if( m_IsAbsolute )
		{
			if( !ComparePathElem(m_Device, other.m_Device) )
			{
				return false;
			}
		}

		if( m_Directories.size() != other.m_Directories.size() )
		{
			return false;
		}

		if( !ComparePathElem(m_Filename, other.m_Filename) )
		{
			return false;
		}

		if( !ComparePathElem(m_Node, other.m_Node) )
		{
			return false;
		}

		auto iterMe = m_Directories.begin();
		auto iterOther = other.m_Directories.begin();

		for(; iterMe != m_Directories.end(); ++iterMe, ++iterOther)
		{
			if( !ComparePathElem(*iterMe, *iterOther) )
			{
				return false;
			}
		}

		return true;
	}

	Path& Path::Append(const Path& path)
	{
		if( !m_Filename.empty() )
		{
			PushDirectory(m_Filename);
		}

		for( auto& dir : path.m_Directories )
		{
			PushDirectory(dir);
		}

		m_Filename = path.m_Filename;

		return *this;
	}

	void Path::SetBasename(const WString& basename)
	{
		m_Filename = basename + GetWExtension();
	}

	void Path::SetBasename(const String& basename)
	{
		m_Filename = geEngineSDK::ToWString(basename) + GetWExtension();
	}

	void Path::SetExtension(const WString& extension)
	{
		WStringStream stream;
		stream << GetWFilename(false);
		stream << extension;
		m_Filename = stream.str();
	}

	void Path::SetExtension(const String& extension)
	{
		SetExtension(geEngineSDK::ToWString(extension));
	}

	WString Path::GetWFilename(bool extension) const
	{
		if(extension)
		{
			return m_Filename;
		}
		else
		{
			WString::size_type pos = m_Filename.rfind(L'.');
			if( pos != WString::npos )
			{
				return m_Filename.substr(0, pos);
			}
			else
			{
				return m_Filename;
			}
		}
	}

	String Path::GetFilename(bool extension) const
	{
		return geEngineSDK::ToString(GetWFilename(extension));
	}

	WString Path::GetWExtension() const
	{
		WString::size_type pos = m_Filename.rfind(L'.');
		if( pos != WString::npos )
		{
			return m_Filename.substr(pos);
		}

		return WString();
	}

	String Path::GetExtension() const
	{
		return geEngineSDK::ToString(GetWExtension());
	}

	const WString& Path::GetWDirectory(uint32 idx) const
	{
		if( idx >= (uint32)m_Directories.size() )
		{
			GE_EXCEPT(InvalidParametersException, "Index out of range: " + geEngineSDK::ToString(idx) + ". Valid range: [0, " + geEngineSDK::ToString((uint32)m_Directories.size() - 1) + "]");
		}

		return m_Directories[idx];
	}

	String Path::GetDirectory(uint32 idx) const
	{
		return geEngineSDK::ToString(GetWDirectory(idx));
	}

	WString Path::GetWTail(PATH_TYPE::E) const
	{
		if( IsFile() )
		{
			return m_Filename;
		}
		else if( m_Directories.size() > 0 )
		{
			return m_Directories.back();
		}
		else
		{
			return StringUtil::WBLANK;
		}
	}

	String Path::GetTail(PATH_TYPE::E type) const
	{
		return geEngineSDK::ToString(GetWTail(type));
	}

	void Path::Clear()
	{
		m_Directories.clear();
		m_Device.clear();
		m_Filename.clear();
		m_Node.clear();
		m_IsAbsolute = false;
	}

	void Path::throwInvalidPathException(const WString& path) const
	{
		GE_EXCEPT(InvalidParametersException, "Incorrectly formatted path provided: " + geEngineSDK::ToString(path));
	}

	void Path::throwInvalidPathException(const String& path) const
	{
		GE_EXCEPT(InvalidParametersException, "Incorrectly formatted path provided: " + path);
	}

	WString Path::BuildWindows() const
	{
		WStringStream result;
		if( !m_Node.empty() )
		{
			result << L"\\\\";
			result << m_Node;
			result << L"\\";
		}
		else if( !m_Device.empty() )
		{
			result << m_Device;
			result << L":\\";
		}
		else if( m_IsAbsolute )
		{
			result << L"\\";
		}

		for( auto& dir : m_Directories )
		{
			result << dir;
			result << L"\\";
		}

		result << m_Filename;
		return result.str();
	}

	WString Path::BuildUnix() const
	{
		WStringStream result;
		auto dirIter = m_Directories.begin();

		if( !m_Device.empty() )
		{
			result << L"/";
			result << m_Device;
			result << L":/";
		}
		else if( m_IsAbsolute )
		{
			if( dirIter != m_Directories.end() && *dirIter == L"~" )
			{
				result << L"~";
				dirIter++;
			}

			result << L"/";
		}

		for(; dirIter != m_Directories.end(); ++dirIter)
		{
			result << *dirIter;
			result << L"/";
		}

		result << m_Filename;
		return result.str();
	}

	Path Path::operator+ (const Path& rhs) const
	{
		return Path::Combine(*this, rhs);
	}

	Path& Path::operator+= (const Path& rhs)
	{
		return Append(rhs);
	}

	bool Path::ComparePathElem(const WString& left, const WString& right)
	{
		if( left.size() != right.size() )
		{
			return false;
		}

		//TODO: Case sensitive/insensitive file path actually depends on used file-system
		for( uint32 i=0; i<(uint32)left.size(); i++ )
		{
			if (tolower(left[i]) != tolower(right[i]))
			{
				return false;
			}
		}

		return true;
	}

	Path Path::Combine(const Path& left, const Path& right)
	{
		Path output = left;
		return output.Append(right);
	}

	void Path::PushDirectory(const WString& dir)
	{
		if( !dir.empty() && dir != L"." )
		{
			if( dir == L"..")
			{
				if( !m_Directories.empty() && m_Directories.back() != L".." )
				{
					m_Directories.pop_back();
				}
				else
				{
					m_Directories.push_back(dir);
				}
			}
			else
			{
				m_Directories.push_back(dir);
			}
		}
	}

	void Path::PushDirectory(const String& dir)
	{
		PushDirectory(geEngineSDK::ToWString(dir));
	}
}
