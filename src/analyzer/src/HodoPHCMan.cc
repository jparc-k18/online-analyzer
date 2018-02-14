// -*- C++ -*-

#include "HodoPHCMan.hh"
#include "ConfMan.hh"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <limits>
#include <fstream>
#include <cstdlib>

namespace
{
  const Double_t EPS = std::numeric_limits<Double_t>::epsilon();

  const Int_t SegMask  = 0x03FF;
  const Int_t CidMask  = 0x00FF;
  const Int_t PlidMask = 0x00FF;
  const Int_t UdMask   = 0x0003;

  const Int_t SegShift  =  0;
  const Int_t CidShift  = 11;
  const Int_t PlidShift = 19;
  const Int_t UdShift   = 27;

  //______________________________________________________________________________
  inline Int_t MakeKey( Int_t cid, Int_t pl, Int_t seg, Int_t ud )
  {
    return (((cid&CidMask)<<CidShift) | ((pl&PlidMask)<<PlidShift)
	    | ((seg&SegMask)<<SegShift) | ((ud&UdMask)<<UdShift) );
  }

}

//______________________________________________________________________________
void
ConfMan::InitializeHodoPHCMan( void )
{
  if(name_file_["HDPHC:"] != ""){
    HodoPHCMan& gHodoPHC = HodoPHCMan::GetInstance();
    gHodoPHC.SetFileName(name_file_["HDPHC:"]);
    flag_[kIsGood] = gHodoPHC.Initialize();
  } else {
    std::cout << "#E ConfMan::"
	      << " File path does not exist in " << name_file_["HDPHC:"]
	      << std::endl;
    flag_.reset(kIsGood);
  }
}

//______________________________________________________________________________
HodoPHCParam::HodoPHCParam( Int_t type, Int_t np, Double_t *parlist )
  : Type(type),NPar(np)
{
  if(np>0){
    ParList = new Double_t [NPar];
    for( Int_t i=0; i<NPar; ++i ) ParList[i]=parlist[i];
  }
  else
    ParList=0;
}

//______________________________________________________________________________
HodoPHCParam::~HodoPHCParam( void )
{
  if(ParList)
    delete [] ParList;
}

//______________________________________________________________________________
Double_t
HodoPHCParam::DoPHC( Double_t time, Double_t de )
{
  static const TString funcname = "[HodoPHCParam::DoPHC]";
  Double_t retval=time;

  switch(Type){
  case 0:
    retval=time; break;
  case 1:
    retval=type1Correction(time,de); break;
  case 2:
    retval=type2Correction(time,de); break; // fiber
  default:
    std::cerr << funcname << ": No Correction Method. Type="
              << Type << std::endl;
  }
  return retval;
}

Double_t HodoPHCParam::DoRPHC( Double_t time, Double_t de )
{
  static const TString funcname = "[HodoPHCParam::DoRPHC]";
  Double_t retval=time;

  switch(Type){
  case 0:
    retval=time; break;
  case 1:
    retval=type1RCorrection(time,de); break;
  default:
    std::cerr << funcname << ": No Correction Method. Type="
              << Type << std::endl;
  }
  return retval;
}

//______________________________________________________________________________
Double_t
HodoPHCParam::type1Correction( Double_t time, Double_t de )
{
  if(fabs(de-ParList[1])<EPS) de=ParList[1]+EPS;
  return time-ParList[0]/sqrt(fabs(de-ParList[1]))+ParList[2];
}

//______________________________________________________________________________
Double_t
HodoPHCParam::type2Correction( Double_t time, Double_t w )
{
  // Correction function for fiber is quadratic function
  return time-(ParList[0]*w*w + ParList[1]*w + ParList[2]);
}

//______________________________________________________________________________
Double_t
HodoPHCParam::type1RCorrection( Double_t time, Double_t de )
{
  if(fabs(de-ParList[1])<EPS) de=ParList[1]+EPS;
  return time+ParList[0]/sqrt(fabs(de-ParList[1]))-ParList[2];
}

//______________________________________________________________________________
HodoPHCMan::HodoPHCMan( void )
  : TObject(),
    m_is_initialized(false)
{
}

//______________________________________________________________________________
HodoPHCMan::~HodoPHCMan( void )
{
  clearMap();
}

//______________________________________________________________________________
void
HodoPHCMan::clearMap( void )
{
  for(PHCPIterator i=m_map.begin(); i!=m_map.end(); i++)
    delete i->second;
  m_map.clear();
}

//______________________________________________________________________________
Bool_t
HodoPHCMan::Initialize( void )
{
  static const TString funcname = "[HodoPHCMan::Initialize]";

  std::ifstream f( m_file_name );

  if(f.fail()){
    std::cerr << funcname << ": file open fail :"
	      << m_file_name << std::endl;
    exit(-1);
  }

  Int_t cid, plid, seg, ud, type, np;
  Double_t par[10];

  while(f.good() ){
    std::string buf;
    std::getline(f,buf);
    if(buf.empty())
      continue;
    if(buf[0]!='#'){
      if( sscanf(buf.c_str(),
                 "%d %d %d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                 &cid, &plid, &seg, &ud, &type, &np, &par[0], &par[1], &par[2],
                 &par[3], &par[4], &par[5], &par[6], &par[7], &par[8],
                 &par[9] ) >= 6 ){
        if(np>10) np=10;
        Int_t key=MakeKey(cid,plid,seg,ud);
        HodoPHCParam *p=new HodoPHCParam(type,np,par);
        if(p) m_map[key]=p;
        else{
          std::cerr << funcname << ": new fail." << std::endl;
          std::cerr << " PlId=" << std::setw(2) << plid
                    << " Seg=" << std::setw(3) << seg
                    << " Ud=" << std::setw(1) << ud << std::endl;
        }
      }
      else{
        std::cerr << funcname << ": Invalid format" << std::endl;
        std::cerr << " ===> " << buf << std::endl;
      } /* if( sscanf... ) */
    } /* if(buf[0]...) */
  } /* while( fgets... ) */

  std::cout << funcname << ": Initialization finished." << std::endl;
  m_is_initialized = true;
  return true;
}

//______________________________________________________________________________
Bool_t
HodoPHCMan::DoCorrection( Int_t cid, Int_t plid, Int_t seg, Int_t ud,
			  Double_t time, Double_t de, Double_t& ctime ) const
{
  HodoPHCParam *map=GetMap(cid,plid,seg,ud);
  if( !map ){
    ctime=time;
    return false;
  }
  ctime=map->DoPHC(time,de);
  return true;
}

//______________________________________________________________________________
Bool_t
HodoPHCMan::DoRCorrection( Int_t cid, Int_t plid, Int_t seg, Int_t ud,
			   Double_t time, Double_t de, Double_t& ctime ) const
{
  HodoPHCParam *map=GetMap(cid,plid,seg,ud);
  if( !map ){
    ctime=time;
    return false;
  }
  ctime=map->DoRPHC(time,de);
  return true;
}

//______________________________________________________________________________
HodoPHCParam*
HodoPHCMan::GetMap( Int_t cid, Int_t plid, Int_t seg, Int_t ud ) const
{
  Int_t key = MakeKey(cid,plid,seg,ud);
  HodoPHCParam *map=0;
  PHCPIterator i=m_map.find(key);
  if( i != m_map.end() ) map=i->second;
  return map;
}
