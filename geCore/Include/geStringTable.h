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
  namespace LOCALIZATION_LANGUAGE {
    enum E {
      kAfar,
      kAbkhazian,
      kAvestan,
      kAfrikaans,
      kAkan,
      kAmharic,
      kAragonese,
      kArabic,
      kAssamese,
      kAvaric,
      kAymara,
      kAzerbaijani,
      kBashkir,
      kBelarusian,
      kBulgarian,
      kBihari,
      kBislama,
      kBambara,
      kBengali,
      kTibetan,
      kBreton,
      kBosnian,
      kCatalan,
      kChechen,
      kChamorro,
      kCorsican,
      kCree,
      kCzech,
      kChurchSlavic,
      kChuvash,
      kWelsh,
      kDanish,
      kGerman,
      kMaldivian,
      kBhutani,
      kEwe,
      kGreek,
      kEnglishUK,
      kEnglishUS,
      kEsperanto,
      kSpanish,
      kEstonian,
      kBasque,
      kPersian,
      kFulah,
      kFinnish,
      kFijian,
      kFaroese,
      kFrench,
      kWesternFrisian,
      kIrish,
      kScottishGaelic,
      kGalician,
      kGuarani,
      kGujarati,
      kManx,
      kHausa,
      kHebrew,
      kHindi,
      kHiriMotu,
      kCroatian,
      kHaitian,
      kHungarian,
      kArmenian,
      kHerero,
      kInterlingua,
      kIndonesian,
      kInterlingue,
      kIgbo,
      kSichuanYi,
      kInupiak,
      kIdo,
      kIcelandic,
      kItalian,
      kInuktitut,
      kJapanese,
      kJavanese,
      kGeorgian,
      kKongo,
      kKikuyu,
      kKuanyama,
      kKazakh,
      kKalaallisut,
      kCambodian,
      kKannada,
      kKorean,
      kKanuri,
      kKashmiri,
      kKurdish,
      kKomi,
      kCornish,
      kKirghiz,
      kLatin,
      kLuxembourgish,
      kGanda,
      kLimburgish,
      kLingala,
      kLaotian,
      kLithuanian,
      kLubaKatanga,
      kLatvian,
      kMalagasy,
      kMarshallese,
      kMaori,
      kMacedonian,
      kMalayalam,
      kMongolian,
      kMoldavian,
      kMarathi,
      kMalay,
      kMaltese,
      kBurmese,
      kNauru,
      kNorwegianBokmal,
      kNdebele,
      kNepali,
      kNdonga,
      kDutch,
      kNorwegianNynorsk,
      kNorwegian,
      kNavaho,
      kNyanja,
      kProvencal,
      kOjibwa,
      kOromo,
      kOriya,
      kOssetic,
      kPunjabi,
      kPali,
      kPolish,
      kPushto,
      kPortuguese,
      kQuechua,
      kRomansh,
      kKirundi,
      kRomanian,
      kRussian,
      kKinyarwanda,
      kSanskrit,
      kSardinian,
      kSindhi,
      kNorthernSami,
      kSangro,
      kSinhalese,
      kSlovak,
      kSlovenian,
      kSamoan,
      kShona,
      kSomali,
      kAlbanian,
      kSerbian,
      kSwati,
      kSesotho,
      kSundanese,
      kSwedish,
      kSwahili,
      kTamil,
      kTelugu,
      kTajik,
      kThai,
      kTigrinya,
      kTurkmen,
      kTagalog,
      kSetswana,
      kTonga,
      kTurkish,
      kTsonga,
      kTatar,
      kTwi,
      kTahitian,
      kUighur,
      kUkrainian,
      kUrdu,
      kUzbek,
      kVenda,
      kVietnamese,
      kVolapuk,
      kWalloon,
      kWolof,
      kXhosa,
      kYiddish,
      kYoruba,
      kZhuang,
      kChinese,
      kZulu,
      kCount //Number of entries
    };
  }

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
      return mIdentifiers.size();
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
    setString(const WString& identifier, Language language, const WString& value);

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
      return mActiveLanguage;
    }

    /**
     * @brief Changes the currently active language. Any newly created strings
     *        will use this value.
     */
    void
    setActiveLanguage(Language language);

    Language mActiveLanguage;
    LanguageData* mActiveLanguageData;
    LanguageData* mDefaultLanguageData;
    LanguageData* mAllLanguages;
    UnorderedSet<WString> mIdentifiers;

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
