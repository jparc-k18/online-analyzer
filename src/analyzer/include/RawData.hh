// -*- C++ -*-

#ifndef RAW_DATA_HH
#define RAW_DATA_HH

#include <vector>

#include <TObject.h>

#include "DetectorID.hh"

// class HodoRawHit;
class DCRawHit;

// typedef std::vector<HodoRawHit*> HodoRHitContainer;
typedef std::vector<DCRawHit*>   DCRHitContainer;

//______________________________________________________________________________
class RawData : public TObject
{
public:
  RawData( void );
  ~RawData( void );

private:
  RawData( const RawData& );
  RawData& operator=( const RawData& );

private:
  Bool_t                         m_is_decoded;
  // HodoRHitContainer              m_BH1RawHC;
  // HodoRHitContainer              m_BH2RawHC;
  // HodoRHitContainer              m_BACRawHC;
  // HodoRHitContainer              m_PVACRawHC;
  // HodoRHitContainer              m_FACRawHC;
  // HodoRHitContainer              m_TOFRawHC;
  // std::vector<HodoRHitContainer> m_BFTRawHC;
  // HodoRHitContainer              m_SCHRawHC;
  // HodoRHitContainer              m_FBHRawHC;
  // HodoRHitContainer              m_SSDTRawHC;

  std::vector<DCRHitContainer> m_BcInRawHC;
  std::vector<DCRHitContainer> m_BcOutRawHC;
  std::vector<DCRHitContainer> m_SdcInRawHC;
  std::vector<DCRHitContainer> m_SdcOutRawHC;
  std::vector<DCRHitContainer> m_SsdInRawHC;
  std::vector<DCRHitContainer> m_SsdOutRawHC;

  // HodoRHitContainer              m_ScalerRawHC;
  // HodoRHitContainer              m_TrigRawHC;
  // HodoRHitContainer              m_VmeCalibRawHC;
  // HodoRHitContainer              m_MsTRMRawHC;
  // std::vector<HodoRHitContainer> m_MsTRawHC;
  // HodoRHitContainer              m_HulTOFRawHC;
  // HodoRHitContainer              m_HulFBHRawHC;
  // HodoRHitContainer              m_HulSCHRawHC;

private:
  enum EUorD { kOneSide=1, kBothSide=2 };
  enum EDCDataType { kLeading, kTrailing, kNDCDataType };

public:
  Bool_t AddDCRawHit( DCRHitContainer& cont,
		      int plane, int wire, int tdc, int type=kLeading );
  void                     ClearAll( void );
  Bool_t                   DecodeHits( void );
  Bool_t                   DecodeCalibHits( void );
  // const HodoRHitContainer& GetBH1RawHC( void ) const { return m_BH1RawHC; }
  // const HodoRHitContainer& GetBH2RawHC( void ) const { return m_BH2RawHC; }
  // const HodoRHitContainer& GetBACRawHC( void ) const { return m_BACRawHC; }
  // const HodoRHitContainer& GetPVACRawHC( void ) const { return m_PVACRawHC; }
  // const HodoRHitContainer& GetFACRawHC( void ) const { return m_FACRawHC; }
  // const HodoRHitContainer& GetTOFRawHC( void ) const { return m_TOFRawHC; }
  // const HodoRHitContainer& GetLACRawHC( void ) const;
  // const HodoRHitContainer& GetLCRawHC( void ) const;
  // const HodoRHitContainer& GetBFTRawHC( Int_t plane ) const { return m_BFTRawHC.at(plane); }
  // const HodoRHitContainer& GetSCHRawHC( void ) const { return m_SCHRawHC; }
  // const HodoRHitContainer& GetFBHRawHC( void ) const { return m_FBHRawHC; }
  // const HodoRHitContainer& GetSSDTRawHC( void ) const { return m_SSDTRawHC; }
  // const DCRHitContainer&   GetBcInRawHC( Int_t layer ) const { return m_BcInRawHC.at(layer); }
  const DCRHitContainer&   GetBcOutRawHC( Int_t layer ) const { return m_BcOutRawHC.at(layer); }
  const DCRHitContainer&   GetSdcInRawHC( Int_t layer ) const { return m_SdcInRawHC.at(layer); }
  const DCRHitContainer&   GetSdcOutRawHC( Int_t layer ) const { return m_SdcOutRawHC.at(layer); }
  const DCRHitContainer&   GetSsdInRawHC( Int_t layer ) const { return m_SsdInRawHC.at(layer); }
  const DCRHitContainer&   GetSsdOutRawHC( Int_t layer ) const { return m_SsdOutRawHC.at(layer); }
  const DCRHitContainer&   GetSsdOutSdcInRawHC( Int_t layer ) const;
  // const HodoRHitContainer& GetScalerRawHC( void ) const { return m_ScalerRawHC; }
  // const HodoRHitContainer& GetTrigRawHC( void ) const { return m_TrigRawHC; }
  // const HodoRHitContainer& GetVmeCalibRawHC( void ) const { return m_VmeCalibRawHC; }
  // const HodoRHitContainer& GetMsTRMRawHC( void ) const { return m_MsTRMRawHC; }
  // const HodoRHitContainer& GetMsTRawHC( Int_t layer ) const { return m_MsTRawHC.at(layer); }
  // const HodoRHitContainer& GetHulTOFRawHC( void ) const { return m_HulTOFRawHC; }
  // const HodoRHitContainer& GetHulFBHRawHC( void ) const { return m_HulFBHRawHC; }
  // const HodoRHitContainer& GetHulSCHRawHC( void ) const { return m_HulSCHRawHC; }

  ClassDef(RawData,0);
};

#endif
