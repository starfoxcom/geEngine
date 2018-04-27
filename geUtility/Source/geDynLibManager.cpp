/*****************************************************************************/
/**
 * @file    geDynLibManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/06/04
 * @brief   This manager keeps track of all the open dynamic-loading libraries.
 *
 * This manager keeps track of all the open dynamic-loading libraries, opens
 * them and returns references to already-open libraries.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geDynLibManager.h"
#include "geDynLib.h"

namespace geEngineSDK {
  static bool
  operator<(const NPtr<DynLib>& lhs, const NPtr<DynLib>& rhs) {
    return lhs->getName() < rhs->getName();
  }

  static bool
  operator<(const NPtr<DynLib>& lhs, const String& rhs) {
    return lhs->getName() < rhs;
  }

  static bool
  operator<(const String& lhs, const NPtr<DynLib>& rhs) {
    return lhs < rhs->getName();
  }

  DynLibManager::~DynLibManager() {
    //Unload & delete resources in turn
    for (auto& entry : m_loadedLibraries) {
      entry->unload();
      ge_delete(entry.get());
    }

    //Empty the list
    m_loadedLibraries.clear();
  }

  DynLib*
  DynLibManager::load(const String& name) {
    //Add the extension (.dll, .so, ...) if necessary.
    String filename = name;
    const SIZE_T length = filename.length();
    const String extension = String(".") + DynLib::EXTENSION;
    const SIZE_T extLength = extension.length();

    if (length <= extLength || filename.substr(length - extLength) != extension) {
      filename.append(extension);
    }

    if constexpr(nullptr != DynLib::PREFIX) {
      filename.insert(0, DynLib::PREFIX);
    }

    const auto& iterFind = m_loadedLibraries.lower_bound(filename);
    if (iterFind != m_loadedLibraries.end() && (*iterFind)->getName() == filename) {
      return iterFind->get();
    }

    DynLib* newLib = new (ge_alloc<DynLib>()) DynLib(std::move(filename));
    m_loadedLibraries.emplace_hint(iterFind, newLib);
    return newLib;
  }

  void
  DynLibManager::unload(DynLib* lib) {
    const auto& iterFind = m_loadedLibraries.find(lib->getName());
    if (iterFind != m_loadedLibraries.end()) {
      m_loadedLibraries.erase(iterFind);
    }

    lib->unload();
    ge_delete(lib);
  }

  DynLibManager&
  g_dynLibManager() {
    return DynLibManager::instance();
  }
}
