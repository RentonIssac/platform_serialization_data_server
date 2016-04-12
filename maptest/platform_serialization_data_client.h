#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <vector>
#include "platform_serialization_data_connection.h" // Must come before boost/serialization headers.
#include <boost/serialization/vector.hpp>
#include "platform_serialization_data.h"

class platform_serialization_data_client {
public:
	/// Constructor starts the asynchronous connect operation.
	platform_serialization_data_client(boost::asio::io_service& io_service, const std::string& host, const std::string& service) : connection_(io_service) {
		// Resolve the host name into an IP address.
		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(host, service);
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		// Start an asynchronous connect operation.
		boost::asio::async_connect(connection_.socket(), endpoint_iterator, boost::bind(&platform_serialization_data_client::handle_connect, this, boost::asio::placeholders::error));
	}

	/// Handle completion of a connect operation.
	void handle_connect(const boost::system::error_code& e) {
		if (!e) {
			// Successfully established connection. Start operation to read the list
			// of stocks. The connection::async_read() function will automatically
			// decode the data that is read from the underlying socket.
			connection_.async_read(struct_data, boost::bind(&platform_serialization_data_client::handle_read, this, boost::asio::placeholders::error));
		} else {
			// An error occurred. Log it and return. Since we are not starting a new
			// operation the io_service will run out of work to do and the client will
			// exit.
			std::cerr << e.message() << std::endl;
		}
	}

	/// Handle completion of a read operation.
	void handle_read(const boost::system::error_code& e) {
		if (!e) {
			// Print out the data that was received.
			//int_data.print_all();
			for( auto iter = struct_data.begin(); iter!= struct_data.end(); iter++ )
				std::cout << "first:\t" << iter->first << "\tsecond:\t" << iter->second.the_number << "\t" << iter->second.the_float << "\t"
				<< iter->second.the_string << "\t" << iter->second.the_double << "\t" << iter->second.the_char << std::endl;
		} else {
			// An error occurred.
			std::cerr << e.message() << std::endl;
		}

		// Since we are not starting a new operation the io_service will run out of
		// work to do and the client will exit.
	}

private:
	/// The connection to the server.
	platform_serialization_data_connection connection_;

	/// The data received from the server.
	//std::vector<stock> stocks_;
	//platform_serialization_data<int, int> int_data;
	platform_serialization_data<int, test_struct> struct_data;
};
