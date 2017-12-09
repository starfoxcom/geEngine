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
  using std::sort;

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
    //Safeguard in case translated strings have different number of parameters
    uint32 actualNumParameters = std::min(numParameterValues, numParameters);

    if (nullptr != parameters) {
      SIZE_T totalNumChars = 0;
      uint32 prevIdx = 0;
      for (uint32 i = 0; i < actualNumParameters; ++i) {
        totalNumChars += (parameterOffsets[i].location - prevIdx) +
                          parameters[parameterOffsets[i].paramIdx].size();
        prevIdx = parameterOffsets[i].location;
      }

      totalNumChars += string.size() - prevIdx;

      outputString.resize(totalNumChars);
      
      //String contiguity required by C++11, but this should work elsewhere as well
      WCHAR* strData = &outputString[0];

      prevIdx = 0;
      for (uint32 i = 0; i < actualNumParameters; ++i) {
        uint32 strSize = parameterOffsets[i].location - prevIdx;
        memcpy(strData, &string[prevIdx], strSize * sizeof(WCHAR));
        strData += strSize;

        WString& param = parameters[parameterOffsets[i].paramIdx];
        memcpy(strData, &param[0], param.size() * sizeof(WCHAR));
        strData += param.size();

        prevIdx = parameterOffsets[i].location;
      }

      memcpy(strData, &string[prevIdx], (string.size() - prevIdx) * sizeof(WCHAR));
    }
    else {
      outputString.resize(string.size());

      //String contiguity required by C++11, but this should work elsewhere as well
      WCHAR* strData = &outputString[0];

      memcpy(strData, &string[0], string.size() * sizeof(WCHAR));
    }
  }

  void
  LocalizedStringData::updateString(const WString& _string) {
    if (nullptr != parameterOffsets) {
      ge_deleteN(parameterOffsets, numParameters);
    }

    Vector<ParamOffset> paramOffsets;

    int32 lastBracket = -1;
    WStringStream bracketChars;
    WStringStream cleanString;
    bool escaped = false;
    uint32 numRemovedChars = 0;
    for (uint32 i = 0; i < static_cast<uint32>(_string.size()); ++i) {
      if ('^' == _string[i] && !escaped) {
        numRemovedChars++;
        escaped = true;
        continue;
      }

      if (-1 == lastBracket) {
        //If current char is non-escaped opening bracket start parameter definition
        if ('{' == _string[i] && !escaped) {
          lastBracket = i;
        }
        else {
          cleanString << _string[i];
        }
      }
      else {
        if (isdigit(_string[i])) {
          bracketChars << _string[i];
        }
        else {
          //If current char is non-escaped closing bracket end parameter definition
          uint32 numParamChars = static_cast<uint32>(bracketChars.tellp());
          if ('}' == _string[i] && numParamChars > 0 && !escaped) {
            //+2 for open and closed brackets
            numRemovedChars += numParamChars + 2;
            uint32 paramIdx = parseUnsignedInt(bracketChars.str());
            paramOffsets.push_back(ParamOffset(paramIdx, i + 1 - numRemovedChars));
          }
          else {
            //Last bracket wasn't really a parameter
            for (uint32 j = lastBracket; j <= i; ++j) {
              cleanString << _string[j];
            }
          }

          lastBracket = -1;

          bracketChars.str(L"");
          bracketChars.clear();
        }
      }

      escaped = false;
    }

    string = cleanString.str();
    numParameters = static_cast<uint32>(paramOffsets.size());

    //Try to find out of order param offsets and fix them
    sort(begin(paramOffsets),
         end(paramOffsets),
         [&](const ParamOffset& a, const ParamOffset& b) {
           return a.paramIdx < b.paramIdx;
         });

    if (paramOffsets.size() > 0) {
      uint32 sequentialIdx = 0;
      uint32 lastParamIdx = paramOffsets[0].paramIdx;
      for (uint32 i = 0; i < numParameters; ++i) {
        if (paramOffsets[i].paramIdx == lastParamIdx) {
          paramOffsets[i].paramIdx = sequentialIdx;
          continue;
        }

        lastParamIdx = paramOffsets[i].paramIdx;
        sequentialIdx++;

        paramOffsets[i].paramIdx = sequentialIdx;
      }
    }

    //Re-sort based on location since we find that more useful at runtime
    sort(begin(paramOffsets),
         end(paramOffsets),
         [&](const ParamOffset& a, const ParamOffset& b) {
           return a.location < b.location;
         });

    parameterOffsets = ge_newN<ParamOffset>(numParameters);
    for (uint32 i = 0; i < numParameters; ++i) {
      parameterOffsets[i] = paramOffsets[i];
    }
  }

  StringTable::StringTable()
    : Resource(false),
      m_activeLanguageData(nullptr),
      m_defaultLanguageData(nullptr),
      m_allLanguages(nullptr) {
    m_allLanguages = ge_newN<LanguageData>(static_cast<uint32>(Language::Count));
    m_defaultLanguageData = &(m_allLanguages[static_cast<uint32>(DEFAULT_LANGUAGE)]);
    m_activeLanguageData = m_defaultLanguageData;
    m_activeLanguage = DEFAULT_LANGUAGE;
  }

  StringTable::~StringTable() {
    ge_deleteN(m_allLanguages, static_cast<uint32>(Language::Count));
  }

  void
  StringTable::setActiveLanguage(Language language) {
    if (language == m_activeLanguage) {
      return;
    }

    m_activeLanguageData = &(m_allLanguages[static_cast<uint32>(language)]);
    m_activeLanguage = language;
  }

  bool
  StringTable::contains(const WString& identifier) {
    return m_identifiers.find(identifier) == m_identifiers.end();
  }

  Vector<WString>
  StringTable::getIdentifiers() const {
    Vector<WString> output;
    for (auto& entry : m_identifiers) {
      output.push_back(entry);
    }
    return output;
  }

  void
  StringTable::setString(const WString& identifier,
                         Language language,
                         const WString& value) {
    LanguageData* curLanguage = &(m_allLanguages[static_cast<uint32>(language)]);
    auto iterFind = curLanguage->strings.find(identifier);

    SPtr<LocalizedStringData> stringData;
    if (iterFind == curLanguage->strings.end()) {
      stringData = ge_shared_ptr_new<LocalizedStringData>();
      curLanguage->strings[identifier] = stringData;
    }
    else {
      stringData = iterFind->second;
    }

    m_identifiers.insert(identifier);
    stringData->updateString(value);
  }

  WString
  StringTable::getString(const WString& identifier, Language language) {
    LanguageData* curLanguage = &(m_allLanguages[static_cast<uint32>(language)]);
    auto iterFind = curLanguage->strings.find(identifier);
    if (iterFind != curLanguage->strings.end()) {
      return iterFind->second->string;
    }
    return identifier;
  }

  void
  StringTable::removeString(const WString& identifier) {
    for (uint32 i = 0; i < static_cast<uint32>(Language::Count); ++i) {
      m_allLanguages[i].strings.erase(identifier);
    }

    m_identifiers.erase(identifier);
  }

  SPtr<LocalizedStringData>
  StringTable::getStringData(const WString& identifier, bool insertIfNonExisting) {
    return getStringData(identifier, m_activeLanguage, insertIfNonExisting);
  }

  SPtr<LocalizedStringData>
  StringTable::getStringData(const WString& identifier,
                             Language language,
                             bool insertIfNonExisting) {
    LanguageData* curLanguage = &(m_allLanguages[static_cast<uint32>(language)]);

    auto iterFind = curLanguage->strings.find(identifier);
    if (iterFind != curLanguage->strings.end()) {
      return iterFind->second;
    }

    auto defaultIterFind = m_defaultLanguageData->strings.find(identifier);
    if (defaultIterFind != m_defaultLanguageData->strings.end()) {
      return defaultIterFind->second;
    }

    if (insertIfNonExisting) {
      setString(identifier, DEFAULT_LANGUAGE, identifier);
      defaultIterFind = m_defaultLanguageData->strings.find(identifier);
      if (defaultIterFind != m_defaultLanguageData->strings.end()) {
        return defaultIterFind->second;
      }
    }

    GE_EXCEPT(InvalidParametersException,
              "There is no string data for the provided identifier.");

    return nullptr;
  }

  HStringTable
  StringTable::create() {
    return static_resource_cast<StringTable>(
             g_resources()._createResourceHandle(_createPtr()));
  }

  SPtr<StringTable>
  StringTable::_createPtr() {
    SPtr<StringTable> scriptCodePtr = ge_core_ptr<StringTable>(
      new (ge_alloc<StringTable>()) StringTable());
    scriptCodePtr->_setThisPtr(scriptCodePtr);
    scriptCodePtr->initialize();

    return scriptCodePtr;
  }

  RTTITypeBase*
  StringTable::getRTTIStatic() {
    return StringTableRTTI::instance();
  }

  RTTITypeBase*
  StringTable::getRTTI() const {
    return StringTable::getRTTIStatic();
  }
}
