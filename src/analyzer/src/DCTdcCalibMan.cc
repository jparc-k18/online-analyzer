// -*- C++ -*-

#include <cstdio>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>

#include "ConfMan.hh"
#include "DCTdcCalibMan.hh"
#include "FuncName.hh"

ClassImp(DCTdcCalibMan);

//______________________________________________________________________________
namespace
{
  inline UInt_t MakeKey( Int_t plane, Double_t wire )
  {
    return (plane<<10) | Int_t(wire);
  }
}

//______________________________________________________________________________
void
ConfMan::InitializeDCTdcCalibMan( void )
{
  if(name_file_["TDCCALIB:"] != ""){
    DCTdcCalibMan& gDCTdcCalib = DCTdcCalibMan::GetInstance();
    gDCTdcCalib.SetFileName(name_file_["TDCCALIB:"]);
    flag_[kIsGood] = gDCTdcCalib.Initialize();
  }else{
    std::cout << "#E ConfMan::"
	      << " File path does not exist in " << name_file_["TDCCALIB:"]
	      << std::endl;
    flag_.reset(kIsGood);
  }
}

//______________________________________________________________________________
struct DCTdcCalMap
{
  DCTdcCalMap( Double_t q0, Double_t q1 )
    : p0(q0), p1(q1)
  {}
  Double_t p0, p1;
};

//______________________________________________________________________________
DCTdcCalibMan::DCTdcCalibMan( void )
  : TObject()
{
}

//______________________________________________________________________________
DCTdcCalibMan::~DCTdcCalibMan( void )
{
  ClearElements();
}

//______________________________________________________________________________
void
DCTdcCalibMan::ClearElements( void )
{
  std::map <UInt_t, DCTdcCalMap *>::iterator itr;
  for( itr=m_map.begin(); itr!=m_map.end(); ++itr ){
    delete itr->second;
    itr->second = 0;
  }
  m_map.clear();
}

//______________________________________________________________________________
Bool_t
DCTdcCalibMan::Initialize( void )
{
  Int_t pid, wid;
  Double_t p0, p1;

  std::ifstream ifs( m_file_name );
  if( !ifs.is_open() ){
    //hddaq::cerr << FUNC_NAME << ": file open fail" << std::endl;
    return false;
  }

  TString line;
  while( ifs.good() && line.ReadLine(ifs) ){
    if( line[0] != '#' ) continue;
    if( std::sscanf( line.Data(), "%d %d %lf %lf",
		     &pid, &wid, &p1, &p0 ) == 4 ){
      UInt_t key = MakeKey( pid, wid );
      DCTdcCalMap *p = new DCTdcCalMap( p0, p1 );
      if(p){
	if(m_map[key]) delete m_map[key];
	m_map[key]=p;
      }
      else{
	// std::cerr << FUNC_NAME << ": new fail. "
	// 	  << " Plane=" << std::setw(3) << std::dec << pid
	// 	  << " Wire=" << std::setw(3) << std::dec << wid
	// 	  << std::endl;
      }
    }
    else{
      // std::cerr << FUNC_NAME << ": Bad format => "
      // 		<< line << std::endl;
    }
  }

  // hddaq::cout << FUNC_NAME << ": Initialization finished" << std::endl;
  return true;
}

//______________________________________________________________________________
DCTdcCalMap*
DCTdcCalibMan::GetMap( Int_t PlaneId, Double_t WireId ) const
{
  UInt_t key = MakeKey( PlaneId, WireId );
  std::map<UInt_t, DCTdcCalMap*>::const_iterator itr = m_map.find(key);
  if( itr != m_map.end() )
    return itr->second;
  else
    return nullptr;
}

//______________________________________________________________________________
Bool_t
DCTdcCalibMan::GetTime( Int_t PlaneId, Double_t WireId,
			Int_t tdc, Double_t & time ) const
{
  DCTdcCalMap *p=GetMap(PlaneId,WireId);
  if(p){
    time=(p->p0)+(p->p1)*tdc;
    return true;
  }
  else{
    // std::cerr << FUNC_NAME << ": No record. "
    // 	      << " PlaneId=" << std::setw(3) << std::dec << PlaneId
    // 	      << " WireId=" << std::setw(3) << std::dec << WireId
    // 	      << std::endl;
    return false;
  }
}

//______________________________________________________________________________
Bool_t
DCTdcCalibMan::GetTdc( Int_t PlaneId, Double_t WireId,
		       Double_t time, Int_t &tdc ) const
{
  DCTdcCalMap *p = GetMap( PlaneId, WireId );
  if(p){
    tdc = (Int_t)((time-(p->p0))/(p->p1));
    return true;
  }
  else{
    // std::cerr << FUNC_NAME << ": No record. "
    // 	      << " PlaneId=" << std::setw(3) << std::dec << PlaneId
    // 	      << " WireId=" << std::setw(3) << std::dec << WireId
    // 	      << std::endl;
    return false;
  }
}
