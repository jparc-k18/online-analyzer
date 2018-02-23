// -*- C++ -*-

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <std_ostream.hh>

#include "BH2Hit.hh"
#include "FuncName.hh"
#include "HodoParamMan.hh"
#include "HodoPHCMan.hh"
#include "RawData.hh"

ClassImp(BH2Hit);

namespace
{
  const HodoParamMan& gHodo = HodoParamMan::GetInstance();
  const HodoPHCMan&   gPHC  = HodoPHCMan::GetInstance();
}

//______________________________________________________________________________
BH2Hit::BH2Hit( void )
  : Hodo2Hit(),
    m_time_offset(0.)
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
BH2Hit::BH2Hit( HodoRawHit *rhit, Int_t index )
  : Hodo2Hit(rhit, index),
    m_time_offset(0.)
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
BH2Hit::~BH2Hit( void )
{
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
bool
BH2Hit::Calculate( void )
{
  if( m_is_calculated ){
    hddaq::cout << FUNC_NAME << " already calculated" << std::endl;
    return false;
  }

  if( m_raw->GetNumOfTdcHits() != 2 )
    return false;

  Int_t cid  = m_raw->DetectorId();
  Int_t plid = m_raw->PlaneId();
  Int_t seg  = m_raw->SegmentId();

  Int_t adc1 = m_raw->GetAdc1();
  Int_t adc2 = m_raw->GetAdc2();
  Int_t tdc1 = m_raw->GetTdc1( Hodo2Hit::m_index );
  Int_t tdc2 = m_raw->GetTdc2( Hodo2Hit::m_index );

  if( tdc1<0 || tdc2<0 || adc1<0 || adc2<0 )
    return false;

  if( !gHodo.IsReady() ){
    hddaq::cout << FUNC_NAME << " HodoParamMan must be initialized" << std::endl;
    return false;
  }

  if( !gPHC.IsReady() ){
    hddaq::cout << FUNC_NAME << " HodoPHCMan must be initialized" << std::endl;
    return false;
  }

  Double_t time1 = 0., time2 = 0.;
  if( !gHodo.GetTime( cid, plid, seg, 0, tdc1, time1 ) ||
      !gHodo.GetTime( cid, plid, seg, 1, tdc2, time2 ) ){
    hddaq::cerr << "#E " << FUNC_NAME
		<< " something is wrong at GetTime("
		<< cid  << ", " << plid << ", " << seg << ", "
		<< "0/1, " << tdc1 << "/" << tdc2 << ", " << "time"
		<< ")" << std::endl;
    return false;
  }

  m_t1.push_back( time1 );
  m_t2.push_back( time2 );

  Double_t dE1 = 0., dE2 = 0.;
  if( !gHodo.GetDe( cid, plid, seg, 0, adc1, dE1 ) ||
      !gHodo.GetDe( cid, plid, seg, 1, adc2, dE2 ) ){
    hddaq::cerr << "#E " << FUNC_NAME
		<< " something is wrong at GetDe("
		<< cid  << ", " << plid << ", " << seg << ", "
		<< "0/1, " << adc1 << "/" << adc2 << ", " << "dE"
		<< ")" << std::endl;
    return false;
  }

  m_a1.push_back( dE1 );
  m_a2.push_back( dE2 );

  Double_t ctime1 = 0., ctime2 = 0.;
  if( gPHC.IsReady() ){
    gPHC.DoCorrection( cid, plid, seg, 0, time1, dE1, ctime1 );
    gPHC.DoCorrection( cid, plid, seg, 1, time2, dE2, ctime2 );
  }

  m_ct1.push_back( ctime1 );
  m_ct2.push_back( ctime2 );

  gHodo.GetTime( cid, plid, seg, 2, 0, m_time_offset );

  m_is_calculated = true;
  return true;
}
