// -*- C++ -*-

#ifndef FIBER_CLUSTER_HH
#define FIBER_CLUSTER_HH

#include <vector>

#include <TObject.h>

class FLHit;

//______________________________________________________________________________
class FiberCluster : public TObject
{
public:
  FiberCluster( void );
  virtual ~FiberCluster( void );

private:
  FiberCluster( const FiberCluster& );
  FiberCluster& operator =( const FiberCluster& );

protected:
  enum FlagsFiber { kInitialized, kGoodForAnalysis, kNFlagsFiber };
  std::vector<FLHit*> m_hit_container;
  Int_t               m_cluster_size;
  Int_t               m_cluster_id;
  Double_t            m_mean_time;
  Double_t            m_real_mean_time; // real mean (not a closest value of CTime)
  Double_t            m_max_width;
  Double_t            m_mean_seg;
  Double_t            m_mean_pos;
  std::vector<Bool_t> m_flag;

public:
  Bool_t   Calculate( void );
  void     push_back( FLHit* hit ) { m_hit_container.push_back(hit); };
  Int_t    VectorSize( void ) const { return m_hit_container.size(); }
  Int_t    ClusterId( void ) const { return m_cluster_id; }
  Int_t    ClusterSize( void ) const { return m_cluster_size; }
  Double_t CMeanTime( void ) const { return m_mean_time; }
  Double_t RCMeanTime( void ) const { return m_real_mean_time; }
  Double_t Width( void ) const { return m_max_width; }
  Double_t Tot( void ) const { return Width(); }
  Double_t MeanPosition( void ) const { return m_mean_pos; }
  Double_t MeanSeg( void ) const { return m_mean_seg; }
  FLHit*   GetHit( Int_t i ) const;
  Bool_t   GoodForAnalysis( void ) const { return m_flag[kGoodForAnalysis]; }
  Bool_t   GoodForAnalysis( Bool_t status );
  Int_t    PlaneId( void ) const;
  Bool_t   ReCalc( Bool_t applyRecusively=false );

private:
  void CalculateWithoutWidth( void );
  void CalculateWithWidth( void );
  void Debug( void );

  ClassDef(FiberCluster,0);
};

#endif
