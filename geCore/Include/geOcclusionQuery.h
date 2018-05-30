/*****************************************************************************/
/**
 * @file    geOcclusionQuery.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   Query that counts number of samples rendered by the GPU while the
 *          query is active.
 *
 * Represents a query that counts number of samples rendered by the GPU while
 * the query is active.
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

namespace geEngineSDK {
  namespace geCoreThread {
    /**
     * @brief Represents a query that counts number of samples rendered by the
     *        GPU while the query is active.
     * @note  Core thread only.
     */
    class GE_CORE_EXPORT OcclusionQuery
    {
     public:
      OcclusionQuery(bool binary);
      virtual ~OcclusionQuery() = default;

      /**
       * @brief Starts the query. Any draw calls after this call will have any
       *        rendered samples counted in the query.
       * @note  Place any commands you want to measure after this call.
       *        Call end() when done.
       */
      virtual void
      begin(const SPtr<CommandBuffer>& cb = nullptr) = 0;

      /**
       * @brief Stops the query.
       * @note  Be aware that queries are executed on the GPU and the results
       *        will not be immediately available.
       */
      virtual void
      end(const SPtr<CommandBuffer>& cb = nullptr) = 0;

      /**
       * @brief Check if GPU has processed the query.
       */
      virtual bool
      isReady() const = 0;

      /**
       * @brief Returns the number of samples that passed the depth and stencil
       *        test between query start and end.
       * @note  If the query is binary, this will return 0 or 1.
       *        1 meaning one or more samples were rendered, but will not give
       *        you the exact count.
       */
      virtual uint32
      getNumSamples() = 0;

      /**
       * @brief Triggered when the query has completed. Argument provided is
       *        the number of samples counted by the query.
       */
      Event<void(uint32)> onComplete;

      /**
       * @brief Creates a new occlusion query.
       * @param[in] binary    If query is binary it will not give you an exact
       *                      count of samples rendered, but will instead just
       *                      return 0 (no samples were rendered)
       *                      or 1 (one or more samples were rendered).
       *                      Binary queries can return sooner as they
       *                      potentially do not need to wait until all of the
       *                      geometry is rendered.
       * @param[in] deviceIdx Index of the GPU device to create the query on.
       */
      static SPtr<OcclusionQuery>
      create(bool binary, uint32 deviceIdx = 0);

     protected:
      friend class QueryManager;

      /**
       * @brief Returns true if it has still not been completed by the GPU.
       */
      bool
      isActive() const {
        return m_active;
      }

      void
      setActive(bool active) {
        m_active = active;
      }

     protected:
      bool m_active;
      bool m_binary;
    };
  }
}
