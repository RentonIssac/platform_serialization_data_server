#pragma once

#include <set>
#include "platform_serialization_data_connection.h"

template<class T_DATA>
class platform_serialization_data_connection_mgr {
public:
	platform_serialization_data_connection_mgr(const platform_serialization_data_connection_mgr&) = delete;
	platform_serialization_data_connection_mgr& operator=(const platform_serialization_data_connection_mgr&) = delete;

	/// Construct a connection manager.
	platform_serialization_data_connection_mgr() {
	}

	/// Add the specified connection to the manager and start it.
	void start(boost::shared_ptr<platform_serialization_data_connection<T_DATA>> c) {
		conns.insert(c);
		c->start();
	}

	/// Stop the specified connection.
	void stop(boost::shared_ptr<platform_serialization_data_connection<T_DATA>> c) {
		c->stop();
	}

	void stop_all() {
		for (auto c : conns)
			c->stop();
		conns.clear();
	}

	void write(const T_DATA& data) {
		for (auto c : conns)
			c->do_write();
	}

private:
	/// The managed connections.
	std::set<boost::shared_ptr<platform_serialization_data_connection<T_DATA>>> conns;
};