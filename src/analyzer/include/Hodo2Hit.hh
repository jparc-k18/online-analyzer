// -*- C++ -*-

#ifndef HODO2_HIT_HH
#define HODO2_HIT_HH

#include <TMath.h>
#include <TObject.h>

#include "HodoRawHit.hh"
#include "ThreeVector.hh"

class RawData;

//______________________________________________________________________________
class Hodo2Hit : public TObject
{
public:
  Hodo2Hit( void );
  Hodo2Hit( HodoRawHit *rhit, Int_t index=0 );
  virtual ~Hodo2Hit( void );

private:
  Hodo2Hit( const Hodo2Hit& );
  Hodo2Hit& operator =( const Hodo2Hit& );

protected:
  HodoRawHit*            m_raw;
  Bool_t                 m_is_calculated;
  Int_t                  m_multi_hit;
  std::vector<Double_t>  m_a1;
  std::vector<Double_t>  m_a2;
  std::vector<Double_t>  m_t1;
  std::vector<Double_t>  m_t2;
  std::vector<Double_t>  m_ct1;
  std::vector<Double_t>  m_ct2;
  Int_t                  m_index;

public:
  HodoRawHit* GetRawHit( void ) { return m_raw; }
  Int_t       DetectorId( void ) const { return m_raw->DetectorId(); }
  Int_t       PlaneId( void ) const { return m_raw->PlaneId(); }
  Int_t       SegmentId( void ) const { return m_raw->SegmentId(); }
  Bool_t      Calculate( void );
  Bool_t      IsCalculated( void ) const { return m_is_calculated; }
  Int_t       GetNumOfHit( void ) const { return m_multi_hit; }
  Double_t    GetAUp( Int_t i=0 ) const { return m_a1[i]; }
  Double_t    GetALeft( Int_t i=0 ) const { return m_a1[i]; }
  Double_t    GetADown( Int_t i=0 ) const { return m_a2[i]; }
  Double_t    GetARight( Int_t i=0 ) const { return m_a2[i]; }
  Double_t    GetTUp( Int_t i=0 ) const { return m_t1[i]; }
  Double_t    GetTLeft( Int_t i=0 ) const { return m_t1[i]; }
  Double_t    GetTDown( Int_t i=0 ) const { return m_t2[i]; }
  Double_t    GetTRight( Int_t i=0 ) const { return m_t2[i]; }
  Double_t    GetCTUp( Int_t i=0 ) const { return m_ct1[i]; }
  Double_t    GetCTLeft( Int_t i=0 ) const { return m_ct1[i]; }
  Double_t    GetCTDown( Int_t i=0 ) const { return m_ct2[i]; }
  Double_t    GetCTRight( Int_t i=0 ) const { return m_ct2[i]; }
  Double_t    MeanTime( Int_t i=0 ) const { return 0.5*(m_t1[i]+m_t2[i]); }
  Double_t    CMeanTime( Int_t i=0 ) const { return 0.5*(m_ct1[i]+m_ct2[i]); }
  Double_t    DeltaE( Int_t i=0 ) const { return TMath::Sqrt(TMath::Abs(m_a1[i]*m_a2[i])); }
  Double_t    TimeDiff( Int_t i=0 ) const { return m_ct2[i] - m_ct1[i]; }

  virtual Bool_t ReCalc( Bool_t applyRecursively=false )
  { return Calculate(); }

  ClassDef(Hodo2Hit,0);
};


#endif
