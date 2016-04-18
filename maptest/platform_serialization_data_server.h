#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <mutex>
#include <set>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/serialization/vector.hpp>

#include "platform_serialization_data_connection.h" // Must come before boost/serialization headers.
#include "platform_serialization_data.h"

template<class T_REQ, class T_RES>
class platform_serialization_data_server {
public:
	platform_serialization_data_server(boost::asio::io_service& _io_service, unsigned short port) : io_service(_io_service), acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
		// Start an accept operation for a new connection.
		boost::shared_ptr<platform_serialization_data_connection<T_REQ, T_RES>> new_conn(new platform_serialization_data_connection<T_REQ, T_RES>(acceptor_.get_io_service()));
		acceptor_.async_accept(new_conn->socket(), boost::bind(&platform_serialization_data_server::handle_accept, this, boost::asio::placeholders::error, new_conn));
	}

	/// Handle completion of a accept operation.
	void handle_accept(const boost::system::error_code& e, boost::shared_ptr<platform_serialization_data_connection<T_REQ,T_RES>> _conn) {
		if (!e) {
			mutex_conns.lock();
			conns.insert(_conn);
			mutex_conns.unlock();
		}

		boost::shared_ptr<platform_serialization_data_connection<T_REQ, T_RES>> new_conn(new platform_serialization_data_connection<T_REQ, T_RES>(acceptor_.get_io_service()));
		acceptor_.async_accept(new_conn->socket(), boost::bind(&platform_serialization_data_server::handle_accept, this, boost::asio::placeholders::error, new_conn));
	}

	void send_data(T_REQ& data) {
		io_service.post(
			[this, data]()
		{
			mutex_conns.lock();
			do_write(data);
			mutex_conns.unlock();
		});
	}

	void do_write(const T_REQ& data) {
		mutex_conns.lock();
		if (conns.size() <= 0) {
			std::cout << "No conn to recv, drop." << std::endl;
			mutex_conns.unlock();
			return;
		}

		mutex_conns.lock();
		for (auto iter = conns.begin(); iter != conns.end(); iter++) {
			//(*iter)->async_write(data, boost::bind(&platform_serialization_data_server::handle_write, this, boost::asio::placeholders::error, (*iter)));
			(*iter)->do_write(data);
		}
		mutex_conns.unlock();
	}

	/*void handle_write(const boost::system::error_code& e, boost::shared_ptr<platform_serialization_data_connection<T_REQ, T_RES>> conn) {
		if (!e) {
		}
	}*/

private:
	boost::asio::io_service& io_service;
	boost::asio::ip::tcp::acceptor acceptor_;

	//data_queue dq;
	std::mutex mutex_conns;

	std::set<boost::shared_ptr<platform_serialization_data_connection<T_REQ, T_RES>>> conns;
};