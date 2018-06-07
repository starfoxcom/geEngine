/*****************************************************************************/
/**
 * @file    geVertexDeclaration.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/22
 * @brief   Describes a single vertex element in a vertex declaration.
 *
 * Describes a single vertex element in a vertex declaration.
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
  /**
   * @brief Semantics that are used for identifying the meaning of vertex
   *        buffer elements.
   */
  namespace VERTEX_ELEMENT_SEMANTIC {
    enum E {
      /**
       * Position
       */
      kPOSITION = 1,

      /**
       * Blend weights
       */
      kBLEND_WEIGHTS = 2,

      /**
       * Blend indices
       */
      kBLEND_INDICES = 3,

      /**
       * Normal
       */
      kNORMAL = 4,

      /**
       * Color
       */
      kCOLOR = 5,

      /**
       * UVW coordinate
       */
      kTEXCOORD = 6,

      /**
       * Binormal
       */
      kBINORMAL = 7,

      /**
       * Tangent
       */
      kTANGENT = 8,

      /**
       * Transformed position
       */
      kPOSITIONT = 9,

      /**
       * Point size
       */
      kPSIZE = 10
    };
  }

  /**
   * @brief Types used to identify base types of vertex element contents.
   */
  namespace VERTEX_ELEMENT_TYPE {
    enum E {
      /**
       * 1D floating point value
       */
      kFLOAT1 = 0,

      /**
       * 2D floating point value
       */
      kFLOAT2 = 1,

      /**
       * 3D floating point value
       */
      kFLOAT3 = 2,

      /**
       * 4D floating point value
       */
      kFLOAT4 = 3,

      /**
       * Color encoded in 32-bits (8-bits per channel).
       */
      kCOLOR = 4,

      /**
       * 1D 16-bit signed integer value
       */
      kSHORT1 = 5,

      /**
       * 2D 16-bit signed integer value
       */
      kSHORT2 = 6,

      /**
       * 4D 16-bit signed integer value
       */
      kSHORT4 = 8,

      /**
       * 4D 8-bit unsigned integer value
       */
      kUBYTE4 = 9,

      /**
       * Color encoded in 32-bits (8-bits per channel) in ARGB order)
       */
      kCOLOR_ARGB = 10,

      /**
       * Color encoded in 32-bits (8-bits per channel) in ABGR order)
       */
      kCOLOR_ABGR = 11,

      /**
       * 4D 32-bit unsigned integer value
       */
      kuint4 = 12,

      /**
       * 4D 32-bit signed integer value
       */
      kINT4 = 13,

      /**
       * 1D 16-bit unsigned integer value
       */
      kUSHORT1 = 14,

      /**
       * 2D 16-bit unsigned integer value
       */
      kUSHORT2 = 15,

      /**
       * 4D 16-bit unsigned integer value
       */
      kUSHORT4 = 17,

      /**
       * 1D 32-bit signed integer value
       */
      kINT1 = 18,

      /**
       * 2D 32-bit signed integer value
       */
      kINT2 = 19,

      /**
       * 3D 32-bit signed integer value
       */
      kINT3 = 20,

      /**
       * 1D 32-bit signed integer value
       */
      kuint1 = 21,

      /**
       * 2D 32-bit signed integer value
       */
      kuint2 = 22,

      /**
       * 3D 32-bit signed integer value
       */
      kuint3 = 23,

      /**
       * 4D 8-bit unsigned integer interpreted as a normalized value in [0, 1]
       * range.
       */
      kUBYTE4_NORM = 24,

      /**
       * Keep at end before VET_UNKNOWN
       */
      kCOUNT,

      kUNKNOWN = 0xffff
    };
  }

  class GE_CORE_EXPORT VertexElement
  {
   public:
    VertexElement() = default;
    VertexElement(uint16 source,
                  uint32 offset,
                  VERTEX_ELEMENT_TYPE::E theType,
                  VERTEX_ELEMENT_SEMANTIC::E semantic,
                  uint16 index = 0,
                  uint32 instanceStepRate = 0);

    bool
    operator==(const VertexElement& rhs) const;

    bool
    operator!=(const VertexElement& rhs) const;

    /**
     * @brief Returns index of the vertex buffer from which this element is
     *        stored.
     */
    uint16
    getStreamIdx() const {
      return m_source;
    }

    /**
     * @brief Returns an offset into the buffer where this vertex is stored.
     *        This value might be in bytes but doesn't have to be, it's
     *        likely to be render API specific.
     */
    uint32
    getOffset() const {
      return m_offset;
    }

    /**
     * @brief Gets the base data type of this element.
     */
    VERTEX_ELEMENT_TYPE::E
    getType() const {
      return m_type;
    }

    /**
     * @brief Gets a semantic that describes what this element contains.
     */
    VERTEX_ELEMENT_SEMANTIC::E
    getSemantic() const {
      return m_semantic;
    }

    /**
     * @brief Gets an index of this element. Only relevant when you have
     *        multiple elements with the same semantic, for example uv0, uv1.
     */
    uint16
    getSemanticIdx() const {
      return m_index;
    }

    /**
     * @brief Returns the size of this element in bytes.
     */
    uint32
    getSize() const;

    /**
     * @brief Returns at what rate do the vertex elements advance during
     *        instanced rendering. Provide zero for default behavior where
     *        each vertex receives the next value from the vertex buffer.
     *        Provide a value larger than zero to ensure vertex data is
     *        advanced with every instance, instead of every vertex (for
     *        example a value of 1 means each instance will retrieve a new
     *        value from the vertex buffer, a value of 2 means each second
     *        instance will, etc.).
     */
    uint32
    getInstanceStepRate() const {
      return m_instanceStepRate;
    }

    /**
     * @brief Returns the size of a base element type.
     */
    static uint32
    getTypeSize(VERTEX_ELEMENT_TYPE::E etype);

    /**
     * @brief Returns the number of values in the provided base element type.
     *        For example float4 has four values.
     */
    static uint16
    getTypeCount(VERTEX_ELEMENT_TYPE::E etype);

    /**
     * @brief Gets packed color vertex element type used by the active render
     *        system.
     */
    static VERTEX_ELEMENT_TYPE::E
    getBestColorVertexElementType();

    /**
     * @brief Calculates a hash value for the provided vertex element.
     */
    static size_t
    getHash(const VertexElement& element);

   protected:
    uint16 m_source;
    uint32 m_offset;
    VERTEX_ELEMENT_TYPE::E m_type;
    VERTEX_ELEMENT_SEMANTIC::E m_semantic;
    uint16 m_index;
    uint32 m_instanceStepRate;
  };

  GE_ALLOW_MEMCPY_SERIALIZATION(VertexElement);

  class GE_CORE_EXPORT VertexDeclarationProperties
  {
   public:
    VertexDeclarationProperties(const Vector<VertexElement>& elements);

    bool
    operator==(const VertexDeclarationProperties& rhs) const;

    bool
    operator!=(const VertexDeclarationProperties& rhs) const;

    /**
     * @brief Get the number of elements in the declaration.
     */
    uint32
    getElementCount() const {
      return static_cast<uint32>(m_elementList.size());
    }

    /**
     * @brief Returns a list of vertex elements in the declaration.
     */
    const Vector<VertexElement>&
    getElements() const {
      return m_elementList;
    }

    /**
     * @brief Returns a single vertex element at the specified index.
     */
    const VertexElement*
    getElement(uint16 index) const;

    /**
     * @brief Attempts to find an element by the given semantic and semantic
     *        index. If no element can be found null is returned.
     */
    const VertexElement*
    findElementBySemantic(VERTEX_ELEMENT_SEMANTIC::E sem,
                          uint16 index = 0) const;

    /**
     * @brief Returns a list of all elements that use the provided source index
     */
    Vector<VertexElement>
    findElementsBySource(uint16 source) const;

    /**
     * @brief Returns the total size of all vertex elements using the provided
     *        source index.
     */
    uint32
    getVertexSize(uint16 source) const;

   protected:
    friend class VertexDeclaration;
    friend class VertexDeclarationRTTI;

    Vector<VertexElement> m_elementList;
  };

  /**
   * @brief Describes a set of vertex elements, used for describing contents of
   *        a vertex buffer or inputs to a vertex GPU program.
   * @note  Sim thread.
   */
  class GE_CORE_EXPORT VertexDeclaration
    : public IReflectable, public CoreObject
  {
   public:
    virtual ~VertexDeclaration() = default;

    /**
     * @brief Returns properties describing the vertex declaration.
     */
    const VertexDeclarationProperties&
    getProperties() const {
      return m_properties;
    }

    /**
     * @brief Retrieves a core implementation of a vertex declaration usable
     *        only from the core thread.
     */
    SPtr<geCoreThread::VertexDeclaration>
    getCore() const;

    /**
     * @copydoc HardwareBufferManager::createVertexDeclaration
     */
    static SPtr<VertexDeclaration>
    create(const SPtr<VertexDataDesc>& desc);

   protected:
    friend class HardwareBufferManager;

    VertexDeclaration(const Vector<VertexElement>& elements);

    /**
     * @copydoc CoreObject::createCore
     */
    SPtr<geCoreThread::CoreObject>
    createCore() const override;

   protected:
    VertexDeclarationProperties m_properties;

    /*************************************************************************/
    /**
     * Serialization
     */
    /*************************************************************************/
  public:
    friend class VertexDeclarationRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };

  /**
   * @brief Converts a vertex semantic enum to a readable name.
   */
  GE_CORE_EXPORT String
  toString(const VERTEX_ELEMENT_SEMANTIC::E& val);

  namespace geCoreThread {
    /**
     * @brief Core thread portion of a bs::VertexDeclaration.
     * @note  Core thread.
     */
    class GE_CORE_EXPORT VertexDeclaration : public CoreObject
    {
     public:
      virtual ~VertexDeclaration() = default;

      /**
       * @copydoc CoreObject::initialize
       */
      void
      initialize() override;

      /**
       * @brief Returns properties describing the vertex declaration.
       */
      const VertexDeclarationProperties&
      getProperties() const {
        return m_properties;
      }

      /**
       * @brief Returns an ID unique to this declaration.
       */
      uint32
      getId() const {
        return m_id;
      }

      /**
       * @brief Checks can a vertex buffer declared with this declaration be
       *        bound to a shader defined with the provided declaration.
       */
      bool
      isCompatible(const SPtr<VertexDeclaration>& shaderDecl);

      /**
       * @brief Returns a list of vertex elements that the provided shader's
       *        vertex declaration expects but aren't present in this vertex
       *        declaration.
       */
      Vector<VertexElement>
      getMissingElements(const SPtr<VertexDeclaration>& shaderDecl);

      /**
       * @copydoc HardwareBufferManager::createVertexDeclaration
       */
      static SPtr<VertexDeclaration>
      create(const SPtr<VertexDataDesc>& desc,
             GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT);

     protected:
      friend class HardwareBufferManager;

      VertexDeclaration(const Vector<VertexElement>& elements,
                        GPU_DEVICE_FLAGS::E deviceMask);

      VertexDeclarationProperties m_properties;
      uint32 m_id;

      static uint32 s_nextFreeId;
    };
  }
}
