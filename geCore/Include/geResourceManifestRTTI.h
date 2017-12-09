/*****************************************************************************/
/**
 * @file    geResourceManifestRTTI.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/30
 * @brief   RTTI Objects for geResourceManifest.
 *
 * RTTI Objects for geResourceManifest.
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
#include "geResourceManifest.h"
#include <geRTTIType.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT ResourceManifestRTTI
    : public RTTIType<ResourceManifest, IReflectable, ResourceManifestRTTI>
  {
   private:
    String&
    getName(ResourceManifest* obj) {
      return obj->m_name;
    }

    void
    setName(ResourceManifest* obj, String& val) {
      obj->m_name = val;
    }

    UnorderedMap<UUID, Path>&
    getUUIDMap(ResourceManifest* obj) {
      return obj->m_uuidToFilePath;
    }

    void
    setUUIDMap(ResourceManifest* obj, UnorderedMap<UUID, Path>& val) {
      obj->m_uuidToFilePath = val;
      obj->m_filePathToUUID.clear();

      for (auto& entry : obj->m_uuidToFilePath) {
        obj->m_filePathToUUID[entry.second] = entry.first;
      }
    }
   public:
    ResourceManifestRTTI() {
      addPlainField("m_name",
                    0,
                    &ResourceManifestRTTI::getName,
                    &ResourceManifestRTTI::setName);
      addPlainField("m_uuidToFilePath",
                    1,
                    &ResourceManifestRTTI::getUUIDMap,
                    &ResourceManifestRTTI::setUUIDMap);
    }

    const String&
    getRTTIName() override {
      static String name = "ResourceManifest";
      return name;
    }

    uint32
    getRTTIId() override {
      return TYPEID_CORE::kID_ResourceManifest;
    }

    SPtr<IReflectable>
    newRTTIObject() override {
      return ResourceManifest::createEmpty();
    }
  };
}
