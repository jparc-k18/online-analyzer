// -*- C++ -*-

#ifndef K18_TRACK_D2U_HH
#define K18_TRACK_D2U_HH

#include <vector>
#include <functional>

#include <TObject.h>

#include "ThreeVector.hh"

class DCLocalTrack;
class TrackHit;
class DCAnalyzer;
class DCHit;

//______________________________________________________________________________
class K18TrackD2U : public TObject
{
public:
  K18TrackD2U( Double_t LocalX, DCLocalTrack *tout, Double_t P0 );
  ~K18TrackD2U( void );

private:
  K18TrackD2U( void );
  K18TrackD2U( const K18TrackD2U & );
  K18TrackD2U& operator= ( const K18TrackD2U & );

private:
  Double_t      m_local_x;
  DCLocalTrack *m_track_out;
  Double_t      m_p0;
  Double_t      m_Xi, m_Yi, m_Ui, m_Vi;
  Double_t      m_Xo, m_Yo, m_Uo, m_Vo;
  Bool_t        m_status;
  Double_t      m_delta;
  Double_t      m_delta3rd;
  Bool_t        m_good_for_analysis;

public:
  ThreeVector   BeamMomentumD2U( void ) const;
  Bool_t        CalcMomentumD2U( void );
  Double_t      Delta( void ) const { return m_delta; }
  Double_t      Delta3rd( void ) const { return m_delta3rd; }
  Double_t      GetChiSquare( void ) const;
  Bool_t        GoodForAnalysis( Bool_t status );
  Bool_t        GoodForAnalysis( void ) const { return m_good_for_analysis; }
  Double_t      P( void ) const { return m_p0*(1.+m_delta); }
  Double_t      P3rd( void ) const { return m_p0*(1.+m_delta3rd); }
  Bool_t        StatusD2U( void ) const { return m_status; }
  DCLocalTrack* TrackOut( void ) { return m_track_out; }
  Double_t      Uin( void )  const { return m_Ui; }
  Double_t      Uout( void ) const { return m_Uo; }
  Double_t      Utgt( void ) const;
  Double_t      Vin( void )  const { return m_Vi; }
  Double_t      Vout( void ) const { return m_Vo; }
  Double_t      Vtgt( void ) const;
  Double_t      Xin( void )  const { return m_Xi; }
  Double_t      Xout( void ) const { return m_Xo; }
  Double_t      Xtgt( void ) const;
  Double_t      Yin( void )  const { return m_Yi; }
  Double_t      Yout( void ) const { return m_Yo; }
  Double_t      Ytgt( void ) const;
  Bool_t        ReCalc( Bool_t applyRecursively=false );

  ClassDef(K18TrackD2U,0);
};

#endif
