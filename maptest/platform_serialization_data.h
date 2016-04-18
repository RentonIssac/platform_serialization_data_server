#pragma once

#include <stdarg.h>
#include <stdio.h>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>

#define MAKE_SERIALIZABLE_BEGIN() \
friend class boost::serialization::access; \
template <typename Archive> \
void serialize(Archive& ar, const unsigned int version) { \


#define MAKE_SERIALIZABLE(T) ( ar & T);

#define MAKE_SERIALIZABLE_END() }

class serialize_object {
public:
	virtual void print() = 0;
};

class req : public serialize_object {
public:
	int req_number;
	float req_float;
	std::string req_string;
	double req_double;
	char req_char[32];

public:
	virtual void print() {
		std::cout << req_number << "\t" << req_float << "\t" << req_string << "\t" << req_double << "\t" << req_char << std::endl;
	}
protected:
	MAKE_SERIALIZABLE_BEGIN()
		MAKE_SERIALIZABLE(req_number)
		MAKE_SERIALIZABLE(req_float)
		MAKE_SERIALIZABLE(req_string)
		MAKE_SERIALIZABLE(req_double)
		MAKE_SERIALIZABLE(req_char)
	MAKE_SERIALIZABLE_END()
};

class res {
public:
	int res_number;
	std::string res_string;
	char res_char[32];

public:
	virtual void print() {
		std::cout << res_number << "\t" << res_string << "\t" << res_char << std::endl;
	}

protected:
	MAKE_SERIALIZABLE_BEGIN()
		MAKE_SERIALIZABLE(res_number)
		MAKE_SERIALIZABLE(res_string)
		MAKE_SERIALIZABLE(res_char)
	MAKE_SERIALIZABLE_END()
};

template<class DATA_KEY_TYPE, class DATA_VALUE_TYPE>
class platform_serialization_data {
public:
	void put(DATA_KEY_TYPE _name, DATA_VALUE_TYPE _value) {
		data[_name] = _value;
	}

	void erase(DATA_KEY_TYPE _name) {
		auto iter = data.find(_name);
		if (iter != data.end())
			data.erase(iter);
	}

	void clear() {
		data.clear();
	}

	auto find(DATA_KEY_TYPE _name) {
		auto iter = data.find(_name);
		return iter;
	}

	auto begin() {
		return data.begin();
	}

	auto end() {
		return data.end();
	}

	void print_all() {
		for (auto iter = data.begin(); iter != data.end(); iter++) {
			std::cout << "first:\t" << iter->first << "\tsecond:\t";
			iter->second.print();
		}
	}
protected:
	std::map<DATA_KEY_TYPE, DATA_VALUE_TYPE> data;

	MAKE_SERIALIZABLE_BEGIN()
		MAKE_SERIALIZABLE(data)
	MAKE_SERIALIZABLE_END()
};