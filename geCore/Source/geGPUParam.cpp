/*****************************************************************************/
/**
 * @file    geGPUParam.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/11
 * @brief   A handle that allows you to set a GPUProgram parameter.
 *
 * A handle that allows you to set a GPUProgram parameter.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geGPUParam.h"
//#include "geGPUParams.h"
//#include "geGPUParamBlockBuffer.h"
#include "geGPUParamDesc.h"
#include "geRenderAPI.h"

#include <geDebug.h>
#include <geException.h>
#include <geVector2I.h>

namespace geEngineSDK {
  using std::min;

  template<class T, bool Core>
  TGPUDataParam<T, Core>::TGPUDataParam() : m_paramDesc(nullptr) {}

  template<class T, bool Core>
  TGPUDataParam<T, Core>::TGPUDataParam(GPUParamDataDesc* paramDesc,
                                        const GPUParamsType& parent)
    : m_parent(parent),
      m_paramDesc(paramDesc)
  {}

  template<class T, bool Core>
  void
  TGPUDataParam<T, Core>::set(const T& value, uint32 arrayIdx) const {
    if (nullptr == m_parent) {
      return;
    }

    GPUParamBufferType paramBlock =
      m_parent->getParamBlockBuffer(m_paramDesc->paramBlockSet,
                                    m_paramDesc->paramBlockSlot);
    if (nullptr == paramBlock) {
      return;
    }

#if GE_DEBUG_MODE
    if (arrayIdx >= m_paramDesc->arraySize) {
      GE_EXCEPT(InvalidParametersException,
                "Array index out of range. Array size: " +
                toString(m_paramDesc->arraySize) +
                ". Requested size: " +
                toString(arrayIdx));
    }
#endif
    uint32 elementSizeBytes = m_paramDesc->elementSize * sizeof(uint32);

    //Truncate if it doesn't fit within parameter size
    uint32 sizeBytes = min(elementSizeBytes, static_cast<uint32>(sizeof(T)));

    bool transposeMatrices = geCoreThread::RenderAPI::instance().
      getAPIInfo().isFlagSet(RENDER_API_FEATURE_FLAG::kColumnMajorMatrices);

    if (TransposePolicy<T>::transposeEnabled(transposeMatrices)) {
      auto transposed = TransposePolicy<T>::transpose(value);
      paramBlock->write((m_paramDesc->cpuMemOffset + arrayIdx *
                          m_paramDesc->arrayElementStride) * sizeof(uint32),
                        &transposed,
                        sizeBytes);
    }
    else {
      paramBlock->write((m_paramDesc->cpuMemOffset + arrayIdx *
                          m_paramDesc->arrayElementStride) * sizeof(uint32),
                        &value,
                        sizeBytes);
    }

    //Set unused bytes to 0
    if (sizeBytes < elementSizeBytes) {
      uint32 diffSize = elementSizeBytes - sizeBytes;
      paramBlock->zeroOut((m_paramDesc->cpuMemOffset + arrayIdx *
                            m_paramDesc->arrayElementStride) *
                            sizeof(uint32) + sizeBytes,
                          diffSize);
    }

    m_parent->_markCoreDirty();
  }

  template<class T, bool Core>
  T
  TGPUDataParam<T, Core>::get(uint32 arrayIdx) const {
    if (nullptr == m_parent) {
      return T();
    }

    GPUParamBufferType paramBlock =
      m_parent->getParamBlockBuffer(m_paramDesc->paramBlockSet,
                                    m_paramDesc->paramBlockSlot);
    if (nullptr == paramBlock) {
      return T();
    }

#if GE_DEBUG_MODE
    if (arrayIdx >= m_paramDesc->arraySize) {
      GE_EXCEPT(InvalidParametersException,
                "Array index out of range. Array size: " +
                toString(m_paramDesc->arraySize) +
                ". Requested size: " +
                toString(arrayIdx));
    }
#endif
    uint32 elementSizeBytes = m_paramDesc->elementSize * sizeof(uint32);
    uint32 sizeBytes = min(elementSizeBytes, static_cast<uint32>(sizeof(T)));

    T value;
    paramBlock->read((m_paramDesc->cpuMemOffset + arrayIdx *
                       m_paramDesc->arrayElementStride) * sizeof(uint32),
                     &value,
                     sizeBytes);
    return value;
  }

  template<bool Core>
  TGPUParamStruct<Core>::TGPUParamStruct() : m_paramDesc(nullptr) {}

  template<bool Core>
  TGPUParamStruct<Core>::TGPUParamStruct(GPUParamDataDesc* paramDesc,
                                         const GPUParamsType& parent)
    : m_parent(parent),
      m_paramDesc(paramDesc)
  {}

  template<bool Core>
  void
  TGPUParamStruct<Core>::set(const void* value,
                             uint32 sizeBytes,
                             uint32 arrayIdx) const {
    if (nullptr == m_parent) {
      return;
    }

    GPUParamBufferType paramBlock =
      m_parent->getParamBlockBuffer(m_paramDesc->paramBlockSet,
                                    m_paramDesc->paramBlockSlot);
    if (nullptr == paramBlock) {
      return;
    }

    uint32 elementSizeBytes = m_paramDesc->elementSize * sizeof(uint32);

#if GE_DEBUG_MODE
    if (sizeBytes > elementSizeBytes) {
      LOGWRN("Provided element size larger than maximum element size. "
             "Maximum size: " + toString(elementSizeBytes) +
             ". Supplied size: " + toString(sizeBytes));
    }

    if (arrayIdx >= m_paramDesc->arraySize) {
      GE_EXCEPT(InvalidParametersException,
                "Array index out of range. Array size: " +
                toString(m_paramDesc->arraySize) +
                ". Requested size: " + toString(arrayIdx));
    }
#endif
    sizeBytes = min(elementSizeBytes, sizeBytes);

    paramBlock->write((m_paramDesc->cpuMemOffset + arrayIdx *
                        m_paramDesc->arrayElementStride) * sizeof(uint32),
                      value,
                      sizeBytes);

    //Set unused bytes to 0
    if (sizeBytes < elementSizeBytes) {
      uint32 diffSize = elementSizeBytes - sizeBytes;
      paramBlock->zeroOut((m_paramDesc->cpuMemOffset + arrayIdx *
                            m_paramDesc->arrayElementStride) *
                            sizeof(uint32) + sizeBytes,
                          diffSize);
    }

    m_parent->_markCoreDirty();
  }

  template<bool Core>
  void
  TGPUParamStruct<Core>::get(void* value,
                             uint32 sizeBytes,
                             uint32 arrayIdx) const {
    if (nullptr == m_parent) {
      return;
    }

    GPUParamBufferType paramBlock =
      m_parent->getParamBlockBuffer(m_paramDesc->paramBlockSet,
                                    m_paramDesc->paramBlockSlot);
    if (nullptr == paramBlock) {
      return;
    }

    uint32 elementSizeBytes = m_paramDesc->elementSize * sizeof(uint32);

#if GE_DEBUG_MODE
    if (sizeBytes > elementSizeBytes) {
      LOGWRN("Provided element size larger than maximum element size. "
             "Maximum size: " + toString(elementSizeBytes) +
             ". Supplied size: " + toString(sizeBytes));
    }

    if (arrayIdx >= m_paramDesc->arraySize) {
      GE_EXCEPT(InvalidParametersException,
                "Array index out of range. Array size: " +
                toString(m_paramDesc->arraySize) +
                ". Requested size: " + toString(arrayIdx));
    }
#endif
    sizeBytes = min(elementSizeBytes, sizeBytes);
    paramBlock->read((m_paramDesc->cpuMemOffset + arrayIdx *
                       m_paramDesc->arrayElementStride) * sizeof(uint32),
                     value,
                     sizeBytes);
  }

  template<bool Core>
  uint32
  TGPUParamStruct<Core>::getElementSize() const {
    if (nullptr == m_parent) {
      return 0;
    }

    return m_paramDesc->elementSize * sizeof(uint32);
  }

  template<bool Core>
  TGPUParamTexture<Core>::TGPUParamTexture() : m_paramDesc(nullptr) {}

  template<bool Core>
  TGPUParamTexture<Core>::TGPUParamTexture(GPUParamObjectDesc* paramDesc,
                                           const GPUParamsType& parent)
    : m_parent(parent),
      m_paramDesc(paramDesc)
  {}

  template<bool Core>
  void
  TGPUParamTexture<Core>::set(const TextureType& texture,
                              const TextureSurface& surface) const {
    if (nullptr == m_parent) {
      return;
    }

    m_parent->setTexture(m_paramDesc->set,
                         m_paramDesc->slot,
                         texture,
                         surface);

    m_parent->_markResourcesDirty();
    m_parent->_markCoreDirty();
  }

  template<bool Core>
  typename TGPUParamTexture<Core>::TextureType
  TGPUParamTexture<Core>::get() const {
    if (nullptr == m_parent) {
      return TextureType();
    }

    return m_parent->getTexture(m_paramDesc->set, m_paramDesc->slot);
  }

  template<bool Core>
  TGPUParamBuffer<Core>::TGPUParamBuffer()
    : m_paramDesc(nullptr)
  {}

  template<bool Core>
  TGPUParamBuffer<Core>::TGPUParamBuffer(GPUParamObjectDesc* paramDesc,
                                         const GPUParamsType& parent)
    : m_parent(parent),
      m_paramDesc(paramDesc)
  {}

  template<bool Core>
  void
  TGPUParamBuffer<Core>::set(const BufferType& buffer) const {
    if (nullptr == m_parent) {
      return;
    }

    m_parent->setBuffer(m_paramDesc->set, m_paramDesc->slot, buffer);

    m_parent->_markResourcesDirty();
    m_parent->_markCoreDirty();
  }

  template<bool Core>
  typename TGPUParamBuffer<Core>::BufferType
  TGPUParamBuffer<Core>::get() const {
    if (nullptr == m_parent) {
      return BufferType();
    }

    return m_parent->getBuffer(m_paramDesc->set, m_paramDesc->slot);
  }

  template<bool Core>
  TGPUParamLoadStoreTexture<Core>::TGPUParamLoadStoreTexture()
    : m_paramDesc(nullptr)
  {}

  template<bool Core>
  TGPUParamLoadStoreTexture<Core>::TGPUParamLoadStoreTexture(GPUParamObjectDesc* paramDesc,
                                                             const GPUParamsType& parent)
    : m_parent(parent),
      m_paramDesc(paramDesc)
  {}

  template<bool Core>
  void
  TGPUParamLoadStoreTexture<Core>::set(const TextureType& texture,
                                       const TextureSurface& surface) const {
    if (nullptr == m_parent) {
      return;
    }

    m_parent->setLoadStoreTexture(m_paramDesc->set,
                                  m_paramDesc->slot,
                                  texture,
                                  surface);

    m_parent->_markResourcesDirty();
    m_parent->_markCoreDirty();
  }

  template<bool Core>
  typename TGPUParamLoadStoreTexture<Core>::TextureType
  TGPUParamLoadStoreTexture<Core>::get() const {
    if (nullptr == m_parent) {
      return TextureType();
    }

    return m_parent->getTexture(m_paramDesc->set, m_paramDesc->slot);
  }

  template<bool Core>
  TGPUParamSampState<Core>::TGPUParamSampState()
    : m_paramDesc(nullptr)
  {}

  template<bool Core>
  TGPUParamSampState<Core>::TGPUParamSampState(GPUParamObjectDesc* paramDesc,
                                               const GPUParamsType& parent)
    : m_parent(parent),
      m_paramDesc(paramDesc)
  {}

  template<bool Core>
  void
  TGPUParamSampState<Core>::set(const SamplerStateType& samplerState) const {
    if (nullptr == m_parent) {
      return;
    }

    m_parent->setSamplerState(m_paramDesc->set,
                              m_paramDesc->slot,
                              samplerState);

    m_parent->_markResourcesDirty();
    m_parent->_markCoreDirty();
  }

  template<bool Core>
  typename TGPUParamSampState<Core>::SamplerStateType
  TGPUParamSampState<Core>::get() const {
    if (nullptr == m_parent) {
      return SamplerStateType();
    }

    return m_parent->getSamplerState(m_paramDesc->set, m_paramDesc->slot);
  }

  template class TGPUDataParam<float, false>;
  template class TGPUDataParam<int, false>;
  template class TGPUDataParam<Color, false>;
  template class TGPUDataParam<Vector2, false>;
  template class TGPUDataParam<Vector3, false>;
  template class TGPUDataParam<Vector4, false>;
  template class TGPUDataParam<Vector2I, false>;
  template class TGPUDataParam<Vector3I, false>;
  template class TGPUDataParam<Vector4I, false>;
  template class TGPUDataParam<Matrix4, false>;

  template class TGPUDataParam<float, true>;
  template class TGPUDataParam<int, true>;
  template class TGPUDataParam<Color, true>;
  template class TGPUDataParam<Vector2, true>;
  template class TGPUDataParam<Vector3, true>;
  template class TGPUDataParam<Vector4, true>;
  template class TGPUDataParam<Vector2I, true>;
  template class TGPUDataParam<Vector3I, true>;
  template class TGPUDataParam<Vector4I, true>;
  template class TGPUDataParam<Matrix4, true>;

  template class TGPUParamStruct<false>;
  template class TGPUParamStruct<true>;

  template class TGPUParamTexture<false>;
  template class TGPUParamTexture<true>;

  template class TGPUParamBuffer<false>;
  template class TGPUParamBuffer<true>;

  template class TGPUParamSampState<false>;
  template class TGPUParamSampState<true>;

  template class TGPUParamLoadStoreTexture<false>;
  template class TGPUParamLoadStoreTexture<true>;
}
