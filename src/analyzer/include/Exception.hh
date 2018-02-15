// -*- C++ -*-

#ifndef EXCEPTION_HH
#define EXCEPTION_HH

#include <exception>
#include <stdexcept>
#include <string>

#include <TObject.h>
#include <TString.h>

  //______________________________________________________________________________
class Exception : public std::exception, TObject
{
public:
  Exception( const TString& msg );
  virtual ~Exception( void ) throw();

private:
  TString m_msg;

public:
  virtual void          hoge( const TString& arg="" ) const;
  virtual const Char_t* what( void ) const throw();

  ClassDef(Exception,0);
};

#endif
