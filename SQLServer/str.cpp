#include "str.h"
#include <algorithm>
#include <iostream>
bool char_is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool char_is_num(char c)
{
    return c >= '0' && c <= '9';
}

bool char_is_word(char c)
{
    return char_is_alpha(c) || char_is_num(c) || c == '_';
}

char char_to_lower(char c)
{
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 'a';
    return c;
}

bool str_start_with(std::string source, std::string str)
{
    if (str.length() > source.length())
        return false;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] != source[i])
            return false;
    }
    return true;
}

bool str_start_with_i(std::string source, std::string str)
{

    if (str.length() > source.length())
        return false;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (char_to_lower(str[i]) != char_to_lower(source[i]))
            return false;
    }
    return true;
}

bool str_equal_i(std::string source, std::string str)
{
    return source.length() == str.length() && str_start_with_i(source, str);
}

int str_to_int(std::string s) {
    int x = 0;
    for (auto i = s.begin(); i < s.end(); i++)
    {
        x = (x * 10) + (*i - '0');
    }
    return x;
}

double str_to_float(std::string s) {
    double f = 0;
    auto dot = std::find(s.begin(), s.end(), '.');
    f += str_to_int(s.substr(0, dot - s.begin()));
    if (dot == s.end())
        return f;
    double d = 10;
    for (auto i = dot + 1; i < s.end(); i++)
    {
        f += (*i - '0') / d;
        d *= 10;
    }
    return f;
}

bool str_to_bool(std::string s)
{
    if (s == "1" || str_equal_i(s, "true"))
        return true;
    return false;
}

bool str_is_int(std::string s)
{
    
    return std::all_of(s.begin(), s.end(), [](char c) {return char_is_num(c); });
}

bool str_is_float(std::string s) {
    auto dot =  std::find(s.begin(), s.end(), '.');
    return std::all_of(s.begin(), dot, [](char c) {return char_is_num(c); }) && (dot == s.end() ||
        std::all_of(dot + 1, s.end(), [](char c) {return char_is_num(c); }));
}

bool str_is_bool(std::string s)
{
    return s == "0" || s == "1" || str_equal_i(s, "true") || str_equal_i(s, "false");
}

bool str_is_date(std::string s)
{
    auto dash1 = std::find(s.begin(), s.end(), '-');
    if (s.end() - dash1 < 3)
        return false;
    auto dash2 = std::find(dash1 + 1, s.end(), '-');
    if (dash2 == s.end() || dash2 - dash1 > 3 || s.end() - dash2 > 3)
        return false;

    return std::all_of(s.begin(), dash1, [](char c) {return char_is_num(c); }) &&
        std::all_of(dash1 + 1, dash2, [](char c) {return char_is_num(c); }) &&
        std::all_of(dash2 + 1, s.end(), [](char c) {return char_is_num(c); });
}

std::string int_to_string(int i)
{
    std::string s;
    while (i != 0) {
        s.push_back((i % 10) + '0');
        i /= 10;
    }
    std::reverse(s.begin(), s.end());
    return s;
}

std::string bool_to_string(bool i)
{
    return i ? "true" : "false";
}

std::string float_to_string(double i)
{
    int n = (int)i;
    auto s = int_to_string(n);
    s.push_back('.');
    i -= n;
    while (i >= 0.0000001)
    {
        i *= 10;
        n = (int)i;
        s.push_back(n + '0');
        i -= n;
    }
    return s;
}

int compare_char(char* c1, char* c2, int size)
{
    for (size_t i = 0; i < size; i++)
    {
        if (c1[i] != c2[i])
            return c1[i] - c2[i];
    }
    return 0;
}



