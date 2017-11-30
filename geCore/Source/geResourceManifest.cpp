/*****************************************************************************/
/**
 * @file    geResourceManifest.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/30
 * @brief   Serializable class that contains UUID <-> file path mapping for
 *          resources.
 *
 * Serializable class that contains UUID <-> file path mapping for resources.
 * This class allows you to reference resources between sessions. At the end of
 * a session save the resource manifest, and then restore it at the start of a
 * new session. This way ensures that resource UUIDs stay consistent and
 * anything referencing them can find the resources.
 *
 * @note Thread safe.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geResourceManifest.h"
#include "geResourceManifestRTTI.h"
#include <geFileSerializer.h>
#include <geException.h>

namespace geEngineSDK {
  ResourceManifest::ResourceManifest(const ConstructPrivately& dummy) {}

  ResourceManifest::ResourceManifest(const String& name) : m_name(name) {}

  SPtr<ResourceManifest>
  ResourceManifest::create(const String& name) {
    return ge_shared_ptr_new<ResourceManifest>(name);
  }

  SPtr<ResourceManifest>
  ResourceManifest::createEmpty() {
    return ge_shared_ptr_new<ResourceManifest>(ConstructPrivately());
  }

  void
  ResourceManifest::registerResource(const UUID& uuid, const Path& filePath) {
    auto iterFind = m_uuidToFilePath.find(uuid);

    if (iterFind != m_uuidToFilePath.end())
    {
      if (iterFind->second != filePath)
      {
        m_filePathToUUID.erase(iterFind->second);

        m_uuidToFilePath[uuid] = filePath;
        m_filePathToUUID[filePath] = uuid;
      }
    }
    else
    {
      auto iterFind2 = m_filePathToUUID.find(filePath);
      if (iterFind2 != m_filePathToUUID.end())
        m_uuidToFilePath.erase(iterFind2->second);

      m_uuidToFilePath[uuid] = filePath;
      m_filePathToUUID[filePath] = uuid;
    }
  }

  void ResourceManifest::unregisterResource(const UUID& uuid)
  {
    auto iterFind = m_uuidToFilePath.find(uuid);

    if (iterFind != m_uuidToFilePath.end())
    {
      m_filePathToUUID.erase(iterFind->second);
      m_uuidToFilePath.erase(uuid);
    }
  }

  bool ResourceManifest::uuidToFilePath(const UUID& uuid, Path& filePath) const
  {
    auto iterFind = m_uuidToFilePath.find(uuid);

    if (iterFind != m_uuidToFilePath.end())
    {
      filePath = iterFind->second;
      return true;
    }
    else
    {
      filePath = Path::BLANK;
      return false;
    }
  }

  bool ResourceManifest::filePathToUUID(const Path& filePath, UUID& outUUID) const
  {
    auto iterFind = m_filePathToUUID.find(filePath);

    if (iterFind != m_filePathToUUID.end())
    {
      outUUID = iterFind->second;
      return true;
    }
    else
    {
      outUUID = UUID::EMPTY;
      return false;
    }
  }

  bool ResourceManifest::uuidExists(const UUID& uuid) const
  {
    auto iterFind = m_uuidToFilePath.find(uuid);

    return iterFind != m_uuidToFilePath.end();
  }

  bool ResourceManifest::filePathExists(const Path& filePath) const
  {
    auto iterFind = m_filePathToUUID.find(filePath);

    return iterFind != m_filePathToUUID.end();
  }

  void ResourceManifest::save(const SPtr<ResourceManifest>& manifest, const Path& path, const Path& relativePath)
  {
    SPtr<ResourceManifest> copy = create(manifest->m_name);

    for (auto& elem : manifest->m_filePathToUUID)
    {
      if (!relativePath.includes(elem.first))
      {
        GE_EXCEPT(InvalidStateException, "Path in resource manifest cannot be made relative to: \"" +
          relativePath.toString() + "\". Path: \"" + elem.first.toString() + "\"");
      }

      Path elementRelativePath = elem.first.getRelative(relativePath);

      copy->m_filePathToUUID[elementRelativePath] = elem.second;
    }

    for (auto& elem : manifest->m_uuidToFilePath)
    {
      if (!relativePath.includes(elem.second))
      {
        GE_EXCEPT(InvalidStateException, "Path in resource manifest cannot be made relative to: \"" +
          relativePath.toString() + "\". Path: \"" + elem.second.toString() + "\"");
      }

      Path elementRelativePath = elem.second.getRelative(relativePath);

      copy->m_uuidToFilePath[elem.first] = elementRelativePath;
    }

    FileEncoder fs(path);
    fs.encode(copy.get());
  }

  SPtr<ResourceManifest> ResourceManifest::load(const Path& path, const Path& relativePath)
  {
    FileDecoder fs(path);
    SPtr<ResourceManifest> manifest = std::static_pointer_cast<ResourceManifest>(fs.decode());

    SPtr<ResourceManifest> copy = create(manifest->m_name);

    for (auto& elem : manifest->m_filePathToUUID)
    {
      Path absPath = elem.first.getAbsolute(relativePath);
      copy->m_filePathToUUID[absPath] = elem.second;
    }

    for (auto& elem : manifest->m_uuidToFilePath)
    {
      Path absPath = elem.second.getAbsolute(relativePath);
      copy->m_uuidToFilePath[elem.first] = absPath;
    }

    return copy;
  }

  RTTITypeBase* ResourceManifest::getRTTIStatic()
  {
    return ResourceManifestRTTI::instance();
  }

  RTTITypeBase* ResourceManifest::getRTTI() const
  {
    return ResourceManifest::getRTTIStatic();
  }
}
