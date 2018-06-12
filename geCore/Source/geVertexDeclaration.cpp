/*****************************************************************************/
/**
 * @file    geVertexDeclaration.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/22
 * @brief   Describes a single vertex element in a vertex declaration.
 *
 * Describes a single vertex element in a vertex declaration.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geVertexDeclaration.h"
#include "geVertexDeclarationRTTI.h"
#include "geHardwareBufferManager.h"
#include "geRenderAPI.h"

namespace geEngineSDK {
  using std::static_pointer_cast;

  VertexElement::VertexElement(uint16 source,
                               uint32 offset,
                               VERTEX_ELEMENT_TYPE::E theType,
                               VERTEX_ELEMENT_SEMANTIC::E semantic,
                               uint16 index,
                               uint32 instanceStepRate)
    : m_source(source),
      m_offset(offset),
      m_type(theType),
      m_semantic(semantic),
      m_index(index),
      m_instanceStepRate(instanceStepRate)
  {}

  uint32
  VertexElement::getSize() const {
    return getTypeSize(m_type);
  }

  uint32
  VertexElement::getTypeSize(VERTEX_ELEMENT_TYPE::E etype) {
    switch (etype)
    {
      case VERTEX_ELEMENT_TYPE::kCOLOR:
      case VERTEX_ELEMENT_TYPE::kCOLOR_ABGR:
      case VERTEX_ELEMENT_TYPE::kCOLOR_ARGB:
        return sizeof(RGBA);
      case VERTEX_ELEMENT_TYPE::kUBYTE4_NORM:
        return sizeof(uint32);
      case VERTEX_ELEMENT_TYPE::kFLOAT1:
        return sizeof(float);
      case VERTEX_ELEMENT_TYPE::kFLOAT2:
        return sizeof(float) * 2;
      case VERTEX_ELEMENT_TYPE::kFLOAT3:
        return sizeof(float) * 3;
      case VERTEX_ELEMENT_TYPE::kFLOAT4:
        return sizeof(float) * 4;
      case VERTEX_ELEMENT_TYPE::kUSHORT1:
        return sizeof(uint16);
      case VERTEX_ELEMENT_TYPE::kUSHORT2:
        return sizeof(uint16) * 2;
      case VERTEX_ELEMENT_TYPE::kUSHORT4:
        return sizeof(uint16) * 4;
      case VERTEX_ELEMENT_TYPE::kSHORT1:
        return sizeof(int16);
      case VERTEX_ELEMENT_TYPE::kSHORT2:
        return sizeof(int16) * 2;
      case VERTEX_ELEMENT_TYPE::kSHORT4:
        return sizeof(int16) * 4;
      case VERTEX_ELEMENT_TYPE::kUINT1:
        return sizeof(uint32);
      case VERTEX_ELEMENT_TYPE::kUINT2:
        return sizeof(uint32) * 2;
      case VERTEX_ELEMENT_TYPE::kUINT3:
        return sizeof(uint32) * 3;
      case VERTEX_ELEMENT_TYPE::kUINT4:
        return sizeof(uint32) * 4;
      case VERTEX_ELEMENT_TYPE::kINT4:
        return sizeof(int32) * 4;
      case VERTEX_ELEMENT_TYPE::kINT1:
        return sizeof(int32);
      case VERTEX_ELEMENT_TYPE::kINT2:
        return sizeof(int32) * 2;
      case VERTEX_ELEMENT_TYPE::kINT3:
        return sizeof(int32) * 3;
      case VERTEX_ELEMENT_TYPE::kUBYTE4:
        return sizeof(uint8) * 4;
      default:
        break;
    }

    return 0;
  }

  uint16
  VertexElement::getTypeCount(VERTEX_ELEMENT_TYPE::E etype) {
    switch (etype)
    {
      case VERTEX_ELEMENT_TYPE::kCOLOR:
      case VERTEX_ELEMENT_TYPE::kCOLOR_ABGR:
      case VERTEX_ELEMENT_TYPE::kCOLOR_ARGB:
        return 4;
      case VERTEX_ELEMENT_TYPE::kFLOAT1:
      case VERTEX_ELEMENT_TYPE::kSHORT1:
      case VERTEX_ELEMENT_TYPE::kUSHORT1:
      case VERTEX_ELEMENT_TYPE::kINT1:
      case VERTEX_ELEMENT_TYPE::kUINT1:
        return 1;
      case VERTEX_ELEMENT_TYPE::kFLOAT2:
      case VERTEX_ELEMENT_TYPE::kSHORT2:
      case VERTEX_ELEMENT_TYPE::kUSHORT2:
      case VERTEX_ELEMENT_TYPE::kINT2:
      case VERTEX_ELEMENT_TYPE::kUINT2:
        return 2;
      case VERTEX_ELEMENT_TYPE::kFLOAT3:
      case VERTEX_ELEMENT_TYPE::kINT3:
      case VERTEX_ELEMENT_TYPE::kUINT3:
        return 3;
      case VERTEX_ELEMENT_TYPE::kFLOAT4:
      case VERTEX_ELEMENT_TYPE::kSHORT4:
      case VERTEX_ELEMENT_TYPE::kUSHORT4:
      case VERTEX_ELEMENT_TYPE::kINT4:
      case VERTEX_ELEMENT_TYPE::kUINT4:
      case VERTEX_ELEMENT_TYPE::kUBYTE4:
      case VERTEX_ELEMENT_TYPE::kUBYTE4_NORM:
        return 4;
      default:
        break;
    }

    GE_EXCEPT(InvalidParametersException, "Invalid type");
    //return 0; //Unreachable code
  }

  VERTEX_ELEMENT_TYPE::E
  VertexElement::getBestColorVertexElementType() {
    //Use the current render system to determine if possible
    if (nullptr != geCoreThread::RenderAPI::instancePtr()) {
      return geCoreThread::RenderAPI::instance().getAPIInfo().
               getColorVertexElementType();
    }
    else {
      //We can't know the specific type right now, so pick a type based on
      //platform
#if GE_PLATFORM == GE_PLATFORM_WIN32
      //Prefer D3D format on Windows
      return VERTEX_ELEMENT_TYPE::kCOLOR_ARGB;
#else
      //Prefer GL format on everything else
      return VERTEX_ELEMENT_TYPE::kCOLOR_ABGR;
#endif
    }
  }

  bool
  VertexElement::operator== (const VertexElement& rhs) const {
    if (m_type != rhs.m_type ||
        m_index != rhs.m_index ||
        m_offset != rhs.m_offset ||
        m_semantic != rhs.m_semantic ||
        m_source != rhs.m_source ||
        m_instanceStepRate != rhs.m_instanceStepRate) {
      return false;
    }

    return true;
  }

  bool
  VertexElement::operator!= (const VertexElement& rhs) const {
    return !(*this == rhs);
  }

  size_t
  VertexElement::getHash(const VertexElement& element) {
    size_t hash = 0;
    hash_combine(hash, element.m_type);
    hash_combine(hash, element.m_index);
    hash_combine(hash, element.m_offset);
    hash_combine(hash, element.m_semantic);
    hash_combine(hash, element.m_source);
    hash_combine(hash, element.m_instanceStepRate);

    return hash;
  }

  VertexDeclarationProperties::VertexDeclarationProperties(
    const Vector<VertexElement>& elements) {
    for (auto& elem : elements) {
      VERTEX_ELEMENT_TYPE::E type = elem.getType();

      if (elem.getType() == VERTEX_ELEMENT_TYPE::kCOLOR) {
        type = VertexElement::getBestColorVertexElementType();
      }

      m_elementList.emplace_back(elem.getStreamIdx(),
                                elem.getOffset(),
                                type,
                                elem.getSemantic(),
                                elem.getSemanticIdx(),
                                elem.getInstanceStepRate());
    }
  }

  bool
  VertexDeclarationProperties::operator==(const VertexDeclarationProperties& rhs) const {
    if (m_elementList.size() != rhs.m_elementList.size()) {
      return false;
    }

    auto myIter = m_elementList.begin();
    auto theirIter = rhs.m_elementList.begin();

    for (; myIter != m_elementList.end() && theirIter != rhs.m_elementList.end();
         ++myIter, ++theirIter) {
      if (!(*myIter == *theirIter)) {
        return false;
      }
    }

    return true;
  }

  bool
  VertexDeclarationProperties::operator!=(const VertexDeclarationProperties& rhs) const {
    return !(*this == rhs);
  }

  const VertexElement*
  VertexDeclarationProperties::getElement(uint16 index) const {
    GE_ASSERT(index < m_elementList.size() && "Index out of bounds");

    auto iter = m_elementList.begin();
    for (uint16 i = 0; i < index; ++i) {
      ++iter;
    }

    return &(*iter);

  }

  const VertexElement*
  VertexDeclarationProperties::findElementBySemantic(VERTEX_ELEMENT_SEMANTIC::E sem,
                                                     uint16 index) const {
    for (auto& elem : m_elementList) {
      if (elem.getSemantic() == sem && elem.getSemanticIdx() == index) {
        return &elem;
      }
    }

    return nullptr;
  }

  Vector<VertexElement>
  VertexDeclarationProperties::findElementsBySource(uint16 source) const {
    Vector<VertexElement> retList;
    for (auto& elem : m_elementList) {
      if (elem.getStreamIdx() == source) {
        retList.push_back(elem);
      }
    }

    return retList;
  }

  uint32
  VertexDeclarationProperties::getVertexSize(uint16 source) const {
    uint32 size = 0;

    for (auto& elem : m_elementList) {
      if (elem.getStreamIdx() == source) {
        size += elem.getSize();
      }
    }

    return size;
  }

  VertexDeclaration::VertexDeclaration(const Vector<VertexElement>& elements)
    : m_properties(elements)
  {}

  SPtr<geCoreThread::VertexDeclaration>
  VertexDeclaration::getCore() const {
    return static_pointer_cast<geCoreThread::VertexDeclaration>(m_coreSpecific);
  }

  SPtr<geCoreThread::CoreObject>
  VertexDeclaration::createCore() const {
    return geCoreThread::HardwareBufferManager::instance().
             createVertexDeclarationInternal(m_properties.m_elementList);
  }

  SPtr<VertexDeclaration>
  VertexDeclaration::create(const SPtr<VertexDataDesc>& desc) {
    return HardwareBufferManager::instance().createVertexDeclaration(desc);
  }

  /***************************************************************************/
  /**
   * Includes
   */
  /***************************************************************************/
  RTTITypeBase*
  VertexDeclaration::getRTTIStatic() {
    return VertexDeclarationRTTI::instance();
  }

  RTTITypeBase*
  VertexDeclaration::getRTTI() const {
    return getRTTIStatic();
  }

  String
  toString(const VERTEX_ELEMENT_SEMANTIC::E& val) {
    switch (val)
    {
      case VERTEX_ELEMENT_SEMANTIC::kPOSITION:
        return "POSITION";
      case VERTEX_ELEMENT_SEMANTIC::kBLEND_WEIGHTS:
        return "BLEND_WEIGHTS";
      case VERTEX_ELEMENT_SEMANTIC::kBLEND_INDICES:
        return "BLEND_INDICES";
      case VERTEX_ELEMENT_SEMANTIC::kNORMAL:
        return "NORMAL";
      case VERTEX_ELEMENT_SEMANTIC::kCOLOR:
        return "COLOR";
      case VERTEX_ELEMENT_SEMANTIC::kTEXCOORD:
        return "TEXCOORD";
      case VERTEX_ELEMENT_SEMANTIC::kBINORMAL:
        return "BINORMAL";
      case VERTEX_ELEMENT_SEMANTIC::kTANGENT:
        return "TANGENT";
      case VERTEX_ELEMENT_SEMANTIC::kPOSITIONT:
        return "POSITIONT";
      case VERTEX_ELEMENT_SEMANTIC::kPSIZE:
        return "PSIZE";
    }

    return "";
  }

  namespace geCoreThread {
    uint32 VertexDeclaration::s_nextFreeId = 0;

    VertexDeclaration::VertexDeclaration(const Vector<VertexElement>& elements,
                                         GPU_DEVICE_FLAGS::E /*deviceMask*/)
      : m_properties(elements)
    {}

    void
    VertexDeclaration::initialize() {
      m_id = s_nextFreeId++;
      CoreObject::initialize();
    }

    SPtr<VertexDeclaration>
    VertexDeclaration::create(const SPtr<VertexDataDesc>& desc,
                              GPU_DEVICE_FLAGS::E deviceMask) {
      return HardwareBufferManager::instance().createVertexDeclaration(desc, deviceMask);
    }

    bool
    VertexDeclaration::isCompatible(const SPtr<VertexDeclaration>& shaderDecl) {
      const Vector<VertexElement>& shaderElems = shaderDecl->getProperties().getElements();
      const Vector<VertexElement>& bufferElems = getProperties().getElements();

      for (const auto & shaderElem : shaderElems) {
        const VertexElement* foundElement = nullptr;
        for (const auto & bufferElem : bufferElems) {
          if (shaderElem.getSemantic() == bufferElem.getSemantic() &&
              shaderElem.getSemanticIdx() == bufferElem.getSemanticIdx()) {
            foundElement = &bufferElem;
            break;
          }
        }

        if (nullptr == foundElement) {
          return false;
        }
      }

      return true;
    }

    Vector<VertexElement>
    VertexDeclaration::getMissingElements(const SPtr<VertexDeclaration>& shaderDecl) {
      Vector<VertexElement> missingElements;

      const Vector<VertexElement>& shaderElems = shaderDecl->getProperties().getElements();
      const Vector<VertexElement>& bufferElems = getProperties().getElements();

      for (const auto & shaderElem : shaderElems) {
        const VertexElement* foundElement = nullptr;
        for (const auto & bufferElem : bufferElems) {
          if (shaderElem.getSemantic() == bufferElem.getSemantic() &&
              shaderElem.getSemanticIdx() == bufferElem.getSemanticIdx()) {
            foundElement = &bufferElem;
            break;
          }
        }

        if (nullptr == foundElement) {
          missingElements.push_back(shaderElem);
        }
      }

      return missingElements;
    }
  }
}
