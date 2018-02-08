// -*- C++ -*-

#include <string>

#include <TString.h>

#include <std_ostream.hh>

#include "Exception.hh"
#include "FuncName.hh"

ClassImp(Exception);

//______________________________________________________________________________
Exception::Exception( const TString& msg )
  : TObject(),
    m_msg()
{
  m_msg = FUNC_NAME + " " + msg;
}

//______________________________________________________________________________
Exception::~Exception( void ) throw()
{
}

//______________________________________________________________________________
void
Exception::hoge( const TString& arg ) const
{
  hddaq::cout << "#D " << FUNC_NAME << " " << arg << std::endl;
}

//______________________________________________________________________________
const Char_t*
Exception::what( void ) const throw()
{
  return m_msg.Data();
}
