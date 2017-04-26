/********************************************************************/
/**
 * @file   geMessageHandlerFwd.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/03/06
 * @brief  Classes used for the global messaging system.
 *
 * Forward declaration of all the objects needed to send messages
 * with the global messaging system
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Includes																												*/
/************************************************************************************************************************/

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Identifier for message used with the global messaging system.
	* @note		Primary purpose of this class is to avoid expensive string compare (i.e. button names), and instead use a
	*			unique message identifier for compare. Generally you want to create one of these using the message name,
	*			and then store it for later use.
	* @note		This class is not thread safe and should only be used on the simulation thread.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT MessageId
	{
	private:
		friend class MessageHandler;

		static Map<String, uint32> UniqueMessageIds;
		static uint32 NextMessageId;

		uint32 m_MsgIdentifier;

	public:
		MessageId();
		MessageId(const String& name);

		bool operator== (const MessageId& rhs) const
		{
			return (m_MsgIdentifier == rhs.m_MsgIdentifier);
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Handle to a subscription for a specific message in the global messaging system.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT HMessage
	{
	private:
		friend class MessageHandler;

		uint32 m_Id;

	public:
		HMessage();

		/************************************************************************************************************************/
		/**
		* @brief	Disconnects the message listener so it will no longer receive events from the messaging system.
		*/
		/************************************************************************************************************************/
		void Disconnect();

	private:
		HMessage(uint32 id);
	};

	/************************************************************************************************************************/
	/**
	* @brief	Sends a message using the global messaging system.
	* @note		Simulation thread only.
	*/
	/************************************************************************************************************************/
	void GE_UTILITY_EXPORT SendMessage(MessageId message);

	class MessageHandler;
}
