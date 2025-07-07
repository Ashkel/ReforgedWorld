#pragma once

/**
 * @file Client.hpp
 * @brief Client connection class supporting Flatbuffers and hard packets.
 */

#include <asio.hpp>
#include <iostream>
#include <memory>
#include "Packet.hpp"
#include "Crypto.hpp"
#include "PacketDispatcher.hpp"
#include "Opcodes.hpp"
//#include "MMO_generated.h"

using asio::ip::tcp;

/**
 * @class Client
 * @brief Represents a client connection to the server.
 */
class Client : public std::enable_shared_from_this<Client>
{
public:
	/**
	 * @brief Constructor to connect and start reading.
	 * @param ioContext asio context.
	 * @param host Server hostname.
	 * @param port Server port.
	 * @param crypto AES crypto helper.
	 * @param dispatcher Packet dispatcher for incoming packets.
	 */
	Client(asio::io_context& ioContext,
		   const std::string& host,
		   uint16_t port,
		   Crypto crypto,
		   PacketDispatcher<Client>& dispatcher)
		: socket_(ioContext), crypto_(crypto), dispatcher_(dispatcher)
	{
		tcp::resolver resolver(ioContext);
		auto endpoints = resolver.resolve(host, std::to_string(port));
		asio::async_connect(socket_, endpoints,
							[this, self = shared_from_this()](std::error_code ec, tcp::endpoint)
							{
								if(!ec)
								{
									readHeader();
								}
								else
								{
									std::cerr << "Connect failed: " << ec.message() << "\n";
								}
							});
	}

	/**
	 * @brief Send a packet (Flatbuffers or hard).
	 * @param packet Packet buffer.
	 */
	void sendPacket(const Packet& packet)
	{
		bool writing = !writeQueue_.size();
		writeQueue_.push(packet);
		if(!writing) return;
		writeNext();
	}

private:
	void readHeader()
	{
		asio::async_read(socket_,
						 asio::buffer(&incomingLength_, sizeof(incomingLength_)),
						 [this, self = shared_from_this()](std::error_code ec, std::size_t)
						 {
							 if(!ec)
							 {
								 if(incomingLength_ > maxPacketSize)
								 {
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

	void readBody()
	{
		asio::async_read(socket_,
						 asio::buffer(incomingEncrypted_),
						 [this, self = shared_from_this()](std::error_code ec, std::size_t)
						 {
							 if(!ec)
							 {
								 auto decrypted = crypto_.decrypt(incomingEncrypted_);

								 if(isFlatbuffers(decrypted))
								 {
									 const MMO::Packet* fbPacket = MMO::GetPacket(decrypted.data());
									 dispatcher_.dispatch(shared_from_this(), fbPacket->opcode(), decrypted);
								 }
								 else
								 {
									 if(decrypted.size() >= sizeof(HardMovePacket))
									 {
										 HardMovePacket p;
										 std::memcpy(&p, decrypted.data(), sizeof(HardMovePacket));
										 dispatcher_.dispatch(shared_from_this(), p.opcode, decrypted);
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

	void writeNext()
	{
		Packet packet;
		if(!writeQueue_.pop(packet)) return;

		auto encrypted = crypto_.encrypt(packet.body());
		uint32_t len = static_cast<uint32_t>(encrypted.size());

		finalWriteBuffer_.resize(sizeof(len) + encrypted.size());
		std::memcpy(finalWriteBuffer_.data(), &len, sizeof(len));
		std::memcpy(finalWriteBuffer_.data() + sizeof(len), encrypted.data(), encrypted.size());

		asio::async_write(socket_, asio::buffer(finalWriteBuffer_),
						  [this, self = shared_from_this()](std::error_code ec, std::size_t)
						  {
							  if(!ec) writeNext();
							  else socket_.close();
						  });
	}

	bool isFlatbuffers(const std::vector<uint8_t>& data)
	{
		if(data.size() < sizeof(uint16_t)) return false;
		uint16_t opcode = 0;
		std::memcpy(&opcode, data.data(), sizeof(uint16_t));
		return opcode < 1000;
	}

private:
	tcp::socket socket_;                    /**< The TCP socket. */
	Crypto crypto_;                        /**< AES crypto helper. */
	PacketDispatcher<Client>& dispatcher_; /**< Dispatcher for incoming packets. */

	uint32_t incomingLength_ = 0;           /**< Incoming packet length. */
	std::vector<uint8_t> incomingEncrypted_; /**< Buffer for encrypted data. */
	std::vector<uint8_t> finalWriteBuffer_;   /**< Buffer for encrypted outgoing data. */

	ThreadSafeQueue<Packet> writeQueue_;   /**< Outgoing packet queue. */

	static constexpr uint32_t maxPacketSize = 64 * 1024; /**< Max packet size. */
};
