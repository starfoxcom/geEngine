/*****************************************************************************/
/**
 * @file    geStringTableManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/01
 * @brief   Manages string tables used for localizing text.
 *
 * Manages string tables used for localizing text. Allows you to add and remove
 * different tables and change the active language.
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
#include "geStringTable.h"
#include <geModule.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT GE_SCRIPT_EXPORT(n:StringTables, m:Localization)
    StringTableManager : public Module<StringTableManager>
  {
   public:
    StringTableManager() = default;

    /**
     * @brief Determines the currently active language. Any newly created
     *        strings will use this value.
     */
    GE_SCRIPT_EXPORT(n:ActiveLanguage, pr:setter)
    void
    setActiveLanguage(Language language);

    /**
     * @copydoc setActiveLanguage()
     */
    GE_SCRIPT_EXPORT(n:ActiveLanguage, pr:getter)
    Language
    getActiveLanguage() const {
      return m_activeLanguage;
    }

    /**
     * @brief Returns the string table with the specified id. If the table
     *        doesn't exist new one is created.
     * @param[in] id  Identifier of the string table.
     * @return  String table with the specified identifier.
     */
    GE_SCRIPT_EXPORT()
    HStringTable
    getTable(uint32 id);

    /**
     * @brief Removes the string table with the specified id.
     * @param[in]  id  Identifier of the string table.
     */
    GE_SCRIPT_EXPORT()
    void
    removeTable(uint32 id);

    /**
     * @brief Registers a new string table or replaces an old one at the
     *        specified id.
     * @param[in] id    Identifier of the string table.
     * @param[in] table New string table to assign to the specified identifier.
     */
    GE_SCRIPT_EXPORT()
    void
    setTable(uint32 id, const HStringTable& table);

   private:
    Language m_activeLanguage = StringTable::DEFAULT_LANGUAGE;
    UnorderedMap<uint32, HStringTable> m_tables;
  };

  /**
   * @brief Provides easier access to StringTableManager.
   */
  GE_CORE_EXPORT StringTableManager&
  g_stringTableManager();
}
