// -*- C++ -*-

#include "RawData.hh"
#include "EventAnalyzer.hh"

ClassImp(analyzer::EventAnalyzer);

namespace analyzer
{

//______________________________________________________________________________
EventAnalyzer::EventAnalyzer( void )
  : TObject(),
    m_raw_data( new RawData )
{
}

//______________________________________________________________________________
EventAnalyzer::~EventAnalyzer( void )
{
  delete m_raw_data;
}

//______________________________________________________________________________
void
EventAnalyzer::DecodeRawData( void )
{
  m_raw_data->DecodeHits();
}

}
