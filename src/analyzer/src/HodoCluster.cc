// -*- C++ -*-

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

#include "DebugCounter.hh"
#include "Hodo2Hit.hh"
#include "HodoAnalyzer.hh"
#include "HodoCluster.hh"

ClassImp(HodoCluster);

//______________________________________________________________________________
HodoCluster::HodoCluster( Hodo2Hit *hitA, Hodo2Hit *hitB, Hodo2Hit *hitC )
  : TObject(),
    m_hitA(hitA),
    m_hitB(hitB),
    m_hitC(hitC),
    m_cluster_size(0),
    m_good_for_analysis(true)
{
  if( hitA ) ++m_cluster_size;
  if( hitB ) ++m_cluster_size;
  if( hitC ) ++m_cluster_size;
  Calculate();
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
HodoCluster::~HodoCluster( void )
{
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
Hodo2Hit*
HodoCluster::GetHit( Int_t i ) const
{
  if( i==0 ) return m_hitA;
  if( i==1 ) return m_hitB;
  if( i==2 ) return m_hitC;
  return 0;
}

//______________________________________________________________________________
void
HodoCluster::Calculate( void )
{
  Double_t ms = 0.;
  Double_t mt = 0.;
  Double_t de = 0.;
  Double_t dt = 0.;
  Double_t s1 = 0.;
  Double_t t1 = 0.;

  if( m_hitA ){
    ms += m_hitA->SegmentId();
    mt += m_hitA->CMeanTime();
    de += m_hitA->DeltaE();
    dt += (m_hitA->GetTDown()-m_hitA->GetTUp());
    s1  = m_hitA->SegmentId();
    t1  = m_hitA->CMeanTime();
  }
  if( m_hitB ){
    ms += m_hitB->SegmentId();
    mt += m_hitB->CMeanTime();
    de += m_hitB->DeltaE();
    dt += (m_hitB->GetTDown()-m_hitB->GetTUp());
    if( m_hitB->CMeanTime()<t1 ){
      s1 = m_hitB->SegmentId();
      t1 = m_hitB->CMeanTime();
    }
  }
  if( m_hitC ){
    ms += m_hitC->SegmentId();
    mt += m_hitC->CMeanTime();
    de += m_hitC->DeltaE();
    dt += (m_hitC->GetTDown()-m_hitC->GetTUp());
    if( m_hitC->CMeanTime()<t1 ){
      s1 = m_hitC->SegmentId();
      t1 = m_hitC->CMeanTime();
    }
  }
  ms /= Double_t(m_cluster_size);
  mt /= Double_t(m_cluster_size);
  dt /= Double_t(m_cluster_size);

  m_mean_time = mt;
  m_de        = de;
  m_mean_seg  = ms;
  m_time_diff = dt;
  m_1st_seg   = s1;
  m_1st_time  = t1;
}

//______________________________________________________________________________
Bool_t
HodoCluster::ReCalc( Bool_t applyRecursively )
{
  if( applyRecursively ){
    if( m_hitA ) m_hitA->ReCalc( applyRecursively );
    if( m_hitB ) m_hitB->ReCalc( applyRecursively );
    if( m_hitC ) m_hitC->ReCalc( applyRecursively );
  }
  Calculate();

  return true;
}
