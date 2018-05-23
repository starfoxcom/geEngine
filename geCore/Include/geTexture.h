/*****************************************************************************/
/**
 * @file    geTexture.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/19
 * @brief   Abstract class representing a texture.
 *
 * Abstract class representing a texture. Specific render systems have their
 * own Texture implementations. Internally represented as one or more surfaces
 * with pixels in a certain number of dimensions, backed by a hardware buffer.
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
#include "geResource.h"
#include "geHardwareBuffer.h"
#include "gePixelUtil.h"
#include "geTextureView.h"
#include "geVectorNI.h"

namespace geEngineSDK {
  /**
   * @brief Flags that describe how is a texture used.
   */
  enum GE_SCRIPT_EXPORT(m:Rendering) TextureUsage {
    /**
     * A regular texture that is not often or ever updated from the CPU.
     */
    TU_STATIC           GE_SCRIPT_EXPORT(n:Default) = GBU_STATIC,

    /**
     * A regular texture that is often updated by the CPU.
     */
    TU_DYNAMIC          GE_SCRIPT_EXPORT(n:Dynamic) = GBU_DYNAMIC,

    /**
     * Texture that can be rendered to by the GPU.
     */
    TU_RENDERTARGET     GE_SCRIPT_EXPORT(n:Render) = 0x200,
    /**
     * Texture used as a depth / stencil buffer by the GPU.
     */
    TU_DEPTHSTENCIL     GE_SCRIPT_EXPORT(n:DepthStencil) = 0x400,

    /**
     * Texture that allows load/store operations from the GPU program.
     */
    TU_LOADSTORE        GE_SCRIPT_EXPORT(n:LoadStore) = 0x800,

    /**
     * All mesh data will also be cached in CPU memory, making it available for
     * fast read access from the CPU.
     */
    TU_CPUCACHED        GE_SCRIPT_EXPORT(n:CPUCached) = 0x1000,

    /**
     * Allows the CPU to directly read the texture data buffers from the GPU.
     */
    TU_CPUREADABLE      GE_SCRIPT_EXPORT(n:CPUReadable) = 0x2000,

    /**
     * Default (most common) texture usage.
     */
    TU_DEFAULT          GE_SCRIPT_EXPORT(ex:true) = TU_STATIC
  };

  /**
   * @brief Texture mipmap options.
   */
  namespace TEXTURE_MIPMAP {
    enum E {
      /**
       * Create all mip maps down to 1x1.
       */
      kUNLIMITED = 0x7FFFFFFF
    };
  }

  /**
   * @brief Descriptor structure used for initialization of a Texture.
   */
  struct TEXTURE_DESC
  {
    /**
     * @brief Type of the texture.
     */
    TextureType type = TEX_TYPE_2D;

    /**
     * @brief Format of pixels in the texture.
     */
    PixelFormat format = PixelFormat::kRGBA8;

    /**
     * @brief Width of the texture in pixels.
     */
    uint32 width = 1;

    /**
     * @brief Height of the texture in pixels.
     */
    uint32 height = 1;

    /**
     * @brief Depth of the texture in pixels (Must be 1 for 2D textures).
     */
    uint32 depth = 1;

    /**
     * @brief Number of mip-maps the texture has. This number excludes the full
     *        resolution map.
     */
    uint32 numMips = 0;

    /**
     * @brief Describes how the caller plans on using the texture in the
     *        pipeline.
     */
    int32 usage = TU_DEFAULT;

    /**
     * @brief If true the texture data is assumed to have been gamma corrected
     *        and will be converted back to linear space when sampled on GPU.
     */
    bool hwGamma = false;

    /**
     * @brief Number of samples per pixel. Set to 1 or 0 to use the default of
     *        a single sample per pixel.
     */
    uint32 numSamples = 0;

    /**
     * @brief Number of texture slices to create if creating a texture array.
     *        Ignored for 3D textures.
     */
    uint32 numArraySlices = 1;
  };

  /**
   * @brief Structure used for specifying information about a texture copy
   *        operation.
   */
  struct TEXTURE_COPY_DESC
  {
    TEXTURE_COPY_DESC() {};

    /**
     * @brief Face from which to copy. This can be an entry in an array of
     *        textures, or a single face of a cube map. If cubemap array, then
     *        each array entry takes up six faces.
     */
    uint32 srcFace = 0;

    /**
     * @brief Mip level from which to copy.
     */
    uint32 srcMip = 0;

    /**
     * @brief Pixel volume from which to copy from. This defaults to all pixels
     *        of the face.
     */
    PixelVolume srcVolume = PixelVolume(0, 0, 0, 0, 0, 0);

    /**
     * @brief Face to which to copy. This can be an entry in an array of
     *        textures, or a single face of a cube map. If cubemap array, then
     *        each array entry takes up six faces.
     */
    uint32 dstFace = 0;

    /**
     * @brief Mip level to which to copy.
     */
    uint32 dstMip = 0;

    /**
     * @brief Coordinates to write the source pixels to. The destination
     *        texture must have enough pixels to fit the entire source volume.
     */
    Vector3I dstPosition;

    GE_CORE_EXPORT static TEXTURE_COPY_DESC DEFAULT;
  };

  /**
   * @brief Properties of a Texture. Shared between sim and core thread
   *        versions of a Texture.
   */
  class GE_CORE_EXPORT TextureProperties
  {
   public:
    TextureProperties() = default;
    TextureProperties(const TEXTURE_DESC& desc);

    /**
     * @brief Gets the type of texture.
     */
    TextureType
    getTextureType() const {
      return m_desc.type;
    }

    /**
     * @brief Gets the number of mipmaps to be used for this texture.
     *        This number excludes the top level map (which is always assumed
     *        to be present).
     */
    uint32
    getNumMipmaps() const {
      return m_desc.numMips;
    }

    /**
     * @brief Determines does the texture contain gamma corrected data.
     *        If true then the GPU will automatically convert the pixels to
     *        linear space before reading from the texture, and convert them to
     *        gamma space when writing to the texture.
     */
    bool
    isHardwareGammaEnabled() const {
      return m_desc.hwGamma;
    }

    /**
     * @brief Gets the number of samples used for multi sampling
     *        (0 or 1 if multi sampling is not used).
     */
    uint32
    getNumSamples() const {
      return m_desc.numSamples;
    }

    /**
     * @brief Returns the height of the texture.
     */
    uint32
    getHeight() const {
      return m_desc.height;
    }

    /**
     * @brief Returns the width of the texture.
     */
    uint32
    getWidth() const {
      return m_desc.width;
    }

    /**
     * @brief Returns the depth of the texture (only for 3D textures).
     */
    uint32
    getDepth() const {
      return m_desc.depth;
    }

    /**
     * @brief Returns a value that signals the engine in what way is the
     *        texture expected to be used.
     */
    int32
    getUsage() const {
      return m_desc.usage;
    }

    /**
     * @brief Returns the pixel format for the texture surface.
     */
    PixelFormat
    getFormat() const {
      return m_desc.format;
    }

    /**
     * @brief Returns true if the texture has an alpha layer.
     */
    bool
    hasAlpha() const;

    /**
     * @brief Returns the number of faces this texture has. This includes array
     *        slices (if texture is an array texture), as well as cube-map
     *        faces.
     */
    uint32
    getNumFaces() const;

    /**
     * @brief Returns the number of array slices of the texture
     *        (if the texture is an array texture).
     */
    uint32
    getNumArraySlices() const {
      return m_desc.numArraySlices;
    }

    /**
     * @brief Allocates a buffer that exactly matches the format of the texture
     *        described by these properties, for the provided face and mip
     *        level. This is a helper function, primarily meant for creating
     *        buffers when reading from, or writing to a texture.
     * @note  Thread safe.
     */
    SPtr<PixelData>
    allocBuffer(uint32 face, uint32 mipLevel) const;

   protected:
    friend class TextureRTTI;
    friend class Texture;

    /**
     * @brief Maps a sub-resource index to an exact face and mip level.
     *        Sub-resource indexes are used when reading or writing to the
     *        resource.
     */
    void
    mapFromSubresourceIdx(uint32 subresourceIdx,
                          uint32& face,
                          uint32& mip) const;

    /**
     * @brief Map a face and a mip level to a sub-resource index you can use
     *        for updating or reading a specific sub-resource.
     */
    uint32
    mapToSubresourceIdx(uint32 face, uint32 mip) const;

    TEXTURE_DESC m_desc;
  };

  /**
   * @brief Abstract class representing a texture. Specific render systems have
   *        their own Texture implementations. Internally represented as one or
   *        more surfaces with pixels in a certain number of dimensions, backed
   *        by a hardware buffer.
   * @note  Sim thread.
   */
  class GE_CORE_EXPORT GE_SCRIPT_EXPORT(m:Rendering) Texture : public Resource
  {
   public:
    /**
     * @brief Updates the texture with new data. Provided data buffer will be
     *        locked until the operation completes.
     * @param[in] data      Pixel data to write. User must ensure it is in
     *            format and size compatible with the texture.
     * @param[in] face      Texture face to write to.
     * @param[in] mipLevel  Mipmap level to write to.
     * @param[in] discardEntireBuffer When true the existing contents of the
     *            resource you are updating will be discarded. This can make
     *            the operation faster. Resources with certain buffer types
     *            might require this flag to be in a specific state otherwise
     *            the operation will fail.
     * @return Async operation object you can use to track operation completion.
     * @note This is an @ref asyncMethod "asynchronous method".
     */
    AsyncOp
    writeData(const SPtr<PixelData>& data,
              uint32 face = 0,
              uint32 mipLevel = 0,
              bool discardEntireBuffer = false);

    /**
     * @brief Reads internal texture data to the provided previously allocated
     *        buffer. Provided data buffer will be locked until the operation
     *        completes.
     * @param[out]  data      Pre-allocated buffer of proper size and format
     *              where data will be read to. You can use
     *              TextureProperties::allocBuffer() to allocate a buffer of a
     *              correct format and size.
     * @param[in]   face      Texture face to read from.
     * @param[in]   mipLevel  Mipmap level to read from.
     * @return  Async operation object you can use to track operation
     *          completion.
     * @note This is an @ref asyncMethod "asynchronous method".
     */
    AsyncOp
    readData(const SPtr<PixelData>& data,
             uint32 face = 0,
             uint32 mipLevel = 0);

    /**
     * @brief Reads data from the cached system memory texture buffer into the
     *        provided buffer.
     * @param[out]  data      Pre-allocated buffer of proper size and format
     *              where data will be read to. You can use
     *              TextureProperties::allocBuffer() to allocate a buffer of a
     *              correct format and size.
     * @param[in]   face      Texture face to read from.
     * @param[in]   mipLevel  Mipmap level to read from.
     * @note  The data read is the cached texture data. Any data written to the
     *        texture from the GPU or core thread will not be reflected in this
     *        data. Use readData() if you require those changes.
     * @note  The texture must have been created with TU_CPUCACHED usage
     *        otherwise this method will not return any data.
     */
    void
    readCachedData(PixelData& data, uint32 face = 0, uint32 mipLevel = 0);

    /**
     * @brief Returns properties that contain information about the texture.
     */
    const TextureProperties&
    getProperties() const {
      return m_properties;
    }

    /**
     * @brief Retrieves a core implementation of a texture usable only from the
     *        core thread.
     */
    SPtr<geCoreThread::Texture>
    getCore() const;

    /*************************************************************************/
    /**
     * Statics
     */
    /*************************************************************************/

    /**
     * @brief Creates a new empty texture.
     * @param[in] desc  Description of the texture to create.
    */
    static HTexture
    create(const TEXTURE_DESC& desc);

    /**
     * @brief Creates a new 2D or 3D texture initialized using the provided
     *        pixel data. Texture will not have any mipmaps.
     * @param[in] pixelData   Data to initialize the texture width.
     * @param[in] usage       Describes planned texture use.
     * @param[in] hwGammaCorrection If true the texture data is assumed to have
     *            been gamma corrected and will be converted back to linear
     *            space when sampled on GPU.
     */
    static HTexture
    create(const SPtr<PixelData>& pixelData,
           int32 usage = TU_DEFAULT,
           bool hwGammaCorrection = false);

    /**
     * @copydoc create(const TEXTURE_DESC&)
     * @note  Internal method. Creates a texture pointer without a handle.
     *        Use create() for normal usage.
     */
    static SPtr<Texture>
    _createPtr(const TEXTURE_DESC& desc);

    /**
     * @copydoc create(const SPtr<PixelData>&, int, bool)
     * @note  Internal method. Creates a texture pointer without a handle.
     *        Use create() for normal usage.
     */
    static SPtr<Texture>
    _createPtr(const SPtr<PixelData>& pixelData,
               int32 usage = TU_DEFAULT,
               bool hwGammaCorrection = false);

   protected:
    friend class TextureManager;

    Texture(const TEXTURE_DESC& desc);
    Texture(const TEXTURE_DESC& desc, const SPtr<PixelData>& pixelData);

    /**
     * @copydoc Resource::initialize
     */
    void
    initialize() override;

    /**
     * @copydoc CoreObject::createCore
     */
    SPtr<geCoreThread::CoreObject>
    createCore() const override;

    /**
     * @brief Calculates the size of the texture, in bytes.
     */
    uint32
    calculateSize() const;

    /**
     * @brief Creates buffers used for caching of CPU texture data.
     * @note  Make sure to initialize all texture properties before calling
     *        this.
     */
    void
    createCPUBuffers();

    /**
     * @brief Updates the cached CPU buffers with new data.
     */
    void
    updateCPUBuffers(uint32 subresourceIdx, const PixelData& data);

   protected:
    Vector<SPtr<PixelData>> m_cpuSubresourceData;
    TextureProperties m_properties;
    mutable SPtr<PixelData> m_initData;

    /*************************************************************************/
    /**
     * RTTI
     */
    /*************************************************************************/
   public:
    Texture() = default;  //Serialization only

    friend class TextureRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };

  namespace geCoreThread {
    /**
     * @brief Core thread version of a geEngineSDK::Texture.
     * @note  Core thread.
     */
    class GE_CORE_EXPORT Texture : public CoreObject
    {
     public:
      Texture(const TEXTURE_DESC& desc,
              const SPtr<PixelData>& initData,
              GPU_DEVICE_FLAGS::E deviceMask);
      virtual ~Texture() = default;

      /**
       * @copydoc CoreObject::initialize
       */
      void
      initialize() override;

      /**
       * @brief Locks the buffer for reading or writing.
       * @param[in] options   Options for controlling what you may do with the
       *                      locked data.
       * @param[in] mipLevel  (optional) Mipmap level to lock.
       * @param[in] face      (optional) Texture face to lock.
       * @param[in] deviceIdx Index of the device whose memory to map. If the
       *                      buffer doesn't exist on this device, the method
       *                      returns null.
       * @param[in] queueIdx  Device queue to perform the read/write operations
       *                      on. See @ref queuesDoc.
       * @note  If you are just reading or writing one block of data use
       *        readData()/writeData() methods as they can be much faster in
       *        certain situations.
       */
      PixelData
      lock(GPU_LOCK_OPTIONS::E options,
           uint32 mipLevel = 0,
           uint32 face = 0,
           uint32 deviceIdx = 0,
           uint32 queueIdx = 0);

      /**
       * @brief Unlocks a previously locked buffer. After the buffer is
       *        unlocked, any data returned by lock becomes invalid.
       * @see lock()
       */
      void
      unlock();

      /**
       * @brief Copies the contents a subresource in this texture to another
       *        texture. Texture format and size of the subresource must match.
       *        You are allowed to copy from a multisampled to non-multisampled
       *        surface, which will resolve the multisampled surface before
       *        copying.
       * @param[in] target  Texture that contains the destination subresource.
       * @param[in] desc    Structure used for customizing the copy operation.
       * @param[in] commandBuffer   Command buffer to queue the copy operation
       *                            on. If null, main command buffer is used.
       */
      void
      copy(const SPtr<Texture>& target,
           const TEXTURE_COPY_DESC& desc = TEXTURE_COPY_DESC::DEFAULT,
           const SPtr<CommandBuffer>& commandBuffer = nullptr);

      /**
       * @brief Sets all the pixels of the specified face and mip level to the
       *        provided value.
       * @param[in] value     Color to clear the pixels to.
       * @param[in] mipLevel  Mip level to clear.
       * @param[in] face      Face (array index or cubemap face) to clear.
       * @param[in] queueIdx  Device queue to perform the write operation on.
       *            See @ref queuesDoc.
       */
      void
      clear(const LinearColor& value,
            uint32 mipLevel = 0,
            uint32 face = 0,
            uint32 queueIdx = 0);

      /**
       * @brief Reads data from the texture buffer into the provided buffer.
       * @param[out]  dest    Previously allocated buffer to read data into.
       * @param[in] mipLevel  (optional) Mipmap level to read from.
       * @param[in] face      (optional) Texture face to read from.
       * @param[in] deviceIdx Index of the device whose memory to read. If the
       *            buffer doesn't exist on this device, no data will be read.
       * @param[in] queueIdx  Device queue to perform the read operation on.
       *            See @ref queuesDoc.
       */
      void
      readData(PixelData& dest,
               uint32 mipLevel = 0,
               uint32 face = 0,
               uint32 deviceIdx = 0,
               uint32 queueIdx = 0);

      /**
       * @brief Writes data from the provided buffer into the texture buffer.
       * @param[in] src       Buffer to retrieve the data from.
       * @param[in] mipLevel  (optional) Mipmap level to write into.
       * @param[in] face      (optional) Texture face to write into.
       * @param[in] discardWholeBuffer  (optional) If true any existing texture
       *            data will be discard. This can improve performance of the
       *            write operation.
       * @param[in] queueIdx  Device queue to perform the write operation on.
       *            See @ref queuesDoc.
       */
      void
      writeData(const PixelData& src,
                uint32 mipLevel = 0,
                uint32 face = 0,
                bool discardWholeBuffer = false,
                uint32 queueIdx = 0);

      /**
       * @brief Returns properties that contain information about the texture.
       */
      const TextureProperties&
      getProperties() const {
        return m_properties;
      }

      /***********************************************************************/
      /**
       * Statics
       */
      /***********************************************************************/

      /**
       * @copydoc geEngineSDK::Texture::create(const TEXTURE_DESC&)
       * @param[in] deviceMask  Mask that determines on which GPU devices
       *            should the object be created on.
       */
      static SPtr<Texture>
      create(const TEXTURE_DESC& desc,
             GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT);

      /**
       * @copydoc
       *    geEngineSDK::Texture::create(const SPtr<PixelData>&,int32,bool)
       * @param[in] deviceMask  Mask that determines on which GPU devices
       *            should the object be created on.
       */
      static SPtr<Texture>
      create(const SPtr<PixelData>& pixelData,
             int32 usage = TU_DEFAULT,
             bool hwGammaCorrection = false,
             GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT);

      /***********************************************************************/
      /**
       * Texture View
       */
      /***********************************************************************/

      /**
       * @brief Requests a texture view for the specified mip and array ranges.
       *        Returns an existing view of one for the specified ranges
       *        already exists, otherwise creates a new one. You must release
       *        all views by calling releaseView() when done.
       * @note  Core thread only.
       */
      SPtr<TextureView>
      requestView(uint32 mostDetailMip,
                  uint32 numMips,
                  uint32 firstArraySlice,
                  uint32 numArraySlices,
                  GPU_VIEW_USAGE::E usage);

      /**
       * @brief Returns a plain white texture.
       */
      static SPtr<Texture> WHITE;

      /**
       * @brief Returns a plain black texture.
       */
      static SPtr<Texture> BLACK;

      /**
       * @brief Returns a plain normal map texture with normal pointing up
       *        (in Y direction).
       */
      static SPtr<Texture> NORMAL;

     protected:
      /**
       * @copydoc lock
       */
      virtual PixelData
      lockImpl(GPU_LOCK_OPTIONS::E options,
               uint32 mipLevel = 0,
               uint32 face = 0,
               uint32 deviceIdx = 0,
               uint32 queueIdx = 0) = 0;

      /**
       * @copydoc unlock
       */
      virtual void
      unlockImpl() = 0;

      /**
       * @copydoc copy
       */
      virtual void
      copyImpl(const SPtr<Texture>& target,
               const TEXTURE_COPY_DESC& desc,
               const SPtr<CommandBuffer>& commandBuffer) = 0;

      /**
       * @copydoc readData
       */
      virtual void
      readDataImpl(PixelData& dest,
                   uint32 mipLevel = 0,
                   uint32 face = 0,
                   uint32 deviceIdx = 0,
                   uint32 queueIdx = 0) = 0;

      /**
       * @copydoc writeData
       */
      virtual void
      writeDataImpl(const PixelData& src,
                    uint32 mipLevel = 0,
                    uint32 face = 0,
                    bool discardWholeBuffer = false,
                    uint32 queueIdx = 0) = 0;

      /**
       * @copydoc clear
       */
      virtual void
      clearImpl(const LinearColor& value,
                uint32 mipLevel = 0,
                uint32 face = 0,
                uint32 queueIdx = 0);

      /***********************************************************************/
      /**
       * Texture View
       */
      /***********************************************************************/

      /**
       * @brief Creates a view of a specific subresource in a texture.
       */
      virtual SPtr<TextureView>
      createView(const TEXTURE_VIEW_DESC& desc);

      /**
       * @brief Releases all internal texture view references.
       */
      void
      clearBufferViews();

      UnorderedMap<TEXTURE_VIEW_DESC,
                   SPtr<TextureView>,
                   TextureView::HashFunction,
                   TextureView::EqualFunction> m_textureViews;
      TextureProperties m_properties;
      SPtr<PixelData> m_initData;
    };
  }
}
