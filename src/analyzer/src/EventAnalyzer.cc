// -*- C++ -*-

#include "BH2Filter.hh"
#include "DCAnalyzer.hh"
#include "EventAnalyzer.hh"
#include "HodoAnalyzer.hh"
#include "RawData.hh"
#include "UserParamMan.hh"

ClassImp(analyzer::EventAnalyzer);

namespace analyzer
{

namespace
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
}

//______________________________________________________________________________
EventAnalyzer::EventAnalyzer( void )
  : TObject(),
    m_bh2filter_is_applied( false ),
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
EventAnalyzer::ApplyBH2Filter( void )
{
  BH2Filter& gBH2Filter = BH2Filter::GetInstance();
  gBH2Filter.Apply( *m_hodo_analyzer, *m_dc_analyzer, m_bh2filter_list );
  m_bh2filter_is_applied = true;
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
void
EventAnalyzer::TimeCutBFT( void )
{
  static const Double_t MinTimeBFT = gUser.GetParameter("TimeBFT", 0);
  static const Double_t MaxTimeBFT = gUser.GetParameter("TimeBFT", 1);
  m_hodo_analyzer->TimeCutBFT(MinTimeBFT, MaxTimeBFT);
}

//______________________________________________________________________________
Bool_t
EventAnalyzer::TrackSearchBcOut( void )
{
  if( !m_dc_analyzer )
    return false;
  if( !m_bh2filter_is_applied )
    return m_dc_analyzer->TrackSearchBcOut();
  else
    return m_dc_analyzer->TrackSearchBcOut( m_bh2filter_list );
}

}
