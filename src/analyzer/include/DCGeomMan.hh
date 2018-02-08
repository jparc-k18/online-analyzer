// -*- C++ -*-

#ifndef DC_GEOM_MAN_HH
#define DC_GEOM_MAN_HH

#include <string>
#include <vector>
#include <map>

#include "ThreeVector.hh"

class DCGeomRecord;

//______________________________________________________________________________
class DCGeomMan : public TObject
{
public:
  static DCGeomMan & GetInstance( void );
  ~DCGeomMan( void );

private:
  DCGeomMan( void );
  DCGeomMan( const DCGeomMan& );
  DCGeomMan& operator =( const DCGeomMan& );

private:
  typedef std::map <Int_t, DCGeomRecord *> DCGeomMap;
  typedef DCGeomMap::const_iterator        DCGeomIterator;
  TString   m_file_name;
  DCGeomMap m_map;
  Int_t     TOFid_;
  Int_t     LCid_;

public:
  void                SetFileName( const char* file_name ) { m_file_name = file_name; }
  void                SetFileName( const TString & file_name ) { m_file_name = file_name; }
  Bool_t              Initialize( void );
  Bool_t              Initialize( const char* file_name )
  { m_file_name = file_name; return Initialize(); }
  Bool_t              Initialize( const TString& file_name )
  { m_file_name = file_name; return Initialize(); }
  Double_t            GetLocalZ( Int_t lnum ) const;
  Double_t            GetResolution( Int_t lnum ) const;
  Double_t            GetTiltAngle( Int_t lnum ) const;
  Double_t            GetRotAngle1( Int_t lnum ) const;
  Double_t            GetRotAngle2( Int_t lnum ) const;
  const ThreeVector&  GetGlobalPosition( Int_t lnum ) const;
  ThreeVector         NormalVector( Int_t lnum ) const;
  ThreeVector         UnitVector( Int_t lnum ) const;
  const DCGeomRecord* GetRecord( Int_t lnum ) const;
  ThreeVector         Local2GlobalPos( Int_t lnum, const ThreeVector &in ) const;
  ThreeVector         Global2LocalPos( Int_t lnum, const ThreeVector &in ) const;
  ThreeVector         Local2GlobalDir( Int_t lnum, const ThreeVector &in ) const;
  ThreeVector         Global2LocalDir( Int_t lnum, const ThreeVector &in ) const;
  Double_t            calcWirePosition( Int_t lnum, Double_t wire ) const;
  Int_t               calcWireNumber( Int_t lnum, Double_t position ) const;
  std::vector<Int_t>  GetDetectorIDList( void ) const;
  Int_t               GetTofId( void ) const { return TOFid_; }
  Int_t               GetLcId( void ) const { return LCid_; }
  Int_t               GetDetectorId( const TString& detName ) const;
  // Do not use this method except for special cases
  void                SetResolution( Int_t lnum, Double_t res );
  //

private:
  void clearElements( void );

  ClassDef(DCGeomMan,0);
};

//______________________________________________________________________________
inline DCGeomMan&
DCGeomMan::GetInstance( void )
{
  static DCGeomMan g_instance;
  return g_instance;
}

#endif
