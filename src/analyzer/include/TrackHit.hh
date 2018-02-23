// -*- C++ -*-

#ifndef TRACK_HIT_HH
#define TRACK_HIT_HH

#include "DCLTrackHit.hh"
#include "ThreeVector.hh"

#include <TObject.h>

class DCLocalTrack;

//______________________________________________________________________________
class TrackHit : public TObject
{
public:
  explicit TrackHit( DCLTrackHit *hit );
  ~TrackHit( void );

private:
  TrackHit( const TrackHit& );
  TrackHit& operator =( const TrackHit& );

private:
  DCLTrackHit *m_dcltrack_hit;
  ThreeVector  m_cal_global_mom;
  ThreeVector  m_cal_global_pos;
  Double_t     m_cal_local_pos;

public:
  Bool_t             IsHoneycomb( void ) const { return m_dcltrack_hit->IsHoneycomb(); }
  void               SetCalGMom( const ThreeVector& mom ) { m_cal_global_mom = mom; }
  void               SetCalGPos( const ThreeVector& pos ) { m_cal_global_pos = pos; }
  void               SetCalLPos( Double_t pos )           { m_cal_local_pos=pos; }
  DCLTrackHit*       GetHit( void ) const { return m_dcltrack_hit; }
  Double_t           GetWirePosition( void ) const { return m_dcltrack_hit->GetWirePosition(); }
  Int_t              GetLayer( void )       const { return m_dcltrack_hit->GetLayer(); }
  Double_t           GetLocalHitPos( void ) const;
  const ThreeVector& GetCalGPos( void )     const { return m_cal_global_pos; }
  Double_t           GetCalLPos( void )     const { return m_cal_local_pos; }
  Double_t           GetResidual( void )    const;
  Double_t           GetResolution( void )  const { return m_dcltrack_hit->GetResolution(); }
  Double_t           GetTiltAngle( void )   const;
  DCLTrackHit*       GetDCLTrack( void ) const { return m_dcltrack_hit; }
  Bool_t             ReCalc( Bool_t applyRecursively=false );
  // for SSD
  void               SetKaonFlag( void ){ m_dcltrack_hit->JoinKaonTrack(); }

  ClassDef(TrackHit,0);
};

#endif
