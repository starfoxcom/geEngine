/*****************************************************************************/
/**
 * @file    geDeferredCallManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/25
 * @brief   Allows you to queue calls that can get executed later.
 *
 * Allows you to queue calls that can get executed later.
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
#include <geModule.h>

namespace geEngineSDK {
  using std::function;

  class GE_CORE_EXPORT DeferredCallManager : public Module<DeferredCallManager>
  {
   public:
    DeferredCallManager() = default;

    /**
     * @brief Register a deferred call that will be executed once at the start
     *        of next frame.
     * @param[in] func  The function to execute.
     */
    void
    queueDeferredCall(function<void()> func);

    /**
     * @brief Executes all the scheduled calls. To be called once per frame.
     */
    void
    _update();

   private:
    friend class DeferredCall;
    Vector<function<void()>> m_callbacks;
  };
}
