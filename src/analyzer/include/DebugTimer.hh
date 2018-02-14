// -*- C++ -*-

#ifndef DEBUG_TIMER_HH
#define DEBUG_TIMER_HH

#include <TObject.h>
#include <TString.h>

class TTimeStamp;

//_____________________________________________________________________
namespace debug
{

class Timer : public TObject
{
public:
  Timer( const TString& msg="", Bool_t verbose=false );
  ~Timer( void );

private:
  Timer( const Timer& );
  Timer& operator=( const Timer& );

private:
  TTimeStamp* m_start;
  TTimeStamp* m_stop;
  TString     m_msg;
  Bool_t      m_verbose;

public:
  Int_t Sec( void ) const;
  Int_t NanoSec( void ) const;
  void  Stop( void );
  void  PrintVerbose( Option_t* option="" ) const;

  virtual void Print( Option_t* option="" ) const;

  ClassDef(Timer,0);
};

}

#endif
