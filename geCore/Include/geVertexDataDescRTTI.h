/*****************************************************************************/
/**
 * @file    geVertexDataDescRTTI.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/10
 * @brief   RTTI Objects for geVertexDataDesc.
 *
 * RTTI Objects for geVertexDataDesc.
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
#include "geVertexDataDesc.h"

#include <geRTTIType.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT VertexDataDescRTTI
    : public RTTIType<VertexDataDesc, IReflectable, VertexDataDescRTTI>
  {
   private:
    VertexElement&
    getVertexElementData(VertexDataDesc* obj, uint32 arrayIdx) {
      return obj->m_vertexElements[arrayIdx];
    }

    void
    setVertexElementData(VertexDataDesc* obj,
                         uint32 arrayIdx,
                         VertexElement& value) {
      obj->m_vertexElements[arrayIdx] = value;
    }

    uint32
    getNumVertexElementData(VertexDataDesc* obj) {
      return static_cast<uint32>(obj->m_vertexElements.size());
    }

    void
    setNumVertexElementData(VertexDataDesc* obj, uint32 numElements) {
      obj->m_vertexElements.resize(numElements);
    }

   public:
    VertexDataDescRTTI() {
      addPlainArrayField("m_vertexData",
                         0,
                         &VertexDataDescRTTI::getVertexElementData,
                         &VertexDataDescRTTI::getNumVertexElementData,
                         &VertexDataDescRTTI::setVertexElementData,
                         &VertexDataDescRTTI::setNumVertexElementData);
    }

    SPtr<IReflectable>
    newRTTIObject() override {
      return ge_shared_ptr<VertexDataDesc>(GE_PVT_NEW(VertexDataDesc));
    }

    const String&
    getRTTIName() override {
      static String name = "VertexDataDesc";
      return name;
    }

    uint32
    getRTTIId() override {
      return TYPEID_CORE::kID_VertexDataDesc;
    }
  };
}
