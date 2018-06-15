/*****************************************************************************/
/**
 * @file    geRenderStats.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   Tracks various render system statistics.
 *
 * Tracks various render system statistics.
 *
 * @note    Core thread only.
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

#include <geModule.h>

namespace geEngineSDK {
  /**
   * @brief Common object types to track resource statistics for.
   */
  namespace RENDER_STAT_RESOURCE_TYPE {
    enum E {
      kIndexBuffer,
      kVertexBuffer,
      kGPUBuffer,
      kGPUParamBuffer,
      kTexture,
      kGPUProgram,
      kQuery
    };
  }

  /**
  * @brief Object that stores various render statistics. */
  struct GE_CORE_EXPORT RenderStatsData
  {
    RenderStatsData()
      : numDrawCalls(0),
        numComputeCalls(0),
        numRenderTargetChanges(0),
        numPresents(0),
        numClears(0),
        numVertices(0),
        numPrimitives(0),
        numPipelineStateChanges(0),
        numGPUParamBinds(0),
        numVertexBufferBinds(0),
        numIndexBufferBinds(0)
    {}

    uint64 numDrawCalls;
    uint64 numComputeCalls;
    uint64 numRenderTargetChanges;
    uint64 numPresents;
    uint64 numClears;

    uint64 numVertices;
    uint64 numPrimitives;

    uint64 numPipelineStateChanges;

    uint64 numGPUParamBinds;
    uint64 numVertexBufferBinds;
    uint64 numIndexBufferBinds;

    uint64 numResourceWrites;
    uint64 numResourceReads;

    uint64 numObjectsCreated;
    uint64 numObjectsDestroyed;
  };

  /**
   * @brief Tracks various render system statistics.
   * @note  Core thread only.
   */
  class GE_CORE_EXPORT RenderStats : public Module<RenderStats>
  {
   public:
    /**
     * @brief Increments draw call counter indicating how many times were
     *        render system API Draw methods called.
     */
    void
    incNumDrawCalls() {
      ++m_data.numDrawCalls;
    }

    /**
     * @brief Increments compute call counter indicating how many times were
     *        compute shaders dispatched.
     */
    void
    incNumComputeCalls() {
      ++m_data.numComputeCalls;
    }

    /**
     * @brief Increments render target change counter indicating how many times
     *        did the active render target change.
     */
    void
    incNumRenderTargetChanges() {
      ++m_data.numRenderTargetChanges;
    }

    /**
     * @brief Increments render target present counter indicating how many
     *        times did the buffer swap happen.
     */
    void
    incNumPresents() {
      ++m_data.numPresents;
    }

    /**
     * @brief Increments render target clear counter indicating how many times
     *        did the target the cleared, entirely or partially.
     */
    void
    incNumClears() {
      ++m_data.numClears;
    }

    /**
     * @brief Increments vertex draw counter indicating how many vertices were
     *        sent to the pipeline.
     */
    void
    addNumVertices(uint32 count) {
      m_data.numVertices += count;
    }

    /**
     * @brief Increments primitive draw counter indicating how many primitives
     *        were sent to the pipeline.
     */
    void
    addNumPrimitives(uint32 count) {
      m_data.numPrimitives += count;
    }

    /**
     * @brief Increments pipeline state change counter indicating how many
     *        times was a pipeline state bound.
     */
    void
    incNumPipelineStateChanges() {
      ++m_data.numPipelineStateChanges;
    }

    /**
     * @brief Increments GPU parameter change counter indicating how many times
     *        were GPU parameters bound to the pipeline.
     */
    void
    incNumGPUParamBinds() {
      ++m_data.numGPUParamBinds;
    }

    /**
     * @brief Increments vertex buffer change counter indicating how many times
     *        was a vertex buffer bound to the pipeline.
     */
    void
    incNumVertexBufferBinds() {
      ++m_data.numVertexBufferBinds;
    }

    /**
     * @brief Increments index buffer change counter indicating how many times
     *        was a index buffer bound to the pipeline.
     */
    void
    incNumIndexBufferBinds() {
      ++m_data.numIndexBufferBinds;
    }

    /**
     * @brief Increments created GPU resource counter.
     * @param[in] category  Category of the resource.
     */
    void
    incResCreated(uint32 /*category*/) {
      //TODO: I'm ignoring resourceType for now. Later I will want to count
      //object creation/destruction/read/write per type. I will also want to
      //allow the caller to assign names to specific "resourceType" id.
      //(Since many types will be RenderAPI specific).

      //TODO: I should also track number of active GPU objects using this
      //method, instead of just keeping track of how many were created and
      //destroyed during the frame.
      ++m_data.numObjectsCreated;
    }

    /**
     * @brief Increments destroyed GPU resource counter.
     * @param[in] category  Category of the resource.
     */
    void
    incResDestroyed(uint32 /*category*/) {
      ++m_data.numObjectsDestroyed;
    }

    /**
     * @brief Increments GPU resource read counter.
     * @param[in] category  Category of the resource.
     */
    void
    incResRead(uint32 /*category*/) {
      ++m_data.numResourceReads;
    }

    /**
     * @brief Increments GPU resource write counter.
     * @param[in] category  Category of the resource.
     */
    void
    incResWrite(uint32 /*category*/) {
      ++m_data.numResourceWrites;
    }

    /**
     * @brief Returns an object containing various rendering statistics.
     * @note  Do not modify the returned state unless you know what you are doing, it will change the actual internal object.
     */
    RenderStatsData&
    getData() {
      return m_data;
    }

   private:
    RenderStatsData m_data;
  };

#if GE_PROFILING_ENABLED
# define GE_INC_RENDER_STAT_CAT(Stat, Category)                               \
  RenderStats::instance().inc##Stat(static_cast<uint32>(Category))
# define GE_INC_RENDER_STAT(Stat)                                             \
  RenderStats::instance().inc##Stat()
# define GE_ADD_RENDER_STAT(Stat, Count)                                      \
  RenderStats::instance().add##Stat(Count)
#else
# define GE_INC_RENDER_STAT_CAT(Stat, Category)
# define GE_INC_RENDER_STAT(Stat)
# define GE_ADD_RENDER_STAT(Stat, Count)
#endif
}
