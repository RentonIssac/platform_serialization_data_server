#pragma once

#include <boost/asio.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

template<class T_REQ, class T_RES>
class platform_serialization_data_connection {
public:
	platform_serialization_data_connection(boost::asio::io_service& io_service) : socket_(io_service) {
	}

	boost::asio::ip::tcp::socket& socket() {
		return socket_;
	}

	void start() {
		do_read();
	}

	void stop() {
		socket_.close();
	}

	void do_read() {
		async_read(data, boost::bind(&platform_serialization_data_connection::handle_read, this, boost::asio::placeholders::error));
	}

	void do_write(const T_REQ& _req) {
		async_write(_req, boost::bind(&platform_serialization_data_connection::after_do_write, this, boost::asio::placeholders::error));
	}

	void after_do_write(const boost::system::error_code& e) {
		if (!e) {
			std::cout << "platform_serialization_data_connection after_do_write" << std::endl;
		}
	}

	template <typename T, typename Handler>
	void async_write(const T& t, Handler handler) {
		// Serialize the data first so we know how large it is.
		std::ostringstream archive_stream;
		boost::archive::text_oarchive archive(archive_stream);
		archive << t;
		outbound_data_ = archive_stream.str();

		// Format the header.
		std::ostringstream header_stream;
		header_stream << std::setw(header_length) << std::hex << outbound_data_.size();
		if (!header_stream || header_stream.str().size() != header_length) {
			// Something went wrong, inform the caller.
			boost::system::error_code error(boost::asio::error::invalid_argument);
			socket_.get_io_service().post(boost::bind(handler, error));
			return;
		}
		outbound_header_ = header_stream.str();

		// Write the serialized data to the socket. We use "gather-write" to send
		// both the header and the data in a single write operation.
		std::vector<boost::asio::const_buffer> buffers;
		buffers.push_back(boost::asio::buffer(outbound_header_));
		buffers.push_back(boost::asio::buffer(outbound_data_));
		boost::asio::async_write(socket_, buffers, handler);
	}

	/// Asynchronously read a data structure from the socket.
	template <typename T, typename Handler>
	void async_read(T& t, Handler handler) {
		// Issue a read operation to read exactly the number of bytes in a header.
		void (platform_serialization_data_connection::*f)(const boost::system::error_code&, T&, boost::tuple<Handler>) = &platform_serialization_data_connection::handle_read_header<T, Handler>;
		boost::asio::async_read(socket_, boost::asio::buffer(inbound_header_), boost::bind(f, this, boost::asio::placeholders::error, boost::ref(t), boost::make_tuple(handler)));
	}

	/// Handle a completed read of a message header. The handler is passed using
	/// a tuple since boost::bind seems to have trouble binding a function object
	/// created using boost::bind as a parameter.
	template <typename T, typename Handler>
	void handle_read_header(const boost::system::error_code& e, T& t, boost::tuple<Handler> handler) {
		if (e) {
			boost::get<0>(handler)(e);
		} else {
			// Determine the length of the serialized data.
			std::istringstream is(std::string(inbound_header_, header_length));
			std::size_t inbound_data_size = 0;
			if (!(is >> std::hex >> inbound_data_size)) {
				// Header doesn't seem to be valid. Inform the caller.
				boost::system::error_code error(boost::asio::error::invalid_argument);
				boost::get<0>(handler)(error);
				return;
			}

			// Start an asynchronous call to receive the data.
			inbound_data_.resize(inbound_data_size);
			void (platform_serialization_data_connection::*f)(const boost::system::error_code&, T&, boost::tuple<Handler>) = &platform_serialization_data_connection::handle_read_data<T, Handler>;
			boost::asio::async_read(socket_, boost::asio::buffer(inbound_data_), boost::bind(f, this, boost::asio::placeholders::error, boost::ref(t), handler));
		}
	}

	/// Handle a completed read of message data.
	template <typename T, typename Handler>
	void handle_read_data(const boost::system::error_code& e, T& t, boost::tuple<Handler> handler) {
		if (e) {
			boost::get<0>(handler)(e);
		} else {
			// Extract the data structure from the data just received.
			try {
				std::string archive_data(&inbound_data_[0], inbound_data_.size());
				std::istringstream archive_stream(archive_data);
				boost::archive::text_iarchive archive(archive_stream);
				archive >> t;
			} catch (std::exception& e) {
				// Unable to decode data.
				boost::system::error_code error(boost::asio::error::invalid_argument);
				boost::get<0>(handler)(error);
				return;
			}

			// Inform caller that data has been received ok.
			boost::get<0>(handler)(e);
		}
	}

	void handle_read(const boost::system::error_code& e) {
		if (!e) {
		}

		async_read(data, boost::bind(&platform_serialization_data_connection::handle_read, this, boost::asio::placeholders::error));
	}

private:
	/// The underlying socket.
	boost::asio::ip::tcp::socket socket_;

	/// The size of a fixed length header.
	enum { header_length = 8 };

	/// Holds an outbound header.
	std::string outbound_header_;

	/// Holds the outbound data.
	std::string outbound_data_;

	/// Holds an inbound header.
	char inbound_header_[header_length];

	/// Holds the inbound data.
	std::vector<char> inbound_data_;

	T_REQ data;
};

