/********************************************************************/
/**
 * @file   geMessageHandler.h
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
#pragma once

/************************************************************************************************************************/
/* Includes																												*/
/************************************************************************************************************************/
#include "gePrerequisitesUtil.h"
#include "geModule.h"

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Allows you to transparently pass messages between different systems.
	* @note		Sim thread only.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT MessageHandler : public Module<MessageHandler>
	{
	private:
		friend class HMessage;

		struct MessageHandlerData
		{
			uint32 Id;
			std::function<void()> Callback;
		};

		Map<uint32, Vector<MessageHandlerData>> m_MessageHandlers;
		Map<uint32, uint32> m_HandlerIdToMessageMap;
		uint32 m_NextCallbackId;

	public:
		MessageHandler();

		/************************************************************************************************************************/
		/**
		* @brief	Sends a message to all subscribed listeners.
		*/
		/************************************************************************************************************************/
		void Send(MessageId message);

		/************************************************************************************************************************/
		/**
		* @brief	Subscribes a message listener for the specified message. Provided callback will be triggered whenever that
		*			message gets sent.
		* @return	A handle to the message subscription that you can use to unsubscribe from listening.
		*/
		/************************************************************************************************************************/
		HMessage Listen(MessageId message, std::function<void()> callback);

	private:
		void Unsubscribe(uint32 handleId);
	};
}
