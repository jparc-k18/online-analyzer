// -*- C++ -*-

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <fstream>
#include <cstdlib>

#include "ConfMan.hh"
#include "HodoParamMan.hh"
#include "FuncName.hh"

ClassImp(HodoParamMan);

namespace
{
  const Int_t SegMask  = 0x03FF;
  const Int_t CidMask  = 0x00FF;
  const Int_t PlidMask = 0x00FF;
  const Int_t UdMask   = 0x0003;

  const Int_t SegShift  =  0;
  const Int_t CidShift  = 11;
  const Int_t PlidShift = 19;
  const Int_t UdShift   = 27;

  inline Int_t MakeKey( Int_t cid, Int_t pl, Int_t seg, Int_t ud )
  {
    return (((cid&CidMask)<<CidShift) | ((pl&PlidMask)<<PlidShift)
	    | ((seg&SegMask)<<SegShift) | ((ud&UdMask)<<UdShift) );
  }
}

//______________________________________________________________________________
void
ConfMan::InitializeHodoParamMan( void )
{
  if(name_file_["HDPRM:"] != ""){
    HodoParamMan& gHodoParam = HodoParamMan::GetInstance();
    gHodoParam.SetFileName(name_file_["HDPRM:"]);
    flag_[kIsGood] = gHodoParam.Initialize();
  }else{
    hddaq::cout << "#E ConfMan::"
		<< " File path does not exist in " << name_file_["HDPRM:"]
		<< std::endl;
    flag_.reset(kIsGood);
  }
}

//______________________________________________________________________________
HodoParamMan::HodoParamMan( void )
  : TObject(),
    m_is_initialized(false),
    m_file_name(),
    m_TPContainer(),
    m_APContainer()
{
}

//______________________________________________________________________________
HodoParamMan::~HodoParamMan( void )
{
  clearACont();
  clearTCont();
}

//______________________________________________________________________________
void
HodoParamMan::clearACont( void )
{
  for( AIterator i=m_APContainer.begin(); i!=m_APContainer.end(); ++i )
    delete i->second;
  m_APContainer.clear();
}

//______________________________________________________________________________
void
HodoParamMan::clearTCont( void )
{
  for( TIterator i=m_TPContainer.begin(); i!=m_TPContainer.end(); ++i )
    delete i->second;
  m_TPContainer.clear();
}

//______________________________________________________________________________
Bool_t
HodoParamMan::Initialize( void )
{
  clearACont(); clearTCont();

  std::ifstream ifs( m_file_name );
  if( !ifs.is_open() ){
    std::cerr << FUNC_NAME << ": file open fail" << std::endl;
    return false;
  }

  Int_t cid, plid, seg, at, ud;
  Double_t p0, p1;
  Int_t invalid=0;
  TString line;
  while( ifs.good() && line.ReadLine(ifs) ){
    ++invalid;
    if( line[0]=='#' ) continue;

    if( std::sscanf( line.Data(), "%d %d %d %d %d %lf %lf",
		     &cid, &plid, &seg, &at, &ud, &p0, &p1 ) == 7 ){
      Int_t key = MakeKey( cid, plid, seg, ud );
      if( at==0 ){ /* ADC */
	HodoAParam *pa = new HodoAParam(p0,p1);
	if(pa) m_APContainer[key]=pa;
	else{
	  std::cerr << FUNC_NAME << ": new fail (A)"
	  	    << " CId=" << std::setw(3) << cid
	  	    << " PlId=" << std::setw(2) << plid
	  	    << " Seg=" << std::setw(3) << seg
	  	    << " Ud=" << std::setw(1) << ud << std::endl;
	}
      }
      else if(at==1){ /* TDC */
	HodoTParam *ta=new HodoTParam(p0,p1);
	if(ta){
	  m_TPContainer[key]=ta;
	}
	else{
	  std::cerr << FUNC_NAME << ": new fail (T)"
	  	    << " CId=" << std::setw(3) << cid
	  	    << " PlId=" << std::setw(2) << plid
	  	    << " Seg=" << std::setw(3) << seg
	  	    << " Ud=" << std::setw(1) << ud << std::endl;
	}
      }
      else{
	std::cerr << FUNC_NAME << ": Invalid Input" << std::endl;
	std::cerr << " ===> " << line <<" "
		  << invalid <<"a"<< std::endl;
      } /* if(at...) */
    }
    else {
      std::cerr << FUNC_NAME << ": Invalid Input" << std::endl;
      std::cerr << " ===> " << line <<" "
      		<< invalid <<"b"<< std::endl;
    } /* if( sscanf... ) */
  } /* while( ifs... ) */

  hddaq::cout << FUNC_NAME << ": Initialization finished" << std::endl;
  m_is_initialized = true;
  return true;
}

//______________________________________________________________________________
Bool_t
HodoParamMan::GetTime( Int_t cid, Int_t plid, Int_t seg, Int_t ud,
		       Int_t tdc, Double_t &time ) const
{
  HodoTParam *map=GetTmap(cid,plid,seg,ud);
  if(!map) return false;
  time=map->time(tdc);
  return true;
}

//______________________________________________________________________________
Bool_t
HodoParamMan::GetDe( Int_t cid, Int_t plid, Int_t seg, Int_t ud,
		     Int_t adc, Double_t &de ) const
{
  HodoAParam *map = GetAmap(cid,plid,seg,ud);
  if(!map) return false;
  de=map->de(adc);
  return true;
}

//______________________________________________________________________________
HodoTParam*
HodoParamMan::GetTmap( Int_t cid, Int_t plid, Int_t seg, Int_t ud ) const
{
  Int_t key = MakeKey(cid,plid,seg,ud);
  HodoTParam *map=0;
  TIterator i=m_TPContainer.find(key);
  if( i!=m_TPContainer.end() ) map=i->second;
  return map;
}

//______________________________________________________________________________
HodoAParam*
HodoParamMan::GetAmap( Int_t cid, Int_t plid, Int_t seg, Int_t ud ) const
{
  Int_t key=MakeKey(cid,plid,seg,ud);
  HodoAParam *map=0;
  AIterator i=m_APContainer.find(key);
  if( i!=m_APContainer.end() ) map=i->second;
  return map;
}
