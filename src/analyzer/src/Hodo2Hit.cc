// -*- C++ -*-

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <std_ostream.hh>

#include "DebugCounter.hh"
#include "FuncName.hh"
#include "Hodo2Hit.hh"
#include "HodoParamMan.hh"
#include "HodoPHCMan.hh"
#include "RawData.hh"

ClassImp(Hodo2Hit);

namespace
{
  const HodoParamMan& gHodo = HodoParamMan::GetInstance();
  const HodoPHCMan&   gPHC  = HodoPHCMan::GetInstance();
}

//______________________________________________________________________________
Hodo2Hit::Hodo2Hit( void )
  : TObject(),
    m_raw(),
    m_is_calculated(false),
    m_multi_hit(),
    m_a1(),
    m_a2(),
    m_t1(),
    m_t2(),
    m_ct1(),
    m_ct2(),
    m_index(0)
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
Hodo2Hit::Hodo2Hit( HodoRawHit *rhit, Int_t index )
  : TObject(),
    m_raw(rhit),
    m_is_calculated(false),
    m_index(index)
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
Hodo2Hit::~Hodo2Hit( void )
{
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
Bool_t
Hodo2Hit::Calculate( void )
{
  if( m_is_calculated ){
    hddaq::cout << FUNC_NAME << " already calculated" << std::endl;
    return false;
  }

  if( m_raw->GetNumOfTdcHits()!=2 )
    return false;

  Int_t tdc1 = m_raw->GetTdc1(m_index);
  Int_t tdc2 = m_raw->GetTdc2(m_index);
  if( tdc1<0 || tdc2<0 )
    return false;

  if( !gHodo.IsReady() ){
    hddaq::cout << FUNC_NAME << " HodoParamMan must be initialized" << std::endl;
    return false;
  }

  if( !gPHC.IsReady() ){
    hddaq::cout << FUNC_NAME << " HodoPHCMan must be initialized" << std::endl;
    return false;
  }

  Int_t cid  = m_raw->DetectorId();
  Int_t plid = m_raw->PlaneId();
  Int_t seg  = m_raw->SegmentId();
  Int_t adc1 = m_raw->GetAdc1(), adc2=m_raw->GetAdc2();

  Double_t time1 = 0., time2 = 0.;
  if( !gHodo.GetTime( cid, plid, seg, 0, tdc1, time1 ) ||
      !gHodo.GetTime( cid, plid, seg, 1, tdc2, time2 ) ){
    hddaq::cerr << "#E " << FUNC_NAME
		<< " something is wrong at GetTime("
		<< cid  << ", " << plid << ", " << seg  << ", "
		<< "U/D, " << tdc1 << "/" << tdc2 << ", " << "time"
		<< ")" << std::endl;
    return false;
  }
  m_t1.push_back( time1 );
  m_t2.push_back( time2 );

  Double_t dE1 = 0., dE2 = 0.;
  if( adc1>=0 ){
    if( !gHodo.GetDe( cid, plid, seg, 0, adc1, dE1 ) ){
      hddaq::cerr << "#E " << FUNC_NAME
		  << " something is wrong at GetDe("
		  << cid  << ", " << plid << ", " << seg  << ", "
		  << "0, " << adc1 << ", " << dE1 << ")" << std::endl;
      return false;
    }
  }
  if( adc2>=0 ){
    if( !gHodo.GetDe( cid, plid, seg, 1, adc2, dE2 ) ){
      hddaq::cerr << "#E " << FUNC_NAME
		  << " something is wrong at GetDe("
		  << cid  << ", " << plid << ", " << seg  << ", "
		  << "0, " << adc2 << ", " << dE2 << ")" << std::endl;
      return false;
    }
  }

  m_a1.push_back( dE1 );
  m_a2.push_back( dE2 );

  Double_t ctime1 = -999., ctime2 = -999.;
  gPHC.DoCorrection( cid, plid, seg, 0, time1, dE1, ctime1 );
  gPHC.DoCorrection( cid, plid, seg, 1, time2, dE2, ctime2 );

  m_ct1.push_back( ctime1 );
  m_ct2.push_back( ctime2 );

  m_is_calculated = true;
  return true;
}
