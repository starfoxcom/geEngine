/*****************************************************************************/
/**
 * @file    geVertexDeclarationRTTI.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/06
 * @brief   RTTI Objects for geVertexDeclaration.
 *
 * RTTI Objects for geVertexDeclaration.
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
#include "geVertexBuffer.h"
#include "geHardwareBufferManager.h"
#include "geVertexDataDesc.h"

#include <geRTTIType.h>

namespace geEngineSDK {
  class VertexDeclarationRTTI
    : public RTTIType<VertexDeclaration, IReflectable, VertexDeclarationRTTI>
  {
   private:
    VertexElement&
    getElement(VertexDeclaration* obj, uint32 idx) {
      Vector<VertexElement>& elemList = obj->m_properties.m_elementList;
      GE_ASSERT(elemList.size() > idx);

      VertexElement& element = elemList[idx];
      return element;
    }

    void
    setElement(VertexDeclaration* obj, uint32 idx, VertexElement& data) {
      Vector<VertexElement>& elemList = obj->m_properties.m_elementList;
      GE_ASSERT(elemList.size() > idx);
      elemList[idx] = data;
    }

    uint32
    getElementArraySize(VertexDeclaration* obj) {
      Vector<VertexElement>& elemList = obj->m_properties.m_elementList;
      return static_cast<uint32>(elemList.size());
    }

    void
    setElementArraySize(VertexDeclaration* obj, uint32 size) {
      Vector<VertexElement>& elemList = obj->m_properties.m_elementList;
      for (size_t i = elemList.size(); i < size; ++i) {
        elemList.emplace_back();
      }
    }

   public:
    VertexDeclarationRTTI() {
      addPlainArrayField("m_elementList",
                         0,
                         &VertexDeclarationRTTI::getElement,
                         &VertexDeclarationRTTI::getElementArraySize,
                         &VertexDeclarationRTTI::setElement,
                         &VertexDeclarationRTTI::setElementArraySize);
    }

    SPtr<IReflectable>
    newRTTIObject() override {
      return HardwareBufferManager::instance().
               createVertexDeclaration(VertexDataDesc::create());
    }

    const String&
    getRTTIName() override {
      static String name = "VertexDeclaration";
      return name;
    }

    uint32
    getRTTIId() override {
      return TYPEID_CORE::kID_VertexDeclaration;
    }
  };
}
