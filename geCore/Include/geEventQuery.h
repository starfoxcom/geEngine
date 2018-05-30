/*****************************************************************************/
/**
 * @file    geEventQuery.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   GPU query that gets triggered when GPU starts processing the query.
 *
 * Represents a GPU query that gets triggered when GPU starts processing the
 * query.
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
  namespace geCoreThread
  {
    /**
     * @brief Represents a GPU query that gets triggered when GPU starts
     *        processing the query.
     * @note  Normally GPU will have many commands in its command buffer. When
     *        begin() is called it is placed in that command buffer. Once the
     *        buffer empties and GPU reaches the EventQuery command, the query
     *        callback is triggered.
     * @note  Core thread only.
     */
    class GE_CORE_EXPORT EventQuery
    {
     public:
      EventQuery() : m_active(false) {}
      virtual ~EventQuery() = default;

      /**
       * @brief Starts the query.
       * @param[in] cb  Optional command buffer to queue the operation on.
       *                If not provided operation is executed on the main
       *                command buffer. Otherwise it is executed when
       *                RenderAPI::executeCommands() is called. Buffer must
       *                support graphics or compute operations.
       * @note  Once the query is started you may poll isReady() method to
       *        check when query has finished, or you may hook up an
       *        #onTriggered callback and be notified that way.
       */
      virtual void
      begin(const SPtr<CommandBuffer>& cb = nullptr) = 0;

      /**
       * @brief Check if GPU has processed the query.
       */
      virtual bool
      isReady() const = 0;

      /**
       * @brief Triggered when GPU starts processing the query.
       */
      Event<void()> onTriggered;

      /**
       * @brief Creates a new query, but does not schedule it on GPU.
       * @param[in] deviceIdx Index of the GPU device to create the query on.
       */
      static SPtr<EventQuery>
      create(uint32 deviceIdx = 0);

     protected:
      friend class QueryManager;

      /**
       * @brief Returns true if the has still not been completed by the GPU.
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
    };
  }
}
