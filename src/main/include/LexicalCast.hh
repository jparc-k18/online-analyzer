// Author: Tomonori Takahashi

#ifndef CINT_LEXICAL_CAST_H
#define CINT_LEXICAL_CAST_H

#include <string>
#include <Rtypes.h>

// string -> integer / float point number functions
short
a2s(const std::string& arg);

unsigned short
a2us(const std::string& arg);

int
a2i(const std::string& arg);

long long
a2ll(const std::string& arg);

unsigned int
a2ui(const std::string& arg);

unsigned long long
a2ull(const std::string& arg);

double
a2d(const std::string& arg);


// integer / float point number -> string functions

std::string
i2a(short arg);

std::string
i2a(unsigned short arg);

std::string
i2a(int arg);

std::string
i2a(long long arg);

std::string
i2a(unsigned int arg);

std::string
i2a(unsigned long long arg);

std::string
d2a(double arg);

#endif
