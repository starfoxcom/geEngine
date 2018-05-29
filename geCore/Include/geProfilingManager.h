/*****************************************************************************/
/**
 * @file    geProfilingManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/28
 * @brief   Tracks CPU profiling info with each frame for sim and core threads.
 *
 * Tracks CPU profiling information with each frame for sim and core threads.
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
#include "geProfilerCPU.h"

#include <geModule.h>

namespace geEngineSDK {
  /**
   * @brief Contains data about a profiling session.
   */
  struct ProfilerReport
  {
    CPUProfilerReport cpuReport;
  };

  /**
   * @brief Type of thread used by the profiler.
   */
  namespace PROFILED_THREAD {
    enum E {
      kSim,
      kCore
    };
  }

  /**
   * @brief Tracks CPU profiling information with each frame for sim and core
   *        threads.
   * @note  Sim thread only unless specified otherwise.
   */
  class GE_CORE_EXPORT ProfilingManager : public Module<ProfilingManager>
  {
   public:
    ProfilingManager();
    ~ProfilingManager();

    /**
     * @brief Called every frame.
     */
    void
    _update();

    /**
     * @brief Called every frame from the core thread.
     * @note  Core thread only.
     */
    void
    _updateCore();

    /**
     * @brief Returns a profiler report for the specified frame, for the
     *        specified thread.
     * @param[in] thread  Thread for which to retrieve the profiler report.
     * @param[in] idx     Profiler report index, ranging [0, NUM_SAVED_FRAMES].
     *                    0 always returns the latest report. Increasing
     *                    indexes return reports for older and older frames.
     *                    Out of range  indexes will be clamped.
     * @note  Profiler reports get updated every frame. Oldest reports that no
     *        longer fit in the saved reports buffer are discarded.
     */
    const ProfilerReport&
    getReport(PROFILED_THREAD::E eThread, uint32 idx = 0) const;

   private:
    static const uint32 NUM_SAVED_FRAMES;
    ProfilerReport* m_savedSimReports;
    uint32 m_nextSimReportIdx;

    ProfilerReport* m_savedCoreReports;
    uint32 m_nextCoreReportIdx;

    mutable Mutex m_sync;
  };

  /**
   * @brief Easy way to access ProfilingManager.
   */
  GE_CORE_EXPORT ProfilingManager&
  g_profiler();
}
