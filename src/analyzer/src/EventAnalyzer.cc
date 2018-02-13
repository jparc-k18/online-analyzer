// -*- C++ -*-

#include "RawData.hh"
#include "DCAnalyzer.hh"
#include "EventAnalyzer.hh"

ClassImp(analyzer::EventAnalyzer);

namespace analyzer
{

//______________________________________________________________________________
EventAnalyzer::EventAnalyzer( void )
  : TObject(),
    m_raw_data( new RawData ),
    m_dc_analyzer( new DCAnalyzer )
{
}

//______________________________________________________________________________
EventAnalyzer::~EventAnalyzer( void )
{
  delete m_raw_data;
  delete m_dc_analyzer;
}

//______________________________________________________________________________
void
EventAnalyzer::DecodeRawData( void )
{
  m_raw_data->DecodeHits();
}

//______________________________________________________________________________
void
EventAnalyzer::DecodeDCRawHits( void )
{
  m_dc_analyzer->DecodeRawHits( m_raw_data );
}

}
