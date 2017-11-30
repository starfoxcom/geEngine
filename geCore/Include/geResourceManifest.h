/*****************************************************************************/
/**
 * @file    geResourceManifest.h
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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include <geIReflectable.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT ResourceManifest : public IReflectable
  {
    struct ConstructPrivately {};

   public:
    explicit ResourceManifest(const ConstructPrivately& dummy);
    ResourceManifest(const String& name);

    /**
     * @brief Returns an unique name of the resource manifest.
     */
    const String&
    getName() const {
      return m_name;
    }

    /**
     * @brief Registers a new resource in the manifest.
     */
    void
    registerResource(const UUID& uuid, const Path& filePath);

    /**
     * @brief Removes a resource from the manifest.
     */
    void
    unregisterResource(const UUID& uuid);

    /**
     * @brief Attempts to find a resource with the provided UUID and outputs
     *        the path to the resource if found. Returns true if UUID was
     *        found, false otherwise.
     */
    bool
    uuidToFilePath(const UUID& uuid, Path& filePath) const;

    /**
     * @brief Attempts to find a resource with the provided path and outputs
     *        the UUID to the resource if found. Returns true if path was
     *        found, false otherwise.
     */
    bool
    filePathToUUID(const Path& filePath, UUID& outUUID) const;

    /**
     * @brief Checks if provided UUID exists in the manifest.
     */
    bool
    uuidExists(const UUID& uuid) const;

    /**
     * @brief Checks if the provided path exists in the manifest.
     */
    bool
    filePathExists(const Path& filePath) const;

    /**
     * @brief Saves the resource manifest to the specified location.
     * @param[in] manifest  Manifest to save.
     * @param[in] path      Full pathname of the file to save the manifest in.
     * @param[in] relativePath  If not empty, all pathnames in the manifest
     *            will be stored as if relative to this path.
     */
    static void
    save(const SPtr<ResourceManifest>& manifest,
         const Path& path,
         const Path& relativePath);

    /**
     * @brief Loads the resource manifest from the specified location.
     * @param[in] path  Full pathname of the file to load the manifest from.
     * @param[in] relativePath  If not empty, all loaded pathnames will have
     *            this path prepended.
     */
    static SPtr<ResourceManifest>
    load(const Path& path, const Path& relativePath);

    /**
     * @brief Creates a new empty resource manifest. Provided name should be
     *        unique among manifests.
     */
    static SPtr<ResourceManifest>
    create(const String& name);

   private:
    String m_name;
    UnorderedMap<UUID, Path> m_uuidToFilePath;
    UnorderedMap<Path, UUID> m_filePathToUUID;

    /*************************************************************************/
    /**
     * RTTI
     */
    /*************************************************************************/

    /**
     * @brief Creates a new empty resource manifest.
     */
    static SPtr<ResourceManifest>
    createEmpty();

   public:
    friend class ResourceManifestRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    virtual RTTITypeBase*
    getRTTI() const override;
  };
}
