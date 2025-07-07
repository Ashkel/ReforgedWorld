#pragma once

/**
 * @file PacketDispatcher.hpp
 * @brief Maps opcode handlers for client or server usage.
 */

#include <functional>
#include <unordered_map>
#include <vector>

/**
 * @class PacketDispatcher
 * @brief Dispatches Flatbuffers packets to handlers based on opcode.
 * @tparam T The session type.
 */
template <typename T>
class PacketDispatcher
{
public:
	/**
	 * @brief Type alias for handler function.
	 * @param session Shared pointer to the session.
	 * @param payload Flatbuffers payload bytes.
	 */
	using Handler = std::function<void(std::shared_ptr<T>, const std::vector<uint8_t>&)>;

	/**
	 * @brief Register a handler for an opcode.
	 * @param opcode Opcode to listen for.
	 * @param handler Callback function.
	 */
	void registerHandler(uint16_t opcode, Handler handler)
	{
		handlers_[opcode] = handler;
	}

	/**
	 * @brief Dispatch a payload to the correct handler.
	 * @param session The source session.
	 * @param opcode Opcode from Flatbuffers.
	 * @param payload Raw bytes.
	 */
	void dispatch(std::shared_ptr<T> session, uint16_t opcode, const std::vector<uint8_t>& payload)
	{
		auto it = handlers_.find(opcode);
		if(it != handlers_.end())
		{
			it->second(session, payload);
		}
	}

private:
	std::unordered_map<uint16_t, Handler> handlers_; /**< Registered handlers. */
};
