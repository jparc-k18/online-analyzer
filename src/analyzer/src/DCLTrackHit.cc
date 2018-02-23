// -*- C++ -*-

#include <cmath>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <std_ostream.hh>

#include "DCAnalyzer.hh"
#include "DCLTrackHit.hh"
#include "DebugCounter.hh"
#include "MathTools.hh"

ClassImp(DCLTrackHit);

//______________________________________________________________________________
DCLTrackHit::DCLTrackHit( DCHit *hit, Double_t pos, Int_t nh )
  : TObject(),
    m_hit(hit),
    m_nth_hit(nh),
    m_local_hit_pos(pos),
    m_cal_pos(-9999.),
    m_xcal(-9999.),
    m_ycal(-9999.),
    m_ucal(-9999.),
    m_vcal(-9999.),
    m_honeycomb(false)
{
  m_hit->RegisterHits(this);
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
DCLTrackHit::DCLTrackHit( const DCLTrackHit& right )
  : TObject(),
    m_hit(right.m_hit),
    m_nth_hit(right.m_nth_hit),
    m_local_hit_pos(right.m_local_hit_pos),
    m_cal_pos(right.m_cal_pos),
    m_xcal(right.m_xcal),
    m_ycal(right.m_ycal),
    m_ucal(right.m_ucal),
    m_vcal(right.m_vcal),
    m_honeycomb(right.m_honeycomb)
{
  m_hit->RegisterHits(this);
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
DCLTrackHit::~DCLTrackHit( void )
{
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
Double_t
DCLTrackHit::GetLocalCalPos( void ) const
{
  Double_t angle = m_hit->GetTiltAngle()*math::Deg2Rad();
  return m_xcal*TMath::Cos(angle) + m_ycal*TMath::Sin(angle);
}

//______________________________________________________________________________
Double_t
DCLTrackHit::GetResidual( void ) const
{
  Double_t a    = GetTiltAngle()*math::Deg2Rad();
  Double_t dsdz = m_ucal*TMath::Cos(a)+m_vcal*TMath::Sin(a);
  Double_t coss = m_honeycomb ? 1./TMath::Sqrt( 1. + dsdz*dsdz ) : 1.;
  Double_t scal = GetLocalCalPos();
  Double_t wp   = GetWirePosition();
  // Double_t ss   = wp+(m_local_hit_pos-wp)/coss;
  return (m_local_hit_pos-wp) - (scal-wp)*coss;
  //return (ss-scal)*coss;
}

//______________________________________________________________________________
void
DCLTrackHit::Print( Option_t* option ) const
{
  m_hit->Print( option );
  hddaq::cout << "local_hit_pos " << m_local_hit_pos << std::endl
	      << "residual "      << GetResidual()   << std::endl
	      << "honeycomb "     << m_honeycomb     << std::endl;
}

//______________________________________________________________________________
Bool_t
DCLTrackHit::ReCalc( Bool_t applyRecursively )
{
  if( applyRecursively ){
    if( !m_hit->ReCalcDC(applyRecursively) )
      return false;
    // if( !m_hit->ReCalcDC(applyRecursively)   ||
    // 	!m_hit->ReCalcMWPC(applyRecursively) ){
    //   return false;
    // }
  }

  Double_t wp = GetWirePosition();
  Double_t dl = GetDriftLength();

  m_local_hit_pos = m_local_hit_pos>wp ? wp+dl : wp-dl;

  return true;
}
