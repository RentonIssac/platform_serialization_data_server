// maptest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <thread>

#include "platform_serialization_data_server.h"

int main() {
	try {
		unsigned short port = 9999;

		boost::asio::io_service io_service;
		platform_serialization_data_server server(io_service, port);
		//io_service.run();
		std::thread t([&io_service]() { io_service.run(); });

		int seed = 0;
		while (1) {
			seed = rand();

			platform_serialization_data<int, test_struct> struct_data_tmp;

			for (int i = 0; i < 100; i++) {
				int r = i * seed;

				test_struct ts;
				ts.the_number = r;
				ts.the_float = r + 0.5f;
				std::ostringstream stm;
				stm << "string" << r;
				ts.the_string = stm.str();
				ts.the_double = r + 0.6;
				_snprintf(ts.the_char, 32, "%s%d", "char", r);
				struct_data_tmp.put(i, ts);
			}

			server.send_map(struct_data_tmp);

			::Sleep(1000);
		}

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
    return 0;
}

