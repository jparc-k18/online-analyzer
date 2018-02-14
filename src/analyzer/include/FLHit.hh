// -*- C++ -*-

#ifndef FL_HIT_HH
#define FL_HIT_HH

#include <vector>

#include <TObject.h>

#include "FiberHit.hh"

//______________________________________________________________________________
class FLHit : public TObject
{
public:
  // One side readout method (BFT, SFT, SCH)
  FLHit( FiberHit* ptr, Int_t index );
  // Both side readout method (FBH)
  FLHit( FiberHit* ptr1, FiberHit* ptr2,
	 Int_t index1, Int_t index2);
  ~FLHit( void );

private:
  FiberHit *m_hit_u;
  FiberHit *m_hit_d;
  Int_t     m_nth_hit_u;
  Int_t     m_nth_hit_d;
  Double_t  m_leading;
  Double_t  m_trailing;
  Double_t  m_time;
  Double_t  m_ctime;
  Double_t  m_width;
  Bool_t    m_flag_fljoin;

public:
  Double_t GetLeading( void )  const { return m_leading;  }
  Double_t GetTrailing( void ) const { return m_trailing; }
  Double_t GetTime( void )     const { return m_time;     }
  Double_t GetCTime( void )    const { return m_ctime;    }
  Double_t GetWidth( void )    const { return m_width;    }
  Double_t GetPosition( void ) const { return m_hit_u->GetPosition(); }
  Int_t    PairId( void )      const { return m_hit_u->PairId();      }
  Double_t SegmentId( void )   const { return m_hit_u->SegmentId();   }
  void     SetJoined( void )         { m_flag_fljoin = true;          }
  Bool_t   Joined( void )      const { return m_flag_fljoin;          }
  void     Dump( void )        const { Debug();                       }

  friend class FiberHit;

private:
  FLHit( void );
  FLHit( const FLHit& object );
  FLHit& operator =( const FLHit& object );

  void Initialize( void );

  void Debug( void ) const
  {
    std::cout << "plid " << m_hit_u->PairId() << " ";
    std::cout << "Pos "  << GetPosition()   << " ";
    std::cout << "Time " << GetCTime()      << std::endl;
  }

  ClassDef(FLHit,0);
};

#endif
