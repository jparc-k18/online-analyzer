// -*- C++ -*-

#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <std_ostream.hh>

#include "DebugCounter.hh"
#include "FuncName.hh"
#include "Hodo1Hit.hh"
#include "HodoParamMan.hh"
#include "HodoPHCMan.hh"
#include "RawData.hh"

ClassImp(Hodo1Hit);

namespace
{
  const HodoParamMan& gHodo = HodoParamMan::GetInstance();
  const HodoPHCMan&   gPHC  = HodoPHCMan::GetInstance();
}

//______________________________________________________________________________
Hodo1Hit::Hodo1Hit( HodoRawHit *rhit, Int_t index )
  : TObject(),
    m_raw(rhit),
    m_is_calculated(false),
    m_multi_hit(),
    m_a(),
    m_t(),
    m_ct(),
    m_index(index)
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
Hodo1Hit::~Hodo1Hit( void )
{
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
bool
Hodo1Hit::Calculate( void )
{
  if( m_is_calculated ){
    hddaq::cout << FUNC_NAME << " already calculated" << std::endl;
    return false;
  }

  if( m_raw->GetNumOfTdcHits()!=1 )
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

  // hit information
  m_multi_hit = m_raw->SizeTdc1();
  Int_t UorD=0;
  Int_t adc = m_raw->GetAdc1(0);
  if( 0 > m_raw->GetTdc1(0) ){
    UorD=1;
    adc = m_raw->GetAdc2(0);
  }

  Double_t dE = 0.;
  if( adc>=0 ){
    if( !gHodo.GetDe( cid, plid, seg, UorD, adc, dE ) ){
      hddaq::cerr << "#E " << FUNC_NAME
		  << " something is wrong at GetDe("
		  << cid  << ", " << plid << ", " << seg << ", "
		  << UorD << ", " << adc  << ", " << dE  << ")" << std::endl;
      return false;
    }
  }

  m_a.push_back(dE);

  for( Int_t m=0; m<m_multi_hit; ++m ){
    Int_t    tdc  = -999;
    Double_t time = -999.;
    if(0 == UorD){
      tdc = m_raw->GetTdc1(m);
    }else{
      tdc = m_raw->GetTdc2(m);
    }

    if( tdc<0 ) continue;

    if( !gHodo.GetTime( cid, plid, seg, UorD, tdc, time ) ){
      hddaq::cerr << "#E " << FUNC_NAME
		  << " something is wrong at GetTime("
		  << cid  << ", " << plid << ", " << seg  << ", "
		  << UorD << ", " << tdc  << ", " << time << ")" << std::endl;
      return false;
    }
    m_t.push_back(time);

    Double_t ctime = time;
    gPHC.DoCorrection(cid, plid, seg, UorD, time, dE, ctime );
    m_ct.push_back(ctime);
  }

  m_is_calculated = true;
  return true;
}
