// maptest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <thread>

#include "platform_serialization_data_server.h"

int main() {
	try {
		unsigned short port = 9999;

		boost::asio::io_service io_service;
		platform_serialization_data_server<platform_serialization_data<int, req>, platform_serialization_data<int, res>> server(io_service, port);
		//io_service.run();
		std::thread t([&io_service]() { io_service.run(); });

		int seed = 0;
		while (1) {
			seed++;

			platform_serialization_data<int, req> struct_data_tmp;

			for (int i = 0; i < 100; i++) {
				int r = seed;

				req ts;
				ts.req_number = r;
				ts.req_float = r + 0.5f;
				std::ostringstream stm;
				stm << "string" << r;
				ts.req_string = stm.str();
				ts.req_double = r + 0.6;
				_snprintf_s(ts.req_char, 32, "%s%d", "char", r);
				struct_data_tmp.put(r, ts);
			}

			server.send_data(struct_data_tmp);

			::Sleep(1);
		}

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
    return 0;
}

