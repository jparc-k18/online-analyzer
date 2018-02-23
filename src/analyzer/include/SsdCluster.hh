// -*- C++ -*-

#ifndef SSD_CLUSTER_HH
#define SSD_CLUSTER_HH

#include <TObject.h>

#include "DCAnalyzer.hh"
#include "DCHit.hh"

class DCAnalyzer;

//______________________________________________________________________________
class SsdCluster : public TObject
{
public:
  SsdCluster( const DCHitContainer& HitCont );
  ~SsdCluster( void );

private:
  SsdCluster( const SsdCluster & );
  SsdCluster & operator =( const SsdCluster & );

private:
  // if kMaxClusterSize = 0, cluster size is unlimited.
  static const UInt_t kMaxClusterSize = 0;
  DCHitContainer  m_hit_array;
  DCHit*          m_rep_hit;
  Int_t           m_cluster_size;
  Int_t           m_layer;
  Double_t        m_mean_time;
  Double_t        m_de;
  Double_t        m_amplitude;
  Double_t        m_mean_seg;
  Double_t        m_mean_pos;
  Bool_t          m_good_for_analysis;

public:
  static const UInt_t MaxClusterSize( void ) { return kMaxClusterSize; }
  DCHit*   GetHit( Int_t i )       const { return m_hit_array[i]; }
  DCHit*   GetHit( void )          const { return m_rep_hit;      }
  Int_t    ClusterSize( void )     const { return m_cluster_size; }
  Int_t    LayerId( void )         const { return m_layer;        }
  Double_t TiltAngle( void )       const { return m_rep_hit->GetTiltAngle(); }
  Double_t Time( void )            const { return m_mean_time;    }
  Double_t DeltaE( void )          const { return m_de;           }
  Double_t Amplitude( void )       const { return m_amplitude;    }
  Double_t MeanSeg( void )         const { return m_mean_seg;     }
  Double_t Position( void )        const { return m_mean_pos;     }
  Bool_t   BelongToTrack( void )   const { return m_rep_hit->BelongToTrack(); }
  Bool_t   GoodForAnalysis( void ) const { return m_good_for_analysis; }
  Bool_t   GoodForAnalysis( Bool_t status );
  void     JoinKaonTrack( void ) { m_rep_hit->JoinKaonTrack(); }
  void     QuitKaonTrack( void ) { m_rep_hit->QuitKaonTrack(); }
  Bool_t   BelongToKaonTrack( void ){ return m_rep_hit->BelongToKaonTrack(); }

  virtual void Print( Option_t* option="" ) const;

private:
  void     Calculate( void );

  ClassDef(SsdCluster,0);
};

#endif
