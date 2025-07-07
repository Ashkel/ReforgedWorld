#pragma once

/**
 * @file ClientSession.hpp
 * @brief Handles a TCP session with AES encryption, receiving Flatbuffers and hard packets.
 */

#include <asio.hpp>
#include <memory>
#include <iostream>
#include <vector>
#include <cstring>

#include "Packet.hpp"
#include "Crypto.hpp"
#include "ThreadSafeQueue.hpp"
#include "GameEvent.hpp"
#include "Opcodes.hpp"
//#include "MMO_generated.h"

using asio::ip::tcp;

/**
 * @class ClientSession
 * @brief Represents a connected client session.
 *
 * Supports receiving and sending both Flatbuffers and hard packets, encrypted with AES.
 * Incoming packets are pushed as GameEvents to a thread-safe queue.
 */
class ClientSession : public std::enable_shared_from_this<ClientSession>
{
public:
	/**
	 * @brief Constructs the session.
	 * @param socket TCP socket from acceptor.
	 * @param crypto AES encryption helper.
	 * @param eventQueue Queue to push incoming events for ECS.
	 */
	ClientSession(tcp::socket socket,
				  Crypto crypto,
				  ThreadSafeQueue<GameEvent>& eventQueue)
		: socket_(std::move(socket)),
		crypto_(crypto),
		eventQueue_(eventQueue)
	{
	}

	  /**
	   * @brief Starts the async read loop.
	   */
	void start() { readHeader(); }

	/**
	 * @brief Asynchronously sends a packet to the client.
	 * @param packet Packet containing raw payload (already serialized).
	 */
	void sendPacket(const Packet& packet)
	{
		bool writing = !writeQueue_.size();
		writeQueue_.push(packet);
		if(!writing) return;
		writeNext();
	}

private:
	/**
	 * @brief Reads the 4-byte incoming packet length header.
	 */
	void readHeader()
	{
		auto self = shared_from_this();
		asio::async_read(socket_,
						 asio::buffer(&incomingLength_, sizeof(incomingLength_)),
						 [this, self](std::error_code ec, std::size_t)
						 {
							 if(!ec)
							 {
								 if(incomingLength_ > maxPacketSize)
								 {
// Invalid size, close connection
									 socket_.close();
									 return;
								 }
								 incomingEncrypted_.resize(incomingLength_);
								 readBody();
							 }
							 else
							 {
								 socket_.close();
							 }
						 });
	}

	/**
	 * @brief Reads the encrypted body based on length header.
	 */
	void readBody()
	{
		auto self = shared_from_this();
		asio::async_read(socket_,
						 asio::buffer(incomingEncrypted_),
						 [this, self](std::error_code ec, std::size_t)
						 {
							 if(!ec)
							 {
								 auto decrypted = crypto_.decrypt(incomingEncrypted_);

								 // Detect if Flatbuffers or hard packet
								 if(isFlatbuffers(decrypted))
								 {
	  // Flatbuffers packet
									 const MMO::Packet* fbPacket = MMO::GetPacket(decrypted.data());
									 auto opcode = static_cast<Opcode>(fbPacket->opcode());
									 eventQueue_.push(GameEvent{ opcode, std::vector<uint8_t>(decrypted), shared_from_this() });
								 }
								 else
								 {
												  // Hard packet
									 if(decrypted.size() >= sizeof(HardMovePacket))
									 {
										 HardMovePacket p;
										 std::memcpy(&p, decrypted.data(), sizeof(HardMovePacket));
										 eventQueue_.push(GameEvent{ static_cast<Opcode>(p.opcode), decrypted, shared_from_this() });
									 }
									 else
									 {
										 std::cerr << "Received malformed hard packet.\n";
									 }
								 }

								 readHeader();
							 }
							 else
							 {
								 socket_.close();
							 }
						 });
	}

	/**
	 * @brief Sends next packet from the write queue.
	 */
	void writeNext()
	{
		Packet packet;
		if(!writeQueue_.pop(packet)) return;

		auto encrypted = crypto_.encrypt(packet.body());
		uint32_t len = static_cast<uint32_t>(encrypted.size());

		// Compose full message: [length][encrypted payload]
		finalWriteBuffer_.resize(sizeof(len) + encrypted.size());
		std::memcpy(finalWriteBuffer_.data(), &len, sizeof(len));
		std::memcpy(finalWriteBuffer_.data() + sizeof(len), encrypted.data(), encrypted.size());

		auto self = shared_from_this();
		asio::async_write(socket_, asio::buffer(finalWriteBuffer_),
						  [this, self](std::error_code ec, std::size_t)
						  {
							  if(!ec)
							  {
								  writeNext();
							  }
							  else
							  {
								  socket_.close();
							  }
						  });
	}

	/**
	 * @brief Heuristic to determine if decrypted data is Flatbuffers.
	 * @param data Decrypted packet bytes.
	 * @return True if data is Flatbuffers packet, false otherwise.
	 */
	bool isFlatbuffers(const std::vector<uint8_t>& data)
	{
		if(data.size() < sizeof(uint16_t)) return false;
		uint16_t opcode = 0;
		std::memcpy(&opcode, data.data(), sizeof(uint16_t));
		// Flatbuffers opcode is defined as < 1000, e.g. 0 for FB packets.
		return opcode < 1000;
	}

private:
	tcp::socket socket_;                    /**< The TCP socket */
	Crypto crypto_;                        /**< AES encrypt/decrypt */
	ThreadSafeQueue<GameEvent>& eventQueue_; /**< Queue for game loop */

	uint32_t incomingLength_ = 0;          /**< Length of next encrypted packet */
	std::vector<uint8_t> incomingEncrypted_; /**< Buffer for encrypted incoming data */
	std::vector<uint8_t> finalWriteBuffer_;   /**< Buffer for encrypted outgoing data */

	ThreadSafeQueue<Packet> writeQueue_;   /**< Queue for outgoing packets */

	static constexpr uint32_t maxPacketSize = 64 * 1024; /**< Max allowed packet size */
};
