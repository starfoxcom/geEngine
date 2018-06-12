/*****************************************************************************/
/**
 * @file    geGPUProgramManager.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geGPUProgramManager.h"
#include "geRenderAPI.h"

namespace geEngineSDK {
  SPtr<GPUProgram> GPUProgramManager::create(const GPU_PROGRAM_DESC& desc)
  {
    GPUProgram* program = new (ge_alloc<GPUProgram>()) GPUProgram(desc);
    SPtr<GPUProgram> ret = ge_core_ptr<GPUProgram>(program);
    ret->_setThisPtr(ret);
    ret->initialize();

    return ret;
  }

  SPtr<GPUProgram>
  GPUProgramManager::createEmpty(const String& language,
                                 GPU_PROGRAM_TYPE::E type) {
    GPU_PROGRAM_DESC desc;
    desc.language = language;
    desc.type = type;

    GPUProgram* program = new (ge_alloc<GPUProgram>()) GPUProgram(desc);
    SPtr<GPUProgram> ret = ge_core_ptr<GPUProgram>(program);
    ret->_setThisPtr(ret);

    return ret;
  }

  namespace geCoreThread {
    String sNullLang = "null";

    /** Null GPU program used in place of GPU programs we cannot create. Null programs don't do anything. */
    class NullProgram : public GPUProgram
    {
    public:
      NullProgram()
        :GPUProgram(GPU_PROGRAM_DESC(), GPU_DEVICE_FLAGS::kDEFAULT)
      { }

      ~NullProgram() { }

      bool isSupported() const { return false; }
      const String& getLanguage() const { return sNullLang; }

    protected:
      void loadFromSource() {}

      void buildConstantDefinitions() const { }
    };

    /**	Factory that creates null GPU programs.  */
    class NullProgramFactory : public GPUProgramFactory
    {
    public:
      NullProgramFactory() {}
      ~NullProgramFactory() {}

      SPtr<GPUProgram> create(const GPU_PROGRAM_DESC& desc, GPU_DEVICE_FLAGS::E deviceMask) override
      {
        SPtr<NullProgram> ret = ge_shared_ptr_new<NullProgram>();
        ret->_setThisPtr(ret);

        return ret;
      }

      SPtr<GPUProgram>
      create(GPU_PROGRAM_TYPE::E type,
             GPU_DEVICE_FLAGS::E deviceMask) override {
        SPtr<NullProgram> ret = ge_shared_ptr_new<NullProgram>();
        ret->_setThisPtr(ret);

        return ret;
      }

      SPtr<GPUProgramBytecode> compileBytecode(const GPU_PROGRAM_DESC& desc) override
      {
        auto bytecode = ge_shared_ptr_new<GPUProgramBytecode>();
        bytecode->compilerId = "Null";

        return bytecode;
      }
    };

    GPUProgramManager::GPUProgramManager()
    {
      mNullFactory = ge_new<NullProgramFactory>();
      addFactory(sNullLang, mNullFactory);
    }

    GPUProgramManager::~GPUProgramManager()
    {
      ge_delete((NullProgramFactory*)mNullFactory);
    }

    void GPUProgramManager::addFactory(const String& language, GPUProgramFactory* factory)
    {
      Lock lock(mMutex);

      mFactories[language] = factory;
    }

    void GPUProgramManager::removeFactory(const String& language)
    {
      Lock lock(mMutex);

      auto iter = mFactories.find(language);
      if (iter != mFactories.end())
        mFactories.erase(iter);
    }

    GPUProgramFactory* GPUProgramManager::getFactory(const String& language)
    {
      auto iter = mFactories.find(language);
      if (iter == mFactories.end())
        iter = mFactories.find(sNullLang);

      return iter->second;
    }

    bool GPUProgramManager::isLanguageSupported(const String& lang)
    {
      Lock lock(mMutex);

      auto iter = mFactories.find(lang);
      return iter != mFactories.end();
    }

    SPtr<GPUProgram> GPUProgramManager::create(const GPU_PROGRAM_DESC& desc, GPU_DEVICE_FLAGS::E deviceMask)
    {
      SPtr<GPUProgram> ret = createInternal(desc, deviceMask);
      ret->initialize();

      return ret;
    }

    SPtr<GPUProgram> GPUProgramManager::createInternal(const GPU_PROGRAM_DESC& desc, GPU_DEVICE_FLAGS::E deviceMask)
    {
      GPUProgramFactory* factory = getFactory(desc.language);
      SPtr<GPUProgram> ret = factory->create(desc, deviceMask);

      return ret;
    }

    SPtr<GPUProgramBytecode> GPUProgramManager::compileBytecode(const GPU_PROGRAM_DESC& desc)
    {
      GPUProgramFactory* factory = getFactory(desc.language);
      return factory->compileBytecode(desc);
    }
  }
}
