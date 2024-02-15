#pragma once
#include <string>
#include <functional>

bool char_is_alpha(char c);

bool char_is_num(char c);

bool char_is_word(char c);

char char_to_lower(char c);

bool str_start_with(std::string source, std::string str);

bool str_start_with_i(std::string source, std::string str);

bool str_equal_i(std::string source, std::string str);

int str_to_int(std::string s);

double str_to_float(std::string s);

bool str_to_bool(std::string s);

bool str_is_int(std::string s);

bool str_is_float(std::string s);

bool str_is_bool(std::string s);

bool str_is_date(std::string s);

std::string int_to_string(int i);

std::string bool_to_string(bool i);

std::string float_to_string(double i);

int compare_char(char* c1, char* c2, int size);