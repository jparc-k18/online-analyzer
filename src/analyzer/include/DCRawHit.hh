// -*- C++ -*-

#ifndef DC_RAW_HIT_HH
#define DC_RAW_HIT_HH

#include <vector>

#include <TObject.h>
#include <TString.h>

//______________________________________________________________________________
class DCRawHit : public TObject
{
public:
  DCRawHit( Int_t plane_id, Int_t wire_id );
  ~DCRawHit( void );

private:
  Int_t              m_plane_id;
  Int_t              m_wire_id;
  std::vector<Int_t> m_tdc;
  std::vector<Int_t> m_trailing;

public:
  Int_t GetTdc( Int_t nh )      const { return m_tdc.at(nh); }
  Int_t GetTdcSize( void )      const { return m_tdc.size(); }
  Int_t GetTrailing( Int_t nh ) const { return m_trailing.at(nh); }
  Int_t GetTrailingSize( void ) const { return m_trailing.size(); }
  Int_t PlaneId( void )         const { return m_plane_id; }
  void  SetTdc( Int_t tdc )           { m_tdc.push_back(tdc); }
  void  SetTrailing( Int_t tdc )      { m_trailing.push_back(tdc); }
  Int_t WireId( void )          const { return m_wire_id;  }
  void  Print( Option_t* option="" ) const;

  ClassDef(DCRawHit,0);
};

#endif
