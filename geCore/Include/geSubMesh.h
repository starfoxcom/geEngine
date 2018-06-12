/*****************************************************************************/
/**
 * @file    geSubMesh.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/12
 * @brief   Data about a sub-mesh and the type of primitives contained.
 *
 * Data about a sub-mesh range and the type of primitives contained in the
 * range.
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
  struct GE_CORE_EXPORT GE_SCRIPT_EXPORT(pl:true, m:Rendering) SubMesh
  {
    SubMesh()
      : indexOffset(0),
        indexCount(0),
        drawOp(DOT_TRIANGLE_LIST)
    {}

    SubMesh(uint32 indexOffset, uint32 indexCount, DrawOperationType drawOp)
      : indexOffset(indexOffset),
        indexCount(indexCount),
        drawOp(drawOp)
    {}

    uint32 indexOffset;
    uint32 indexCount;
    DrawOperationType drawOp;
  };
}
