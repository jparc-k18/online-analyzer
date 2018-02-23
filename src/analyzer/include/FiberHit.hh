// -*- C++ -*-

#ifndef FIBER_HIT_HH
#define FIBER_HIT_HH

#include <string>

#include <TObject.h>

#include <std_ostream.hh>

#include "Hodo1Hit.hh"

class FLHit;

//______________________________________________________________________________
class FiberHit : public Hodo1Hit
{
public:
  explicit  FiberHit( HodoRawHit *object, const TString& name );
  virtual  ~FiberHit( void );

private:
  FiberHit( void );
  FiberHit( const FiberHit& object );
  FiberHit& operator =( const FiberHit& object );

protected:
  TString             m_detector_name;
  Int_t               m_segment;
  Double_t            m_position;
  Double_t            m_offset;
  Int_t               m_pair_id;
  Bool_t              m_status;
  std::vector<Bool_t> m_flag_join;
  std::vector<FLHit*> m_hit_container;

public:
  void     SetDetectorName( const TString& name ){ m_detector_name = name; }
  Bool_t   Calculate( void );
  Double_t GetLeading( Int_t n=0 )  const { return m_raw->GetTdc1(n);         }
  Double_t GetTrailing( Int_t n=0 ) const { return m_raw->GetTdc2(n);         }
  Double_t GetTime( Int_t n=0 )     const { return Hodo1Hit::GetT(n);         }
  Double_t GetCTime( Int_t n=0 )    const { return Hodo1Hit::GetCT(n);        }
  Double_t GetWidth( Int_t n=0 )    const { return Double_t(Hodo1Hit::GetA(n)); }
  Double_t GetTot( Int_t n=0 )      const { return GetWidth(n);               }
  Double_t GetPosition( void )  const { return m_position + m_offset;       }
  Int_t    PairId( void )       const { return m_pair_id;                   }
  Double_t SegmentId( void )    const { return m_segment;                   }
  void     SetJoined( Int_t m )           { m_flag_join.at(m) = true;         }
  Bool_t   Joined( Int_t m )        const { return m_flag_join.at(m);         }
  virtual void Print( Option_t* option="" ) const;
  void     RegisterHits( FLHit* hit )   { m_hit_container.push_back(hit);   }

  virtual Bool_t ReCalc( Bool_t allpyRecursively=false )
  { return FiberHit::Calculate(); }

  static Bool_t CompFiberHit( const FiberHit* left, const FiberHit* right );

  ClassDef(FiberHit,0);
};

//______________________________________________________________________________
inline Bool_t
FiberHit::CompFiberHit( const FiberHit* left, const FiberHit* right )
{
  return left->PairId() < right->PairId();
}

#endif
