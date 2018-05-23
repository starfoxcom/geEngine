/*****************************************************************************/
/**
 * @file    gePixelDataRTTI.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/13
 * @brief   RTTI Objects for gePixelData.
 *
 * RTTI Objects for gePixelData.
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
#include "gePixelData.h"

#include <geRTTIType.h>
#include <geDataStream.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT PixelDataRTTI
    : public RTTIType<PixelData, GPUResourceData, PixelDataRTTI>
  {
    uint32&
    getLeft(PixelData* obj) {
      return obj->m_extents.left;
    }

    void
    setLeft(PixelData* obj, uint32& val) {
      obj->m_extents.left = val;
    }

    uint32&
    getTop(PixelData* obj) {
      return obj->m_extents.top;
    }

    void
    setTop(PixelData* obj, uint32& val) {
      obj->m_extents.top = val;
    }

    uint32&
    getRight(PixelData* obj) {
      return obj->m_extents.right;
    }

    void
    setRight(PixelData* obj, uint32& val) {
      obj->m_extents.right = val;
    }

    uint32&
    getBottom(PixelData* obj) {
      return obj->m_extents.bottom;
    }

    void
    setBottom(PixelData* obj, uint32& val) {
      obj->m_extents.bottom = val;
    }

    uint32&
    getFront(PixelData* obj) {
      return obj->m_extents.front;
    }

    void
    setFront(PixelData* obj, uint32& val) {
      obj->m_extents.front = val;
    }

    uint32&
    getBack(PixelData* obj) {
      return obj->m_extents.back;
    }

    void
    setBack(PixelData* obj, uint32& val) {
      obj->m_extents.back = val;
    }

    uint32&
    getRowPitch(PixelData* obj) {
      return obj->m_rowPitch;
    }

    void
    setRowPitch(PixelData* obj, uint32& val) {
      obj->m_rowPitch = val;
    }

    uint32&
    getSlicePitch(PixelData* obj) {
      return obj->m_slicePitch;
    }

    void
    setSlicePitch(PixelData* obj, uint32& val) {
      obj->m_slicePitch = val;
    }

    PixelFormat&
    getFormat(PixelData* obj) {
      return obj->m_format;
    }

    void
    setFormat(PixelData* obj, PixelFormat& val) {
      obj->m_format = val;
    }

    SPtr<DataStream>
    getData(PixelData* obj, uint32& size) {
      size = obj->getConsecutiveSize();
      return ge_shared_ptr_new<MemoryDataStream>(obj->getData(), size, false);
    }

    void
    setData(PixelData* obj, const SPtr<DataStream>& value, uint32 size) {
      obj->allocateInternalBuffer(size);
      value->read(obj->getData(), size);
    }

   public:
    PixelDataRTTI() {
      addPlainField("left",
                    0,
                    &PixelDataRTTI::getLeft,
                    &PixelDataRTTI::setLeft);
      addPlainField("top",
                    1,
                    &PixelDataRTTI::getTop,
                    &PixelDataRTTI::setTop);
      addPlainField("right",
                    2,
                    &PixelDataRTTI::getRight,
                    &PixelDataRTTI::setRight);
      addPlainField("bottom",
                    3,
                    &PixelDataRTTI::getBottom,
                    &PixelDataRTTI::setBottom);
      addPlainField("front",
                    4,
                    &PixelDataRTTI::getFront,
                    &PixelDataRTTI::setFront);
      addPlainField("back",
                    5,
                    &PixelDataRTTI::getBack,
                    &PixelDataRTTI::setBack);
      addPlainField("rowPitch",
                    6,
                    &PixelDataRTTI::getRowPitch,
                    &PixelDataRTTI::setRowPitch);
      addPlainField("slicePitch",
                    7,
                    &PixelDataRTTI::getSlicePitch,
                    &PixelDataRTTI::setSlicePitch);
      addPlainField("format",
                    8,
                    &PixelDataRTTI::getFormat,
                    &PixelDataRTTI::setFormat);
      addDataBlockField("data",
                        9,
                        &PixelDataRTTI::getData,
                        &PixelDataRTTI::setData,
                        0);
    }

    virtual const String&
    getRTTIName() override {
      static String name = "PixelData";
      return name;
    }

    virtual uint32
    getRTTIId() override {
      return TYPEID_CORE::kID_PixelData;
    }

    virtual SPtr<IReflectable>
    newRTTIObject() override {
      SPtr<PixelData> newPixelData = ge_shared_ptr_new<PixelData>();
      return newPixelData;
    }
  };
}
