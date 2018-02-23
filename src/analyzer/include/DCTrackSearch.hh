// -*- C++ -*-

#ifndef DC_TRACK_SEARCH_HH
#define DC_TRACK_SEARCH_HH

#include "DCAnalyzer.hh"

#include <vector>

struct DCPairPlaneInfo;
class  DCPairHitCluster;
class  DCLocalTrack;
class  DCLTrackHit;
class  MWPCCluster;

typedef std::vector<DCPairHitCluster*> ClusterList;
typedef std::vector<Int_t>             IndexList;

namespace track
{

//______________________________________________________________________________
inline TString&
ClassName( void )
{
  static TString g_name("DCTrackSearch");
  return g_name;
}

//______________________________________________________________________________
// MakeCluster _________________________________________________________________
Bool_t MakePairPlaneHitCluster( const DCHitContainer & HC1,
				const DCHitContainer & HC2,
				Double_t CellSize,
				ClusterList& Cont,
				Bool_t honeycomb=false );
Bool_t MakeUnPairPlaneHitCluster( const DCHitContainer& HC,
				  ClusterList& Cont,
				  Bool_t honeycomb=false );
Bool_t MakeMWPCPairPlaneHitCluster( const DCHitContainer& HC, ClusterList& Cont );
Bool_t MakeSsdHitCluster( const DCHitContainer& HC, ClusterList& Cont );
Bool_t MakeTOFHitCluster( const DCHitContainer& HitCont, ClusterList& Cont, Int_t xy );

//______________________________________________________________________________
// MakeIndex ___________________________________________________________________
std::vector<IndexList> MakeIndex( Int_t ndim, const Int_t *index1 );
std::vector<IndexList> MakeIndex( Int_t ndim, const IndexList& index1 );
std::vector<IndexList> MakeIndex_VXU( Int_t ndim,Int_t maximumHit, const Int_t *index1 );
std::vector<IndexList> MakeIndex_VXU( Int_t ndim,Int_t maximumHit, const IndexList& index1 );

//______________________________________________________________________________
// MakeTrack ___________________________________________________________________
DCLocalTrack* MakeTrack( const std::vector<ClusterList>& CandCont,
			 const IndexList& combination );

//______________________________________________________________________________
Int_t LocalTrackSearch( const std::vector<DCHitContainer>& HC,
			const DCPairPlaneInfo *PpInfo,
			Int_t npp, std::vector<DCLocalTrack*>& TrackCont,
			Int_t MinNumOfHits=6,
			Bool_t delete_flag=true );

//______________________________________________________________________________
Int_t LocalTrackSearch( const std::vector< std::vector<DCHitContainer> >& hcAssemble,
			const DCPairPlaneInfo *PpInfo,
			Int_t npp, std::vector<DCLocalTrack*>& TrackCont,
			Int_t MinNumOfHits=6 );

//______________________________________________________________________________
Int_t LocalTrackSearchSdcInFiber( const std::vector<DCHitContainer>& HC,
				  const DCPairPlaneInfo *PpInfo,
				  Int_t npp, std::vector<DCLocalTrack*>& trackCont,
				  Int_t MinNumOfHits=6 );

//______________________________________________________________________________
Int_t LocalTrackSearchVUX( const std::vector<DCHitContainer>& HC,
			   const DCPairPlaneInfo *PpInfo,
			   Int_t npp, std::vector<DCLocalTrack*>& TrackCont,
			   Int_t MinNumOfHits=6 );

//______________________________________________________________________________
Int_t LocalTrackSearchSdcOut( const std::vector<DCHitContainer>& SdcOutHC,
			      const DCPairPlaneInfo *PpInfo, Int_t npp,
			      std::vector<DCLocalTrack*>& TrackCont,
			      Int_t MinNumOfHits=6 );

//______________________________________________________________________________
Int_t LocalTrackSearchSdcOut( const DCHitContainer& TOFHC,
			      const std::vector<DCHitContainer>& SdcOutHC,
			      const DCPairPlaneInfo *PpInfo,
			      Int_t npp,
			      std::vector<DCLocalTrack*>& TrackCont,
			      Int_t MinNumOfHits=6 );

//______________________________________________________________________________
Int_t LocalTrackSearchSsdIn( const std::vector<DCHitContainer>& HC,
			     std::vector<DCLocalTrack*>& TrackCont,
			     Int_t MinNumOfHits=4,
			     Bool_t delete_flag=false );

//______________________________________________________________________________
// Int_t LocalTrackSearchSsdIn( const std::vector<SsdClusterContainer>& SsdInClCont,
// 			     std::vector<DCLocalTrack*>& TrackCont,
// 			     Int_t MinNumOfHits=4,
// 			     Bool_t delete_flag=false );

//______________________________________________________________________________
// Int_t LocalTrackSearchSsdOut( const std::vector<DCHitContainer>& HC,
// 			      std::vector<DCLocalTrack*>& TrackCont,
// 			      Int_t MinNumOfHits=4 );

//______________________________________________________________________________
// Int_t LocalTrackSearchSsdOut( const std::vector<SsdClusterContainer>& SsdOutClCont,
// 			      std::vector<DCLocalTrack*>& TrackCont,
// 			      Int_t MinNumOfHits=4 );

//______________________________________________________________________________
// Int_t LocalTrackSearchSsdInXY( const std::vector<DCHitContainer>& HC,
// 			       std::vector<DCLocalTrack*>& TrackContX,
// 			       std::vector<DCLocalTrack*>& TrackContY );

//______________________________________________________________________________
// Int_t LocalTrackSearchSsdInXY( const std::vector<SsdClusterContainer>& SsdInClCont,
// 			       std::vector<DCLocalTrack*>& TrackContX,
// 			       std::vector<DCLocalTrack*>& TrackContY );

//______________________________________________________________________________
// Int_t PreTrackSearchSsdXY( const std::vector<SsdClusterContainer>& SsdInClCont,
// 			   const std::vector<SsdClusterContainer>& SsdOutClCont,
// 			   std::vector<DCLocalTrack*>& TrackContX,
// 			   std::vector<DCLocalTrack*>& TrackContY );

//______________________________________________________________________________
Int_t LocalTrackSearchBcOutSdcIn( const std::vector<DCHitContainer>& BcHC,
				  const DCPairPlaneInfo *BcPpInfo,
				  const std::vector<DCHitContainer>& SdcHC,
				  const DCPairPlaneInfo *SdcPpInfo,
				  Int_t BcNpp, Int_t SdcNpp,
				  std::vector<DCLocalTrack*>& TrackCont,
				  Int_t MinNumOfHits=18 );

//______________________________________________________________________________
Int_t LocalTrackSearchBcOutSsdIn( const std::vector<DCHitContainer>& BcHC,
				  const DCPairPlaneInfo *BcPpInfo,
				  Int_t BcNpp,
				  const std::vector<DCHitContainer>& SsdHC,
				  std::vector<DCLocalTrack*>& TrackCont,
				  Int_t MinNumOfHits=14 );

//______________________________________________________________________________
Int_t LocalTrackSearchSsdInSsdOut( const std::vector<DCHitContainer>& SsdInHC,
				   const std::vector<DCHitContainer>& SsdOutHC,
				   std::vector<DCLocalTrack*>& TrackCont,
				   Int_t MinNumOfHits=6 );

//______________________________________________________________________________
Int_t LocalTrackSearchSsdOutSdcIn( const std::vector<DCHitContainer>& SsdInHC,
				   const std::vector<DCHitContainer>& SsdOutHC,
				   const std::vector<DCHitContainer>& SdcHC,
				   const DCPairPlaneInfo *SdcPpInfo,
				   Int_t SdcNpp,
				   std::vector<DCLocalTrack*>& TrackCont,
				   Int_t MinNumOfHits=10 );

//______________________________________________________________________________
// Int_t LocalTrackSearchSsdOutSdcIn( const std::vector<SsdClusterContainer>& SsdInClCont,
// 				   const std::vector<SsdClusterContainer>& SsdOutClCont,
// 				   const std::vector<DCHitContainer>& SdcHC,
// 				   const DCPairPlaneInfo *SdcPpInfo,
// 				   Int_t SdcNpp,
// 				   std::vector<DCLocalTrack*>& TrackCont,
// 				   Int_t MinNumOfHits=10,
// 				   // Delete layers having many hits
// 				   Bool_t DeleteFlag=false );

//______________________________________________________________________________
// w/SsdPreTracking
Int_t LocalTrackSearchSsdOutSdcIn( const std::vector<DCHitContainer>& SdcInHC,
				   const DCPairPlaneInfo *PpInfo,
				   Int_t SdcInNpp,
				   std::vector<DCLocalTrack*>& SsdXTC,
				   std::vector<DCLocalTrack*>& SsdYTC,
				   std::vector<DCLocalTrack*>& TrackCont,
				   Int_t MinNumOfHits=10 );

//______________________________________________________________________________
Int_t LocalTrackSearchSdcInSdcOut( const std::vector<DCHitContainer>& SdcInHC,
				   const DCPairPlaneInfo *SdcInPpInfo,
				   const std::vector<DCHitContainer>& SdcOutHC,
				   const DCPairPlaneInfo *SdcOutPpInfo,
				   Int_t SdcInNpp, Int_t SdcOutNpp,
				   std::vector<DCLocalTrack*>& TrackCont,
				   Int_t MinNumOfHits=12 );
//______________________________________________________________________________
// Int_t MWPCLocalTrackSearch( const std::vector<DCHitContainer>& HC,
// 			    std::vector<DCLocalTrack*>& TrackCont );

//______________________________________________________________________________
// Int_t MWPCLocalTrackSearch( const std::vector< std::vector<DCHitContainer> >& hcList,
// 			    std::vector<DCLocalTrack*>& trackCont );

}

#endif
