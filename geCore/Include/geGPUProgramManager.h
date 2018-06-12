/*****************************************************************************/
/**
 * @file    geGPUProgramManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/12
 * @brief   Manager responsible for creating GPU programs.
 *
 * Manager responsible for creating GPU programs. It will automatically try to
 * find the appropriate handler for a specific GPU program language and create
 * the program if possible.
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
#include "geGPUProgram.h"

#include <geModule.h>
#include <geException.h>

namespace geEngineSDK {
  /**
   * @brief Manager responsible for creating GPU programs.
   *        It will automatically try to find the appropriate handler for a
   *        specific GPU program language and create the program if possible.
   * @note  Sim thread only.
   */
  class GE_CORE_EXPORT GPUProgramManager : public Module<GPUProgramManager>
  {
   public:
    /**
     * @copydoc GPUProgram::create
     */
    SPtr<GPUProgram>
    create(const GPU_PROGRAM_DESC& desc);

    /**
     * @brief Creates a completely empty and uninitialized GPUProgram.
     *        Should only be used for specific purposes, like deserialization,
     *        as it requires additional manual initialization that is not
     *        required normally.
     */
    SPtr<GPUProgram>
    createEmpty(const String& language, GPU_PROGRAM_TYPE::E type);
  };

  namespace geCoreThread {
    /**
     * @brief Factory responsible for creating GPU programs of a certain type.
     */
    class GE_CORE_EXPORT GPUProgramFactory
    {
     public:
      GPUProgramFactory() = default;
      virtual ~GPUProgramFactory() = default;

      /**
       * @copydoc GPUProgram::create
       */
      virtual SPtr<GPUProgram>
      create(const GPU_PROGRAM_DESC& desc,
             GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT) = 0;

      /**
       * @copydoc geEngineSDK::GPUProgramManager::createEmpty
       */
      virtual SPtr<GPUProgram>
      create(GPU_PROGRAM_TYPE::E type,
             GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT) = 0;

      /**
       * @copydoc GPUProgram::compileBytecode
       */
      virtual SPtr<GPUProgramBytecode>
      compileBytecode(const GPU_PROGRAM_DESC& desc) = 0;
    };

    /**
     * @brief Manager responsible for creating GPU programs.
     *        It will automatically try to find the appropriate handler for a
     *        specific GPU program language and create the program if possible.
     * @note  Core thread only unless otherwise specified.
     */
    class GE_CORE_EXPORT GPUProgramManager : public Module<GPUProgramManager>
    {
     public:
      GPUProgramManager();
      virtual ~GPUProgramManager();

      /**
       * @brief Registers a new factory that is able to create GPU programs for
       *        a certain language. If any other factory for the same language
       *        exists, it will overwrite it.
       */
      void
      addFactory(const String& language, GPUProgramFactory* factory);

      /**
       * @brief Unregisters a GPU program factory, essentially making it not
       *        possible to create GPU programs using the language the factory
       *        supported.
       */
      void
      removeFactory(const String& language);

      /**
       * @brief Query if a GPU program language is supported
       *        (for example "hlsl", "glsl").
       *        Thread safe.
       */
      bool
      isLanguageSupported(const String& language);

      /**
       * @copydoc GPUProgram::create
       */
      SPtr<GPUProgram>
      create(const GPU_PROGRAM_DESC& desc,
             GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT);

      /**
       * @copydoc GPUProgram::compileBytecode
       */
      SPtr<GPUProgramBytecode>
      compileBytecode(const GPU_PROGRAM_DESC& desc);

     protected:
      friend class geEngineSDK::GPUProgram;

      /**
       * @brief Creates a GPU program without initializing it.
       * @see   create
       */
      SPtr<GPUProgram>
      createInternal(const GPU_PROGRAM_DESC& desc,
                     GPU_DEVICE_FLAGS::E deviceMask =
                       GPU_DEVICE_FLAGS::kDEFAULT);

      /**
       * @brief Attempts to find a factory for the specified language.
       *        Returns null if it cannot find one.
       */
      GPUProgramFactory*
      getFactory(const String& language);

     protected:
      Mutex m_mutex;

      UnorderedMap<String, GPUProgramFactory*> m_factories;

      /**
       * @brief Factory for dealing with GPU programs that can't be created.
       */
      GPUProgramFactory* m_nullFactory;
    };
  }
}
