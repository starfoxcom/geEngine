/*****************************************************************************/
/**
 * @file    geCoreObjectManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/12/06
 * @brief   Manager that keeps track of all active CoreObject's.
 *
 * Manager that keeps track of all active CoreObject's.
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
#include "geCoreObjectCore.h"
#include <geModule.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT CoreObjectManager : public Module<CoreObjectManager>
  {
    /**
     * @brief Stores dirty data that is to be transferred from sim thread to
     *        core thread part of a CoreObject, for a single object.
     */
    struct CoreStoredSyncObjData
    {
      CoreStoredSyncObjData() : internalId(0) {}
      CoreStoredSyncObjData(const SPtr<geCoreThread::CoreObject> destObj,
                            uint64 internalId,
                            const CoreSyncData& syncData)
        : destinationObj(destObj),
          syncData(syncData),
          internalId(internalId)
      {}

      SPtr<geCoreThread::CoreObject> destinationObj;
      CoreSyncData syncData;
      uint64 internalId;
    };

    /**
     * @brief Stores dirty data that is to be transferred from sim thread to
     *        core thread part of a CoreObject, for all dirty objects in one
     *        frame.
     */
    struct CoreStoredSyncData
    {
      FrameAlloc* alloc = nullptr;
      Vector<CoreStoredSyncObjData> entries;
    };

    /**
     * @brief Contains information about a dirty CoreObject that requires
     *        syncing to the core thread.
     */
    struct DirtyObjectData
    {
      CoreObject* object;
      int32 syncDataId;
    };

   public:
    CoreObjectManager() = default;
    ~CoreObjectManager();

    /**
     * @brief Generates a new unique ID for a core object.
     */
    uint64
    generateId();

    /**
     * @brief Registers a new CoreObject notifying the manager the object	is
     *        created.
     */
    void
    registerObject(CoreObject* object);

    /**
     * @brief Unregisters a CoreObject notifying the manager the object is
     *        destroyed.
     */
    void
    unregisterObject(CoreObject* object);

    /**
     * @brief Notifies the system that a CoreObject is dirty and needs to be
     *        synced with the core thread.
     */
    void
    notifyCoreDirty(CoreObject* object);

    /**
     * @brief Notifies the system that CoreObject dependencies are dirty and
     *        should be updated.
     */
    void
    notifyDependenciesDirty(CoreObject* object);

    /**
     * @brief Synchronizes all dirty CoreObjects with the core thread. Their
     *        dirty data will be allocated using the global frame allocator and
     *        then queued for update using the core thread queue for the
     *        calling thread.
     * @note  Sim thread only.
     * @note  This is an @ref asyncMethod "asynchronous method".
     */
    void
    syncToCore();

    /**
     * @brief Synchronizes an individual dirty CoreObject with the core thread.
     *        Its dirty data will be allocated using the global frame allocator
     *        and then queued for update the core thread queue for the calling
     *        thread.
     * @note  Sim thread only.
     * @note  This is an @ref asyncMethod "asynchronous method".
     */
    void
    syncToCore(CoreObject* object);

   private:
    /**
     * @brief Stores all syncable data from dirty core objects into memory
     *        allocated by the provided allocator. Additional meta-data is
     *        stored internally to be used by call to syncUpload().
     * @param[in] allocator Allocator to use for allocating memory for stored
     *            data.
     * @note  Sim thread only.
     * @note  Must be followed by a call to syncUpload() with the same type.
     */
    void
    syncDownload(FrameAlloc* allocator);

    /**
     * @brief Copies all the data stored by previous call to syncDownload() 
     *        into core thread versions of CoreObjects.
     * @note  Core thread only.
     * @note  Must be preceded by a call to syncDownload().
     */
    void
    syncUpload();

    /**
     * @brief Updates the cached list of dependencies and dependants for the
     *        specified object.
     * @param[in] object        Update to update dependencies for.
     * @param[in] dependencies  New set of dependencies, or null to clear all
     *                          dependencies.
     */
    void
    updateDependencies(CoreObject* object, Vector<CoreObject*>* dependencies);

    uint64 m_nextAvailableID = 1;
    Map<uint64, CoreObject*> m_objects;
    Map<uint64, DirtyObjectData> m_dirtyObjects;
    Map<uint64, Vector<CoreObject*>> m_dependencies;
    Map<uint64, Vector<CoreObject*>> m_dependants;

    Vector<CoreStoredSyncObjData> m_destroyedSyncData;
    List<CoreStoredSyncData> m_coreSyncData;

    Mutex m_objectsMutex;
  };
}
