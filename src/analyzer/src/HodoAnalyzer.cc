// -*- C++ -*-

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#include "BH2Cluster.hh"
#include "BH2Hit.hh"
#include "DebugCounter.hh"
#include "DeleteUtility.hh"
#include "FiberCluster.hh"
#include "FiberHit.hh"
#include "FLHit.hh"
#include "FuncName.hh"
#include "Hodo1Hit.hh"
#include "Hodo2Hit.hh"
#include "HodoAnalyzer.hh"
#include "HodoCluster.hh"
#include "RawData.hh"

ClassImp(HodoAnalyzer);

namespace
{
  const Double_t MaxTimeDifBH1 =  2.0;
  const Double_t MaxTimeDifBH2 =  2.0;
  const Double_t MaxTimeDifTOF =  3.5;
  const Double_t MaxTimeDifBFT =  8.0;
  const Double_t MaxTimeDifSCH = 10.0;
}

#define Cluster 1

//______________________________________________________________________________
HodoAnalyzer::HodoAnalyzer( void )
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
HodoAnalyzer::~HodoAnalyzer( void )
{
  ClearBH1Hits();
  ClearBH2Hits();
  ClearBACHits();
  ClearSACHits();
  ClearTOFHits();
  ClearLCHits();
  ClearBFTHits();
  ClearSCHHits();
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
void
HodoAnalyzer::ClearBH1Hits( void )
{
  utility::ClearContainer( m_BH1Cont );
  utility::ClearContainer( m_BH1ClCont );
}

//______________________________________________________________________________
void
HodoAnalyzer::ClearBH2Hits( void )
{
  utility::ClearContainer( m_BH2Cont );
  utility::ClearContainer( m_BH2ClCont );
}

//______________________________________________________________________________
void
HodoAnalyzer::ClearBACHits( void )
{
  utility::ClearContainer( m_BACCont );
}

//______________________________________________________________________________
void
HodoAnalyzer::ClearSACHits( void )
{
  utility::ClearContainer( m_SACCont );
}

//______________________________________________________________________________
void
HodoAnalyzer::ClearTOFHits( void )
{
  utility::ClearContainer( m_TOFCont );
  utility::ClearContainer( m_TOFClCont );
}

//______________________________________________________________________________
void
HodoAnalyzer::ClearLCHits( void )
{
  utility::ClearContainer( m_LCCont );
}

//______________________________________________________________________________
void
HodoAnalyzer::ClearBFTHits( void )
{
  utility::ClearContainer( m_BFTCont );
  utility::ClearContainer( m_BFTClCont );
}

//______________________________________________________________________________
void
HodoAnalyzer::ClearSCHHits( void )
{
  utility::ClearContainer( m_SCHCont );
  utility::ClearContainer( m_SCHClCont );
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::DecodeRawHits( RawData *rawData )
{
  DecodeBH1Hits( rawData );
  DecodeBH2Hits( rawData );
  DecodeBACHits( rawData );
  DecodeSACHits( rawData );
  DecodeTOFHits( rawData );
  DecodeLCHits( rawData );
  DecodeBFTHits( rawData );
  DecodeSCHHits( rawData );
  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::DecodeBH1Hits( RawData *rawData )
{
  ClearBH1Hits();
  const HodoRHitContainer &cont = rawData->GetBH1RawHC();
  for( Int_t i=0, n=cont.size(); i<n; ++i ){
    HodoRawHit *hit = cont[i];
    if( !hit ) continue;
    if( hit->GetTdcUp()<=0 || hit->GetTdcDown()<=0 ) continue;
    Hodo2Hit *hp = new Hodo2Hit( hit );
    if( !hp ) continue;
    if( hp->Calculate() )
      m_BH1Cont.push_back(hp);
    else
      delete hp;
  }//for(i)

#if Cluster
  MakeUpClusters( m_BH1Cont, m_BH1ClCont, MaxTimeDifBH1 );
#endif

  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::DecodeBH2Hits( RawData *rawData )
{
  ClearBH2Hits();
  const HodoRHitContainer &cont = rawData->GetBH2RawHC();
  for( Int_t i=0, n=cont.size(); i<n; ++i ){
    HodoRawHit *hit = cont[i];
    if( !hit ) continue;
    if( hit->GetTdcUp()<=0 || hit->GetTdcDown()<=0 ) continue;
    BH2Hit *hp = new BH2Hit( hit );
    if( !hp ) continue;
    if( hp->Calculate() )
      m_BH2Cont.push_back(hp);
    else
      delete hp;
  }//for(i)

#if Cluster
  MakeUpClusters( m_BH2Cont, m_BH2ClCont, MaxTimeDifBH2 );
#endif

  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::DecodeBACHits( RawData *rawData )
{
  ClearBACHits();
  const HodoRHitContainer &cont = rawData->GetBACRawHC();
  for( Int_t i=0, n=cont.size(); i<n; ++i ){
    HodoRawHit *hit = cont[i];
    if( !hit ) continue;
    Hodo1Hit *hp = new Hodo1Hit( hit );
    if( !hp ) continue;
    if( hp->Calculate() )
      m_BACCont.push_back(hp);
    else
      delete hp;
  }//for(i)
  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::DecodeSACHits( RawData *rawData )
{
  ClearSACHits();
  const HodoRHitContainer &cont = rawData->GetSACRawHC();
  for( Int_t i=0, n=cont.size(); i<n; ++i ){
    HodoRawHit *hit = cont[i];
    if( !hit ) continue;
    Hodo1Hit *hp = new Hodo1Hit( hit );
    if( !hp ) continue;
    if( hp->Calculate() )
      m_SACCont.push_back(hp);
    else
      delete hp;
  }//for(i)
  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::DecodeTOFHits( RawData *rawData )
{
  ClearTOFHits();
  const HodoRHitContainer &cont = rawData->GetTOFRawHC();
  for( Int_t i=0, n=cont.size(); i<n; ++i ){
    HodoRawHit *hit = cont[i];
    if( !hit ) continue;
    if( hit->GetTdcUp()<=0 || hit->GetTdcDown()<=0 ) continue;
    Hodo2Hit *hp = new Hodo2Hit( hit );
    if( !hp ) continue;
    if( hp->Calculate() )
      m_TOFCont.push_back(hp);
    else
      delete hp;
  }//for(i)

#if Cluster
  MakeUpClusters( m_TOFCont, m_TOFClCont, MaxTimeDifTOF );
#endif

  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::DecodeLCHits( RawData *rawData )
{
  ClearLCHits();
  const HodoRHitContainer &cont = rawData->GetLCRawHC();
  for( Int_t i=0, n=cont.size(); i<n; ++i ){
    HodoRawHit *hit = cont[i];
    if( !hit ) continue;
    if( hit->GetTdcUp()<=0 || hit->GetTdcDown()<=0 ) continue;
    Hodo2Hit *hp = new Hodo2Hit( hit );
    if( !hp ) continue;
    if( hp->Calculate() )
      m_TOFCont.push_back(hp);
    else
      delete hp;
  }//for(i)

  return true;
}


//______________________________________________________________________________
Bool_t
HodoAnalyzer::DecodeBFTHits( RawData* rawData )
{
  ClearBFTHits();
  for( Int_t p=0; p<NumOfPlaneBFT; ++p ){
    const HodoRHitContainer &cont = rawData->GetBFTRawHC(p);
    for( Int_t i=0, n=cont.size(); i<n; ++i ){
      HodoRawHit *hit=cont[i];
      if( !hit ) continue;
      FiberHit *hp = new FiberHit(hit, "BFT");
      if( !hp ) continue;
      if( hp->Calculate() )
	m_BFTCont.push_back(hp);
      else
	delete hp;
    }
  }

  std::sort( m_BFTCont.begin(), m_BFTCont.end(), FiberHit::CompFiberHit );

#if Cluster
  MakeUpClusters( m_BFTCont, m_BFTClCont, MaxTimeDifBFT, 3 );
#endif

  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::DecodeSCHHits( RawData* rawData )
{
  ClearSCHHits();
  const HodoRHitContainer &cont = rawData->GetSCHRawHC();
  for( Int_t i=0, n=cont.size(); i<n; ++i ){
    HodoRawHit *hit = cont[i];
    if( !hit ) continue;
    FiberHit *hp = new FiberHit( hit, "SCH" );
    if( !hp ) continue;
    if( hp->Calculate() )
      m_SCHCont.push_back( hp );
    else
      delete hp;
  }

  std::sort( m_SCHCont.begin(), m_SCHCont.end(), FiberHit::CompFiberHit );

#if Cluster
  MakeUpClusters( m_SCHCont, m_SCHClCont, MaxTimeDifSCH, 1 );
#endif

  return true;
}

//______________________________________________________________________________
Int_t
HodoAnalyzer::MakeUpClusters( const Hodo2HitContainer& HitCont,
			      HodoClusterContainer& ClusterCont,
			      Double_t maxTimeDif )
{
  utility::ClearContainer( ClusterCont );

  Int_t n = HitCont.size();
  std::vector<Int_t> flag( n, 0 );
  for( Int_t i=0; i<n; ++i ){
    if( flag[i] ) continue;
    Hodo2Hit *hitA = HitCont[i];
    Int_t     segA = hitA->SegmentId();
    Double_t  cmtA = hitA->CMeanTime();
    Hodo2Hit *hitB = 0;
    Int_t       iB = -1;
    Double_t  cmtB;
    Int_t     segB;
    for( Int_t j=i+1; j<n; ++j ){
      Hodo2Hit *hit = HitCont[j];
      Int_t     seg = hit->SegmentId();
      Double_t  cmt = hit->CMeanTime();
      if( TMath::Abs(seg-segA)==1 && TMath::Abs(cmt-cmtA)<maxTimeDif ){
	hitB = hit; ++flag[j]; iB = j; segB = seg; cmtB = cmt; break;
      }
    }
    if( hitB ){
      Hodo2Hit *hitC = 0;
      for( Int_t j=i+1; j<n; ++j ){
	if( j==iB ) continue;
	Hodo2Hit *hit = HitCont[j];
	Int_t    seg = hit->SegmentId();
	Double_t cmt = hit->CMeanTime();
	if( (TMath::Abs(seg-segA)==1 && TMath::Abs(cmt-cmtA)<maxTimeDif) ||
	    (TMath::Abs(seg-segB)==1 && TMath::Abs(cmt-cmtB)<maxTimeDif) ){
	  hitC=hit; ++flag[j]; break;
	}
      }
      if( hitC ){
	HodoCluster *cluster = new HodoCluster( hitA, hitB, hitC );
	if( cluster ) ClusterCont.push_back( cluster );
      }
      else{
	HodoCluster *cluster = new HodoCluster( hitA, hitB );
	if( cluster ) ClusterCont.push_back( cluster );
      }
    }
    else{
      HodoCluster *cluster = new HodoCluster( hitA );
      if( cluster ) ClusterCont.push_back( cluster );
    }
  }

  return ClusterCont.size();
}

//______________________________________________________________________________
Int_t
HodoAnalyzer::MakeUpClusters( const BH2HitContainer& HitCont,
			      BH2ClusterContainer& ClusterCont,
			      Double_t maxTimeDif )
{
  utility::ClearContainer( ClusterCont );

  Int_t nh = HitCont.size();

  std::vector <Int_t> flag(nh,0);

  for(Int_t i=0; i<nh; ++i ){
    if( flag[i] ) continue;
    BH2Hit *hitA = HitCont[i];
    Int_t     segA = hitA->SegmentId();
    Double_t  cmtA = hitA->CMeanTime();
    BH2Hit *hitB = 0;
    Int_t       iB = -1;
    Double_t  cmtB;
    Int_t     segB;
    for( Int_t j=i+1; j<nh; ++j ){
      BH2Hit *hit = HitCont[j];
      Int_t     seg = hit->SegmentId();
      Double_t  cmt = hit->CMeanTime();
      if( TMath::Abs(seg-segA)==1 && TMath::Abs(cmt-cmtA)<maxTimeDif ){
	hitB = hit; ++flag[j]; iB = j; segB = seg; cmtB = cmt; break;
      }
    }
    if(hitB){
      BH2Hit *hitC = 0;
      for( Int_t j=i+1; j<nh; ++j ){
        if( j==iB ) continue;
        BH2Hit *hit = HitCont[j];
        Int_t     seg = hit->SegmentId();
        Double_t  cmt = hit->CMeanTime();
        if( (TMath::Abs(seg-segA)==1 && TMath::Abs(cmt-cmtA)<maxTimeDif) ||
            (TMath::Abs(seg-segB)==1 && TMath::Abs(cmt-cmtB)<maxTimeDif) ){
          hitC = hit; ++flag[j]; break;
        }
      }
      if( hitC ){
        BH2Cluster *cluster = new BH2Cluster( hitA, hitB, hitC );
        if( cluster ) ClusterCont.push_back( cluster );
      }
      else{
	BH2Cluster *cluster = new BH2Cluster( hitA, hitB );
	if( cluster ) ClusterCont.push_back( cluster );
      }
    }
    else{
      BH2Cluster *cluster = new BH2Cluster( hitA );
      if( cluster ) ClusterCont.push_back( cluster );
    }
  }

  return ClusterCont.size();
}

//______________________________________________________________________________
Int_t
HodoAnalyzer::MakeUpClusters( const FiberHitContainer& cont,
			      FiberClusterContainer& ClusterCont,
			      Double_t maxTimeDif,
			      Int_t DifPairId )
{
  utility::ClearContainer( ClusterCont );

  Int_t NofSeg = cont.size();
  for( Int_t seg=0; seg<NofSeg; ++seg ){
    FiberHit* HitA = cont.at(seg);
    Bool_t fl_ClCandA = false;
    if( seg != (NofSeg -1) ){
      if( DifPairId > (cont.at(seg+1)->PairId() - HitA->PairId())){
	fl_ClCandA = true;
      }
    }

    Int_t NofHitA = HitA->GetNumOfHit();
    for(Int_t mhitA = 0; mhitA<NofHitA; ++mhitA){
      if(HitA->Joined(mhitA)) continue;

      FiberCluster *cluster = new FiberCluster();
      cluster->push_back( new FLHit(HitA, mhitA) );

      if(!fl_ClCandA){
	// there is no more candidates
	if(cluster->Calculate()) ClusterCont.push_back(cluster);
	continue;
      }

      // Start Search HitB
      Double_t cmtA    = (Double_t)HitA->GetCTime(mhitA);
      Int_t    NofHitB = cont.at(seg+1)->GetNumOfHit();
      Bool_t   fl_HitB = false;
      Double_t cmtB    = -1;
      Int_t    CurrentPair = HitA->PairId();
      for(Int_t mhitB = 0; mhitB<NofHitB; ++mhitB){
	if(cont.at(seg+1)->Joined(mhitB)){continue;}

	FiberHit* HitB = cont.at(seg+1);

	cmtB = (Double_t)HitB->GetCTime(mhitB);
	if(TMath::Abs(cmtB-cmtA)<maxTimeDif){
	  cluster->push_back(new FLHit(HitB, mhitB));
	  CurrentPair = HitB->PairId();
	  fl_HitB = true;
	  break;
	}
      }

      Bool_t fl_ClCandB  = false;
      if((seg+1) != (NofSeg -1)){
	if( DifPairId > (cont.at(seg+2)->PairId() - CurrentPair) ){
	  fl_ClCandB = true;
	}
      }

      if(!fl_ClCandB){
	// there is no more candidates
	if(cluster->Calculate()) ClusterCont.push_back(cluster);
	continue;
      }

      // Start Search HitC
      Int_t    NofHitC = cont.at(seg+2)->GetNumOfHit();
      Bool_t   fl_HitC = false;
      Double_t cmtC    = -1;
      for(Int_t mhitC = 0; mhitC<NofHitC; ++mhitC){
	if(cont.at(seg+2)->Joined(mhitC)){continue;}

	FiberHit* HitC = cont.at(seg+2);

	cmtC = (Double_t)HitC->GetCTime(mhitC);
	if(true
	   && TMath::Abs(cmtC-cmtA)<maxTimeDif
	   && !(fl_HitB && (TMath::Abs(cmtC-cmtB)>maxTimeDif))
	   ){
	  cluster->push_back(new FLHit(HitC, mhitC));
	  CurrentPair = HitC->PairId();
	  fl_HitC = true;
	  break;
	}
      }

      Bool_t fl_ClCandC  = false;
      if((seg+2) != (NofSeg -1)){
	if( DifPairId > (cont.at(seg+3)->PairId() - CurrentPair) ){
	  fl_ClCandC = true;
	}
      }

      if(!fl_ClCandC){
	// there is no more candidates
	if(cluster->Calculate()) ClusterCont.push_back(cluster);
	continue;
      }

      // Start Search HitD
      Int_t    NofHitD = cont.at(seg+3)->GetNumOfHit();
      Double_t cmtD    = -1;
      for(Int_t mhitD = 0; mhitD<NofHitD; ++mhitD){
	if(cont.at(seg+3)->Joined(mhitD)){continue;}

	FiberHit* HitD = cont.at(seg+3);

	cmtD = (Double_t)HitD->GetCTime(mhitD);
	if(true
	   && TMath::Abs(cmtD-cmtA)<maxTimeDif
	   && !(fl_HitB && (TMath::Abs(cmtD-cmtB)>maxTimeDif))
	   && !(fl_HitC && (TMath::Abs(cmtD-cmtC)>maxTimeDif))
	   ){
	  cluster->push_back(new FLHit(HitD, mhitD));
	  break;
	}
      }

      // Finish
      if(cluster->Calculate()) ClusterCont.push_back(cluster);
    }
  }

  return ClusterCont.size();
}

//______________________________________________________________________________
Int_t
HodoAnalyzer::MakeUpClusters( const FLHitContainer& cont,
			      FiberClusterContainer& ClusterCont,
			      Double_t maxTimeDif,
			      Int_t DifPairId)
{
  // Clustering function for FBH

  std::vector<FiberCluster*> DeleteCand;

  Int_t NofSeg = cont.size();
  for(Int_t seg = 0; seg<NofSeg; ++seg){
    FLHit* HitA = cont.at(seg);

    if(HitA->Joined()){continue;}

    HitA->SetJoined();
    FiberCluster *cluster = new FiberCluster();
    cluster->push_back(HitA);

    Int_t CurrentPair = HitA->PairId();
    for(Int_t smarker = seg+1; smarker<NofSeg; ++smarker){
      FLHit* HitB = cont.at(smarker);

      if(false
	 || DifPairId < (HitB->PairId() - CurrentPair)
	 || HitB->PairId() == CurrentPair
	 ) {continue;}
      if(HitB->Joined()) continue;
      HitB->SetJoined();

      Double_t cmtB = HitB->GetCTime();
      Bool_t   fl_all_valid = true;
      for(Int_t cindex = 0; cindex<cluster->VectorSize(); ++cindex){
	Double_t cmt = cluster->GetHit(cindex)->GetCTime();
	if(TMath::Abs(cmt-cmtB) > maxTimeDif){fl_all_valid = false; break;}
      }

      if(fl_all_valid){
	// we found a cluster candidate
	cluster->push_back(HitB);
	CurrentPair = HitB->PairId();
	break;
      }
    }

    // Finish
    if(cluster->Calculate()
       ){
      ClusterCont.push_back(cluster);
    }else{
      DeleteCand.push_back(cluster);

    }
  }

  utility::ClearContainer( DeleteCand );

  return ClusterCont.size();
}

// MakeUpCoincidence ------------------------------------------------------
Int_t
HodoAnalyzer::MakeUpCoincidence( const FiberHitContainer& cont,
				 FLHitContainer& CoinCont,
				 Double_t maxTimeDif )
{
  Int_t NofSeg = cont.size();
  for(Int_t seg = 0; seg<NofSeg; ++seg){
    FiberHit* HitA = cont.at(seg);

    Int_t NofHitA = HitA->GetNumOfHit();
    for( Int_t mhitA = 0; mhitA<NofHitA; ++mhitA ){
      if( HitA->Joined(mhitA) ) continue;
      Double_t cmt = HitA->GetCTime(mhitA);

      Int_t CurrentPair = HitA->PairId();
      for( Int_t smarker = seg+1; smarker<NofSeg; ++smarker ){
	FiberHit* HitB = cont.at(smarker);

	if( 0 != (HitB->PairId() - CurrentPair) ) continue;

	Int_t NofHitB = HitB->GetNumOfHit();
	for( Int_t mhitB = 0; mhitB<NofHitB; ++mhitB ){
	  if( HitB->Joined(mhitB) ) continue;

	  Double_t cmtB = HitB->GetCTime(mhitB);
	  Bool_t   fl_all_valid = true;
	  if( TMath::Abs(cmt-cmtB)>maxTimeDif ){
	    fl_all_valid = false;
	    break;
	  }
	  if( fl_all_valid ){
	    // we found a coin candidate
	    //	    hddaq::cout << "yes" << std::endl;
	    CoinCont.push_back(new FLHit(HitA, HitB, mhitA, mhitB));

	    break;
	  }
	}// for(mhitB)
      }// for(segB)

    }// for(mhitA)
  }// for(segA)

  return CoinCont.size();
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::ReCalcBH1Hits( Bool_t applyRecursively )
{
  Int_t n = m_BH1Cont.size();
  for( Int_t i=0; i<n; ++i ){
    Hodo2Hit *hit = m_BH1Cont[i];
    if(hit) hit->ReCalc(applyRecursively);
  }
  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::ReCalcBH2Hits( Bool_t applyRecursively )
{
  Int_t n = m_BH2Cont.size();
  for( Int_t i=0; i<n; ++i ){
    BH2Hit *hit = m_BH2Cont[i];
    if(hit) hit->ReCalc(applyRecursively);
  }
  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::ReCalcBACHits( Bool_t applyRecursively )
{
  Int_t n = m_BACCont.size();
  for( Int_t i=0; i<n; ++i ){
    Hodo1Hit *hit = m_BACCont[i];
    if(hit) hit->ReCalc(applyRecursively);
  }
  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::ReCalcSACHits( Bool_t applyRecursively )
{
  Int_t n = m_SACCont.size();
  for( Int_t i=0; i<n; ++i ){
    Hodo1Hit *hit = m_SACCont[i];
    if(hit) hit->ReCalc(applyRecursively);
  }
  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::ReCalcTOFHits( Bool_t applyRecursively )
{
  Int_t n = m_TOFCont.size();
  for( Int_t i=0; i<n; ++i ){
    Hodo2Hit *hit = m_TOFCont[i];
    if(hit) hit->ReCalc(applyRecursively);
  }
  return true;

}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::ReCalcLCHits( Bool_t applyRecursively )
{
  Int_t n = m_LCCont.size();
  for( Int_t i=0; i<n; ++i ){
    Hodo2Hit *hit = m_LCCont[i];
    if(hit) hit->ReCalc(applyRecursively);
  }
  return true;

}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::ReCalcBH1Clusters( Bool_t applyRecursively )
{
  Int_t n = m_BH1ClCont.size();
  for( Int_t i=0; i<n; ++i ){
    HodoCluster *cl = m_BH1ClCont[i];
    if(cl) cl->ReCalc(applyRecursively);
  }
  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::ReCalcBH2Clusters( Bool_t applyRecursively )
{
  Int_t n = m_BH2ClCont.size();
  for( Int_t i=0; i<n; ++i ){
    BH2Cluster *cl = m_BH2ClCont[i];
    if(cl) cl->ReCalc(applyRecursively);
  }
  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::ReCalcTOFClusters( Bool_t applyRecursively )
{
  Int_t n = m_TOFClCont.size();
  for( Int_t i=0; i<n; ++i ){
    HodoCluster *cl = m_TOFClCont[i];
    if(cl) cl->ReCalc(applyRecursively);
  }
  return true;
}

//______________________________________________________________________________
Bool_t
HodoAnalyzer::ReCalcAll( void )
{
  ReCalcBH1Hits();
  ReCalcBH2Hits();
  ReCalcBACHits();
  ReCalcSACHits();
  ReCalcTOFHits();
  ReCalcLCHits();
  ReCalcBH1Clusters();
  ReCalcBH2Clusters();
  ReCalcTOFClusters();
  return true;
}

//______________________________________________________________________________
void
HodoAnalyzer::TimeCutBH1( Double_t tmin, Double_t tmax )
{
  TimeCut( m_BH1ClCont, tmin, tmax );
}

//______________________________________________________________________________
void
HodoAnalyzer::TimeCutBH2( Double_t tmin, Double_t tmax )
{
  TimeCut( m_BH2ClCont, tmin, tmax );
}

//______________________________________________________________________________
void
HodoAnalyzer::TimeCutBFT( Double_t tmin, Double_t tmax )
{
  TimeCut( m_BFTClCont, tmin, tmax );
}

//______________________________________________________________________________
void
HodoAnalyzer::TimeCutSCH( Double_t tmin, Double_t tmax )
{
  TimeCut( m_SCHClCont, tmin, tmax );
}

//______________________________________________________________________________
//Implementation of Time cut for the cluster container
template <typename TypeCluster>
void
HodoAnalyzer::TimeCut( std::vector<TypeCluster>& cont,
		       Double_t tmin, Double_t tmax )
{
  std::vector<TypeCluster> DeleteCand;
  std::vector<TypeCluster> ValidCand;
  for( Int_t i=0, n=cont.size(); i<n; ++i ){
    Double_t ctime = cont.at(i)->CMeanTime();
    if( tmin < ctime && ctime < tmax ){
      ValidCand.push_back( cont.at(i) );
    }else{
      DeleteCand.push_back( cont.at(i) );
    }
  }

  utility::ClearContainer( DeleteCand );

  cont.clear();
  cont.resize( ValidCand.size() );
  std::copy( ValidCand.begin(), ValidCand.end(), cont.begin() );
  ValidCand.clear();
}
