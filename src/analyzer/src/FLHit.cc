// -*- C++ -*-

#include <string>

#include "DebugCounter.hh"
#include "FLHit.hh"

ClassImp(FLHit);

//______________________________________________________________________________
// One side readout method (BFT, SFT, SCH)
FLHit::FLHit( FiberHit* hit, Int_t index )
  : m_hit_u(hit), m_hit_d(NULL),
    m_nth_hit_u(index), m_nth_hit_d(-1)
{
  m_hit_u->RegisterHits(this);
  m_hit_u->SetJoined(index);
  Initialize();
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
// Both side readout method (FBH)
FLHit::FLHit( FiberHit* hit1,  FiberHit* hit2,
	      Int_t index1, Int_t index2)
 : m_hit_u(hit1), m_hit_d(hit2),
   m_nth_hit_u(index1), m_nth_hit_d(index2)
{
  m_hit_u->RegisterHits(this);
  m_hit_u->SetJoined(index1);
  m_hit_d->SetJoined(index2);
  Initialize();
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
FLHit::~FLHit( void )
{
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
void
FLHit::Initialize( void )
{
  if( m_hit_d == NULL ){
    m_leading  = m_hit_u->GetLeading(m_nth_hit_u);
    m_trailing = m_hit_u->GetTrailing(m_nth_hit_u);
    m_time     = m_hit_u->GetTime(m_nth_hit_u);
    m_ctime    = m_hit_u->GetCTime(m_nth_hit_u);
    m_width    = m_hit_u->GetWidth(m_nth_hit_u);
  }else{
    m_leading  =
      (m_hit_u->GetLeading(m_nth_hit_u) + m_hit_d->GetLeading(m_nth_hit_d))/2;
    m_trailing  =
      (m_hit_u->GetTrailing(m_nth_hit_u) + m_hit_d->GetTrailing(m_nth_hit_d))/2;
    m_time  =
      (m_hit_u->GetTime(m_nth_hit_u) + m_hit_d->GetTime(m_nth_hit_d))/2;
    m_ctime  =
      (m_hit_u->GetCTime(m_nth_hit_u) + m_hit_d->GetCTime(m_nth_hit_d))/2;
    m_width  =
      (m_hit_u->GetWidth(m_nth_hit_u) + m_hit_d->GetWidth(m_nth_hit_d))/2;
  }

  m_flag_fljoin = false;
}
