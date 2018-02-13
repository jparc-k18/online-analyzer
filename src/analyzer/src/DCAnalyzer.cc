// // -*- C++ -*-

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>

#include <TString.h>

#include "ConfMan.hh"
#include "DCAnalyzer.hh"
#include "DCDriftParamMan.hh"
#include "DCGeomMan.hh"
#include "DCHit.hh"
// #include "DCLocalTrack.hh"
#include "DCRawHit.hh"
// #include "DCTrackSearch.hh"
#include "DebugCounter.hh"
// #include "DebugTimer.hh"
// #include "EventDisplay.hh"
// #include "FiberCluster.hh"
#include "FuncName.hh"
// #include "Hodo1Hit.hh"
// #include "Hodo2Hit.hh"
// #include "HodoAnalyzer.hh"
// #include "HodoCluster.hh"
// #include "K18Parameters.hh"
// #include "K18TrackD2U.hh"
// #include "KuramaTrack.hh"
#include "MathTools.hh"
// #include "MWPCCluster.hh"
#include "RawData.hh"
// #include "SsdCluster.hh"
#include "UserParamMan.hh"
#include "DeleteUtility.hh"

#define DefStatic
#include "DCParameters.hh"
#undef DefStatic

// Tracking routine selection __________________________________________________
/* BcOutTracking */
#define BcOut_XUV  0 // XUV Tracking (slow but accerate)
#define BcOut_Pair 1 // Pair plane Tracking (fast but bad for large angle track)
/* SdcInTracking */
#define UseSsdCluster     1 // use SSD Cluster
#define SdcIn_XUV         0 // XUV Tracking (not used in KURAMA)
#define SdcIn_Pair        1 // Pair plane Tracking (fast but bad for large angle track)
#define SdcIn_SsdPreTrack 1 // SsdPreTracking for too many combinations
#define SdcIn_Deletion    1 // Deletion method for too many combinations

// SsdSlopeFilter  _____________________________________________________________
#define SlopeFilter_Tight 0 // 0->1->2->3 : Up   4->5->6->7 : Down
#define SlopeFilter_Wide  1 // 0->1->2    : Up      5->6->7 : Down

ClassImp(DCAnalyzer);

namespace
{
  // using namespace K18Parameter;
  const ConfMan&      gConf   = ConfMan::GetInstance();
  // const EventDisplay& gEvDisp = EventDisplay::GetInstance();
  const DCGeomMan&    gGeom   = DCGeomMan::GetInstance();
  const UserParamMan& gUser   = UserParamMan::GetInstance();

  //______________________________________________________________________________
  // const Double_t& pK18 = ConfMan::Get<Double_t>("PK18");
  // const Int_t& IdTOFUX = gGeom.DetectorId("TOF-UX");
  // const Int_t& IdTOFUY = gGeom.DetectorId("TOF-UY");
  // const Int_t& IdTOFDX = gGeom.DetectorId("TOF-DX");
  // const Int_t& IdTOFDY = gGeom.DetectorId("TOF-DY");

  const Double_t MaxChiSqrKuramaTrack = 10000.;
  const Double_t MaxTimeDifMWPC       =   100.;

  const Double_t kMWPCClusteringWireExtension =  1.0; // [mm]
  const Double_t kMWPCClusteringTimeExtension = 10.0; // [nsec]

  //______________________________________________________________________________
  inline Bool_t /* for MWPCCluster */
  isConnectable( Double_t wire1, Double_t leading1, Double_t trailing1,
		 Double_t wire2, Double_t leading2, Double_t trailing2,
		 Double_t wExt,  Double_t tExt )
  {
    Double_t w1Min = wire1 - wExt;
    Double_t w1Max = wire1 + wExt;
    Double_t t1Min = leading1  - tExt;
    Double_t t1Max = trailing1 + tExt;
    Double_t w2Min = wire2 - wExt;
    Double_t w2Max = wire2 + wExt;
    Double_t t2Min = leading2  - tExt;
    Double_t t2Max = trailing2 + tExt;
    Bool_t isWireOk = !(w1Min>w2Max || w1Max<w2Min);
    Bool_t isTimeOk = !(t1Min>t2Max || t1Max<t2Min);
#if 0
    hddaq::cout << FUNC_NAME << std::endl
		<< " w1 = " << wire1
		<< " le1 = " << leading1
		<< " tr1 = " << trailing1 << "\n"
		<< " w2 = " << wire2
		<< " le2 = " << leading2
		<< " tr2 = " << trailing2 << "\n"
		<< " w1(" << w1Min << " -- " << w1Max << "), t1("
		<< t1Min << " -- " << t1Max << ")\n"
		<< " w2(" << w2Min << " -- " << w2Max << "), t2("
		<< t2Min << " -- " << t2Max << ")\n"
		<< " wire : " << isWireOk
		<< ", time : " << isTimeOk
		<< std::endl;
#endif
    return ( isWireOk && isTimeOk );
  }

  //______________________________________________________________________________
  inline void
  printConnectionFlag( const std::vector<std::deque<Bool_t> >& flag )
  {
    for( UInt_t i=0, n=flag.size(); i<n; ++i ){
      hddaq::cout << std::endl;
      for( UInt_t j=0, m=flag[i].size(); j<m; ++j ){
	hddaq::cout << " " << flag[i][j];
      }
    }
    hddaq::cout << std::endl;
  }

  //______________________________________________________________________________
  inline Bool_t /* for SsdCluster */
  IsClusterable( const DCHitContainer& HitCont, DCHit* CandHit )
  {
    if( !CandHit )
      return false;
    if( !CandHit->IsGoodWaveForm() )
      return false;

    for( UInt_t i=0, n=HitCont.size(); i<n; ++i ){
      DCHit* hit      = HitCont[i];
      Int_t  wire     = hit->GetWire();
      Int_t  CandWire = CandHit->GetWire();
      // Double_t time     = hit->GetPeakTime();
      // Double_t CandTime = CandHit->GetPeakTime();
      if( std::abs( wire-CandWire )==1
	  // && std::abs( time-CandTime )<25.
	  ){
	return true;
      }
    }
    return false;
  }
}

//______________________________________________________________________________
DCAnalyzer::DCAnalyzer( void )
  : TObject(),
    m_is_decoded(n_type),
    m_much_combi(n_type),
    m_MWPCClCont(NumOfLayersBcIn+1),
    m_TempBcInHC(NumOfLayersBcIn+1),
    m_BcInHC(NumOfLayersBcIn+1),
    m_BcOutHC(NumOfLayersBcOut+1),
    m_SdcInHC(NumOfLayersSdcIn+1),
    m_SdcOutHC(NumOfLayersSdcOut+1),
    m_SsdInHC(NumOfLayersSsdIn+1),
    m_SsdOutHC(NumOfLayersSsdOut+1),
    m_SsdInClCont(NumOfLayersSsdIn+1),
    m_SsdOutClCont(NumOfLayersSsdOut+1),
    m_SdcInExTC(NumOfLayersSdcIn+1),
    m_SdcOutExTC(NumOfLayersSdcOut+1)
{
  for( Int_t i=0; i<n_type; ++i ){
    m_is_decoded[i] = false;
    m_much_combi[i] = 0;
  }
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
DCAnalyzer::~DCAnalyzer( void )
{
  // ClearKuramaTracks();
  // ClearK18TracksD2U();
  // ClearTracksSsdOut();
  // ClearTracksSsdIn();
  // ClearTracksSsdXY();
  // ClearTracksSdcOut();
  // ClearTracksSdcIn();
  ClearTracksBcOut();
  // ClearTracksBcOutSdcIn();
  // ClearTracksBcOutSsdIn();
  // ClearTracksSsdOutSdcIn();
  // ClearTracksSdcInSdcOut();
  ClearDCHits();
  // ClearVtxHits();
  debug::ObjectCounter::Decrease(ClassName());
}

// //______________________________________________________________________________
// void
// DCAnalyzer::PrintKurama( const TString& arg ) const
// {
//   Int_t nn = m_KuramaTC.size();
//   hddaq::cout << FUNC_NAME << " " << arg << std::endl
// 	      << "   KuramaTC.size : " << nn << std::endl;
//   for( Int_t i=0; i<nn; ++i ){
//     KuramaTrack *tp = m_KuramaTC[i];
//     hddaq::cout << std::setw(3) << i
// 		<< " Niter=" << std::setw(3) << tp->Niteration()
// 		<< " ChiSqr=" << tp->Chisqr()
// 		<< " P=" << tp->PrimaryMomentum().Mag()
// 		<< " PL(TOF)=" << tp->PathLengthToTOF()
// 		<< std::endl;
//   }
// }

// //______________________________________________________________________________
// #if UseBcIn
// Bool_t
// DCAnalyzer::DecodeBcInHits( RawData *rawData )
// {
//   if( m_is_decoded[k_BcIn] ){
//     hddaq::cout << "#D " << FUNC_NAME << " "
// 		<< "already decoded" << std::endl;
//     return true;
//   }

//   ClearBcInHits();

//   for( Int_t layer=1; layer<=NumOfLayersBcIn; ++layer ){
//     const DCRHitContainer &RHitCont = rawData->GetBcInRawHC(layer);
//     Int_t nh = RHitCont.size();
//     for( Int_t i=0; i<nh; ++i ){
//       DCRawHit *rhit  = RHitCont[i];
//       DCHit    *thit  = new DCHit( rhit->PlaneId()+PlOffsBc, rhit->WireId() );
//       Int_t       nhtdc = rhit->GetTdcSize();
//       if(!thit) continue;
//       for( Int_t j=0; j<nhtdc; ++j ){
// 	thit->SetTdcVal( rhit->GetTdc(j) );
// 	thit->SetTdcTrailing( rhit->GetTrailing(j) );
//       }

//       if(thit->CalcMWPCObservables())
// 	m_TempBcInHC[layer].push_back(thit);
//       else
// 	delete thit;
//     }

//     // hddaq::cout<<"*************************************"<<std::endl;
//     Int_t ncl = clusterizeMWPCHit( m_TempBcInHC[layer], m_MWPCClCont[layer]);
//     // hddaq::cout<<"numCl="<< ncl << std::endl;
//      for( Int_t i=0; i<ncl; ++i ){
//       MWPCCluster *p = m_MWPCClCont[layer][i];
//       if(!p) continue;

//       const MWPCCluster::Statistics& mean  = p->GetMean();
//       const MWPCCluster::Statistics& first = p->GetFirst();
//       Double_t mwire    = mean.m_wire;
//       Double_t mwirepos = mean.m_wpos;
//       Double_t mtime    = mean.m_leading;
//       Double_t mtrail   = mean.m_trailing;

//       DCHit *hit = new DCHit( layer+PlOffsBc, mwire );
//       if(!hit) continue;
//       hit->SetClusterSize( p->GetClusterSize() );
//       hit->SetMWPCFlag( true );
//       hit->SetWire( mwire );
//       hit->SetMeanWire( mwire );
//       hit->SetMeanWirePosition( mwirepos );
//       hit->SetDriftTime( mtime );
//       hit->SetTrailingTime( mtrail );
//       hit->SetDriftLength( 0. );
//       hit->SetTdcVal( 0 );
//       hit->SetTdcTrailing( 0 );

//       if( hit->CalcMWPCObservables() )
// 	m_BcInHC[layer].push_back(hit);
//       else
// 	delete hit;
//     }
//     // hddaq::cout << "nh="<< m_BcInHC[layer].size() <<std::endl;
//   }

//   m_is_decoded[k_BcIn] = true;
//   return true;
// }
// #endif

//______________________________________________________________________________
Bool_t
DCAnalyzer::DecodeBcOutHits( RawData *rawData )
{
  if( m_is_decoded[k_BcOut] ){
    hddaq::cout << "#D " << FUNC_NAME << " "
		<< "already decoded" << std::endl;
    return true;
  }

  ClearBcOutHits();

  for( Int_t layer=1; layer<=NumOfLayersBcOut; ++layer ){
    const DCRHitContainer &RHitCont=rawData->GetBcOutRawHC(layer);
    Int_t nh = RHitCont.size();
    for( Int_t i=0; i<nh; ++i ){
      DCRawHit *rhit  = RHitCont[i];
      DCHit    *hit   = new DCHit( rhit->PlaneId()+PlOffsBc, rhit->WireId() );
      Int_t     nhtdc = rhit->GetTdcSize();
      if(!hit) continue;
      for( Int_t j=0; j<nhtdc; ++j ){
	hit->SetTdcVal( rhit->GetTdc(j) );
      }

      if( hit->CalcDCObservables() )
	m_BcOutHC[layer].push_back(hit);
      else
	delete hit;
    }
  }

  m_is_decoded[k_BcOut] = true;
  return true;
}

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::DecodeSdcInHits( RawData *rawData )
// {
//   if( m_is_decoded[k_SdcIn] ){
//     hddaq::cout << "#D " << FUNC_NAME << " "
// 		<< "already decoded" << std::endl;
//     return true;
//   }

//   ClearSdcInHits();

//   for( Int_t layer=1; layer<=NumOfLayersSdcIn; ++layer ){
//     const DCRHitContainer &RHitCont=rawData->GetSdcInRawHC(layer);
//     Int_t nh = RHitCont.size();
//     for( Int_t i=0; i<nh; ++i ){
//       DCRawHit *rhit  = RHitCont[i];
//       DCHit    *hit   = new DCHit( rhit->PlaneId(), rhit->WireId() );
//       Int_t       nhtdc = rhit->GetTdcSize();
//       if(!hit) continue;
//       for( Int_t j=0; j<nhtdc; ++j ){
// 	hit->SetTdcVal( rhit->GetTdc(j) );
//       }
//       if( hit->CalcDCObservables() )
// 	m_SdcInHC[layer].push_back(hit);
//       else
// 	delete hit;
//     }
//   }

//   m_is_decoded[k_SdcIn] = true;
//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::DecodeSdcOutHits( RawData *rawData )
// {
//   if( m_is_decoded[k_SdcOut] ){
//     hddaq::cout << "#D " << FUNC_NAME << " "
// 		<< "already decoded" << std::endl;
//     return true;
//   }

//   ClearSdcOutHits();

//   for( Int_t layer=1; layer<=NumOfLayersSdcOut; ++layer ){
//     const DCRHitContainer &RHitCont = rawData->GetSdcOutRawHC(layer);
//     Int_t nh = RHitCont.size();
//     for( Int_t i=0; i<nh; ++i ){
//       DCRawHit *rhit  = RHitCont[i];
//       DCHit    *hit   = new DCHit( rhit->PlaneId(), rhit->WireId() );
//       Int_t       nhtdc = rhit->GetTdcSize();
//       if(!hit) continue;
//       for( Int_t j=0; j<nhtdc; ++j ){
// 	hit->SetTdcVal( rhit->GetTdc(j) );
//       }
//       if( hit->CalcDCObservables() )
// 	m_SdcOutHC[layer].push_back(hit);
//       else
// 	delete hit;
//     }
//   }

//   m_is_decoded[k_SdcOut] = true;
//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::DecodeSsdInHits( RawData *rawData )
// {
//   if( m_is_decoded[k_SsdIn] ){
//     hddaq::cout << "#D " << FUNC_NAME << " "
// 		<< "already decoded" << std::endl;
//     return true;
//   }

//   ClearSsdInHits();

//   for( Int_t layer=1; layer<NumOfLayersSsdIn+1;++layer){
//     const DCRHitContainer &RHitCont = rawData->GetSsdInRawHC(layer);
//     Int_t nh = RHitCont.size();
//     for( Int_t i=0; i<nh; ++i ){
//       DCRawHit *rhit = RHitCont[i];
//       DCHit    *hit  = new DCHit( rhit->PlaneId()+PlOffsSsd, rhit->WireId() );
//       if(!hit) continue;
//       // zero suppression flag
//       if( rhit->GetTrailingSize()>0 )
//       	hit->SetTdcTrailing( rhit->GetTrailing(0) );

//       Int_t nhadc = rhit->GetTdcSize();
//       Int_t adc[nhadc];
//       Int_t tdc[nhadc];
//       for( Int_t j=0; j<nhadc; ++j ){
// 	adc[j] = rhit->GetTdc(j);
// 	tdc[j] = j+1;
// 	hit->SetAdcVal( adc[j] );
// 	hit->SetTdcVal( tdc[j] );
//       }
//       if( hit->CalcSsdObservables() )
// 	m_SsdInHC[layer].push_back(hit);
//       else
// 	delete hit;
//     }
//     ClusterizeSsd( m_SsdInHC[layer], m_SsdInClCont[layer] );
//   }

//   m_is_decoded[k_SsdIn] = true;
//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::DecodeSsdOutHits( RawData *rawData )
// {
//   if( m_is_decoded[k_SsdOut] ){
//     hddaq::cout << "#D " << FUNC_NAME << " "
// 		<< "already decoded" << std::endl;
//     return true;
//   }

//   ClearSsdOutHits();

//   for( Int_t layer=1; layer<NumOfLayersSsdOut+1;++layer){
//     const DCRHitContainer &RHitCont = rawData->GetSsdOutRawHC(layer);
//     Int_t nh = RHitCont.size();
//     for( Int_t i=0; i<nh; ++i ){
//       DCRawHit *rhit = RHitCont[i];
//       DCHit    *hit  = new DCHit(rhit->PlaneId()+PlOffsSsd,
// 				 rhit->WireId());
//       if(!hit) continue;
//       // zero suppression flag
//       if( rhit->GetTrailingSize()>0 )
// 	hit->SetTdcTrailing( rhit->GetTrailing(0) );

//       Int_t nhadc = rhit->GetTdcSize();
//       Int_t adc[nhadc];
//       Int_t tdc[nhadc];
//       for( Int_t j=0; j<nhadc; ++j ){
// 	adc[j] = rhit->GetTdc(j);
// 	tdc[j] = j+1;
// 	hit->SetAdcVal( adc[j] );
// 	hit->SetTdcVal( tdc[j] );
//       }
//       if( hit->CalcSsdObservables() )
// 	m_SsdOutHC[layer].push_back(hit);
//       else
// 	delete hit;
//     }
//     ClusterizeSsd( m_SsdOutHC[layer], m_SsdOutClCont[layer] );
//   }

//   m_is_decoded[k_SsdOut] = true;
//   return true;
// }

//______________________________________________________________________________
Bool_t
DCAnalyzer::DecodeRawHits( RawData *rawData )
{
  ClearDCHits();
  DecodeBcOutHits( rawData );
  // DecodeSdcInHits( rawData );
  // DecodeSdcOutHits( rawData );
  // DecodeSsdInHits( rawData );
  // DecodeSsdOutHits( rawData );
  return true;
}

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::DecodeTOFHits( const Hodo2HitContainer& HitCont )
// {
//   if( m_is_decoded[k_TOF] ){
//     hddaq::cout << "#D " << FUNC_NAME << " "
// 		<< "already decoded" << std::endl;
//     return true;
//   }

//   ClearTOFHits();

//   static const Double_t RA2 = gGeom.GetRotAngle2("TOF");
//   static const ThreeVector TOFPos[2] = {
//     gGeom.GetGlobalPosition("TOF-UX"),
//     gGeom.GetGlobalPosition("TOF-DX") };

//   const UInt_t nh = HitCont.size();
//   for( UInt_t i=0; i<nh; ++i ){
//     Hodo2Hit *hodo_hit = HitCont[i];
//     if( !hodo_hit ) continue;
//     const Double_t seg  = hodo_hit->SegmentId()+1;
//     const Double_t dt   = hodo_hit->TimeDiff();
//     Int_t layer_x = -1;
//     Int_t layer_y = -1;
//     if( (Int_t)seg%2==0 ){
//       layer_x  = IdTOFUX;
//       layer_y  = IdTOFUY;
//     }
//     if( (Int_t)seg%2==1 ){
//       layer_x  = IdTOFDX;
//       layer_y  = IdTOFDY;
//     }
//     Double_t wpos = gGeom.CalcWirePosition( layer_x, seg );
//     ThreeVector w( wpos, 0., 0. );
//     w.RotateY( RA2*math::Deg2Rad() );
//     const ThreeVector& hit_pos = TOFPos[(Int_t)seg%2] + w
//       + ThreeVector( 0., dt/0.01285, 0. );
//     // X
//     DCHit *dc_hit_x = new DCHit( layer_x, seg );
//     dc_hit_x->SetTdcVal(0);
//     dc_hit_x->SetWirePosition( hit_pos.x() );
//     dc_hit_x->SetZ( hit_pos.z() );
//     dc_hit_x->SetTiltAngle( 0. );
//     dc_hit_x->SetDriftTime( 0. );
//     dc_hit_x->SetDriftLength( 0. );
//     m_TOFHC.push_back( dc_hit_x );
//     // Y
//     DCHit *dc_hit_y = new DCHit( layer_y, seg );
//     dc_hit_y->SetTdcVal(0);
//     dc_hit_y->SetWirePosition( hit_pos.y() ); // [ns] -> [mm]
//     dc_hit_y->SetZ( hit_pos.z() );
//     dc_hit_y->SetTiltAngle( 90. );
//     dc_hit_y->SetDriftTime( 0. );
//     dc_hit_y->SetDriftLength( 0. );
//     m_TOFHC.push_back( dc_hit_y );
//   }

//   m_is_decoded[k_TOF] = true;
//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::DecodeTOFHits( const HodoClusterContainer& ClCont )
// {
//   if( m_is_decoded[k_TOF] ){
//     hddaq::cout << "#D " << FUNC_NAME << " "
// 		<< "already decoded" << std::endl;
//     return true;
//   }

//   ClearTOFHits();

//   static const Double_t RA2 = gGeom.GetRotAngle2("TOF");
//   static const ThreeVector TOFPos[2] = {
//     gGeom.GetGlobalPosition("TOF-UX"),
//     gGeom.GetGlobalPosition("TOF-DX") };

//   const UInt_t nh = ClCont.size();
//   for( UInt_t i=0; i<nh; ++i ){
//     HodoCluster *hodo_cluster = ClCont[i];
//     if( !hodo_cluster ) continue;
//     const Double_t seg  = hodo_cluster->MeanSeg()+1;
//     const Double_t dt   = hodo_cluster->TimeDif();
//     Int_t layer_x = -1;
//     Int_t layer_y = -1;
//     if( (Int_t)seg%2==0 ){
//       layer_x  = IdTOFUX;
//       layer_y  = IdTOFUY;
//     }
//     if( (Int_t)seg%2==1 ){
//       layer_x  = IdTOFDX;
//       layer_y  = IdTOFDY;
//     }
//     Double_t wpos = gGeom.CalcWirePosition( layer_x, seg );
//     ThreeVector w( wpos, 0., 0. );
//     w.RotateY( RA2*math::Deg2Rad() );
//     const ThreeVector& hit_pos = TOFPos[(Int_t)seg%2] + w
//       + ThreeVector( 0., dt/0.01285, 0. );
//     // X
//     DCHit *dc_hit_x = new DCHit( layer_x, seg );
//     dc_hit_x->SetWirePosition( hit_pos.x() );
//     dc_hit_x->SetZ( hit_pos.z() );
//     dc_hit_x->SetTiltAngle( 0. );
//     dc_hit_x->SetDriftTime( 0. );
//     dc_hit_x->SetDriftLength( 0. );
//     m_TOFHC.push_back( dc_hit_x );
//     // Y
//     DCHit *dc_hit_y = new DCHit( layer_y, seg );
//     dc_hit_y->SetWirePosition( hit_pos.y() ); // [ns] -> [mm]
//     dc_hit_y->SetZ( hit_pos.z() );
//     dc_hit_y->SetTiltAngle( 90. );
//     dc_hit_y->SetDriftTime( 0. );
//     dc_hit_y->SetDriftLength( 0. );
//     m_TOFHC.push_back( dc_hit_y );
//   }

//   m_is_decoded[k_TOF] = true;
//   return true;
// }

// //______________________________________________________________________________
// #if UseBcIn
// Bool_t
// DCAnalyzer::TrackSearchBcIn( void )
// {
//   track::MWPCLocalTrackSearch( &(m_BcInHC[1]), m_BcInTC );
//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchBcIn( const std::vector<std::vector<DCHitContainer> >& hc )
// {
//   track::MWPCLocalTrackSearch( hc, m_BcInTC );
//   return true;
// }
// #endif

//______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchBcOut( void )
// {
//   static const Int_t MinLayer = gUser.GetParameter("MinLayerBcOut");

// #if BcOut_Pair //Pair Plane Tracking Routine for BcOut
//   track::LocalTrackSearch( m_BcOutHC, PPInfoBcOut, NPPInfoBcOut,
// 			   m_BcOutTC, MinLayer );
//   return true;
// #endif

// #if BcOut_XUV  //XUV Tracking Routine for BcOut
//   track::LocalTrackSearchVUX( m_BcOutHC, PPInfoBcOut, NPPInfoBcOut,
// 			      m_BcOutTC, MinLayer );
//   return true;
// #endif

//   return false;
// }

// //______________________________________________________________________________
// // Use with BH2Filter
// Bool_t
// DCAnalyzer::TrackSearchBcOut( const std::vector<std::vector<DCHitContainer> >& hc )
// {
//   static const Int_t MinLayer = gUser.GetParameter("MinLayerBcOut");

// #if BcOut_Pair //Pair Plane Tracking Routine for BcOut
//   track::LocalTrackSearch( hc, PPInfoBcOut, NPPInfoBcOut, m_BcOutTC, MinLayer );
//   return true;
// #endif

// #if BcOut_XUV  //XUV Tracking Routine for BcOut
//   track::LocalTrackSearchVUX( hc, PPInfoBcOut, NPPInfoBcOut, m_BcOutTC, MinLayer );
//   return true;
// #endif

//   return false;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchSdcIn( void )
// {
//   static const Int_t MinLayer = gUser.GetParameter("MinLayerSdcIn");

// #if SdcIn_Pair //Pair Plane Tracking Routine for SdcIn

// # if UseSsdCluster
//   Int_t ntrack =
//     track::LocalTrackSearchSsdOutSdcIn( m_SsdInClCont, m_SsdOutClCont,
// 					m_SdcInHC, PPInfoSdcIn, NPPInfoSdcIn,
// 					m_SsdOutSdcInTC, MinLayer );

//   if( ntrack>=0 ) return true;
//   if( ntrack<0 ) m_much_combi[k_SdcIn]++;

// #  if SdcIn_SsdPreTrack
//   ClearTracksSsdOutSdcIn();

//   track::PreTrackSearchSsdXY( m_SsdInClCont, m_SsdOutClCont,
// 			      m_SsdXTC, m_SsdYTC );

//   ntrack =
//     track::LocalTrackSearchSsdOutSdcIn( m_SdcInHC, PPInfoSdcIn, NPPInfoSdcIn,
// 					m_SsdXTC, m_SsdYTC, m_SsdOutSdcInTC,
// 					MinLayer );
//   if( ntrack<0 ) m_much_combi[k_SdcIn]++;
//   else return true;

// #  endif
// #  if SdcIn_Deletion
//   ClearTracksSsdOutSdcIn();

//   track::LocalTrackSearchSsdOutSdcIn( m_SsdInClCont, m_SsdOutClCont,
// 				      m_SdcInHC, PPInfoSdcIn, NPPInfoSdcIn,
// 				      m_SsdOutSdcInTC, MinLayer, true );
// #  endif

// # else // UseSsdCluster
//   track::LocalTrackSearchSsdOutSdcIn( m_SsdInHC, m_SsdOutHC,
// 				      m_SdcInHC, PPInfoSdcIn, NPPInfoSdcIn,
// 				      m_SsdOutSdcInTC, MinLayer );
// # endif
//   // track::LocalTrackSearch( m_SdcInHC, PPInfoSdcIn, NPPInfoSdcIn, m_SdcInTC, MinLayer );
//   return true;
// #endif

// #if SdcIn_XUV  //XUV Tracking Routine for SdcIn
//   track::LocalTrackSearchVUX( m_SdcInHC, PPInfoSdcIn, NPPInfoSdcIn, m_SdcInTC, MinLayer );
//   return true;
// #endif

//   return false;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchSdcOut( void )
// {
//   static const Int_t MinLayer = gUser.GetParameter("MinLayerSdcOut");

//   track::LocalTrackSearchSdcOut( m_SdcOutHC, PPInfoSdcOut, NPPInfoSdcOut,
// 				 m_SdcOutTC, MinLayer );

//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchSdcOut( const Hodo2HitContainer& TOFCont )
// {
//   static const Int_t MinLayer = gUser.GetParameter("MinLayerSdcOut");

//   if( !DecodeTOFHits( TOFCont ) ) return false;

// #if 0
//   for( UInt_t i=0, n=m_TOFHC.size(); i<n; ++i ){
//     m_TOFHC[i]->Print();
//   }
// #endif

//   track::LocalTrackSearchSdcOut( m_TOFHC, m_SdcOutHC, PPInfoSdcOut, NPPInfoSdcOut,
// 				 m_SdcOutTC, MinLayer );

//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchSdcOut( const HodoClusterContainer& TOFCont )
// {
//   static const Int_t MinLayer = gUser.GetParameter("MinLayerSdcOut");

//   if( !DecodeTOFHits( TOFCont ) ) return false;

//   track::LocalTrackSearchSdcOut( m_TOFHC, m_SdcOutHC, PPInfoSdcOut, NPPInfoSdcOut+2,
// 				 m_SdcOutTC, MinLayer );

//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchSsdIn( Bool_t delete_flag )
// {
//   static const Int_t MinLayer = gUser.GetParameter("MinLayerSsdIn");

// #if UseSsdCluster
//   track::LocalTrackSearchSsdIn( m_SsdInClCont, m_SsdInTC, MinLayer, delete_flag );
// #else
//   track::LocalTrackSearchSsdIn( m_SsdInHC, m_SsdInTC, MinLayer,  delete_flag );
// #endif

//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchSsdOut( void )
// {
//   static const Int_t MinLayer = gUser.GetParameter("MinLayerSsdOut");

//   track::LocalTrackSearchSsdOut( m_SsdOutHC, m_SsdOutTC, MinLayer );

//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchSsdInXY( void )
// {
// #if UseSsdCluster
//   track::LocalTrackSearchSsdInXY( m_SsdInClCont, m_SsdInXTC, m_SsdInYTC );
// #else
//   track::LocalTrackSearchSsdInXY( m_SsdInHC, m_SsdInXTC, m_SsdInYTC );
// #endif

//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchBcOutSdcIn( void )
// {
//   static const Int_t MinLayer = gUser.GetParameter("MinLayerBcOutSdcIn");

//   track::LocalTrackSearchBcOutSdcIn( m_BcOutHC, PPInfoBcOut,
// 				     m_SdcInHC, PPInfoSdcIn,
// 				     NPPInfoBcOut,NPPInfoSdcIn,
// 				     m_BcOutSdcInTC, MinLayer );

//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchBcOutSsdIn( void )
// {
//   static const Int_t MinLayer = gUser.GetParameter("MinLayerBcOutSsdIn");

//   track::LocalTrackSearchBcOutSsdIn( m_BcOutHC, PPInfoBcOut, NPPInfoBcOut,
// 				     m_SsdInHC,
// 				     m_BcOutSsdInTC, MinLayer );

//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchSsdOutSdcIn( void )
// {
//   static const Int_t MinLayer = gUser.GetParameter("MinLayerSsdOutSdcIn");

//   track::LocalTrackSearchSsdOutSdcIn( m_SsdInHC, m_SsdOutHC,
// 				      m_SdcInHC, PPInfoSdcIn, NPPInfoSdcIn,
// 				      m_SsdOutSdcInTC, MinLayer );

//   // track::LocalTrackSearchSsdOutSdcIn( m_SsdOutHC,
//   // 			       m_SdcInHC, PPInfoSdcIn, NPPInfoSdcIn,
//   // 			       m_SsdOutSdcInTC, MinLayer );

//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchK18D2U( const std::vector<Double_t>& XinCont )
// {
//   ClearK18TracksD2U();

//   UInt_t nIn  = XinCont.size();
//   UInt_t nOut = m_BcOutTC.size();

//   if( nIn==0 || nOut==0 )
//     return true;

//   for( UInt_t iIn=0; iIn<nIn; ++iIn ){
//     Double_t LocalX = XinCont[iIn];
//     for( UInt_t iOut=0; iOut<nOut; ++iOut ){
//       DCLocalTrack *trOut = m_BcOutTC[iOut];
// #if 0
//       hddaq::cout << "TrackOut :" << std::setw(2) << iOut
// 		  << " X0=" << trOut->GetX0() << " Y0=" << trOut->GetY0()
// 		  << " U0=" << trOut->GetU0() << " V0=" << trOut->GetV0()
// 		  << std::endl;
// #endif
//       if( !trOut->GoodForTracking() ||
// 	  trOut->GetX0()<MinK18OutX || trOut->GetX0()>MaxK18OutX ||
// 	  trOut->GetY0()<MinK18OutY || trOut->GetY0()>MaxK18OutY ||
// 	  trOut->GetU0()<MinK18OutU || trOut->GetU0()>MaxK18OutU ||
// 	  trOut->GetV0()<MinK18OutV || trOut->GetV0()>MaxK18OutV ) continue;

// #if 0
//       hddaq::cout << FUNC_NAME
// 		  << "Out -> " << trOut->GetChiSquare()
// 		  << " (" << std::setw(2) << trOut->GetNHit() << ") "
// 		  << std::endl;
// #endif

//       K18TrackD2U *track = new K18TrackD2U( LocalX, trOut, pK18 );
//       if( !track ) continue;
//       track->CalcMomentumD2U();
//       m_K18D2UTC.push_back(track);
//     }
//   }

// #if 0
//   hddaq::cout<<"********************"<<std::endl;
//   {
//     Int_t nn = m_K18D2UTC.size();
//     hddaq::cout << FUNC_NAME << ": Before sorting. #Track="
// 		<< nn << std::endl;
//     for( Int_t i=0; i<nn; ++i ){
//       K18TrackD2U *tp = m_K18D2UTC[i];
//       hddaq::cout << std::setw(3) << i
// 		  << " ChiSqr=" << tp->chisquare()
// 		  << " Delta=" << tp->Delta()
// 		  << " P=" << tp->P() << "\n";
// //      hddaq::cout<<"********************"<<std::endl;
// //      hddaq::cout << "In :"
// // 	       << " X " << tp->Xin() << "(" << tp->TrackIn()->GetX0() << ")"
// // 	       << " Y " << tp->Yin() << "(" << tp->TrackIn()->GetY0() << ")"
// // 	       << " U " << tp->Uin() << "(" << tp->TrackIn()->GetU0() << ")"
// // 	       << " V " << tp->Vin() << "(" << tp->TrackIn()->GetV0() << ")"
// // 	       << "\n";
// //      hddaq::cout << "Out:"
// // 	       << " X " << tp->Xout() << "(" << tp->TrackOut()->GetX0() << ")"
// // 	       << " Y " << tp->Yout() << "(" << tp->TrackOut()->GetY0() << ")"
// // 	       << " U " << tp->Uout() << "(" << tp->TrackOut()->GetU0() << ")"
// // 	       << " V " << tp->Vout() << "(" << tp->TrackOut()->GetV0() << ")"
// // 	       << std::endl;
//    }
//  }
// #endif

//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchKurama( void )
// {
//   ClearKuramaTracks();

//   UInt_t nIn  = GetNtracksSdcIn();
//   UInt_t nOut = GetNtracksSdcOut();

//   if( nIn==0 || nOut==0 )
//     return false;

//   for( UInt_t iIn=0; iIn<nIn; ++iIn ){
//     DCLocalTrack *trIn = GetTrackSdcIn( iIn );
//     if( !trIn->GoodForTracking() ) continue;
//     for( UInt_t iOut=0; iOut<nOut; ++iOut ){
//       DCLocalTrack * trOut = GetTrackSdcOut( iOut );
//       if( !trOut->GoodForTracking() ) continue;

//       if( gEvDisp.IsReady() ){
// 	Double_t v0In  = trIn->GetV0();
// 	Double_t v0Out = trOut->GetV0();
// 	Double_t y0In  = trIn->GetY0();
// 	Double_t y0Out = trOut->GetY0();
// 	if( TMath::Abs(v0In-v0Out)>0.050 )
// 	  continue;
// 	if( TMath::Abs(y0In-y0Out)>15. )
// 	  continue;
//       }

//       KuramaTrack *trKurama = new KuramaTrack( trIn, trOut );
//       if( !trKurama ) continue;
//       Double_t u0In    = trIn->GetU0();
//       Double_t u0Out   = trOut->GetU0();
//       Double_t bending = u0Out - u0In;
//       Double_t p[3] = { 0.08493, 0.2227, 0.01572 };
//       Double_t initial_momentum = p[0] + p[1]/( bending-p[2] );
//       if( bending>0. && initial_momentum>0. )
// 	trKurama->SetInitialMomentum( initial_momentum );
//       else
// 	trKurama->SetInitialMomentum( 1.8 );

//       if( trKurama->DoFit() && trKurama->Chisqr()<MaxChiSqrKuramaTrack ){
// 	m_KuramaTC.push_back( trKurama );
//       }
//       else{
// 	// trKurama->Print( "in "+FUNC_NAME );
// 	delete trKurama;
//       }
//     }// for( iOut )
//   }// for( iIn )

//   std::sort( m_KuramaTC.begin(), m_KuramaTC.end(), KuramaTrackComp() );

// #if 0
//   PrintKurama( "Before Deleting" );
// #endif

//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::TrackSearchKurama( Double_t initial_momentum )
// {
//   ClearKuramaTracks();

//   Int_t nIn  = GetNtracksSdcIn();
//   Int_t nOut = GetNtracksSdcOut();

//   if( nIn==0 || nOut==0 ) return true;

//   for( Int_t iIn=0; iIn<nIn; ++iIn ){
//     DCLocalTrack *trIn = GetTrackSdcIn( iIn );
//     if( !trIn->GoodForTracking() ) continue;
//     for( Int_t iOut=0; iOut<nOut; ++iOut ){
//       DCLocalTrack * trOut = GetTrackSdcOut( iOut );
//       if( !trOut->GoodForTracking() ) continue;
//       KuramaTrack *trKurama = new KuramaTrack( trIn, trOut );
//       if( !trKurama ) continue;
//       trKurama->SetInitialMomentum( initial_momentum );
//       if( trKurama->DoFit() && trKurama->Chisqr()<MaxChiSqrKuramaTrack ){
// 	m_KuramaTC.push_back( trKurama );
//       }
//       else{
// 	// trKurama->Print( " in "+FUNC_NAME );
// 	delete trKurama;
//       }
//     }// for( iOut )
//   }// for( iIn )

//   std::sort( m_KuramaTC.begin(), m_KuramaTC.end(), KuramaTrackComp() );

// #if 0
//   PrintKurama( "Before Deleting" );
// #endif

//   return true;
// }

//______________________________________________________________________________
void
DCAnalyzer::ClearDCHits( void )
{
  ClearBcOutHits();
  // ClearSdcInHits();
  // ClearSdcOutHits();
  // ClearSsdInHits();
  // ClearSsdOutHits();
  // ClearTOFHits();
}

//______________________________________________________________________________
void
DCAnalyzer::ClearBcOutHits( void )
{
  utility::ClearContainerAll( m_BcOutHC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearSdcInHits( void )
{
  utility::ClearContainerAll( m_SdcInHC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearSdcOutHits( void )
{
  utility::ClearContainerAll( m_SdcOutHC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearSsdInHits( void )
{
  utility::ClearContainerAll( m_SsdInHC );
  utility::ClearContainerAll( m_SsdInClCont );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearSsdOutHits( void )
{
  utility::ClearContainerAll( m_SsdOutHC );
  utility::ClearContainerAll( m_SsdOutClCont );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearVtxHits( void )
{
  utility::ClearContainer( m_VtxPoint );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearTOFHits( void )
{
  utility::ClearContainer( m_TOFHC );
}

//______________________________________________________________________________
#if UseBcIn
void
DCAnalyzer::ClearTracksBcIn( void )
{
  utility::ClearContainer( m_BcInTC );
}
#endif

//______________________________________________________________________________
void
DCAnalyzer::ClearTracksBcOut( void )
{
  utility::ClearContainer( m_BcOutTC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearTracksSdcIn( void )
{
  utility::ClearContainer( m_SdcInTC );
  utility::ClearContainerAll( m_SdcInExTC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearTracksSdcOut( void )
{
  utility::ClearContainer( m_SdcOutTC );
  utility::ClearContainerAll( m_SdcOutExTC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearTracksSsdIn( void )
{
  utility::ClearContainer( m_SsdInTC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearTracksSsdOut( void )
{
  utility::ClearContainer( m_SsdOutTC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearTracksSsdXY( void )
{
  utility::ClearContainer( m_SsdXTC );
  utility::ClearContainer( m_SsdYTC );
  utility::ClearContainer( m_SsdInXTC );
  utility::ClearContainer( m_SsdInYTC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearK18TracksD2U( void )
{
  utility::ClearContainer( m_K18D2UTC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearKuramaTracks( void )
{
  utility::ClearContainer( m_KuramaTC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearTracksBcOutSdcIn( void )
{
  utility::ClearContainer( m_BcOutSdcInTC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearTracksBcOutSsdIn( void )
{
  utility::ClearContainer( m_BcOutSsdInTC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearTracksSsdOutSdcIn( void )
{
  utility::ClearContainer( m_SsdOutSdcInTC );
}

//______________________________________________________________________________
void
DCAnalyzer::ClearTracksSdcInSdcOut( void )
{
  utility::ClearContainer( m_SdcInSdcOutTC );
}

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcMWPCHits( std::vector<DCHitContainer>& cont,
// 			    Bool_t applyRecursively )
// {
//   const UInt_t n = cont.size();
//   for( UInt_t l=0; l<n; ++l ){
//     const UInt_t m = cont[l].size();
//     for( UInt_t i=0; i<m; ++i ){
//       DCHit *hit = (cont[l])[i];
//       if( !hit ) continue;
//       hit->ReCalcMWPC(applyRecursively);
//     }
//   }
//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcDCHits( std::vector<DCHitContainer>& cont,
// 			  Bool_t applyRecursively )
// {
//   const UInt_t n = cont.size();
//   for( UInt_t l=0; l<n; ++l ){
//     const UInt_t m = cont[l].size();
//     for( UInt_t i=0; i<m; ++i ){
//       DCHit *hit = (cont[l])[i];
//       if( !hit ) continue;
//       hit->ReCalcDC(applyRecursively);
//     }
//   }
//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcDCHits( Bool_t applyRecursively )
// {
// #if UseBcIn
//   ReCalcMWPCHits( m_TempBcInHC, applyRecursively );
//   ReCalcMWPCHits( m_BcInHC, applyRecursively );
// #endif

//   ReCalcDCHits( m_BcOutHC, applyRecursively );
//   ReCalcDCHits( m_SdcInHC, applyRecursively );
//   ReCalcDCHits( m_SdcOutHC, applyRecursively );
//   ReCalcDCHits( m_SsdInHC, applyRecursively );
//   ReCalcDCHits( m_SsdOutHC, applyRecursively );

//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcTrack( DCLocalTrackContainer& cont,
// 			 Bool_t applyRecursively )
// {
//   const UInt_t n = cont.size();
//   for( UInt_t i=0; i<n; ++i ){
//     DCLocalTrack *track = cont[i];
//     if( track ) track->ReCalc( applyRecursively );
//   }
//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcTrack( K18TrackD2UContainer& cont,
// 			 Bool_t applyRecursively )
// {
//   const UInt_t n = cont.size();
//   for( UInt_t i=0; i<n; ++i ){
//     K18TrackD2U *track = cont[i];
//     if( track ) track->ReCalc( applyRecursively );
//   }
//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcTrack( KuramaTrackContainer& cont,
// 			 Bool_t applyRecursively )
// {
//   const UInt_t n = cont.size();
//   for( UInt_t i=0; i<n; ++i ){
//     KuramaTrack *track = cont[i];
//     if( track ) track->ReCalc( applyRecursively );
//   }
//   return true;
// }

// //______________________________________________________________________________
// #if UseBcIn
// Bool_t
// DCAnalyzer::ReCalcTrackBcIn( Bool_t applyRecursively )
// {
//   return ReCalcTrack( m_BcInTC );
// }
// #endif

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcTrackBcOut( Bool_t applyRecursively )
// {
//   return ReCalcTrack( m_BcOutTC );
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcTrackSdcIn( Bool_t applyRecursively )
// {
//   return ReCalcTrack( m_SdcInTC );
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcTrackSdcOut( Bool_t applyRecursively )
// {
//   return ReCalcTrack( m_SdcOutTC );
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcTrackSsdIn( Bool_t applyRecursively )
// {
//   return ReCalcTrack( m_SsdInTC );
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcTrackSsdOut( Bool_t applyRecursively )
// {
//   return ReCalcTrack( m_SsdOutTC );
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcK18TrackD2U( Bool_t applyRecursively )
// {
//   return ReCalcTrack( m_K18D2UTC, applyRecursively );
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcKuramaTrack( Bool_t applyRecursively )
// {
//   return ReCalcTrack( m_KuramaTC, applyRecursively );
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ReCalcAll( void )
// {
//   ReCalcDCHits();
//   ReCalcTrackBcOut();
//   ReCalcTrackSdcIn();
//   ReCalcTrackSdcOut();
//   ReCalcTrackSsdIn();
//   ReCalcTrackSsdOut();

//   //ReCalcK18TrackD2U();
//   ReCalcKuramaTrack();

//   return true;
// }

// //______________________________________________________________________________
// // Int_t
// // clusterizeMWPCHit(const DCHitContainer& hits,
// // 		  MWPCClusterContainer& clusters)
// // {
// //   if (!clusters.empty()){
// //       std::for_each(clusters.begin(), clusters.end(), DeleteObject());
// //       clusters.clear();
// //   }

// //   const Int_t nhits = hits.size();
// //   //   hddaq::cout << "#D " << __func__ << " " << nhits << std::endl;
// //   if (nhits==0)
// //     return 0;

// //   Int_t n = 0;
// //   for (Int_t i=0; i<nhits; ++i){
// //     const DCHit* h = hits[i];
// //     if (!h)
// //       continue;
// //     n += h->GetTdcSize();
// //   }

// //   DCHitContainer singleHits;
// //   singleHits.reserve(n);
// //   for (Int_t i=0; i<nhits; ++i){
// //     const DCHit* h = hits[i];
// //     if (!h)
// //       continue;
// //     Int_t nn = h->GetTdcSize();
// //     for (Int_t ii=0; ii<nn; ++ii){
// //       DCHit* htmp = new DCHit(h->GetLayer(), h->GetWire());
// //       htmp->SetTdcVal(h->GetTdcVal());
// //       htmp->SetTdcTrailing(h->GetTdcTrailing());
// //       htmp->SetTrailingTime(h->GetTrailingTime());
// //       htmp->SetDriftTime(h->GetDriftTime());
// //       htmp->SetDriftLength(h->GetDriftLength());
// //       htmp->SetTiltAngle(h->GetTiltAngle());
// //       htmp->SetWirePosition(h->GetWirePosition());
// //       htmp->setRangeCheckStatus(h->rangecheck(), 0);
// //       singleHits.push_back(htmp);
// //     }
// //   }

// //   std::vector<std::deque<Bool_t> > flag(n, std::deque<Bool_t>(n, false));
// //   n = singleHits.size();
// //   for (Int_t i=0;  i<n; ++i){
// //     flag[i][i] = true;
// //     const DCHit* h1 = singleHits[i];
// //     //       h1->print("h1");
// //     for (Int_t j=i+1; j<n; ++j){
// //       const DCHit* h2 = singleHits[j];
// //       // 	  h2->print("h2");
// //       // 	  hddaq::cout << " (i,j) = (" << i << ", " << j << ")" << std::endl;
// //       Bool_t val
// // 	= isConnectable(h1->GetWirePosition(),
// // 			h1->GetDriftTime(),
// // 			h1->GetTrailingTime(),
// // 			h2->GetWirePosition(),
// // 			h2->GetDriftTime(),
// // 			h2->GetTrailingTime(),
// // 			kMWPCClusteringWireExtension,
// // 			kMWPCClusteringTimeExtension);
// //       // 	  hddaq::cout << "#D val = " << val << std::endl;
// //       flag[i][j] = val;
// //       flag[j][i] = val;
// //     }
// //   }

// //   //   hddaq::cout << "#D " << __func__ << "  before " << std::endl;
// //   //   printConnectionFlag(flag);

// //   const Int_t maxLoop = static_cast<Int_t>(std::log(x)/std::log(2.))+1;
// //   for (Int_t loop=0; loop<maxLoop; ++loop){
// //     std::vector<std::deque<Bool_t> > tmp(n, std::deque<Bool_t>(n, false));
// //     for (Int_t i=0; i<n; ++i){
// //       for (Int_t j=i; j<n; ++j){
// // 	for (Int_t k=0; k<n; ++k){
// // 	  tmp[i][j] |= (flag[i][k] && flag[k][j]);
// // 	  tmp[j][i] = tmp[i][j];
// // 	}
// //       }
// //     }
// //     flag = tmp;
// //     //       hddaq::cout << " n iteration = " << loop << std::endl;
// //     //       printConnectionFlag(flag);
// //   }

// //   //   hddaq::cout << "#D " << __func__ << "  after " << std::endl;
// //   //   printConnectionFlag(flag);

// //   std::set<Int_t> checked;
// //   for (Int_t i=0; i<n; ++i){
// //     if (checked.find(i)!=checked.end())
// //       continue;
// //     MWPCCluster* c = 0;
// //     for (Int_t j=i; j<n; ++j){
// //       if (flag[i][j]){
// // 	checked.insert(j);
// // 	if (!c) {
// // 	  c = new MWPCCluster;
// // 	  // 		  hddaq::cout << " new cluster " << std::endl;
// // 	}
// // 	// 	      hddaq::cout << " " << i << "---" << j << std::endl;
// // 	c->Add(singleHits[j]);
// //       }
// //     }

// //     if (c){
// //       c->Calculate();
// //       clusters.push_back(c);
// //     }
// //   }

// //   //   hddaq::cout << " end of " << __func__
// //   // 	    << " : n = " << n << ", " << checked.size()
// //   // 	    << std::endl;

// //   //   hddaq::cout << __func__ << " n clusters = " << clusters.size() << std::endl;

// //   return clusters.size();
// // }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ClusterizeSsd( void )
// {
//   for( Int_t l=1; l<NumOfLayersSsdIn+1; ++l )
//     ClusterizeSsd( m_SsdInHC[l], m_SsdInClCont[l] );
//   for( Int_t l=1; l<NumOfLayersSsdOut+1; ++l )
//     ClusterizeSsd( m_SsdOutHC[l], m_SsdOutClCont[l] );
//   return true;
// }

// //______________________________________________________________________________
// Bool_t
// DCAnalyzer::ClusterizeSsd( const DCHitContainer& HitCont,
// 			   SsdClusterContainer& ClCont,
// 			   Double_t MaxTimeDiff )
// {
//   utility::ClearContainer( ClCont );

//   const UInt_t nh = HitCont.size();
//   if( nh==0 )
//     return false;

//   std::vector<Int_t> flag( nh, 0 );

//   for( UInt_t i=0; i<nh; ++i ){
//     if( flag[i]>0 )
//       continue;
//     DCHitContainer CandCont;
//     DCHit* hitA = HitCont[i];
//     if( !hitA || !hitA->IsGoodWaveForm() )
//       continue;
//     CandCont.push_back( hitA );
//     ++flag[i];

//     for( UInt_t j=0; j<nh; ++j ){
//       if( CandCont.size()==SsdCluster::MaxClusterSize() )
// 	break;
//       if( i==j || flag[j]>0 )
// 	continue;
//       DCHit* hitB = HitCont[j];
//       if( !hitB || !hitB->IsGoodWaveForm() )
// 	continue;
//       if( IsClusterable( CandCont, hitB ) ){
// 	CandCont.push_back( hitB );
// 	++flag[j];
//       }
//     }
//     SsdCluster *cluster = new SsdCluster( CandCont );
//     if( cluster ) ClCont.push_back( cluster );
//   }

//   return true;
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::DoTimeCorrectionSsd( const std::vector<Double_t>& t0 )
// {
//   DoTimeCorrectionSsdIn( t0 );
//   DoTimeCorrectionSsdOut( t0 );
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::DoTimeCorrectionSsdIn( const std::vector<Double_t>& t0 )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdIn+1; ++layer ){
//     const DCHitContainer& HitCont = GetSsdInHC(layer);
//     const UInt_t nh = HitCont.size();
//     for( UInt_t i=0; i<nh; ++i ){
//       DCHit *hit = HitCont[i];
//       if( !hit ) continue;
//       Double_t offset = 0.;
//       switch( layer ){
//       case 1: case 2: offset = t0[1]; break;
//       case 3: case 4: offset = t0[1]; break;
//       }
//       hit->DoTimeCorrection( offset );
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::DoTimeCorrectionSsdOut( const std::vector<Double_t>& t0 )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdOut+1; ++layer ){
//     const DCHitContainer &HitCont = GetSsdOutHC(layer);
//     const UInt_t nh = HitCont.size();
//     for( UInt_t i=0; i<nh; ++i ){
//       DCHit *hit = HitCont[i];
//       if( !hit ) continue;
//       Double_t offset = 0.;
//       switch(layer){
//       case 1: case 2: offset = t0[1]; break;
//       case 3: case 4: offset = t0[1]; break;
//       }
//       hit->DoTimeCorrection( offset );
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::ResetStatusSsd( void )
// {
//   ResetStatusSsdIn();
//   ResetStatusSsdOut();
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::ResetStatusSsdIn( void )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdIn+1;++layer){
//     const DCHitContainer &HitCont = GetSsdInHC(layer);
//     const UInt_t nh = HitCont.size();
//     for( UInt_t i=0; i<nh; ++i ){
//       DCHit* hit = HitCont[i];
//       if( !hit ) continue;
//       hit->SetGoodWaveForm( true );
//     }
//     const SsdClusterContainer &ClCont = GetClusterSsdIn(layer);
//     const UInt_t ncl = ClCont.size();
//     for( UInt_t i=0; i<ncl; ++i ){
//       SsdCluster *cluster = ClCont[i];
//       if( !cluster ) continue;
//       cluster->GoodForAnalysis( true );
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::ResetStatusSsdOut( void )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdOut+1;++layer){
//     const DCHitContainer &HitCont = GetSsdOutHC(layer);
//     const UInt_t nh = HitCont.size();
//     for( UInt_t i=0; i<nh; ++i ){
//       DCHit *hit = HitCont[i];
//       if( !hit ) continue;
//       hit->SetGoodWaveForm( true );
//     }
//     const SsdClusterContainer &ClCont = GetClusterSsdOut(layer);
//     const UInt_t ncl = ClCont.size();
//     for( UInt_t i=0; i<ncl; ++i ){
//       SsdCluster *cluster = ClCont[i];
//       if( !cluster ) continue;
//       cluster->GoodForAnalysis( true );
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::SlopeFilterSsd( void )
// {
//   SlopeFilterSsdIn();
//   SlopeFilterSsdOut();
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::SlopeFilterSsdIn( void )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdIn+1;++layer){
//     const DCHitContainer &HitCont = GetSsdInHC(layer);
//     const UInt_t nh = HitCont.size();
//     for( UInt_t i=0; i<nh; ++i ){
//       DCHit *hit = HitCont[i];
//       if( !hit ) continue;
//       if( !hit->IsGoodWaveForm() ) continue;

//       std::vector<Double_t> waveform = hit->GetWaveform();
//       if( waveform.size()!=NumOfSampleSSD ) continue;

//       Bool_t slope[NumOfSampleSSD];
//       for( Int_t s=0; s<NumOfSampleSSD; ++s ){
// 	if( s>0 ) slope[s-1] = ( waveform[s]>waveform[s-1] );
//       }

//       Bool_t status = true;
// #if SlopeFilter_Tight
//       status = slope[0] && slope[1] && slope[2]
//       	&& !slope[4] && !slope[5] && !slope[6];
// #elif SlopeFilter_Wide
//       status = slope[0] && slope[1] && !slope[5] && !slope[6];
// #endif
//       hit->SetGoodWaveForm(status);
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::SlopeFilterSsdOut( void )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdOut+1;++layer){
//     const DCHitContainer &HitCont = GetSsdOutHC(layer);
//     const UInt_t nh = HitCont.size();
//     for( UInt_t i=0; i<nh; ++i ){
//       DCHit *hit = HitCont[i];
//       if( !hit ) continue;
//       if( !hit->IsGoodWaveForm() ) continue;

//       std::vector<Double_t> waveform = hit->GetWaveform();
//       if( waveform.size()!=NumOfSampleSSD ) continue;

//       Bool_t slope[NumOfSampleSSD];
//       for( Int_t s=0; s<NumOfSampleSSD; ++s ){
// 	if(s>0) slope[s-1] = waveform[s] > waveform[s-1];
//       }

//       Bool_t status = true;
// #if SlopeFilter_Tight
//       status = slope[0] && slope[1] && slope[2]
//       	&& !slope[4] && !slope[5] && !slope[6];
// #elif SlopeFilter_Wide
//       status = slope[0] && slope[1] && !slope[5] && !slope[6];
// #endif
//       hit->SetGoodWaveForm(status);
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::AdcPeakHeightFilterSsd( Double_t min, Double_t max )
// {
//   AdcPeakHeightFilterSsdIn( min, max );
//   AdcPeakHeightFilterSsdOut( min, max );
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::AdcPeakHeightFilterSsdIn( Double_t min, Double_t max )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdIn+1;++layer ){
//     const DCHitContainer &HitCont = GetSsdInHC(layer);
//     const UInt_t nh = HitCont.size();
//     for( UInt_t i=0; i<nh; ++i ){
//       DCHit *hit = HitCont[i];
//       if( !hit ) continue;
//       if( !hit->IsGoodWaveForm() ) continue;
//       Double_t peak_height = hit->GetAdcPeakHeight();
//       Double_t pedestal    = hit->GetPedestal();
//       peak_height -= pedestal;
//       if( peak_height<min || max<peak_height ){
// 	hit->SetGoodWaveForm( false );
//       }
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::AdcPeakHeightFilterSsdOut( Double_t min, Double_t max )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdOut+1;++layer ){
//     const DCHitContainer &HitCont = GetSsdOutHC(layer);
//     const UInt_t nh = HitCont.size();
//     for( UInt_t i=0; i<nh; ++i ){
//       DCHit *hit = HitCont[i];
//       if( !hit ) continue;
//       if( !hit->IsGoodWaveForm() ) continue;
//       Double_t peak_height = hit->GetAdcPeakHeight();
//       Double_t pedestal    = hit->GetPedestal();
//       peak_height -= pedestal;
//       if( peak_height<min || max<peak_height ){
// 	hit->SetGoodWaveForm( false );
//       }
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::AmplitudeFilterSsd( Double_t min, Double_t max, Bool_t cluster_flag /*=false*/ )
// {
//   AmplitudeFilterSsdIn( min, max, cluster_flag );
//   AmplitudeFilterSsdOut( min, max, cluster_flag );
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::AmplitudeFilterSsdIn( Double_t min, Double_t max, Bool_t cluster_flag /*=false*/ )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdIn+1;++layer ){
//     if( !cluster_flag ){
//       const DCHitContainer &HitCont = GetSsdInHC(layer);
//       const UInt_t nh = HitCont.size();
//       for( UInt_t i=0; i<nh; ++i ){
// 	DCHit *hit = HitCont[i];
// 	if( !hit ) continue;
// 	if( !hit->IsGoodWaveForm() ) continue;
// 	Double_t amplitude = hit->GetAmplitude();
// 	if( amplitude<min || max<amplitude ){
// 	  hit->SetGoodWaveForm(false);
// 	}
//       }
//     }
//     if( cluster_flag ){
//       const SsdClusterContainer &ClCont = GetClusterSsdIn(layer);
//       const UInt_t ncl = ClCont.size();
//       for( UInt_t i=0; i<ncl; ++i ){
// 	SsdCluster *cluster = ClCont[i];
// 	if( !cluster ) continue;
// 	if( !cluster->GoodForAnalysis() ) continue;
// 	Double_t amplitude = cluster->Amplitude();
// 	if( amplitude<min || max<amplitude ){
// 	  cluster->GoodForAnalysis(false);
// 	}
//       }
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::AmplitudeFilterSsdOut( Double_t min, Double_t max, Bool_t cluster_flag /*=false*/ )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdOut+1;++layer){
//     if( !cluster_flag ){
//       const DCHitContainer &HitCont = GetSsdOutHC(layer);
//       const UInt_t nh = HitCont.size();
//       for( UInt_t i=0; i<nh; ++i ){
// 	DCHit *hit = HitCont[i];
// 	if( !hit ) continue;
// 	if( !hit->IsGoodWaveForm() ) continue;
// 	Double_t amplitude = hit->GetAmplitude();
// 	if( amplitude<min || max<amplitude ){
// 	  hit->SetGoodWaveForm(false);
// 	}
//       }
//     }
//     if( cluster_flag ){
//       const SsdClusterContainer &ClCont = GetClusterSsdOut(layer);

//       const UInt_t ncl = ClCont.size();
//       for( UInt_t i=0; i<ncl; ++i ){
// 	SsdCluster *cluster = ClCont[i];
// 	if( !cluster ) continue;
// 	if( !cluster->GoodForAnalysis() ) continue;
// 	Double_t amplitude = cluster->Amplitude();
// 	if( amplitude<min || max<amplitude ){
// 	  cluster->GoodForAnalysis(false);
// 	}
//       }
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::DeltaEFilterSsd( Double_t min, Double_t max, Bool_t cluster_flag /*=false*/ )
// {
//   DeltaEFilterSsdIn( min, max, cluster_flag );
//   DeltaEFilterSsdOut( min, max, cluster_flag );
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::DeltaEFilterSsdIn( Double_t min, Double_t max, Bool_t cluster_flag /*=false*/ )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdIn+1;++layer){
//     if( !cluster_flag ){
//       const DCHitContainer &HitCont = GetSsdInHC(layer);
//       const UInt_t nh = HitCont.size();
//       for( UInt_t i=0; i<nh; ++i ){
// 	DCHit *hit = HitCont[i];
// 	if( !hit ) continue;
// 	if( !hit->IsGoodWaveForm() ) continue;
// 	Double_t de = hit->GetDe();
// 	if( de<min || max<de ){
// 	  hit->SetGoodWaveForm(false);
// 	}
//       }
//     }
//     if( cluster_flag ){
//       const SsdClusterContainer &ClCont = GetClusterSsdIn(layer);
//       const UInt_t ncl = ClCont.size();
//       for( UInt_t i=0; i<ncl; ++i ){
// 	SsdCluster *cluster = ClCont[i];
// 	if( !cluster ) continue;
// 	if( !cluster->GoodForAnalysis() ) continue;
// 	Double_t de = cluster->DeltaE();
// 	if( de<min || max<de ){
// 	  cluster->GoodForAnalysis(false);
// 	}
//       }
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::DeltaEFilterSsdOut( Double_t min, Double_t max, Bool_t cluster_flag /*=false*/ )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdOut+1;++layer){
//     if( !cluster_flag ){
//       const DCHitContainer &HitCont = GetSsdOutHC(layer);
//       const UInt_t nh = HitCont.size();
//       for( UInt_t i=0; i<nh; ++i ){
// 	DCHit *hit = HitCont[i];
// 	if( !hit ) continue;
// 	if( !hit->IsGoodWaveForm() ) continue;
// 	Double_t de = hit->GetDe();
// 	if( de<min || max<de ){
// 	  hit->SetGoodWaveForm(false);
// 	}
//       }
//     }
//     if( cluster_flag ){
//       const SsdClusterContainer &ClCont = GetClusterSsdOut(layer);
//       const UInt_t ncl = ClCont.size();
//       for( UInt_t i=0; i<ncl; ++i ){
// 	SsdCluster *cluster = ClCont[i];
// 	if( !cluster ) continue;
// 	if( !cluster->GoodForAnalysis() ) continue;
// 	Double_t de = cluster->DeltaE();
// 	if( de<min || max<de ){
// 	  cluster->GoodForAnalysis(false);
// 	}
//       }
//     }
//   }
// }
// //______________________________________________________________________________
// void
// DCAnalyzer::RmsFilterSsd( Double_t min, Double_t max )
// {
//   RmsFilterSsdIn(min, max);
//   RmsFilterSsdOut(min, max);
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::RmsFilterSsdIn( Double_t min, Double_t max )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdIn+1;++layer){
//     const DCHitContainer &HitCont = GetSsdInHC(layer);
//     const UInt_t nh = HitCont.size();
//     for( UInt_t i=0; i<nh; ++i ){
//       DCHit *hit = HitCont[i];
//       if(!hit) continue;
//       if(!hit->IsGoodWaveForm()) continue;
//       Double_t de  = hit->GetDe();
//       Double_t rms = hit->GetRms();
//       if( de<min*rms || max*rms<de )
// 	hit->SetGoodWaveForm(false);
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::RmsFilterSsdOut( Double_t min, Double_t max )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdOut+1;++layer){
//     const DCHitContainer &HitCont = GetSsdOutHC(layer);
//     const UInt_t nh = HitCont.size();
//     for( UInt_t i=0; i<nh; ++i ){
//       DCHit *hit = HitCont[i];
//       if( !hit ) continue;
//       if( !hit->IsGoodWaveForm() ) continue;
//       Double_t de  = hit->GetDe();
//       Double_t rms = hit->GetRms();
//       if( de<min*rms||max*rms<de ){
// 	hit->SetGoodWaveForm(false);
//       }
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::DeviationFilterSsd( Double_t min, Double_t max )
// {
//   DeviationFilterSsdIn( min, max );
//   DeviationFilterSsdOut( min, max );
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::DeviationFilterSsdIn( Double_t min, Double_t max )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdIn+1;++layer){
//     const DCHitContainer &HitCont = GetSsdInHC(layer);
//     const UInt_t nh = HitCont.size();
//     for( UInt_t i=0; i<nh; ++i ){
//       DCHit *hit = HitCont[i];
//       if( !hit ) continue;
//       if( !hit->IsGoodWaveForm() ) continue;
//       Double_t deviation = hit->GetDeviation();
//       if( deviation<min || max<deviation ){
// 	hit->SetGoodWaveForm(false);
//       }
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::DeviationFilterSsdOut( Double_t min, Double_t max )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdOut+1;++layer){
//     const DCHitContainer &HitCont = GetSsdOutHC(layer);
//     const UInt_t nh = HitCont.size();
//     for( UInt_t i=0; i<nh; ++i ){
//       DCHit *hit = HitCont[i];
//       if( !hit ) continue;
//       if( !hit->IsGoodWaveForm() ) continue;
//       Double_t deviation = hit->GetDeviation();
//       if( deviation<min || max<deviation ){
// 	hit->SetGoodWaveForm(false);
//       }
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::TimeFilterSsd( Double_t min, Double_t max, Bool_t cluster_flag /*=false*/ )
// {
//   TimeFilterSsdIn( min, max, cluster_flag );
//   TimeFilterSsdOut( min, max, cluster_flag );
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::TimeFilterSsdIn( Double_t min, Double_t max, Bool_t cluster_flag /*=false*/ )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdIn+1;++layer){
//     if( !cluster_flag ){
//       const DCHitContainer &HitCont = GetSsdInHC(layer);
//       const UInt_t nh = HitCont.size();
//       for( UInt_t i=0; i<nh; ++i ){
// 	DCHit *hit = HitCont[i];
// 	if( !hit ) continue;
// 	if( !hit->IsGoodWaveForm() ) continue;
// 	Double_t peaktime = hit->GetPeakTime();
// 	if( peaktime<min || max<peaktime ){
// 	  hit->SetGoodWaveForm(false);
// 	}
//       }
//     }
//     if( cluster_flag ){
//       const SsdClusterContainer &ClCont = GetClusterSsdIn(layer);
//       const UInt_t ncl = ClCont.size();
//       for( UInt_t i=0; i<ncl; ++i ){
// 	SsdCluster *cluster = ClCont[i];
// 	if( !cluster ) continue;
// 	if( !cluster->GoodForAnalysis() ) continue;
// 	Double_t time = cluster->Time();
// 	if( time<min || max<time ){
// 	  cluster->GoodForAnalysis(false);
// 	}
//       }
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::TimeFilterSsdOut( Double_t min, Double_t max, Bool_t cluster_flag /*=false*/ )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdOut+1;++layer){
//     if( !cluster_flag ){
//       const DCHitContainer &HitCont = GetSsdOutHC(layer);
//       const UInt_t nh = HitCont.size();
//       for( UInt_t i=0; i<nh; ++i ){
// 	DCHit *hit = HitCont[i];
// 	if( !hit ) continue;
// 	if( !hit->IsGoodWaveForm() ) continue;
// 	Double_t peaktime = hit->GetPeakTime();
// 	if( peaktime<min || max<peaktime ){
// 	  hit->SetGoodWaveForm(false);
// 	}
//       }
//     }
//     if( cluster_flag ){
//       const SsdClusterContainer &ClCont = GetClusterSsdOut(layer);
//       const UInt_t ncl = ClCont.size();
//       for( UInt_t i=0; i<ncl; ++i ){
// 	SsdCluster *cluster = ClCont[i];
// 	if( !cluster ) continue;
// 	if( !cluster->GoodForAnalysis() ) continue;
// 	Double_t time = cluster->Time();
// 	if( time<min || max<time ){
// 	  cluster->GoodForAnalysis(false);
// 	}
//       }
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::ChisqrFilterSsd( Double_t maxchi2 )
// {
//   ChisqrFilterSsdIn( maxchi2 );
//   ChisqrFilterSsdOut( maxchi2 );
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::ChisqrFilterSsdIn( Double_t maxchi2 )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdIn+1;++layer){
//     const DCHitContainer &HitCont = GetSsdInHC(layer);
//     Int_t nh = HitCont.size();
//     for(Int_t i=0;i<nh;++i){
//       DCHit *hit = HitCont[i];
//       if(!hit) continue;
//       if(!hit->IsGoodWaveForm()) continue;
//       Double_t chisqr = hit->GetChisquare();
//       if( chisqr>maxchi2 ){
// 	hit->SetGoodWaveForm(false);
//       }
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::ChisqrFilterSsdOut( Double_t maxchi2 )
// {
//   for( Int_t layer=1; layer<NumOfLayersSsdOut+1;++layer){
//     const DCHitContainer &HitCont = GetSsdOutHC(layer);
//     Int_t nh = HitCont.size();
//     for(Int_t i=0;i<nh;++i){
//       DCHit *hit = HitCont[i];
//       if(!hit) continue;
//       if(!hit->IsGoodWaveForm()) continue;
//       Double_t chisqr = hit->GetChisquare();
//       if( chisqr>maxchi2 ){
// 	hit->SetGoodWaveForm(false);
//       }
//     }
//   }
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::ChiSqrCutBcOut( Double_t chisqr )
// {
//   ChiSqrCut( m_BcOutTC, chisqr );
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::ChiSqrCutSdcIn( Double_t chisqr )
// {
//   ChiSqrCut( m_SdcInTC, chisqr );
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::ChiSqrCutSdcOut( Double_t chisqr )
// {
//   ChiSqrCut( m_SdcOutTC, chisqr );
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::ChiSqrCutSsdIn( Double_t chisqr )
// {
//   ChiSqrCut( m_SsdInTC, chisqr );
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::ChiSqrCutSsdOut( Double_t chisqr )
// {
//   ChiSqrCut( m_SsdOutTC, chisqr );
// }

// //______________________________________________________________________________
// void
// DCAnalyzer::ChiSqrCut( DCLocalTrackContainer& TrackCont,
// 		       Double_t chisqr )
// {
//   DCLocalTrackContainer DeleteCand;
//   DCLocalTrackContainer ValidCand;
//   Int_t NofTrack = TrackCont.size();
//   for( Int_t i=NofTrack-1; i>=0; --i ){
//     DCLocalTrack* tempTrack = TrackCont[i];
//     if(tempTrack->GetChiSquare() > chisqr){
//       DeleteCand.push_back(tempTrack);
//     }else{
//       ValidCand.push_back(tempTrack);
//     }
//   }

//   utility::ClearContainer( DeleteCand );

//   TrackCont.clear();
//   TrackCont.resize( ValidCand.size() );
//   std::copy( ValidCand.begin(), ValidCand.end(), TrackCont.begin() );
//   ValidCand.clear();
// }
