/*****************************************************************************/
/**
 * @file    geD3D11RenderAPIFactory.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/11
 * @brief   Handles creation of the DX11 render system.
 *
 * Contains a list of all available drivers.
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
#include "gePrerequisitesD3D11.h"
#include "geD3D11RenderAPI.h"

#include <geRenderAPIManager.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11RenderAPIFactory final : public RenderAPIFactory
    {
     public:
      static constexpr const char* SYSTEM_NAME = "geD3D11RenderAPI";

      /**
       * @copydoc RenderAPIFactory::create
       */
      void
      create() override;

      /**
       * @copydoc RenderAPIFactory::name
       */
      const char*
      name() const override {
        return SYSTEM_NAME;
      }

     private:
      /**
       * @brief Registers the factory with the render system manager when
       *        constructed.
       */
      class InitOnStart
      {
       public:
        InitOnStart() {
          static SPtr<RenderAPIFactory> newFactory;

          if (nullptr == newFactory) {
            newFactory = ge_shared_ptr_new<D3D11RenderAPIFactory>();
            RenderAPIManager::instance().registerFactory(newFactory);
          }
        }
      };

      //Makes sure factory is registered on program start
      static InitOnStart initOnStart;
    };
  }
}
