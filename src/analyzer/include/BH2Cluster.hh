// -*- C++ -*-

#ifndef BH2_CLUSTER_HH
#define BH2_CLUSTER_HH

#include <Rtypes.h>
#include <TObject.h>

class BH2Hit;
class HodoAnalyzer;

//______________________________________________________________________________
class BH2Cluster : public TObject
{
public:
  BH2Cluster( BH2Hit *hitA, BH2Hit *hitB=nullptr, BH2Hit *hitC=nullptr );
  ~BH2Cluster( void );

private:
  BH2Cluster( const BH2Cluster& );
  BH2Cluster& operator =( const BH2Cluster& );

private:
  BH2Hit*  m_hitA;
  BH2Hit*  m_hitB;
  BH2Hit*  m_hitC;
  Int_t    m_cluster_size;
  Double_t m_mean_time;
  Double_t m_cmean_time;
  Double_t m_de;
  Double_t m_mean_seg;
  Double_t m_time_diff;
  Double_t m_time0;
  Bool_t   m_good_for_analysis;

public:
  Int_t    ClusterSize( void ) const { return m_cluster_size; }
  Double_t MeanTime( void ) const { return m_mean_time; }
  Double_t CMeanTime( void ) const { return m_cmean_time; }
  Double_t DeltaE( void ) const { return m_de; }
  Double_t MeanSeg( void ) const { return m_mean_seg; }
  Double_t CTime0( void ) const { return m_time0; }
  Double_t TimeDif( void ) const { return m_time_diff; }
  BH2Hit*  GetHit( Int_t i ) const;
  Bool_t   GoodForAnalysis( void ) const { return m_good_for_analysis; }
  Bool_t   GoodForAnalysis( Bool_t status );
  Bool_t   ReCalc( Bool_t applyRecusively=false );

protected:
  void    Calculate( void );

  ClassDef(BH2Cluster,0);
};

#endif
