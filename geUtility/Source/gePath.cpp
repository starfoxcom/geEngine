/*****************************************************************************/
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
/*****************************************************************************/

/*****************************************************************************/
/**
* Includes
*/
/*****************************************************************************/
#include "gePrerequisitesUtil.h"
#include "geException.h"

namespace geEngineSDK {
  const Path Path::BLANK = Path();

  Path::Path() : m_isAbsolute(false) {}

  Path::Path(const ANSICHAR* pathStr, PATH_TYPE::E type) {
    assign(pathStr, type);
  }

  Path::Path(const UNICHAR* pathStr, PATH_TYPE::E type) {
    assign(pathStr, type);
  }

  Path::Path(const String& pathStr, PATH_TYPE::E type) {
    assign(pathStr, type);
  }

  Path::Path(const WString& pathStr, PATH_TYPE::E type) {
    assign(pathStr, type);
  }

  Path::Path(const Path& other) {
    assign(other);
  }

  Path& Path::operator=(const Path& path) {
    assign(path);
    return *this;
  }

  Path& Path::operator=(const WString& pathStr) {
    assign(pathStr);
    return *this;
  }

  Path& Path::operator=(const String& pathStr) {
    assign(pathStr);
    return *this;
  }

  Path& Path::operator=(const UNICHAR* pathStr) {
    assign(pathStr);
    return *this;
  }

  Path& Path::operator=(const ANSICHAR* pathStr) {
    assign(pathStr);
    return *this;
  }

  void
  Path::swap(Path& path) {
    std::swap(m_directories, path.m_directories);
    std::swap(m_filename, path.m_filename);
    std::swap(m_device, path.m_device);
    std::swap(m_node, path.m_node);
    std::swap(m_isAbsolute, path.m_isAbsolute);
  }

  void
  Path::assign(const Path& path) {
    m_directories = path.m_directories;
    m_filename = path.m_filename;
    m_device = path.m_device;
    m_node = path.m_node;
    m_isAbsolute = path.m_isAbsolute;
  }

  void
  Path::assign(const ANSICHAR* pathStr, PATH_TYPE::E type) {
    assign(pathStr, strlen(pathStr), type);
  }

  void
  Path::assign(const UNICHAR* pathStr, PATH_TYPE::E type) {
    assign(pathStr, wcslen(pathStr), type);
  }

  void
  Path::assign(const String& pathStr, PATH_TYPE::E type) {
    assign(pathStr.data(), pathStr.length(), type);
  }

  void
  Path::assign(const WString& pathStr, PATH_TYPE::E type) {
    assign(pathStr.data(), pathStr.length(), type);
  }

  Path&
  Path::append(const Path& path) {
    if (!m_filename.empty()) {
      pushDirectory(m_filename);
    }

    for (auto& dir : path.m_directories) {
      pushDirectory(dir);
    }

    m_filename = path.m_filename;
    return *this;
  }

  void
  Path::assign(const UNICHAR* pathStr, SIZE_T numChars, PATH_TYPE::E type) {
    switch (type) {
    case PATH_TYPE::kWindows:
      parseWindows(pathStr, numChars);
      break;
    case PATH_TYPE::kUnix:
      parseUnix(pathStr, numChars);
      break;
    case PATH_TYPE::kDefault:
    default:
#if GE_PLATFORM == GE_PLATFORM_WIN32
      parseWindows(pathStr, numChars);
#elif GE_PLATFORM == GE_PLATFORM_OSX   || \
      GE_PLATFORM == GE_PLATFORM_LINUX || \
      GE_PLATFORM == GE_PLATFORM_PS4
      //TODO: Test parsing with PS4
      parseUnix(pathStr, numChars);
#else
      static_assert(false, "Unsupported platform for path.");
#endif
      break;
    }
  }

  void
  Path::assign(const ANSICHAR* pathStr, SIZE_T numChars, PATH_TYPE::E type) {
    switch (type) {
    case PATH_TYPE::kWindows:
      parseWindows(pathStr, numChars);
      break;
    case PATH_TYPE::kUnix:
      parseUnix(pathStr, numChars);
      break;
    case PATH_TYPE::kDefault:
    default:
#if GE_PLATFORM == GE_PLATFORM_WIN32
      parseWindows(pathStr, numChars);
#elif GE_PLATFORM == GE_PLATFORM_OSX   || \
      GE_PLATFORM == GE_PLATFORM_LINUX || \
      GE_PLATFORM == GE_PLATFORM_PS4
      //TODO: Test parsing with PS4
      parseUnix(pathStr, numChars);
#else
      static_assert(false, "Unsupported platform for path.");
#endif
      break;
    }
  }

  String
  Path::toString(PATH_TYPE::E type) const {
    switch (type) {
    case PATH_TYPE::kWindows:
      return geEngineSDK::toString(buildWindows());
    case PATH_TYPE::kUnix:
      return geEngineSDK::toString(buildUnix());
    case PATH_TYPE::kDefault:
    default:
#if GE_PLATFORM == GE_PLATFORM_WIN32
      return geEngineSDK::toString(buildWindows());
#elif GE_PLATFORM == GE_PLATFORM_OSX   || \
      GE_PLATFORM == GE_PLATFORM_LINUX || \
      GE_PLATFORM == GE_PLATFORM_PS4
      return geEngineSDK::toString(BuildUnix());
#else
      static_assert(false, "Unsupported platform for path.");
#endif
      break;
    }
  }

  WString
  Path::toWString(PATH_TYPE::E type) const {
    switch (type) {
    case PATH_TYPE::kWindows:
      return buildWindows();
    case PATH_TYPE::kUnix:
      return buildUnix();
    case PATH_TYPE::kDefault:
    default:
#if GE_PLATFORM == GE_PLATFORM_WIN32
      return buildWindows();
#elif GE_PLATFORM == GE_PLATFORM_OSX   || \
      GE_PLATFORM == GE_PLATFORM_LINUX || \
      GE_PLATFORM == GE_PLATFORM_PS4
      return buildUnix();
#else
      static_assert(false, "Unsupported platform for path.");
#endif
      break;
    }
  }

  Path
  Path::getParent() const {
    Path copy = *this;
    copy.makeParent();
    return copy;
  }

  Path
  Path::getAbsolute(const Path& base) const {
    Path copy = *this;
    copy.makeAbsolute(base);
    return copy;
  }

  Path
  Path::getRelative(const Path& base) const {
    Path copy = *this;
    copy.makeRelative(base);
    return copy;
  }

  Path
  Path::getDirectory() const {
    Path copy = *this;
    copy.m_filename.clear();
    return copy;
  }

  Path&
  Path::makeParent() {
    if (m_filename.empty()) {
      if (m_directories.empty()) {
        if (!m_isAbsolute) {
          m_directories.push_back(L"..");
        }
      }
      else {
        if (L".." == m_directories.back()) {
          m_directories.push_back(L"..");
        }
        else {
          m_directories.pop_back();
        }
      }
    }
    else {
      m_filename.clear();
    }

    return *this;
  }

  Path&
  Path::makeAbsolute(const Path& base) {
    if (m_isAbsolute) {
      return *this;
    }

    Path absDir = base.getDirectory();
    if (base.isFile()) {
      absDir.pushDirectory(base.m_filename);
    }

    for (auto& dir : m_directories) {
      absDir.pushDirectory(dir);
    }

    absDir.setFilename(m_filename);
    *this = absDir;
    return *this;
  }

  Path&
  Path::makeRelative(const Path& base) {
    if (!base.includes(*this)) {
      return *this;
    }

    m_directories.erase(m_directories.begin(), 
                        m_directories.begin() + base.m_directories.size());

    /** Sometimes a directory name can be interpreted as a file and we're okay with that.
     *  Check for that special case. */
    if (base.isFile()) {
      if (0 < m_directories.size()) {
        m_directories.erase(m_directories.begin());
      }
      else {
        m_filename = L"";
      }
    }

    m_device = L"";
    m_node = L"";
    m_isAbsolute = false;
    return *this;
  }

  bool
  Path::includes(const Path& child) const {
    if (m_device != child.m_device) {
      return false;
    }

    if (m_node != child.m_node) {
      return false;
    }

    auto iterParent = m_directories.begin();
    auto iterChild = child.m_directories.begin();

    for (; iterParent != m_directories.end(); ++iterChild, ++iterParent) {
      if (iterChild == child.m_directories.end()) {
        return false;
      }

      if (!comparePathElem(*iterChild, *iterParent)) {
        return false;
      }
    }

    if (!m_filename.empty()) {
      if (iterChild == child.m_directories.end()) {
        if (child.m_filename.empty()) {
          return false;
        }

        if (!comparePathElem(child.m_filename, m_filename)) {
          return false;
        }
      }
      else {
        if (!comparePathElem(*iterChild, m_filename)) {
          return false;
        }
      }
    }

    return true;
  }

  bool
  Path::equals(const Path& other) const {
    if (m_isAbsolute != other.m_isAbsolute) {
      return false;
    }

    if (m_isAbsolute) {
      if (!comparePathElem(m_device, other.m_device)) {
        return false;
      }
    }

    if (m_directories.size() != other.m_directories.size()) {
      return false;
    }

    if (!comparePathElem(m_filename, other.m_filename)) {
      return false;
    }

    if (!comparePathElem(m_node, other.m_node)) {
      return false;
    }

    auto iterMe = m_directories.begin();
    auto iterOther = other.m_directories.begin();

    for (; iterMe != m_directories.end(); ++iterMe, ++iterOther) {
      if (!comparePathElem(*iterMe, *iterOther)) {
        return false;
      }
    }

    return true;
  }

  void
  Path::setBasename(const String& basename) {
    m_filename = geEngineSDK::toWString(basename) + getWExtension();
  }

  void
  Path::setBasename(const WString& basename) {
    m_filename = basename + getWExtension();
  }

  void
  Path::setExtension(const WString& extension) {
    WStringStream stream;
    stream << getWFilename(false);
    stream << extension;
    m_filename = stream.str();
  }

  void
  Path::setExtension(const String& extension) {
    setExtension(geEngineSDK::toWString(extension));
  }

  WString
  Path::getWFilename(bool extension) const {
    if (extension) {
      return m_filename;
    }
    else {
      WString::size_type pos = m_filename.rfind(L'.');
      if (pos != WString::npos) {
        return m_filename.substr(0, pos);
      }
      else {
        return m_filename;
      }
    }
  }

  String
  Path::getFilename(bool extension) const {
    return geEngineSDK::toString(getWFilename(extension));
  }

  WString
  Path::getWExtension() const {
    WString::size_type pos = m_filename.rfind(L'.');
    if (WString::npos != pos) {
      return m_filename.substr(pos);
    }
    return WString();
  }

  String
  Path::getExtension() const {
    return geEngineSDK::toString(getWExtension());
  }

  String
  Path::getDirectory(SIZE_T idx) const {
    return geEngineSDK::toString(getWDirectory(idx));
  }

  const WString&
  Path::getWDirectory(SIZE_T idx) const {
    if (idx >= m_directories.size()) {
      GE_EXCEPT(InvalidParametersException,
                "Index out of range: " + 
                geEngineSDK::toString(static_cast<uint32>(idx)) +
                ". Valid range: [0, " + 
                geEngineSDK::toString(static_cast<uint32>(m_directories.size() - 1)) +
                "]");
    }
    return m_directories[idx];
  }

  WString
  Path::getWTail(PATH_TYPE::E) const {
    if (isFile()) {
      return m_filename;
    }
    else if (0 < m_directories.size()) {
      return m_directories.back();
    }
    else {
      return StringUtil::WBLANK;
    }
  }

  String
  Path::getTail(PATH_TYPE::E type) const {
    return geEngineSDK::toString(getWTail(type));
  }

  void
  Path::clear() {
    m_directories.clear();
    m_device.clear();
    m_filename.clear();
    m_node.clear();
    m_isAbsolute = false;
  }

  void
  Path::throwInvalidPathException(const WString& path) const {
    GE_EXCEPT(InvalidParametersException, 
              "Incorrectly formatted path provided: " + geEngineSDK::toString(path));
  }

  void
  Path::throwInvalidPathException(const String& path) const {
    GE_EXCEPT(InvalidParametersException, "Incorrectly formatted path provided: " + path);
  }

  WString
  Path::buildWindows() const {
    WStringStream result;
    if (!m_node.empty()) {
      result << L"\\\\";
      result << m_node;
      result << L"\\";
    }
    else if (!m_device.empty()) {
      result << m_device;
      result << L":\\";
    }
    else if (m_isAbsolute) {
      result << L"\\";
    }

    for (auto& dir : m_directories) {
      result << dir;
      result << L"\\";
    }

    result << m_filename;
    return result.str();
  }

  WString
  Path::buildUnix() const {
    WStringStream result;
    auto dirIter = m_directories.begin();

    if (!m_device.empty()) {
      result << L"/";
      result << m_device;
      result << L":/";
    }
    else if (m_isAbsolute) {
      if (dirIter != m_directories.end() && L"~" == *dirIter) {
        result << L"~";
        dirIter++;
      }
      result << L"/";
    }

    for (; dirIter != m_directories.end(); ++dirIter) {
      result << *dirIter;
      result << L"/";
    }

    result << m_filename;
    return result.str();
  }

  Path
  Path::operator+(const Path& rhs) const {
    return Path::combine(*this, rhs);
  }

  Path&
  Path::operator+=(const Path& rhs) {
    return append(rhs);
  }

  bool
  Path::comparePathElem(const WString& left, const WString& right) {
    if (left.size() != right.size()) {
      return false;
    }

    //TODO: Case sensitive/insensitive file path actually depends on used file-system
    for (SIZE_T i = 0; i<left.size(); ++i) {
      if (tolower(left[i]) != tolower(right[i])) {
        return false;
      }
    }

    return true;
  }

  Path
  Path::combine(const Path& left, const Path& right) {
    Path output = left;
    return output.append(right);
  }

  void
  Path::pushDirectory(const WString& dir) {
    if (!dir.empty() && L"." != dir) {
      if (dir == L"..") {
        if (!m_directories.empty() && m_directories.back() != L"..") {
          m_directories.pop_back();
        }
        else {
          m_directories.push_back(dir);
        }
      }
      else {
        m_directories.push_back(dir);
      }
    }
  }

  void
  Path::pushDirectory(const String& dir) {
    pushDirectory(geEngineSDK::toWString(dir));
  }
}
