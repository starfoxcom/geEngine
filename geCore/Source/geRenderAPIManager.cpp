/*****************************************************************************/
/**
 * @file    geRenderAPIManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/11
 * @brief   Manager that handles render system start up.
 *
 * Manager that handles render system start up.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geRenderAPIManager.h"
#include "geRenderAPI.h"

#include <geException.h>
#include <geDynLib.h>
#include <geDynLibManager.h>

namespace geEngineSDK {
  RenderAPIManager::RenderAPIManager()
    : m_renderAPIInitialized(false)
  {}

  RenderAPIManager::~RenderAPIManager() {
    if (m_renderAPIInitialized) {
      geCoreThread::RenderAPI::instance().destroy();
      geCoreThread::RenderAPI::shutDown();
    }
  }

  SPtr<RenderWindow>
  RenderAPIManager::initialize(const String& pluginFilename,
                               RENDER_WINDOW_DESC& primaryWindowDesc) {
    if (m_renderAPIInitialized) {
      return nullptr;
    }

    DynLib* loadedLibrary = g_dynLibManager().load(pluginFilename);
    const ANSICHAR* name = "";

    if (nullptr != loadedLibrary) {
      using GetPluginNameFunc = const ANSICHAR* (*)();

      GetPluginNameFunc getPluginNameFunc =
        reinterpret_cast<GetPluginNameFunc>(loadedLibrary->getSymbol("getPluginName"));
      name = getPluginNameFunc();
    }

    for (const auto& factory : m_availableFactories) {
      if (strcmp(factory->name(), name) == 0) {
        factory->create();
        m_renderAPIInitialized = true;
        return geCoreThread::RenderAPI::instance().initialize(primaryWindowDesc);
      }
    }

    return nullptr;
  }

  void
  RenderAPIManager::registerFactory(SPtr<RenderAPIFactory> factory) {
    GE_ASSERT(nullptr != factory);
    m_availableFactories.push_back(factory);
  }
}
