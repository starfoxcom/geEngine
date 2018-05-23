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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include "geRenderAPIFactory.h"

#include <geModule.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT RenderAPIManager final : public Module<RenderAPIManager>
  {
   public:
    RenderAPIManager();
    ~RenderAPIManager();

    /**
     * @brief Starts the render API with the provided name and creates the
     *        primary render window.
     * @param[in] name  Name of the render system to start. Factory for this
                  render system must be previously registered.
     * @param[in] primaryWindowDesc Contains options used for creating the
                  primary window.
     * @return  Created render window if initialization is successful,
                null otherwise.
     */
    SPtr<RenderWindow>
    initialize(const String& name, RENDER_WINDOW_DESC& primaryWindowDesc);

    /**
     * @brief Registers a new render API factory responsible for creating a
     *        specific render system type.
     */
    void
    registerFactory(SPtr<RenderAPIFactory> factory);

   private:
    Vector<SPtr<RenderAPIFactory>> m_availableFactories;
    bool m_renderAPIInitialized;
  };
}
