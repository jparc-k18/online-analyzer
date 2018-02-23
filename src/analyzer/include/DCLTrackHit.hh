// -*- C++ -*-

#ifndef DC_LTRACK_HIT_HH
#define DC_LTRACK_HIT_HH

#include <TObject.h>

#include "DCHit.hh"
#include "MathTools.hh"

class DCAnalyzer;

//______________________________________________________________________________
class DCLTrackHit : public TObject
{
public:
  DCLTrackHit( DCHit *hit, Double_t pos, Int_t nh );
  DCLTrackHit( const DCLTrackHit& right );

private:
  ~DCLTrackHit( void );

private:
  DCHit    *m_hit;
  Int_t     m_nth_hit;
  Double_t  m_local_hit_pos;
  Double_t  m_cal_pos;
  Double_t  m_xcal;
  Double_t  m_ycal;
  Double_t  m_ucal;
  Double_t  m_vcal;
  Bool_t    m_honeycomb;

public:
  void     SetLocalHitPos( Double_t xl ) { m_local_hit_pos = xl; }
  void     SetCalPosition( Double_t x, Double_t y ) { m_xcal = x; m_ycal = y; }
  void     SetCalUV( Double_t u, Double_t v ) { m_ucal = u; m_vcal = v; }
  void     SetHoneycomb( Bool_t flag=true ) { m_honeycomb = flag;     }
  Bool_t   IsHoneycomb( void ) const { return m_honeycomb; }
  Int_t    GetLayer( void ) const { return m_hit->GetLayer(); }
  Double_t GetWire( void ) const { return m_hit->GetWire(); }
  Int_t    GetTdcVal( void ) const { return m_hit->GetTdcVal(m_nth_hit); }
  Int_t    GetTdcSize( void ) const { return m_hit->GetTdcSize(); }
  Double_t GetDriftTime( void ) const { return m_hit->GetDriftTime(m_nth_hit); }
  void     ClearDriftTime( void ) { return m_hit->ClearDriftTime(); }
  void     SetDriftTime( Double_t dt ) { return m_hit->SetDriftTime(dt); }
  Double_t GetDriftLength( void ) const { return m_hit->GetDriftLength(m_nth_hit); }
  void     ClearDriftLength( void ) { return m_hit->ClearDriftLength(); }
  void     SetDriftLength( Double_t dl )  { return m_hit->SetDriftLength(dl); }
  DCHit*   GetHit( void ) const { return m_hit; }
  Double_t GetTiltAngle( void ) const { return m_hit->GetTiltAngle(); }
  Double_t GetWirePosition( void ) const { return m_hit->GetWirePosition(); }
  Double_t GetLocalHitPos( void ) const { return m_local_hit_pos; }
  Double_t GetLocalCalPos( void ) const;
  Double_t GetXcal( void ) const { return m_xcal; }
  Double_t GetYcal( void ) const { return m_ycal; }
  Double_t GetUcal( void ) const { return m_ucal; }
  Double_t GetVcal( void ) const { return m_vcal; }
  Double_t GetResidual( void ) const;
  Double_t GetResolution( void ) const { return m_hit->GetResolution(); }
  ///// for XUV tracking
  void     SetLocalCalPosVXU( Double_t xcl ) { m_cal_pos=xcl; }
  Double_t GetLocalCalPosVXU( void ) const { return m_cal_pos; }
  Double_t GetResidualVXU( void ) const { return m_local_hit_pos-m_cal_pos; }
  ///// for SSD
  Bool_t   IsSsd( void ) const { return m_hit->IsSsd(); }
  Double_t GetAdcPeakHeight( void ) const { return m_hit->GetAdcPeakHeight(); }
  Double_t GetAmplitude( void ) const { return m_hit->GetAmplitude(); }
  Double_t GetDe( void ) const { return m_hit->GetDe();        }
  Double_t GetChisquare( void ) const { return m_hit->GetChisquare(); }
  void     JoinKaonTrack( void ) { m_hit->JoinKaonTrack(); }
  void     QuitKaonTrack( void ) { m_hit->QuitKaonTrack(); }
  Bool_t   BelongToKaonTrack( void ) const { return m_hit->BelongToKaonTrack(); }
  ///// for TOF
  Double_t GetZ( void ) const { return m_hit->GetZ(); }

  void   JoinTrack( void ) { m_hit->JoinTrack(m_nth_hit); }
  void   QuitTrack( void ) { m_hit->QuitTrack(m_nth_hit); }
  Bool_t BelongToTrack( void ) const { return m_hit->BelongToTrack(m_nth_hit); }
  Bool_t ReCalc( Bool_t applyRecursively=false );

  virtual void Print( Option_t* option="" ) const;

  friend class DCHit;

  ClassDef(DCLTrackHit,0);
};

#endif
