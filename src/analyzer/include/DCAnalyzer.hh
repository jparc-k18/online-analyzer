// -*- C++ -*-

#ifndef DC_ANALYZER_HH
#define DC_ANALYZER_HH

#include <vector>

#include <TObject.h>
#include <TString.h>

#include <std_ostream.hh>

#include "DetectorID.hh"
#include "ThreeVector.hh"

class DCHit;
class DCLocalTrack;
class K18TrackD2U;
class KuramaTrack;
class RawData;
class MWPCCluster;
class FiberCluster;
class SsdCluster;
class HodoCluster;

class Hodo1Hit;
class Hodo2Hit;
class HodoAnalyzer;

typedef std::vector<DCHit*>        DCHitContainer;
typedef std::vector<MWPCCluster*>  MWPCClusterContainer;
typedef std::vector<SsdCluster*>   SsdClusterContainer;
typedef std::vector<DCLocalTrack*> DCLocalTrackContainer;
typedef std::vector<K18TrackD2U*>  K18TrackD2UContainer;
typedef std::vector<KuramaTrack*>  KuramaTrackContainer;

typedef std::vector<Hodo1Hit*> Hodo1HitContainer;
typedef std::vector<Hodo2Hit*> Hodo2HitContainer;
typedef std::vector<HodoCluster*> HodoClusterContainer;

//______________________________________________________________________________
class DCAnalyzer : public TObject
{
public:
  DCAnalyzer( void );
  ~DCAnalyzer( void );

private:
  DCAnalyzer( const DCAnalyzer& );
  DCAnalyzer& operator =( const DCAnalyzer& );

private:
  enum e_type {
    k_BcIn,
    k_BcOut,
    k_SdcIn,
    k_SdcOut,
    k_SsdIn,
    k_SsdOut,
    k_TOF,
    n_type
  };
  std::vector<Bool_t>               m_is_decoded;
  std::vector<Int_t>                m_much_combi;
  std::vector<MWPCClusterContainer> m_MWPCClCont;
  std::vector<DCHitContainer>       m_TempBcInHC;
  std::vector<DCHitContainer>       m_BcInHC;
  std::vector<DCHitContainer>       m_BcOutHC;
  std::vector<DCHitContainer>       m_SdcInHC;
  std::vector<DCHitContainer>       m_SdcOutHC;
  std::vector<DCHitContainer>       m_SsdInHC;
  std::vector<DCHitContainer>       m_SsdOutHC;
  DCHitContainer                    m_TOFHC;
  std::vector<SsdClusterContainer>  m_SsdInClCont;
  std::vector<SsdClusterContainer>  m_SsdOutClCont;
  DCHitContainer                    m_VtxPoint;
  DCLocalTrackContainer             m_BcInTC;
  DCLocalTrackContainer             m_BcOutTC;
  DCLocalTrackContainer             m_SdcInTC;
  DCLocalTrackContainer             m_SdcOutTC;
  DCLocalTrackContainer             m_SsdInTC;
  DCLocalTrackContainer             m_SsdOutTC;
  DCLocalTrackContainer             m_SsdXTC; // for PreTracking
  DCLocalTrackContainer             m_SsdYTC; // for PreTracking
  DCLocalTrackContainer             m_SsdInXTC; // for XiTracking
  DCLocalTrackContainer             m_SsdInYTC; // for XiTracking
  K18TrackD2UContainer              m_K18D2UTC;
  KuramaTrackContainer              m_KuramaTC;
  DCLocalTrackContainer             m_BcOutSdcInTC;
  DCLocalTrackContainer             m_BcOutSsdInTC;
  DCLocalTrackContainer             m_SsdOutSdcInTC;
  DCLocalTrackContainer             m_SdcInSdcOutTC;
  // Exclusive Tracks
  std::vector<DCLocalTrackContainer> m_SdcInExTC;
  std::vector<DCLocalTrackContainer> m_SdcOutExTC;
public:
  Int_t  MuchCombinationSdcIn( void ) const { return m_much_combi[k_SdcIn]; }
  Bool_t DecodeRawHits( RawData* rawData );
  // Bool_t DecodeFiberHits( FiberCluster* FiberCl, Int_t layer );
  Bool_t DecodeBcInHits( RawData* rawData );
  Bool_t DecodeBcOutHits( RawData* rawData );
  Bool_t DecodeSdcInHits( RawData* rawData );
  Bool_t DecodeSdcOutHits( RawData* rawData );
  Bool_t DecodeSsdInHits( RawData* rawData );
  Bool_t DecodeSsdOutHits( RawData* rawData );
  Bool_t DecodeTOFHits( const Hodo2HitContainer& HitCont );
  Bool_t DecodeTOFHits( const HodoClusterContainer& ClCont );
  //Bool_t DecodeSimuHits( SimuData *simuData );
  Int_t  ClusterizeMWPCHit( const DCHitContainer& hits,
                            MWPCClusterContainer& clusters );
  Bool_t ClusterizeSsd( void );
  Bool_t ClusterizeSsd( const DCHitContainer& HitCont,
                        SsdClusterContainer& ClCont,
                        const Double_t MaxTimeDiff=25. );

  const DCHitContainer& GetTempBcInHC( Int_t layer ) const { return m_TempBcInHC[layer]; }
  const DCHitContainer& GetBcInHC( Int_t layer ) const { return m_BcInHC[layer]; }
  const DCHitContainer& GetBcOutHC( Int_t layer ) const { return m_BcOutHC[layer]; }
  const DCHitContainer& GetSdcInHC( Int_t layer ) const { return m_SdcInHC[layer]; }
  const DCHitContainer& GetSdcOutHC( Int_t layer ) const { return m_SdcOutHC[layer]; }
  const DCHitContainer& GetSsdInHC( Int_t layer ) const { return m_SsdInHC[layer]; }
  const DCHitContainer& GetSsdOutHC( Int_t layer ) const { return m_SsdOutHC[layer]; }
  const DCHitContainer& GetTOFHC( void ) const { return m_TOFHC; }

  Bool_t TrackSearchBcIn( void );
  Bool_t TrackSearchBcIn( const std::vector< std::vector<DCHitContainer> >& hc );
  Bool_t TrackSearchBcOut( void );
  Bool_t TrackSearchBcOut( const std::vector< std::vector<DCHitContainer> >& hc );
  Bool_t TrackSearchSdcIn( void );
  Bool_t TrackSearchSdcInFiber( void );
  Bool_t TrackSearchSdcOut( void );
  Bool_t TrackSearchSdcOut( const Hodo2HitContainer& HitCont );
  Bool_t TrackSearchSdcOut( const HodoClusterContainer& ClCont );
  Bool_t TrackSearchSsdIn( Bool_t delete_flag );
  Bool_t TrackSearchSsdOut( void );
  Bool_t TrackSearchSsdInXY( void );
  Int_t GetNtracksBcIn( void )   const { return m_BcInTC.size(); }
  Int_t GetNtracksBcOut( void )  const { return m_BcOutTC.size(); }
  // Int_t GetNtracksSdcIn( void )  const { return m_SdcInTC.size(); }
  Int_t GetNtracksSdcIn( void )  const { return m_SsdOutSdcInTC.size(); }
  Int_t GetNtracksSdcOut( void ) const { return m_SdcOutTC.size(); }
  Int_t GetNtracksSsdIn( void )  const { return m_SsdInTC.size(); }
  Int_t GetNtracksSsdOut( void ) const { return m_SsdOutTC.size(); }
  Int_t GetNtracksSsdInX( void ) const { return m_SsdInXTC.size(); }
  Int_t GetNtracksSsdInY( void ) const { return m_SsdInYTC.size(); }
  // for PreTracking
  Int_t GetNtracksSsdX( void ) const { return m_SsdXTC.size(); }
  Int_t GetNtracksSsdY( void ) const { return m_SsdYTC.size(); }
  // Exclusive Tracks
  Int_t GetNtracksSdcInEx( Int_t layer ) const { return m_SdcInExTC[layer].size(); }
  Int_t GetNtracksSdcOutEx( Int_t layer ) const { return m_SdcOutExTC[layer].size(); }

  DCLocalTrack* GetTrackBcIn( Int_t i ) const { return m_BcInTC[i]; }
  DCLocalTrack* GetTrackBcOut( Int_t i ) const { return m_BcOutTC[i]; }
  DCLocalTrack* GetTrackSdcIn( Int_t i ) const { return m_SsdOutSdcInTC[i]; }
  DCLocalTrack* GetTrackSdcOut( Int_t i ) const { return m_SdcOutTC[i]; }
  DCLocalTrack* GetTrackSsdIn( Int_t i ) const { return m_SsdInTC[i]; }
  DCLocalTrack* GetTrackSsdOut( Int_t i ) const { return m_SsdOutTC[i]; }
  DCLocalTrack* GetTrackSsdInX( Int_t i ) const { return m_SsdInXTC[i]; }
  DCLocalTrack* GetTrackSsdInY( Int_t i ) const { return m_SsdInYTC[i]; }
  // for PreTracking
  DCLocalTrack* GetTrackSsdX( Int_t i ) const { return m_SsdXTC[i]; }
  DCLocalTrack* GetTrackSsdY( Int_t i ) const { return m_SsdYTC[i]; }
  // Exclusive Tracks
  DCLocalTrack* GetTrackSdcInEx( Int_t layer, Int_t i ) const { return m_SdcInExTC[layer][i]; }
  DCLocalTrack* GetTrackSdcOutEx( Int_t layer, Int_t i ) const { return m_SdcOutExTC[layer][i]; }

  Bool_t TrackSearchK18D2U( const std::vector<Double_t>& XinCont );
  Bool_t TrackSearchKurama( Double_t initial_momentum );
  Bool_t TrackSearchKurama( void );

  ////////// Filters for SSD
  void DoTimeCorrectionSsd( const std::vector<Double_t>& t0 );
  void DoTimeCorrectionSsdIn( const std::vector<Double_t>& t0 );
  void DoTimeCorrectionSsdOut( const std::vector<Double_t>& t0 );
  void ResetStatusSsd( void );
  void ResetStatusSsdIn( void );
  void ResetStatusSsdOut( void );
  void SlopeFilterSsd( void );
  void SlopeFilterSsdIn( void );
  void SlopeFilterSsdOut( void );
  void AdcPeakHeightFilterSsd( Double_t min, Double_t max );
  void AdcPeakHeightFilterSsdIn( Double_t min, Double_t max );
  void AdcPeakHeightFilterSsdOut( Double_t min, Double_t max );
  void AmplitudeFilterSsd( Double_t min, Double_t max, Bool_t cluster_flag=false );
  void AmplitudeFilterSsdIn(Double_t min, Double_t max, Bool_t cluster_flag=false );
  void AmplitudeFilterSsdOut( Double_t min, Double_t max, Bool_t cluster_flag=false );
  void DeltaEFilterSsd( Double_t min, Double_t max, Bool_t cluster_flag=false );
  void DeltaEFilterSsdIn( Double_t min, Double_t max, Bool_t cluster_flag=false );
  void DeltaEFilterSsdOut( Double_t min, Double_t max, Bool_t cluster_flag=false );
  void RmsFilterSsd( Double_t min, Double_t max );
  void RmsFilterSsdIn( Double_t min, Double_t max );
  void RmsFilterSsdOut( Double_t min, Double_t max );
  void DeviationFilterSsd( Double_t min, Double_t max );
  void DeviationFilterSsdIn( Double_t min, Double_t max );
  void DeviationFilterSsdOut( Double_t min, Double_t max );
  void TimeFilterSsd( Double_t min, Double_t max, Bool_t cluster_flag=false );
  void TimeFilterSsdIn( Double_t min, Double_t max, Bool_t cluster_flag=false );
  void TimeFilterSsdOut( Double_t min, Double_t max, Bool_t cluster_flag=false );
  void ChisqrFilterSsd( Double_t maxchi2 );
  void ChisqrFilterSsdIn( Double_t maxchi2 );
  void ChisqrFilterSsdOut( Double_t maxchi2 );

  void ChiSqrCutBcOut( Double_t chisqr );
  void ChiSqrCutSdcIn( Double_t chisqr );
  void ChiSqrCutSdcOut( Double_t chisqr );
  void ChiSqrCutSsdIn( Double_t chisqr );
  void ChiSqrCutSsdOut( Double_t chisqr );

  Int_t GetNTracksK18D2U( void ) const { return m_K18D2UTC.size(); }
  Int_t GetNTracksKurama( void ) const { return m_KuramaTC.size(); }

  K18TrackD2U* GetK18TrackD2U( Int_t i ) const { return m_K18D2UTC[i]; }
  KuramaTrack* GetKuramaTrack( Int_t i ) const { return m_KuramaTC[i]; }
  Int_t GetNClustersMWPC( Int_t layer ) const { return m_MWPCClCont[layer].size(); };
  Int_t GetNClustersSsdIn( Int_t layer ) const { return m_SsdInClCont[layer].size(); };
  Int_t GetNClustersSsdOut( Int_t layer ) const { return m_SsdOutClCont[layer].size(); };

  const MWPCClusterContainer& GetClusterMWPC( Int_t layer ) const { return m_MWPCClCont[layer]; }
  const SsdClusterContainer& GetClusterSsdIn( Int_t layer ) const { return m_SsdInClCont[layer]; }
  const SsdClusterContainer& GetClusterSsdOut( Int_t layer ) const { return m_SsdOutClCont[layer]; }

  Bool_t ReCalcMWPCHits( std::vector<DCHitContainer>& cont,
                         Bool_t applyRecursively=false );
  Bool_t ReCalcDCHits( std::vector<DCHitContainer>& cont,
                       Bool_t applyRecursively=false );
  Bool_t ReCalcDCHits( Bool_t applyRecursively=false );

  Bool_t ReCalcTrack( DCLocalTrackContainer& cont, Bool_t applyRecursively=false );
  Bool_t ReCalcTrack( K18TrackD2UContainer& cont, Bool_t applyRecursively=false );
  Bool_t ReCalcTrack( KuramaTrackContainer& cont, Bool_t applyRecursively=false );

  Bool_t ReCalcTrackBcIn( Bool_t applyRecursively=false );
  Bool_t ReCalcTrackBcOut( Bool_t applyRecursively=false );
  Bool_t ReCalcTrackSdcIn( Bool_t applyRecursively=false );
  Bool_t ReCalcTrackSdcOut( Bool_t applyRecursively=false );
  Bool_t ReCalcTrackSsdIn( Bool_t applyRecursively=false );
  Bool_t ReCalcTrackSsdOut( Bool_t applyRecursively=false );

  Bool_t ReCalcK18TrackD2U( Bool_t applyRecursively=false );
  Bool_t ReCalcKuramaTrack( Bool_t applyRecursively=false );

  Bool_t ReCalcAll( void );

  Bool_t TrackSearchBcOutSdcIn( void );
  Bool_t TrackSearchBcOutSsdIn( void );
  Bool_t TrackSearchSsdOutSdcIn( void );
  Bool_t TrackSearchSdcInSdcOut( void );
  Int_t GetNtracksBcOutSdcIn( void ) const { return m_BcOutSdcInTC.size(); }
  Int_t GetNtracksBcOutSsdIn( void ) const { return m_BcOutSsdInTC.size(); }
  Int_t GetNtracksSsdOutSdcIn( void ) const { return m_SsdOutSdcInTC.size(); }
  Int_t GetNtracksSdcInSdcOut( void ) const { return m_SdcInSdcOutTC.size(); }
  DCLocalTrack* GetTrackBcOutSdcIn( Int_t i ) const { return m_BcOutSdcInTC[i]; }
  DCLocalTrack* GetTrackBcOutSsdIn( Int_t i ) const { return m_BcOutSsdInTC[i]; }
  DCLocalTrack* GetTrackSsdOutSdcIn( Int_t i ) const { return m_SsdOutSdcInTC[i]; }
  DCLocalTrack* GetTrackSdcInSdcOut( Int_t i ) const { return m_SdcInSdcOutTC[i]; }
  void   PrintKurama( const TString& arg="" ) const;

protected:
  void ClearDCHits( void );
  void ClearBcInHits( void );
  void ClearBcOutHits( void );
  void ClearSdcInHits( void );
  void ClearSdcOutHits( void );
  void ClearSsdInHits( void );
  void ClearSsdOutHits( void );
  void ClearTOFHits( void );
  void ClearVtxHits( void );
  void ClearTracksBcIn( void );
  void ClearTracksBcOut( void );
  void ClearTracksSdcIn( void );
  void ClearTracksSdcOut( void );
  void ClearTracksSsdIn( void );
  void ClearTracksSsdOut( void );
  void ClearTracksSsdXY( void );
  void ClearTracksBcOutSdcIn( void );
  void ClearTracksBcOutSsdIn( void );
  void ClearTracksSsdOutSdcIn( void );
  void ClearTracksSdcInSdcOut( void );
  void ClearK18TracksD2U( void );
  void ClearKuramaTracks( void );
  void ChiSqrCut( DCLocalTrackContainer& cont, Double_t chisqr );
  static Int_t MakeUpMWPCClusters( const DCHitContainer& HitCont,
                                   MWPCClusterContainer& ClusterCont,
                                   Double_t maxTimeDif );
public:
  void ResetTracksBcIn( void )        { ClearTracksBcIn();        }
  void ResetTracksBcOut( void )       { ClearTracksBcOut();       }
  void ResetTracksSdcIn( void )       { ClearTracksSdcIn();       }
  void ResetTracksSdcOut( void )      { ClearTracksSdcOut();      }
  void ResetTracksSsdIn( void )       { ClearTracksSsdIn();       }
  void ResetTracksSsdXY( void )       { ClearTracksSsdXY();       }
  void ResetTracksSsdOut( void )      { ClearTracksSsdOut();      }
  void ResetTracksBcOutSdcIn( void )  { ClearTracksBcOutSdcIn();  }
  void ResetTracksBcOutSsdIn( void )  { ClearTracksBcOutSsdIn();  }
  void ResetTracksSsdOutSdcIn( void ) { ClearTracksSsdOutSdcIn(); }
  void ResetTracksSdcInSdcOut( void )  { ClearTracksSdcInSdcOut();  }
  void ApplyBh1SegmentCut( const std::vector<Double_t>& validBh1Cluster );
  void ApplyBh2SegmentCut( const Double_t Time0_Cluster );

  ClassDef(DCAnalyzer,0);
};

#endif
