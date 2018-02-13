// -*- C++ -*-

#include "DCPairHitCluster.hh"
#include "DCLTrackHit.hh"
#include "DebugCounter.hh"

ClassImp(DCPairHitCluster);

//______________________________________________________________________________
DCPairHitCluster::DCPairHitCluster( DCLTrackHit *hitA, DCLTrackHit *hitB )
  : TObject(),
    m_hitA(hitA),
    m_hitB(hitB),
    m_nhits(0)
{
  if(m_hitA) ++m_nhits;
  if(m_hitB) ++m_nhits;
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
DCPairHitCluster::~DCPairHitCluster( void )
{
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
DCLTrackHit*
DCPairHitCluster::GetHit( Int_t i ) const
{
  if(i==0) return m_hitA;
  if(i==1) return m_hitB;
  return 0;
}

//______________________________________________________________________________
bool
DCPairHitCluster::IsHoneycomb( void ) const
{
  if( m_hitA ) return m_hitA->IsHoneycomb();
  if( m_hitB ) return m_hitB->IsHoneycomb();

  return false;
}

//______________________________________________________________________________
void
DCPairHitCluster::SetHoneycomb( Bool_t flag )
{
  if( m_hitA ) m_hitA->SetHoneycomb( flag );
  if( m_hitB ) m_hitB->SetHoneycomb( flag );
}
