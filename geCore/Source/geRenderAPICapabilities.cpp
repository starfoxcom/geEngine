/*****************************************************************************/
/**
 * @file    geRenderAPICapabilities.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geRenderAPICapabilities.h"

#include <geException.h>

namespace geEngineSDK {
  Vector<geEngineSDK::String> RenderAPICapabilities::s_gpuVendorStrings;

  RenderAPICapabilities::RenderAPICapabilities() {
    for (auto& caps : m_capabilities) {
      caps = 0;
    }

    m_numTextureUnitsPerStage[GPU_PROGRAM_TYPE::kVERTEX_PROGRAM] = 0;
    m_numTextureUnitsPerStage[GPU_PROGRAM_TYPE::kFRAGMENT_PROGRAM] = 0;
    m_numTextureUnitsPerStage[GPU_PROGRAM_TYPE::kGEOMETRY_PROGRAM] = 0;
    m_numTextureUnitsPerStage[GPU_PROGRAM_TYPE::kHULL_PROGRAM] = 0;
    m_numTextureUnitsPerStage[GPU_PROGRAM_TYPE::kDOMAIN_PROGRAM] = 0;
    m_numTextureUnitsPerStage[GPU_PROGRAM_TYPE::kCOMPUTE_PROGRAM] = 0;

    m_numGPUParamBlocksPerStage[GPU_PROGRAM_TYPE::kVERTEX_PROGRAM] = 0;
    m_numGPUParamBlocksPerStage[GPU_PROGRAM_TYPE::kFRAGMENT_PROGRAM] = 0;
    m_numGPUParamBlocksPerStage[GPU_PROGRAM_TYPE::kGEOMETRY_PROGRAM] = 0;
    m_numGPUParamBlocksPerStage[GPU_PROGRAM_TYPE::kHULL_PROGRAM] = 0;
    m_numGPUParamBlocksPerStage[GPU_PROGRAM_TYPE::kDOMAIN_PROGRAM] = 0;
    m_numGPUParamBlocksPerStage[GPU_PROGRAM_TYPE::kCOMPUTE_PROGRAM] = 0;

    m_numLoadStoreTextureUnitsPerStage[GPU_PROGRAM_TYPE::kVERTEX_PROGRAM] = 0;
    m_numLoadStoreTextureUnitsPerStage[GPU_PROGRAM_TYPE::kFRAGMENT_PROGRAM] = 0;
    m_numLoadStoreTextureUnitsPerStage[GPU_PROGRAM_TYPE::kGEOMETRY_PROGRAM] = 0;
    m_numLoadStoreTextureUnitsPerStage[GPU_PROGRAM_TYPE::kHULL_PROGRAM] = 0;
    m_numLoadStoreTextureUnitsPerStage[GPU_PROGRAM_TYPE::kDOMAIN_PROGRAM] = 0;
    m_numLoadStoreTextureUnitsPerStage[GPU_PROGRAM_TYPE::kCOMPUTE_PROGRAM] = 0;
  }

  GPU_VENDOR::E
  RenderAPICapabilities::vendorFromString(const String& vendorString) {
    initVendorStrings();
    GPU_VENDOR::E ret = GPU_VENDOR::kUNKNOWN;
    String cmpString = vendorString;
    StringUtil::toLowerCase(cmpString);

    for (int32 i = 0; i < GPU_VENDOR::kVENDOR_COUNT; ++i) {
      //Case insensitive (lower case)
      if (cmpString == s_gpuVendorStrings[i]) {
        ret = static_cast<GPU_VENDOR::E>(i);
        break;
      }
    }

    return ret;
  }

  String
  RenderAPICapabilities::vendorToString(GPU_VENDOR::E v) {
    initVendorStrings();
    return s_gpuVendorStrings[v];
  }

  void
  RenderAPICapabilities::initVendorStrings() {
    if (s_gpuVendorStrings.empty()) {
      //Always lower case
      s_gpuVendorStrings.resize(GPU_VENDOR::kVENDOR_COUNT);
      s_gpuVendorStrings[GPU_VENDOR::kUNKNOWN] = "unknown";
      s_gpuVendorStrings[GPU_VENDOR::kNVIDIA] = "nvidia";
      s_gpuVendorStrings[GPU_VENDOR::kAMD] = "amd";
      s_gpuVendorStrings[GPU_VENDOR::kINTEL] = "intel";
    }
  }
}
