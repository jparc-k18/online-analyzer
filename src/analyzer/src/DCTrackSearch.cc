// -*- C++ -*-

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>

#include "DCGeomMan.hh"
#include "DCLocalTrack.hh"
#include "DCLTrackHit.hh"
#include "DCPairHitCluster.hh"
#include "DCParameters.hh"
#include "DCTrackSearch.hh"
#include "DetectorID.hh"
#include "FuncName.hh"
// #include "Hodo1Hit.hh"
// #include "Hodo2Hit.hh"
#include "MathTools.hh"
// #include "MWPCCluster.hh"
// #include "SsdCluster.hh"
// #include "TrackMaker.hh"
#include "UserParamMan.hh"
#include "DeleteUtility.hh"

namespace
{
  DCGeomMan& gGeom = DCGeomMan::GetInstance();
  const Double_t MaxChisquare    = 2000.; // Set to be More than 30
  const Double_t MaxNumOfCluster = 30.; // Set to be Less than 30
  const Double_t MaxCombi = 1.0e6; // Set to be Less than 10^6
  // SdcIn & BcOut for XUV Tracking routine
  const Double_t MaxChisquareVXU = 50.;//
  const Double_t ChisquareCutVXU = 50.;//
  const Double_t MaxChisquarePreSsdXY = 1.0e5;

  //_____________________________________________________________________
  // Local Functions ____________________________________________________

  //_____________________________________________________________________
  inline void
  CalcTracks( std::vector<DCLocalTrack*>& trackCont )
  {
    for( Int_t i=0, n=trackCont.size(); i<n; ++i )
      trackCont[i]->Calculate();
  }

  //_____________________________________________________________________
  inline void
  ClearFlags( std::vector<DCLocalTrack*>& trackCont )
  {
    for( Int_t i=0, n=trackCont.size(); i<n; ++i){
      const DCLocalTrack* const tp = trackCont.at(i);
      if (!tp) continue;
      for( Int_t j=0, m=tp->GetNHit(); j<m; ++j )
	tp->GetHit(j)->QuitTrack();
    }
  }

  //_____________________________________________________________________
  inline void
  SetKaonFlags( std::vector<DCLocalTrack*>& TrackCont )
  {
    for( Int_t i=0, n=TrackCont.size(); i<n; ++i ){
      const DCLocalTrack* const tp = TrackCont[i];
      if (!tp) continue;
      for( Int_t ii=0, nn=tp->GetNHit(); ii<nn; ++ii ){
	tp->GetHit(ii)->JoinKaonTrack();
      }
    }
  }

  //_____________________________________________________________________
  inline void
  DeleteDuplicatedTracks( std::vector<DCLocalTrack*>& trackCont,
			  Double_t ChisqrCut=0. )
  {
    // evaluate container size in every iteration
    for( Int_t i=0; i<(Int_t)trackCont.size(); ++i ){
      const DCLocalTrack* const tp = trackCont.at(i);
      if (!tp) continue;
      Int_t nh = tp->GetNHit();
      for( Int_t j=0; j<nh; ++j ){
	tp->GetHit(j)->JoinTrack();
      }
      for( Int_t i2=trackCont.size()-1; i2>i; --i2 ){
	const DCLocalTrack* tp2 = trackCont.at(i2);
	Int_t nh2 = tp2->GetNHit(), flag=0;
	Double_t chisqr = tp2->GetChiSquare();
	for( Int_t j=0; j<nh2; ++j )
	  if( tp2->GetHit(j)->BelongToTrack() ) ++flag;
	if( flag>0 && chisqr>ChisqrCut ){
	  delete tp2;
	  tp2 = 0;
	  trackCont.erase(trackCont.begin()+i2);
	}
      }
    }
  }

  //_____________________________________________________________________
  inline void // for SSD PreTrack
  DeleteWideTracks( std::vector<DCLocalTrack*>& TrackContX,
		    std::vector<DCLocalTrack*>& TrackContY,
		    const Int_t Nth=1 )
  {
    const Double_t xSize = 400./2*1.; // SDC1 X-Size
    const Double_t ySize = 250./2*1.; // SDC1 Y-Size
    const Double_t zPos  = -658.;  // SDC1 Z-Position "V1"
    // X
    if( (Int_t)TrackContX.size()>Nth ){
      for( Int_t i=TrackContX.size()-1; i>=0; --i ){
	const DCLocalTrack* tp = TrackContX.at(i);
	if (!tp) continue;
	Double_t x = tp->GetVXU(zPos);
	if( TMath::Abs(x)>xSize ){
	  delete tp;
	  tp = 0;
	  TrackContX.erase( TrackContX.begin()+i );
	}
      }
    }
    // Y
    if( (Int_t)TrackContY.size()>Nth ){
      for( Int_t i=TrackContY.size()-1; i>=0; --i ){
	const DCLocalTrack* tp = TrackContY.at(i);
	if (!tp) continue;
	Double_t y = tp->GetVXU(zPos);
	if( TMath::Abs(y)>ySize ){
	  delete tp;
	  tp = 0;
	  TrackContY.erase( TrackContY.begin()+i );
	}
      }
    }
  }

  //_____________________________________________________________________
  inline void // for SSD PreTrack
  DeleteWideTracks( std::vector<DCLocalTrack*>& TrackContX,
		    const std::vector<DCHitContainer>& SdcInHC,
		    const Int_t Nth=1 )
  {
    const Double_t xSize =  100.;    // MaxDiff
    const Double_t zPos  = -635.467; // SDC1 Z-Position "X1/X2"
    // X
    if( (Int_t)TrackContX.size()>Nth ){
      for( Int_t i=TrackContX.size()-1; i>=0; --i ){
	const DCLocalTrack* tp = TrackContX[i];
	if (!tp) continue;
	Double_t x = tp->GetVXU(zPos);

	Bool_t accept = false;
	for( Int_t j=0; j<NumOfLayersSDC1; ++j ){
	  if( j!=2 && j!=3 ) continue; // only X1/X2
	  const Int_t nh = SdcInHC[j+1].size();
	  for( Int_t k=0; k<nh; ++k ){
	    const DCHit *hit = SdcInHC[j+1][k];
	    if( !hit ) continue;
	    if( TMath::Abs( x-hit->GetWirePosition() ) < xSize ){
	      accept = true;
	    }
	  }
	}
	if( !accept ){
	  delete tp;
	  tp = 0;
	  TrackContX.erase( TrackContX.begin()+i );
	}
      }
    }

    return;
  }

  //_____________________________________________________________________
  inline void
  DebugPrint( const IndexList& nCombi,
	      const TString& func_name="",
	      const TString& msg="" )
  {
    Int_t n  =1;
    Int_t nn =1;
    Int_t sum=0;
    hddaq::cout << "#D " << func_name << ":" ;
    IndexList::const_iterator itr, end = nCombi.end();
    for( itr=nCombi.begin(); itr!=end; ++itr ){
      Int_t val = *itr;
      sum += val;
      nn *= (val+1);
      hddaq::cout << " " << val;
      if ( val!=0 ) n *= val;
    }
    if (sum==0)
      n=0;
    hddaq::cout << ": total = " << n << ", " << nn << ", " << std::endl;
    return;
  }

  //_____________________________________________________________________
  inline void
  DebugPrint( const std::vector<DCLocalTrack*>& trackCont,
	      const TString& arg="" )
  {
    const Int_t nn = trackCont.size();
    hddaq::cout << arg << " " << nn << std::endl;
    for( Int_t i=0; i<nn; ++i ){
      const DCLocalTrack * const track=trackCont.at(i);
      if (!track) continue;
      Int_t    nh     = track->GetNHit();
      Double_t chisqr = track->GetChiSquare();
      hddaq::cout << std::setw(4) << i
		  << "  #Hits : " << std::setw(2) << nh
		  << "  ChiSqr : " << chisqr << std::endl;
    }
    hddaq::cout << std::endl;
  }

  //_____________________________________________________________________
  inline void
  DebugPrint( const IndexList& nCombi,
	      const std::vector<ClusterList>& CandCont,
	      const TString& arg="" )
  {
    hddaq::cout << arg << " #Hits of each group" << std::endl;
    Int_t np = nCombi.size();
    Int_t nn = 1;
    for( Int_t i=0; i<np; ++i ){
      hddaq::cout << std::setw(4) << nCombi[i];
      nn *= nCombi[i] + 1;
    }
    hddaq::cout << " -> " << nn-1 << " Combinations" << std::endl;
    for( Int_t i=0; i<np; ++i ){
      Int_t n=CandCont[i].size();
      hddaq::cout << "[" << std::setw(3) << i << "]: "
		  << std::setw(3) << n << " ";
      for( Int_t j=0; j<n; ++j ){
	hddaq::cout << ((DCLTrackHit *)CandCont[i][j]->GetHit(0))->GetWire() << " ";
      }
      hddaq::cout << std::endl;
    }
  }

  //______________________________________________________________________________
  template <class Functor>
  inline void
  FinalizeTrack( const TString& arg,
		 std::vector<DCLocalTrack*>& trackCont,
		 Functor comp,
		 std::vector<ClusterList>& candCont,
		 Bool_t delete_flag=true )
  {
    ClearFlags(trackCont);

#if 0
    DebugPrint( trackCont, arg+" Before Sorting " );
#endif

    std::stable_sort( trackCont.begin(), trackCont.end(), comp );

#if 0
    DebugPrint( trackCont, arg+" After Sorting " );
#endif

    if( delete_flag )
      DeleteDuplicatedTracks( trackCont );

#if 0
    DebugPrint( trackCont, arg+" After Deleting " );
#endif

    CalcTracks( trackCont );
    utility::ClearContainerAll( candCont );
  }
}

//______________________________________________________________________________
namespace track
{
  //______________________________________________________________________________
  Bool_t
  MakePairPlaneHitCluster( const DCHitContainer & HC1,
			   const DCHitContainer & HC2,
			   Double_t CellSize,
			   ClusterList& Cont,
			   Bool_t honeycomb )
  {
    Int_t nh1=HC1.size(), nh2=HC2.size();
    std::vector<Int_t> UsedFlag(nh2,0);
    for( Int_t i1=0; i1<nh1; ++i1 ){
      DCHit *hit1 = HC1[i1];
      Double_t wp1 = hit1->GetWirePosition();
      Bool_t flag = false;
      for( Int_t i2=0; i2<nh2; ++i2 ){
	DCHit *hit2=HC2[i2];
	Double_t wp2=hit2->GetWirePosition();
	if( TMath::Abs(wp1-wp2)<CellSize ){
	  Int_t multi1 = hit1->GetDriftLengthSize();
	  Int_t multi2 = hit2->GetDriftLengthSize();
	  for ( Int_t m1=0; m1<multi1; ++m1 ) {
	    if( !hit1->IsWithinRange(m1) )
	      continue;
	    for ( Int_t m2=0; m2<multi2; ++m2 ) {
	      if( !hit2->IsWithinRange(m2) )
		continue;
	      Double_t x1,x2;
	      if( wp1<wp2 ){
		x1=wp1+hit1->GetDriftLength(m1);
		x2=wp2-hit2->GetDriftLength(m2);
	      }
	      else {
		x1=wp1-hit1->GetDriftLength(m1);
		x2=wp2+hit2->GetDriftLength(m2);
	      }
	      DCPairHitCluster *cluster =
		new DCPairHitCluster( new DCLTrackHit(hit1,x1,m1),
				      new DCLTrackHit(hit2,x2,m2) );
	      cluster->SetHoneycomb( honeycomb );
	      Cont.push_back( cluster );
	      flag=true; ++UsedFlag[i2];
	    }
	  }
	}
      }
#if 1
      if(!flag){
	Int_t multi1 = hit1->GetDriftLengthSize();
	for (Int_t m1=0; m1<multi1; m1++) {
	  if( !(hit1->IsWithinRange(m1)) ) continue;
	  Double_t dl=hit1->GetDriftLength(m1);
	  DCPairHitCluster *cluster1 = new DCPairHitCluster( new DCLTrackHit(hit1,wp1+dl,m1) );
	  DCPairHitCluster *cluster2 = new DCPairHitCluster( new DCLTrackHit(hit1,wp1-dl,m1) );
	  cluster1->SetHoneycomb( honeycomb );
	  cluster2->SetHoneycomb( honeycomb );
	  Cont.push_back( cluster1 );
	  Cont.push_back( cluster2 );
	}
      }
#endif
    }
#if 1
    for( Int_t i2=0; i2<nh2; ++i2 ){
      if( UsedFlag[i2]==0 ) {
	DCHit *hit2=HC2[i2];
	Int_t multi2 = hit2->GetDriftLengthSize();
	for (Int_t m2=0; m2<multi2; m2++) {
	  if( !(hit2->IsWithinRange(m2)) ) continue;
	  Double_t wp=hit2->GetWirePosition();
	  Double_t dl=hit2->GetDriftLength(m2);
	  DCPairHitCluster *cluster1 = new DCPairHitCluster( new DCLTrackHit(hit2,wp+dl,m2) );
	  DCPairHitCluster *cluster2 = new DCPairHitCluster( new DCLTrackHit(hit2,wp-dl,m2) );
	  cluster1->SetHoneycomb( honeycomb );
	  cluster2->SetHoneycomb( honeycomb );
	  Cont.push_back( cluster1 );
	  Cont.push_back( cluster2 );
	}
      }
    }
#endif
    return true;
  }

  //______________________________________________________________________________
  Bool_t
  MakeUnPairPlaneHitCluster( const DCHitContainer& HC,
			     ClusterList& Cont,
			     Bool_t honeycomb )
  {
    const Int_t nh = HC.size();
    for( Int_t i=0; i<nh; ++i ){
      DCHit *hit = HC[i];
      if( !hit ) continue;
      Int_t mh = hit->GetDriftLengthSize();
      for ( Int_t m=0; m<mh; ++m ) {
	if( !hit->IsWithinRange(m) ) continue;
	Double_t wp = hit->GetWirePosition();
	Double_t dl = hit->GetDriftLength(m);
	DCPairHitCluster *cluster1 =
	  new DCPairHitCluster( new DCLTrackHit(hit,wp+dl,m) );
	DCPairHitCluster *cluster2 =
	  new DCPairHitCluster( new DCLTrackHit(hit,wp-dl,m) );
	cluster1->SetHoneycomb( honeycomb );
	cluster2->SetHoneycomb( honeycomb );
	Cont.push_back( cluster1 );
	Cont.push_back( cluster2 );
      }
    }

    return true;
  }

  //______________________________________________________________________________
  Bool_t
  MakeMWPCPairPlaneHitCluster( const DCHitContainer& HC,
			       ClusterList& Cont )
  {
    Int_t nh=HC.size();
    for( Int_t i=0; i<nh; ++i ){
      DCHit *hit=HC[i];
      if( hit ){
	Int_t multi = hit->GetDriftTimeSize();
	for (Int_t m=0; m<multi; m++) {
	  if( !(hit->IsWithinRange(m)) ) continue;
	  Double_t wp=hit->GetWirePosition();
	  // Double_t dl=hit->GetDriftLength(m);
	  Cont.push_back( new DCPairHitCluster( new DCLTrackHit( hit, wp, m ) ) );
	}
      }
    }
    return true;
  }

  //______________________________________________________________________________
  Bool_t
  MakeSsdHitCluster( const DCHitContainer& HC,
		     ClusterList& Cont )
  {
    Int_t nh = HC.size();
    for( Int_t i=0; i<nh; ++i ){
      DCHit *hit = HC[i];
      if( !hit ) continue;
      if( !hit->IsGoodWaveForm() ) continue;
      if( hit->BelongToKaonTrack() ) continue;
      Double_t wp = hit->GetWirePosition();
      Cont.push_back( new DCPairHitCluster( new DCLTrackHit( hit, wp, 0 ) ) );
    }
    return true;
  }

  //______________________________________________________________________________
  // Bool_t
  // MakeSsdHitCluster( const SsdClusterContainer& ClCont,
  // 		     ClusterList& Cont )
  // {
  //   Int_t ncl = ClCont.size();
  //   for( Int_t i=0; i<ncl; ++i ){
  //     SsdCluster *cluster = ClCont[i];
  //     if( !cluster ) continue;
  //     if( !cluster->GoodForAnalysis() )  continue;
  //     if( cluster->BelongToKaonTrack() ) continue;
  //     DCHit  *hit = cluster->GetHit();
  //     Double_t  wp  = hit->GetWirePosition();
  //     Cont.push_back( new DCPairHitCluster( new DCLTrackHit( hit, wp, 0 ) ) );
  //   }
  //   return true;
  // }

  //______________________________________________________________________________
  Bool_t
  MakeTOFHitCluster( const DCHitContainer& HitCont,
		     ClusterList& Cont,
		     Int_t xy )
  {
    Int_t nh = HitCont.size();
    for( Int_t i=0; i<nh; ++i ){
      if( i%2!=xy ) continue;
      DCHit *hit = HitCont[i];
      if( !hit ) continue;
      Double_t wp = hit->GetWirePosition();
      Cont.push_back( new DCPairHitCluster( new DCLTrackHit( hit, wp, 0 ) ) );
    }
    return true;
  }

  //______________________________________________________________________________
  Bool_t
  MakePairPlaneHitClusterVUX( const DCHitContainer& HC1,
			      const DCHitContainer& HC2,
			      Double_t CellSize,
			      ClusterList& Cont,
			      Bool_t honeycomb )
  {
    Int_t nh1=HC1.size(), nh2=HC2.size();
    std::vector<Int_t> UsedFlag(nh2,0);

    for( Int_t i1=0; i1<nh1; ++i1 ){
      DCHit *hit1=HC1[i1];

      Double_t wp1=hit1->GetWirePosition();
      Bool_t flag=false;
      for( Int_t i2=0; i2<nh2; ++i2 ){
	DCHit *hit2=HC2[i2];
	Double_t wp2=hit2->GetWirePosition();
	if( TMath::Abs(wp1-wp2)<CellSize ){

	  Int_t multi1 = hit1->GetDriftLengthSize();
	  Int_t multi2 = hit2->GetDriftLengthSize();
	  for (Int_t m1=0; m1<multi1; m1++) {
	    if( !(hit1->IsWithinRange(m1)) ) continue;
	    for (Int_t m2=0; m2<multi2; m2++) {
	      if( !(hit2->IsWithinRange(m2)) ) continue;
	      Double_t dl1=hit1->GetDriftLength(m1);
	      Double_t dl2=hit2->GetDriftLength(m2);

	      Cont.push_back( new DCPairHitCluster( new DCLTrackHit(hit1,wp1+dl1,m1),
						    new DCLTrackHit(hit2,wp2+dl2,m2) ) );
	      Cont.push_back( new DCPairHitCluster( new DCLTrackHit(hit1,wp1+dl1,m1),
						    new DCLTrackHit(hit2,wp2-dl2,m2) ) );
	      Cont.push_back( new DCPairHitCluster( new DCLTrackHit(hit1,wp1-dl1,m1),
						    new DCLTrackHit(hit2,wp2+dl2,m2) ) );
	      Cont.push_back( new DCPairHitCluster( new DCLTrackHit(hit1,wp1-dl1,m1),
						    new DCLTrackHit(hit2,wp2-dl2,m2) ) );

	      flag=true; ++UsedFlag[i2];
	    }
	  }
	}
      }
      if(!flag){
	Int_t multi1 = hit1->GetDriftLengthSize();
	for (Int_t m1=0; m1<multi1; m1++) {
	  if( !(hit1->IsWithinRange(m1)) ) continue;
	  Double_t dl=hit1->GetDriftLength(m1);
	  Cont.push_back( new DCPairHitCluster( new DCLTrackHit(hit1,wp1+dl,m1) ) );
	  Cont.push_back( new DCPairHitCluster( new DCLTrackHit(hit1,wp1-dl,m1) ) );
	}
      }
    }
    for( Int_t i2=0; i2<nh2; ++i2 ){
      if( UsedFlag[i2]==0 ) {
	DCHit *hit2=HC2[i2];
	Int_t multi2 = hit2->GetDriftLengthSize();
	for (Int_t m2=0; m2<multi2; m2++) {
	  if( !(hit2->IsWithinRange(m2)) ) continue;

	  Double_t wp=hit2->GetWirePosition();
	  Double_t dl=hit2->GetDriftLength(m2);
	  Cont.push_back( new DCPairHitCluster( new DCLTrackHit(hit2,wp+dl,m2) ) );
	  Cont.push_back( new DCPairHitCluster( new DCLTrackHit(hit2,wp-dl,m2) ) );
	}
      }
    }

    return true;
  }

  //______________________________________________________________________________
  std::vector<IndexList>
  MakeIndex( Int_t ndim, const Int_t *index1 )
  {
    if( ndim==1 ){
      std::vector<IndexList> index2;
      for( Int_t i=-1; i<index1[0]; ++i ){
	IndexList elem(1,i);
	index2.push_back(elem);
      }
      return index2;
    }

    std::vector<IndexList> index2 = MakeIndex( ndim-1, index1+1 );

    std::vector<IndexList> index;
    Int_t n2=index2.size();
    for( Int_t j=0; j<n2; ++j ){
      for( Int_t i=-1; i<index1[0]; ++i ){
	IndexList elem;
	Int_t n3=index2[j].size();
	elem.reserve(n3+1);
	elem.push_back(i);
	for( Int_t k=0; k<n3; ++k )
	  elem.push_back(index2[j][k]);
	index.push_back(elem);
	Int_t size1=index.size();
	if( size1>MaxCombi ){
#if 1
	  hddaq::cout << FUNC_NAME << " too much combinations..." << std::endl;
#endif
	  return std::vector<IndexList>(0);
	}
      }
    }

    return index;
  }

  //______________________________________________________________________________
  std::vector<IndexList>
  MakeIndex( Int_t ndim, const IndexList& index1 )
  {
    return MakeIndex( ndim, &(index1[0]) );
  }

  //______________________________________________________________________________
  std::vector<IndexList>
  MakeIndex_VXU( Int_t ndim,Int_t maximumHit, const Int_t *index1 )
  {
    if( ndim==1 ){
      std::vector<IndexList> index2;
      for( Int_t i=-1; i<index1[0]; ++i ){
	IndexList elem(1,i);
	index2.push_back(elem);
      }
      return index2;
    }

    std::vector<IndexList>
      index2=MakeIndex_VXU( ndim-1, maximumHit, index1+1 );

    std::vector<IndexList> index;
    Int_t n2=index2.size();
    for( Int_t j=0; j<n2; ++j ){
      for( Int_t i=-1; i<index1[0]; ++i ){
	IndexList elem;
	Int_t validHitNum=0;
	Int_t n3=index2[j].size();
	elem.reserve(n3+1);
	elem.push_back(i);
	if (i != -1)
	  validHitNum++;
	for( Int_t k=0; k<n3; ++k ){
	  elem.push_back(index2[j][k]);
	  if (index2[j][k] != -1)
	    validHitNum++;
	}
	if (validHitNum <= maximumHit)
	  index.push_back(elem);
	// Int_t size1=index.size();
      }
    }

    return index;
  }

  //______________________________________________________________________________
  std::vector<IndexList>
  MakeIndex_VXU( Int_t ndim,Int_t maximumHit, const IndexList& index1 )
  {
    return MakeIndex_VXU( ndim, maximumHit, &(index1[0]) );
  }

  //______________________________________________________________________________
  DCLocalTrack*
  MakeTrack( const std::vector<ClusterList>& CandCont,
	     const IndexList& combination )
  {
    DCLocalTrack *tp = new DCLocalTrack;
    for( Int_t i=0, n=CandCont.size(); i<n; ++i ){
      Int_t m = combination.at(i);
      if( m<0 ) continue;
      DCPairHitCluster *cluster = CandCont.at(i).at(m);
      if( !cluster ) continue;
      Int_t mm = cluster->NumberOfHits();
      for( Int_t j=0; j<mm; ++j ){
	DCLTrackHit *hitp = cluster->GetHit(j);
	if( !hitp ) continue;
	tp->AddHit( hitp );
      }
#if 0
      hddaq::cout << FUNC_NAME << ":" << std::setw(3)
		  << i << std::setw(3) << m  << " "
		  << CandCont.at(i).at(m) << " " << mm << std::endl;
#endif
    }
    return tp;
  }

  //______________________________________________________________________________
  Int_t /* Local Track Search without BH2Filter */
  LocalTrackSearch( const std::vector<DCHitContainer>& HC,
		    const DCPairPlaneInfo * PpInfo,
		    Int_t npp, std::vector<DCLocalTrack*>& TrackCont,
		    Int_t MinNumOfHits, /*=6*/
		    Bool_t delete_flag /*=true*/ )
  {
    std::vector<ClusterList> CandCont(npp);

    for( Int_t i=0; i<npp; ++i ){
      Bool_t ppFlag    = PpInfo[i].pair;
      Bool_t honeycomb = PpInfo[i].honeycomb;
      Bool_t fiber     = PpInfo[i].fiber;
      Int_t  layer1    = PpInfo[i].id1;
      Int_t  layer2    = PpInfo[i].id2;
      if( ppFlag && !fiber ){
	MakePairPlaneHitCluster( HC[layer1], HC[layer2],
				 PpInfo[i].CellSize, CandCont[i], honeycomb );
      }else if( !ppFlag && fiber ){
	PpInfo[i].Print( FUNC_NAME+" invalid parameter", hddaq::cerr );
      }else{
	MakeUnPairPlaneHitCluster( HC[layer1], CandCont[i], honeycomb );
      }
    }

    IndexList nCombi(npp);
    for( Int_t i=0; i<npp; ++i ){
      Int_t n = CandCont[i].size();
      nCombi[i] = n>MaxNumOfCluster ? 0 : n;
    }

#if 0
    DebugPrint( nCombi, CandCont, FUNC_NAME );
#endif

    std::vector<IndexList> CombiIndex = MakeIndex( npp, nCombi );

    for( Int_t i=0, n=CombiIndex.size(); i<n; ++i ){
      DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
      if( !track ) continue;
      if( track->GetNHit()>=MinNumOfHits
    	  && track->DoFit()
    	  && track->GetChiSquare()<MaxChisquare ){
    	TrackCont.push_back( track );
      }
      else
    	delete track;
    }

    FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackComp(), CandCont );
    return TrackCont.size();
  }

  //______________________________________________________________________________
  Int_t /* Local Track Search with BH2Filter */
  LocalTrackSearch( const std::vector< std::vector<DCHitContainer> > &hcAssemble,
		    const DCPairPlaneInfo * PpInfo,
		    Int_t npp, std::vector<DCLocalTrack*> &trackCont,
		    Int_t MinNumOfHits /*=6*/ )
  {
    std::vector< std::vector<DCHitContainer> >::const_iterator
      itr, itr_end = hcAssemble.end();
    for ( itr=hcAssemble.begin(); itr!=itr_end; ++itr ){
      const std::vector<DCHitContainer>& l = *itr;
      std::vector<DCLocalTrack*> tc;
      LocalTrackSearch( l, PpInfo, npp, tc, MinNumOfHits );
      trackCont.insert( trackCont.end(), tc.begin(), tc.end() );
    }

    ClearFlags(trackCont);
    std::stable_sort( trackCont.begin(), trackCont.end(), DCLTrackComp() );
    DeleteDuplicatedTracks( trackCont );
    CalcTracks( trackCont );

    return trackCont.size();
  }

  //______________________________________________________________________________
  Int_t
  LocalTrackSearchSsdOutSdcIn( const std::vector<DCHitContainer>& SdcInHC,
			       const DCPairPlaneInfo *PpInfo,
			       Int_t SdcInNpp,
			       std::vector<DCLocalTrack*>& SsdXTC,
			       std::vector<DCLocalTrack*>& SsdYTC,
			       std::vector<DCLocalTrack*>& TrackCont,
			       Int_t MinNumOfHits /*=10*/ )
  {
    DeleteWideTracks( SsdXTC, SsdYTC );
    DeleteWideTracks( SsdXTC, SdcInHC );

    const Int_t nSsdXTC = SsdXTC.size();
    const Int_t nSsdYTC = SsdYTC.size();

    std::vector< std::vector<DCPairHitCluster*> > CandCont(SdcInNpp);
    for( Int_t i=0; i<SdcInNpp; ++i ){
      Bool_t ppFlag    = PpInfo[i].pair;
      Bool_t honeycomb = PpInfo[i].honeycomb;
      Bool_t fiber     = PpInfo[i].fiber;
      Int_t  layer1    = PpInfo[i].id1;
      Int_t  layer2    = PpInfo[i].id2;
      if( ppFlag && !fiber ){
	MakePairPlaneHitCluster( SdcInHC[layer1], SdcInHC[layer2],
				 PpInfo[i].CellSize, CandCont[i], honeycomb );
      }else if( !ppFlag && fiber ){
	PpInfo[i].Print( FUNC_NAME+" invalid paramter", hddaq::cerr );
      }else{
	MakeUnPairPlaneHitCluster( SdcInHC[layer1], CandCont[i], honeycomb );
      }
    }

    IndexList nCombi(SdcInNpp);
    for( Int_t i=0; i<SdcInNpp; ++i ){
      Int_t n = CandCont[i].size();
      nCombi[i] = n>MaxNumOfCluster ? 0 : n;
    }

#if 0
    DebugPrint( nCombi, CandCont, FUNC_NAME );
#endif

    std::vector<IndexList> CombiIndex = MakeIndex( SdcInNpp, nCombi );
    Int_t nnCombi = CombiIndex.size();
    if( nnCombi==0 ){
      utility::ClearContainerAll( CandCont );
      return -1;
    }

    if( nnCombi > (Int_t)1.0e6 ){
      utility::ClearContainerAll( CandCont );
      hddaq::cout << FUNC_NAME << " too much SdcIn combinations... "
      	      << nnCombi << std::endl;
      return -1;
    }

    Double_t nnn = (Double_t)nnCombi * nSsdXTC * nSsdYTC;
    if( nnn > 1.0e7 || nnn < 0. ){
      utility::ClearContainerAll( CandCont );
      hddaq::cout << FUNC_NAME << " too much SdcIn*SsdX*SsdY combinations... "
      	      << nnn << std::endl;
      return -1;
    }

    for( Int_t iX=0; iX<nSsdXTC; ++iX ){
      const DCLocalTrack * const tpX = SsdXTC[iX];
      if( !tpX ) continue;
      const Int_t nhX = tpX->GetNHit();
      for( Int_t iY=0; iY<nSsdYTC; ++iY ){
	const DCLocalTrack * const tpY = SsdYTC[iY];
	if( !tpY ) continue;
	const Int_t nhY = tpY->GetNHit();

#if 0
	hddaq::cout << " iX : " << std::setw(3) << iX+1 << "/" << std::setw(3) << nSsdXTC << " "
		    << " iY : " << std::setw(3) << iY+1 << "/" << std::setw(3) << nSsdYTC << " "
		    << " nhX : " << nhX << " " << " nhY : " << nhY
		    << " nSdcIn : " << nnCombi << " " << " NNN : " << nnn << std::endl;// << "\x1b[1A";
#endif

	for( Int_t i=0; i<nnCombi; ++i ){
	  DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
	  if( !track ) continue;

	  for( Int_t j=0; j<nhX; ++j )
	    track->AddHit( tpX->GetHit(j) );
	  for( Int_t j=0; j<nhY; ++j )
	    track->AddHit( tpY->GetHit(j) );

	  if( track->GetNHit()>=MinNumOfHits
	      && track->DoFit()
	      && track->GetChiSquare()<MaxChisquare
	      ){
	    TrackCont.push_back( track );
	  }
	  else
	    delete track;
	}
      }// for(iY)
    }// for(iX)

    FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackComp(), CandCont );
    return TrackCont.size();
  }

  //______________________________________________________________________________
  Int_t
  LocalTrackSearchSdcOut( const std::vector<DCHitContainer>& SdcOutHC,
			  const DCPairPlaneInfo *PpInfo,
			  Int_t npp, std::vector<DCLocalTrack*>& TrackCont,
			  Int_t MinNumOfHits /*=6*/ )
  {
    std::vector<ClusterList> CandCont(npp);

    for( Int_t i=0; i<npp; ++i ){
      Bool_t ppFlag    = PpInfo[i].pair;
      Bool_t honeycomb = PpInfo[i].honeycomb;
      Int_t  layer1    = PpInfo[i].id1;
      Int_t  layer2    = PpInfo[i].id2;
      if( ppFlag  )
	MakePairPlaneHitCluster( SdcOutHC[layer1], SdcOutHC[layer2],
				 PpInfo[i].CellSize, CandCont[i], honeycomb );
      else
	MakeUnPairPlaneHitCluster( SdcOutHC[layer1], CandCont[i], honeycomb );
    }

    IndexList nCombi(npp);
    for( Int_t i=0; i<npp; ++i ){
      Int_t n = CandCont[i].size();
      nCombi[i] = n>MaxNumOfCluster ? 0 : n;
    }

    std::vector<IndexList> CombiIndex = MakeIndex( npp, nCombi );

#if 0
    DebugPrint( nCombi, CandCont, FUNC_NAME );
#endif

    for ( Int_t i=0, n=CombiIndex.size(); i<n; ++i ) {
      DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
      if ( !track ) continue;
      if ( track->GetNHit()>=MinNumOfHits     &&
	   track->DoFit()                     &&
	   track->GetChiSquare()<MaxChisquare )
	{
	  TrackCont.push_back( track );
	}
      else
	delete track;
    }

    FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackComp(), CandCont );
    return TrackCont.size();
  }

  //______________________________________________________________________________
  Int_t
  LocalTrackSearchSdcOut( const DCHitContainer& TOFHC,
			  const std::vector<DCHitContainer>& SdcOutHC,
			  const DCPairPlaneInfo *PpInfo,
			  Int_t npp,
			  std::vector<DCLocalTrack*>& TrackCont,
			  Int_t MinNumOfHits /*=6*/ )
  {
    std::vector<ClusterList> CandCont(npp);

    for( Int_t i=0; i<npp; ++i ){
      Bool_t ppFlag    = PpInfo[i].pair;
      Bool_t honeycomb = PpInfo[i].honeycomb;
      Int_t  layer1    = PpInfo[i].id1;
      Int_t  layer2    = PpInfo[i].id2;
      if( ppFlag  )
	MakePairPlaneHitCluster( SdcOutHC[layer1], SdcOutHC[layer2],
				 PpInfo[i].CellSize, CandCont[i], honeycomb );
      else
	MakeUnPairPlaneHitCluster( SdcOutHC[layer1], CandCont[i], honeycomb );
    }

    IndexList nCombi(npp);
    for( Int_t i=0; i<npp; ++i ){
      Int_t n = CandCont[i].size();
      nCombi[i] = n>MaxNumOfCluster ? 0 : n;
    }

    std::vector<IndexList> CombiIndex = MakeIndex( npp, nCombi );

#if 0
    DebugPrint( nCombi, CandCont, FUNC_NAME );
#endif

    for( Int_t i=0, n=CombiIndex.size(); i<n; ++i ){
      for( Int_t j=0, m=TOFHC.size()/2; j<m; ++j ){
	DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
	if( !track ) continue;
	DCHit *hitX = TOFHC[j*2];
	DCHit *hitY = TOFHC[j*2+1];
	Double_t wpX = hitX->GetWirePosition();
	Double_t wpY = hitY->GetWirePosition();
	if( !hitX || !hitY ) continue;
	track->AddHit( new DCLTrackHit( hitX, wpX, 0. ) );
	track->AddHit( new DCLTrackHit( hitY, wpY, 0. ) );
	if( track->GetNHit()>=MinNumOfHits+2   &&
	    track->DoFit()                     &&
	    track->GetChiSquare()<MaxChisquare ){
	  TrackCont.push_back( track );
	}
	else
	  delete track;

      }
    }

    FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackComp(), CandCont );
    return TrackCont.size();
  }

  //______________________________________________________________________________
  Int_t /* Local Track Search SdcIn w/Fiber */
  LocalTrackSearchSdcInFiber( const std::vector<DCHitContainer>& HC,
			      const DCPairPlaneInfo *PpInfo,
			      Int_t npp, std::vector<DCLocalTrack*>& TrackCont,
			      Int_t MinNumOfHits /*=6*/ )
  {
    std::vector<ClusterList> CandCont(npp);

    for( Int_t i=0; i<npp; ++i ){
      Bool_t ppFlag    = PpInfo[i].pair;
      Bool_t honeycomb = PpInfo[i].honeycomb;
      Int_t  layer1    = PpInfo[i].id1;
      Int_t  layer2    = PpInfo[i].id2;
      if(ppFlag) {
	MakePairPlaneHitCluster( HC[layer1], HC[layer2],
				 PpInfo[i].CellSize, CandCont[i], honeycomb );
      }else if(layer1==layer2){
	MakeMWPCPairPlaneHitCluster( HC[layer1], CandCont[i]);
      }else{
	MakeUnPairPlaneHitCluster( HC[layer1], CandCont[i], honeycomb );
      }
    }

    IndexList nCombi(npp);
    for( Int_t i=0; i<npp; ++i ){
      Int_t n = CandCont[i].size();
      nCombi[i] = n>MaxNumOfCluster ? 0 : n;
    }

    std::vector<IndexList> CombiIndex = MakeIndex( npp, nCombi );

    for( Int_t i=0, n=CombiIndex.size(); i<n; ++i ){
      DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
      if( !track ) continue;
      if( track->GetNHit()>=MinNumOfHits && track->DoFit() &&
	  track->GetChiSquare()<MaxChisquare ){
	TrackCont.push_back(track);
      }
      else
	delete track;
    }

    FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackCompSdcInFiber(), CandCont );
    return TrackCont.size();
  }

  // BC3&4, SDC1 VUX Tracking ___________________________________________
  Int_t
  LocalTrackSearchVUX( const std::vector<DCHitContainer>& HC,
		       const DCPairPlaneInfo *PpInfo,
		       Int_t npp, std::vector<DCLocalTrack*>& TrackCont,
		       Int_t MinNumOfHits /*=6*/ )
  {
    std::vector<DCLocalTrack*> TrackContV;
    std::vector<DCLocalTrack*> TrackContX;
    std::vector<DCLocalTrack*> TrackContU;
    std::vector<ClusterList>   CandCont(npp);
    std::vector<ClusterList>   CandContV(npp);
    std::vector<ClusterList>   CandContX(npp);
    std::vector<ClusterList>   CandContU(npp);

    //  Int_t NumOfLayersDC_12 = 12 ;
    Int_t iV=0, iX=0, iU=0;
    Int_t nV=0, nX=0, nU=0;
    for( Int_t i=0; i<npp; ++i ){
      Bool_t ppFlag    = PpInfo[i].pair;
      Bool_t honeycomb = PpInfo[i].honeycomb;
      Int_t  layer1    = PpInfo[i].id1;
      Int_t  layer2    = PpInfo[i].id2;
      Int_t  nh1       = HC[layer1].size();
      Int_t  nh2       = HC[layer2].size();
      Double_t TiltAngle = 0.;
      if(nh1>0) TiltAngle = HC[layer1][0]->GetTiltAngle();
      if( ppFlag && nh1==0 && nh2>0 )
	TiltAngle = HC[layer2][0]->GetTiltAngle();
      if( ppFlag && nh1>0 && nh2>0 ){
	if( TiltAngle<0 ){
	  MakePairPlaneHitClusterVUX( HC[layer1], HC[layer2],
				      PpInfo[i].CellSize, CandContV[iV],
				      honeycomb );
	  ++iV; nV = nV+2;
	}
	if( TiltAngle==0 ){
	  MakePairPlaneHitClusterVUX( HC[layer1], HC[layer2],
				      PpInfo[i].CellSize, CandContX[iX],
				      honeycomb );
	  ++iX; nX = nX+2;
	}
	if( TiltAngle>0 ){
	  MakePairPlaneHitClusterVUX( HC[layer1], HC[layer2],
				      PpInfo[i].CellSize, CandContU[iU],
				      honeycomb );
	  ++iU; nU = nU+2;
	}
      }
      if( !ppFlag ){
	if( TiltAngle<0 ){
	  MakeUnPairPlaneHitCluster( HC[layer1], CandContV[iV], honeycomb );
	  ++nV; ++iV;
	}
	if( TiltAngle==0 ){
	  MakeUnPairPlaneHitCluster( HC[layer1], CandContX[iX], honeycomb );
	  ++nX; ++iX;
	}
	if( TiltAngle>0 ){
	  MakeUnPairPlaneHitCluster( HC[layer1], CandContU[iU], honeycomb );
	  ++nU; ++iU;
	}
      }
      if( ppFlag && nh1==0 && nh2>0 ){
	if( TiltAngle<0 ){
	  MakeUnPairPlaneHitCluster( HC[layer2], CandContV[iV], honeycomb );
	  ++nV; ++iV;
	}
	if( TiltAngle==0 ){
	  MakeUnPairPlaneHitCluster( HC[layer2], CandContX[iX], honeycomb );
	  ++nX; ++iX;
	}
	if( TiltAngle>0 ){
	  MakeUnPairPlaneHitCluster( HC[layer2], CandContU[iU], honeycomb );
	  ++nU; ++iU;
	}
      }
    }

    IndexList nCombi(npp);
    IndexList nCombiV(npp);
    IndexList nCombiX(npp);
    IndexList nCombiU(npp);

    for( Int_t i=0; i<npp; ++i ){
      nCombiV[i]=(CandContV[i]).size();
      nCombiX[i]=(CandContX[i]).size();
      nCombiU[i]=(CandContU[i]).size();
    }

#if 0
    DebugPrint( nCombiV, CandContV, FUNC_NAME+" V" );
    DebugPrint( nCombiX, CandContX, FUNC_NAME+" X" );
    DebugPrint( nCombiU, CandContU, FUNC_NAME+" U" );
#endif

    std::vector<IndexList> CombiIndexV = MakeIndex( npp, nCombiV );
    Int_t nnCombiV=CombiIndexV.size();
    std::vector<IndexList> CombiIndexX = MakeIndex( npp, nCombiX );
    Int_t nnCombiX=CombiIndexX.size();
    std::vector<IndexList> CombiIndexU = MakeIndex( npp, nCombiU );
    Int_t nnCombiU=CombiIndexU.size();

    for( Int_t i=0; i<nnCombiV; ++i ){
      DCLocalTrack *track = MakeTrack( CandContV, CombiIndexV[i] );
      if( !track ) continue;
      if( track->GetNHit()>=3 && track->DoFitVXU() &&
	  track->GetChiSquare()<MaxChisquareVXU ){
	TrackContV.push_back(track);
      }
      else{
	delete track;
      }
    }

    for( Int_t i=0; i<nnCombiX; ++i ){
      DCLocalTrack *track = MakeTrack( CandContX, CombiIndexX[i] );
      if( !track ) continue;
      if( track->GetNHit()>=3 && track->DoFitVXU() &&
	  track->GetChiSquare()<MaxChisquareVXU ){
	TrackContX.push_back(track);
      }
      else{
	delete track;
      }
    }

    for( Int_t i=0; i<nnCombiU; ++i ){
      DCLocalTrack *track = MakeTrack( CandContU, CombiIndexU[i] );
      if( !track ) continue;
      if( track->GetNHit()>=3 && track->DoFitVXU() &&
	  track->GetChiSquare()<MaxChisquareVXU ){
	TrackContU.push_back(track);
      }
      else{
	delete track;
      }
    }

    // Clear Flags
    if(nV>3) ClearFlags( TrackContV );
    if(nX>3) ClearFlags( TrackContX );
    if(nU>3) ClearFlags( TrackContU );

    std::stable_sort( TrackContV.begin(), TrackContV.end(), DCLTrackComp1() );
    std::stable_sort( TrackContX.begin(), TrackContX.end(), DCLTrackComp1() );
    std::stable_sort( TrackContU.begin(), TrackContU.end(), DCLTrackComp1() );

#if 0
    DebugPrint( TrackContV, FUNC_NAME+" V After Sorting." );
    DebugPrint( TrackContX, FUNC_NAME+" X After Sorting." );
    DebugPrint( TrackContU, FUNC_NAME+" U After Sorting." );
#endif

    // Delete Duplicated Tracks (cut chisqr>100 & flag)
    Double_t chiV = ChisquareCutVXU;
    Double_t chiX = ChisquareCutVXU;
    Double_t chiU = ChisquareCutVXU;

    DeleteDuplicatedTracks( TrackContV, chiV );
    DeleteDuplicatedTracks( TrackContX, chiX );
    DeleteDuplicatedTracks( TrackContU, chiU );
    CalcTracks( TrackContV );
    CalcTracks( TrackContX );
    CalcTracks( TrackContU );

#if 0
    DebugPrint( TrackContV, FUNC_NAME+" V After Delete." );
    DebugPrint( TrackContX, FUNC_NAME+" X After Delete." );
    DebugPrint( TrackContU, FUNC_NAME+" U After Delete." );
#endif

    Int_t nnV=1, nnX=1, nnU=1;
    Int_t nkV=0, nkX=0, nkU=0;
    Int_t nnVT=1, nnXT=1, nnUT=1;
    Int_t checkV=0, checkX=0, checkU=0;

    Int_t cV=TrackContV.size();
    if(chiV>1.5 || cV<5 ) checkV++;
    Int_t cX=TrackContX.size();
    if(chiX>1.5 || cX<5 ) checkX++;
    Int_t cU=TrackContU.size();
    if(chiU>1.5 || cU<5 ) checkU++;

    std::vector<IndexList> CombiIndexSV;
    std::vector<IndexList> CombiIndexSX;
    std::vector<IndexList> CombiIndexSU;

    {
      if((nV>=3) && (cV)){
	nnV = TrackContV.size();
	nnVT = TrackContV.size();
	++nkV;
      }
      if((nV>0) && checkV){
	CombiIndexSV = MakeIndex_VXU( npp, 2, nCombiV );
	nnV = nnV +  CombiIndexSV.size();
      }
    }

    {
      if((nX>=3) && (cX)){
	nnX = TrackContX.size();
	nnXT = TrackContX.size();
	++nkX;
      }
      if((nX>0) && checkX){
	CombiIndexSX = MakeIndex_VXU( npp, 2, nCombiX );
	nnX = nnX + CombiIndexSX.size();
      }
    }

    {
      if((nU>=3) && (cU)){
	nnU = TrackContU.size();
	nnUT = TrackContU.size();
	++nkU;
      }
      if((nU>0) && checkU){
	CombiIndexSU = MakeIndex_VXU( npp, 2, nCombiU );
	nnU = nnU + CombiIndexSU.size();
      }
    }

    // Double_t DifVXU=0.0;
    // Double_t Av=0.0;
    // Double_t Ax=0.0;
    // Double_t Au=0.0;

    Double_t chiv, chix, chiu;

#if 0
    for( Int_t i=0; i<nnV; ++i){
      for( Int_t j=0; j<nnX; ++j){
	for( Int_t k=0; k<nnU; ++k){

	  chiv=-1.0,chix=-1.0,chiu=-1.0;

	  DCLocalTrack *track = new DCLocalTrack();

	  /* V Plane  */
	  if(nkV){
	    DCLocalTrack *trackV=TrackContV[i];
	    //Av=trackV->GetVXU_A();
	    chiv=trackV->GetChiSquare();
	    for( Int_t l=0; l<(trackV->GetNHit()); ++l){
	      DCLTrackHit *hitpV=trackV->GetHit(l);
	      if(hitpV){
		track->AddHit( hitpV ) ;
	      }
	    }
	    //delete trackV;
	  }
	  if((!nkV) && (nV>0)){
	    DCLocalTrack *trackV = MakeTrack( CandContV, CombiIndexV[i] );
	    for( Int_t l=0; l<(trackV->GetNHit()); ++l){
	      DCLTrackHit *hitpV=trackV->GetHit(l);
	      if(hitpV){
		track->AddHit( hitpV ) ;
	      }
	    }
	    delete trackV;
	  }

	  /* X Plane  */
	  if(nkX){
	    DCLocalTrack *trackX=TrackContX[j];
	    //Ax=trackX->GetVXU_A();
	    chix=trackX->GetChiSquare();
	    for( Int_t l=0; l<(trackX->GetNHit()); ++l){
	      DCLTrackHit *hitpX=trackX->GetHit(l);
	      if(hitpX){
		track->AddHit( hitpX ) ;
	      }
	    }
	    //delete trackX;
	  }
	  if((!nkX) && (nX>0)){
	    DCLocalTrack *trackX = MakeTrack( CandContX, CombiIndexX[j] );
	    for( Int_t l=0; l<(trackX->GetNHit()); ++l){
	      DCLTrackHit *hitpX=trackX->GetHit(l);
	      if(hitpX){
		track->AddHit( hitpX ) ;
	      }
	    }
	    delete trackX;
	  }

	  /* U Plane  */
	  if(nkU){
	    DCLocalTrack *trackU=TrackContU[k];
	    //Au=trackU->GetVXU_A();
	    chiu=trackU->GetChiSquare();
	    for( Int_t l=0; l<(trackU->GetNHit()); ++l){
	      DCLTrackHit *hitpU=trackU->GetHit(l);
	      if(hitpU){
		track->AddHit( hitpU ) ;
	      }
	    }
	    //delete trackU;
	  }
	  if((!nkU) && (nU>0)){
	    DCLocalTrack *trackU = MakeTrack( CandContU, CombiIndexU[k] );
	    for( Int_t l=0; l<(trackU->GetNHit()); ++l){
	      DCLTrackHit *hitpU=trackU->GetHit(l);
	      if(hitpU){
		track->AddHit( hitpU ) ;
	      }
	    }
	    delete trackU;
	  }
	  //track->SetAv(Av);
	  //track->SetAx(Ax);
	  //track->SetAu(Au);
	  //DifVXU = track->GetDifVXU();

	  track->SetChiv(chiv);
	  track->SetChix(chix);
	  track->SetChiu(chiu);

	  if(!track) continue;
	  if( track->GetNHit()>=MinNumOfHits && track->DoFit() &&
	      track->GetChiSquare()<MaxChisquare ){
	    TrackCont.push_back(track);
	  }
	  else{
	    delete track;
	  }
	}
      }
    }
#endif

    for( Int_t i=-1; i<nnV; ++i){
      for( Int_t j=-1; j<nnX; ++j){
	for( Int_t k=-1; k<nnU; ++k){
	  if( ((i+j)==-2) || ((j+k)==-2) || ((k+i)==-2)) continue;

	  chiv=-1.0,chix=-1.0,chiu=-1.0;

	  DCLocalTrack *track = new DCLocalTrack();

	  /* V Plane  */
	  if(i>-1){
	    if(nkV && i<nnVT){
	      DCLocalTrack *trackV=TrackContV[i];
	      // Av=trackV->GetVXU_A();
	      chiv=trackV->GetChiSquare();

	      for( Int_t l=0; l<(trackV->GetNHit()); ++l){
		DCLTrackHit *hitpV=trackV->GetHit(l);
		if(hitpV){
		  track->AddHit( hitpV ) ;
		}
	      }
	    }
	    if((i>=nnVT) && (nV>0)){
	      DCLocalTrack *trackV = MakeTrack( CandContV, CombiIndexSV[i-nnVT] );
	      for( Int_t l=0; l<(trackV->GetNHit()); ++l){
		DCLTrackHit *hitpV=trackV->GetHit(l);
		if(hitpV){
		  track->AddHit( hitpV ) ;
		}
	      }
	      delete trackV ;
	    }
	    // Int_t NHitV = track->GetNHit();
	  }

	  /* X Plane  */
	  if(j>-1){
	    if(nkX && j<nnXT){
	      DCLocalTrack *trackX=TrackContX[j];
	      // Ax=trackX->GetVXU_A();
	      chix=trackX->GetChiSquare();
	      for( Int_t l=0; l<(trackX->GetNHit()); ++l){
		DCLTrackHit *hitpX=trackX->GetHit(l);
		if(hitpX){
		  track->AddHit( hitpX ) ;
		}
	      }
	    }
	    if((j>=nnXT) && (nX>0)){
	      DCLocalTrack *trackX = MakeTrack( CandContX, CombiIndexSX[j-nnXT] );
	      for( Int_t l=0; l<(trackX->GetNHit()); ++l){
		DCLTrackHit *hitpX=trackX->GetHit(l);
		if(hitpX){
		  track->AddHit( hitpX ) ;
		}
	      }
	      delete trackX;
	    }
	    // Int_t NHitX = track->GetNHit();
	  }
	  /* U Plane  */
	  if(k>-1){
	    if(nkU && k<nnUT){
	      DCLocalTrack *trackU=TrackContU[k];
	      // Au=trackU->GetVXU_A();
	      chiu=trackU->GetChiSquare();
	      for( Int_t l=0; l<(trackU->GetNHit()); ++l){
		DCLTrackHit *hitpU=trackU->GetHit(l);
		if(hitpU){
		  track->AddHit( hitpU ) ;
		}
	      }
	    }
	    if((k>=nnUT) && (nU>0)){
	      DCLocalTrack *trackU = MakeTrack( CandContU, CombiIndexSU[k-nnUT] );
	      for( Int_t l=0; l<(trackU->GetNHit()); ++l){
		DCLTrackHit *hitpU=trackU->GetHit(l);
		if(hitpU){
		  track->AddHit( hitpU ) ;
		}
	      }
	      delete trackU;
	    }
	    // Int_t NHitU = track->GetNHit();
	  }

	  //track->SetAv(Av);
	  //track->SetAx(Ax);
	  //track->SetAu(Au);
	  //DifVXU = track->GetDifVXU();

	  track->SetChiv(chiv);
	  track->SetChix(chix);
	  track->SetChiu(chiu);

	  if(!track) continue;
	  if(track->GetNHit()>=MinNumOfHits && track->DoFit() &&
	     track->GetChiSquare()<MaxChisquare ){//MaXChisquare
	    TrackCont.push_back(track);
	  }
	  else{
	    delete track;
	  }
	}
      }
    }

    {

      for( Int_t i=0; i<Int_t(TrackContV.size()); ++i ){
	DCLocalTrack *tp=TrackContV[i];
	delete tp;
	TrackContV.erase(TrackContV.begin()+i);
      }
      for( Int_t i=0; i<Int_t(TrackContX.size()); ++i ){
	DCLocalTrack *tp=TrackContX[i];
	delete tp;
	TrackContX.erase(TrackContX.begin()+i);
      }
      for( Int_t i=0; i<Int_t(TrackContU.size()); ++i ){
	DCLocalTrack *tp=TrackContU[i];
	delete tp;
	TrackContU.erase(TrackContU.begin()+i);
      }

    }

    ClearFlags( TrackCont );

    std::stable_sort( TrackCont.begin(), TrackCont.end(), DCLTrackComp1() );

#if 1
    // Delete Tracks about  (Nhit1 > Nhit2+1) (Nhit1 > Nhit2  && chi1 < chi2)
    for( Int_t i=0; i<Int_t(TrackCont.size()); ++i ){
      DCLocalTrack *tp=TrackCont[i];
      Int_t nh=tp->GetNHit();
      Double_t chi=tp->GetChiSquare();
      for( Int_t j=0; j<nh; ++j ) tp->GetHit(j)->JoinTrack();

      for( Int_t i2=TrackCont.size()-1; i2>i; --i2 ){
	DCLocalTrack *tp2=TrackCont[i2];
	Int_t nh2=tp2->GetNHit(), flag=0;
	Double_t chi2=tp2->GetChiSquare();
	for( Int_t j=0; j<nh2; ++j )
	  if( tp2->GetHit(j)->BelongToTrack() ) ++flag;
	if((flag>=2) && ((nh==nh2) || ((nh>nh2) && (chi<chi2)))){
	  //      if((flag) && ((nh>nh2+1) || ((nh==nh2) || (nh>nh2) && (chi<chi2)))){
	  //if((nh>nh2) && (chi<chi2)){
	  delete tp2;
	  TrackCont.erase(TrackCont.begin()+i2);
	}
      }
    }
#endif

    FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackCompSdcInFiber(), CandCont );

    std::stable_sort( TrackCont.begin(), TrackCont.end(), DCLTrackComp() );

    // Clear Flags
    {
      Int_t nbefore=TrackCont.size();
      for( Int_t i=0; i<nbefore; ++i ){
	DCLocalTrack *tp=TrackCont[i];
	Int_t nh=tp->GetNHit();
	for( Int_t j=0; j<nh; ++j ){
	  tp->GetHit(j)->QuitTrack();
	}
      }
    }

    // Delete Duplicated Tracks

    for( Int_t i=0; i<Int_t(TrackCont.size()); ++i ){
      DCLocalTrack *tp=TrackCont[i];
      Int_t nh=tp->GetNHit();
      for( Int_t j=0; j<nh; ++j ) tp->GetHit(j)->JoinTrack();

      for( Int_t i2=TrackCont.size()-1; i2>i; --i2 ){
	DCLocalTrack *tp2=TrackCont[i2];
	Int_t nh2=tp2->GetNHit(), flag=0;
	for( Int_t j=0; j<nh2; ++j )
	  if( tp2->GetHit(j)->BelongToTrack() ) ++flag;
	if(flag){
	  delete tp2;
	  TrackCont.erase(TrackCont.begin()+i2);
	}
      }
    }

    // Clear Flags
    {
      Int_t nbefore=TrackCont.size();
      for( Int_t i=0; i<nbefore; ++i ){
	DCLocalTrack *tp=TrackCont[i];
	Int_t nh=tp->GetNHit();
	for( Int_t j=0; j<nh; ++j ){
	  tp->GetHit(j)->QuitTrack();
	}
      }
    }

    {
      Int_t nn=TrackCont.size();
      for(Int_t i=0; i<nn; ++i ){
	DCLocalTrack *tp=TrackCont[i];
	Int_t nh=tp->GetNHit();
	for( Int_t j=0; j<nh; ++j ){
	  Int_t lnum = tp->GetHit(j)->GetLayer();
	  Double_t zz = gGeom.GetLocalZ( lnum );
	  tp->GetHit(j)->SetCalPosition(tp->GetX(zz), tp->GetY(zz));
	}
      }
    }

#if 0
    if(TrackCont.size()>0){
      Int_t nn=TrackCont.size();
      hddaq::cout << FUNC_NAME << ": After Deleting. #Tracks = " << nn << std::endl;
      for( Int_t i=0; i<nn; ++i ){
	DCLocalTrack *track=TrackCont[i];
	hddaq::cout << std::setw(3) << i << " #Hits="
		    << std::setw(2) << track->GetNHit()
		    << " ChiSqr=" << track->GetChiSquare()
		    << std::endl;
	hddaq::cout << std::endl;
	for( Int_t j=0; j<(track->GetNHit()); ++j){
	  DCLTrackHit *hit = track->GetHit(j);
	  hddaq::cout << "layer = " << hit->GetLayer()+1 << " Res = " << hit->GetResidual() << std::endl ;
	  hddaq::cout << "hitp = " << hit->GetLocalHitPos() << " calp = " << hit->GetLocalCalPos() << std::endl ;
	  hddaq::cout << "X = " << hit->GetXcal() << " Y = " << hit->GetYcal() << std::endl ;
	  hddaq::cout << std::endl;
	}
	hddaq::cout << "*********************************************" << std::endl;
      }
      hddaq::cout << std::endl;
    }
#endif

    utility::ClearContainerAll( CandCont );
    utility::ClearContainerAll( CandContV );
    utility::ClearContainerAll( CandContX );
    utility::ClearContainerAll( CandContU );

    return TrackCont.size();
  }

  //______________________________________________________________________________
  Int_t
  LocalTrackSearchSsdIn( const std::vector<DCHitContainer>& HC,
			 std::vector<DCLocalTrack*>& TrackCont,
			 Int_t MinNumOfHits, /*=4*/
			 Bool_t delete_flag /* =false*/ )
  {
    std::vector<ClusterList> CandCont(NumOfLayersSsdIn);

    for( Int_t i=0; i<NumOfLayersSsdIn; ++i ){
      MakeSsdHitCluster( HC[i+1], CandCont[i] );
    }

    IndexList nCombi(NumOfLayersSsdIn);
    for( Int_t i=0; i<NumOfLayersSsdIn; ++i ){
      nCombi[i]=(CandCont[i]).size();
    }

    std::vector<IndexList>
      CombiIndex = MakeIndex( NumOfLayersSsdIn, nCombi );
    Int_t nnCombi=CombiIndex.size();

    for( Int_t i=0; i<nnCombi; ++i ){
      DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
      if( !track ) continue;
      if( track->GetNHit()>=MinNumOfHits && track->DoFit()
	  ){
	TrackCont.push_back(track);
      }
      else
	delete track;
    }

    if( TrackCont.empty() ) {
      utility::ClearContainerAll( CandCont );
      return 0;
    }

    FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackCompSsd(), CandCont, delete_flag );
    return TrackCont.size();
  }

  //______________________________________________________________________________
  // Int_t
  // LocalTrackSearchSsdIn( const std::vector<SsdClusterContainer>& SsdInClCont,
  // 			 std::vector<DCLocalTrack*>& TrackCont,
  // 			 Int_t MinNumOfHits, /*=4*/
  // 			 Bool_t delete_flag /* =false*/ )
  // {
  //   Int_t npp = NumOfLayersSsdIn;
  //   std::vector<ClusterList> CandCont( npp );

  //   for( Int_t i=0; i<NumOfLayersSsdIn; ++i ){
  //     MakeSsdHitCluster( SsdInClCont[i+1], CandCont[i] );
  //   }

  //   IndexList nCombi(npp);

  //   for( Int_t i=0; i<npp; ++i ){
  //     nCombi[i]=(CandCont[i]).size();
  //   }

  //   std::vector<IndexList> CombiIndex = MakeIndex( npp, nCombi );

  //   Int_t nnCombi = CombiIndex.size();
  //   for( Int_t i=0; i<nnCombi; ++i ){
  //     DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
  //     if( !track ) continue;
  //     if( track->GetNHit()>=MinNumOfHits && track->DoFit() ){
  // 	TrackCont.push_back( track );
  //     } else {
  // 	delete track;
  //     }
  //   }

  //   FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackCompSsd(), CandCont, delete_flag );
  //   return TrackCont.size();
  // }

  //______________________________________________________________________________
  // Int_t
  // LocalTrackSearchSsdOut( const std::vector<DCHitContainer>& HC,
  // 			  std::vector<DCLocalTrack*>& TrackCont,
  // 			  Int_t MinNumOfHits /*=4*/ )
  // {
  //   std::vector<ClusterList> CandCont(NumOfLayersSsdOut);

  //   for( Int_t i=0; i<NumOfLayersSsdOut; ++i ){
  //     MakeSsdHitCluster( HC[i+1], CandCont[i] );
  //   }

  //   IndexList nCombi(NumOfLayersSsdOut);
  //   for( Int_t i=0; i<NumOfLayersSsdOut; ++i ){
  //     nCombi[i]=(CandCont[i]).size();
  //   }

  //   std::vector<IndexList> CombiIndex = MakeIndex( NumOfLayersSsdOut, nCombi );
  //   Int_t nnCombi=CombiIndex.size();
  //   for( Int_t i=0; i<nnCombi; ++i ){
  //     DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
  //     if( !track ) continue;
  //     if( track->GetNHit()>=MinNumOfHits && track->DoFit() ){
  // 	TrackCont.push_back( track );
  //     } else {
  // 	delete track;
  //     }
  //   }

  //   FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackCompSsd(), CandCont, false );
  //   return TrackCont.size();
  // }

  //______________________________________________________________________________
  // Int_t
  // LocalTrackSearchSsdOut( const std::vector<SsdClusterContainer>& SsdOutClCont,
  // 			  std::vector<DCLocalTrack*>& TrackCont,
  // 			  Int_t MinNumOfHits /*=4*/ )
  // {
  //   Int_t npp = NumOfLayersSsdOut;
  //   std::vector< std::vector<DCPairHitCluster*> > CandCont( npp );
  //   for( Int_t i=0; i<NumOfLayersSsdOut; ++i ){
  //     MakeSsdHitCluster( SsdOutClCont[i+1], CandCont[i] );
  //   }

  //   IndexList nCombi(npp);
  //   for( Int_t i=0; i<npp; ++i ){
  //     nCombi[i]=(CandCont[i]).size();
  //   }

  //   std::vector<IndexList> CombiIndex = MakeIndex( npp, nCombi );

  //   Int_t nnCombi = CombiIndex.size();
  //   for( Int_t i=0; i<nnCombi; ++i ){
  //     DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
  //     if( !track ) continue;
  //     if( track->GetNHit()>=MinNumOfHits && track->DoFit() ){
  // 	TrackCont.push_back( track );
  //     } else {
  // 	delete track;
  //     }
  //   }

  //   FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackCompSsd(), CandCont, false );
  //   return TrackCont.size();
  // }

  //______________________________________________________________________________
  // Int_t
  // LocalTrackSearchSsdInXY( const std::vector<SsdClusterContainer>& SsdInClCont,
  // 			   std::vector<DCLocalTrack*>& TrackContX,
  // 			   std::vector<DCLocalTrack*>& TrackContY )
  // {
  //   Int_t npp = 2;
  //   std::vector< std::vector<DCPairHitCluster*> > CandContX(npp);
  //   std::vector< std::vector<DCPairHitCluster*> > CandContY(npp);

  //   for( Int_t i=0; i<NumOfLayersSsdIn; ++i ){
  //     if( SsdInClCont[i+1].size()==0 ) continue;
  //     SsdCluster *cluster = SsdInClCont[i+1][0];
  //     if( !cluster ) continue;
  //     Int_t    layer = cluster->LayerId() - PlOffsSsd;
  //     Double_t tilt  = cluster->TiltAngle();
  //     if( tilt==0. || tilt==180. ){
  // 	MakeSsdHitCluster( SsdInClCont[layer], CandContX[layer-1] );
  //     }
  //     else{
  // 	MakeSsdHitCluster( SsdInClCont[layer], CandContY[layer-1] );
  //     }
  //   }

  //   IndexList nCombiX(npp);
  //   IndexList nCombiY(npp);

  //   for( Int_t i=0; i<npp; ++i ){
  //     nCombiX[i]=(CandContX[i]).size();
  //     nCombiY[i]=(CandContY[i]).size();
  //   }

  //   std::vector<IndexList> CombiIndexX = MakeIndex( npp, nCombiX );
  //   std::vector<IndexList> CombiIndexY = MakeIndex( npp, nCombiY );
  //   Int_t nnCombiX = CombiIndexX.size();
  //   Int_t nnCombiY = CombiIndexY.size();

  //   for( Int_t i=0; i<nnCombiX; ++i ){
  //     DCLocalTrack *track = MakeTrack( CandContX, CombiIndexX[i] );
  //     if( !track ) continue;
  //     if( track->DoFitVXU() ){
  // 	TrackContX.push_back( track );
  //     } else {
  // 	delete track;
  //     }
  //   }
  //   for( Int_t i=0; i<nnCombiY; ++i ){
  //     DCLocalTrack *track = MakeTrack( CandContY, CombiIndexY[i] );
  //     if( !track ) continue;
  //     if( track->DoFitVXU() ){
  // 	TrackContY.push_back( track );
  //     } else {
  // 	delete track;
  //     }
  //   }

  //   FinalizeTrack( FUNC_NAME, TrackContX, DCLTrackCompSsd(), CandContX, false );
  //   FinalizeTrack( FUNC_NAME, TrackContY, DCLTrackCompSsd(), CandContY, false );
  //   return TrackContX.size()*TrackContY.size();
  // }

  //______________________________________________________________________________
//   Int_t
//   PreTrackSearchSsdXY( const std::vector<SsdClusterContainer>& SsdInClCont,
// 		       const std::vector<SsdClusterContainer>& SsdOutClCont,
// 		       std::vector<DCLocalTrack*>& TrackContX,
// 		       std::vector<DCLocalTrack*>& TrackContY )
//   {
//     static const Int_t npp = NumOfLayersSsdIn + NumOfLayersSsdOut;

//     std::vector<ClusterList> CandContX(npp);
//     std::vector<ClusterList> CandContY(npp);

//     Int_t nX=0, nY=0;
//     for( Int_t i=0; i<NumOfLayersSsdIn; ++i ){
//       if( SsdInClCont[i+1].size()==0 ) continue;
//       DCHit *hit   = SsdInClCont[i+1][0]->GetHit(0);
//       if( !hit ) continue;
//       Int_t    layer = hit->GetLayer() - PlOffsSsd;
//       Double_t tilt  = hit->GetTiltAngle();
//       if( tilt==0. || tilt==180. ){
// 	MakeSsdHitCluster( SsdInClCont[layer], CandContX[layer-1] );
// 	++nX;
//       }
//       else{
// 	MakeSsdHitCluster( SsdInClCont[layer], CandContY[layer-1] );
// 	++nY;
//       }
//     }

//     for( Int_t i=0; i<NumOfLayersSsdOut; ++i ){
//       if( SsdOutClCont[i+1].size()==0 ) continue;
//       DCHit *hit = SsdOutClCont[i+1][0]->GetHit(0);
//       if( !hit ) continue;
//       Int_t    layer = hit->GetLayer() - PlOffsSsd;
//       Double_t tilt  = hit->GetTiltAngle();
//       if( tilt==0. || tilt==180. ){
// 	MakeSsdHitCluster( SsdOutClCont[layer-NumOfLayersSsdIn], CandContX[layer-1] );
// 	++nX;
//       }
//       else{
// 	MakeSsdHitCluster( SsdOutClCont[layer-NumOfLayersSsdIn], CandContY[layer-1] );
// 	++nY;
//       }
//     }

//     IndexList nCombiX(npp);
//     IndexList nCombiY(npp);

//     for( Int_t i=0; i<npp; ++i ){
//       Int_t n = CandContX[i].size();
//       Int_t m = CandContY[i].size();
//       nCombiX[i] = n>MaxNumOfCluster ? 0 : n;
//       nCombiY[i] = m>MaxNumOfCluster ? 0 : m;
//     }

// #if 0
//     DebugPrint( nCombiX, CandContX, FUNC_NAME+" X " );
//     DebugPrint( nCombiY, CandContY, FUNC_NAME+" Y " );
// #endif

//     std::vector<IndexList> CombiIndexX = MakeIndex( npp, nCombiX );
//     std::vector<IndexList> CombiIndexY = MakeIndex( npp, nCombiY );
//     Int_t nnCombiX=CombiIndexX.size();
//     Int_t nnCombiY=CombiIndexY.size();

// #if 0
//     hddaq::cout << "X Plane [" << nX << "] " << nnCombiX
// 		<< " combinations will be checked..." << std::endl;
//     hddaq::cout << "Y Plane [" << nY << "] " << nnCombiY
// 		<< " combinations will be checked..." << std::endl;
// #endif

//     for( Int_t i=0; i<nnCombiX; ++i ){
//       DCLocalTrack *track = MakeTrack( CandContX, CombiIndexX[i] );
//       if( !track ) continue;
//       if( track->GetNHit()>=2 && track->DoFitVXU()
// 	  && track->GetChiSquare()<MaxChisquarePreSsdXY
// 	  ){
// 	TrackContX.push_back(track);
//       }else{
// 	delete track;
//       }
//     }

//     for( Int_t i=0; i<nnCombiY; ++i ){
//       DCLocalTrack *track = MakeTrack( CandContY, CombiIndexY[i] );
//       if( !track ) continue;
//       if( track->GetNHit()>=2 && track->DoFitVXU()
// 	  && track->GetChiSquare()<MaxChisquarePreSsdXY
// 	  ){
// 	TrackContY.push_back(track);
//       }else{
// 	delete track;
//       }
//     }

//     FinalizeTrack( FUNC_NAME, TrackContX, DCLTrackComp1(), CandContX, false );
//     FinalizeTrack( FUNC_NAME, TrackContY, DCLTrackComp1(), CandContY, false );
//     return 0;
//   }

  //______________________________________________________________________________
  Int_t
  LocalTrackSearchBcOutSdcIn( const std::vector<DCHitContainer>& BcHC,
			      const DCPairPlaneInfo *BcPpInfo,
			      const std::vector<DCHitContainer>& SdcHC,
			      const DCPairPlaneInfo *SdcPpInfo,
			      Int_t BcNpp, Int_t SdcNpp,
			      std::vector<DCLocalTrack*>& TrackCont,
			      Int_t MinNumOfHits )
  {
    const Int_t npp = BcNpp + SdcNpp;

    std::vector<ClusterList> CandCont(npp);

    for( Int_t i=0; i<BcNpp; ++i ){
      Bool_t ppFlag=BcPpInfo[i].pair;
      Int_t layer1=BcPpInfo[i].id1, layer2=BcPpInfo[i].id2;
      if(ppFlag)
	MakePairPlaneHitCluster( BcHC[layer1], BcHC[layer2],
				 BcPpInfo[i].CellSize, CandCont[i] );
      else
	MakeUnPairPlaneHitCluster( BcHC[layer1], CandCont[i] );
    }

    for( Int_t i=0; i<SdcNpp; ++i ){
      Bool_t ppFlag=SdcPpInfo[i].pair;
      Int_t layer1=SdcPpInfo[i].id1, layer2=SdcPpInfo[i].id2;
      if(ppFlag)
	MakePairPlaneHitCluster( SdcHC[layer1], SdcHC[layer2],
				 SdcPpInfo[i].CellSize, CandCont[i+BcNpp] );
      else
	MakeUnPairPlaneHitCluster( SdcHC[layer1], CandCont[i+BcNpp] );
    }

    IndexList nCombi( npp );
    for( Int_t i=0; i<npp; ++i ){
      Int_t n = CandCont[i].size();
      nCombi[i] = n>MaxNumOfCluster ? 0 : n;
    }

#if 0
    DebugPrint( nCombi, CandCont, FUNC_NAME );
#endif

    std::vector<IndexList> CombiIndex = MakeIndex( npp, nCombi );
    Int_t nnCombi = CombiIndex.size();

#if 0
    hddaq::cout << " ===> " << nnCombi << " combinations will be checked.."
		<< std::endl;
    for( Int_t i=0; i<nnCombi; ++i ){
      for (Int_t j=0;j<npp;j++) {
	hddaq::cout << CombiIndex[i][j] << " ";
      }
      hddaq::cout << std::endl;
    }
#endif

    for( Int_t i=0; i<nnCombi; ++i ){
      DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
      if( !track ) continue;
      if( track->GetNHit()>=MinNumOfHits && track->DoFitBcSdc() &&
	  track->GetChiSquare()<MaxChisquare )
	TrackCont.push_back(track);
      else
	delete track;
    }

    FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackComp(), CandCont );

    // {
    //   static const Double_t& zTarget    = gGeom.GetLocalZ("Target");
    //   static const Double_t& zK18Target = gGeom.GetLocalZ("K18Target");
    //   Int_t nn=TrackCont.size();
    //   for(Int_t i=0; i<nn; ++i ){
    // 	DCLocalTrack *tp = TrackCont[i];
    // 	Int_t nh=tp->GetNHit();
    // 	for( Int_t j=0; j<nh; ++j ){
    // 	  Int_t lnum = tp->GetHit(j)->GetLayer();
    // 	  Double_t zz = gGeom.GetLocalZ( lnum );
    // 	  if ( lnum>=113 && lnum<=124 )
    // 	    zz -= ( zK18Target - zTarget );
    // 	  tp->GetHit(j)->SetCalPosition(tp->GetX(zz), tp->GetY(zz));
    // 	}
    //   }
    // }

    return TrackCont.size();
  }

  //______________________________________________________________________________
  Int_t
  LocalTrackSearchBcOutSsdIn( const std::vector<DCHitContainer>& BcHC,
			      const DCPairPlaneInfo *BcPpInfo,
			      Int_t BcNpp,
			      const std::vector<DCHitContainer>& SsdHC,
			      std::vector<DCLocalTrack*>& TrackCont,
			      Int_t MinNumOfHits )
  {
    const Int_t npp = BcNpp+NumOfLayersSsdIn;

    std::vector<ClusterList> CandCont(npp);

    for( Int_t i=0; i<BcNpp; ++i ){
      Bool_t ppFlag=BcPpInfo[i].pair;
      Int_t layer1=BcPpInfo[i].id1, layer2=BcPpInfo[i].id2;
      if(ppFlag)
	MakePairPlaneHitCluster( BcHC[layer1], BcHC[layer2],
				 BcPpInfo[i].CellSize, CandCont[i] );
      else
	MakeUnPairPlaneHitCluster( BcHC[layer1], CandCont[i] );
    }

    for( Int_t i=0; i<NumOfLayersSsdIn; ++i ){
      MakeSsdHitCluster( SsdHC[i+1], CandCont[i+BcNpp] );
    }

    IndexList nCombi(npp);

    for( Int_t i=0; i<npp; ++i ){
      Int_t n = CandCont[i].size();
      nCombi[i] = n>MaxNumOfCluster ? 0 : n;
    }

#if 0
    DebugPrint(nCombi, CandCont, FUNC_NAME);
#endif

    std::vector<IndexList> CombiIndex = MakeIndex( npp, nCombi );
    Int_t nnCombi=CombiIndex.size();

    for( Int_t i=0; i<nnCombi; ++i ){
      DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
      if( !track ) continue;
      if( track->GetNHit()>=MinNumOfHits && track->DoFit()
	  && track->GetChiSquare()<MaxChisquare
	  )
	TrackCont.push_back(track);
      else
	delete track;
    }

    FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackComp(), CandCont );
    return TrackCont.size();
  }

  //______________________________________________________________________________
  Int_t
  LocalTrackSearchSsdInSsdOut( const std::vector<DCHitContainer>& SsdInHC,
			       const std::vector<DCHitContainer>& SsdOutHC,
			       std::vector<DCLocalTrack*>& TrackCont,
			       Int_t MinNumOfHits )
  {
    const Int_t npp = NumOfLayersSsdIn + NumOfLayersSsdIn;

    std::vector<ClusterList> CandCont(npp);

    for( Int_t i=0; i<NumOfLayersSsdIn; ++i ){
      MakeSsdHitCluster( SsdInHC[i+1], CandCont[i] );
    }
    for( Int_t i=0; i<NumOfLayersSsdOut; ++i ){
      MakeSsdHitCluster( SsdOutHC[i+1], CandCont[i+NumOfLayersSsdIn] );
    }

    IndexList nCombi(npp);
    for( Int_t i=0; i<npp; ++i ){
      Int_t n = CandCont[i].size();
      nCombi[i] = n>MaxNumOfCluster ? 0 : n;
    }

#if 0
    DebugPrint( nCombi, CandCont, FUNC_NAME );
#endif

    std::vector<IndexList> CombiIndex = MakeIndex( npp, nCombi );
    Int_t nnCombi=CombiIndex.size();

    for( Int_t i=0; i<nnCombi; ++i ){
      DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
      if( !track ) continue;
      if( track->GetNHit()>=MinNumOfHits && track->DoFit()
	  //&& track->GetChiSquare()<MaxChisquare
	  ){
	TrackCont.push_back(track);
      }
      else
	delete track;
    }

    FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackCompSsd(), CandCont );
    return TrackCont.size();
  }

  //______________________________________________________________________________
  Int_t
  LocalTrackSearchSsdOutSdcIn( const std::vector<DCHitContainer>& SsdInHC,
			       const std::vector<DCHitContainer>& SsdOutHC,
			       const std::vector<DCHitContainer>& SdcHC,
			       const DCPairPlaneInfo *SdcPpInfo,
			       Int_t SdcNpp,
			       std::vector<DCLocalTrack*>& TrackCont,
			       Int_t MinNumOfHits )
  {
    const Int_t npp = NumOfLayersSsdIn + NumOfLayersSsdOut + SdcNpp;
    std::vector<ClusterList> CandCont(npp);

    for( Int_t i=0; i<NumOfLayersSsdIn; ++i )
      MakeSsdHitCluster( SsdInHC[i+1], CandCont[i] );
    for( Int_t i=0; i<NumOfLayersSsdOut; ++i )
      MakeSsdHitCluster( SsdOutHC[i+1], CandCont[i+NumOfLayersSsdIn] );

    for( Int_t i=0; i<SdcNpp; ++i ){
      Bool_t ppFlag    = SdcPpInfo[i].pair;
      Bool_t honeycomb = SdcPpInfo[i].honeycomb;
      Int_t  layer1    = SdcPpInfo[i].id1;
      Int_t  layer2    = SdcPpInfo[i].id2;
      if(ppFlag)
	MakePairPlaneHitCluster( SdcHC[layer1], SdcHC[layer2],
				 SdcPpInfo[i].CellSize, CandCont[i+NumOfLayersSsdIn+NumOfLayersSsdOut], honeycomb );
      else
	MakeUnPairPlaneHitCluster( SdcHC[layer1], CandCont[i+NumOfLayersSsdIn+NumOfLayersSsdOut], honeycomb );
    }

    IndexList nCombi(npp);
    for( Int_t i=0; i<npp; ++i ){
      Int_t n = CandCont[i].size();
      nCombi[i] = n>MaxNumOfCluster ? 0 : n;
    }

#if 0
    DebugPrint( nCombi, CandCont, FUNC_NAME );
#endif

    std::vector<IndexList> CombiIndex = MakeIndex( npp, nCombi );

    Int_t nnCombi = CombiIndex.size();
    if( nnCombi==0 ){
      utility::ClearContainerAll( CandCont );
      return -1;
    }

    for( Int_t i=0; i<nnCombi; ++i ){
      DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
      if( !track ) continue;
      if( track->GetNHit()>=MinNumOfHits && track->DoFit()
	  && track->GetChiSquare()<MaxChisquare
	  ){
	TrackCont.push_back(track);
      } else {
	delete track;
      }
    }

    FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackComp(), CandCont );
    return TrackCont.size();
  }

  //______________________________________________________________________________
  inline Bool_t
  IsDeletionTarget( const std::vector< std::pair<Int_t,Int_t> >& nh,
		    Int_t NDelete, Int_t layer )
  {
    if( NDelete==0 ) return false;

    std::vector< std::pair<Int_t,Int_t> >::const_iterator itr, end;
    for( itr=nh.begin(), end=nh.end(); itr!=end; ++itr ){
      if( itr->first==0 && itr->second==layer )
	return true;
    }

    if( nh[0].second == layer )
      return true;

    return false;
  }

  //______________________________________________________________________________
//   Int_t
//   LocalTrackSearchSsdOutSdcIn( const std::vector<SsdClusterContainer>& SsdInClCont,
// 			       const std::vector<SsdClusterContainer>& SsdOutClCont,
// 			       const std::vector<DCHitContainer>& SdcHC,
// 			       const DCPairPlaneInfo *SdcPpInfo,
// 			       Int_t SdcNpp,
// 			       std::vector<DCLocalTrack*>& TrackCont,
// 			       Int_t MinNumOfHits, Bool_t DeleteFlag /* =false */ )
//   {
//     const Int_t npp = NumOfLayersSsdIn + NumOfLayersSsdOut + SdcNpp;
//     std::vector<ClusterList> CandCont(npp);

//     std::vector<IndexList> CombiIndex;
//     Int_t nnCombi = 0;

//     // first: NHits, second: LayerId
//     std::vector< std::pair<Int_t,Int_t> >  NhitsInfo(npp);

//     Int_t NDelete = 0;
//     while( nnCombi==0 ){
//       utility::ClearContainerAll( CandCont );

//       for( Int_t i=0; i<NumOfLayersSsdIn; ++i ){
// 	if( !IsDeletionTarget( NhitsInfo, NDelete, i ) ){
// 	  MakeSsdHitCluster( SsdInClCont[i+1], CandCont[i] );
// 	}
//       }

//       for( Int_t i=0; i<NumOfLayersSsdOut; ++i ){
// 	Int_t n = NumOfLayersSsdIn;
// 	if( !IsDeletionTarget( NhitsInfo, NDelete, i+n ) ){
// 	  MakeSsdHitCluster( SsdOutClCont[i+1], CandCont[i+n] );
// 	}
//       }

//       for( Int_t i=0; i<SdcNpp; ++i ){
// 	Int_t n = NumOfLayersSsdIn + NumOfLayersSsdOut;
// 	Bool_t ppFlag    = SdcPpInfo[i].pair;
// 	Bool_t honeycomb = SdcPpInfo[i].honeycomb;
// 	Int_t  layer1    = SdcPpInfo[i].id1;
// 	Int_t  layer2    = SdcPpInfo[i].id2;
// 	DCHitContainer tempHC1(0), tempHC2(0);
// 	if( ppFlag ){
// 	  if( !IsDeletionTarget( NhitsInfo, NDelete, i+n ) ){
// 	    MakePairPlaneHitCluster( SdcHC[layer1], SdcHC[layer2],
// 				     SdcPpInfo[i].CellSize, CandCont[i+n], honeycomb );
// 	  }
// 	}else{
// 	  if( !IsDeletionTarget( NhitsInfo, NDelete, i+n ) ){
// 	    MakeUnPairPlaneHitCluster( SdcHC[layer1], CandCont[i+n], honeycomb );
// 	  }
// 	}
//       }

//       IndexList nCombi(npp);
//       for( Int_t i=0; i<npp; ++i ){
// 	Int_t n = CandCont[i].size();
// 	nCombi[i] = n>MaxNumOfCluster ? 0 : n;
// 	NhitsInfo[i] = std::make_pair( nCombi[i], i );
//       }

// #if 0
//       if( DeleteFlag ){
// 	hddaq::cout << "NDelete=" << NDelete << " ===> ";
// 	for( Int_t i=0, n=NhitsInfo.size(); i<n; ++i ){
// 	  hddaq::cout << std::setw(4) << std::right << NhitsInfo[i].first << " ";
// 	}
// 	hddaq::cout <<std::endl;
//       }
// #endif

//       std::stable_sort( NhitsInfo.begin(), NhitsInfo.end(),
// 			std::greater< std::pair<Int_t,Int_t> >() );

// #if 0
//       DebugPrint( nCombi, CandCont, FUNC_NAME );
// #endif

//       CombiIndex = MakeIndex( npp, nCombi );

//       nnCombi = CombiIndex.size();

//       if( nnCombi==0 && !DeleteFlag ){
// 	  utility::ClearContainerAll( CandCont );
// 	  return -1;
//       }

//       if( !DeleteFlag ) break;
//       NDelete++;
//     }// while()

// #if 0
//     hddaq::cout << " ===> " << std::setw(7) << nnCombi
// 		<< " combinations will be checked..." << std::endl;
// #endif

//     for( Int_t i=0; i<nnCombi; ++i ){
//       DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
//       if( !track ) continue;
//       if( track->GetNHit()>=MinNumOfHits && track->DoFit()
// 	  && track->GetChiSquare()<MaxChisquare
// 	  ){
// 	TrackCont.push_back(track);
//       }
//       else {
// 	delete track;
//       }
//     }

//     FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackComp(), CandCont );
//     return TrackCont.size();
//   }

  //______________________________________________________________________________
  Int_t
  LocalTrackSearchSdcInSdcOut( const std::vector<DCHitContainer>& SdcInHC,
			       const DCPairPlaneInfo* SdcInPpInfo,
			       const std::vector<DCHitContainer>& SdcOutHC,
			       const DCPairPlaneInfo* SdcOutPpInfo,
			       Int_t SdcInNpp, Int_t SdcOutNpp,
			       std::vector<DCLocalTrack*>& TrackCont,
			       Int_t MinNumOfHits )
  {
    const Int_t npp = SdcInNpp+SdcOutNpp;

    std::vector<ClusterList> CandCont(npp);

    for( Int_t i=0; i<SdcInNpp; ++i ){
      Bool_t ppFlag = SdcInPpInfo[i].pair;
      Int_t  layer1 = SdcInPpInfo[i].id1;
      Int_t  layer2 = SdcInPpInfo[i].id2;
      if( ppFlag ){
	MakePairPlaneHitCluster( SdcInHC[layer1], SdcInHC[layer2],
				 SdcInPpInfo[i].CellSize, CandCont[i] );
      } else {
	MakeUnPairPlaneHitCluster( SdcInHC[layer1], CandCont[i] );
      }
    }

    for( Int_t i=0; i<SdcOutNpp; ++i ){
      Bool_t ppFlag = SdcOutPpInfo[i].pair;
      Int_t  layer1 = SdcOutPpInfo[i].id1;
      Int_t  layer2 = SdcOutPpInfo[i].id2;
      if( ppFlag ){
	MakePairPlaneHitCluster( SdcOutHC[layer1], SdcOutHC[layer2],
				 SdcOutPpInfo[i].CellSize, CandCont[i+SdcInNpp] );
      } else {
	MakeUnPairPlaneHitCluster( SdcOutHC[layer1], CandCont[i+SdcInNpp] );
      }
    }

    IndexList nCombi( npp );
    for( Int_t i=0; i<npp; ++i ){
	Int_t n = CandCont[i].size();
	nCombi[i] = n>MaxNumOfCluster ? 0 : n;
    }

    std::vector<IndexList> CombiIndex = MakeIndex( npp, nCombi );
    Int_t nnCombi = CombiIndex.size();

    for( Int_t i=0; i<nnCombi; ++i ){
      DCLocalTrack *track = MakeTrack( CandCont, CombiIndex[i] );
      if( !track ) continue;
      if( track->GetNHit()>=MinNumOfHits &&
	  track->DoFit() &&
	  track->GetChiSquare()<MaxChisquare )
	TrackCont.push_back( track );
      else
	delete track;
    }

    FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackComp(), CandCont );
    return TrackCont.size();
  }

  //For MWPC
  //_____________________________________________________________________________
//   Int_t MWPCLocalTrackSearch( const std::vector<DCHitContainer>& HC,
// 			    std::vector<DCLocalTrack*>& TrackCont )

//   {
//     std::vector<ClusterList> CandCont(NumOfLayersBcIn);

//     for( Int_t i=0; i<NumOfLayersBcIn; ++i ){
//       MakeMWPCPairPlaneHitCluster( HC[i], CandCont[i] );
//     }

//     //   IndexList nCombi(NumOfLayersBcIn);
//     //   for( Int_t i=0; i<NumOfLayersBcIn; ++i ){
//     //     nCombi[i]=(CandCont[i]).size();

//     //     // If #Cluster>MaxNumerOfCluster,  error return

//     //     if(nCombi[i]>MaxNumOfCluster){
//     //       hddaq::cout << "#W  too many clusters " << FUNC_NAME
//     // 		<< "  layer = " << i << " : " << nCombi[i] << std::endl;
//     //       utility::ClearContainerAll(CandCont);
//     //       return 0;
//     //     }
//     //   }

//     //   DebugPrint(nCombi, FUNC_NAME);

// #if 0
//     DebugPrint(nCombi, CandCont, FUNC_NAME);
// #endif

//     const Int_t MinNumOfHitsBcIn   = 6;
//     TrackMaker trackMaker(CandCont, MinNumOfHitsBcIn, MaxCombi, MaxChisquare);
//     trackMaker.MakeTracks(TrackCont);

//     FinalizeTrack( FUNC_NAME, TrackCont, DCLTrackComp(), CandCont );
//     return TrackCont.size();
//   }

  //______________________________________________________________________________
  // Int_t MWPCLocalTrackSearch( const std::vector< std::vector<DCHitContainer> >& hcList,
  // 			    std::vector<DCLocalTrack*>& trackCont )
  // {
  //   for ( std::vector< std::vector<DCHitContainer> >::const_iterator
  // 	    it    = hcList.begin(),
  // 	    itEnd = hcList.end(); it!=itEnd; ++it )
  //     {
  // 	const std::vector<DCHitContainer>& l = *it;
  // 	std::vector<DCLocalTrack*> tc;
  // 	MWPCLocalTrackSearch( l, tc );
  // 	trackCont.insert(trackCont.end(), tc.begin(), tc.end());
  // 	//       hddaq::cout << " tc " << tc.size()
  // 	// 		<< " : " << trackCont.size() << std::endl;
  //     }

  //   ClearFlags(trackCont);
  //   DeleteDuplicatedTracks(trackCont);
  //   CalcTracks( trackCont );
  //   return trackCont.size();
  // }
}
