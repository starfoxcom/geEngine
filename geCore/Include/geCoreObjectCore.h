/*****************************************************************************/
/**
 * @file    geCoreObjectCore.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/01
 * @brief   Counterpart of a CoreObject used specifically on the core thread.
 *
 * Core thread only.
 * Different CoreObject implementations should implement this class for their
 * own needs.
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
#include <geAsyncOp.h>

namespace geEngineSDK {
  namespace geCoreThread {
    /**
     * @brief Values that represent current state of the object
     */
    namespace CO_FLAGS {
      enum E {
        kInitialized = 0x01,      //Object has been initialized and can be used.
        kScheduledForInit = 0x02  //Object has been scheduled for initialization
                                  // but core thread has not completed it yet.
      };
    }

    class GE_CORE_EXPORT CoreObject
    {
     public:
      CoreObject();
      virtual ~CoreObject();

      /**
       * @brief Called on the core thread when the object is first created.
       */
      virtual void
        initialize();

      /**
       * @brief Returns a shared_ptr version of "this" pointer.
       */
      SPtr<CoreObject>
      getThisPtr() const {
        return mThis.lock();
      }

     public:
      /**
       * @brief Sets a shared this pointer to this object. This MUST be called
       *        immediately after construction.
       * @note  Called automatically by the factory creation methods so user
       *        should not call this manually.
       */
      void
      _setThisPtr(SPtr<CoreObject> ptrThis);

     protected:
      friend class CoreObjectManager;
      friend class geEngineSDK::CoreObjectManager;
      friend class geEngineSDK::CoreObject;

      /**
       * @brief Update internal data from provided memory buffer that was
       *        populated with data from the sim thread.
       * @note  This generally happens at the start of a core thread frame.
       *        Data used was recorded on the previous sim thread frame.
       */
      virtual void
      syncToCore(const CoreSyncData& data) {}

      /**
       * @brief Blocks the current thread until the resource is fully initialized.
       * @note If you call this without calling initialize first a deadlock will occur.
       *       You should not call this from core thread.
       */
      void
      synchronize();

      /**
       * @brief Returns true if the object has been properly initialized.
       *        Methods are not allowed to be called on the object until it is
       *        initialized.
       */
      bool
      isInitialized() const {
        return (mFlags & CO_FLAGS::kInitialized) != 0;
      }

      bool
      isScheduledToBeInitialized() const {
        return (mFlags & CO_FLAGS::kScheduledForInit) != 0;
      }

      void
      setIsInitialized(bool initialized) {
        mFlags = initialized ?
          mFlags | CO_FLAGS::kInitialized :
          mFlags & ~CO_FLAGS::kScheduledForInit;
      }

      void
      setScheduledToBeInitialized(bool scheduled) {
        mFlags = scheduled ?
          mFlags | CO_FLAGS::kScheduledForInit :
          mFlags & ~CO_FLAGS::kScheduledForInit;
      }

      volatile uint8 mFlags;
      std::weak_ptr<CoreObject> mThis;
      static Signal mCoreGpuObjectLoadedCondition;
      static Mutex mCoreGpuObjectLoadedMutex;
    };
  }
}
