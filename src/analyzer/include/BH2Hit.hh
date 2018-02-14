// -*- C++ -*-

#ifndef BH2_HIT_HH
#define BH2_HIT_HH

#include <TObject.h>

#include "DebugCounter.hh"
#include "Hodo1Hit.hh"
#include "Hodo2Hit.hh"

//______________________________________________________________________________
class BH2Hit : public Hodo2Hit
{
public:
  BH2Hit( void );
  BH2Hit( HodoRawHit *rhit, Int_t index=0 );
  ~BH2Hit( void );

private:
  Double_t m_time_offset;

public:
  Bool_t   Calculate( void );
  Double_t UTime0( Int_t n=0 ) const { return m_t1.at(n)  +m_time_offset; }
  Double_t UCTime0( Int_t n=0 ) const { return m_ct1.at(n) +m_time_offset; }
  Double_t DTime0( Int_t n=0 ) const { return m_t2.at(n)  +m_time_offset; }
  Double_t DCTime0( Int_t n=0 ) const { return m_ct2.at(n) +m_time_offset; }
  Double_t Time0( Int_t n=0 ) const { return 0.5*(m_t1.at(n)+m_t2.at(n)) +m_time_offset; }
  Double_t CTime0( Int_t n=0 ) const { return 0.5*(m_ct1.at(n)+m_ct2.at(n)) +m_time_offset; }

  virtual Bool_t ReCalc( Bool_t applyRecursively=false )
  { return Calculate(); };

  ClassDef(BH2Hit,0);
};

#endif
