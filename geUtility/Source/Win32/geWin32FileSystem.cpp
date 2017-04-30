/********************************************************************/
/**
 * @file   geWin32FileSystem.cpp
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/09/19
 * @brief  Utility class for dealing with Win32 files.
 *
 * Utility class for dealing with Win32 files.
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#include "geFileSystem.h"
#include "geException.h"
#include "geDataStream.h"
#include "geDebug.h"
#include <windows.h>

namespace geEngineSDK
{
	void Win32_HandleError(DWORD error, const WString& path)
	{
		switch(error)
		{
		case ERROR_FILE_NOT_FOUND:
			LOGERR("File at path: \"" + toString(path) + "\" not found.");
			break;
		case ERROR_PATH_NOT_FOUND:
		case ERROR_BAD_NETPATH:
		case ERROR_CANT_RESOLVE_FILENAME:
		case ERROR_INVALID_DRIVE:
			LOGERR("Path \"" + toString(path) + "\" not found.");
			break;
		case ERROR_ACCESS_DENIED:
			LOGERR("Access to path \"" + toString(path) + "\" denied.");
			break;
		case ERROR_ALREADY_EXISTS:
		case ERROR_FILE_EXISTS:
			LOGERR("File/folder at path \"" + toString(path) + "\" already exists.");
			break;
		case ERROR_INVALID_NAME:
		case ERROR_DIRECTORY:
		case ERROR_FILENAME_EXCED_RANGE:
		case ERROR_BAD_PATHNAME:
			LOGERR("Invalid path string: \"" + toString(path) + "\".");
			break;
		case ERROR_FILE_READ_ONLY:
			LOGERR("File at path \"" + toString(path) + "\" is read only.");
			break;
		case ERROR_CANNOT_MAKE:
			LOGERR("Cannot create file/folder at path: \"" + toString(path) + "\".");
			break;
		case ERROR_DIR_NOT_EMPTY:
			LOGERR("Directory at path \"" + toString(path) + "\" not empty.");
			break;
		case ERROR_WRITE_FAULT:
			LOGERR("Error while writing a file at path \"" + toString(path) + "\".");
			break;
		case ERROR_READ_FAULT:
			LOGERR("Error while reading a file at path \"" + toString(path) + "\".");
			break;
		case ERROR_SHARING_VIOLATION:
			LOGERR("Sharing violation at path \"" + toString(path) + "\".");
			break;
		case ERROR_LOCK_VIOLATION:
			LOGERR("Lock violation at path \"" + toString(path) + "\".");
			break;
		case ERROR_HANDLE_EOF:
			LOGERR("End of file reached for file at path \"" + toString(path) + "\".");
			break;
		case ERROR_HANDLE_DISK_FULL:
		case ERROR_DISK_FULL:
			LOGERR("Disk full.");
			break;
		case ERROR_NEGATIVE_SEEK:
			LOGERR("Negative seek.");
			break;
		default:
			LOGERR("Undefined file system exception: " + toString((uint32)error));
			break;
		}
	}

	WString Win32_GetCurrentDirectory()
	{
		DWORD len = GetCurrentDirectoryW(0, NULL);
		if( len > 0 )
		{
			wchar_t* buffer = (wchar_t*)ge_alloc(len * sizeof(wchar_t));
			DWORD n = GetCurrentDirectoryW(len, buffer);

			if( n > 0 && n <= len )
			{
				WString result(buffer);
				if( result[result.size()-1] != '\\' )
				{
					result.append(L"\\");
				}

				ge_free(buffer);
				return result;
			}

			ge_free(buffer);
		}

		return StringUtil::WBLANK;
	}

	WString Win32_GetTempDirectory()
	{
		DWORD len = GetTempPathW(0, NULL);
		if( len > 0 )
		{
			wchar_t* buffer = (wchar_t*)ge_alloc(len * sizeof(wchar_t));
			DWORD n = GetTempPathW(len, buffer);

			if( n > 0 && n <= len )
			{
				WString result(buffer);
				if( result[result.size()-1] != '\\' )
				{
					result.append(L"\\");
				}

				ge_free(buffer);
				return result;
			}

			ge_free(buffer);
		}

		return StringUtil::WBLANK;
	}

	bool Win32_PathExists(const WString& path)
	{
		DWORD attr = GetFileAttributesW( path.c_str() );
		if( attr == 0xFFFFFFFF )
		{
			switch( GetLastError() )
			{
			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
			case ERROR_NOT_READY:
			case ERROR_INVALID_DRIVE:
				return false;
			default:
				Win32_HandleError(GetLastError(), path);
			}
		}
		return true;
	}

	bool Win32_IsDirectory(const WString& path)
	{
		DWORD attr = GetFileAttributesW(path.c_str());
		if( attr == 0xFFFFFFFF )
		{
			Win32_HandleError(GetLastError(), path);
		}

		return (attr & FILE_ATTRIBUTE_DIRECTORY) != FALSE;
	}

	bool Win32_IsDevice(const WString& path)
	{
		WString ucPath = path;
		StringUtil::toUpperCase(ucPath);

		return
			ucPath.compare(0, 4, L"\\\\.\\") == 0 ||
			ucPath.compare(L"CON") == 0 ||
			ucPath.compare(L"PRN") == 0 ||
			ucPath.compare(L"AUX") == 0 ||
			ucPath.compare(L"NUL") == 0 ||
			ucPath.compare(L"LPT1") == 0 ||
			ucPath.compare(L"LPT2") == 0 ||
			ucPath.compare(L"LPT3") == 0 ||
			ucPath.compare(L"LPT4") == 0 ||
			ucPath.compare(L"LPT5") == 0 ||
			ucPath.compare(L"LPT6") == 0 ||
			ucPath.compare(L"LPT7") == 0 ||
			ucPath.compare(L"LPT8") == 0 ||
			ucPath.compare(L"LPT9") == 0 ||
			ucPath.compare(L"COM1") == 0 ||
			ucPath.compare(L"COM2") == 0 ||
			ucPath.compare(L"COM3") == 0 ||
			ucPath.compare(L"COM4") == 0 ||
			ucPath.compare(L"COM5") == 0 ||
			ucPath.compare(L"COM6") == 0 ||
			ucPath.compare(L"COM7") == 0 ||
			ucPath.compare(L"COM8") == 0 ||
			ucPath.compare(L"COM9") == 0;
	}

	bool Win32_IsFile(const WString& path)
	{
		return !Win32_IsDirectory(path) && !Win32_IsDevice(path);
	}

	bool Win32_CreateFile(const WString& path)
	{
		HANDLE hFile = CreateFileW(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_NEW, 0, 0);
		if( hFile != INVALID_HANDLE_VALUE )
		{
			CloseHandle(hFile);
			return true;
		}
		else if( GetLastError() == ERROR_FILE_EXISTS )
		{
			return false;
		}
		else
		{
			Win32_HandleError(GetLastError(), path);
		}

		return false;
	}


	bool Win32_CreateDirectory(const WString& path)
	{
		if( Win32_PathExists(path) && Win32_IsDirectory(path))
		{
			return false;
		}

		if( CreateDirectoryW(path.c_str(), 0) == FALSE )
		{
			Win32_HandleError(GetLastError(), path);
		}

		return true;
	}

	void Win32_Remove(const WString& path)
	{
		if( Win32_IsDirectory(path) )
		{
			if( RemoveDirectoryW(path.c_str()) == 0 )
			{
				Win32_HandleError(GetLastError(), path);
			}
		}
		else
		{
			if( DeleteFileW(path.c_str()) == 0 )
			{
				Win32_HandleError(GetLastError(), path);
			}
		}
	}

	void Win32_CopyFile(const WString& from, const WString& to)
	{
		if( CopyFileW(from.c_str(), to.c_str(), FALSE) == FALSE )
		{
			Win32_HandleError(GetLastError(), from);
		}
	}

	void Win32_Rename(const WString& oldPath, const WString& newPath)
	{
		if( MoveFileW(oldPath.c_str(), newPath.c_str()) == 0 )
		{
			Win32_HandleError(GetLastError(), oldPath);
		}
	}

	uint64 Win32_GetFileSize(const WString& path)
	{
		WIN32_FILE_ATTRIBUTE_DATA attrData;
		if( GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attrData) == FALSE)
		{
			Win32_HandleError(GetLastError(), path);
		}

		LARGE_INTEGER li;
		li.LowPart = attrData.nFileSizeLow;
		li.HighPart = attrData.nFileSizeHigh;
		return (uint64)li.QuadPart;
	}

	std::time_t Win32_GetLastModifiedTime(const WString& path)
	{
		WIN32_FILE_ATTRIBUTE_DATA fad;
		if( GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fad) == 0 )
		{
			Win32_HandleError(GetLastError(), path);
		}

		ULARGE_INTEGER ull;
		ull.LowPart = fad.ftLastWriteTime.dwLowDateTime;
		ull.HighPart = fad.ftLastWriteTime.dwHighDateTime;

		return (std::time_t) ((ull.QuadPart / 10000000ULL) - 11644473600ULL);
	}

	DataStreamPtr FileSystem::OpenFile(const Path& fullPath, bool readOnly)
	{
		WString pathWString = fullPath.ToWString();
		const wchar_t* pathString = pathWString.c_str();

		if( !Win32_PathExists(pathString) || !Win32_IsFile(pathString) )
		{
			LOGWRN("Attempting to open a file that doesn't exist: " + fullPath.ToString());
			return nullptr;
		}

		uint64 fileSize = GetFileSize(fullPath);

		//Always open in binary mode. Also, always include reading
		std::ios::openmode mode = std::ios::in | std::ios::binary;
		std::shared_ptr<std::istream> baseStream = 0;
		std::shared_ptr<std::ifstream> roStream = 0;
		std::shared_ptr<std::fstream> rwStream = 0;

		if( !readOnly )
		{
			mode |= std::ios::out;
			rwStream = ge_shared_ptr_new<std::fstream>();
			rwStream->open(pathString, mode);
			baseStream = rwStream;
		}
		else
		{
			roStream = ge_shared_ptr_new<std::ifstream>();
			roStream->open(pathString, mode);
			baseStream = roStream;
		}

		//Should check ensure open succeeded, in case fail for some reason.
		if( baseStream->fail() )
		{
			LOGWRN("Cannot open file: " + fullPath.ToString());
			return nullptr;
		}

		//Construct return stream, tell it to delete on destroy
		FileDataStream* stream = 0;
		if( rwStream )
		{
			//Use the writable stream 
			stream = ge_new<FileDataStream>(rwStream, (SIZE_T)fileSize, true);
		}
		else
		{
			//Read-only stream
			stream = ge_new<FileDataStream>(roStream, (SIZE_T)fileSize, true);
		}

		return ge_shared_ptr<FileDataStream>(stream);
	}

	DataStreamPtr FileSystem::CreateAndOpenFile(const Path& fullPath)
	{
		//Always open in binary mode. Also, always include reading
		std::ios::openmode mode = std::ios::out | std::ios::binary;
		std::shared_ptr<std::fstream> rwStream = ge_shared_ptr_new<std::fstream>();
		rwStream->open(fullPath.ToWString().c_str(), mode);

		//Should check ensure open succeeded, in case fail for some reason.
		if( rwStream->fail() )
		{
			GE_EXCEPT(FileNotFoundException, "Cannot open file: " + fullPath.ToString());
		}

		//Construct return stream, tell it to delete on destroy
		return ge_shared_ptr_new<FileDataStream>(rwStream, 0, true);
	}

	uint64 FileSystem::GetFileSize(const Path& fullPath)
	{
		return Win32_GetFileSize(fullPath.ToWString());
	}

	void FileSystem::Remove(const Path& fullPath, bool recursively)
	{
		WString fullPathStr = fullPath.ToWString();

		if( !FileSystem::Exists(fullPath) )
		{
			return;
		}

		if( recursively )
		{
			Vector<Path> files;
			Vector<Path> directories;

			GetChildren(fullPath, files, directories);

			for( auto& file : files )
			{
				Remove(file, false);
			}

			for( auto& dir : directories )
			{
				Remove(dir, true);
			}
		}

		Win32_Remove(fullPathStr);
	}

	void FileSystem::Move(const Path& oldPath, const Path& newPath, bool overwriteExisting)
	{
		WString oldPathStr = oldPath.ToWString();
		WString newPathStr = newPath.ToWString();

		if( Win32_PathExists(newPathStr) )
		{
			if( overwriteExisting )
			{
				Win32_Remove(newPathStr);
			}
			else
			{
				LOGWRN("Move operation failed because another file already exists at the new path: \"" + toString(newPathStr) + "\"");
				return;
			}
		}

		Win32_Rename(oldPathStr, newPathStr);
	}

	void FileSystem::Copy(const Path& oldPath, const Path& newPath, bool overwriteExisting)
	{
		Stack<std::tuple<Path, Path>> todo;
		todo.push(std::make_tuple(oldPath, newPath));

		while( !todo.empty() )
		{
			auto current = todo.top();
			todo.pop();

			Path sourcePath = std::get<0>(current);
			WString sourcePathStr = sourcePath.ToWString();
			if( !Win32_PathExists(sourcePathStr) )
			{
				continue;
			}

			bool srcIsFile = Win32_IsFile(sourcePathStr);

			Path destinationPath = std::get<1>(current);
			WString destPathStr = destinationPath.ToWString();

			bool destExists = Win32_PathExists(destPathStr);
			if( destExists )
			{
				if( Win32_IsFile(destPathStr) )
				{
					if( overwriteExisting )
					{
						Win32_Remove(destPathStr);
					}
					else
					{
						LOGWRN("Copy operation failed because another file already exists at the new path: \"" + toString(destPathStr) + "\"");
						return;
					}
				}
			}

			bool destIsFile = !destinationPath.GetWExtension().empty();

			if( !srcIsFile && destIsFile )
			{
				LOGWRN("Cannot copy a source folder to a destination file.");
				return;
			}
			else if( srcIsFile && !destIsFile )
			{
				Path destinationFilePath = destinationPath;
				destinationFilePath.Append(sourcePath.GetWTail());

				Win32_CopyFile(sourcePathStr, destinationFilePath.ToWString());
			}
			else if( srcIsFile && destIsFile )
			{
				Win32_CopyFile(sourcePathStr, destPathStr);
			}
			else if (!srcIsFile && !destIsFile)
			{
				if( !destExists )
				{
					Win32_CreateDirectory(destPathStr);
				}

				Vector<Path> files;
				Vector<Path> directories;
				GetChildren(destinationPath, files, directories);

				for( auto& file : files )
				{
					Path fileDestPath = destinationPath;
					fileDestPath.Append(file.GetWTail());

					todo.push(std::make_tuple(file, fileDestPath));
				}

				for( auto& dir : directories )
				{
					Path dirDestPath = destinationPath;
					dirDestPath.Append(dir.GetWTail());

					todo.push(std::make_tuple(dir, dirDestPath));
				}
			}
		}
	}

	bool FileSystem::Exists(const Path& fullPath)
	{
		return Win32_PathExists(fullPath.ToWString());
	}

	bool FileSystem::IsFile(const Path& fullPath)
	{
		WString pathStr = fullPath.ToWString();

		return Win32_PathExists(pathStr) && Win32_IsFile(pathStr);
	}

	bool FileSystem::IsDirectory(const Path& fullPath)
	{
		WString pathStr = fullPath.ToWString();

		return Win32_PathExists(pathStr) && Win32_IsDirectory(pathStr);
	}

	void FileSystem::CreateDir(const Path& fullPath)
	{
		Path parentPath = fullPath;
		while( !Exists(parentPath) && parentPath.GetNumDirectories() > 0 )
		{
			parentPath = parentPath.GetParent();
		}

		for( uint32 i = parentPath.GetNumDirectories(); i < fullPath.GetNumDirectories(); ++i )
		{
			parentPath.Append(fullPath[i]);
			Win32_CreateDirectory(parentPath.ToWString());
		}

		if( fullPath.IsFile() )
		{
			Win32_CreateDirectory(fullPath.ToWString());
		}
	}

	void FileSystem::GetChildren(const Path& dirPath, Vector<Path>& files, Vector<Path>& directories)
	{
		if( dirPath.IsFile() )
		{
			return;
		}

		WString findPath = dirPath.ToWString();
		findPath.append(L"*");

		WIN32_FIND_DATAW findData;
		HANDLE fileHandle = FindFirstFileW(findPath.c_str(), &findData);

		bool lastFailed = false;
		WString tempName;
		do
		{
			if( lastFailed || fileHandle == INVALID_HANDLE_VALUE )
			{
				if( GetLastError() == ERROR_NO_MORE_FILES )
				{
					break;
				}
				else
				{
					Win32_HandleError(GetLastError(), findPath);
				}
			}
			else
			{
				tempName = findData.cFileName;

				if( tempName != L"." && tempName != L".." )
				{
					Path fullPath = dirPath;
					if( (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
					{
						directories.push_back(fullPath.Append(tempName + L"/"));
					}
					else
					{
						files.push_back(fullPath.Append(tempName));
					}
				}
			}

			lastFailed = FindNextFileW(fileHandle, &findData) == FALSE;
		}while(true);

		FindClose(fileHandle);
	}

	bool FileSystem::Iterate(const Path& dirPath, std::function<bool(const Path&)> fileCallback, std::function<bool(const Path&)> dirCallback, bool recursive)
	{
		if( dirPath.IsFile() )
		{
			return true;
		}

		WString findPath = dirPath.ToWString();
		findPath.append(L"*");

		WIN32_FIND_DATAW findData;
		HANDLE fileHandle = FindFirstFileW(findPath.c_str(), &findData);

		bool lastFailed = false;
		WString tempName;
		do
		{
			if( lastFailed || fileHandle == INVALID_HANDLE_VALUE )
			{
				if( GetLastError() != ERROR_NO_MORE_FILES )
				{
					Win32_HandleError(GetLastError(), findPath);
				}

				break;
			}
			else
			{
				tempName = findData.cFileName;

				if( tempName != L"." && tempName != L".." )
				{
					Path fullPath = dirPath;
					if( (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
					{
						Path childDir = fullPath.Append(tempName + L"/");
						if( dirCallback != nullptr )
						{
							if( !dirCallback(childDir) )
							{
								FindClose(fileHandle);
								return false;
							}
						}

						if( recursive )
						{
							if( !Iterate(childDir, fileCallback, dirCallback, recursive) )
							{
								FindClose(fileHandle);
								return false;
							}
						}
					}
					else
					{
						Path filePath = fullPath.Append(tempName);
						if( fileCallback != nullptr )
						{
							if( !fileCallback(filePath) )
							{
								FindClose(fileHandle);
								return false;
							}
						}
					}
				}
			}

			lastFailed = FindNextFileW(fileHandle, &findData) == FALSE;
		}while(true);

		FindClose(fileHandle);
		return true;
	}

	std::time_t FileSystem::GetLastModifiedTime(const Path& fullPath)
	{
		return Win32_GetLastModifiedTime(fullPath.ToWString().c_str());
	}

	Path FileSystem::GetWorkingDirectoryPath()
	{
		return Path(Win32_GetCurrentDirectory());
	}

	Path FileSystem::GetTempDirectoryPath()
	{
		return Path(Win32_GetTempDirectory());
	}
}
