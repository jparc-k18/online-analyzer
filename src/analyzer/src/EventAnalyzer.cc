// -*- C++ -*-

#include "RawData.hh"
#include "DCAnalyzer.hh"
#include "EventAnalyzer.hh"
#include "HodoAnalyzer.hh"

ClassImp(analyzer::EventAnalyzer);

namespace analyzer
{

//______________________________________________________________________________
EventAnalyzer::EventAnalyzer( void )
  : TObject(),
    m_raw_data( new RawData ),
    m_dc_analyzer( new DCAnalyzer ),
    m_hodo_analyzer( new HodoAnalyzer )
{
}

//______________________________________________________________________________
EventAnalyzer::~EventAnalyzer( void )
{
  delete m_raw_data;
  delete m_dc_analyzer;
  delete m_hodo_analyzer;
}

//______________________________________________________________________________
void
EventAnalyzer::DecodeRawData( void )
{
  m_raw_data->DecodeHits();
}

//______________________________________________________________________________
void
EventAnalyzer::DecodeDCAnalyzer( void )
{
  m_dc_analyzer->DecodeRawHits( m_raw_data );
}

//______________________________________________________________________________
void
EventAnalyzer::DecodeHodoAnalyzer( void )
{
  m_hodo_analyzer->DecodeRawHits( m_raw_data );
}

//______________________________________________________________________________
Bool_t
EventAnalyzer::TrackSearchBcOut( void )
{
  if( !m_dc_analyzer )
    return false;
  else
    return m_dc_analyzer->TrackSearchBcOut();
}

}
