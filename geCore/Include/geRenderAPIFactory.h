/*****************************************************************************/
/**
 * @file    geRenderAPIFactory.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/11
 * @brief   Factory to start up a specific render system.
 *
 * Factory class that you may specialize in order to start up a specific render
 * system.
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

namespace geEngineSDK {
  class RenderAPIFactory
  {
   public:
    virtual ~RenderAPIFactory() = default;

    /**
     * @brief Creates and starts up the render system managed by this factory.
     */
    virtual void
    create() = 0;

    /**
     * @brief Returns the name of the render system this factory creates.
     */
    virtual const char*
    name() const = 0;
  };
}
