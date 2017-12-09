/*****************************************************************************/
/**
 * @file    geGameObjectHandleRTTI.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/12/06
 * @brief   RTTI Objects for geGameObjectHandle.
 *
 * RTTI Objects for geGameObjectHandle.
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
#include "geGameObjectHandle.h"
#include "geGameObjectManager.h"
#include <geRTTIType.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT GameObjectHandleRTTI
    : public RTTIType<GameObjectHandleBase, IReflectable, GameObjectHandleRTTI>
  {
   private:
    uint64&
    getInstanceId(GameObjectHandleBase* obj) {
      static uint64 invalidId = 0;

      if (nullptr != obj->m_data->m_ptr) {
        return obj->m_data->m_ptr->instanceId;
      }

      return invalidId;
    }

    void
    setInstanceId(GameObjectHandleBase* obj, uint64& value) {
      obj->m_rttiData = value;
    }

   public:
    GameObjectHandleRTTI() {
      addPlainField("instanceId",
                    0,
                    &GameObjectHandleRTTI::getInstanceId,
                    &GameObjectHandleRTTI::setInstanceId);
    }

    void
    onDeserializationEnded(IReflectable* obj,
                           const UnorderedMap<String, uint64>& /*params*/) override {
      GameObjectHandleBase* gameObjectHandle = static_cast<GameObjectHandleBase*>(obj);

      uint64 originalInstanceId = any_cast<uint64>(gameObjectHandle->m_rttiData);
      GameObjectManager::instance().registerUnresolvedHandle(originalInstanceId,
                                                             *gameObjectHandle);
      gameObjectHandle->m_rttiData = nullptr;
    }

    const String&
    getRTTIName() override {
      static String name = "GameObjectHandleBase";
      return name;
    }

    uint32
    getRTTIId() override {
      return TYPEID_CORE::kID_GameObjectHandleBase;
    }

    SPtr<IReflectable>
    newRTTIObject() override {
      SPtr<GameObjectHandleBase> obj = ge_shared_ptr<GameObjectHandleBase>
        (new (ge_alloc<GameObjectHandleBase>()) GameObjectHandleBase());
      return obj;
    }
  };
}
