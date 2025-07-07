#pragma once

/**
 * @file Packet.hpp
 * @brief General wrapper for serialized packet buffers.
 */

#include <vector>
#include <cstdint>
#include <cstring>
#include "HardPacket.hpp"

/**
 * @class Packet
 * @brief Wraps a buffer holding serialized data (Flatbuffers or hard packets).
 */
class Packet
{
public:
	/**
	 * @brief Construct from a raw byte buffer.
	 * @param buffer Raw serialized packet bytes.
	 */
	Packet(const std::vector<uint8_t>& buffer) : buffer_(buffer) {}
	 
	/**
	 * @brief Construct a Packet directly from any HardPacket.
	 * @param pkt Pointer to HardPacket struct.
	 * @param size Size of the struct.
	 *
	 * Example:
	 * @code
	 * HardMovePacket move;
	 * Packet p(move);
	 * @endcode
	 */
	Packet(const HardPacket& pkt, size_t size)
	{
		buffer_.resize(size);
		std::memcpy(buffer_.data(), &pkt, size);
	}

	/**
	 * @brief Access the raw packet data.
	 * @return Const reference to internal buffer.
	 */
	const std::vector<uint8_t>& body() const { return buffer_; }

	/**
	 * @brief Build a Packet from any HardPacket-derived struct.
	 * @param pkt Pointer to the hard packet struct.
	 * @param size Size of the hard packet struct.
	 * @return Packet wrapping the binary data.
	 */
	static Packet buildHardPacket(const HardPacket* pkt, size_t size)
	{
		std::vector<uint8_t> buffer(size);
		std::memcpy(buffer.data(), pkt, size);
		return Packet(buffer);
	}

private:
	std::vector<uint8_t> buffer_; /**< Serialized packet data */
};
