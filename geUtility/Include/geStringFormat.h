/********************************************************************/
/**
 * @file   geStringFormat.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/03/05
 * @brief  Helper class used for string formatting operations
 *
 * Helper class used for string formatting operations
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#pragma once

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/* Template functions specialization																					*/
	/************************************************************************************************************************/
	template<class T> inline std::string ToString(const T& param) { return std::to_string(param); }
	template<class T> inline std::string ToString(T*) { GE_ASSERT("Invalid pointer type."); return std::string(""); }
	template<class T> inline std::wstring ToWString(const T& param) { return std::to_wstring(param); }
	template<class T> inline std::wstring ToWString(T*) { GE_ASSERT("Invalid pointer type.");  return std::wstring(L""); }

	template<> inline std::string ToString(const std::string& param) { return param; }
	template<> inline std::string ToString(const String& param) { return std::string(param.c_str()); }
	template<> inline std::string ToString<const ANSICHAR>(const ANSICHAR* param) { return std::string(param); }
	template<> inline std::string ToString<ANSICHAR>(ANSICHAR* param) { return std::string(param); }

	template<> inline std::wstring ToWString<std::wstring>(const std::wstring& param) { return param; }
	template<> inline std::wstring ToWString<WString>(const WString& param) { return std::wstring(param.c_str()); }
	template<> inline std::wstring ToWString<const UNICHAR>(const UNICHAR* param) { return std::wstring(param); }
	template<> inline std::wstring ToWString<UNICHAR>(UNICHAR* param) { return std::wstring(param); }

	class StringFormat
	{
	private:
		static const uint32 MAX_PARAMS = 20;
		static const uint32 MAX_IDENTIFIER_SIZE = 2;
		static const uint32 MAX_PARAM_REFERENCES = 200;

	private:
		/************************************************************************************************************************/
		/**
		* @brief	Data structure used during string formatting. It holds information about parameter identifiers to replace
		*			with actual parameters.
		*/
		/************************************************************************************************************************/
		struct FormatParamRange
		{
			FormatParamRange()
			{
			}

			FormatParamRange(uint32 start, uint32 identifierSize, uint32 paramIdx)
				: Start(start), IdentifierSize(identifierSize), ParamIdx(paramIdx)
			{
			}

			uint32 Start;
			uint32 IdentifierSize;
			uint32 ParamIdx;
		};

		/************************************************************************************************************************/
		/**
		* @brief	Structure that holds value of a parameter during string formatting.
		*/
		/************************************************************************************************************************/
		template<class T>
		struct ParamData
		{
			T* Buffer;
			uint32 Size;
		};

	public:
		/************************************************************************************************************************/
		/**
		* @brief	Formats the provided string by replacing the identifiers with the provided parameters. The identifiers are
		*			represented like "{0}, {1}" in the source string, where the number represents the position of the parameter
		*			that will be used for replacing the identifier.
		*
		* @note		You may use "\" to escape ID brackets.
		* @note		Maximum ID number is 19 (for a total of 20 unique IDs. e.g. {20} won't be recognized as an Identifier).
		* @note		Total number of parameters that can be referenced is 200.
		*/
		/************************************************************************************************************************/
		template<class T, class... Args>
		static BasicString<T> Format(const T* source, Args&&... args)
		{
			//Get the length of the string
			uint32 strLength = GetLength(source);

			//Create an array to store the parameters and fill it with the parameters sent
			ParamData<T> parameters[MAX_PARAMS];
			memset(parameters, 0, sizeof(parameters));
			GetParams(parameters, 0U, std::forward<Args>(args)...);

			//Brackets characters plus NULL terminator
			T bracketChars[MAX_IDENTIFIER_SIZE + 1];
			uint32 bracketWriteIdx = 0;

			FormatParamRange paramRanges[MAX_PARAM_REFERENCES];
			memset(paramRanges, 0, sizeof(paramRanges));
			uint32 paramRangeWriteIdx = 0;

			//Determine parameter positions
			int32 lastBracket = -1;
			bool escaped = false;
			uint32 charWriteIdx = 0;
			for( uint32 i = 0; i < strLength; i++ )
			{
				if( source[i] == '\\' && !escaped && paramRangeWriteIdx < MAX_PARAM_REFERENCES )
				{
					escaped = true;
					paramRanges[paramRangeWriteIdx++] = FormatParamRange(charWriteIdx, 1, (uint32)-1);	//TODO: Test -1 this with PS4 or change for MAX_UINT32
					continue;
				}

				if( lastBracket == -1 )
				{
					//If current char is non-escaped opening bracket start parameter definition
					if( source[i] == '{' && !escaped )
					{
						lastBracket = (int32)i;
					}
					else
					{
						charWriteIdx++;
					}
				}
				else
				{
					if( isdigit(source[i]) && bracketWriteIdx < MAX_IDENTIFIER_SIZE )
					{
						bracketChars[bracketWriteIdx++] = source[i];
					}
					else
					{
						//If current char is non-escaped closing bracket end parameter definition
						uint32 numParamChars = bracketWriteIdx;
						bool processedBracket = false;
						if( source[i] == '}' && numParamChars > 0 && !escaped )
						{
							bracketChars[bracketWriteIdx] = '\0';
							uint32 paramIdx = StrToInt(bracketChars);

							//Check if exceeded maximum parameter limit
							if( paramIdx < MAX_PARAMS && paramRangeWriteIdx < MAX_PARAM_REFERENCES )
							{
								paramRanges[paramRangeWriteIdx++] = FormatParamRange(charWriteIdx, numParamChars + 2, paramIdx);
								charWriteIdx += parameters[paramIdx].Size;
								processedBracket = true;
							}
						}

						if( !processedBracket )
						{
							//Last bracket wasn't really a parameter
							for( uint32 j=(uint32)lastBracket; j<=i; ++j )
							{
								charWriteIdx++;
							}
						}

						lastBracket = -1;
						bracketWriteIdx = 0;
					}
				}

				escaped = false;
			}

			//Copy the clean string into output buffer
			uint32 finalStringSize = charWriteIdx;

			T* outputBuffer = (T*)ge_alloc(finalStringSize * sizeof(T));
			uint32 copySourceIdx = 0;
			uint32 copyDestIdx = 0;

			for( uint32 i=0; i<paramRangeWriteIdx; i++ )
			{
				const FormatParamRange& rangeInfo = paramRanges[i];
				uint32 copySize = rangeInfo.Start - copyDestIdx;

				memcpy(outputBuffer + copyDestIdx, source + copySourceIdx, copySize * sizeof(T));
				copySourceIdx += copySize + rangeInfo.IdentifierSize;
				copyDestIdx += copySize;

				if( rangeInfo.ParamIdx == (uint32)-1 )
				{
					continue;
				}

				uint32 paramSize = parameters[rangeInfo.ParamIdx].Size;
				memcpy(outputBuffer + copyDestIdx, parameters[rangeInfo.ParamIdx].Buffer, paramSize * sizeof(T));
				copyDestIdx += paramSize;
			}

			memcpy(outputBuffer + copyDestIdx, source + copySourceIdx, (finalStringSize - copyDestIdx) * sizeof(T));

			BasicString<T> outputStr(outputBuffer, finalStringSize);
			ge_free(outputBuffer);

			//Free the memory of all the parameters buffers
			for( uint32 i=0; i<MAX_PARAMS; i++ )
			{
				if( parameters[i].Buffer != nullptr )
				{
					ge_free(parameters[i].Buffer);
				}
			}

			return outputStr;
		}

	private:
		/************************************************************************************************************************/
		/**
		* @brief	Set of methods that can be specialized so we have a generalized way for retrieving length of strings of
		*			different types.
		*/
		/************************************************************************************************************************/
		static uint32 GetLength(const ANSICHAR* source)
		{
			return (uint32)strlen(source);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Set of methods that can be specialized so we have a generalized way for retrieving length of strings of
		*			different types.
		*/
		/************************************************************************************************************************/
		static uint32 GetLength(const UNICHAR* source)
		{
			return (uint32)wcslen(source);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Parses the string and returns an integer value extracted from string characters.
		*/
		/************************************************************************************************************************/
		static uint32 StrToInt(const ANSICHAR* buffer)
		{
			return (uint32)strtoul(buffer, NULL, 10);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Parses the string and returns an integer value extracted from string characters.
		*/
		/************************************************************************************************************************/
		static uint32 StrToInt(const UNICHAR* buffer)
		{
			return (uint32)wcstoul(buffer, nullptr, 10);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Helper method for converting any data type to a narrow string.
		*/
		/************************************************************************************************************************/
		template<class T>
		static std::string ToString(const T& param)
		{
			return geEngineSDK::ToString<T>(param);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Helper method that converts a narrow character array to a narrow string.
		*/
		/************************************************************************************************************************/
		template<class T>
		static std::string ToString(T* param)
		{
			return geEngineSDK::ToString<T>(param);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Helper method for converting any data type to a wide string.
		*/
		/************************************************************************************************************************/
		template<class T>
		static std::wstring ToWString(const T& param)
		{
			return geEngineSDK::ToWString<T>(param);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Helper method that converts a wide character array to a wide string.
		*/
		/************************************************************************************************************************/
		template<class T>
		static std::wstring ToWString(T* param)
		{
			return geEngineSDK::ToWString<T>(param);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Converts all the provided parameters into string representations and populates the provided @p parameters array.
		*/
		/************************************************************************************************************************/
		template<class P, class... Args>
		static void GetParams(ParamData<ANSICHAR>* parameters, uint32 idx, P&& param, Args&&... args)
		{
			if( idx >= MAX_PARAMS )
			{
				return;
			}

			std::basic_string<ANSICHAR> sourceParam = ToString(param);
			parameters[idx].Buffer = (ANSICHAR*)ge_alloc((uint32)sourceParam.size() * sizeof(ANSICHAR));
			parameters[idx].Size = (uint32)sourceParam.size();

			sourceParam.copy(parameters[idx].Buffer, parameters[idx].Size, 0);

			GetParams(parameters, idx + 1, std::forward<Args>(args)...);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Converts all the provided parameters into string representations and populates the provided @p parameters array.
		*/
		/************************************************************************************************************************/
		template<class P, class... Args>
		static void GetParams(ParamData<UNICHAR>* parameters, uint32 idx, P&& param, Args&&... args)
		{
			if( idx >= MAX_PARAMS )
			{
				return;
			}

			std::basic_string<UNICHAR> sourceParam = ToWString(param);
			parameters[idx].Buffer = (UNICHAR*)ge_alloc((uint32)sourceParam.size() * sizeof(UNICHAR));
			parameters[idx].Size = (uint32)sourceParam.size();

			sourceParam.copy(parameters[idx].Buffer, parameters[idx].Size, 0);

			GetParams(parameters, idx + 1, std::forward<Args>(args)...);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Helper method for parameter size calculation. Used as a stopping point in template recursion.
		*/
		/************************************************************************************************************************/
		static void GetParams(ParamData<ANSICHAR>*, uint32)
		{
			// Do nothing
		}

		/************************************************************************************************************************/
		/**
		* @brief	Helper method for parameter size calculation. Used as a stopping point in template recursion.
		*/
		/************************************************************************************************************************/
		static void GetParams(ParamData<UNICHAR>*, uint32)
		{
			// Do nothing
		}		
	};

	
}
