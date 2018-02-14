// -*- C++ -*-

#ifndef HODO_CLUSTER_HH
#define HODO_CLUSTER_HH

#include <TObject.h>

class Hodo2Hit;
class HodoAnalyzer;

//______________________________________________________________________________
class HodoCluster : public TObject
{
public:
  HodoCluster( Hodo2Hit *hitA, Hodo2Hit *hitB=0, Hodo2Hit *hitC=0 );
  virtual ~HodoCluster( void );

private:
  HodoCluster( const HodoCluster & );
  HodoCluster & operator = ( const HodoCluster & );

private:
  Hodo2Hit *m_hitA;
  Hodo2Hit *m_hitB;
  Hodo2Hit *m_hitC;
  Int_t     m_cluster_size;
  Double_t  m_mean_time;
  Double_t  m_de;
  Double_t  m_mean_seg;
  Double_t  m_time_diff;
  Double_t  m_1st_seg;
  Double_t  m_1st_time;
  Bool_t    m_good_for_analysis;

public:
  Hodo2Hit* GetHit( Int_t i )       const;
  Int_t     ClusterSize( void )     const { return m_cluster_size; }
  Double_t  CMeanTime( void )       const { return m_mean_time;    }
  Double_t  DeltaE( void )          const { return m_de;           }
  Double_t  MeanSeg( void )         const { return m_mean_seg;     }
  Double_t  TimeDif( void )         const { return m_time_diff;    }
  Double_t  C1stSeg( void )         const { return m_1st_seg;      }
  Double_t  C1stTime( void )        const { return m_1st_time;     }
  Bool_t    GoodForAnalysis( void ) const { return m_good_for_analysis; }
  Bool_t    GoodForAnalysis( Bool_t status )
  {
    Bool_t pre_status = m_good_for_analysis;
    m_good_for_analysis = status;
    return pre_status;
  }

  Bool_t ReCalc( Bool_t applyRecusively=false );

private:
  void Calculate( void );

  ClassDef(HodoCluster,0);
};

#endif
