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

typedef struct __test_struct {
	int the_number;
	float the_float;
	std::string the_string;
	double the_double;
	char the_char[32];

protected:
	/*friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & the_number;
		ar & the_float;
		ar & the_string;
		ar & the_double;
		ar & the_char;
	}*/
	MAKE_SERIALIZABLE_BEGIN()
		MAKE_SERIALIZABLE(the_number)
		MAKE_SERIALIZABLE(the_float)
		MAKE_SERIALIZABLE(the_string)
		MAKE_SERIALIZABLE(the_double)
		MAKE_SERIALIZABLE(the_char)
	MAKE_SERIALIZABLE_END()
} test_struct;

typedef struct __test_struct1 {
	int the_number;
	std::string the_string;
	double the_double;
	char the_char[32];
} test_struct1;

template<class DATA_KEY_TYPE, class DATA_VALUE_TYPE>
class platform_serialization_data {
public:
	void put(DATA_KEY_TYPE _name, DATA_VALUE_TYPE _value) {
		//data.insert(_name, _value);
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
		auto iter = data.begin();
		for (; iter != data.end(); iter++) {
			std::cout << "first:\t" << iter->first << "\tsecond:\t" << iter->second << std::endl;
		}
	}
protected:
	std::map<DATA_KEY_TYPE, DATA_VALUE_TYPE> data;

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & data;
	}
};