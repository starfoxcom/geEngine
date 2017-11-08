/*****************************************************************************/
/**
 * @file    gePrerequisitesCore.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/01
 * @brief   Provides core engine functionality.
 *
 * Second lowest layer that provides core engine functionality and abstract
 * interfaces for various systems.
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
#include <gePrerequisitesUtil.h>

/**
 * Maximum number of surfaces that can be attached to a multi render target.
 */
#define GE_MAX_MULTIPLE_RENDER_TARGETS 8
#define GE_FORCE_SINGLETHREADED_RENDERING 0

/**
 * Maximum number of individual GPU queues, per type.
 */
#define GE_MAX_QUEUES_PER_TYPE 8

/**
 * Maximum number of hardware devices usable at once.
 */
#define GE_MAX_DEVICES 5U

/**
 * Maximum number of devices one resource can exist at the same time.
 */
#define GE_MAX_LINKED_DEVICES 4U

//DLL export
#if GE_PLATFORM == GE_PLATFORM_WIN32
# if GE_COMPILER == GE_COMPILER_MSVC
#   if defined( GE_STATIC_LIB )
#     define GE_CORE_EXPORT
#   else
#     if defined( GE_CORE_EXPORTS )
#       define GE_CORE_EXPORT __declspec( dllexport )
#     else
#       define GE_CORE_EXPORT __declspec( dllimport )
#     endif
#   endif
# else  //Any other Compiler
#   if defined( GE_STATIC_LIB )
#     define GE_CORE_EXPORT
#   else
#     if defined( GE_CORE_EXPORTS )
#       define GE_CORE_EXPORT __attribute__ ((dllexport))
#     else
#       define GE_CORE_EXPORT __attribute__ ((dllimport))
#     endif
#   endif
# endif
# define GE_CORE_HIDDEN
#else //Linux/Mac settings
# define GE_CORE_EXPORT __attribute__ ((visibility ("default")))
# define GE_CORE_HIDDEN __attribute__ ((visibility ("hidden")))
#endif

#include "geHString.h"

namespace geEngineSDK {

}
