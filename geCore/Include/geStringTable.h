/*****************************************************************************/
/**
 * @file    geStringTable.h
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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include "geResource.h"

namespace geEngineSDK {
  enum class GE_SCRIPT_EXPORT(m:Localization) Language {
    Afar,
      Abkhazian,
      Avestan,
      Afrikaans,
      Akan,
      Amharic,
      Aragonese,
      Arabic,
      Assamese,
      Avaric,
      Aymara,
      Azerbaijani,
      Bashkir,
      Belarusian,
      Bulgarian,
      Bihari,
      Bislama,
      Bambara,
      Bengali,
      Tibetan,
      Breton,
      Bosnian,
      Catalan,
      Chechen,
      Chamorro,
      Corsican,
      Cree,
      Czech,
      ChurchSlavic,
      Chuvash,
      Welsh,
      Danish,
      German,
      Maldivian,
      Bhutani,
      Ewe,
      Greek,
      EnglishUK,
      EnglishUS,
      Esperanto,
      Spanish,
      Estonian,
      Basque,
      Persian,
      Fulah,
      Finnish,
      Fijian,
      Faroese,
      French,
      WesternFrisian,
      Irish,
      ScottishGaelic,
      Galician,
      Guarani,
      Gujarati,
      Manx,
      Hausa,
      Hebrew,
      Hindi,
      HiriMotu,
      Croatian,
      Haitian,
      Hungarian,
      Armenian,
      Herero,
      Interlingua,
      Indonesian,
      Interlingue,
      Igbo,
      SichuanYi,
      Inupiak,
      Ido,
      Icelandic,
      Italian,
      Inuktitut,
      Japanese,
      Javanese,
      Georgian,
      Kongo,
      Kikuyu,
      Kuanyama,
      Kazakh,
      Kalaallisut,
      Cambodian,
      Kannada,
      Korean,
      Kanuri,
      Kashmiri,
      Kurdish,
      Komi,
      Cornish,
      Kirghiz,
      Latin,
      Luxembourgish,
      Ganda,
      Limburgish,
      Lingala,
      Laotian,
      Lithuanian,
      LubaKatanga,
      Latvian,
      Malagasy,
      Marshallese,
      Maori,
      Macedonian,
      Malayalam,
      Mongolian,
      Moldavian,
      Marathi,
      Malay,
      Maltese,
      Burmese,
      Nauru,
      NorwegianBokmal,
      Ndebele,
      Nepali,
      Ndonga,
      Dutch,
      NorwegianNynorsk,
      Norwegian,
      Navaho,
      Nyanja,
      Provencal,
      Ojibwa,
      Oromo,
      Oriya,
      Ossetic,
      Punjabi,
      Pali,
      Polish,
      Pushto,
      Portuguese,
      Quechua,
      Romansh,
      Kirundi,
      Romanian,
      Russian,
      Kinyarwanda,
      Sanskrit,
      Sardinian,
      Sindhi,
      NorthernSami,
      Sangro,
      Sinhalese,
      Slovak,
      Slovenian,
      Samoan,
      Shona,
      Somali,
      Albanian,
      Serbian,
      Swati,
      Sesotho,
      Sundanese,
      Swedish,
      Swahili,
      Tamil,
      Telugu,
      Tajik,
      Thai,
      Tigrinya,
      Turkmen,
      Tagalog,
      Setswana,
      Tonga,
      Turkish,
      Tsonga,
      Tatar,
      Twi,
      Tahitian,
      Uighur,
      Ukrainian,
      Urdu,
      Uzbek,
      Venda,
      Vietnamese,
      Volapuk,
      Walloon,
      Wolof,
      Xhosa,
      Yiddish,
      Yoruba,
      Zhuang,
      Chinese,
      Zulu,
      Count //Number of entries
  };

  /**
   * @brief Internal data used for representing a localized string instance.
   *        For example a specific instance of a localized string using
   *        specific parameters.
   */
  struct LocalizedStringData
  {
    struct ParamOffset
    {
      ParamOffset() : paramIdx(0), location(0) {}

      ParamOffset(uint32 _paramIdx, uint32 _location)
        : paramIdx(_paramIdx),
          location(_location)
      {}

      uint32 paramIdx;
      uint32 location;
    };

    LocalizedStringData();
    ~LocalizedStringData();

    void
    concatenateString(WString& outputString,
                      WString* parameters,
                      uint32 numParameterValues) const;

    void
    updateString(const WString& string);

    WString string;
    uint32 numParameters;
    ParamOffset* parameterOffsets;
  };

  /**
   * @brief Data for a single language in the string table.
   */
  struct LanguageData
  {
    UnorderedMap<WString, SPtr<LocalizedStringData>> strings;
  };

  /**
   * @brief Used for string localization. Stores strings and their translations
   *        in various languages.
   */
  class GE_CORE_EXPORT StringTable : public Resource
  {
    //TODO:When editing string table I will need to ensure that all languages
    //of the same string have the same number of parameters
  public:
    StringTable();
    ~StringTable();

    /**
     * @brief Checks does the string table contain the provided identifier.
     * @param[in] identifier  Identifier to look for.
     * @return  True if the identifier exists in the table, false otherwise.
     */
    bool
    contains(const WString& identifier);

    /**
     * @brief Returns a total number of strings in the table.
     */
    SIZE_T
    getNumStrings() const {
      return m_identifiers.size();
    }

    /**
     * @brief Returns all identifiers that the string table contains localized
     *        strings for.
     */
    Vector<WString>
    getIdentifiers() const;

    /**
     * @brief Adds or modifies string translation for the specified language.
     */
    void
    setString(const WString& identifier,
              Language language,
              const WString& value);

    /**
     * @brief Returns a string translation for the specified language.
     *        Returns the identifier itself if one doesn't exist.
     */
    WString
    getString(const WString& identifier, Language language);

    /**
     * @brief Removes the string described by identifier, from all languages.
     */
    void
    removeString(const WString& identifier);

    /**
     * @brief Gets a string data for the specified string identifier and
     *        currently active language.
     * @param[in] identifier  Unique string identifier.
     * @param[in] insertIfNonExisting If true, a new string data for the
     *            specified identifier and language will be added to the table
     *            if data doesn't already exist. The data will use the
     *            identifier as the translation string.
     * @return  The string data. Don't store reference to this data as it may
     *          get deleted.
     */
    SPtr<LocalizedStringData>
    getStringData(const WString& identifier, bool insertIfNonExisting = true);

    /**
     * @brief Gets a string data for the specified identifier and language.
     * @param[in] identifier  Unique string identifier.
     * @param[in] language    Language.
     * @param[in] insertIfNonExisting If true, a new string data for the
     *            specified identifier and language will be added to the table
     *            if data doesn't already exist. The data will use the
     *            identifier as the translation string.
     * @return  The string data. Don't store reference to this data as it may
     *          get deleted.
     */
    SPtr<LocalizedStringData>
    getStringData(const WString& identifier,
                  Language language,
                  bool insertIfNonExisting = true);

    /**
     * @brief Creates a new empty string table resource.
     */
    static HStringTable
    create();

    static const Language DEFAULT_LANGUAGE;

   public:
    /**
     * @brief Creates a new empty string table resource.
     * @note  Internal method. Use create() for normal use.
     */
    static SPtr<StringTable>
    _createPtr();

   private:
    friend class HString;
    friend class StringTableManager;

    /**
     * @brief Gets the currently active language.
     */
    Language
    getActiveLanguage() const {
      return m_activeLanguage;
    }

    /**
     * @brief Changes the currently active language. Any newly created strings
     *        will use this value.
     */
    void
    setActiveLanguage(Language language);

    Language m_activeLanguage;
    LanguageData* m_activeLanguageData;
    LanguageData* m_defaultLanguageData;
    LanguageData* m_allLanguages;
    UnorderedSet<WString> m_identifiers;

    /*************************************************************************/
    /**
    * Serialization
    */
    /*************************************************************************/
   public:
    friend class StringTableRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };
}
