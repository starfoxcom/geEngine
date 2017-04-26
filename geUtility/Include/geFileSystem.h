/********************************************************************/
/**
 * @file   geFileSystem.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/09/19
 * @brief  Utility class for dealing with files.
 *
 * Utility class for dealing with files.
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#pragma once

#include "gePrerequisitesUtil.h"

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Utility class for dealing with files.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT FileSystem
	{
	public:
		/************************************************************************************************************************/
		/**
		* @brief	Opens a file and returns a data stream capable of reading or writing to that file.
		* @param[in]	fullPath	Full path to a file.
		* @param[in]	readOnly	(optional) If true, returned stream will only be readable.
		*/
		/************************************************************************************************************************/
		static DataStreamPtr OpenFile(const Path& fullPath, bool readOnly = true);

		/************************************************************************************************************************/
		/**
		* @brief	Opens a file and returns a data stream capable of reading and writing to that file. If the file doesn't exist
		*			a one will be created.
		* @param[in]	fullPath	Full path to a file.
		*/
		/************************************************************************************************************************/
		static DataStreamPtr CreateAndOpenFile(const Path& fullPath);

		/************************************************************************************************************************/
		/**
		* @brief	Returns the size of a file in bytes.
		* @param[in]	fullPath	Full path to a file.
		*/
		/************************************************************************************************************************/
		static uint64 GetFileSize(const Path& fullPath);

		/************************************************************************************************************************/
		/**
		* @brief	Deletes a file or a folder at the specified path.
		* @param[in]	fullPath   	Full path to a file or a folder..
		* @param[in]	recursively	(optional) If true, folders will have their contents deleted as well. Otherwise an
		*							exception will be thrown for non-empty folders.
		*/
		/************************************************************************************************************************/
		static void Remove(const Path& fullPath, bool recursively = true);

		/************************************************************************************************************************/
		/**
		* @brief	Moves a file or a folder from one to another path. This can also be used as a rename operation.
		* @param[in]	oldPath			 	Full path to the old file/folder.
		* @param[in]	newPath			 	Full path to the new file/folder.
		* @param[in]	overwriteExisting	(optional) If true, any existing file/folder at the new location will be
		*									overwritten, otherwise an exception will be thrown if a file/folder already exists.
		*/
		/************************************************************************************************************************/
		static void Move(const Path& oldPath, const Path& newPath, bool overwriteExisting = true);

		/************************************************************************************************************************/
		/**
		* @brief	Makes a copy of a file or a folder in the specified path.
		* @param[in]	oldPath			 	Full path to the old file/folder.
		* @param[in]	newPath			 	Full path to the new file/folder.
		* @param[in]	overwriteExisting	(optional) If true, any existing file/folder at the new location will be
		*									overwritten, otherwise an exception will be thrown if a file/folder already exists.
		*/
		/************************************************************************************************************************/
		static void Copy(const Path& oldPath, const Path& newPath, bool overwriteExisting = true);

		/************************************************************************************************************************/
		/**
		* @brief	Creates a folder at the specified path.
		* @param[in]	fullPath	Full path to a full folder to create.
		*/
		/************************************************************************************************************************/
		static void CreateDir(const Path& fullPath);

		/************************************************************************************************************************/
		/**
		* @brief	Returns true if a file or a folder exists at the specified path.
		* @param[in]	fullPath	Full path to a file or folder.
		*/
		/************************************************************************************************************************/
		static bool Exists(const Path& fullPath);

		/************************************************************************************************************************/
		/**
		* @brief	Returns true if a file exists at the specified path.
		* @param[in]	fullPath	Full path to a file or folder.
		*/
		/************************************************************************************************************************/
		static bool IsFile(const Path& fullPath);

		/************************************************************************************************************************/
		/**
		* @brief	Returns true if a folder exists at the specified path.
		* @param[in]	fullPath	Full path to a file or folder.
		*/
		/************************************************************************************************************************/
		static bool IsDirectory(const Path& fullPath);

		/************************************************************************************************************************/
		/**
		* @brief	Returns all files or folders located in the specified folder.
		* @param[in]	dirPath			Full path to the folder to retrieve children files/folders from.
		* @param[out]	files	   		Full paths to all files located directly in specified folder.
		* @param[out]	directories		Full paths to all folders located directly in specified folder.
		*/
		/************************************************************************************************************************/
		static void GetChildren(const Path& dirPath, Vector<Path>& files, Vector<Path>& directories);

		/************************************************************************************************************************/
		/**
		* @brief	Iterates over all files and directories in the specified folder and calls the provided callback when a
		* file/folder is iterated over.
		* @param[in]	dirPath			Directory over which to iterate
		* @param[in]	fileCallback	Callback to call whenever a file is found. If callback returns false iteration stops. Can be null.
		* @param[in]	dirCallback		Callback to call whenever a directory is found. If callback returns false iteration stops. Can be null.
		* @param[in]	recursive		If false then only the direct children of the provided folder will be iterated over,
		*								and if true then child directories will be recursively visited as well.
		* @return						True if iteration finished iterating over all files/folders, or false if it was
		*								interrupted by a callback returning false.
		*/
		/************************************************************************************************************************/
		static bool Iterate(const Path& dirPath, std::function<bool(const Path&)> fileCallback, std::function<bool(const Path&)> dirCallback = nullptr, bool recursive = true);

		/************************************************************************************************************************/
		/**
		* @brief	Returns the last modified time of a file or a folder at the specified path.
		* @param[in]	fullPath	Full path to a file or a folder.
		*/
		/************************************************************************************************************************/
		static std::time_t GetLastModifiedTime(const Path& fullPath);

		/************************************************************************************************************************/
		/**
		* @brief	Returns the path to the currently working directory.
		*/
		/************************************************************************************************************************/
		static Path GetWorkingDirectoryPath();

		/************************************************************************************************************************/
		/**
		* @brief	Returns the path to a directory where temporary files may be stored.
		*/
		/************************************************************************************************************************/
		static Path GetTempDirectoryPath();
	};
}
