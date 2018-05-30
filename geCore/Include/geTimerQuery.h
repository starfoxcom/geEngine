/*****************************************************************************/
/**
 * @file    geTimerQuery.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   GPU query that measures execution time of GPU operations.
 *
 * Represents a GPU query that measures execution time of GPU operations.
 * The query will measure any GPU operations that take place between its
 * begin() and end() calls.
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
     * @brief Represents a GPU query that measures execution time of GPU operations. The query will measure any GPU operations that take place between its begin() and end() calls.
     * @note	Core thread only.
     */
    class GE_CORE_EXPORT TimerQuery
    {
     public:
      virtual ~TimerQuery() = default;

      /**
       * @brief Starts the counter.
       * @param[in] cb  Optional command buffer to queue the operation on. If not provided operation is executed on the main command buffer. Otherwise it is executed when RenderAPI::executeCommands() is called. Buffer must support graphics or compute operations.
       * @note  Place any commands you want to measure after this call. Call "end" when done.
       */
      virtual void begin(const SPtr<CommandBuffer>& cb = nullptr) = 0;

      /**
       * @brief Stops the counter.
       * @param[in] cb  Command buffer that was provided to the last begin() operation (if any).
       */
      virtual void end(const SPtr<CommandBuffer>& cb = nullptr) = 0;

      /**
       * @brief Check if GPU has processed the query.
       */
      virtual bool isReady() const = 0;

      /**
       * @brief Returns the time it took for the query to execute.
       * @return  The time milliseconds.
       * @note  Only valid after isReady() returns true.
       */
      virtual float getTimeMs() = 0;

      /**
       * @brief Triggered when GPU processes the query. As a parameter it provides query duration in milliseconds.
       */
      Event<void(float)> onTriggered;

      /**
       * @brief Creates a new query, but does not schedule it on GPU.
       * @param[in] deviceIdx Index of the GPU device to create the query on.
       */
      static SPtr<TimerQuery> create(uint32 deviceIdx = 0);

     protected:
      friend class QueryManager;

      /**
       * @brief Returns true if the has still not been completed by the GPU.
       */
      bool isActive() const { return m_active; }
      void setActive(bool active) { m_active = active; }

    protected:
      bool m_active;
    };
  }
}
