// -*- C++ -*-

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include <std_ostream.hh>

#include "ConfMan.hh"
#include "DCGeomMan.hh"
#include "DCGeomRecord.hh"
#include "Exception.hh"
#include "FuncName.hh"

ClassImp(DCGeomMan);

//______________________________________________________________________________
void
ConfMan::InitializeDCGeomMan( void )
{
  if(name_file_["DCGEOM:"] != ""){
    DCGeomMan& gDCGeo = DCGeomMan::GetInstance();
    flag_[kIsGood] = gDCGeo.Initialize(name_file_["DCGEOM:"]);
  }else{
    hddaq::cout << "#E ConfMan::"
		<< " File path does not exist in " << name_file_["DCGEOM:"]
		<< std::endl;
    flag_.reset(kIsGood);
  }
}

//______________________________________________________________________________
DCGeomMan::DCGeomMan( void )
  : TObject(),
    m_is_ready(false),
    m_file_name(),
    m_map(),
    TOFid_(51),
    LCid_(54)
{
}

//______________________________________________________________________________
DCGeomMan::~DCGeomMan( void )
{
}

//______________________________________________________________________________
Double_t
DCGeomMan::GetLocalZ( Int_t lnum ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    return itr->second->length_;
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
Double_t
DCGeomMan::GetLocalZ( const TString& name ) const
{
  return GetLocalZ( GetDetectorId(name) );
}

//______________________________________________________________________________
Double_t
DCGeomMan::GetResolution( Int_t lnum ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    return itr->second->resol_;
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
Double_t
DCGeomMan::GetResolution( const TString& name ) const
{
  return GetResolution( GetDetectorId(name) );
}

//______________________________________________________________________________
Double_t
DCGeomMan::GetTiltAngle( Int_t lnum ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    return itr->second->tiltAngle_;
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
Double_t
DCGeomMan::GetTiltAngle( const TString& name ) const
{
  return GetTiltAngle( GetDetectorId(name) );
}

//______________________________________________________________________________
Double_t
DCGeomMan::GetRotAngle1( Int_t lnum ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    return itr->second->rotAngle1_;
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
Double_t
DCGeomMan::GetRotAngle1( const TString& name ) const
{
  return GetRotAngle1( GetDetectorId(name) );
}

//______________________________________________________________________________
Double_t
DCGeomMan::GetRotAngle2( Int_t lnum ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    return itr->second->rotAngle2_;
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
Double_t
DCGeomMan::GetRotAngle2( const TString& name ) const
{
  return GetRotAngle2( GetDetectorId(name) );
}

//______________________________________________________________________________
const ThreeVector&
DCGeomMan::GetGlobalPosition( Int_t lnum ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    return itr->second->pos_;
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
const ThreeVector&
DCGeomMan::GetGlobalPosition( const TString& name ) const
{
  return GetGlobalPosition( GetDetectorId(name) );
}

//______________________________________________________________________________
ThreeVector
DCGeomMan::NormalVector( Int_t lnum ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    return itr->second->NormalVector();
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
ThreeVector
DCGeomMan::NormalVector( const TString& name ) const
{
  return NormalVector( GetDetectorId(name) );
}

//______________________________________________________________________________
ThreeVector
DCGeomMan::UnitVector( Int_t lnum ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    return itr->second->UnitVector();
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
ThreeVector
DCGeomMan::UnitVector( const TString& name ) const
{
  return UnitVector( GetDetectorId(name) );
}

//______________________________________________________________________________
const DCGeomRecord*
DCGeomMan::GetRecord( Int_t lnum ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    return itr->second;
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
const DCGeomRecord*
DCGeomMan::GetRecord( const TString& name ) const
{
  return GetRecord( GetDetectorId(name) );
}

//______________________________________________________________________________
Double_t
DCGeomMan::CalcWirePosition( Int_t lnum, Double_t wire ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    return itr->second->WirePos(wire);
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
Int_t
DCGeomMan::CalcWireNumber( Int_t lnum, Double_t pos ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    return itr->second->WireNumber(pos);
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
void
DCGeomMan::clearElements( void )
{
  //  for_each( m_map.begin(), m_map.end(), DeleteObject() );
  std::map <Int_t, DCGeomRecord *>::iterator itr;
  for( itr=m_map.begin(); itr!=m_map.end(); ++itr ){
    delete itr->second;
    itr->second = 0;
  }
  m_map.clear();
  TOFid_=51;
}

//______________________________________________________________________________
Bool_t
DCGeomMan::Initialize( void )
{
  Int_t id;
  Double_t xs, ys, zs, ta, ra1, ra2, l, res, w0, dd, ofs;
  char cname[100];
  //Double_t A = 0.8/750.;

  std::ifstream ifs( m_file_name );

  if( !ifs.is_open() ){
    hddaq::cerr << FUNC_NAME << ": file open fail" << std::endl;
    return false;
  }

  clearElements();

  TString line;
  while( ifs.good() && line.ReadLine(ifs) ){
    if( line[0] == '#' ) continue;

    if( std::sscanf( line.Data(), "%d %s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
		     &id, cname, &xs, &ys, &zs, &ta, &ra1, &ra2, &l, &res,
		     &w0, &dd, &ofs ) == 13 ){
      DCGeomRecord *pRec = new DCGeomRecord( id, cname, xs, ys, zs,
					     ta, ra1, ra2, l, res,
					     w0, dd, ofs );
      DCGeomRecord *pOld = m_map[id];
      m_map[id] = pRec;
      if(strcmp(cname,"TOF")==0) {
	TOFid_=id;
      }

      if( pOld ){
	hddaq::cerr << FUNC_NAME << ": duplicated id number. "
		    << " following record is deleted." << std::endl;
	hddaq::cerr << "Id=" << pOld->id_ << " " << pOld->pos_
		    << " ) ... " << std::endl;
	delete pOld;
      }
    }
    else {
      hddaq::cerr << FUNC_NAME << ": Invalid format " << line << std::endl;
    }
  }

  hddaq::cout << FUNC_NAME << " Initialization finished." << std::endl;
  m_is_ready = true;
  return true;
}

//______________________________________________________________________________
std::vector<Int_t>
DCGeomMan::GetDetectorIDList( void ) const
{
  std::vector<Int_t> vlist;
  vlist.reserve( m_map.size() );
  DCGeomIterator itr=m_map.begin(), end=m_map.end();
  for(; itr!=end; ++itr ){
    vlist.push_back( itr->first );
  }
  return vlist;
}

//______________________________________________________________________________
ThreeVector
DCGeomMan::Local2GlobalPos( Int_t lnum,
			    const ThreeVector &in ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    DCGeomRecord *pGeo = itr->second;
    Double_t x = pGeo->dxds_*in.x() + pGeo->dxdt_*in.y()
      + pGeo->dxdu_*in.z() + pGeo->pos_.x();
    Double_t y = pGeo->dyds_*in.x() + pGeo->dydt_*in.y()
      + pGeo->dydu_*in.z() + pGeo->pos_.y();
    Double_t z = pGeo->dzds_*in.x() + pGeo->dzdt_*in.y()
      + pGeo->dzdu_*in.z() + pGeo->pos_.z();
    return ThreeVector( x, y, z );
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
ThreeVector
DCGeomMan::Local2GlobalPos( const TString& name,
			    const ThreeVector &in ) const
{
  return Local2GlobalPos( GetDetectorId( name ), in );
}

//______________________________________________________________________________
ThreeVector
DCGeomMan::Global2LocalPos( Int_t lnum,
			    const ThreeVector &in ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    DCGeomRecord *pGeo = itr->second;
    Double_t x
      = pGeo->dsdx_*(in.x()-pGeo->pos_.x())
      + pGeo->dsdy_*(in.y()-pGeo->pos_.y())
      + pGeo->dsdz_*(in.z()-pGeo->pos_.z());
    Double_t y
      = pGeo->dtdx_*(in.x()-pGeo->pos_.x())
      + pGeo->dtdy_*(in.y()-pGeo->pos_.y())
      + pGeo->dtdz_*(in.z()-pGeo->pos_.z());
    Double_t z
      = pGeo->dudx_*(in.x()-pGeo->pos_.x())
      + pGeo->dudy_*(in.y()-pGeo->pos_.y())
      + pGeo->dudz_*(in.z()-pGeo->pos_.z());
    return ThreeVector( x, y, z );
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
ThreeVector
DCGeomMan::Global2LocalPos( const TString& name,
			    const ThreeVector &in ) const
{
  return Global2LocalPos( GetDetectorId( name ), in );
}

//______________________________________________________________________________
ThreeVector
DCGeomMan::Local2GlobalDir( Int_t lnum,
			    const ThreeVector &in ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    DCGeomRecord *pGeo = itr->second;
    Double_t x = pGeo->dxds_*in.x() + pGeo->dxdt_*in.y()
      + pGeo->dxdu_*in.z();
    Double_t y = pGeo->dyds_*in.x() + pGeo->dydt_*in.y()
      + pGeo->dydu_*in.z();
    Double_t z = pGeo->dzds_*in.x() + pGeo->dzdt_*in.y()
      + pGeo->dzdu_*in.z();
    return ThreeVector( x, y, z );
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
ThreeVector
DCGeomMan::Local2GlobalDir( const TString& name,
			    const ThreeVector &in ) const
{
  return Local2GlobalDir( GetDetectorId( name ), in );
}

//______________________________________________________________________________
ThreeVector
DCGeomMan::Global2LocalDir( Int_t lnum,
			    const ThreeVector& in ) const
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    DCGeomRecord *pGeo = itr->second;
    Double_t x = pGeo->dsdx_*in.x() + pGeo->dsdy_*in.y()+ pGeo->dsdz_*in.z();
    Double_t y = pGeo->dtdx_*in.x() + pGeo->dtdy_*in.y()+ pGeo->dtdz_*in.z();
    Double_t z = pGeo->dudx_*in.x() + pGeo->dudy_*in.y()+ pGeo->dudz_*in.z();
    return ThreeVector( x, y, z );
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
ThreeVector
DCGeomMan::Global2LocalDir( const TString& name,
			    const ThreeVector &in ) const
{
  return Global2LocalDir( GetDetectorId( name ), in );
}

//______________________________________________________________________________
void
DCGeomMan::SetResolution( Int_t lnum,  Double_t res)
{
  DCGeomIterator itr = m_map.find(lnum);
  if( itr != m_map.end() ){
    itr->second->resol_ = res;
  } else {
    hddaq::cerr << FUNC_NAME << ": No record. Layer#="
		<< lnum << std::endl;
    throw Exception( FUNC_NAME+": No record" );
  }
}

//______________________________________________________________________________
Int_t
DCGeomMan::GetDetectorId( const TString& name ) const
{
  DCGeomIterator itr=m_map.begin(), end=m_map.end();
  for(; itr!=end; ++itr ){
    if( itr->second->name_ == name ){
      return itr->second->id_;
    }
  }

  hddaq::cerr << FUNC_NAME << " : No such detector " << name << std::endl;
  return -9999;
}
