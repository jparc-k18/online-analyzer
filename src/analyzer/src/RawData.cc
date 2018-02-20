// -*- C++ -*-

#include <algorithm>
#include <iostream>

#include <std_ostream.hh>
#include <UnpackerManager.hh>

#include "ConfMan.hh"
#include "DebugCounter.hh"
#include "DeleteUtility.hh"
#include "DetectorID.hh"
#include "DCRawHit.hh"
#include "FuncName.hh"
#include "HodoRawHit.hh"
#include "RawData.hh"
#include "UserParamMan.hh"

#define OscillationCut 0

ClassImp(RawData);

namespace
{
  const hddaq::unpacker::UnpackerManager&
  gUnpacker = hddaq::unpacker::GUnpacker::get_instance();
  const UserParamMan& gUser = UserParamMan::GetInstance();
#if OscillationCut
  const Int_t  MaxMultiHitDC  = 16;
#endif
}

//______________________________________________________________________________
RawData::RawData( void )
  : TObject(),
    m_is_decoded(false),
    m_BH1RawHC(),
    m_BH2RawHC(),
    m_BACRawHC(),
    m_SACRawHC(),
    m_TOFRawHC(),
    m_LCRawHC(),
    m_BFTRawHC(NumOfPlaneBFT),
    m_SFTRawHC(NumOfPlaneSFT),
    m_SCHRawHC(),
    // m_BcInRawHC(NumOfLayersBcIn+1),
    m_BcOutRawHC(NumOfLayersBcOut+1),
    m_SdcInRawHC(NumOfLayersSdcIn+1),
    m_SdcOutRawHC(NumOfLayersSdcOut+1),
    // m_SsdInRawHC(NumOfLayersSsdIn+1),
    // m_SsdOutRawHC(NumOfLayersSsdOut+1),
    //     m_ScalerRawHC(),
    m_TrigRawHC()
    //     m_VmeCalibRawHC(),
    //     m_MsTRMRawHC(),
    //     m_MsTRawHC(3), // TOF SCH TAG @Phase2
    // #endif
    //     m_HulTOFRawHC(),
    //     m_HulFBHRawHC(),
    //     m_HulSCHRawHC()
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
RawData::~RawData( void )
{
  ClearAll();
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
Bool_t
RawData::AddHodoRawHit( HodoRHitContainer& cont,
			Int_t id, Int_t plane, Int_t seg, Int_t UorD, Int_t AorT, Int_t data )
{
  HodoRawHit *p = 0;
  for( std::size_t i=0, n=cont.size(); i<n; ++i ){
    HodoRawHit *q = cont[i];
    if( q->DetectorId()==id &&
	q->PlaneId()==plane &&
	q->SegmentId()==seg ){
      p=q; break;
    }
  }
  if( !p ){
    p = new HodoRawHit( id, plane, seg );
    cont.push_back(p);
  }

  switch(AorT){
  case 0:
    if( UorD==0 ) p->SetAdcUp(data);
    else          p->SetAdcDown(data);
    break;
  case 1:
    if( UorD==0 ) p->SetTdcUp(data);
    else          p->SetTdcDown(data);
    break;
  default:
    std::cerr << FUNC_NAME << " wrong AorT " << std::endl
	      << "DetectorId = " << id    << std::endl
	      << "PlaneId    = " << plane << std::endl
	      << "SegId      = " << seg   << std::endl
	      << "AorT       = " << AorT  << std::endl
	      << "UorD       = " << UorD  << std::endl;
    return false;
  }
  return true;
}

//______________________________________________________________________________
Bool_t
RawData::AddDCRawHit( DCRHitContainer& cont,
		      Int_t plane, Int_t wire, Int_t tdc, Int_t type )
{
  DCRawHit *p = nullptr;
  for( std::size_t i=0, n=cont.size(); i<n; ++i ){
    DCRawHit *q = cont[i];
    if( q->PlaneId()==plane && q->WireId()==wire ){
      p = q; break;
    }
  }
  if( !p ){
    p = new DCRawHit( plane, wire );
    cont.push_back(p);
  }

  switch( type ){
  case kLeading:
    p->SetTdc(tdc);
    break;
  case kTrailing:
    p->SetTrailing(tdc);
    break;
  default:
    std::cerr << FUNC_NAME << " wrong data type " << std::endl
	      << "PlaneId    = " << plane << std::endl
	      << "WireId     = " << wire  << std::endl
	      << "DataType   = " << type  << std::endl;
    return false;
  }
  return true;
}

//______________________________________________________________________________
void
RawData::DecodeHodo( Int_t id, Int_t plane, Int_t nseg, Int_t nch, HodoRHitContainer& cont )
{
  if( nseg==0 ) nseg = 1;
  for( Int_t seg=0; seg<nseg; ++seg ){
    for( Int_t UorD=0; UorD<nch; ++UorD ){
      for( Int_t AorT=0; AorT<2; ++AorT ){
	Int_t nhit = gUnpacker.get_entries( id, plane, seg, UorD, AorT );
	if( nhit<=0 ) continue;
	Int_t data = gUnpacker.get( id, plane, seg, UorD, AorT );
	AddHodoRawHit( cont, id, plane, seg, UorD, AorT, data );
      }
    }
  }
}

//______________________________________________________________________________
void
RawData::DecodeHodo( Int_t id, Int_t nseg, Int_t nch, HodoRHitContainer& cont )
{
  DecodeHodo( id, 0, nseg, nch, cont );
}

//______________________________________________________________________________
void
RawData::ClearAll( void )
{
  utility::ClearContainer( m_BH1RawHC );
  utility::ClearContainer( m_BH2RawHC );
  utility::ClearContainer( m_BACRawHC );
  utility::ClearContainer( m_SACRawHC );
  utility::ClearContainer( m_TOFRawHC );
  utility::ClearContainer( m_LCRawHC );
  utility::ClearContainerAll( m_BFTRawHC );
  utility::ClearContainerAll( m_SFTRawHC );
  utility::ClearContainer( m_SCHRawHC );

  // utility::ClearContainerAll( m_BcInRawHC );
  utility::ClearContainerAll( m_BcOutRawHC );
  utility::ClearContainerAll( m_SdcInRawHC );
  utility::ClearContainerAll( m_SdcOutRawHC );
  // utility::ClearContainerAll( m_SsdInRawHC );
  // utility::ClearContainerAll( m_SsdOutRawHC );

  // utility::ClearContainer( m_ScalerRawHC );
  utility::ClearContainer( m_TrigRawHC );
  // utility::ClearContainer( m_VmeCalibRawHC );

  // utility::ClearContainer( m_MsTRMRawHC );
  // utility::ClearContainerAll( m_MsTRawHC );

  // utility::ClearContainer( m_HulTOFRawHC );
  // utility::ClearContainer( m_HulFBHRawHC );
  // utility::ClearContainer( m_HulSCHRawHC );

  m_is_decoded = false;
}

//______________________________________________________________________________
bool
RawData::DecodeHits( void )
{
  static const Double_t MinTdcBC3  = gUser.GetParameter("BC3_TDC", 0);
  static const Double_t MaxTdcBC3  = gUser.GetParameter("BC3_TDC", 1);
  static const Double_t MinTdcBC4  = gUser.GetParameter("BC4_TDC", 0);
  static const Double_t MaxTdcBC4  = gUser.GetParameter("BC4_TDC", 1);
  static const Double_t MinTdcSDC1 = gUser.GetParameter("SDC1_TDC", 0);
  static const Double_t MaxTdcSDC1 = gUser.GetParameter("SDC1_TDC", 1);
  static const Double_t MinTdcSDC2 = gUser.GetParameter("SDC2_TDC", 0);
  static const Double_t MaxTdcSDC2 = gUser.GetParameter("SDC2_TDC", 1);
  static const Double_t MinTdcSDC3 = gUser.GetParameter("SDC3_TDC", 0);
  static const Double_t MaxTdcSDC3 = gUser.GetParameter("SDC3_TDC", 1);

  if( m_is_decoded ){
    hddaq::cerr << "#D " << FUNC_NAME << " "
		<< "already decoded!" << std::endl;
    return false;
  }

  ClearAll();

  // BH1
  DecodeHodo( DetIdBH1, NumOfSegBH1, kBothSide, m_BH1RawHC );
  // BH2
  DecodeHodo( DetIdBH2, NumOfSegBH2, kBothSide, m_BH2RawHC );
  // BAC
  // DecodeHodo( DetIdBAC, NumOfSegBAC, kOneSide, m_BACRawHC );
  // SAC
  DecodeHodo( DetIdSAC, NumOfRoomsSAC, kOneSide, m_SACRawHC );
  // TOF
  DecodeHodo( DetIdTOF, NumOfSegTOF, kBothSide, m_TOFRawHC );

  //BFT
  for( Int_t plane=0; plane<NumOfPlaneBFT; ++plane ){
    for(Int_t seg = 0; seg<NumOfSegBFT; ++seg){
      Int_t nhit = gUnpacker.get_entries( DetIdBFT, plane, 0, seg, 0 );
      if( nhit>0 ){
  	for(Int_t i = 0; i<nhit; ++i){
  	  Int_t leading  = gUnpacker.get( DetIdBFT, plane, 0, seg, 0, i )  ;
  	  Int_t trailing = gUnpacker.get( DetIdBFT, plane, 0, seg, 1, i )  ;
  	  AddHodoRawHit( m_BFTRawHC[plane], DetIdBFT, plane, seg , 0, 1, leading );
  	  AddHodoRawHit( m_BFTRawHC[plane], DetIdBFT, plane, seg , 1, 1, trailing );
  	}
      }
    }
  }

  //SFT
  for( Int_t plane=0; plane<NumOfPlaneSFT; ++plane ){
    for( Int_t seg=0; seg<NumOfSegSFT[plane]; ++seg ){
      for( Int_t lort=0; lort<2; ++lort ){
	Int_t nhit = gUnpacker.get_entries( DetIdSFT, plane, 0, seg, lort );
	if( nhit>0 ){
	  for( Int_t i=0; i<nhit; ++i ){
	    Int_t data  = gUnpacker.get( DetIdSFT, plane, 0, seg, lort, i )  ;
	    AddHodoRawHit( m_SFTRawHC[plane], DetIdSFT, plane, seg , lort, 1, data );
	  }
	}
      }
    }
  }

  //SCH
  for( Int_t seg=0; seg<NumOfSegSCH; ++seg ){
    Int_t nhit = gUnpacker.get_entries( DetIdSCH, 0, seg, 0, 0 );
    if( nhit>0 ){
      for( Int_t i = 0; i<nhit; ++i ){
  	Int_t leading  = gUnpacker.get( DetIdSCH, 0, seg, 0, 0, i );
  	Int_t trailing = gUnpacker.get( DetIdSCH, 0, seg, 0, 1, i );
  	AddHodoRawHit( m_SCHRawHC, DetIdSCH, 0, seg , 0, 1, leading );
  	AddHodoRawHit( m_SCHRawHC, DetIdSCH, 0, seg , 1, 1, trailing );
      }
    }
  }

  // BC3&BC4 MWDC
  for(Int_t plane=0; plane<NumOfLayersBcOut; ++plane ){
    if( plane<NumOfLayersBC3 ){
      for(Int_t wire=0; wire<NumOfWireBC3; ++wire){
	Int_t nhit = gUnpacker.get_entries( DetIdBC3, plane, 0, wire, 0 );
#if OscillationCut
	if( nhit>MaxMultiHitDC ) continue;
#endif
	for(Int_t i=0; i<nhit; i++ ){
	  Int_t data = gUnpacker.get( DetIdBC3, plane, 0, wire, 0, i);
	  if( data<MinTdcBC3 || MaxTdcBC3<data ) continue;
	  AddDCRawHit( m_BcOutRawHC[plane+1], plane+PlMinBcOut, wire+1, data );
	}
      }
    }
    else{
      for(Int_t wire=0; wire<NumOfWireBC4; ++wire){
	Int_t nhit = gUnpacker.get_entries( DetIdBC4, plane-NumOfLayersBC3, 0, wire, 0 );
#if OscillationCut
	if( nhit>MaxMultiHitDC ) continue;
#endif
	for(Int_t i=0; i<nhit; i++ ){
	  Int_t data =  gUnpacker.get( DetIdBC4, plane-NumOfLayersBC3, 0, wire, 0, i );
	  if( data<MinTdcBC4 || MaxTdcBC4<data ) continue;
	  AddDCRawHit( m_BcOutRawHC[plane+1], plane+PlMinBcOut, wire+1, data );
	}
      }
    }
  }

  // SdcIn (SDC1)
  for( Int_t plane=0; plane<NumOfLayersSDC1; ++plane ){
    for( Int_t wire=0; wire<NumOfWireSDC1; ++wire ){
      Int_t nhit = gUnpacker.get_entries( DetIdSDC1, plane, 0, wire, 0 );
#if OscillationCut
      if( nhit>MaxMultiHitDC ) continue;
#endif
      for(Int_t i=0; i<nhit; i++ ){
        Int_t data = gUnpacker.get( DetIdSDC1, plane, 0, wire, 0, i ) ;
        if( data<MinTdcSDC1 || MaxTdcSDC1<data ) continue;
        AddDCRawHit( m_SdcInRawHC[plane+1], plane+PlMinSdcIn, wire+1, data );
      }
    }
  }

  // SdcOut (SDC2&SDC3)
  for( Int_t plane=0; plane<NumOfLayersSdcOut; ++plane ){
    if( plane<NumOfLayersSDC2 ){
      for( Int_t wire=0; wire<NumOfWireSDC2; ++wire ){
  	Int_t nhit = gUnpacker.get_entries( DetIdSDC2, plane, 0, wire, 0 );
#if OscillationCut
	if( nhit>MaxMultiHitDC ) continue;
#endif
	for(Int_t i=0; i<nhit; i++ ){
	  Int_t data = gUnpacker.get( DetIdSDC2, plane, 0, wire, 0, i );
	  if( data<MinTdcSDC2 || MaxTdcSDC2<data ) continue;
	  AddDCRawHit( m_SdcOutRawHC[plane+1], plane+PlMinSdcOut, wire+1, data );
	}
      }
    }
    else{
      Int_t NumOfWireSDC3;
      if( plane==NumOfLayersSDC2 || plane==(NumOfLayersSDC2+1) )
	NumOfWireSDC3 = NumOfWireSDC3Y;
      else
	NumOfWireSDC3 = NumOfWireSDC3X;
      for( Int_t wire=0; wire<NumOfWireSDC3; ++wire ){
  	Int_t nhit = gUnpacker.get_entries( DetIdSDC3, plane-NumOfLayersSDC2, 0, wire, 0 );
#if OscillationCut
	if( nhit>MaxMultiHitDC ) continue;
#endif
	for(Int_t i=0; i<nhit; i++ ){
	  Int_t data = gUnpacker.get( DetIdSDC3, plane-NumOfLayersSDC2, 0, wire, 0 ,i );
	  if( data<MinTdcSDC3 || MaxTdcSDC3<data ) continue;
	  AddDCRawHit( m_SdcOutRawHC[plane+1],  plane+PlMinSdcOut, wire+1, data );
  	}
      }
    }
  }

//   // Scaler
//   // for( Int_t seg=0; seg<NumOfSegScaler; ++seg ){
//   //   Int_t nhit = gUnpacker.get_entries( DetIdScaler, 0, 0, seg, 0 );
//   //   if( nhit>0 ){
//   //     Int_t data = gUnpacker.get( DetIdScaler, 0, 0, seg, 0 );
//   //     AddHodoRawHit( m_ScalerRawHC, DetIdScaler, 0, seg, 0, 0, data );
//   //   }
//   // }

  // trigger Flag
  DecodeHodo( DetIdTFlag, NumOfSegTFlag, kOneSide, m_TrigRawHC );

//   // MsT Tag
//   DecodeHodo( DetIdMsT, 2, 0, kOneSide, m_MsTRawHC[2] );
//   DecodeHodo( DetIdMsT, 0, NumOfSegTOF, kOneSide, m_MsTRawHC[0] );
//   DecodeHodo( DetIdMsT, 1, NumOfSegSCH, kOneSide, m_MsTRawHC[1] );

//   // HUL
//   {
//     static const Int_t device_id = gUnpacker.get_device_id("Mtx3D");
//     static const Int_t t_tof = gUnpacker.get_data_id("Mtx3D", "ttof");
//     static const Int_t t_fbh = gUnpacker.get_data_id("Mtx3D", "tfbh");
//     static const Int_t t_sch = gUnpacker.get_data_id("Mtx3D", "tch");
//     for( Int_t seg=0; seg<NumOfSegTOF; ++seg ){
//       Int_t nhit = gUnpacker.get_entries( device_id, 0, seg, 0, t_tof );
//       if( nhit>0 ){
// 	Int_t data = gUnpacker.get( device_id, 0, seg, 0 , t_tof);
// 	AddHodoRawHit( m_HulTOFRawHC, DetIdHulTOF, 0, seg, 0, 1, data );
//       }//if
//     }// for(seg)
//     for( Int_t seg=0; seg<NumOfSegSCH; ++seg ){
//       Int_t nhit = gUnpacker.get_entries( device_id, 0, seg, 0, t_sch );
//       if( nhit>0 ){
// 	Int_t data = gUnpacker.get( device_id, 0, seg, 0 , t_sch);
// 	AddHodoRawHit( m_HulSCHRawHC, DetIdHulSCH, 0, seg, 0, 1, data );
//       }//if
//     }// for(seg)
//     for( Int_t seg=0; seg<NumOfSegCFBH; ++seg ){
//       Int_t nhit = gUnpacker.get_entries( device_id, 0, seg, 0, t_fbh );
//       if( nhit>0 ){
// 	Int_t data = gUnpacker.get( device_id, 0, seg, 0 , t_fbh);
// 	AddHodoRawHit( m_HulFBHRawHC, DetIdHulFBH, 0, seg, 0, 1, data );
//       }//if
//     }// for(seg)
//   }

  m_is_decoded = true;
  return true;
}

//______________________________________________________________________________
// Bool_t
// RawData::DecodeCalibHits( void )
// {
//   utility::ClearContainer( m_VmeCalibRawHC );

//   for( Int_t plane=0; plane<NumOfPlaneVmeCalib; ++plane ){
//     DecodeHodo( DetIdVmeCalib, plane, NumOfSegVmeCalib,
//   		kOneSide, m_VmeCalibRawHC );
//   }

//   return true;
// }
