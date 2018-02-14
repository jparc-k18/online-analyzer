// -*- C++ -*-

#ifndef HODO_ANALYZER_HH
#define HODO_ANALYZER_HH

#include <vector>

#include <TObject.h>

#include "DetectorID.hh"
#include "RawData.hh"

class RawData;
class Hodo1Hit;
class Hodo2Hit;
class BH2Hit;
class FiberHit;
class FLHit;
class HodoCluster;
class BH2Cluster;
class FiberCluster;

typedef std::vector <Hodo1Hit*> Hodo1HitContainer;
typedef std::vector <Hodo2Hit*> Hodo2HitContainer;
typedef std::vector <BH2Hit*>   BH2HitContainer;
typedef std::vector <FiberHit*> FiberHitContainer;
typedef std::vector <FLHit*>    FLHitContainer;

typedef std::vector <HodoCluster*>  HodoClusterContainer;
typedef std::vector <BH2Cluster*>   BH2ClusterContainer;
typedef std::vector <FiberCluster*> FiberClusterContainer;

//______________________________________________________________________________
class HodoAnalyzer : public TObject
{
public:
  HodoAnalyzer( void );
  ~HodoAnalyzer( void );

  static HodoAnalyzer& GetInstance( void );

private:
  HodoAnalyzer( const HodoAnalyzer& );
  HodoAnalyzer& operator =( const HodoAnalyzer& );

private:
  Hodo2HitContainer     m_BH1Cont;
  BH2HitContainer       m_BH2Cont;
  Hodo1HitContainer     m_BACCont;
  Hodo1HitContainer     m_SACCont;
  Hodo2HitContainer     m_TOFCont;
  Hodo2HitContainer     m_LCCont;
  FiberHitContainer     m_BFTCont;
  FiberHitContainer     m_SCHCont;
  HodoClusterContainer  m_BH1ClCont;
  BH2ClusterContainer   m_BH2ClCont;
  HodoClusterContainer  m_TOFClCont;
  FiberClusterContainer m_BFTClCont;
  FiberClusterContainer m_SCHClCont;

public:
  Bool_t DecodeRawHits( RawData* rawData );
  Bool_t DecodeBH1Hits( RawData* rawData );
  Bool_t DecodeBH2Hits( RawData* rawData );
  Bool_t DecodeBACHits( RawData* rawData );
  Bool_t DecodeSACHits( RawData* rawData );
  Bool_t DecodeTOFHits( RawData* rawData );
  Bool_t DecodeLCHits( RawData* rawData );
  Bool_t DecodeBFTHits( RawData* rawData );
  Bool_t DecodeSCHHits( RawData* rawData );
  Int_t  GetNHitsBH1( void )  const { return m_BH1Cont.size(); };
  Int_t  GetNHitsBH2( void )  const { return m_BH2Cont.size(); };
  Int_t  GetNHitsBAC( void )  const { return m_BACCont.size(); };
  Int_t  GetNHitsSAC( void )  const { return m_SACCont.size(); };
  Int_t  GetNHitsTOF( void )  const { return m_TOFCont.size(); };
  Int_t  GetNHitsLC( void ) const { return m_LCCont.size(); };
  Int_t  GetNHitsBFT( void )  const { return m_BFTCont.size(); };
  Int_t  GetNHitsSCH( void )  const { return m_SCHCont.size(); };
  Int_t  GetNClustersBH1( void ) const { return m_BH1ClCont.size(); };
  Int_t  GetNClustersBH2( void ) const { return m_BH2ClCont.size(); };
  Int_t  GetNClustersTOF( void ) const { return m_TOFClCont.size(); }
  Int_t  GetNClustersBFT( void ) const { return m_BFTClCont.size(); };
  Int_t  GetNClustersSCH( void ) const { return m_SCHClCont.size(); };
  Hodo2Hit* GetHitBH1( Int_t i ) const { return m_BH1Cont.at(i); }
  BH2Hit*   GetHitBH2( Int_t i ) const { return m_BH2Cont.at(i); }
  Hodo1Hit* GetHitBAC( Int_t i ) const { return m_BACCont.at(i); }
  Hodo1Hit* GetHitSAC( Int_t i ) const { return m_SACCont.at(i); }
  Hodo2Hit* GetHitTOF( Int_t i ) const { return m_TOFCont.at(i); }
  Hodo2Hit* GetHitLC( Int_t i ) const { return m_LCCont.at(i); }
  FiberHit* GetHitBFT( Int_t i ) const { return m_BFTCont.at(i); }
  FiberHit* GetHitSCH( Int_t i ) const { return m_SCHCont.at(i); }
  HodoCluster*  GetClusterBH1( Int_t i ) const { return m_BH1ClCont.at(i); }
  BH2Cluster*   GetClusterBH2( Int_t i ) const { return m_BH2ClCont.at(i); }
  HodoCluster*  GetClusterTOF( Int_t i ) const { return m_TOFClCont.at(i); }
  FiberCluster* GetClusterBFT( Int_t i ) const { return m_BFTClCont.at(i); }
  FiberCluster* GetClusterSCH( Int_t i ) const { return m_SCHClCont.at(i); }
  Bool_t ReCalcBH1Hits( Bool_t applyRecursively=false );
  Bool_t ReCalcBH2Hits( Bool_t applyRecursively=false );
  Bool_t ReCalcBACHits( Bool_t applyRecursively=false );
  Bool_t ReCalcSACHits( Bool_t applyRecursively=false );
  Bool_t ReCalcTOFHits( Bool_t applyRecursively=false );
  Bool_t ReCalcLCHits( Bool_t applyRecursively=false );
  Bool_t ReCalcBH1Clusters( Bool_t applyRecursively=false );
  Bool_t ReCalcBH2Clusters( Bool_t applyRecursively=false );
  Bool_t ReCalcTOFClusters( Bool_t applyRecursively=false );
  Bool_t ReCalcAll( void );
  void TimeCutBH1( Double_t tmin, Double_t tmax );
  void TimeCutBH2( Double_t tmin, Double_t tmax );
  void TimeCutBFT( Double_t tmin, Double_t tmax );
  void TimeCutSCH( Double_t tmin, Double_t tmax );

private:
  void ClearBH1Hits( void );
  void ClearBH2Hits( void );
  void ClearBACHits( void );
  void ClearSACHits( void );
  void ClearTOFHits( void );
  void ClearLCHits( void );
  void ClearBFTHits( void );
  void ClearSCHHits( void );

  template<typename TypeCluster>
  void TimeCut( std::vector<TypeCluster>& cont,
		Double_t tmin, Double_t tmax );
  static Int_t MakeUpClusters( const Hodo2HitContainer& HitCont,
			       HodoClusterContainer& ClusterCont,
			       Double_t maxTimeDif );
  static Int_t MakeUpClusters( const BH2HitContainer& HitCont,
			       BH2ClusterContainer& ClusterCont,
			       Double_t maxTimeDif );
  static Int_t MakeUpClusters( const FiberHitContainer& cont,
			       FiberClusterContainer& ClusterCont,
			       Double_t maxTimeDif,
			       Int_t DifPairId );
  static Int_t MakeUpClusters( const FLHitContainer& cont,
			       FiberClusterContainer& ClusterCont,
			       Double_t maxTimeDif,
			       Int_t DifPairId );
  static Int_t MakeUpCoincidence( const FiberHitContainer& cont,
				  FLHitContainer& CoinCont,
				  Double_t maxTimeDif );

  ClassDef(HodoAnalyzer,0);
};

#endif
