#pragma once

/**
 * @file Server.hpp
 * @brief Simple TCP server accepting multiple ClientSessions.
 */

#include <asio.hpp>
#include <memory>
#include "ClientSession.hpp"
#include "Crypto.hpp"
#include "ThreadSafeQueue.hpp"
#include "GameEvent.hpp"

using asio::ip::tcp;

/**
 * @class Server
 * @brief Accepts incoming TCP connections and creates ClientSessions.
 */
class Server
{
public:
	/**
	 * @brief Starts server listening on specified port.
	 * @param ioContext asio IO context.
	 * @param port TCP port to listen.
	 * @param crypto AES crypto helper.
	 * @param eventQueue Event queue to pass GameEvents.
	 */
	Server(asio::io_context& ioContext,
		   uint16_t port,
		   Crypto crypto,
		   ThreadSafeQueue<GameEvent>& eventQueue)
		: acceptor_(ioContext, tcp::endpoint(tcp::v4(), port)),
		crypto_(crypto),
		eventQueue_(eventQueue)
	{
		doAccept();
	}

private:
	/**
	 * @brief Accepts incoming connections asynchronously.
	 */
	void doAccept()
	{
		acceptor_.async_accept(
			[this](std::error_code ec, tcp::socket socket)
			{
				if(!ec)
				{
					std::make_shared<ClientSession>(std::move(socket), crypto_, eventQueue_)->start();
				}
				doAccept();
			});
	}

	tcp::acceptor acceptor_;               /**< Accepts TCP connections. */
	Crypto crypto_;                        /**< AES crypto helper. */
	ThreadSafeQueue<GameEvent>& eventQueue_; /**< Event queue for ECS/game loop. */
};
