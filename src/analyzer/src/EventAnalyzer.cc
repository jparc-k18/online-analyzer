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

//______________________________________________________________________________
Bool_t
EventAnalyzer::TrackSearchBcOut( void )
{
  if( !m_dc_analyzer )
    return false;
  else
    return m_dc_analyzer->TrackSearchBcOut();
}

//______________________________________________________________________________
Int_t
EventAnalyzer::GetNTrackBcOut( void ) const
{
  if( !m_dc_analyzer )
    return -1;
  else
    return m_dc_analyzer->GetNtracksBcOut();
}

//______________________________________________________________________________
DCLocalTrack*
EventAnalyzer::GetTrackBcOut( Int_t i ) const
{
  if( !m_dc_analyzer ||
      m_dc_analyzer->GetNtracksBcOut()== 0 )
    return nullptr;
  else
    return m_dc_analyzer->GetTrackBcOut(i);
}

}
