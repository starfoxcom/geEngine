/*****************************************************************************/
/**
 * @file    geCommonTypes.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/25
 * @brief   Common definitions for core objects.
 *
 * Common definitions for core objects.
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

namespace geEngineSDK {
  //Undefine defines from other libs, that conflict with enums below
# undef None
# undef Convex

  /**
   * @brief Factors used when blending new pixels with existing pixels.
   */
  enum BlendFactor {
    BF_ONE,               //Use a value of one for all pixel components.
    BF_ZERO,              //Use a value of zero for all pixel components.
    BF_DEST_COLOR,        //Use the existing pixel value.
    BF_SOURCE_COLOR,      //Use the newly generated pixel value.
    BF_INV_DEST_COLOR,    //Use the inverse of the existing value.
    BF_INV_SOURCE_COLOR,  //Use the inverse of the newly generated pixel value.
    BF_DEST_ALPHA,        //Use the existing alpha value.
    BF_SOURCE_ALPHA,      //Use the newly generated alpha value.
    BF_INV_DEST_ALPHA,    //Use the inverse of the existing alpha value.
    BF_INV_SOURCE_ALPHA   //Use the inverse of the newly generated alpha value.
  };

  /**
   * @brief Operations that determines how are blending factors combined.
   */
  enum BlendOperation {
    //Blend factors are added together.
    BO_ADD,
    //Blend factors are subtracted in "srcFactor - dstFactor" order.
    BO_SUBTRACT,
    //Blend factors are subtracted in "dstFactor - srcFactor" order.
    BO_REVERSE_SUBTRACT,
    //Minimum of the two factors is chosen.
    BO_MIN,
    //Maximum of the two factors is chosen.0
    BO_MAX
  };

  /**
   * @brief Comparison functions used for the depth/stencil buffer.
   */
  enum CompareFunction {
    //Operation will always fail.
    CMPF_ALWAYS_FAIL,
    //Operation will always pass.
    CMPF_ALWAYS_PASS,
    //Operation will pass if the new value is less than existing value.
    CMPF_LESS,
    //Operation will pass if the new value is less or equal than existing value
    CMPF_LESS_EQUAL,    
    //Operation will pass if the new value is equal to the existing value.
    CMPF_EQUAL,
    //Operation will pass if the new value is not equal to the existing value.
    CMPF_NOT_EQUAL,
    //Operation will pass if the new value greater or equal than the existing
    //value.
    CMPF_GREATER_EQUAL,
    //Operation will pass if the new value greater than the existing value.
    CMPF_GREATER
  };

  /**
   * @brief Types of texture addressing modes that determine what happens when
   *        texture coordinates are outside of the valid range.
   */
  enum TextureAddressingMode {
    //Coordinates wrap back to the valid range.
    TAM_WRAP,
    //Coordinates flip every time the size of the valid range is passed.
    TAM_MIRROR,
    //Coordinates are clamped within the valid range.
    TAM_CLAMP,
    //Coordinates outside of the valid range will return a separately set
    //border color.
    TAM_BORDER
  };

  /**
   * @brief Types of available filtering situations.
   */
  enum FilterType {
    FT_MIN, //The filter used when shrinking a texture.
    FT_MAG, //The filter used when magnifying a texture.
    FT_MIP  //The filter used when filtering between mipmaps.
  };

  /**
   * @brief Filtering options for textures.
   */
  enum FilterOptions {
    //Use no filtering. Only relevant for mipmap filtering.
    FO_NONE = 0,
    //Filter using the nearest found pixel. Most basic filtering.
    FO_POINT = 1,
    //Average a 2x2 pixel area, bilinear filtering for texture, trilinear for
    //mipmaps.
    FO_LINEAR = 2,
    //More advanced filtering that improves quality when viewing textures at a
    //steep angle
    FO_ANISOTROPIC = 3,
  };

  /**
   * @brief Types of frame buffers.
   */
  enum FrameBufferType {
    FBT_COLOR = 0x1,    //Clear the color surface.
    FBT_DEPTH = 0x2,    //Clear the depth surface.
    FBT_STENCIL = 0x4   //Clear the stencil surface.
  };

  /**
   * @brief Types of culling that determine how (and if) hardware discards
   *        faces with certain winding order. Winding order can be used for
   *        determining front or back facing polygons by checking the order of
   *        its vertices from the render perspective.
   */
  enum CullingMode {
    //Hardware performs no culling and renders both sides.
    CULL_NONE = 0,
    //Hardware culls faces that have a clockwise vertex ordering.
    CULL_CLOCKWISE = 1,
    //Hardware culls faces that have a counter-clockwise vertex ordering.
    CULL_COUNTERCLOCKWISE = 2
  };

  /**
   * @brief Polygon mode to use when rasterizing.
   */
  enum PolygonMode {
    PM_WIREFRAME = 1, //Render as wireframe showing only polygon outlines.
    PM_SOLID = 2      //Render as solid showing whole polygons.
  };

  /**
   * @brief Types of action that can happen on the stencil buffer.
   */
  enum StencilOperation {
    //Leave the stencil buffer unchanged.
    SOP_KEEP,
    //Set the stencil value to zero.
    SOP_ZERO,
    //Replace the stencil value with the reference value.
    SOP_REPLACE,
    //Increase the stencil value by 1, clamping at the maximum value.
    SOP_INCREMENT,
    //Decrease the stencil value by 1, clamping at 0.
    SOP_DECREMENT,
    //Increase the stencil value by 1, wrapping back to 0 when incrementing
    //past the maximum value.
    SOP_INCREMENT_WRAP,
    //Decrease the stencil value by 1, wrapping when decrementing 0.
    SOP_DECREMENT_WRAP,
    //Invert the bits of the stencil buffer.
    SOP_INVERT
  };

  /**
   * @brief Describes operation that will be used for rendering a certain set
   *        of vertices.
   */
  enum GE_SCRIPT_EXPORT(n:MeshTopology, m:Rendering) DrawOperationType {
    //Each vertex represents a point.
    DOT_POINT_LIST      GE_SCRIPT_EXPORT(n:PointList) = 1,
    //Each sequential pair of vertices represent a line.
    DOT_LINE_LIST       GE_SCRIPT_EXPORT(n:LineList) = 2,
    //Each vertex (except the first) forms a line with the previous vertex.
    DOT_LINE_STRIP      GE_SCRIPT_EXPORT(n:LineStrip) = 3,
    //Each sequential 3-tuple of vertices represent a triangle.
    DOT_TRIANGLE_LIST	  GE_SCRIPT_EXPORT(n:TriangleList) = 4,
    //Each vertex (except the first two) form a triangle with the previous two
    //vertices.
    DOT_TRIANGLE_STRIP  GE_SCRIPT_EXPORT(n:TriangleStrip) = 5,
    //Each vertex (except the first two) form a triangle with the first vertex
    //and previous vertex.
    DOT_TRIANGLE_FAN    GE_SCRIPT_EXPORT(n:TriangleFan) = 6
  };

  /**
   * @brief Type of mesh indices used, used for determining maximum number of
   *        vertices in a mesh.
   */
  enum GE_SCRIPT_EXPORT(m:Rendering) IndexType {
    IT_16BIT  GE_SCRIPT_EXPORT(n:Index16),  //16-bit indices.
    IT_32BIT  GE_SCRIPT_EXPORT(n:Index32) //32-bit indices.
  };

  /**
   * @brief These values represent a hint to the driver when locking a hardware
   *        buffer.
   */
  enum GpuLockOptions {
    /**
     * Allows you to write to the buffer. Can cause a CPU-GPU sync point so
     * avoid using it often (every frame) as that might limit your performance
     * significantly.
     */
    GBL_READ_WRITE,
    /**
     * Allows you to write to the buffer. Tells the driver to completely
     * discard the contents of the buffer you are writing to. The driver will
     * (most likely) internally allocate another buffer with same
     * specifications (which is fairly fast) and you will avoid CPU-GPU stalls.
     */
    GBL_WRITE_ONLY_DISCARD,
    /**
     * Allows you to write to the buffer. Tells the driver to discard the
     * contents of the mapped buffer range (but not the entire buffer like
     * with GBL_WRITE_ONLY_DISCARD). Use this if you plan on overwriting all
     * of the range. This can help avoid CPU-GPU stalls.
     */
    GBL_WRITE_ONLY_DISCARD_RANGE,
    /** 
     * Allows you to read from a buffer. Be aware that reading is usually a
     * very slow operation.
     */
    GBL_READ_ONLY,
    /**
     * Allows you to write to the buffer. Guarantees the driver that you will
     * not be updating any part of the buffer that is currently used. This will
     * also avoid CPU-GPU stalls, without requiring you to discard the entire
     * buffer. However it is hard to guarantee when GPU has finished using a
     * buffer.
     */
    GBL_WRITE_ONLY_NO_OVERWRITE,
    /**
     * Allows you to write to a buffer.
     */
    GBL_WRITE_ONLY
  };

  /**
   * @brief Types of programs that may run on GPU.
   */
  enum GpuProgramType {
    GPT_VERTEX_PROGRAM,   //Vertex program.
    GPT_FRAGMENT_PROGRAM, //Fragment (pixel) program.
    GPT_GEOMETRY_PROGRAM, //Geometry program.
    GPT_DOMAIN_PROGRAM,   //Domain (tesselation evaluation) program.
    GPT_HULL_PROGRAM,     //Hull (tesselation control) program.
    GPT_COMPUTE_PROGRAM,  //Compute program.
    GPT_COUNT             //Keep at end
  };

  /**
   * @brief Values that represent hardware buffer usage. These usually
   *        determine in what type of memory is buffer placed in, however that
   *        depends on rendering API.
   */
  enum GpuBufferUsage {
    /**
     * Signifies that you don't plan on modifying the buffer often (or at all)
     * after creation. Modifying such buffer will involve a larger performance
     * hit. Mutually exclusive with GBU_DYNAMIC.
     */
    GBU_STATIC = 0x01,
    /**
     * Signifies that you will modify this buffer fairly often
     * (e.g. every frame). Mutually exclusive with GBU_STATIC.
     */
    GBU_DYNAMIC = 0x02,
  };

  /**
   * @brief Types of generic GPU buffers that may be attached to GPU programs.
   */
  enum GpuBufferType {
    /**
     * Buffer containing an array of primitives (e.g. float4's).
     */
    GBT_STANDARD,
    /**
     * Buffer containing an array of structures. Structure parameters can
     * usually be easily accessed from within the GPU program.
     */
    GBT_STRUCTURED,
    /**
     * Special type of buffer allowing you to specify arguments for draw
     * operations inside the buffer instead of providing them directly. Useful
     * when you want to control drawing directly from GPU.
     */
    GBT_INDIRECTARGUMENT
  };

  /**
   * @brief Types of valid formats used for standard GPU buffers.
   */
  enum GpuBufferFormat {
    BF_16X1F,           //1D 16-bit floating-point format.
    BF_16X2F,           //2D 16-bit floating-point format.
    BF_16X4F,           //4D 16-bit floating-point format.
    BF_32X1F,           //1D 32-bit floating-point format.
    BF_32X2F,           //2D 32-bit floating-point format.
    BF_32X3F,           //3D 32-bit floating-point format.
    BF_32X4F,           //4D 32-bit floating-point format.
    BF_8X1,             //1D 8-bit normalized format.
    BF_8X2,             //2D 8-bit normalized format.
    BF_8X4,             //4D 8-bit normalized format.
    BF_16X1,            //1D 16-bit normalized format.
    BF_16X2,            //2D 16-bit normalized format.
    BF_16X4,            //4D 16-bit normalized format.
    BF_8X1S,            //1D 8-bit signed integer format.
    BF_8X2S,            //2D 8-bit signed integer format.
    BF_8X4S,            //4D 8-bit signed integer format.
    BF_16X1S,           //1D 16-bit signed integer format.
    BF_16X2S,           //2D 16-bit signed integer format.
    BF_16X4S,           //4D 16-bit signed integer format.
    BF_32X1S,           //1D 32-bit signed integer format.
    BF_32X2S,           //2D 32-bit signed integer format.
    BF_32X3S,           //3D 32-bit signed integer format.
    BF_32X4S,           //4D 32-bit signed integer format.
    BF_8X1U,            //1D 8-bit unsigned integer format.
    BF_8X2U,            //2D 8-bit unsigned integer format.
    BF_8X4U,            //4D 8-bit unsigned integer format.
    BF_16X1U,           //1D 16-bit unsigned integer format.
    BF_16X2U,           //2D 16-bit unsigned integer format.
    BF_16X4U,           //4D 16-bit unsigned integer format.
    BF_32X1U,           //1D 32-bit unsigned integer format.
    BF_32X2U,           //2D 32-bit unsigned integer format.
    BF_32X3U,           //3D 32-bit unsigned integer format.
    BF_32X4U,           //4D 32-bit unsigned integer format.
    BF_COUNT,           //Not a valid format. Keep just before BF_UNKNOWN.
    //Unknown format (used for non-standard buffers, like structured or raw.
    BF_UNKNOWN = 0xffff
  };

  /**
   * @brief Different types of GPU views that control how GPU sees a hardware
   *        buffer.
   */
  enum GpuViewUsage {
    /**
     * Buffer is seen as a default shader resource, used primarily for reading.
     * (for example a texture for sampling)
     */
    GVU_DEFAULT = 0x01,
    /**
     * Buffer is seen as a render target that color pixels will be written to
     * after pixel shader stage.
     */
    GVU_RENDERTARGET = 0x02,
    /**
     * Buffer is seen as a depth stencil target that depth and stencil
     * information is written to.
     */
    GVU_DEPTHSTENCIL = 0x04,
    /**
     * Buffer that allows you to write to any part of it from within a GPU
     * program.
     */
    GVU_RANDOMWRITE = 0x08
  };

  /**
   * @brief Combinable set of bits that describe a set of physical GPU's.
   */
  enum GpuDeviceFlags {
    /**
     * Use the default set of devices. This may be the primary device or
     * multiple devices. Cannot be used together with other device flags.
     */
    GDF_DEFAULT = 0,
    /**
     * Use only the primary GPU.
     */
    GDF_PRIMARY = 0x01,
    /**
     * Use the second GPU.
     */
    GDF_GPU2 = 0x02,
    /**
     * Use the third GPU.
     */
    GDF_GPU3 = 0x04,
    /**
     * Use the fourth GPU.
     */
    GDF_GPU4 = 0x08,
    /**
     * Use the fifth GPU.
     */
    GDF_GPU5 = 0x10
  };

  /**
   * @brief Type of parameter block usages. Signifies how often will parameter
   *        blocks be changed.
   */
  enum GpuParamBlockUsage {
    GPBU_STATIC,  //Buffer will be rarely, if ever, updated.
    GPBU_DYNAMIC  //Buffer will be updated often (for example every frame).
  };

  /**
   * @brief Type of a parameter in a GPU program.
   */
  enum GpuParamType {
    GPT_DATA,     //Raw data type like float, Vector3, Color, etc.
    GPT_TEXTURE,  //Texture type (2D, 3D, cube, etc.)
    GPT_BUFFER,   //Data buffer (raw, structured, etc.)
    GPT_SAMPLER   //Sampler type (2D, 3D, cube, etc.)
  };

  /**
   * @brief Type of GPU data parameters that can be used as inputs to a GPU
   *        program.
   */
  enum GpuParamDataType {
    GPDT_FLOAT1 = 1,      //1D floating point value.
    GPDT_FLOAT2 = 2,      //2D floating point value.
    GPDT_FLOAT3 = 3,      //3D floating point value.
    GPDT_FLOAT4 = 4,      //4D floating point value.
    GPDT_MATRIX_2X2 = 11, //2x2 matrix.
    GPDT_MATRIX_2X3 = 12, //2x3 matrix.
    GPDT_MATRIX_2X4 = 13, //2x4 matrix.
    GPDT_MATRIX_3X2 = 14, //3x2 matrix.
    GPDT_MATRIX_3X3 = 15, //3x3 matrix.
    GPDT_MATRIX_3X4 = 16, //3x4 matrix.
    GPDT_MATRIX_4X2 = 17, //4x2 matrix.
    GPDT_MATRIX_4X3 = 18, //4x3 matrix.
    GPDT_MATRIX_4X4 = 19, //4x4 matrix.
    GPDT_INT1 = 20,       //1D signed integer value.
    GPDT_INT2 = 21,       //2D signed integer value.
    GPDT_INT3 = 22,       //3D signed integer value.
    GPDT_INT4 = 23,       //4D signed integer value.
    GPDT_BOOL = 24,       //Boolean value.
    GPDT_STRUCT = 25,     //Variable size structure.
    GPDT_COLOR = 26,      //Same as GPDT_FLOAT4, but easier to deduct usage.
    GPDT_COUNT = 27,      //Keep at end before GPDT_UNKNOWN
    GPDT_UNKNOWN = 0xffff
  };

  /**
   * @brief Available texture types.
   */
  enum GE_SCRIPT_EXPORT(m:Rendering) TextureType {
    //One dimensional texture. Just a row of pixels.
    TEX_TYPE_1D         GE_SCRIPT_EXPORT(n:Texture1D) = 1,
    //Two dimensional texture.
    TEX_TYPE_2D         GE_SCRIPT_EXPORT(n:Texture2D) = 2,
    //Three dimensional texture.
    TEX_TYPE_3D         GE_SCRIPT_EXPORT(n:Texture3D) = 3,
    //Texture consisting out of six 2D textures describing an inside of a cube.
    //Allows special sampling.
    TEX_TYPE_CUBE_MAP   GE_SCRIPT_EXPORT(n:TextureCube) = 4
  };

  /**
   * @brief Projection type to use by the camera.
   */
  enum GE_SCRIPT_EXPORT() ProjectionType {
    /**
     * Projection type where object size remains constant and parallel lines
     * remain parallel.
     */
    PT_ORTHOGRAPHIC   GE_SCRIPT_EXPORT(n:Orthographic),
    /**
     * Projection type that emulates human vision. Objects farther away appear
     * smaller.
     */
    PT_PERSPECTIVE    GE_SCRIPT_EXPORT(n:Perspective)
  };

  /**
   * @brief Contains data about a type used for GPU data parameters.
   */
  struct GpuParamDataTypeInfo
  {
    uint32 baseTypeSize;
    uint32 size;
    uint32 alignment;
    uint32 numRows;
    uint32 numColumns;
  };

  /**
   * @brief Contains a lookup table for various information of all types used
   *        for data GPU parameters. Sizes are in bytes.
   */
  struct GpuDataParamInfos
  {
    GpuDataParamInfos() {
      memset(lookup, 0, sizeof(lookup));
      lookup[static_cast<uint32>(GPDT_FLOAT1)] = { 4, 4, 4, 1, 1 };
      lookup[static_cast<uint32>(GPDT_FLOAT2)] = { 4, 8, 8, 1, 2 };
      lookup[static_cast<uint32>(GPDT_FLOAT3)] = { 4, 16, 16, 1, 3 };
      lookup[static_cast<uint32>(GPDT_FLOAT4)] = { 4, 16, 16, 1, 4 };
      lookup[static_cast<uint32>(GPDT_COLOR)] = { 4, 16, 16, 1, 4 };
      lookup[static_cast<uint32>(GPDT_MATRIX_2X2)] = { 4, 16, 8, 2, 2 };
      lookup[static_cast<uint32>(GPDT_MATRIX_2X3)] = { 4, 32, 16, 2, 3 };
      lookup[static_cast<uint32>(GPDT_MATRIX_2X4)] = { 4, 32, 16, 2, 4 };
      lookup[static_cast<uint32>(GPDT_MATRIX_3X2)] = { 4, 24, 8, 3, 2 };
      lookup[static_cast<uint32>(GPDT_MATRIX_3X3)] = { 4, 48, 16, 3, 3 };
      lookup[static_cast<uint32>(GPDT_MATRIX_3X4)] = { 4, 48, 16, 3, 4 };
      lookup[static_cast<uint32>(GPDT_MATRIX_4X2)] = { 4, 32, 8, 4, 2 };
      lookup[static_cast<uint32>(GPDT_MATRIX_4X3)] = { 4, 64, 16, 4, 3 };
      lookup[static_cast<uint32>(GPDT_MATRIX_4X4)] = { 4, 64, 16, 4, 4 };
      lookup[static_cast<uint32>(GPDT_INT1)] = { 4, 4, 4, 1, 1 };
      lookup[static_cast<uint32>(GPDT_INT2)] = { 4, 8, 8, 1, 2 };
      lookup[static_cast<uint32>(GPDT_INT3)] = { 4, 12, 16, 1, 3 };
      lookup[static_cast<uint32>(GPDT_INT4)] = { 4, 16, 16, 1, 4 };
      lookup[static_cast<uint32>(GPDT_BOOL)] = { 4, 4, 4, 1, 1 };
    }

    GpuParamDataTypeInfo lookup[GPDT_COUNT];
  };

  /**
   * @brief Type of GPU object parameters that can be used as inputs to a GPU
   *        program.
   */
  enum GpuParamObjectType {
    //Sampler state for a 1D texture.
    GPOT_SAMPLER1D                        = 1,
    //Sampler state for a 2D texture.
    GPOT_SAMPLER2D                        = 2,
    //Sampler state for a 3D texture.
    GPOT_SAMPLER3D                        = 3,
    //Sampler state for a cube texture.
    GPOT_SAMPLERCUBE                      = 4,
    //Sampler state for a 2D texture with multiple samples.
    GPOT_SAMPLER2DMS                      = 5,
    //1D texture.
    GPOT_TEXTURE1D                        = 11,
    //2D texture.
    GPOT_TEXTURE2D                        = 12,
    //3D texture.
    GPOT_TEXTURE3D                        = 13,
    //Cube texture.
    GPOT_TEXTURECUBE                      = 14,
    //2D texture with multiple samples.
    GPOT_TEXTURE2DMS                      = 15,
    //Buffer containing raw bytes (no interpretation).
    GPOT_BYTE_BUFFER                      = 32,
    //Buffer containing a set of structures.
    GPOT_STRUCTURED_BUFFER                = 33,
    //Read-write buffer containing a set of primitives.
    GPOT_RWTYPED_BUFFER                   = 41,
    //Read-write buffer containing raw bytes (no interpretation).
    GPOT_RWBYTE_BUFFER                    = 42,
    //Read-write buffer containing a set of structures.
    GPOT_RWSTRUCTURED_BUFFER              = 43,
    //Read-write buffer containing a set of structures, with a counter.
    GPOT_RWSTRUCTURED_BUFFER_WITH_COUNTER = 44,
    //Buffer that can be used for appending data in a stack-like fashion.
    GPOT_RWAPPEND_BUFFER                  = 45,
    //Buffer that can be used for consuming data in a stack-like fashion.
    GPOT_RWCONSUME_BUFFER                 = 46,
    //1D texture with unordered read/writes.
    GPOT_RWTEXTURE1D                      = 50,
    //2D texture with unordered read/writes.
    GPOT_RWTEXTURE2D                      = 51,
    //3D texture with unordered read/writes.
    GPOT_RWTEXTURE3D                      = 52,
    //2D texture with multiple samples and unordered read/writes.
    GPOT_RWTEXTURE2DMS                    = 53,
    //1D texture with multiple array entries.
    GPOT_TEXTURE1DARRAY                   = 54,
    //2D texture with multiple array entries.
    GPOT_TEXTURE2DARRAY                   = 55,
    //Cubemap texture with multiple array entries.
    GPOT_TEXTURECUBEARRAY                 = 56,
    //2D texture with multiple samples and array entries.
    GPOT_TEXTURE2DMSARRAY                 = 57,
    //1D texture with multiple array entries and unordered read/writes.
    GPOT_RWTEXTURE1DARRAY                 = 58,
    //2D texture with multiple array entries and unordered read/writes.
    GPOT_RWTEXTURE2DARRAY                 = 59,
    //2D texture with multiple array entries, samples and unordered read/writes.
    GPOT_RWTEXTURE2DMSARRAY               = 60,
    GPOT_UNKNOWN                          = 0xffff
  };

  /**
   * @brief Types of GPU queues.
   */
  enum GpuQueueType {
    /**
     * Queue used for rendering. Allows the use of draw commands, but also all
     * commands supported by compute or upload buffers.
     */
    GQT_GRAPHICS,
    /**
     * Discrete queue used for compute operations. Allows the use of dispatch
     * and upload commands.
     */
    GQT_COMPUTE,
    /**
     * Queue used for memory transfer operations only. No rendering or compute
     * dispatch allowed.
     */
    GQT_UPLOAD,
    GQT_COUNT //Keep at end
  };

  /**
   * @brief These values represent a hint to the driver when writing to a GPU
   *        buffer.
   */
  enum BufferWriteType {
    /**
     * Default flag with least restrictions. Can cause a CPU-GPU sync point so
     * avoid using it often (every frame) as that might limit your performance
     * significantly.
     */
    BWT_NORMAL,
    /**
     * Tells the driver to completely discard the contents of the buffer you
     * are writing to. The driver will (most likely) internally allocate
     * another buffer with same specifications (which is fairly fast) and you
     * will avoid CPU-GPU stalls.
     */
    BWT_DISCARD,
    /**
     * Guarantees the driver that you will not be updating any part of the
     * buffer that is currently used. This will also avoid CPU-GPU stalls,
     * without requiring you to discard the entire buffer. However it is hard
     * to guarantee when GPU has finished using a buffer.
     */
    BTW_NO_OVERWRITE
  };

  /**
   * @brief Suggested queue priority numbers used for sorting objects in the
   * render queue. Objects with higher priority will be renderer sooner.
   */
  namespace QUEUE_PRIORITY {
    enum E {
      kOpaque = 100000,
      kTransparent = 90000,
      kSkybox = 80000,
      kOverlay = 70000
    };
  }

  /**
   * @brief Type of sorting to perform on an object when added to a render
   *        queue.
   */
  namespace QUEUE_SORT_TYPE {
    enum E {
      /**
       * All objects with the same priority will be rendered front to back based
       * on their center.
       */
      kFrontToBack,
      /**
       * All objects with the same priority will be rendered back to front based
       * on their center.
       */
      kBackToFront,
      /**
       * Objects will not be sorted and will be processed in the order they were
       * added to the queue.
       */
      kNone
    };
  }

  /**
   * @brief Flags that may be assigned to a shader that let the renderer know
   *        how to interpret the shader.
   */
  namespace SHADER_FLAGS {
    enum E {
      /**
       * Signifies that the shader is rendering a transparent object.
       */
      kTransparent = 0x1,

      /**
       * Signifies the shader should use the forward rendering pipeline,
       * if relevant.
       */
      kForward = 0x2
    };
  }

  /**
   * @brief Valid types of a mesh used for physics.
   */
  enum class GE_SCRIPT_EXPORT() PhysicsMeshType {
    /**
     * A regular triangle mesh. Mesh can be of arbitrary size but cannot be
     * used for triggers and non-kinematic objects. Incurs a significantly
     * larger performance impact than convex meshes.
     */
    Triangle,
    
    /**
     * Mesh representing a convex shape. Mesh will not have more than 256
     * vertices. Incurs a significantly lower performance impact than triangle
     * meshes.
     */
    Convex
  };

  /**
   * @brief Determines the type of the source image for generating cubemaps.
   */
  namespace CUBEMAP_SOURCE_TYPE {
    enum E {
      /**
       * Source is a single image that will be replicated on all cubemap faces.
       */
      kSingle,

      /**
       * Source is a list of 6 images, either sequentially next to each other or
       * in a cross format. The system will automatically guess the layout and
       * orientation based on the aspect ratio.
       */
      kFaces,

      /**
       * Source is a single spherical panoramic image.
       */
      kSpherical,

      /**
       * Source is a single cylindrical panoramic image.
       */
      kCylindrical
    };
  }

  /**
   * @brief Bits that map to a specific surface of a render target. Combine the
   *        bits to generate a mask that references only specific render target
   *        surfaces.
   */
  namespace RENDER_SURFACE_MASK_BITS {
    enum E {
      kNONE = 0,
      kCOLOR0 = 1 << 0,
      kCOLOR1 = 1 << 1,
      kCOLOR2 = 1 << 2,
      kCOLOR3 = 1 << 3,
      kCOLOR4 = 1 << 4,
      kCOLOR5 = 1 << 5,
      kCOLOR6 = 1 << 6,
      kCOLOR7 = 1 << 7,
      kDEPTH = 1 << 30,
      kSTENCIL = 1 << 31,
      kDEPTH_STENCIL = (1 << 30) | (1 << 31),
      kALL = 0xFF
    };
  }

  typedef Flags<RENDER_SURFACE_MASK_BITS::E> RenderSurfaceMask;
  GE_FLAGS_OPERATORS(RENDER_SURFACE_MASK_BITS::E);

  /**
   * @brief Controls what kind of mobility restrictions a scene object has.
   *        This is used primarily as a performance hint to other systems.
   *        Generally the more restricted the mobility the higher performance
   *        can be achieved.
   */
  enum class GE_SCRIPT_EXPORT() ObjectMobility {
    /**
     * Scene object can be moved and has no mobility restrictions.
     */
    Movable,
    
    /**
     * Scene object isn't allowed to be moved but is allowed to be visually
     * changed in other ways (e.g. changing the displayed mesh or light
     * intensity (depends on attached components).
     */
    Immovable,
    
    /**
     * Scene object isn't allowed to be moved nor is it allowed to be visually
     * changed. Object must be fully static.
     */
    Static
  };

  /**
   * @brief Texture addressing mode, per component.
   */
  struct UVWAddressingMode {
    bool
    operator==(const UVWAddressingMode& rhs) const {
      return u == rhs.u && v == rhs.v && w == rhs.w;
    }

    TextureAddressingMode u{ TAM_WRAP };
    TextureAddressingMode v{ TAM_WRAP };
    TextureAddressingMode w{ TAM_WRAP };
  };

  /**
   * @brief References a subset of surfaces within a texture.
   */
  struct GE_SCRIPT_EXPORT(m:Rendering, pl:true) TextureSurface
  {
    TextureSurface(uint32 _mipLevel = 0,
                   uint32 _numMipLevels = 1,
                   uint32 _face = 0,
                   uint32 _numFaces = 1)
      : mipLevel(_mipLevel),
        numMipLevels(_numMipLevels),
        face(_face),
        numFaces(_numFaces)
    {}

    /**
     * @brief First mip level to reference.
     */
    uint32 mipLevel;

    /**
     * @brief Number of mip levels to reference. Must be greater than zero.
     */
    uint32 numMipLevels;

    /**
     * @brief First face to reference. Face can represent a single cubemap
     *        face, or a single array entry in a texture array. If cubemaps are
     *        laid out in a texture array then every six sequential faces
     *        represent a single array entry.
     */
    uint32 face;

    /**
     * @brief Number of faces to reference, if the texture has more than one.
     */
    uint32 numFaces;

    /**
     * @brief Surface that covers all texture sub-resources.
     */
    static GE_CORE_EXPORT const TextureSurface COMPLETE;
  };

  /**
   * @brief Meta-data describing a chunk of audio.
   */
  struct AudioDataInfo
  {
    /**
     * Total number of audio samples in the audio data (includes all channels).
     */
    uint32 numSamples;
    /**
     * Number of audio samples per second, per channel.
     */
    uint32 sampleRate;
    /**
     * Number of channels. Each channel has its own set of samples.
     */
    uint32 numChannels;
    /**
     * Number of bits per sample.
     */
    uint32 bitDepth;
  };

  /**
   * @brief Helper class for syncing dirty data from sim CoreObject to core
   *        CoreObject and other way around.
   */
  class CoreSyncData
  {
   public:
    CoreSyncData() : m_data(nullptr), m_size(0) {}
    CoreSyncData(uint8* data, uint32 size) : m_data(data), m_size(size) {}

    /**
    * @brief Gets the internal data and checks the data is of valid size. */
    template<class T>
    const T&
    getData() const {
      GE_ASSERT(sizeof(T) == m_size);
      return *reinterpret_cast<T*>(m_data);
    }

    /**
     * @brief Returns a pointer to internal data buffer.
     */
    uint8*
    getBuffer() const {
      return m_data;
    }

    /**
     * @brief Returns the size of the internal data buffer.
     */
    uint32
    getBufferSize() const {
      return m_size;
    }

   private:
    uint8* m_data;
    uint32 m_size;
  };

  typedef Map<String, String> NameValuePairList;

  template<bool Core>
  struct TMeshType {};

  template<>
  struct TMeshType<false>
  {
    typedef HMesh Type;
  };
  
  template<>
  struct TMeshType<true>
  {
    typedef SPtr<geCoreThread::Mesh> Type;
  };

  template<bool Core>
  struct TMaterialPtrType {};
  
  template<>
  struct TMaterialPtrType<false>
  {
    typedef HMaterial Type;
  };

  template<>
  struct TMaterialPtrType<true>
  {
    typedef SPtr<geCoreThread::Material> Type;
  };

  template<bool Core>
  struct TTextureType {};

  template<>
  struct TTextureType<false>
  {
    typedef HTexture Type;
  };

  template<>
  struct TTextureType<true>
  {
    typedef SPtr<geCoreThread::Texture> Type;
  };

  GE_ALLOW_MEMCPY_SERIALIZATION(TextureSurface);
}
