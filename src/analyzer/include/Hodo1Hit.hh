// -*- C++ -*-

#ifndef HODO1_HIT_HH
#define HODO1_HIT_HH

#include <TObject.h>

#include "HodoRawHit.hh"

class RawData;

//______________________________________________________________________________
class Hodo1Hit : public TObject
{
public:
  Hodo1Hit( HodoRawHit *rhit, Int_t index=0 );
  virtual ~Hodo1Hit( void );

private:
  Hodo1Hit( const Hodo1Hit& );
  Hodo1Hit& operator =( const Hodo1Hit& );

protected:
  HodoRawHit*            m_raw;
  Bool_t                 m_is_calculated;
  Int_t                  m_multi_hit;
  std::vector<Double_t>  m_a;
  std::vector<Double_t>  m_t;
  std::vector<Double_t>  m_ct;
  Int_t                  m_index;

public:
  HodoRawHit* GetRawHit( void ) { return m_raw; }
  Bool_t      Calculate( void );
  Bool_t      IsCalculated( void ) const { return m_is_calculated; }
  Int_t       GetNumOfHit(void) const { return m_multi_hit; };
  Double_t    GetA( Int_t i=0 )   const { return m_a[i]; }
  Double_t    GetT( Int_t i=0 )   const { return m_t[i]; }
  Double_t    GetCT( Int_t i=0 )  const { return m_ct[i]; }
  Double_t    Time( Int_t i=0 )   const { return GetT(i); }
  Double_t    CTime( Int_t i=0 )  const { return GetCT(i); }
  Double_t    DeltaE( Int_t i=0 ) const { return GetA(i); }
  Double_t    GetAUp( Int_t i=0 )     const { return m_a[i]; }
  Double_t    GetALeft( Int_t i=0 )   const { return m_a[i]; }
  Double_t    GetADown( Int_t i=0 )   const { return m_a[i]; }
  Double_t    GetARight( Int_t i=0 )  const { return m_a[i]; }
  Double_t    GetTUp( Int_t i=0 )     const { return m_t[i]; }
  Double_t    GetTLeft( Int_t i=0 )   const { return m_t[i]; }
  Double_t    GetTDown( Int_t i=0 )   const { return m_t[i]; }
  Double_t    GetTRight( Int_t i=0 )  const { return m_t[i]; }
  Double_t    GetCTUp( Int_t i=0 )    const { return m_ct[i]; }
  Double_t    GetCTLeft( Int_t i=0 )  const { return m_ct[i]; }
  Double_t    GetCTDown( Int_t i=0 )  const { return m_ct[i]; }
  Double_t    GetCTRight( Int_t i=0 ) const { return m_ct[i]; }
  Double_t    MeanTime( Int_t i=0 )  const { return GetT(i); }
  Double_t    CMeanTime( Int_t i=0 ) const { return GetCT(i); }
  Int_t       DetectorId( void ) const { return m_raw->DetectorId(); }
  Int_t       PlaneId( void )    const { return m_raw->PlaneId(); }
  Int_t       SegmentId( void )  const { return m_raw->SegmentId(); }

  virtual Bool_t ReCalc( Bool_t applyRecursively=false )
  { return Calculate(); }

  ClassDef(Hodo1Hit,0);
};

#endif
