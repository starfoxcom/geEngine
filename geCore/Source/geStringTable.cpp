/*****************************************************************************/
/**
 * @file    geStringTable.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/01
 * @brief   A set of all languages that localized strings can be translated to.
 *
 * A set of all languages that localized strings can be translated to. Loosely
 * based on ISO 639-1 two letter language
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geStringTable.h"
#include "geResources.h"
#include "geStringTableRTTI.h"
#include <geException.h>

namespace geEngineSDK {
  const Language StringTable::DEFAULT_LANGUAGE = Language::EnglishUS;

  LocalizedStringData::LocalizedStringData()
    : numParameters(0),
    parameterOffsets(nullptr)
  {}

  LocalizedStringData::~LocalizedStringData() {
    if (nullptr != parameterOffsets) {
      ge_deleteN(parameterOffsets, numParameters);
    }
  }

  void
  LocalizedStringData::concatenateString(WString& outputString,
                                         WString* parameters,
                                         uint32 numParameterValues) const {
    //A safeguard in case translated strings have different number of parameters
    uint32 actualNumParameters = std::min(numParameterValues, numParameters);

    if (nullptr != parameters) {
      uint32 totalNumChars = 0;
      uint32 prevIdx = 0;
      for (uint32 i = 0; i < actualNumParameters; i++)
      {
        totalNumChars += (parameterOffsets[i].location - prevIdx) + (uint32)parameters[parameterOffsets[i].paramIdx].size();;

        prevIdx = parameterOffsets[i].location;
      }

      totalNumChars += (uint32)string.size() - prevIdx;

      outputString.resize(totalNumChars);
      wchar_t* strData = &outputString[0]; // String contiguity required by C++11, but this should work elsewhere as well

      prevIdx = 0;
      for (uint32 i = 0; i < actualNumParameters; i++)
      {
        uint32 strSize = parameterOffsets[i].location - prevIdx;
        memcpy(strData, &string[prevIdx], strSize * sizeof(wchar_t));
        strData += strSize;

        WString& param = parameters[parameterOffsets[i].paramIdx];
        memcpy(strData, &param[0], param.size() * sizeof(wchar_t));
        strData += param.size();

        prevIdx = parameterOffsets[i].location;
      }

      memcpy(strData, &string[prevIdx], (string.size() - prevIdx) * sizeof(wchar_t));
    }
    else
    {
      outputString.resize(string.size());
      wchar_t* strData = &outputString[0]; // String contiguity required by C++11, but this should work elsewhere as well

      memcpy(strData, &string[0], string.size() * sizeof(wchar_t));
    }
  }

  void LocalizedStringData::updateString(const WString& _string)
  {
    if (parameterOffsets != nullptr)
      ge_deleteN(parameterOffsets, numParameters);

    Vector<ParamOffset> paramOffsets;

    int32 lastBracket = -1;
    WStringStream bracketChars;
    WStringStream cleanString;
    bool escaped = false;
    uint32 numRemovedChars = 0;
    for (uint32 i = 0; i < (uint32)_string.size(); i++)
    {
      if (_string[i] == '^' && !escaped)
      {
        numRemovedChars++;
        escaped = true;
        continue;
      }

      if (lastBracket == -1)
      {
        // If current char is non-escaped opening bracket start parameter definition
        if (_string[i] == '{' && !escaped)
          lastBracket = i;
        else
          cleanString << _string[i];
      }
      else
      {
        if (isdigit(_string[i]))
          bracketChars << _string[i];
        else
        {
          // If current char is non-escaped closing bracket end parameter definition
          uint32 numParamChars = (uint32)bracketChars.tellp();
          if (_string[i] == '}' && numParamChars > 0 && !escaped)
          {
            numRemovedChars += numParamChars + 2; // +2 for open and closed brackets

            uint32 paramIdx = parseUnsignedInt(bracketChars.str());
            paramOffsets.push_back(ParamOffset(paramIdx, i + 1 - numRemovedChars));
          }
          else
          {
            // Last bracket wasn't really a parameter
            for (uint32 j = lastBracket; j <= i; j++)
              cleanString << _string[j];
          }

          lastBracket = -1;

          bracketChars.str(L"");
          bracketChars.clear();
        }
      }

      escaped = false;
    }

    string = cleanString.str();
    numParameters = (uint32)paramOffsets.size();

    // Try to find out of order param offsets and fix them
    std::sort(begin(paramOffsets), end(paramOffsets),
      [&](const ParamOffset& a, const ParamOffset& b) { return a.paramIdx < b.paramIdx; });

    if (paramOffsets.size() > 0)
    {
      uint32 sequentialIdx = 0;
      uint32 lastParamIdx = paramOffsets[0].paramIdx;
      for (uint32 i = 0; i < numParameters; i++)
      {
        if (paramOffsets[i].paramIdx == lastParamIdx)
        {
          paramOffsets[i].paramIdx = sequentialIdx;
          continue;
        }

        lastParamIdx = paramOffsets[i].paramIdx;
        sequentialIdx++;

        paramOffsets[i].paramIdx = sequentialIdx;
      }
    }

    // Re-sort based on location since we find that more useful at runtime
    std::sort(begin(paramOffsets), end(paramOffsets),
      [&](const ParamOffset& a, const ParamOffset& b) { return a.location < b.location; });

    parameterOffsets = ge_newN<ParamOffset>(numParameters);
    for (uint32 i = 0; i < numParameters; i++)
      parameterOffsets[i] = paramOffsets[i];
  }

  StringTable::StringTable()
    :Resource(false), mActiveLanguageData(nullptr), mDefaultLanguageData(nullptr), mAllLanguages(nullptr)
  {
    mAllLanguages = ge_newN<LanguageData>((uint32)Language::Count);

    mDefaultLanguageData = &(mAllLanguages[(uint32)DEFAULT_LANGUAGE]);
    mActiveLanguageData = mDefaultLanguageData;
    mActiveLanguage = DEFAULT_LANGUAGE;
  }

  StringTable::~StringTable()
  {
    ge_deleteN(mAllLanguages, (uint32)Language::Count);
  }

  void StringTable::setActiveLanguage(Language language)
  {
    if (language == mActiveLanguage)
      return;

    mActiveLanguageData = &(mAllLanguages[(uint32)language]);
    mActiveLanguage = language;
  }

  bool StringTable::contains(const WString& identifier)
  {
    return mIdentifiers.find(identifier) == mIdentifiers.end();
  }

  Vector<WString> StringTable::getIdentifiers() const
  {
    Vector<WString> output;
    for (auto& entry : mIdentifiers)
      output.push_back(entry);

    return output;
  }

  void StringTable::setString(const WString& identifier, Language language, const WString& value)
  {
    LanguageData* curLanguage = &(mAllLanguages[(uint32)language]);

    auto iterFind = curLanguage->strings.find(identifier);

    SPtr<LocalizedStringData> stringData;
    if (iterFind == curLanguage->strings.end())
    {
      stringData = ge_shared_ptr_new<LocalizedStringData>();
      curLanguage->strings[identifier] = stringData;
    }
    else
    {
      stringData = iterFind->second;
    }

    mIdentifiers.insert(identifier);
    stringData->updateString(value);
  }

  WString StringTable::getString(const WString& identifier, Language language)
  {
    LanguageData* curLanguage = &(mAllLanguages[(uint32)language]);

    auto iterFind = curLanguage->strings.find(identifier);
    if (iterFind != curLanguage->strings.end())
      return iterFind->second->string;

    return identifier;
  }

  void StringTable::removeString(const WString& identifier)
  {
    for (uint32 i = 0; i < (uint32)Language::Count; i++)
    {
      mAllLanguages[i].strings.erase(identifier);
    }

    mIdentifiers.erase(identifier);
  }

  SPtr<LocalizedStringData> StringTable::getStringData(const WString& identifier, bool insertIfNonExisting)
  {
    return getStringData(identifier, mActiveLanguage, insertIfNonExisting);
  }

  SPtr<LocalizedStringData> StringTable::getStringData(const WString& identifier, Language language, bool insertIfNonExisting)
  {
    LanguageData* curLanguage = &(mAllLanguages[(uint32)language]);

    auto iterFind = curLanguage->strings.find(identifier);
    if (iterFind != curLanguage->strings.end())
      return iterFind->second;

    auto defaultIterFind = mDefaultLanguageData->strings.find(identifier);
    if (defaultIterFind != mDefaultLanguageData->strings.end())
      return defaultIterFind->second;

    if (insertIfNonExisting)
    {
      setString(identifier, DEFAULT_LANGUAGE, identifier);

      auto defaultIterFind = mDefaultLanguageData->strings.find(identifier);
      if (defaultIterFind != mDefaultLanguageData->strings.end())
        return defaultIterFind->second;
    }

    GE_EXCEPT(InvalidParametersException, "There is no string data for the provided identifier.");
    return nullptr;
  }

  HStringTable StringTable::create()
  {
    return static_resource_cast<StringTable>(gResources()._createResourceHandle(_createPtr()));
  }

  SPtr<StringTable> StringTable::_createPtr()
  {
    SPtr<StringTable> scriptCodePtr = ge_core_ptr<StringTable>(
      new (ge_alloc<StringTable>()) StringTable());
    scriptCodePtr->_setThisPtr(scriptCodePtr);
    scriptCodePtr->initialize();

    return scriptCodePtr;
  }

  RTTITypeBase* StringTable::getRTTIStatic()
  {
    return StringTableRTTI::instance();
  }

  RTTITypeBase* StringTable::getRTTI() const
  {
    return StringTable::getRTTIStatic();
  }
}
