#include "vec.h"

char* encode(int* i)
{
    char* c = new char[sizeof(int)];
    encode_in(i, c);
    return c;
}

char* encode(double* d)
{
    char* c = new char[sizeof(double)];
    encode_in(d, c);
    return c;
}

char* encode(bool* b)
{
    return new char(*b);
}

void encode_in(int* i, char* c)
{
    char* r = (char*)i;
    std::copy(r, r + sizeof(int), c);
}

void encode_in(double* d, char* c)
{
    char* r = (char*)d;
    std::copy(r, r + sizeof(double), c);
}

void encode_in(bool* b, char* c)
{
    *c = *b;
}

int decode_int(char* c)
{
    int* i = new int();
    char* r = (char*)i;
    std::copy(c, c + sizeof(int), r);
    return *i;
}

double decode_double(char* c)
{
    double* d = new double();
    char* r = (char*)d;
    std::copy(c, c + sizeof(double), r);
    return *d;
}

bool decode_bool(char* c)
{
    return *c;
}
