/*****************************************************************************/
/**
 * @file    geD3D11Mappings.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/06
 * @brief   Helper class that maps engine types to DirectX 11 types.
 *
 * Helper class that maps engine types to DirectX 11 types.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11Mappings.h"

#include <geDebug.h>
#include <geException.h>
#include <geColor.h>
#include <geMath.h>

namespace geEngineSDK {
  namespace geCoreThread {
    D3D11_TEXTURE_ADDRESS_MODE
    D3D11Mappings::get(TEXTURE_ADDRESSING_MODE::E tam) {
      switch (tam)
      {
        case TEXTURE_ADDRESSING_MODE::kWRAP:
          return D3D11_TEXTURE_ADDRESS_WRAP;
        case TEXTURE_ADDRESSING_MODE::kMIRROR:
          return D3D11_TEXTURE_ADDRESS_MIRROR;
        case TEXTURE_ADDRESSING_MODE::kCLAMP:
          return D3D11_TEXTURE_ADDRESS_CLAMP;
        case TEXTURE_ADDRESSING_MODE::kBORDER:
          return D3D11_TEXTURE_ADDRESS_BORDER;
      }

      return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
    }

    D3D11_BLEND
    D3D11Mappings::get(BLEND_FACTOR::E bf) {
      switch (bf)
      {
        case BLEND_FACTOR::kONE:
          return D3D11_BLEND_ONE;
        case BLEND_FACTOR::kZERO:
          return D3D11_BLEND_ZERO;
        case BLEND_FACTOR::kDEST_COLOR:
          return D3D11_BLEND_DEST_COLOR;
        case BLEND_FACTOR::kSOURCE_COLOR:
          return D3D11_BLEND_SRC_COLOR;
        case BLEND_FACTOR::kINV_DEST_COLOR:
          return D3D11_BLEND_INV_DEST_COLOR;
        case BLEND_FACTOR::kINV_SOURCE_COLOR:
          return D3D11_BLEND_INV_SRC_COLOR;
        case BLEND_FACTOR::kDEST_ALPHA:
          return D3D11_BLEND_DEST_ALPHA;
        case BLEND_FACTOR::kSOURCE_ALPHA:
          return D3D11_BLEND_SRC_ALPHA;
        case BLEND_FACTOR::kINV_DEST_ALPHA:
          return D3D11_BLEND_INV_DEST_ALPHA;
        case BLEND_FACTOR::kINV_SOURCE_ALPHA:
          return D3D11_BLEND_INV_SRC_ALPHA;
      }

      //Unsupported type
      return D3D11_BLEND_ZERO;
    }

    D3D11_BLEND_OP
    D3D11Mappings::get(BLEND_OPERATION::E bo) {
      switch (bo)
      {
        case BLEND_OPERATION::kADD:
          return D3D11_BLEND_OP_ADD;
        case BLEND_OPERATION::kSUBTRACT:
          return D3D11_BLEND_OP_SUBTRACT;
        case BLEND_OPERATION::kREVERSE_SUBTRACT:
          return D3D11_BLEND_OP_REV_SUBTRACT;
        case BLEND_OPERATION::kMIN:
          return D3D11_BLEND_OP_MIN;
        case BLEND_OPERATION::kMAX:
          return D3D11_BLEND_OP_MAX;
      }

      //Unsupported type
      return D3D11_BLEND_OP_ADD;
    }

    D3D11_COMPARISON_FUNC
    D3D11Mappings::get(COMPARE_FUNCTION::E cf) {
      switch (cf)
      {
        case COMPARE_FUNCTION::kALWAYS_FAIL:
          return D3D11_COMPARISON_NEVER;
        case COMPARE_FUNCTION::kALWAYS_PASS:
          return D3D11_COMPARISON_ALWAYS;
        case COMPARE_FUNCTION::kLESS:
          return D3D11_COMPARISON_LESS;
        case COMPARE_FUNCTION::kLESS_EQUAL:
          return D3D11_COMPARISON_LESS_EQUAL;
        case COMPARE_FUNCTION::kEQUAL:
          return D3D11_COMPARISON_EQUAL;
        case COMPARE_FUNCTION::kNOT_EQUAL:
          return D3D11_COMPARISON_NOT_EQUAL;
        case COMPARE_FUNCTION::kGREATER_EQUAL:
          return D3D11_COMPARISON_GREATER_EQUAL;
        case COMPARE_FUNCTION::kGREATER:
          return D3D11_COMPARISON_GREATER;
      };

      //Unsupported type
      return D3D11_COMPARISON_ALWAYS;
    }

    D3D11_CULL_MODE
    D3D11Mappings::get(CULLING_MODE::E cm) {
      switch (cm)
      {
        case CULLING_MODE::kNONE:
          return D3D11_CULL_NONE;
        case CULLING_MODE::kCLOCKWISE:
          return D3D11_CULL_FRONT;
        case CULLING_MODE::kCOUNTERCLOCKWISE:
          return D3D11_CULL_BACK;
      }

      //Unsupported type
      return D3D11_CULL_NONE;
    }

    D3D11_FILL_MODE
    D3D11Mappings::get(POLYGON_MODE::E mode) {
      switch (mode)
      {
        case POLYGON_MODE::kWIREFRAME:
          return D3D11_FILL_WIREFRAME;
        case POLYGON_MODE::kSOLID:
          return D3D11_FILL_SOLID;
      }

      return D3D11_FILL_SOLID;
    }

    D3D11_STENCIL_OP
    D3D11Mappings::get(STENCIL_OPERATION::E op, bool invert) {
      switch (op)
      {
        case STENCIL_OPERATION::kKEEP:
          return D3D11_STENCIL_OP_KEEP;
        case STENCIL_OPERATION::kZERO:
          return D3D11_STENCIL_OP_ZERO;
        case STENCIL_OPERATION::kREPLACE:
          return D3D11_STENCIL_OP_REPLACE;
        case STENCIL_OPERATION::kINCREMENT:
          return invert ? D3D11_STENCIL_OP_DECR_SAT : D3D11_STENCIL_OP_INCR_SAT;
        case STENCIL_OPERATION::kDECREMENT:
          return invert ? D3D11_STENCIL_OP_INCR_SAT : D3D11_STENCIL_OP_DECR_SAT;
        case STENCIL_OPERATION::kINCREMENT_WRAP:
          return invert ? D3D11_STENCIL_OP_DECR : D3D11_STENCIL_OP_INCR;
        case STENCIL_OPERATION::kDECREMENT_WRAP:
          return invert ? D3D11_STENCIL_OP_INCR : D3D11_STENCIL_OP_DECR;
        case STENCIL_OPERATION::kINVERT:
          return D3D11_STENCIL_OP_INVERT;
      }

      //Unsupported type
      return D3D11_STENCIL_OP_KEEP;
    }

    DWORD
    D3D11Mappings::get(FILTER_TYPE::E ft) {
      switch (ft)
      {
        case FILTER_TYPE::kMIN:
          return D3D11_MIN_FILTER_SHIFT;
        case FILTER_TYPE::kMAG:
          return D3D11_MAG_FILTER_SHIFT;
        case FILTER_TYPE::kMIP:
          return D3D11_MIP_FILTER_SHIFT;
      }

      // Unsupported type
      return D3D11_MIP_FILTER_SHIFT;
    }

# define MERGE_FOR_SWITCH(_comparison_, _min_ , _mag_, _mip_ )                \
         ((_comparison_ << 16) | (_min_ << 8) | (_mag_ << 4) | (_mip_))

    D3D11_FILTER
    D3D11Mappings::get(const FILTER_OPTIONS::E min,
                       const FILTER_OPTIONS::E mag,
                       const FILTER_OPTIONS::E mip,
                       const bool comparison) {
      D3D11_FILTER res;

      switch ((MERGE_FOR_SWITCH(comparison, min, mag, mip)))
      {
        case MERGE_FOR_SWITCH(true,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kPOINT):
          res = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
          break;
        case MERGE_FOR_SWITCH(true,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kLINEAR):
          res = D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
          break;
        case MERGE_FOR_SWITCH(true,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kPOINT):
          res = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
          break;
        case MERGE_FOR_SWITCH(true,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kLINEAR):
          res = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
          break;
        case MERGE_FOR_SWITCH(true,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kPOINT):
          res = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
          break;
        case MERGE_FOR_SWITCH(true,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kLINEAR):
          res = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
          break;
        case MERGE_FOR_SWITCH(true,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kPOINT):
          res = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
          break;
        case MERGE_FOR_SWITCH(true,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kLINEAR):
          res = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
          break;
        case MERGE_FOR_SWITCH(true,
                              FILTER_OPTIONS::kANISOTROPIC,
                              FILTER_OPTIONS::kANISOTROPIC,
                              FILTER_OPTIONS::kANISOTROPIC):
          res = D3D11_FILTER_COMPARISON_ANISOTROPIC;
          break;
        case MERGE_FOR_SWITCH(false,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kPOINT):
          res = D3D11_FILTER_MIN_MAG_MIP_POINT;
          break;
        case MERGE_FOR_SWITCH(false,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kLINEAR):
          res = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
          break;
        case MERGE_FOR_SWITCH(false,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kPOINT):
          res = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
          break;
        case MERGE_FOR_SWITCH(false,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kLINEAR):
          res = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
          break;
        case MERGE_FOR_SWITCH(false,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kPOINT):
          res = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
          break;
        case MERGE_FOR_SWITCH(false,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kPOINT,
                              FILTER_OPTIONS::kLINEAR):
          res = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
          break;
        case MERGE_FOR_SWITCH(false,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kPOINT):
          res = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
          break;
        case MERGE_FOR_SWITCH(false,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kLINEAR,
                              FILTER_OPTIONS::kLINEAR):
          res = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
          break;
        case MERGE_FOR_SWITCH(false,
                              FILTER_OPTIONS::kANISOTROPIC,
                              FILTER_OPTIONS::kANISOTROPIC,
                              FILTER_OPTIONS::kANISOTROPIC):
          res = D3D11_FILTER_ANISOTROPIC;
          break;
        default:
          res = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      }

      return res;
    }

    DWORD
    D3D11Mappings::get(GPU_BUFFER_USAGE::E usage) {
      DWORD ret = D3D11_USAGE_DEFAULT;

      if (usage & GPU_BUFFER_USAGE::kDYNAMIC) {
        ret = D3D11_USAGE_DYNAMIC;
      }

      return ret;
    }

    D3D11_MAP
    D3D11Mappings::get(GPU_LOCK_OPTIONS::E options,
                       GPU_BUFFER_USAGE::E usage) {
      D3D11_MAP ret = D3D11_MAP_READ_WRITE;
      if (GPU_LOCK_OPTIONS::kWRITE_ONLY_DISCARD == options) {
        //D3D doesn't like discard on non-dynamic buffers
        if (usage & GPU_BUFFER_USAGE::kDYNAMIC) {
          ret = D3D11_MAP_WRITE_DISCARD;
        }
        else {
          ret = D3D11_MAP_WRITE;
        }
      }
      else if (GPU_LOCK_OPTIONS::kREAD_ONLY == options) {
        ret = D3D11_MAP_READ;
      }
      else if (GPU_LOCK_OPTIONS::kWRITE_ONLY_NO_OVERWRITE == options) {
        //Only allowed for vertex / index buffers
        ret = D3D11_MAP_WRITE_NO_OVERWRITE;
      }

      return ret;
    }

    DXGI_FORMAT
    D3D11Mappings::get(VERTEX_ELEMENT_TYPE::E type) {
      switch (type)
      {
        case VERTEX_ELEMENT_TYPE::kCOLOR:
        case VERTEX_ELEMENT_TYPE::kCOLOR_ABGR:
        case VERTEX_ELEMENT_TYPE::kCOLOR_ARGB:
        case VERTEX_ELEMENT_TYPE::kUBYTE4_NORM:
          return DXGI_FORMAT_R8G8B8A8_UNORM;
        case VERTEX_ELEMENT_TYPE::kFLOAT1:
          return DXGI_FORMAT_R32_FLOAT;
        case VERTEX_ELEMENT_TYPE::kFLOAT2:
          return DXGI_FORMAT_R32G32_FLOAT;
        case VERTEX_ELEMENT_TYPE::kFLOAT3:
          return DXGI_FORMAT_R32G32B32_FLOAT;
        case VERTEX_ELEMENT_TYPE::kFLOAT4:
          return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case VERTEX_ELEMENT_TYPE::kUSHORT1:
          return DXGI_FORMAT_R16_UINT;
        case VERTEX_ELEMENT_TYPE::kUSHORT2:
          return DXGI_FORMAT_R16G16_UINT;
        case VERTEX_ELEMENT_TYPE::kUSHORT4:
          return DXGI_FORMAT_R16G16B16A16_UINT;
        case VERTEX_ELEMENT_TYPE::kSHORT1:
          return DXGI_FORMAT_R16_SINT;
        case VERTEX_ELEMENT_TYPE::kSHORT2:
          return DXGI_FORMAT_R16G16_SINT;
        case VERTEX_ELEMENT_TYPE::kSHORT4:
          return DXGI_FORMAT_R16G16B16A16_SINT;
        case VERTEX_ELEMENT_TYPE::kUINT1:
          return DXGI_FORMAT_R32_UINT;
        case VERTEX_ELEMENT_TYPE::kUINT2:
          return DXGI_FORMAT_R32G32_UINT;
        case VERTEX_ELEMENT_TYPE::kUINT3:
          return DXGI_FORMAT_R32G32B32_UINT;
        case VERTEX_ELEMENT_TYPE::kUINT4:
          return DXGI_FORMAT_R32G32B32A32_UINT;
        case VERTEX_ELEMENT_TYPE::kINT1:
          return DXGI_FORMAT_R32_SINT;
        case VERTEX_ELEMENT_TYPE::kINT2:
          return DXGI_FORMAT_R32G32_SINT;
        case VERTEX_ELEMENT_TYPE::kINT3:
          return DXGI_FORMAT_R32G32B32_SINT;
        case VERTEX_ELEMENT_TYPE::kINT4:
          return DXGI_FORMAT_R32G32B32A32_SINT;
        case VERTEX_ELEMENT_TYPE::kUBYTE4:
          return DXGI_FORMAT_R8G8B8A8_UINT;
      }

      //Unsupported type
      return DXGI_FORMAT_R32G32B32A32_FLOAT;
    }

    VERTEX_ELEMENT_SEMANTIC::E
    D3D11Mappings::get(LPCSTR sem) {
      if (strcmp(sem, "BLENDINDICES") == 0) {
        return VERTEX_ELEMENT_SEMANTIC::kBLEND_INDICES;
      }
      if (strcmp(sem, "BLENDWEIGHT") == 0) {
        return VERTEX_ELEMENT_SEMANTIC::kBLEND_WEIGHTS;
      }
      if (strcmp(sem, "COLOR") == 0) {
        return VERTEX_ELEMENT_SEMANTIC::kCOLOR;
      }
      if (strcmp(sem, "NORMAL") == 0) {
        return VERTEX_ELEMENT_SEMANTIC::kNORMAL;
      }
      if (strcmp(sem, "POSITION") == 0) {
        return VERTEX_ELEMENT_SEMANTIC::kPOSITION;
      }
      if (strcmp(sem, "TEXCOORD") == 0) {
        return VERTEX_ELEMENT_SEMANTIC::kTEXCOORD;
      }
      if (strcmp(sem, "BINORMAL") == 0) {
        return VERTEX_ELEMENT_SEMANTIC::kBINORMAL;
      }
      if (strcmp(sem, "TANGENT") == 0) {
        return VERTEX_ELEMENT_SEMANTIC::kTANGENT;
      }
      if (strcmp(sem, "POSITIONT") == 0) {
        return VERTEX_ELEMENT_SEMANTIC::kPOSITIONT;
      }
      if (strcmp(sem, "PSIZE") == 0) {
        return VERTEX_ELEMENT_SEMANTIC::kPSIZE;
      }

      //Unsupported type
      return VERTEX_ELEMENT_SEMANTIC::kPOSITION;
    }

    LPCSTR
    D3D11Mappings::get(VERTEX_ELEMENT_SEMANTIC::E sem) {
      switch (sem)
      {
        case VERTEX_ELEMENT_SEMANTIC::kBLEND_INDICES:
          return "BLENDINDICES";
        case VERTEX_ELEMENT_SEMANTIC::kBLEND_WEIGHTS:
          return "BLENDWEIGHT";
        case VERTEX_ELEMENT_SEMANTIC::kCOLOR:
          return "COLOR";
        case VERTEX_ELEMENT_SEMANTIC::kNORMAL:
          return "NORMAL";
        case VERTEX_ELEMENT_SEMANTIC::kPOSITION:
          return "POSITION";
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

      //Unsupported type
      return "";
    }

    VERTEX_ELEMENT_TYPE::E
    D3D11Mappings::getInputType(D3D_REGISTER_COMPONENT_TYPE type) {
      switch (type)
      {
        case D3D_REGISTER_COMPONENT_FLOAT32:
          return VERTEX_ELEMENT_TYPE::kFLOAT4;
        case D3D_REGISTER_COMPONENT_SINT32:
          return VERTEX_ELEMENT_TYPE::kINT4;
        case D3D_REGISTER_COMPONENT_UINT32:
          return VERTEX_ELEMENT_TYPE::kUINT4;
        default:
          return VERTEX_ELEMENT_TYPE::kFLOAT4;
      }
    }

    void
    D3D11Mappings::get(const LinearColor& inColor, float* outColor) {
      //This is a risk, and we might just use the color directly
      outColor[0] = inColor.r;
      outColor[1] = inColor.g;
      outColor[2] = inColor.b;
      outColor[3] = inColor.a;
    }

    PixelFormat
    D3D11Mappings::getPF(DXGI_FORMAT pf) {
      switch (pf)
      {
        case DXGI_FORMAT_UNKNOWN:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
          return PixelFormat::kRGBA32F;
        case DXGI_FORMAT_R32G32B32A32_UINT:
          return PixelFormat::kRGBA32U;
        case DXGI_FORMAT_R32G32B32A32_SINT:
          return PixelFormat::kRGBA32I;
        case DXGI_FORMAT_R32G32B32_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R32G32B32_FLOAT:
          return PixelFormat::kRGB32F;
        case DXGI_FORMAT_R32G32B32_UINT:
          return PixelFormat::kRGB32U;
        case DXGI_FORMAT_R32G32B32_SINT:
          return PixelFormat::kRGB32I;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
          return PixelFormat::kRGBA16F;
        case DXGI_FORMAT_R16G16B16A16_UNORM:
          return PixelFormat::kRGBA16;
        case DXGI_FORMAT_R16G16B16A16_UINT:
          return PixelFormat::kRGBA16U;
        case DXGI_FORMAT_R16G16B16A16_SNORM:
          return PixelFormat::kRGBA16S;
        case DXGI_FORMAT_R16G16B16A16_SINT:
          return PixelFormat::kRGBA16I;
        case DXGI_FORMAT_R32G32_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R32G32_FLOAT:
          return PixelFormat::kRG32F;
        case DXGI_FORMAT_R32G32_UINT:
          return PixelFormat::kRG32U;
        case DXGI_FORMAT_R32G32_SINT:
          return PixelFormat::kRG32I;
        case DXGI_FORMAT_R32G8X24_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
          return PixelFormat::kD32_S8X24;
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R10G10B10A2_UNORM:
          return PixelFormat::kRGB10A2;
        case DXGI_FORMAT_R10G10B10A2_UINT:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R11G11B10_FLOAT:
          return PixelFormat::kRG11B10F;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
          return PixelFormat::kRGBA8;
        case DXGI_FORMAT_R8G8B8A8_UINT:
          return PixelFormat::kRGBA8U;
        case DXGI_FORMAT_R8G8B8A8_SNORM:
          return PixelFormat::kRGBA8S;
        case DXGI_FORMAT_R8G8B8A8_SINT:
          return PixelFormat::kRGBA8I;
        case DXGI_FORMAT_R16G16_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R16G16_FLOAT:
          return PixelFormat::kRG16F;
        case DXGI_FORMAT_R16G16_UNORM:
          return PixelFormat::kRG16;
        case DXGI_FORMAT_R16G16_UINT:
          return PixelFormat::kRG16U;
        case DXGI_FORMAT_R16G16_SNORM:
          return PixelFormat::kRG16S;
        case DXGI_FORMAT_R16G16_SINT:
          return PixelFormat::kRG16I;
        case DXGI_FORMAT_R32_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_D32_FLOAT:
          return PixelFormat::kD32;
        case DXGI_FORMAT_R32_FLOAT:
          return PixelFormat::kR32F;
        case DXGI_FORMAT_R32_UINT:
          return PixelFormat::kR32U;
        case DXGI_FORMAT_R32_SINT:
          return PixelFormat::kR32I;
        case DXGI_FORMAT_R24G8_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
          return PixelFormat::kD24S8;
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R8G8_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R8G8_UNORM:
          return PixelFormat::kRG8;
        case DXGI_FORMAT_R8G8_UINT:
          return PixelFormat::kRG8U;
        case DXGI_FORMAT_R8G8_SNORM:
          return PixelFormat::kRG8S;
        case DXGI_FORMAT_R8G8_SINT:
          return PixelFormat::kRG8I;
        case DXGI_FORMAT_R16_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R16_FLOAT:
          return PixelFormat::kR16F;
        case DXGI_FORMAT_D16_UNORM:
          return PixelFormat::kD16;
        case DXGI_FORMAT_R16_UNORM:
          return PixelFormat::kR16;
        case DXGI_FORMAT_R16_UINT:
          return PixelFormat::kR16U;
        case DXGI_FORMAT_R16_SNORM:
          return PixelFormat::kR16S;
        case DXGI_FORMAT_R16_SINT:
          return PixelFormat::kR16I;
        case DXGI_FORMAT_R8_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R8_UNORM:
          return PixelFormat::kR8;
        case DXGI_FORMAT_R8_UINT:
          return PixelFormat::kR8U;
        case DXGI_FORMAT_R8_SNORM:
          return PixelFormat::kR8S;
        case DXGI_FORMAT_R8_SINT:
          return PixelFormat::kR8I;
        case DXGI_FORMAT_A8_UNORM:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R1_UNORM:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_BC1_TYPELESS:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_BC1_UNORM:
        case DXGI_FORMAT_BC1_UNORM_SRGB:
          return PixelFormat::kBC1;
        case DXGI_FORMAT_BC2_TYPELESS:
          return PixelFormat::kBC2;
        case DXGI_FORMAT_BC2_UNORM:
        case DXGI_FORMAT_BC2_UNORM_SRGB:
          return PixelFormat::kBC2;
        case DXGI_FORMAT_BC3_TYPELESS:
          return PixelFormat::kBC3;
        case DXGI_FORMAT_BC3_UNORM:
        case DXGI_FORMAT_BC3_UNORM_SRGB:
          return PixelFormat::kBC3;
        case DXGI_FORMAT_BC4_TYPELESS:
          return PixelFormat::kBC4;
        case DXGI_FORMAT_BC4_UNORM:
          return PixelFormat::kBC4;
        case DXGI_FORMAT_BC4_SNORM:
          return PixelFormat::kBC4;
        case DXGI_FORMAT_BC5_TYPELESS:
          return PixelFormat::kBC5;
        case DXGI_FORMAT_BC5_UNORM:
          return PixelFormat::kBC5;
        case DXGI_FORMAT_BC5_SNORM:
          return PixelFormat::kBC5;
        case DXGI_FORMAT_BC6H_UF16:
          return PixelFormat::kBC6H;
        case DXGI_FORMAT_BC6H_SF16:
          return PixelFormat::kBC6H;
        case DXGI_FORMAT_BC6H_TYPELESS:
          return PixelFormat::kBC6H;
        case DXGI_FORMAT_BC7_UNORM:
        case DXGI_FORMAT_BC7_UNORM_SRGB:
          return PixelFormat::kBC7;
        case DXGI_FORMAT_BC7_TYPELESS:
          return PixelFormat::kBC7;
        case DXGI_FORMAT_B5G6R5_UNORM:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_B5G5R5A1_UNORM:
          return PixelFormat::kUNKNOWN;
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
          return PixelFormat::kBGR8;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
          return PixelFormat::kBGRA8;
      }

      return PixelFormat::kUNKNOWN;
    }

    DXGI_FORMAT
    D3D11Mappings::getPF(PixelFormat pf, bool gamma) {
      switch (pf)
      {
        case PixelFormat::kR8:
          return DXGI_FORMAT_R8_UNORM;
        case PixelFormat::kR8S:
          return DXGI_FORMAT_R8_SNORM;
        case PixelFormat::kR8I:
          return DXGI_FORMAT_R8_SINT;
        case PixelFormat::kR8U:
          return DXGI_FORMAT_R8_UINT;
        case PixelFormat::kRG8:
          return DXGI_FORMAT_R8G8_UNORM;
        case PixelFormat::kRG8S:
          return DXGI_FORMAT_R8G8_SNORM;
        case PixelFormat::kRG8I:
          return DXGI_FORMAT_R8G8_SINT;
        case PixelFormat::kRG8U:
          return DXGI_FORMAT_R8G8_UINT;
        case PixelFormat::kBGR8:
          if (gamma) {
            return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
          }
          return DXGI_FORMAT_B8G8R8X8_UNORM;
        case PixelFormat::kRGB8:
        case PixelFormat::kRGBA8:
          if (gamma) {
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
          }
          return DXGI_FORMAT_R8G8B8A8_UNORM;
        case PixelFormat::kRGBA8I:
          return DXGI_FORMAT_R8G8B8A8_SINT;
        case PixelFormat::kRGBA8U:
          return DXGI_FORMAT_R8G8B8A8_UINT;
        case PixelFormat::kRGBA8S:
          return DXGI_FORMAT_R8G8B8A8_SNORM;
        case PixelFormat::kBGRA8:
          if (gamma) {
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
          }
          return DXGI_FORMAT_B8G8R8A8_UNORM;
        case PixelFormat::kR16F:
          return DXGI_FORMAT_R16_FLOAT;
        case PixelFormat::kRG16F:
          return DXGI_FORMAT_R16G16_FLOAT;
        case PixelFormat::kRGBA16F:
          return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case PixelFormat::kR32F:
          return DXGI_FORMAT_R32_FLOAT;
        case PixelFormat::kRG32F:
          return DXGI_FORMAT_R32G32_FLOAT;
        case PixelFormat::kRGB32F:
          return DXGI_FORMAT_R32G32B32_FLOAT;
        case PixelFormat::kRGBA32F:
          return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case PixelFormat::kR16I:
          return DXGI_FORMAT_R16_SINT;
        case PixelFormat::kRG16I:
          return DXGI_FORMAT_R16G16_SINT;
        case PixelFormat::kRGBA16I:
          return DXGI_FORMAT_R16G16B16A16_SINT;
        case PixelFormat::kR16U:
          return DXGI_FORMAT_R16_UINT;
        case PixelFormat::kRG16U:
          return DXGI_FORMAT_R16G16_UINT;
        case PixelFormat::kRGBA16U:
          return DXGI_FORMAT_R16G16B16A16_UINT;
        case PixelFormat::kR32I:
          return DXGI_FORMAT_R32_SINT;
        case PixelFormat::kRG32I:
          return DXGI_FORMAT_R32G32_SINT;
        case PixelFormat::kRGB32I:
          return DXGI_FORMAT_R32G32B32_SINT;
        case PixelFormat::kR32U:
          return DXGI_FORMAT_R32_UINT;
        case PixelFormat::kRG32U:
          return DXGI_FORMAT_R32G32_UINT;
        case PixelFormat::kRGB32U:
          return DXGI_FORMAT_R32G32B32_UINT;
        case PixelFormat::kRGBA32U:
          return DXGI_FORMAT_R32G32B32A32_UINT;
        case PixelFormat::kR16S:
          return DXGI_FORMAT_R16_SNORM;
        case PixelFormat::kRG16S:
          return DXGI_FORMAT_R16G16_SNORM;
        case PixelFormat::kRGBA16S:
          return DXGI_FORMAT_R16G16B16A16_SNORM;
        case PixelFormat::kR16:
          return DXGI_FORMAT_R16_UNORM;
        case PixelFormat::kRG16:
          return DXGI_FORMAT_R16G16_UNORM;
        case PixelFormat::kRGBA16:
          return DXGI_FORMAT_R16G16B16A16_UNORM;
        case PixelFormat::kBC1:
        case PixelFormat::kBC1a:
          if (gamma) {
            return DXGI_FORMAT_BC1_UNORM_SRGB;
          }
          return DXGI_FORMAT_BC1_UNORM;
        case PixelFormat::kBC2:
          if (gamma) {
            return DXGI_FORMAT_BC2_UNORM_SRGB;
          }
          return DXGI_FORMAT_BC2_UNORM;
        case PixelFormat::kBC3:
          if (gamma) {
            return DXGI_FORMAT_BC3_UNORM_SRGB;
          }
          return DXGI_FORMAT_BC3_UNORM;
        case PixelFormat::kBC4:
          return DXGI_FORMAT_BC4_UNORM;
        case PixelFormat::kBC5:
          return DXGI_FORMAT_BC5_UNORM;
        case PixelFormat::kBC6H:
          return DXGI_FORMAT_BC6H_UF16;
        case PixelFormat::kBC7:
          if (gamma) {
            return DXGI_FORMAT_BC7_UNORM_SRGB;
          }
          return DXGI_FORMAT_BC7_UNORM;
        case PixelFormat::kD32_S8X24:
          return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        case PixelFormat::kD24S8:
          return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case PixelFormat::kD32:
          return DXGI_FORMAT_D32_FLOAT;
        case PixelFormat::kD16:
          return DXGI_FORMAT_D16_UNORM;
        case PixelFormat::kRG11B10F:
          return DXGI_FORMAT_R11G11B10_FLOAT;
        case PixelFormat::kRGB10A2:
          return DXGI_FORMAT_R10G10B10A2_UNORM;
      }

      return DXGI_FORMAT_UNKNOWN;
    }

    DXGI_FORMAT
    D3D11Mappings::getBF(GPU_BUFFER_FORMAT::E format) {
      static bool lookupInitialized = false;

      static DXGI_FORMAT lookup[GPU_BUFFER_FORMAT::kCOUNT];
      if (!lookupInitialized) {
        lookup[GPU_BUFFER_FORMAT::k16X1F] = DXGI_FORMAT_R16_FLOAT;
        lookup[GPU_BUFFER_FORMAT::k16X2F] = DXGI_FORMAT_R16G16_FLOAT;
        lookup[GPU_BUFFER_FORMAT::k16X4F] = DXGI_FORMAT_R16G16B16A16_FLOAT;
        lookup[GPU_BUFFER_FORMAT::k32X1F] = DXGI_FORMAT_R32_FLOAT;
        lookup[GPU_BUFFER_FORMAT::k32X2F] = DXGI_FORMAT_R32G32_FLOAT;
        lookup[GPU_BUFFER_FORMAT::k32X3F] = DXGI_FORMAT_R32G32B32_FLOAT;
        lookup[GPU_BUFFER_FORMAT::k32X4F] = DXGI_FORMAT_R32G32B32A32_FLOAT;
        lookup[GPU_BUFFER_FORMAT::k8X1] = DXGI_FORMAT_R8_UNORM;
        lookup[GPU_BUFFER_FORMAT::k8X2] = DXGI_FORMAT_R8G8_UNORM;
        lookup[GPU_BUFFER_FORMAT::k8X4] = DXGI_FORMAT_R8G8B8A8_UNORM;
        lookup[GPU_BUFFER_FORMAT::k16X1] = DXGI_FORMAT_R16_UNORM;
        lookup[GPU_BUFFER_FORMAT::k16X2] = DXGI_FORMAT_R16G16_UNORM;
        lookup[GPU_BUFFER_FORMAT::k16X4] = DXGI_FORMAT_R16G16B16A16_UNORM;
        lookup[GPU_BUFFER_FORMAT::k8X1S] = DXGI_FORMAT_R8_SINT;
        lookup[GPU_BUFFER_FORMAT::k8X2S] = DXGI_FORMAT_R8G8_SINT;
        lookup[GPU_BUFFER_FORMAT::k8X4S] = DXGI_FORMAT_R8G8B8A8_SINT;
        lookup[GPU_BUFFER_FORMAT::k16X1S] = DXGI_FORMAT_R16_SINT;
        lookup[GPU_BUFFER_FORMAT::k16X2S] = DXGI_FORMAT_R16G16_SINT;
        lookup[GPU_BUFFER_FORMAT::k16X4S] = DXGI_FORMAT_R16G16B16A16_SINT;
        lookup[GPU_BUFFER_FORMAT::k32X1S] = DXGI_FORMAT_R32_SINT;
        lookup[GPU_BUFFER_FORMAT::k32X2S] = DXGI_FORMAT_R32G32_SINT;
        lookup[GPU_BUFFER_FORMAT::k32X3S] = DXGI_FORMAT_R32G32B32_SINT;
        lookup[GPU_BUFFER_FORMAT::k32X4S] = DXGI_FORMAT_R32G32B32A32_SINT;
        lookup[GPU_BUFFER_FORMAT::k8X1U] = DXGI_FORMAT_R8_UINT;
        lookup[GPU_BUFFER_FORMAT::k8X2U] = DXGI_FORMAT_R8G8_UINT;
        lookup[GPU_BUFFER_FORMAT::k8X4U] = DXGI_FORMAT_R8G8B8A8_UINT;
        lookup[GPU_BUFFER_FORMAT::k16X1U] = DXGI_FORMAT_R16_UINT;
        lookup[GPU_BUFFER_FORMAT::k16X2U] = DXGI_FORMAT_R16G16_UINT;
        lookup[GPU_BUFFER_FORMAT::k16X4U] = DXGI_FORMAT_R16G16B16A16_UINT;
        lookup[GPU_BUFFER_FORMAT::k32X1U] = DXGI_FORMAT_R32_UINT;
        lookup[GPU_BUFFER_FORMAT::k32X2U] = DXGI_FORMAT_R32G32_UINT;
        lookup[GPU_BUFFER_FORMAT::k32X3U] = DXGI_FORMAT_R32G32B32_UINT;
        lookup[GPU_BUFFER_FORMAT::k32X4U] = DXGI_FORMAT_R32G32B32A32_UINT;
        lookupInitialized = true;
      }

      if (GPU_BUFFER_FORMAT::kCOUNT <= format) {
        return DXGI_FORMAT_UNKNOWN;
      }

      return lookup[static_cast<uint32>(format)];
    }

    DXGI_FORMAT
    D3D11Mappings::getTypelessDepthStencilPF(PixelFormat format) {
      switch (format)
      {
        case PixelFormat::kD32_S8X24:
          return DXGI_FORMAT_R32G8X24_TYPELESS;
        case PixelFormat::kD24S8:
          return DXGI_FORMAT_R24G8_TYPELESS;
        case PixelFormat::kD32:
          return DXGI_FORMAT_R32_TYPELESS;
        case PixelFormat::kD16:
          return DXGI_FORMAT_R16_TYPELESS;
      }

      return DXGI_FORMAT_UNKNOWN;
    }

    DXGI_FORMAT
    D3D11Mappings::getShaderResourceDepthStencilPF(PixelFormat format)
    {
      switch (format)
      {
        case PixelFormat::kD32_S8X24:
          return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        case PixelFormat::kD24S8:
          return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        case PixelFormat::kD32:
          return DXGI_FORMAT_R32_FLOAT;
        case PixelFormat::kD16:
          return DXGI_FORMAT_R16_UNORM;
      }

      return DXGI_FORMAT_UNKNOWN;
    }

    PixelFormat
    D3D11Mappings::getClosestSupportedPF(PixelFormat pf,
                                         TextureType texType,
                                         int32 usage) {
      //Check for any obvious issues first
      PixelUtil::checkFormat(pf, texType, usage);

      //Check for formats that are not supported at all by DX11
      switch (pf)
      {
        case PixelFormat::kRGB8:
          pf = PixelFormat::kRGBA8;
          break;
        default:
          break;
      }

      //Check for usage specific format restrictions
      if ((usage & TU_LOADSTORE) != 0)
      {
        switch (pf)
        {
          case PixelFormat::kBGRA8:
            pf = PixelFormat::kRGBA8;
            break;
          default:
            break;
        }
      }

      return pf;
    }

    D3D11_USAGE
    D3D11Mappings::getUsage(GPU_BUFFER_USAGE::E usage) {
      if (isDynamic(usage)) {
        return D3D11_USAGE_DYNAMIC;
      }

      return D3D11_USAGE_DEFAULT;
    }

    bool
    D3D11Mappings::isDynamic(GPU_BUFFER_USAGE::E usage) {
      return (usage & GPU_BUFFER_USAGE::kDYNAMIC) != 0;
    }

    bool
    D3D11Mappings::isMappingWrite(D3D11_MAP map) {
      if (D3D11_MAP_READ == map) {
        return false;
      }

      return true;
    }

    bool
    D3D11Mappings::isMappingRead(D3D11_MAP map) {
      if (D3D11_MAP_READ == map || D3D11_MAP_READ_WRITE == map) {
        return true;
      }

      return false;
    }

    UINT
    D3D11Mappings::getAccessFlags(GPU_BUFFER_USAGE::E usage) {
      if (isDynamic(usage)) {
        return D3D11_CPU_ACCESS_WRITE;
      }

      return 0;
    }

    D3D11_PRIMITIVE_TOPOLOGY
    D3D11Mappings::getPrimitiveType(DrawOperationType type) {
      switch (type)
      {
        case DrawOperationType::DOT_POINT_LIST:
          return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        case DrawOperationType::DOT_LINE_LIST:
          return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        case DrawOperationType::DOT_LINE_STRIP:
          return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case DrawOperationType::DOT_TRIANGLE_LIST:
          return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case DrawOperationType::DOT_TRIANGLE_STRIP:
          return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        case DrawOperationType::DOT_TRIANGLE_FAN:
          GE_EXCEPT(InvalidParametersException,
                    "D3D11 doesn't support triangle fan primitive type.");
      }

      return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }

    uint32
    D3D11Mappings::getSizeInBytes(PixelFormat pf,
                                  uint32 width,
                                  uint32 height) {
      if (PixelUtil::isCompressed(pf)) {
        uint32 blockWidth = Math::divideAndRoundUp(width, 4U);
        uint32 blockHeight = Math::divideAndRoundUp(height, 4U);

        //D3D wants the width of one row of cells in bytes
        if (PixelFormat::kBC1 == pf || PixelFormat::kBC4 == pf) {
          //64 bits (8 bytes) per 4x4 block
          return blockWidth * blockHeight * 8;
        }
        else {
          //128 bits (16 bytes) per 4x4 block
          return blockWidth * blockHeight * 16;
        }
      }
      
      return width * height * PixelUtil::getNumElemBytes(pf);
    }

    D3D11_MAP
    D3D11Mappings::getLockOptions(GPU_LOCK_OPTIONS::E lockOptions) {
      switch (lockOptions)
      {
        case GPU_LOCK_OPTIONS::kWRITE_ONLY_NO_OVERWRITE:
          return D3D11_MAP_WRITE_NO_OVERWRITE;
        case GPU_LOCK_OPTIONS::kREAD_WRITE:
          return D3D11_MAP_READ_WRITE;
        case GPU_LOCK_OPTIONS::kWRITE_ONLY_DISCARD:
          return D3D11_MAP_WRITE_DISCARD;
        case GPU_LOCK_OPTIONS::kREAD_ONLY:
          return D3D11_MAP_READ;
        case GPU_LOCK_OPTIONS::kWRITE_ONLY:
          return D3D11_MAP_WRITE;
        default:
          break;
      };

      GE_EXCEPT(RenderingAPIException,
                "Invalid lock option. No DX11 equivalent of: " +
                toString(lockOptions));

      return D3D11_MAP_WRITE;
    }
  }
}
