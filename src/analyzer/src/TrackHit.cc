// -*- C++ -*-

#include <cstring>
#include <sstream>
#include <stdexcept>

#include "DCGeomMan.hh"
#include "DCLocalTrack.hh"
#include "DebugCounter.hh"
#include "TrackHit.hh"

ClassImp(TrackHit);

//______________________________________________________________________________
TrackHit::TrackHit( DCLTrackHit *hit )
  : TObject(),
    m_dcltrack_hit(hit)
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
TrackHit::~TrackHit( void )
{
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
Double_t
TrackHit::GetLocalHitPos( void ) const
{
  return m_dcltrack_hit->GetLocalHitPos();
}

//______________________________________________________________________________
Double_t
TrackHit::GetResidual( void ) const
{
  Double_t a = GetTiltAngle()*math::Deg2Rad();
  Double_t u = m_cal_global_mom.x()/m_cal_global_mom.z();
  Double_t v = m_cal_global_mom.y()/m_cal_global_mom.z();
  Double_t dsdz = u*std::cos(a)+v*std::sin(a);
  Double_t coss = IsHoneycomb() ? std::cos( std::atan(dsdz) ) : 1.;
  Double_t wp   = GetWirePosition();
  Double_t ss   = wp+(GetLocalHitPos()-wp)/coss;
  return (ss-m_cal_local_pos)*coss;
}

//______________________________________________________________________________
Double_t
TrackHit::GetTiltAngle( void ) const
{
  return m_dcltrack_hit->GetTiltAngle();
}

//______________________________________________________________________________
Bool_t
TrackHit::ReCalc( Bool_t applyRecursively )
{
  if( applyRecursively )
    return m_dcltrack_hit->ReCalc(applyRecursively);
  else
    return true;
}
