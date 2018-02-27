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
  DynLibManager::DynLibManager() {}

  DynLibManager::~DynLibManager() {
    //Unload & delete resources in turn
    for (auto& entry : m_loadedLibraries) {
      entry.second->unload();
      ge_delete(entry.second);
    }

    // Empty the list
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
      filename += extension;
    }

    auto iterFind = m_loadedLibraries.find(filename);
    if (iterFind != m_loadedLibraries.end()) {
      return iterFind->second;
    }

    DynLib* newLib = ge_new<DynLib>(filename);
    m_loadedLibraries[filename] = newLib;
    return newLib;
  }

  void
  DynLibManager::unload(DynLib* lib) {
    auto iterFind = m_loadedLibraries.find(lib->getName());
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
