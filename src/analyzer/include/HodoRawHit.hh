// -*- C++ -*-

#ifndef HODO_RAW_HIT_HH
#define HODO_RAW_HIT_HH

#include <iostream>
#include <vector>

#include <TObject.h>

//______________________________________________________________________________
class HodoRawHit : public TObject
{
public:
  HodoRawHit( Int_t detector_id, Int_t plane_id, Int_t segment_id );
  ~HodoRawHit( void );

private:
  Int_t              m_detector_id;
  Int_t              m_plane_id;
  Int_t              m_segment_id;
  std::vector<Int_t> m_adc1;
  std::vector<Int_t> m_adc2;
  std::vector<Int_t> m_tdc1;
  std::vector<Int_t> m_tdc2;
  Int_t              m_nhtdc;

public:
  void  SetAdc1( Int_t adc );
  void  SetAdc2( Int_t adc );
  void  SetTdc1( Int_t tdc );
  void  SetTdc2( Int_t tdc );
  void  SetAdcUp( Int_t adc )    { SetAdc1(adc); }
  void  SetAdcLeft( Int_t adc )  { SetAdc1(adc); }
  void  SetAdcDown( Int_t adc )  { SetAdc2(adc); }
  void  SetAdcRight( Int_t adc ) { SetAdc2(adc); }
  void  SetTdcUp( Int_t tdc )    { SetTdc1(tdc); }
  void  SetTdcLeft( Int_t tdc )  { SetTdc1(tdc); }
  void  SetTdcDown( Int_t tdc )  { SetTdc2(tdc); }
  void  SetTdcRight( Int_t tdc ) { SetTdc2(tdc); }
  Int_t DetectorId( void )      const { return m_detector_id; }
  Int_t PlaneId( void )         const { return m_plane_id;    }
  Int_t SegmentId( void )       const { return m_segment_id;  }
  // for Multi-hit method
  Int_t GetNumOfTdcHits( void )  const { return m_nhtdc;       }
  Int_t GetAdc1( Int_t i=0 )     const { return m_adc1.at(i);  }
  Int_t GetAdc2( Int_t i=0 )     const { return m_adc2.at(i);  }
  Int_t GetTdc1( Int_t i=0 )     const { return m_tdc1.at(i);  }
  Int_t GetTdc2( Int_t i=0 )     const { return m_tdc2.at(i);  }
  Int_t GetAdcUp( Int_t i=0 )    const { return GetAdc1(i);    }
  Int_t GetAdcLeft( Int_t i=0 )  const { return GetAdc1(i);    }
  Int_t GetAdcDown( Int_t i=0 )  const { return GetAdc2(i);    }
  Int_t GetAdcRight( Int_t i=0 ) const { return GetAdc2(i);    }
  Int_t GetTdcUp( Int_t i=0 )    const { return GetTdc1(i);    }
  Int_t GetTdcLeft( Int_t i=0 )  const { return GetTdc1(i);    }
  Int_t GetTdcDown( Int_t i=0 )  const { return GetTdc2(i);    }
  Int_t GetTdcRight( Int_t i=0 ) const { return GetTdc2(i);    }
  Int_t SizeAdc1( void ) const;
  Int_t SizeAdc2( void ) const;
  Int_t SizeTdc1( void ) const;
  Int_t SizeTdc2( void ) const;
  Int_t GetSizeAdcUp( void )    const { return SizeAdc1(); }
  Int_t GetSizeAdcLeft( void )  const { return SizeAdc1(); }
  Int_t GetSizeAdcDown( void )  const { return SizeAdc2(); }
  Int_t GetSizeAdcRight( void ) const { return SizeAdc2(); }
  Int_t GetSizeTdcUp( void )    const { return SizeTdc1(); }
  Int_t GetSizeTdcLeft( void )  const { return SizeTdc1(); }
  Int_t GetSizeTdcDown( void )  const { return SizeTdc2(); }
  Int_t GetSizeTdcRight( void ) const { return SizeTdc2(); }
  void  Clear( Option_t* option="" );
  void  Print( Option_t* option="" ) const;

  ClassDef(HodoRawHit,0);
};

#endif
