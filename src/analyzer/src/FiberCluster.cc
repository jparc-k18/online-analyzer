// -*- C++ -*-

#include <cmath>
#include <string>

#include <TMath.h>

#include <std_ostream.hh>

#include "DebugCounter.hh"
#include "FLHit.hh"
#include "FiberCluster.hh"
#include "FiberHit.hh"
#include "FuncName.hh"

ClassImp(FiberCluster);

//______________________________________________________________________________
FiberCluster::FiberCluster( void )
  : TObject(),
    m_hit_container(),
    m_cluster_size(0),
    m_mean_time(),
    m_real_mean_time(),
    m_max_width(),
    m_mean_seg(),
    m_mean_pos(),
    m_flag( kNFlagsFiber, false )
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
FiberCluster::~FiberCluster( void )
{
  m_hit_container.clear();
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
FLHit*
FiberCluster::GetHit( Int_t i ) const
{
  return m_hit_container[i];
}

//______________________________________________________________________________
Bool_t
FiberCluster::GoodForAnalysis( Bool_t status )
{
  Bool_t ret = m_flag[kGoodForAnalysis];
  m_flag[kGoodForAnalysis] = status;
  return ret;
}

//______________________________________________________________________________
Bool_t
FiberCluster::Calculate( void )
{
  if( m_flag[kInitialized] ){
    hddaq::cerr << "#E " << FUNC_NAME << " "
		<< "Already initialied" << std::endl;
    return false;
  }

  m_cluster_size = m_hit_container.size();
  if( m_cluster_size == 0 ){
    hddaq::cerr << "#E " << FUNC_NAME << " "
		<< "No FiberHit in local container" << std::endl;
    return false;
  }

  Int_t width = (Int_t)m_hit_container[0]->GetWidth();
  if( width == -1 )
    CalculateWithoutWidth();
  else
    CalculateWithWidth();

  m_flag[kInitialized] = true;

#ifdef DEBUG
  Debug();
#endif

  return true;
}

//______________________________________________________________________________
void
FiberCluster::CalculateWithoutWidth( void )
{
  Double_t mean_seg  = 0.;
  Double_t mean_pos  = 0.;
  Double_t mean_time = 0.;

  mean_seg  = m_hit_container[0]->SegmentId();
  mean_pos  = m_hit_container[0]->GetPosition();
  mean_time = m_hit_container[0]->GetCTime();

  for( Int_t i = 1; i<m_cluster_size; ++i ){
    mean_seg += m_hit_container[0]->SegmentId();
    mean_pos += m_hit_container[i]->GetPosition();
    Double_t time = m_hit_container[i]->GetCTime();
    if( TMath::Abs(time) < TMath::Abs(mean_time) ){
      mean_time = time;
    }
  }

  mean_seg /= Double_t(m_cluster_size);
  mean_pos /= Double_t(m_cluster_size);

  m_mean_seg  = mean_seg;
  m_mean_pos  = mean_pos;
  m_mean_time = mean_time;
  m_max_width = -1;

}

//______________________________________________________________________________
void
FiberCluster::CalculateWithWidth( void )
{
  Double_t mean_seg       = 0.;
  Double_t mean_pos       = 0.;
  Double_t mean_time      = 0.;
  Double_t max_width      = 0.;
  Double_t real_mean_time = 0.;
  Int_t    cluster_id     = 0;

  mean_seg       = m_hit_container[0]->SegmentId();
  mean_pos       = m_hit_container[0]->GetPosition();
  mean_time      = m_hit_container[0]->GetCTime();
  max_width      = m_hit_container[0]->GetWidth();
  real_mean_time = m_hit_container[0]->GetCTime();
  cluster_id     = m_hit_container[0]->PairId();

  for( Int_t i=1; i<m_cluster_size; ++i ){
    mean_seg       += m_hit_container[i]->SegmentId();
    mean_pos       += m_hit_container[i]->GetPosition();
    real_mean_time += m_hit_container[i]->GetCTime();
    Double_t time = m_hit_container[i]->GetCTime();
    if( TMath::Abs(time) < TMath::Abs(mean_time) ){
      mean_time = time;
    }
    if( max_width < m_hit_container[i]->GetWidth() ){
      max_width = m_hit_container[i]->GetWidth();
    }
    cluster_id  += m_hit_container[i]->PairId();
  }

  mean_seg       /= Double_t(m_cluster_size);
  mean_pos       /= Double_t(m_cluster_size);
  real_mean_time /= Double_t(m_cluster_size);

  m_mean_time      = mean_time;
  m_max_width      = max_width;
  m_mean_seg       = mean_seg;
  m_mean_pos       = mean_pos;
  m_real_mean_time = real_mean_time;
  m_cluster_id     = (m_cluster_size == 1) ? 2*cluster_id : cluster_id;
}

//______________________________________________________________________________
void
FiberCluster::Debug( void )
{
  hddaq::cout << "Used hit" << std::endl;
  for( Int_t i = 0; i<m_cluster_size; ++i ){
    m_hit_container[i]->Dump();
  }
}
