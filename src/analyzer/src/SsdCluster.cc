// -*- C++ -*-

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

#include <std_ostream.hh>

#include "DCGeomMan.hh"
#include "DCHit.hh"
#include "DCAnalyzer.hh"
#include "DebugCounter.hh"
#include "DeleteUtility.hh"
#include "FuncName.hh"
#include "SsdCluster.hh"

ClassImp(SsdCluster);

namespace
{
  const DCGeomMan& gGeom = DCGeomMan::GetInstance();
}

//______________________________________________________________________________
SsdCluster::SsdCluster( const DCHitContainer& HitCont )
  : m_rep_hit(0),
    m_good_for_analysis( true )
{
  m_hit_array.resize( HitCont.size() );
  std::copy( HitCont.begin(), HitCont.end(), m_hit_array.begin() );
  m_cluster_size = m_hit_array.size();

  Calculate();
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
SsdCluster::~SsdCluster( void )
{
  utility::DeleteObject( m_rep_hit );
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
void
SsdCluster::Calculate( void )
{
  if( m_cluster_size>0 )
    m_layer = m_hit_array[0]->GetLayer();

  Double_t mean_time = 0.;
  Double_t sum_de    = 0.;
  Double_t sum_amp   = 0.;
  Double_t mean_seg  = 0.;
  Double_t mean_pos  = 0.;
  Double_t mean_chi2 = 0.;

  for( Int_t i=0; i<m_cluster_size; ++i ){
    if( !m_hit_array[i] ) continue;
    Double_t de   = m_hit_array[i]->GetDe();
    Double_t amp  = m_hit_array[i]->GetAmplitude();
    Double_t time = m_hit_array[i]->GetPeakTime();
    mean_seg  += m_hit_array[i]->GetWire() * de;
    mean_time += time * de;
    sum_de    += de;
    sum_amp   += amp;
    mean_pos  += m_hit_array[i]->GetWirePosition() * de;
    mean_chi2 += m_hit_array[i]->GetChisquare();
  }

  mean_seg  /= sum_de;
  mean_time /= sum_de;
  mean_pos  /= sum_de;

  mean_chi2 /= m_cluster_size;

  m_mean_time = mean_time;
  m_de        = sum_de;
  m_amplitude = sum_amp;
  m_mean_seg  = mean_seg;
  m_mean_pos  = mean_pos;

  Double_t angle  = gGeom.GetTiltAngle( m_layer );
  Double_t localz = gGeom.GetLocalZ( m_layer );

  // repesentative hit for tracking
  m_rep_hit = new DCHit( m_layer, m_mean_seg );
  m_rep_hit->SetTdcVal(0);
  m_rep_hit->SetTiltAngle( angle );
  m_rep_hit->SetWire( m_mean_seg );
  m_rep_hit->SetWirePosition( m_mean_pos );
  m_rep_hit->SetZ( localz );
  m_rep_hit->SetDriftTime( 0. );
  m_rep_hit->SetDriftLength( 0. );
  m_rep_hit->SetSsdFlag();
  m_rep_hit->SetAmplitude( m_amplitude );
  m_rep_hit->SetDe( m_de );
  m_rep_hit->SetPeakTime( m_mean_time );
  m_rep_hit->SetChisquare( mean_chi2 );
}

//______________________________________________________________________________
Bool_t
SsdCluster::GoodForAnalysis( Bool_t status )
{
  Bool_t pre_status = m_good_for_analysis;
  m_good_for_analysis = status;
  return pre_status;
}

//______________________________________________________________________________
void
SsdCluster::Print( Option_t* ) const
{
  hddaq::cout << "#D " << FUNC_NAME << std::endl
	      << "ClusterSize     : " << m_cluster_size << std::endl
	      << "Layer           : " << m_layer        << std::endl
	      << "MeanTime        : " << m_mean_time    << std::endl
	      << "DeltaE          : " << m_de           << std::endl
	      << "Amplitude       : " << m_amplitude    << std::endl
	      << "MeanSeg         : " << m_mean_seg     << std::endl
	      << "MeanPosition    : " << m_mean_pos     << std::endl
	      << "GoodForAnalysis : " << m_good_for_analysis << std::endl;
  m_rep_hit->Print();
#if 0
  for( DCHit *hit : m_hit_array ) hit->Print();
#endif
}
