// Author: Tomonori Takahashi

#include "LexicalCast.hh"

#include "lexical_cast.hh"

//______________________________________________________________________________
short
a2s(const std::string& arg)
{
  return hddaq::a2s(arg);
}

//______________________________________________________________________________
unsigned short
a2us(const std::string& arg)
{
  return hddaq::a2us(arg);
}

//______________________________________________________________________________
int
a2i(const std::string& arg)
{
  return hddaq::a2i(arg);
}

//______________________________________________________________________________
long long
a2ll(const std::string& arg)
{
  return hddaq::a2ll(arg);
}

//______________________________________________________________________________
unsigned int
a2ui(const std::string& arg)
{
  return hddaq::a2ui(arg);
}

//______________________________________________________________________________
unsigned long long
a2ull(const std::string& arg)
{
  return hddaq::a2ull(arg);
}

//______________________________________________________________________________
double
a2d(const std::string& arg)
{
  return hddaq::a2d(arg);
}


//______________________________________________________________________________
//______________________________________________________________________________
std::string
i2a(short arg)
{
  return hddaq::i2a(arg);
}

//______________________________________________________________________________
std::string
i2a(unsigned short arg)
{
  return hddaq::i2a(arg);
}

//______________________________________________________________________________
std::string
i2a(int arg)
{
  return hddaq::i2a(arg);
}

//______________________________________________________________________________
std::string
i2a(long long arg)
{
  return hddaq::i2a(arg);
}

//______________________________________________________________________________
std::string
i2a(unsigned int arg)
{
  return hddaq::i2a(arg);
}

//______________________________________________________________________________
std::string
i2a(unsigned long long arg)
{
  return hddaq::i2a(arg);
}

//______________________________________________________________________________
std::string
d2a(double arg)
{
  return hddaq::d2a(arg);
}
