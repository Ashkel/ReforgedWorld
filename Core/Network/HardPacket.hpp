#pragma once

/**
 * @file HardPacket.hpp
 * @brief Base struct for all fixed-layout hard packets with opcode.
 */

#include <cstdint>

#pragma pack(push, 1)

/**
 * @struct HardPacket
 * @brief Base for all hard packets containing the opcode.
 */
struct HardPacket
{
	uint16_t opcode; /**< Packet opcode */
};

/**
 * @struct HardMovePacket
 * @brief Hard packet for player movement updates.
 *
 * Inherits from HardPacket, contains player id and position.
 */
struct HardMovePacket : public HardPacket
{
	uint32_t playerId; /**< Unique player ID */
	float x;           /**< X position */
	float y;           /**< Y position */
	float z;           /**< Z position */

	/**
	 * @brief Default constructor sets opcode to MOVE opcode.
	 */
	HardMovePacket()
	{
		opcode = 1001; // MOVE opcode
	}
};

#pragma pack(pop)
