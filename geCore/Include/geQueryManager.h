/*****************************************************************************/
/**
 * @file    geQueryManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   Handles creation and destruction of GPU queries.
 *
 * Handles creation and destruction of GPU queries.
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
#include "geEventQuery.h"

#include <geModule.h>

namespace geEngineSDK {
  namespace geCoreThread {
    /**
     * @brief Handles creation and destruction of GPU queries.
     * @note  Core thread only.
     */
    class GE_CORE_EXPORT QueryManager : public Module<QueryManager>
    {
     public:
      QueryManager() = default;
      ~QueryManager();

      /**
       * @brief Creates a new event query that allows you to get notified when
       *        GPU starts executing the query.
       * @param[in] deviceIdx Index of the GPU device to create the query on.
       */
      virtual SPtr<EventQuery>
      createEventQuery(uint32 deviceIdx = 0) const = 0;

      /**
       * @brief Creates a new timer query that allows you to get notified of
       *        how much time has passed between query start and end.
       * @param[in] deviceIdx Index of the GPU device to create the query on.
       */
      virtual SPtr<TimerQuery>
      createTimerQuery(uint32 deviceIdx = 0) const = 0;

      /**
       * @brief Creates a new occlusion query that allows you to know how many
       *        fragments were rendered between query start and end.
       * @param[in] binary    If query is binary it will not give you an exact
                              count of fragments rendered, but will instead
                              just return 0 (no fragments were rendered) or 1
                              (one or more fragments were rendered).
                              Binary queries can return sooner as they
                              potentially do not need to wait until all of the
                              geometry is rendered.
       * @param[in] deviceIdx Index of the GPU device to create the query on.
       */
      virtual SPtr<OcclusionQuery>
      createOcclusionQuery(bool binary, uint32 deviceIdx = 0) const = 0;

      /**
       * @brief Triggers completed queries. Should be called every frame.
       */
      void
      _update();

     protected:
      friend class EventQuery;
      friend class TimerQuery;
      friend class OcclusionQuery;

      /**
       * @brief Deletes an Event query.
       *        Always use this method and don't delete them manually.
       *        Actual deletion will be delayed until next update.
       */
      static void
      deleteEventQuery(EventQuery* query);

      /**
       * @brief Deletes a Timer query.
       *        Always use this method and don't delete them manually.
       *        Actual deletion will be delayed until next update.
       */
      static void
      deleteTimerQuery(TimerQuery* query);

      /**
       * @brief Deletes an Occlusion query.
       *        Always use this method and don't delete them manually.
       *        Actual deletion will be delayed until next update.
       */
      static void
      deleteOcclusionQuery(OcclusionQuery* query);

      /**
       * @brief Deletes any queued queries.
       */
      void processDeletedQueue();

     protected:
      mutable Vector<EventQuery*> m_eventQueries;
      mutable Vector<TimerQuery*> m_timerQueries;
      mutable Vector<OcclusionQuery*> m_occlusionQueries;

      mutable Vector<EventQuery*> m_deletedEventQueries;
      mutable Vector<TimerQuery*> m_deletedTimerQueries;
      mutable Vector<OcclusionQuery*> m_deletedOcclusionQueries;
    };
  }
}
