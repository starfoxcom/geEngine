/*****************************************************************************/
/**
 * @file    geResources.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/30
 * @brief   Manager for dealing with all engine resources. It allows you to
 *          save new resources and load existing ones.
 *
 * Manager for dealing with all engine resources. It allows you to save new
 * resources and load existing ones.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geResources.h"
#include "geResource.h"
#include "geResourceManifest.h"
#include "geUtility.h"
#include "geSavedResourceData.h"
#include "geResourceListenerManager.h"

#include <geException.h>
#include <geFileSerializer.h>
#include <geFileSystem.h>
#include <geTaskScheduler.h>
#include <geUUID.h>
#include <geDebug.h>
#include <geMemorySerializer.h>
#include <geCompression.h>
#include <geDataStream.h>
#include <geBinarySerializer.h>

namespace geEngineSDK {
  Resources::Resources() {
    m_defaultResourceManifest = ResourceManifest::create("Default");
    m_resourceManifests.push_back(m_defaultResourceManifest);
  }

  Resources::~Resources() {
    //Unload and invalidate all resources
    UnorderedMap<UUID, LoadedResourceData> loadedResourcesCopy;
    {
      Lock lock(m_loadedResourceMutex);
      loadedResourcesCopy = m_loadedResources;
    }

    for (auto& loadedResourcePair : loadedResourcesCopy) {
      destroy(loadedResourcePair.second.resource);
    }
  }

  HResource
  Resources::load(const Path& filePath, ResourceLoadFlags loadFlags) {
    if (!FileSystem::isFile(filePath)) {
      LOGWRN("Cannot load resource. Specified file: " + filePath.toString() + " doesn't exist.");
      return HResource();
    }

    UUID uuid;
    bool foundUUID = getUUIDFromFilePath(filePath, uuid);

    if (!foundUUID) {
      uuid = UUIDGenerator::generateRandom();
    }

    return loadInternal(uuid, filePath, true, loadFlags);
  }

  HResource
  Resources::load(const WeakResourceHandle<Resource>& handle, ResourceLoadFlags loadFlags) {
    if (nullptr == handle.m_data) {
      return HResource();
    }

    UUID uuid = handle.getUUID();
    return loadFromUUID(uuid, false, loadFlags);
  }

  HResource
  Resources::loadAsync(const Path& filePath, ResourceLoadFlags loadFlags) {
    if (!FileSystem::isFile(filePath)) {
      LOGWRN("Cannot load resource. Specified file: " + filePath.toString() + " doesn't exist.");
      return HResource();
    }

    UUID uuid;
    bool foundUUID = getUUIDFromFilePath(filePath, uuid);

    if (!foundUUID) {
      uuid = UUIDGenerator::generateRandom();
    }

    return loadInternal(uuid, filePath, false, loadFlags);
  }

  HResource
  Resources::loadFromUUID(const UUID& uuid, bool async, ResourceLoadFlags loadFlags) {
    Path filePath;

    // Default manifest is at 0th index but all other take priority since Default manifest could contain obsolete data. 
    for (auto iter = m_resourceManifests.rbegin(); iter != m_resourceManifests.rend(); ++iter) {
      if ((*iter)->uuidToFilePath(uuid, filePath)) {
        break;
      }
    }

    return loadInternal(uuid, filePath, !async, loadFlags);
  }

  HResource
  Resources::loadInternal(const UUID& uuid, const Path& filePath, bool synchronous, ResourceLoadFlags loadFlags) {
    HResource outputResource;

    bool alreadyLoading = false;
    bool loadInProgress = false;
    {
      // Check if resource is already being loaded on a worker thread
      Lock inProgressLock(m_inProgressResourcesMutex);
      auto iterFind2 = m_inProgressResources.find(uuid);
      if (iterFind2 != m_inProgressResources.end()) {
        LoadedResourceData& resData = iterFind2->second->resData;
        outputResource = resData.resource.lock();

        if (loadFlags.isSet(ResourceLoadFlag::KeepInternalRef)) {
          resData.numInternalRefs++;
          outputResource.addInternalRef();
        }

        alreadyLoading = true;
        loadInProgress = true;
      }

      // Previously being loaded as async but now we want it synced, so we wait
      if (loadInProgress && synchronous) {
        outputResource.blockUntilLoaded();
      }

      if (!alreadyLoading) {
        Lock loadedLock(m_loadedResourceMutex);
        auto iterFind = m_loadedResources.find(uuid);
        if (iterFind != m_loadedResources.end()) // Resource is already loaded
        {
          LoadedResourceData& resData = iterFind->second;
          outputResource = resData.resource.lock();

          if (loadFlags.isSet(ResourceLoadFlag::KeepInternalRef)) {
            resData.numInternalRefs++;
            outputResource.addInternalRef();
          }

          alreadyLoading = true;
        }
      }
    }

    // Not loaded and not in progress, start loading of new resource
    // (or if already loaded or in progress, load any dependencies)
    if (!alreadyLoading) {
      // Check if the handle already exists
      Lock lock(m_loadedResourceMutex);
      auto iterFind = m_handles.find(uuid);
      if (iterFind != m_handles.end()) {
        outputResource = iterFind->second.lock();
      }
      else {
        outputResource = HResource(uuid);
        m_handles[uuid] = outputResource.getWeak();
      }
    }

    // We have nowhere to load from, warn and complete load if a file path was provided,
    // otherwise pass through as we might just want to load from memory. 
    if (filePath.isEmpty()) {
      if (!alreadyLoading) {
        LOGWRN_VERBOSE("Cannot load resource. Resource with UUID '" + UUID + "' doesn't exist.");

        // Complete the load as that the depedency counter is properly reduced, in case this 
        // is a dependency of some other resource.
        loadComplete(outputResource);
        return outputResource;
      }
    }
    else if (!FileSystem::isFile(filePath)) {
      LOGWRN_VERBOSE("Cannot load resource. Specified file: " + filePath.toString() + " doesn't exist.");

      // Complete the load as that the depedency counter is properly reduced, in case this 
      // is a dependency of some other resource.
      loadComplete(outputResource);
      assert(!loadInProgress); // Resource already being loaded but we can't find its path now?

      return outputResource;
    }

    // Load dependency data if a file path is provided
    SPtr<SavedResourceData> savedResourceData;
    if (!filePath.isEmpty()) {
      FileDecoder fs(filePath);
      savedResourceData = std::static_pointer_cast<SavedResourceData>(fs.decode());
    }

    // If already loading keep the old load operation active, otherwise create a new one
    if (!alreadyLoading) {
      {
        Lock lock(m_inProgressResourcesMutex);

        ResourceLoadData* loadData = ge_new<ResourceLoadData>(outputResource.getWeak(), 0);
        m_inProgressResources[uuid] = loadData;
        loadData->resData = outputResource.getWeak();

        if (loadFlags.isSet(ResourceLoadFlag::KeepInternalRef)) {
          loadData->resData.numInternalRefs++;
          outputResource.addInternalRef();
        }

        loadData->remainingDependencies = 1;
        loadData->notifyImmediately = synchronous; // Make resource listener trigger before exit if loading synchronously

                                                   // Register dependencies and count them so we know when the resource is fully loaded
        if (loadFlags.isSet(ResourceLoadFlag::LoadDependencies) && savedResourceData != nullptr) {
          for (auto& dependency : savedResourceData->getDependencies()) {
            if (dependency != uuid) {
              m_dependantLoads[dependency].push_back(loadData);
              loadData->remainingDependencies++;
            }
          }
        }
      }

      if (loadFlags.isSet(ResourceLoadFlag::LoadDependencies) && savedResourceData != nullptr) {
        const Vector<UUID>& dependencyUUIDs = savedResourceData->getDependencies();
        uint32 numDependencies = (uint32)dependencyUUIDs.size();
        Vector<HResource> dependencies(numDependencies);

        ResourceLoadFlags depLoadFlags = ResourceLoadFlag::LoadDependencies;
        if (loadFlags.isSet(ResourceLoadFlag::KeepSourceData))
          depLoadFlags |= ResourceLoadFlag::KeepSourceData;

        for (uint32 i = 0; i < numDependencies; i++) {
          dependencies[i] = loadFromUUID(dependencyUUIDs[i], !synchronous, depLoadFlags);
        }

        // Keep dependencies alive until the parent is done loading
        {
          Lock lock(m_inProgressResourcesMutex);

          // At this point the resource is guaranteed to still be in-progress, so it's safe to update its dependency list
          m_inProgressResources[uuid]->dependencies = dependencies;
        }
      }
    }
    else if (loadFlags.isSet(ResourceLoadFlag::LoadDependencies) && savedResourceData != nullptr) // Queue dependencies in case they aren't already loaded
    {
      const Vector<UUID>& dependencies = savedResourceData->getDependencies();
      if (!dependencies.empty()) {
        {
          Lock lock(m_inProgressResourcesMutex);

          ResourceLoadData* loadData = nullptr;

          auto iterFind = m_inProgressResources.find(uuid);
          if (iterFind == m_inProgressResources.end()) // Fully loaded
          {
            loadData = ge_new<ResourceLoadData>(outputResource.getWeak(), 0);
            loadData->resData = outputResource.getWeak();
            loadData->remainingDependencies = 0;
            loadData->notifyImmediately = synchronous; // Make resource listener trigger before exit if loading synchronously

            m_inProgressResources[uuid] = loadData;
          }
          else {
            loadData = iterFind->second;
          }

          // Register dependencies and count them so we know when the resource is fully loaded
          for (auto& dependency : dependencies)
          {
            if (dependency != uuid)
            {
              bool registerDependency = true;

              auto iterFind2 = m_dependantLoads.find(dependency);
              if (iterFind2 != m_dependantLoads.end())
              {
                Vector<ResourceLoadData*>& dependantData = iterFind2->second;
                auto iterFind3 = std::find_if(dependantData.begin(), dependantData.end(),
                  [&](ResourceLoadData* x)
                {
                  return x->resData.resource.getUUID() == outputResource.getUUID();
                });

                registerDependency = iterFind3 == dependantData.end();
              }

              if (registerDependency)
              {
                m_dependantLoads[dependency].push_back(loadData);
                loadData->remainingDependencies++;
                loadData->dependencies.push_back(_getResourceHandle(dependency));
              }
            }
          }
        }

        ResourceLoadFlags depLoadFlags = ResourceLoadFlag::LoadDependencies;
        if (loadFlags.isSet(ResourceLoadFlag::KeepSourceData))
          depLoadFlags |= ResourceLoadFlag::KeepSourceData;

        for (auto& dependency : dependencies)
          loadFromUUID(dependency, !synchronous, depLoadFlags);
      }
    }

    // Actually start the file read operation if not already loaded or in progress
    if (!alreadyLoading && !filePath.isEmpty())
    {
      // Synchronous or the resource doesn't support async, read the file immediately
      if (synchronous || !savedResourceData->allowAsyncLoading())
      {
        loadCallback(filePath, outputResource, loadFlags.isSet(ResourceLoadFlag::KeepSourceData));
      }
      else // Asynchronous, read the file on a worker thread
      {
        String fileName = filePath.getFilename();
        String taskName = "Resource load: " + fileName;

        bool keepSourceData = loadFlags.isSet(ResourceLoadFlag::KeepSourceData);
        SPtr<Task> task = Task::create(taskName,
          std::bind(&Resources::loadCallback, this, filePath, outputResource, keepSourceData));
        TaskScheduler::instance().addTask(task);
      }
    }
    else // File already loaded or in progress
    {
      // Complete the load unless its in progress in which case we wait for its worker thread to complete it.
      // In case file is already loaded this will only decrement dependency count in case this resource is a dependency.
      if (!loadInProgress)
        loadComplete(outputResource);
      else
      {
        // In case loading finished in the meantime we cannot be sure at what point ::loadComplete was triggered,
        // so trigger it manually so that the dependency count is properly decremented in case this resource
        // is a dependency.
        Lock lock(m_loadedResourceMutex);
        auto iterFind = m_loadedResources.find(uuid);
        if (iterFind != m_loadedResources.end())
          loadComplete(outputResource);
      }
    }

    return outputResource;
  }

  SPtr<Resource> Resources::loadFromDiskAndDeserialize(const Path& filePath, bool loadWithSaveData)
  {
    // Note: Called asynchronously over multiple resources this will cause performance issues on hard drives as they
    // work best when they are reading one thing at a time, and it won't have benefits on an SSD either. Think about
    // executing all file reads on a single thread, while decompression and similar operations can execute on multiple.

    SPtr<DataStream> stream = FileSystem::openFile(filePath, true);
    if (stream == nullptr)
      return nullptr;

    if (stream->size() > std::numeric_limits<uint32>::max())
    {
      GE_EXCEPT(InternalErrorException,
        "File size is larger that UINT32 can hold. Ask a programmer to use a bigger data type.");
    }

    UnorderedMap<String, uint64> params;
    if (loadWithSaveData)
      params["keepSourceData"] = 1;

    // Read meta-data
    SPtr<SavedResourceData> metaData;
    {
      if (!stream->isEOF())
      {
        uint32 objectSize = 0;
        stream->read(&objectSize, sizeof(objectSize));

        BinarySerializer bs;
        metaData = std::static_pointer_cast<SavedResourceData>(bs.decode(stream, objectSize, params));
      }
    }

    // Read resource data
    SPtr<IReflectable> loadedData;
    {
      if (metaData && !stream->isEOF())
      {
        uint32 objectSize = 0;
        stream->read(&objectSize, sizeof(objectSize));

        if (metaData->getCompressionMethod() != 0)
          stream = Compression::decompress(stream);

        BinarySerializer bs;
        loadedData = std::static_pointer_cast<SavedResourceData>(bs.decode(stream, objectSize, params));
      }
    }

    if (loadedData == nullptr)
    {
      LOGERR("Unable to load resource at path \"" + filePath.toString() + "\"");
    }
    else
    {
      if (!loadedData->isDerivedFrom(Resource::getRTTIStatic()))
        GE_EXCEPT(InternalErrorException, "Loaded class doesn't derive from Resource.");
    }

    SPtr<Resource> resource = std::static_pointer_cast<Resource>(loadedData);
    return resource;
  }

  void Resources::release(ResourceHandleBase& resource)
  {
    const UUID& uuid = resource.getUUID();

    {
      bool loadInProgress = false;

      Lock inProgressLock(m_inProgressResourcesMutex);
      auto iterFind2 = m_inProgressResources.find(uuid);
      if (iterFind2 != m_inProgressResources.end())
        loadInProgress = true;

      // Technically we should be able to just cancel a load in progress instead of blocking until it finishes.
      // However that would mean the last reference could get lost on whatever thread did the loading, which
      // isn't something that's supported. If this ends up being a problem either make handle counting atomic
      // or add a separate queue for objects destroyed from the load threads.
      if (loadInProgress)
        resource.blockUntilLoaded();

      {
        Lock loadedLock(m_loadedResourceMutex);
        auto iterFind = m_loadedResources.find(uuid);
        if (iterFind != m_loadedResources.end()) // Resource is already loaded
        {
          LoadedResourceData& resData = iterFind->second;

          assert(resData.numInternalRefs > 0);
          resData.numInternalRefs--;
          resource.removeInternalRef();

          return;
        }
      }
    }
  }

  void Resources::unloadAllUnused()
  {
    Vector<HResource> resourcesToUnload;

    {
      Lock lock(m_loadedResourceMutex);
      for (auto iter = m_loadedResources.begin(); iter != m_loadedResources.end(); ++iter)
      {
        const LoadedResourceData& resData = iter->second;

        if (resData.resource.m_data->m_refCount == resData.numInternalRefs) // Only internal references exist, free it
          resourcesToUnload.push_back(resData.resource.lock());
      }
    }

    // Note: When unloading multiple resources it's possible that unloading one will also unload
    // another resource in "resourcesToUnload". This is fine because "unload" deals with invalid
    // handles gracefully.
    for (auto iter = resourcesToUnload.begin(); iter != resourcesToUnload.end(); ++iter)
    {
      release(*iter);
    }
  }

  void Resources::destroy(ResourceHandleBase& resource)
  {
    if (resource.m_data == nullptr)
      return;

    const UUID& uuid = resource.getUUID();
    if (!resource.isLoaded(false))
    {
      bool loadInProgress = false;
      {
        Lock lock(m_inProgressResourcesMutex);
        auto iterFind2 = m_inProgressResources.find(uuid);
        if (iterFind2 != m_inProgressResources.end())
          loadInProgress = true;
      }

      if (loadInProgress) // If it's still loading wait until that finishes
        resource.blockUntilLoaded();
      else
        return; // Already unloaded
    }

    // Notify external systems before we actually destroy it
    onResourceDestroyed(uuid);
    resource.m_data->m_ptr->destroy();

    {
      Lock lock(m_loadedResourceMutex);

      auto iterFind = m_loadedResources.find(uuid);
      if (iterFind != m_loadedResources.end())
      {
        LoadedResourceData& resData = iterFind->second;
        while (resData.numInternalRefs > 0)
        {
          resData.numInternalRefs--;
          resData.resource.removeInternalRef();
        }

        m_loadedResources.erase(iterFind);
      }
      else
      {
        assert(false); // This should never happen but in case it does fail silently in release mode
      }
    }

    resource.setHandleData(nullptr, uuid);
  }

  void Resources::save(const HResource& resource, const Path& filePath, bool overwrite, bool compress)
  {
    if (resource == nullptr)
      return;

    if (!resource.isLoaded(false))
    {
      bool loadInProgress = false;
      {
        Lock lock(m_inProgressResourcesMutex);
        auto iterFind2 = m_inProgressResources.find(resource.getUUID());
        if (iterFind2 != m_inProgressResources.end())
          loadInProgress = true;
      }

      if (loadInProgress) // If it's still loading wait until that finishes
        resource.blockUntilLoaded();
      else
        return; // Nothing to save
    }

    bool fileExists = FileSystem::isFile(filePath);
    if (fileExists)
    {
      if (overwrite)
        FileSystem::remove(filePath);
      else
      {
        LOGERR("Another file exists at the specified location. Not saving.");
        return;
      }
    }

    if (!resource->m_keepSourceData)
    {
      LOGWRN("Saving a resource that was created/loaded without ResourceLoadFlag::KeepSourceData. Some data might "
        "not be available for saving. File path: " + filePath.toString());
    }

    m_defaultResourceManifest->registerResource(resource.getUUID(), filePath);

    Vector<ResourceDependency> dependencyList = Utility::findResourceDependencies(*resource.get());
    Vector<UUID> dependencyUUIDs(dependencyList.size());
    for (uint32 i = 0; i < (uint32)dependencyList.size(); i++)
      dependencyUUIDs[i] = dependencyList[i].resource.getUUID();

    uint32 compressionMethod = (compress && resource->isCompressible()) ? 1 : 0;
    SPtr<SavedResourceData> resourceData = ge_shared_ptr_new<SavedResourceData>(dependencyUUIDs,
      resource->allowAsyncLoading(), compressionMethod);

    Path parentDir = filePath.getDirectory();
    if (!FileSystem::exists(parentDir))
      FileSystem::createDir(parentDir);

    std::ofstream stream;
    stream.open(filePath.toPlatformString().c_str(), std::ios::out | std::ios::binary);
    if (stream.fail())
      LOGWRN("Failed to save file: \"" + filePath.toString() + "\". Error: " + strerror(errno) + ".");

    // Write meta-data
    {
      MemorySerializer ms;
      uint32 numBytes = 0;
      uint8* bytes = ms.encode(resourceData.get(), numBytes);

      stream.write((char*)&numBytes, sizeof(numBytes));
      stream.write((char*)bytes, numBytes);

      ge_free(bytes);
    }

    // Write object data
    {
      MemorySerializer ms;
      uint32 numBytes = 0;
      uint8* bytes = ms.encode(resource.get(), numBytes);

      SPtr<MemoryDataStream> objStream = ge_shared_ptr_new<MemoryDataStream>(bytes, numBytes);
      if (compressionMethod != 0)
      {
        SPtr<DataStream> srcStream = std::static_pointer_cast<DataStream>(objStream);
        objStream = Compression::compress(srcStream);
      }

      stream.write((char*)&numBytes, sizeof(numBytes));
      stream.write((char*)objStream->getPtr(), objStream->size());
    }

    stream.close();
    stream.clear();
  }

  void Resources::save(const HResource& resource, bool compress)
  {
    if (resource == nullptr)
      return;

    Path path;
    if (getFilePathFromUUID(resource.getUUID(), path))
      save(resource, path, true, compress);
  }

  void Resources::update(HResource& handle, const SPtr<Resource>& resource)
  {
    const UUID& uuid = handle.getUUID();
    handle.setHandleData(resource, uuid);

    {
      Lock lock(m_loadedResourceMutex);
      auto iterFind = m_loadedResources.find(uuid);
      if (iterFind == m_loadedResources.end())
      {
        LoadedResourceData& resData = m_loadedResources[uuid];
        resData.resource = handle.getWeak();
      }
    }

    onResourceModified(handle);
    ResourceListenerManager::instance().notifyListeners(uuid);
  }

  Vector<UUID> Resources::getDependencies(const Path& filePath)
  {
    SPtr<SavedResourceData> savedResourceData;
    if (!filePath.isEmpty())
    {
      FileDecoder fs(filePath);
      savedResourceData = std::static_pointer_cast<SavedResourceData>(fs.decode());
    }

    return savedResourceData->getDependencies();
  }

  void Resources::registerResourceManifest(const SPtr<ResourceManifest>& manifest)
  {
    if (manifest->getName() == "Default")
      return;

    auto findIter = std::find(m_resourceManifests.begin(), m_resourceManifests.end(), manifest);
    if (findIter == m_resourceManifests.end())
      m_resourceManifests.push_back(manifest);
    else
      *findIter = manifest;
  }

  void Resources::unregisterResourceManifest(const SPtr<ResourceManifest>& manifest)
  {
    if (manifest->getName() == "Default")
      return;

    auto findIter = std::find(m_resourceManifests.begin(), m_resourceManifests.end(), manifest);
    if (findIter != m_resourceManifests.end())
      m_resourceManifests.erase(findIter);
  }

  SPtr<ResourceManifest> Resources::getResourceManifest(const String& name) const
  {
    for (auto iter = m_resourceManifests.rbegin(); iter != m_resourceManifests.rend(); ++iter)
    {
      if (name == (*iter)->getName())
        return (*iter);
    }

    return nullptr;
  }

  bool Resources::isLoaded(const UUID& uuid, bool checkInProgress)
  {
    if (checkInProgress)
    {
      Lock inProgressLock(m_inProgressResourcesMutex);
      auto iterFind2 = m_inProgressResources.find(uuid);
      if (iterFind2 != m_inProgressResources.end())
      {
        return true;
      }

      {
        Lock loadedLock(m_loadedResourceMutex);
        auto iterFind = m_loadedResources.find(uuid);
        if (iterFind != m_loadedResources.end())
        {
          return true;
        }
      }
    }

    return false;
  }

  HResource Resources::_createResourceHandle(const SPtr<Resource>& obj)
  {
    UUID uuid = UUIDGenerator::generateRandom();
    return _createResourceHandle(obj, uuid);
  }

  HResource Resources::_createResourceHandle(const SPtr<Resource>& obj, const UUID& UUID)
  {
    HResource newHandle(obj, UUID);

    {
      Lock lock(m_loadedResourceMutex);

      LoadedResourceData& resData = m_loadedResources[UUID];
      resData.resource = newHandle.getWeak();
      m_handles[UUID] = newHandle.getWeak();
    }

    return newHandle;
  }

  HResource Resources::_getResourceHandle(const UUID& uuid)
  {
    Lock lock(m_loadedResourceMutex);
    auto iterFind3 = m_handles.find(uuid);
    if (iterFind3 != m_handles.end()) // Not loaded, but handle does exist
    {
      return iterFind3->second.lock();
    }

    // Create new handle
    HResource handle(uuid);
    m_handles[uuid] = handle.getWeak();

    return handle;
  }

  bool Resources::getFilePathFromUUID(const UUID& uuid, Path& filePath) const
  {
    for (auto iter = m_resourceManifests.rbegin(); iter != m_resourceManifests.rend(); ++iter)
    {
      if ((*iter)->uuidToFilePath(uuid, filePath))
        return true;
    }

    return false;
  }

  bool Resources::getUUIDFromFilePath(const Path& path, UUID& uuid) const
  {
    Path manifestPath = path;
    if (!manifestPath.isAbsolute())
      manifestPath.makeAbsolute(FileSystem::getWorkingDirectoryPath());

    for (auto iter = m_resourceManifests.rbegin(); iter != m_resourceManifests.rend(); ++iter)
    {
      if ((*iter)->filePathToUUID(manifestPath, uuid))
        return true;
    }

    return false;
  }

  void Resources::loadComplete(HResource& resource)
  {
    UUID uuid = resource.getUUID();

    ResourceLoadData* myLoadData = nullptr;
    bool finishLoad = true;
    Vector<ResourceLoadData*> dependantLoads;
    {
      Lock inProgresslock(m_inProgressResourcesMutex);

      auto iterFind = m_inProgressResources.find(uuid);
      if (iterFind != m_inProgressResources.end())
      {
        myLoadData = iterFind->second;
        finishLoad = myLoadData->remainingDependencies == 0;

        if (finishLoad)
          m_inProgressResources.erase(iterFind);
      }

      auto iterFind2 = m_dependantLoads.find(uuid);

      if (iterFind2 != m_dependantLoads.end())
        dependantLoads = iterFind2->second;

      if (finishLoad)
      {
        m_dependantLoads.erase(uuid);

        // If loadedData is null then we're probably completing load on an already loaded resource, triggered
        // by its dependencies.
        if (myLoadData != nullptr && myLoadData->loadedData != nullptr)
        {
          Lock loadedLock(m_loadedResourceMutex);

          m_loadedResources[uuid] = myLoadData->resData;
          resource.setHandleData(myLoadData->loadedData, uuid);
        }

        for (auto& dependantLoad : dependantLoads)
          dependantLoad->remainingDependencies--;
      }
    }

    for (auto& dependantLoad : dependantLoads)
    {
      HResource dependant = dependantLoad->resData.resource.lock();
      loadComplete(dependant);
    }

    if (finishLoad && myLoadData != nullptr)
    {
      onResourceLoaded(resource);

      // This should only ever be true on the main thread
      if (myLoadData->notifyImmediately)
        ResourceListenerManager::instance().notifyListeners(uuid);

      ge_delete(myLoadData);
    }
  }

  void Resources::loadCallback(const Path& filePath, HResource& resource, bool loadWithSaveData)
  {
    SPtr<Resource> rawResource = loadFromDiskAndDeserialize(filePath, loadWithSaveData);

    {
      Lock lock(m_inProgressResourcesMutex);

      // Check if all my dependencies are loaded
      ResourceLoadData* myLoadData = m_inProgressResources[resource.getUUID()];
      myLoadData->loadedData = rawResource;
      myLoadData->remainingDependencies--;
    }

    loadComplete(resource);
  }

  GE_CORE_EXPORT Resources&
  g_resources() {
    return Resources::instance();
  }
}
