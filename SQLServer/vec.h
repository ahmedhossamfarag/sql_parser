#pragma once
#include <vector>
#include <functional>
#include <iostream>
template<typename T> int vector_sum_int(std::vector<T>& vec, std::function<int(T)> conv);

template<typename T>
int vector_sum_int(std::vector<T>& vec, std::function<int(T)> conv)
{
	int s = 0;
	for (auto i = vec.begin(); i < vec.end(); i++)
	{ 
		s += conv(*i);
	}
	return s;
}

char* encode(int* i);

char* encode(double* d);

char* encode(bool* b);

void encode_in(int* i, char* c);

void encode_in(double* d, char* c);

void encode_in(bool* b, char* c);

int decode_int(char* c);

double decode_double(char* c);

bool decode_bool(char* c);

template<class T>
std::tuple<std::vector<T>, std::vector<T>> split(std::vector<T>* vc, std::function<bool(T)> cnd);

template<class T>
inline std::tuple<std::vector<T>, std::vector<T>> split(std::vector<T>* vc, std::function<bool(T)> cnd)
{
	std::vector<T> v1, v2;
	for (size_t i = 0; i < vc->size(); i++)
	{
		T vl = vc->at(i);
		if (cnd(vl))
			v1.push_back(vl);
		else
			v2.push_back(vl);
	}
	return std::make_tuple(v1, v2);
}

template<class T>
auto find_if(std::vector<T>* vc, std::function<bool(T)> cnd);


template<class T>
inline auto find_if(std::vector<T>* vc, std::function<bool(T)> cnd)
{
	return std::find_if(vc->begin(), vc->end(), cnd);
}
;