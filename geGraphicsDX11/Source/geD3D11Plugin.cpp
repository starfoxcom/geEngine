/*****************************************************************************/
/**
 * @file    geD3D11Plugin.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/11
 * @brief   Define the plugin getPluginName function.
 *
 * Define the plugin getPluginName function.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesD3D11.h"
#include "geD3D11RenderAPIFactory.h"

namespace geEngineSDK {
  extern "C" GE_PLUGIN_EXPORT const char*
  getPluginName() {
    return geCoreThread::D3D11RenderAPIFactory::SYSTEM_NAME;
  }
}
