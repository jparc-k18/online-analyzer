// -*- C++ -*-

#ifndef DC_PAIR_HIT_CLUSTER_HH
#define DC_PAIR_HIT_CLUSTER_HH

#include <TObject.h>

class DCLTrackHit;

//______________________________________________________________________________
class DCPairHitCluster : public TObject
{
public:
  DCPairHitCluster( DCLTrackHit *hitA, DCLTrackHit *hitB=nullptr );
  ~DCPairHitCluster( void );

private:
  DCPairHitCluster( const DCPairHitCluster& );
  DCPairHitCluster& operator =( const DCPairHitCluster& );

private:
  DCLTrackHit *m_hitA;
  DCLTrackHit *m_hitB;
  Int_t        m_nhits;

public:
  Int_t        NumberOfHits( void ) const { return m_nhits; }
  DCLTrackHit* GetHit( Int_t i=0 )  const;
  Bool_t       IsHoneycomb( void )  const;
  void         SetHoneycomb( Bool_t flag );

  ClassDef(DCPairHitCluster,0);
};

#endif
