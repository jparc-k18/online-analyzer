// -*- C++ -*-

// Author: Shuhei Hayakawa

#ifndef FUNC_NAME_HH
#define FUNC_NAME_HH

#include <TString.h>

#include <std_ostream.hh>

#define FUNC_NAME \
  TString("["+TString(ClassName())+"::"+__func__+"()]")

#endif
