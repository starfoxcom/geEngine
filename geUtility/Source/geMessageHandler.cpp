/********************************************************************/
/**
 * @file   geMessageHandler.cpp
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/03/06
 * @brief  Allows to transparently pass messages between different
 *			systems
 *
 * Message system that allows you to transparently pass messages
 * between different systems. Only usable in the Simulation thread
 *
 * @bug	   No known bugs.
 */
/********************************************************************/

/************************************************************************************************************************/
/* Includes																												*/
/************************************************************************************************************************/
#include "geMessageHandler.h"

namespace geEngineSDK
{
	Map<String, uint32> MessageId::UniqueMessageIds;
	uint32 MessageId::NextMessageId = 0;

	MessageId::MessageId() : m_MsgIdentifier(0)
	{
	
	}

	MessageId::MessageId(const String& name)
	{
		auto findIter = UniqueMessageIds.find(name);

		if( findIter != UniqueMessageIds.end() )
		{
			m_MsgIdentifier = findIter->second;
		}
		else
		{
			m_MsgIdentifier = NextMessageId;
			UniqueMessageIds[name] = NextMessageId++;
		}
	}

	HMessage::HMessage() : m_Id(0)
	{
	}

	HMessage::HMessage(uint32 id) : m_Id(id)
	{
	}

	void HMessage::Disconnect()
	{
		if( m_Id > 0 )
		{
			MessageHandler::Instance().Unsubscribe(m_Id);
		}
	}

	MessageHandler::MessageHandler() : m_NextCallbackId(1) //0 is reserved for not initialized
	{
	}

	void MessageHandler::Send(MessageId message)
	{
		auto iterFind = m_MessageHandlers.find(message.m_MsgIdentifier);
		if( iterFind != m_MessageHandlers.end() )
		{
			for( auto& handlerData : iterFind->second )
			{
				handlerData.Callback();
			}
		}
	}

	HMessage MessageHandler::Listen(MessageId message, std::function<void()> callback)
	{
		uint32 callbackId = m_NextCallbackId++;

		MessageHandlerData data;
		data.Id = callbackId;
		data.Callback = callback;

		m_MessageHandlers[message.m_MsgIdentifier].push_back(data);
		m_HandlerIdToMessageMap[callbackId] = message.m_MsgIdentifier;

		return HMessage(callbackId);
	}

	void MessageHandler::Unsubscribe(uint32 handleId)
	{
		uint32 msgId = m_HandlerIdToMessageMap[handleId];

		auto iterFind = m_MessageHandlers.find(msgId);
		if( iterFind != m_MessageHandlers.end() )
		{
			Vector<MessageHandlerData>& handlerData = iterFind->second;

			handlerData.erase(
				std::remove_if(handlerData.begin(), handlerData.end(), [&](MessageHandlerData& x)
				{
					return x.Id == handleId;
				})
			);
		}

		m_HandlerIdToMessageMap.erase(handleId);
	}

	void SendMessage(MessageId message)
	{
		MessageHandler::Instance().Send(message);
	}
}
