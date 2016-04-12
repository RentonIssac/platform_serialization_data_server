// maptest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "platform_serialization_data_server.h"

int main() {
	try {
		unsigned short port = 9999;

		boost::asio::io_service io_service;
		platform_serialization_data_server server(io_service, port);
		io_service.run();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
    return 0;
}

