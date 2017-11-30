/*****************************************************************************/
/**
 * @file    geResourceHandle.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/23
 * @brief   Represents a handle to a resource.
 *
 * Represents a handle to a resource. Handles are similar to smart pointers,
 * but they have two advantages:
 * - When loading a resource asynchronously you can be immediately returned the
 *   handle that you may use throughout the engine. The handle will be made
 *   valid as soon as the resource is loaded.
 * - Handles can be serialized and deserialized, therefore saving / restoring
 *   references to their original resource.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include "geResourceHandle.h"
#include "geResource.h"
#include "geResourceHandleRTTI.h"
#include "geResources.h"
#include "geResourceListenerManager.h"

namespace geEngineSDK {

}
