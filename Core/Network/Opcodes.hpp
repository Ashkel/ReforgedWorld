#pragma once

/**
 * @file Opcodes.hpp
 * @brief Defines opcode values for the hybrid protocol.
 */

#include <cstdint>

/**
 * @enum Opcode
 * @brief Logical opcodes for the ECS/game loop.
 */
enum Opcode : uint16_t
{
	NONE = 0,   /**< Undefined opcode (or Flatbuffers only). */
	PING = 1,   /**< Flatbuffers PING. */
	LOGIN = 2,  /**< Flatbuffers LOGIN. */

	MOVE = 1001 /**< Hard packet MOVE. */
};
