#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <vector>
#include <deque>
#include "platform_serialization_data_connection.h" // Must come before boost/serialization headers.
#include <boost/serialization/vector.hpp>
#include "platform_serialization_data.h"

typedef std::deque<platform_serialization_data<int, test_struct>> map_message_queue;

class platform_serialization_data_server {
public:
	platform_serialization_data_server(boost::asio::io_service& _io_service, unsigned short port) : io_service(_io_service), acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {

		/*for (int i = 0; i < 100; i++) {
			//int_data.put(i, i + 1);
			test_struct ts;
			ts.the_number = i;
			ts.the_float = i + 0.5f;
			std::ostringstream stm;
			stm << "string" << i;
			ts.the_string = stm.str();
			ts.the_double = i + 0.6;
			_snprintf(ts.the_char, 32, "%s%d", "char", i);
			struct_data.put(i, ts);
		}*/

		/*for (auto iter = struct_data.begin(); iter != struct_data.end(); iter++)
			std::cout << "first:\t" << iter->first << "\tsecond:\t" << iter->second.the_number << "\t" << iter->second.the_float << "\t"
			<< iter->second.the_string << "\t" << iter->second.the_double << "\t" << iter->second.the_char << std::endl;*/
		
		// Start an accept operation for a new connection.
		platform_serialization_data_connection_ptr new_conn(new platform_serialization_data_connection(acceptor_.get_io_service()));
		acceptor_.async_accept(new_conn->socket(), boost::bind(&platform_serialization_data_server::handle_accept, this, boost::asio::placeholders::error, new_conn));
	}

	void send_map(platform_serialization_data<int, test_struct>& struct_data_tmp) {

		io_service.post(
			[this, struct_data_tmp]()
		{
			bool write_in_progress = !mmq.empty();
			mmq.push_back(struct_data_tmp);
			if (!write_in_progress)
			{
				do_write();
			}
		});
	}

	void do_write() {
		if (conn != nullptr)
			conn->async_write(mmq.front(), boost::bind(&platform_serialization_data_server::handle_write, this, boost::asio::placeholders::error, conn));
	}

	/// Handle completion of a accept operation.
	void handle_accept(const boost::system::error_code& e, platform_serialization_data_connection_ptr _conn) {
		if (!e) {
			// Successfully accepted a new connection. Send the list of stocks to the
			// client. The connection::async_write() function will automatically
			// serialize the data structure for us.
			//conn->async_write(struct_data, boost::bind(&platform_serialization_data_server::handle_write, this, boost::asio::placeholders::error, conn));
			conn = _conn;
		}

		// Start an accept operation for a new connection.
		platform_serialization_data_connection_ptr new_conn(new platform_serialization_data_connection(acceptor_.get_io_service()));
		acceptor_.async_accept(new_conn->socket(), boost::bind(&platform_serialization_data_server::handle_accept, this, boost::asio::placeholders::error, new_conn));
	}

	/// Handle completion of a write operation.
	void handle_write(const boost::system::error_code& e, platform_serialization_data_connection_ptr conn) {
		// Nothing to do. The socket will be closed automatically when the last
		// reference to the connection object goes away.
		if (!e) {
			mmq.pop_front();
			if (!mmq.empty()) {
				do_write();
			}
		}
	}

private:
	/// The acceptor object used to accept incoming socket connections.
	boost::asio::io_service& io_service;
	boost::asio::ip::tcp::acceptor acceptor_;

	/// The data to be sent to each client.
	//std::vector<stock> stocks_;
	//platform_serialization_data<int,int> int_data;
	//platform_serialization_data<int, test_struct> struct_data;
	map_message_queue mmq;

	platform_serialization_data_connection_ptr conn = nullptr;
};