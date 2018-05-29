/*****************************************************************************/
/**
 * @file    geProfilingManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/28
 * @brief   Tracks CPU profiling info with each frame for sim and core threads.
 *
 * Tracks CPU profiling information with each frame for sim and core threads.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geProfilingManager.h"

#include <geMath.h>

namespace geEngineSDK {
  const uint32 ProfilingManager::NUM_SAVED_FRAMES = 200;

  ProfilingManager::ProfilingManager()
    : m_savedSimReports(nullptr),
      m_nextSimReportIdx(0),
      m_savedCoreReports(nullptr),
      m_nextCoreReportIdx(0) {
    m_savedSimReports = ge_newN<ProfilerReport, ProfilerAlloc>(NUM_SAVED_FRAMES);
    m_savedCoreReports = ge_newN<ProfilerReport, ProfilerAlloc>(NUM_SAVED_FRAMES);
  }

  ProfilingManager::~ProfilingManager() {
    if (nullptr != m_savedSimReports) {
      ge_deleteN<ProfilerReport, ProfilerAlloc>(m_savedSimReports, NUM_SAVED_FRAMES);
    }

    if (nullptr != m_savedCoreReports) {
      ge_deleteN<ProfilerReport, ProfilerAlloc>(m_savedCoreReports, NUM_SAVED_FRAMES);
    }
  }

  void
  ProfilingManager::_update() {
#if GE_PROFILING_ENABLED
    m_savedSimReports[m_nextSimReportIdx].cpuReport = g_profilerCPU().generateReport();
    g_profilerCPU().reset();
    m_nextSimReportIdx = (m_nextSimReportIdx + 1) % NUM_SAVED_FRAMES;
#endif
  }

  void
  ProfilingManager::_updateCore() {
#if GE_PROFILING_ENABLED
    Lock lock(m_sync);
    m_savedCoreReports[m_nextCoreReportIdx].cpuReport = g_profilerCPU().generateReport();
    g_profilerCPU().reset();
    m_nextCoreReportIdx = (m_nextCoreReportIdx + 1) % NUM_SAVED_FRAMES;
#endif
  }

  const ProfilerReport&
  ProfilingManager::getReport(PROFILED_THREAD::E eThread, uint32 idx) const {
    idx = Math::clamp(idx, 0U, static_cast<uint32>(NUM_SAVED_FRAMES - 1));

    if (PROFILED_THREAD::kCore == eThread) {
      Lock lock(m_sync);
      uint32 reportIdx = m_nextCoreReportIdx + static_cast<uint32>(
        static_cast<int32>(NUM_SAVED_FRAMES) - (static_cast<int32>(idx) + 1));
      reportIdx = (reportIdx) % NUM_SAVED_FRAMES;

      return m_savedCoreReports[reportIdx];
    }

    uint32 reportIdx = m_nextSimReportIdx + static_cast<uint32>(
      static_cast<int32>(NUM_SAVED_FRAMES) - (static_cast<int32>(idx) + 1));
    reportIdx = (reportIdx) % NUM_SAVED_FRAMES;

    return m_savedSimReports[reportIdx];
  }

  ProfilingManager&
  g_profiler() {
    return ProfilingManager::instance();
  }
}
