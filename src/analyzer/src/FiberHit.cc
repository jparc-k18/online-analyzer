// -*- C++ -*-

#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <std_ostream.hh>

#include "ConfMan.hh"
#include "DCGeomMan.hh"
#include "DebugCounter.hh"
#include "DeleteUtility.hh"
#include "FLHit.hh"
#include "FiberHit.hh"
#include "FuncName.hh"
#include "HodoParamMan.hh"
#include "HodoPHCMan.hh"
#include "RawData.hh"

ClassImp(FiberHit);

namespace
{
  const DCGeomMan&    gGeom = DCGeomMan::GetInstance();
  const HodoParamMan& gHodo = HodoParamMan::GetInstance();
  const HodoPHCMan&   gPHC  = HodoPHCMan::GetInstance();
}

//______________________________________________________________________________
FiberHit::FiberHit( HodoRawHit *object, const TString& name )
  : Hodo1Hit(object),
    m_detector_name(name),
    m_segment(-1),
    m_position(-999.),
    m_offset(0),
    m_pair_id(0),
    m_status(false)
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
FiberHit::~FiberHit( void )
{
  utility::ClearContainer( m_hit_container );
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
Bool_t
FiberHit::Calculate( void )
{
  if( m_status ){
    hddaq::cerr << FUNC_NAME << " already calculated" << std::endl;
    return false;
  }

  if( !gHodo.IsReady() ){
    hddaq::cerr << FUNC_NAME << " HodoParamMan must be initialized" << std::endl;
    return false;
  }

  if( !gPHC.IsReady() ){
    hddaq::cerr << FUNC_NAME << " HodoPHCMan must be initialized" << std::endl;
    return false;
  }

  // Detector information
  Int_t cid  = m_raw->DetectorId();
  Int_t plid = m_raw->PlaneId();
  Int_t seg  = m_raw->SegmentId();
  m_segment = seg;

  // Geometry calibration
  Int_t UorD = 0; // BFT is allways U
  if( "BFT" == m_detector_name || "SFT_X" == m_detector_name ){
    // case of BFT and SFT X layers
    // They have up and down planes in 1 layer.
    // We treat these 2 planes as 1 dimentional plane.
    if(1 == m_raw->PlaneId()){
      // case of down plane
      m_offset = 0.5;
      m_pair_id  = 1;
    }
    m_pair_id += 2*m_raw->SegmentId();
  }else if("FBH" == m_detector_name){
    // SegId 0-15  : U
    // SegId 16-31 : D
    m_pair_id = (seg < 16) ? seg : seg-16;
    UorD      = (seg < 16) ? 0   : 1; // FBH has D
    seg = m_pair_id;
  }else{
    // case of SFT UV layers
    // They have only 1 plane in 1 layer.
    m_pair_id = m_raw->SegmentId();
  }

  Int_t DetectorId = gGeom.GetDetectorId( m_detector_name );
  m_position     = gGeom.CalcWirePosition( DetectorId, seg );

  // hit information
  Int_t size_tdc1 = m_raw->SizeTdc1();
  Int_t size_tdc2 = m_raw->SizeTdc2();
  if( size_tdc1 != size_tdc2 ){
    // hddaq::cerr << "#W " << FUNC_NAME
    // 		<< " SizeTdc1/2 mismatch ["
    // 		<< size_tdc1 << "/" << size_tdc2 << "]" << std::endl;
  }
  m_multi_hit = TMath::Min( m_raw->SizeTdc1(),
			    m_raw->SizeTdc2() );

  Bool_t has_trailing = ( m_raw->GetTdc2() != -1 );

  for(Int_t m = 0; m<m_multi_hit; ++m){
    Int_t leading  = m_raw->GetTdc1(m); // leading
    Int_t width    = -1;

    if( has_trailing ){
      Int_t trailing = m_raw->GetTdc2(m); // trailing
      width = leading - trailing; // width
    }
    m_a.push_back(width);

    // Time calibration
    if( leading<0 ){
      hddaq::cerr << "#E " << FUNC_NAME
		  << " No valid TDC : " << leading
		  << std::endl;
      return false;
    }
    // time is overwriten
    Double_t time = 0;
    if( !gHodo.GetTime(cid, plid, seg, UorD, leading, time) ){
      hddaq::cerr << "#E " << FUNC_NAME
		  << " something is wrong at GetTime("
		  << cid  << ", " << plid << ", " << seg  << ", "
		  << UorD << ", " << leading << ", " << time << std::endl;
      return false;
    }

    m_t.push_back(time);

    Double_t ctime = time;
    gPHC.DoCorrection( cid, plid, seg, UorD, time, width, ctime );
    m_ct.push_back(ctime);
    m_flag_join.push_back(false);
  }

  m_status = true;
  return true;
}

//______________________________________________________________________________
void
FiberHit::Print( Option_t* option ) const
{
  std::cout << FUNC_NAME << std::endl
	    << std::setw(10) << "DetectorName" << std::setw(8) << m_detector_name << std::endl
	    << std::setw(10) << "Segment" << std::setw(8) << m_segment << std::endl
	    << std::setw(10) << "Position" << std::setw(8) << m_position << std::endl
	    << std::setw(10) << "Offset" << std::setw(8) << m_offset << std::endl
	    << std::setw(10) << "PairID" << std::setw(8) << m_pair_id << std::endl
	    << std::setw(10) << "Status" << std::setw(8) << m_status << std::endl;
}
