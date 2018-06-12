/*****************************************************************************/
/**
 * @file    geRenderAPICapabilities.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/12
 * @brief   Holds information about render hardware and driver capabilities.
 *
 * Holds information about render hardware and driver capabilities and allows
 * you to easily set and query those capabilities.
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
#include "geGPUProgram.h"

#define CAPS_CATEGORY_SIZE  INT64_C(8)
#define GE_CAPS_BITSHIFT    (INT64_C(64) - CAPS_CATEGORY_SIZE)
#define CAPS_CATEGORY_MASK  (((INT64_C(1)<<CAPS_CATEGORY_SIZE)-INT64_C(1))<<GE_CAPS_BITSHIFT)

#define GE_CAPS_VALUE(cat, val) ((cat << GE_CAPS_BITSHIFT) | (INT64_C(1) << val))

#define GE_MAX_BOUND_VERTEX_BUFFERS 16

namespace geEngineSDK {
  /**
   * @brief Categories of render API capabilities.
   */
  namespace CAPABILITIES_CATEGORY {
    enum E : uint64 {
      kCOMMON = 0,
      kGL     = 1,
      kD3D11  = 2,
      kVULKAN = 3,
      kCOUNT  = 32  /** Maximum number of categories. */
    };
  }

  /**
   * @brief Enum describing the different hardware capabilities we can
   *        check for.
   */
  namespace CAPABILITIES {
    enum E : uint64 {
      /**
       * Supports compressed textures in the BC formats.
       */
      RSC_TEXTURE_COMPRESSION_BC    = GE_CAPS_VALUE(CAPABILITIES_CATEGORY::kCOMMON, 0),

      /**
       * Supports compressed textures in the ETC2 and EAC format.
       */
      RSC_TEXTURE_COMPRESSION_ETC2  = GE_CAPS_VALUE(CAPABILITIES_CATEGORY::kCOMMON, 1),

      /**
       * Supports compressed textures in the ASTC format.
       */
      RSC_TEXTURE_COMPRESSION_ASTC  = GE_CAPS_VALUE(CAPABILITIES_CATEGORY::kCOMMON, 2),

      /**
       * Supports hardware geometry programs.
       */
      RSC_GEOMETRY_PROGRAM          = GE_CAPS_VALUE(CAPABILITIES_CATEGORY::kCOMMON, 3),

      /**
       * Supports hardware tessellation programs.
       */
      RSC_TESSELLATION_PROGRAM      = GE_CAPS_VALUE(CAPABILITIES_CATEGORY::kCOMMON, 4),

      /**
       * Supports hardware compute programs.
       */
      RSC_COMPUTE_PROGRAM           = GE_CAPS_VALUE(CAPABILITIES_CATEGORY::kCOMMON, 5),
    };
  }

  /**
   * @brief Holds data about render system driver version.
   */
  struct GE_CORE_EXPORT DriverVersion
  {
    DriverVersion() = default;

    /**
     * @brief Returns the driver version as a single string.
     */
    String
    toString() const {
      StringStream str;
      str << major << "." << minor << "." << release << "." << build;
      return str.str();
    }

    /**
     * @brief Parses a string in the major.minor.release.build format and
     *        stores the version numbers.
     */
    void
    fromString(const String& versionString) {
      auto tokens = StringUtil::split(versionString, ".");
      if (!tokens.empty()) {
        major = parseInt(tokens[0]);
        if (tokens.size() > 1) {
          minor = parseInt(tokens[1]);
        }
        if (tokens.size() > 2) {
          release = parseInt(tokens[2]);
        }
        if (tokens.size() > 3) {
          build = parseInt(tokens[3]);
        }
      }
    }

    int32 major = 0;
    int32 minor = 0;
    int32 release = 0;
    int32 build = 0;
  };

  /**
   * @brief Types of GPU vendors.
   */
  namespace GPU_VENDOR {
    enum E {
      kUNKNOWN = 0,
      kNVIDIA = 1,
      kAMD = 2,
      kINTEL = 3,
      kVENDOR_COUNT = 4
    };
  }

  /**
   * @brief Holds information about render hardware and driver capabilities
   *        and allows you to easily set and query those capabilities.
   */
  class GE_CORE_EXPORT RenderAPICapabilities
  {
   public:
    RenderAPICapabilities();
    virtual ~RenderAPICapabilities() = default;

    /**
     * @brief Sets the current driver version.
     */
    void
    setDriverVersion(const DriverVersion& version) {
      m_driverVersion = version;
    }

    /**
     * @brief Returns current driver version.
     */
    DriverVersion
    getDriverVersion() const {
      return m_driverVersion;
    }

    /**
     * @brief Returns vendor of the currently used GPU.
     */
    GPU_VENDOR::E
    getVendor() const {
      return m_vendor;
    }

    /**
     * @brief Sets the GPU vendor.
     */
    void
    setVendor(GPU_VENDOR::E v) {
      m_vendor = v;
    }

    /**
     * @brief Parses a vendor string and returns an enum with the vendor if
     *        parsed successfully.
     */
    static GPU_VENDOR::E
    vendorFromString(const String& vendorString);

    /**
     * @brief Converts a vendor enum to a string.
     */
    static String
    vendorToString(GPU_VENDOR::E v);

    /**
     * @brief Sets the maximum number of texture units per pipeline stage.
     */
    void
    setNumTextureUnits(GPU_PROGRAM_TYPE::E type, uint16 num) {
      m_numTextureUnitsPerStage[type] = num;
    }

    /**
     * @brief Sets the maximum number of texture units in all pipeline stages.
     */
    void
    setNumCombinedTextureUnits(uint16 num) {
      m_numCombinedTextureUnits = num;
    }

    /**
     * @brief Sets the maximum number of load-store texture units per pipeline
     *        stage.
     */
    void
    setNumLoadStoreTextureUnits(GPU_PROGRAM_TYPE::E type, uint16 num) {
      m_numLoadStoreTextureUnitsPerStage[type] = num;
    }

    /**
     * @brief Sets the maximum number of load-store texture units in all
     *        pipeline stages.
     */
    void
    setNumCombinedLoadStoreTextureUnits(uint16 num) {
      m_numCombinedLoadStoreTextureUnits = num;
    }

    /**
     * @brief Sets the maximum number of GPU param block buffers per pipeline
     *        stage.
     */
    void
    setNumGPUParamBlockBuffers(GPU_PROGRAM_TYPE::E type, uint16 num) {
      m_numGPUParamBlocksPerStage[type] = num;
    }

    /**
     * @brief Sets the maximum number of GPU param block buffers in all
     *        pipeline stages.
     */
    void
    setNumCombinedGPUParamBlockBuffers(uint16 num) {
      m_numCombinedUniformBlocks = num;
    }

    /**
     * @brief Sets maximum number of bound vertex buffers.
     */
    void
    setMaxBoundVertexBuffers(uint32 num) {
      m_maxBoundVertexBuffers = num;
    }

    /**
     * @brief Sets maximum number of simultaneously set render targets.
     */
    void
    setNumMultiRenderTargets(uint16 num) {
      m_numMultiRenderTargets = num;
    }

    /**
     * @brief Returns the number of texture units supported per pipeline stage.
     */
    uint16
    getNumTextureUnits(GPU_PROGRAM_TYPE::E type) const {
      auto iterFind = m_numTextureUnitsPerStage.find(type);
      if (m_numTextureUnitsPerStage.end() != iterFind) {
        return iterFind->second;
      }

      return 0;
    }

    /**
     * @brief Returns the number of texture units supported in all pipeline
     * stages.
     */
    uint16
    getNumCombinedTextureUnits() const {
      return m_numCombinedTextureUnits;
    }

    /**
     * @brief Returns the number of load-store texture units supported per
     *        pipeline stage.
     */
    uint16
    getNumLoadStoreTextureUnits(GPU_PROGRAM_TYPE::E type) const {
      auto iterFind = m_numLoadStoreTextureUnitsPerStage.find(type);
      if (m_numLoadStoreTextureUnitsPerStage.end() != iterFind) {
        return iterFind->second;
      }

      return 0;
    }

    /**
     * @brief Returns the number of load-store texture units supported in all
     *        pipeline stages.
     */
    uint16
    getNumCombinedLoadStoreTextureUnits() const {
      return m_numCombinedLoadStoreTextureUnits;
    }

    /**
     * @brief Returns the maximum number of bound GPU program param block
     *        buffers per pipeline stage.
     */
    uint16
    getNumGPUParamBlockBuffers(GPU_PROGRAM_TYPE::E type) const {
      auto iterFind = m_numGPUParamBlocksPerStage.find(type);
      if (m_numGPUParamBlocksPerStage.end() != iterFind) {
        return iterFind->second;
      }

      return 0;
    }

    /**
     * @brief Returns the maximum number of bound GPU program param block
     *        buffers in all pipeline stages.
     */
    uint16
    getNumCombinedGPUParamBlockBuffers() const {
      return m_numCombinedUniformBlocks;
    }

    /**
     * @brief Returns the maximum number of vertex buffers that can be bound
     *        at once.
     */
    uint32
    getMaxBoundVertexBuffers() const {
      return m_maxBoundVertexBuffers;
    }

    /**
     * @brief Returns the maximum number of render targets we can render to
     *        simultaneously.
     */
    uint16
    getNumMultiRenderTargets() const {
      return m_numMultiRenderTargets;
    }

    /**
     * @brief Sets a capability flag indicating this capability is supported.
     */
    void
    setCapability(const CAPABILITIES::E c) {
      uint64 index = (CAPS_CATEGORY_MASK & c) >> GE_CAPS_BITSHIFT;
      m_capabilities[index] |= (c & ~CAPS_CATEGORY_MASK);
    }

    /**
     * @brief Remove a capability flag indicating this capability is not
     *        supported (default).
     */
    void
    unsetCapability(const CAPABILITIES::E c) {
      uint64 index = (CAPS_CATEGORY_MASK & c) >> GE_CAPS_BITSHIFT;
      m_capabilities[index] &= (~c | CAPS_CATEGORY_MASK);
    }

    /**
     * @brief Checks is the specified capability supported.
     */
    bool
    hasCapability(const CAPABILITIES::E c) const {
      uint64 index = (CAPS_CATEGORY_MASK & c) >> GE_CAPS_BITSHIFT;
      return (m_capabilities[index] & (c & ~CAPS_CATEGORY_MASK)) != 0;
    }

    /**
     * @brief Adds a shader profile to the list of render-system specific
     *        supported profiles.
     */
    void
    addShaderProfile(const String& profile) {
      m_supportedShaderProfiles.insert(profile);
    }

    /**
     * @brief Returns true if the provided profile is supported.
     */
    bool
    isShaderProfileSupported(const String& profile) const {
      return (m_supportedShaderProfiles.end() != m_supportedShaderProfiles.find(profile));
    }

    /**
     * @brief Returns a set of all supported shader profiles.
     */
    const Set<String>&
    getSupportedShaderProfiles() const {
      return m_supportedShaderProfiles;
    }

    /**
     * @brief Sets the current GPU device name.
     */
    void
    setDeviceName(const String& name) {
      m_deviceName = name;
    }

    /**
     * @brief Gets the current GPU device name.
     */
    String
    getDeviceName() const {
      return m_deviceName;
    }

    /**
     * @brief Sets the number of vertices a single geometry program run can
     *        emit.
     */
    void
    setGeometryProgramNumOutputVertices(uint32 numOutputVertices) {
      m_geometryProgramNumOutputVertices = numOutputVertices;
    }

    /**
     * @brief Gets the number of vertices a single geometry program run can
     *        emit.
     */
    uint32
    getGeometryProgramNumOutputVertices() const {
      return m_geometryProgramNumOutputVertices;
    }

    /**
     * @brief Get the identifier of the render system from which these
     *        capabilities were generated.
     */
    StringID
    getRenderAPIName() const {
      return m_renderAPIName;
    }

    /**
     * @brief Set the identifier of the render system from which these
     *        capabilities were generated.
     */
    void
    setRenderAPIName(const StringID& rs) {
      m_renderAPIName = rs;
    }

   private:
     /**
      * @brief Initializes vendor enum -> vendor name mappings.
      */
    static void
    initVendorStrings();

   private:
    static Vector<String> s_gpuVendorStrings;

    DriverVersion m_driverVersion;
    GPU_VENDOR::E m_vendor = GPU_VENDOR::kUNKNOWN;

    //The number of texture units available per stage
    Map<GPU_PROGRAM_TYPE::E, uint16> m_numTextureUnitsPerStage;

    //Total number of texture units available
    uint16 m_numCombinedTextureUnits = 0;

    //The number of uniform blocks available per stage
    Map<GPU_PROGRAM_TYPE::E, uint16> m_numGPUParamBlocksPerStage;

    //Total number of uniform blocks available
    uint16 m_numCombinedUniformBlocks = 0;

    //The number of load-store texture units available per stage
    Map<GPU_PROGRAM_TYPE::E, uint16> m_numLoadStoreTextureUnitsPerStage;

    //Total number of load-store texture units available
    uint16 m_numCombinedLoadStoreTextureUnits = 0;

    //Maximum number of vertex buffers we can bind at once
    uint32 m_maxBoundVertexBuffers = 0;

    //Stores the capabilities flags.
    uint32 m_capabilities[CAPABILITIES_CATEGORY::kCOUNT];

    //The name of the device as reported by the render system
    String m_deviceName;

    /**
     * @brief The identifier associated with the render API for which these
     *        capabilities are valid
     */
    StringID m_renderAPIName;

    //The number of simultaneous render targets supported
    uint16 m_numMultiRenderTargets = 0;

    //The number of vertices a geometry program can emit in a single run
    uint32 m_geometryProgramNumOutputVertices = 0;

    //The list of supported shader profiles
    Set<String> m_supportedShaderProfiles;
  };
}
