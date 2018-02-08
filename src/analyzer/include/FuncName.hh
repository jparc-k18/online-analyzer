// -*- C++ -*-

// Author: Shuhei Hayakawa

#ifndef FUNC_NAME_HH
#define FUNC_NAME_HH

#include <TString.h>

#include <std_ostream.hh>

#define FUNC_NAME \
  TString("["+TString(ClassName())+"::"+__func__+"()]")

#ifdef DebugPrint
#include <escape_sequence.hh>

#define DEBUG_PRINT { \
    hddaq::cerr << hddaq::unpacker::esc::k_yellow << "#DEBUG_PRINT" \
		<< hddaq::unpacker::esc::k_default_color	    \
		<< " call " << FUNC_NAME << std::endl;		    \
  }
#else
#define DEBUG_PRINT { }
#endif

#endif
