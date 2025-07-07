#pragma once

/**
 * @file GameEvent.hpp
 * @brief Represents a decoded packet for ECS/game loop.
 */

#include <vector>
#include <memory>
#include "Opcodes.hpp"

/**
 * @class ClientSession
 * @brief Forward declaration for session pointer.
 */
class ClientSession;

/**
 * @struct GameEvent
 * @brief Carries a received opcode, raw payload, and the session it came from.
 */
struct GameEvent
{
	Opcode opcode;                          /**< Decoded opcode. */
	std::vector<uint8_t> payload;           /**< Raw payload bytes. */
	std::shared_ptr<ClientSession> session; /**< Source session. */
};
