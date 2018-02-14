// -*- C++ -*-

#include <ctime>
#include <iomanip>
#include <iostream>

#include <TTimeStamp.h>

#include <std_ostream.hh>

#include "DebugCounter.hh"
#include "DebugTimer.hh"

ClassImp(debug::Timer);

namespace debug
{

//______________________________________________________________________________
Timer::Timer( const TString& msg, Bool_t verbose )
  : TObject(),
    m_start( new TTimeStamp ),
    m_stop(),
    m_msg(msg),
    m_verbose(verbose)
{
  m_start->Add( -TTimeStamp::GetZoneOffset() );
  ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
Timer::~Timer( void )
{
  if( !m_stop ){
    Stop();
    if( m_verbose )
      PrintVerbose("s");
    else
      Print();
  }
  delete m_stop;
  delete m_start;
  ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
Int_t
Timer::Sec( void ) const
{
  if ( m_start && m_stop )
    return m_stop->GetSec() - m_start->GetSec();
  else
    return 0;
}

//______________________________________________________________________________
Int_t
Timer::NanoSec( void ) const
{
  if ( m_start && m_stop )
    return m_stop->GetNanoSec() - m_start->GetNanoSec();
  else
    return 0;
}

//______________________________________________________________________________
void
Timer::Print( Option_t* ) const
{
  hddaq::cout << "#DTimer " << m_msg << " "
	      << std::setw(10) << (Sec()*1.e9 + NanoSec()) << " nsec ("
	      << (Sec() + NanoSec()*1.e-9) << " sec)" << std::endl;
}

//______________________________________________________________________________
void
Timer::PrintVerbose( Option_t* option ) const
{
  const Int_t t = Sec();
  hddaq::cout << "#DTimer " << m_msg << std::endl
	      << "   Process Start   : " << m_start->AsString(option) << std::endl
	      << "   Process Stop    : " << m_stop->AsString(option) << std::endl
	      << "   Processing Time : " << t/3600 << ":"
	      << std::setfill('0') << std::setw(2) << std::right
	      << (t/60)%60 << ":"
	      << std::setfill('0') << std::setw(2) << std::right
	      << t%60 << std::endl;
}

//______________________________________________________________________________
void
Timer::Stop( void )
{
  m_stop = new TTimeStamp;
  m_stop->Add( -TTimeStamp::GetZoneOffset() );
}

}
