// maptest_client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "..\maptest\platform_serialization_data_client.h"


int main()
{
	try	{
		boost::asio::io_service io_service;
		platform_serialization_data_client client(io_service, "127.0.0.1", "9999");
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	getchar();

    return 0;
}

