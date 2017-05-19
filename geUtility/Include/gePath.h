/*****************************************************************************/
/**
 * @file    gePath.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/06
 * @brief   Class for storing and manipulating file paths
 *
 * Class for storing and manipulating file paths. Paths may be
 * parsed from and to raw strings according to various platform
 * specific path types.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
namespace geEngineSDK {
  namespace PATH_TYPE {
    enum E {
      Windows,
      Unix,
      Default
    };
  }

  /**
   * @class Path
   * @brief Class for storing and manipulating file paths. Paths may be parsed
   *        from and to raw strings according to various platform specific path types.
   * @note  In order to allow the system to easily distinguish between file and directory
   *        paths, try to ensure that all directory paths end with a separator
   *        (\ or / depending on platform). System won't fail if you don't
   *        but it will be easier to misuse.
   */
  class GE_UTILITY_EXPORT Path
  {
   public:
    Path();

    /**
     * @brief Constructs a path by parsing the provided path string. 
     *        Throws exception if provided path is not valid.
     * @param[in] type  If set to default path will be parsed according to the
     *                  rules of the platform the application is being compiled to.
     *                  Otherwise it will be parsed according to provided type.
     */
    Path(const WString& pathStr, PATH_TYPE::E type = PATH_TYPE::Default);
    Path(const String& pathStr, PATH_TYPE::E type = PATH_TYPE::Default);
    Path(const UNICHAR* pathStr, PATH_TYPE::E type = PATH_TYPE::Default);
    Path(const ANSICHAR* pathStr, PATH_TYPE::E type = PATH_TYPE::Default);

    /**
     * @brief	Copy constructor
     */
    Path(const Path& other);

    /**
     * @brief Assigns a path by parsing the provided path string. Path will be
     *        parsed according to the rules of the platform the application is
     *        being compiled to.
     */
    Path& operator=(const WString& pathStr);
    Path& operator=(const String& pathStr);
    Path& operator=(const UNICHAR* pathStr);
    Path& operator=(const ANSICHAR* pathStr);
    Path& operator=(const Path& path);

    /**
     * @brief Compares two paths and returns true if they match. Comparison is
     *        case insensitive and paths will be compared as-is, without canonization.
     */
    bool
    operator==(const Path& path) const {
      return equals(path);
    }

    /**
     * @brief Compares two paths and returns true if they don't match. Comparison is case
     *        insensitive and paths will be compared as-is, without canonization.
     */
    bool
    operator!=(const Path& path) const {
      return !equals(path);
    }

    /**
     * @brief Gets a directory name with the specified index from the path.
     */
    const WString&
    operator[](uint32 idx) const {
      return getWDirectory(static_cast<SIZE_T>(idx));
    }

    /**
     * @brief Gets a directory name with the specified index from the path.
     */
    const WString&
    operator[](SIZE_T idx) const {
      return getWDirectory(idx);
    }

    /**
    * @brief	Swap internal data with another Path object.
    */
    void
    swap(Path& path);

    /**
     * @brief Create a path from another Path object.
     */
    void
    assign(const Path& path);

    /**
     * @brief Constructs a path by parsing the provided path string.
     *        Throws exception if provided path is not valid.
     * @param[in] type  If set to default path will be parsed according to the
     *                  rules of the platform the application is being compiled to.
     *                  Otherwise it will be parsed according to provided type.
     */
    void
    assign(const ANSICHAR* pathStr, PATH_TYPE::E type = PATH_TYPE::Default);

    /**
     * @copydoc void Path::assign(const ANSICHAR*, PATH_TYPE::E)
     */
    void
    assign(const UNICHAR* pathStr, PATH_TYPE::E type = PATH_TYPE::Default);

    /**
     * @copydoc void Path::assign(const ANSICHAR*, PATH_TYPE::E)
     */
    void
    assign(const String& pathStr, PATH_TYPE::E type = PATH_TYPE::Default);

    /**
     * @copydoc void Path::assign(const ANSICHAR*, PATH_TYPE::E)
     */
    void
    assign(const WString& pathStr, PATH_TYPE::E type = PATH_TYPE::Default);
    
    /**
    * @brief Appends another path to the end of this path.
    */
    Path&
    append(const Path& path);

    /**
     * @brief Converts the path in a string according to platform path rules.
     * @param[in] type  If set to default path will be parsed according to the
     *                  rules of the platform the application is being compiled to.
     *                  Otherwise it will be parsed according to provided type.
     */
    String
    toString(PATH_TYPE::E type = PATH_TYPE::Default) const;

    /**
     * @copydoc void Path::toString(PATH_TYPE::E)
     */
    WString
    toWString(PATH_TYPE::E type = PATH_TYPE::Default) const;

    /**
     * @brief Converts the path to either a string or a wstring, doing The
     *        Right Thing for the current platform. This method is equivalent
     *        to toWString() on Windows, and to toString() elsewhere.
     */
#if GE_PLATFORM == GE_PLATFORM_WIN32
    WString
    toPlatformString() const {
      return toWString();
    }
#else
    String
    toPlatformString() const {
      return toString();
    }
#endif

    /**
     * @brief Checks is the path a directory (contains no file-name).
     */
    bool
    isDirectory() const {
      return m_filename.empty();
    }

    /**
     * @brief Checks does the path point to a file.
     */
    bool
    isFile() const {
      return !m_filename.empty();
    }

    /**
     * @brief Checks is the contained path absolute.
     */
    bool
    isAbsolute() const {
      return m_isAbsolute;
    }

    /**
     * @brief Returns parent path. If current path points to a file the parent
     *        path will be the folder where the file is located. Or if it
     *        contains a directory the parent will be the parent directory.
     *        If no parent exists same path will be returned.
     */
    Path
    getParent() const;

    /**
     * @brief Returns an absolute path by appending the current path to the provided base.
     *        If path was already absolute no changes are made and copy of current path is
     *        returned. If base is not absolute, then the returned path will be made
     *        relative to base, but will not be absolute.
     */
    Path
    getAbsolute(const Path& base) const;

    /**
     * @brief Returns a relative path by making the current path relative to the provided
     *        base. Base must be a part of the current path. If base isn't a part of the
     *        path no changes are made and a copy of the current path is returned.
     */
    Path
    getRelative(const Path& base) const;

    /**
     * @brief Returns the path as a path to directory. If path was pointing to
     *        a file, the filename is removed, otherwise no changes are made
     *        and exact copy is returned.
     */
    Path
    getDirectory() const;

    /**
     * @brief Makes the path the parent of the current path. If current path
     *        points to a file the parent path will be the folder where the
     *        file is located. Or if it contains a directory the parent will be
     *        the parent directory. If no parent exists, same path will be returned.
     */
    Path&
    makeParent();

    /**
     * @brief Makes the current path absolute by appending it to base. If path
     *        was already absolute no changes are made and copy of current path
     *        is returned. If base is not absolute, then the returned path will
     *        be made relative to base, but will not be absolute.
     */
    Path&
    makeAbsolute(const Path& base);

    /**
     * @brief Makes the current path relative to the provided base. Base must 
     *        be a part of the current path. If base is not a part of the path 
     *        no changes are made and a copy of the current path is returned.
     */
    Path&
    makeRelative(const Path& base);

    /**
     * @brief Checks if the current path contains the provided path.
     *        Comparison is case insensitive and paths will be compared as-is,
     *        without canonization.
     */
    bool
    includes(const Path& child) const;

    /**
     * @brief Compares two paths and returns true if they match.
     *        Comparison is case insensitive and paths will be compared as-is,
     *        without canonization.
     */
    bool
    equals(const Path& other) const;

    /**
     * @brief Change or set the filename in the path.
     */
    void
    setFilename(const WString& filename) {
      m_filename = filename;
    }

    /**
     * @brief Change or set the filename in the path.
     */
    void
    setFilename(const String& filename) {
      m_filename = geEngineSDK::toWString(filename);
    }

    /**
     * @brief Change or set the base name in the path. Base name changes the filename
     *        by changing its base to the provided value but keeping extension intact.
     */
    void
    setBasename(const WString& basename);

    /**
     * @brief	Change or set the base name in the path. Base name changes the filename
     *        by changing its base to the provided value but keeping extension intact.
     */
    void
    setBasename(const String& basename);

    /**
     * @brief Change or set the extension of the filename in the path.
     * @param[in] extension Extension with a leading ".".
     */
    void
    setExtension(const WString& extension);

    /**
     * @brief Change or set the extension of the filename in the path.
     * @param[in] extension Extension with a leading ".".
     */
    void
    setExtension(const String& extension);

    /**
     * @brief Returns a filename in the path.
     * @param[in] extension If true, returned filename will contain an extension.
     */
    WString
    getWFilename(bool extension = true) const;

    /**
     * @brief Returns a filename in the path.
     * @param[in] extension If true, returned filename will contain an extension.
     */
    String
    getFilename(bool extension = true) const;

    /**
     * @brief Returns file extension with the leading "."
     */
    WString
    getWExtension() const;

    /**
     * @brief Returns file extension with the leading "."
     */
    String
    getExtension() const;

    /**
     * @brief Gets the number of directories in the path.
     */
    SIZE_T
    getNumDirectories() const {
      return m_directories.size();
    }

    /**
     * @brief Gets a directory name with the specified index from the path.
     */
    const WString&
    getWDirectory(SIZE_T idx) const;

    /**
     * @brief Gets a directory name with the specified index from the path.
     */
    String
    getDirectory(SIZE_T idx) const;

    /**
     * @brief Returns path device (e.g. drive, volume, etc.) if one exists in the path.
     */
    const WString&
    getWDevice() const {
      return m_device;
    }

    /**
     * @brief Returns path device (e.g. drive, volume, etc.) if one exists in the path.
     */
    String
    getDevice() const {
      return geEngineSDK::toString(m_device);
    }

    /**
     * @brief Returns path node (e.g. network name) if one exists in the path.
     */
    const WString&
    getWNode() const {
      return m_node;
    }

    /**
     * @brief Returns path node (e.g. network name) if one exists in the path.
     */
    String
    getNode() const {
      return geEngineSDK::toString(m_node);
    }

    /**
     * @brief Gets last element in the path, filename if it exists, otherwise
     *        the last directory. If no directories exist returns device or node.
     * @param[in] type  Determines format of node or device, in case they are
     *                  returned. When default, format for the active platform
     *                  will be used, otherwise the format defined by the parameter
     *                  will be used.
     */
    WString
    getWTail(PATH_TYPE::E type = PATH_TYPE::Default) const;

    /**
     * @brief Gets last element in the path, filename if it exists, otherwise
     *        the last directory. If no directories exist returns device or node.
     * @param[in] type  Determines format of node or device, in case they are
     *                  returned. When default, format for the active platform
     *                  will be used, otherwise the format defined by the parameter
     *                  will be used.
     */
    String
    getTail(PATH_TYPE::E type = PATH_TYPE::Default) const;

    /**
     * @brief	Clears the path to nothing.
     */
    void
    clear();

    /**
     * @brief Returns true if no path has been set.
     */
    bool
    isEmpty() const {
      return (m_directories.empty() && 
              m_filename.empty() && 
              m_device.empty() && 
              m_node.empty());
    }

    /**
     * @brief Concatenates two paths.
     */
    Path
    operator+ (const Path& rhs) const;
    
    /**
     * @copydoc Path::operator+ (const Path& rhs) const
     */
    Path&
    operator+= (const Path& rhs);

    /**
     * @brief Compares two path elements (i.e. filenames, directory names, etc.)
     */
    static bool
    comparePathElem(const WString& left, const WString& right);

    /**
     * @brief Combines two paths and returns the result. Right path should be relative.
     */
    static Path
    combine(const Path& left, const Path& right);

   private:
    /**
     * @brief Constructs a path by parsing the provided raw string data.
     *        Throws exception if provided path is not valid.
     * @param[in] type  If set to default path will be parsed according to the
     *                  rules of the platform the application is being compiled to.
     *                  Otherwise it will be parsed according to provided type.
     */
    void
    assign(const UNICHAR* pathStr, SIZE_T numChars, PATH_TYPE::E type = PATH_TYPE::Default);

    /**
     * @copydoc Path::assign(const UNICHAR*, uint32, PATH_TYPE::E)
     */
    void
    assign(const ANSICHAR* pathStr, SIZE_T numChars, PATH_TYPE::E type = PATH_TYPE::Default);

    /**
     * @brief Parses a Windows path and stores the parsed data internally.
     *        Throws an exception if parsing fails.
     */
    template<class T>
    void
    parseWindows(const T* pathStr, SIZE_T numChars) {
      clear();

      SIZE_T idx = 0;
      BasicStringStream<T> tempStream;

      if (idx < numChars) {
        if ('\\' == pathStr[idx] || '/' == pathStr[idx]) {
          m_isAbsolute = true;
          idx++;
        }
      }

      if (idx < numChars) { //Path starts with a node, a drive letter or is relative
        if (m_isAbsolute && ('\\' == pathStr[idx] || '/' ==pathStr[idx])) {//Node
          idx++;
          tempStream.str(BasicString<T>());
          tempStream.clear();

          while (idx < numChars && '\\' != pathStr[idx] && '/' != pathStr[idx]) {
            tempStream << pathStr[idx++];
          }

          setNode(tempStream.str());

          if (idx < numChars) {
            idx++;
          }
        }
        else {  //A drive letter or not absolute
          T drive = pathStr[idx];
          idx++;

          if (idx < numChars && ':' == pathStr[idx]) {
            if (m_isAbsolute 
                || !(('a' <= drive && 'z' >= drive) 
                || ('A' <= drive && 'Z' >= drive))) {
              //The drive letter is not valid
              throwInvalidPathException(BasicString<T>(pathStr, numChars));
            }

            m_isAbsolute = true;
            setDevice(geEngineSDK::toWString(drive));

            idx++;
            if (idx >= numChars || ('\\' != pathStr[idx] && '/' != pathStr[idx])) {
              //The path does not end with a trailing slash
              throwInvalidPathException(BasicString<T>(pathStr, numChars));
            }

            idx++;
          }
          else {
            idx--;
          }
        }

        while (idx < numChars) {
          tempStream.str(BasicString<T>());
          tempStream.clear();
          while (idx < numChars && '\\' != pathStr[idx] && '/' != pathStr[idx]) {
            tempStream << pathStr[idx];
            idx++;
          }

          if (idx < numChars) {
            pushDirectory(tempStream.str());
          }
          else {
            setFilename(tempStream.str());
          }

          idx++;
        }
      }
    }

    /**
     * @brief Parses a Unix path and stores the parsed data internally.
     *        Throws an exception if parsing fails.
     */
    template<class T>
    void
    parseUnix(const T* pathStr, SIZE_T numChars) {
      clear();

      SIZE_T idx = 0;
      BasicStringStream<T> tempStream;

      if (idx < numChars) {
        if (pathStr[idx] == '/') {
          m_isAbsolute = true;
          idx++;
        }
        else if (pathStr[idx] == '~') {
          idx++;
          if (idx >= numChars || '/' == pathStr[idx]) {
            pushDirectory(geEngineSDK::toWString('~'));
            m_isAbsolute = true;
          }
          else {
            idx--;
          }
        }

        while (idx < numChars) {
          tempStream.str(BasicString<T>());
          tempStream.clear();

          while (idx < numChars && '/' != pathStr[idx]) {
            tempStream << pathStr[idx];
            idx++;
          }

          if (idx < numChars) {
            if (m_directories.empty()) {
              BasicString<T> deviceStr = tempStream.str();
              if (!deviceStr.empty() && ':' == *(deviceStr.rbegin())) {
                setDevice(deviceStr.substr(0, deviceStr.length() - 1));
                m_isAbsolute = true;
              }
              else {
                pushDirectory(deviceStr);
              }
            }
            else {
              pushDirectory(tempStream.str());
            }
          }
          else {
            setFilename(tempStream.str());
          }

          idx++;
        }
      }
    }

    void
    setNode(const WString& node) {
      m_node = node;
    }

    void
    setNode(const String& node) {
      m_node = geEngineSDK::toWString(node);
    }

    void
    setDevice(const WString& device) {
      m_device = device;
    }

    void setDevice(const String& device) {
      m_device = geEngineSDK::toWString(device);
    }

    /**
     * @brief Build a Windows path string from internal path data.
     */
    WString
    buildWindows() const;

    /**
     * @brief Build a Unix path string from internal path data.
     */
    WString
    buildUnix() const;

    /**
     * @brief Add new directory to the end of the path.
     */
    void
    pushDirectory(const String& dir);

    /**
     * @copydoc Path::pushDirectory(const String&)
     */
    void
    pushDirectory(const WString& dir);

    /**
     * @brief Helper method that throws invalid path exception.
     */
    void
    throwInvalidPathException(const String& path) const;

    /**
     * @copydoc Path::throwInvalidPathException(const String&)
     */
    void
    throwInvalidPathException(const WString& path) const;

   public:
    static const Path BLANK;

   private:
    friend struct RTTIPlainType<Path>;
    friend struct ::std::hash<geEngineSDK::Path>;

    Vector<WString> m_directories;
    WString m_device;
    WString m_filename;
    WString m_node;
    bool m_isAbsolute;
  };

  /**
   * @brief RTTIPlainType specialization for Path that allows paths be
   *        serialized as value types.
   * @see   RTTIPlainType
   */
  template<>
  struct RTTIPlainType<Path>
  {
    enum { kID = TYPEID_UTILITY::kID_Path }; enum { kHasDynamicSize = 1 };

    static void
    toMemory(const Path& data, char* memory) {
      uint64 size = getDynamicSize(data);
      memcpy(memory, &size, sizeof(uint64));
      memory += sizeof(uint64);

      memory = rttiWriteElement(data.m_device, memory);
      memory = rttiWriteElement(data.m_node, memory);
      memory = rttiWriteElement(data.m_filename, memory);
      memory = rttiWriteElement(data.m_isAbsolute, memory);
      memory = rttiWriteElement(data.m_directories, memory);
    }

    static uint64
    fromMemory(Path& data, char* memory) {
      uint64 size;
      memcpy(&size, memory, sizeof(uint64));
      memory += sizeof(uint64);

      memory = rttiReadElement(data.m_device, memory);
      memory = rttiReadElement(data.m_node, memory);
      memory = rttiReadElement(data.m_filename, memory);
      memory = rttiReadElement(data.m_isAbsolute, memory);
      memory = rttiReadElement(data.m_directories, memory);

      return size;
    }

    static uint64
    getDynamicSize(const Path& data) {
      uint64 dataSize = rttiGetElementSize(data.m_device) +
        rttiGetElementSize(data.m_node) +
        rttiGetElementSize(data.m_filename) +
        rttiGetElementSize(data.m_isAbsolute) +
        rttiGetElementSize(data.m_directories) + sizeof(uint64);

#if GE_DEBUG_MODE
      if (dataSize > std::numeric_limits<uint64>::max()) {
        __string_throwDataOverflowException();
      }
#endif
      return dataSize;
    }
  };
}

/**
* @brief	Hash value generator for Path.
*/
template<>
struct std::hash<geEngineSDK::Path>
{
  size_t
  operator()(const geEngineSDK::Path& path) const {
    size_t hash = 0;
    geEngineSDK::hash_combine(hash, path.m_filename);
    geEngineSDK::hash_combine(hash, path.m_device);
    geEngineSDK::hash_combine(hash, path.m_node);

    for (auto& dir : path.m_directories) {
      geEngineSDK::hash_combine(hash, dir);
    }

    return hash;
  }
};
