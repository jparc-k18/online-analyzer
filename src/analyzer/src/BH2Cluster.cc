// -*- C++ -*-

#include <cmath>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "BH2Cluster.hh"
#include "BH2Hit.hh"
#include "DebugCounter.hh"
#include "FuncName.hh"
#include "HodoAnalyzer.hh"

ClassImp(BH2Cluster);

//______________________________________________________________________________
BH2Cluster::BH2Cluster( BH2Hit *hitA, BH2Hit *hitB, BH2Hit *hitC )
  : TObject(),
    m_hitA(hitA),
    m_hitB(hitB),
    m_hitC(hitC),
    m_cluster_size(0),
    m_good_for_analysis(true)
{
  if(hitA) ++m_cluster_size;
  if(hitB) ++m_cluster_size;
  if(hitC) ++m_cluster_size;
  Calculate();
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
BH2Cluster::~BH2Cluster( void )
{
  debug::ObjectCounter::Decrease(ClassName());
};

//______________________________________________________________________________
BH2Hit*
BH2Cluster::GetHit( Int_t i ) const
{
  if(i==0) return m_hitA;
  if(i==1) return m_hitB;
  if(i==2) return m_hitC;
  return 0;
}

//______________________________________________________________________________
void
BH2Cluster::Calculate( void )
{
  Double_t ms=0., mt=0., ct=0., de=0., t0=0., dt=0;
  if( m_hitA ){
    ms += m_hitA->SegmentId();
    mt += m_hitA->MeanTime();
    ct += m_hitA->CMeanTime();
    de += m_hitA->DeltaE();
    t0 += m_hitA->CTime0();
    dt += ( m_hitA->GetTDown() - m_hitA->GetTUp() );
  }
  if( m_hitB ){
    ms += m_hitB->SegmentId();
    mt += m_hitB->MeanTime();
    ct += m_hitB->CMeanTime();
    de += m_hitB->DeltaE();
    t0 += m_hitB->CTime0();
    dt += ( m_hitB->GetTDown() - m_hitB->GetTUp() );
  }
  if( m_hitC ){
    ms += m_hitC->SegmentId();
    mt += m_hitC->MeanTime();
    ct += m_hitC->CMeanTime();
    de += m_hitC->DeltaE();
    t0 += m_hitA->CTime0();
    dt += ( m_hitC->GetTDown() - m_hitB->GetTUp() );
  }

  ms /= Double_t(m_cluster_size);
  mt /= Double_t(m_cluster_size);
  ct /= Double_t(m_cluster_size);
  t0 /= Double_t(m_cluster_size);
  dt /= Double_t(m_cluster_size);

  m_mean_seg   = ms;
  m_mean_time  = mt;
  m_cmean_time = ct;
  m_de         = de;
  m_time0      = t0;
  m_time_diff  = dt;

}

//______________________________________________________________________________
Bool_t
BH2Cluster::GoodForAnalysis( Bool_t status )
{
  Bool_t pre_status = m_good_for_analysis;
  m_good_for_analysis = status;
  return pre_status;
}

//______________________________________________________________________________
Bool_t
BH2Cluster::ReCalc( Bool_t applyRecursively )
{
  if( applyRecursively ){
    if(m_hitA) m_hitA->ReCalc(applyRecursively);
    if(m_hitB) m_hitB->ReCalc(applyRecursively);
    if(m_hitC) m_hitC->ReCalc(applyRecursively);
  }
  Calculate();
  return true;
}
