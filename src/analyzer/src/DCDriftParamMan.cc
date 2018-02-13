// -*- C++ -*-

#include <cstdio>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>

#include <TString.h>

#include "ConfMan.hh"
#include "DCDriftParamMan.hh"
#include "FuncName.hh"

ClassImp(DCDriftParamMan);

namespace
{
  inline UInt_t
  MakeKey( Int_t plane, Double_t wire )
  {
    return (plane<<10) | Int_t(wire);
  }
}

//______________________________________________________________________________
void ConfMan::InitializeDCDriftParamMan( void )
{
  if(name_file_["DRFTPM:"] != ""){
    DCDriftParamMan& gDCDriftParam = DCDriftParamMan::GetInstance();
    gDCDriftParam.SetFileName(name_file_["DRFTPM:"]);
    flag_[kIsGood] = gDCDriftParam.Initialize();
  }else{
    std::cout << "#E ConfMan::"
	      << " File path does not exist in " << name_file_["DRFTPM:"]
	      << std::endl;
    flag_.reset(kIsGood);
  }
}

//______________________________________________________________________________
DCDriftParamMan::DCDriftParamMan( void )
  : TObject(),
    m_is_ready(false),
    m_file_name(),
    m_map()
{
}

//______________________________________________________________________________
DCDriftParamMan::~DCDriftParamMan( void )
{
  clearElements();
}

//______________________________________________________________________________
void
DCDriftParamMan::clearElements( void )
{
  std::map <UInt_t, DCDriftParamRecord *>::iterator itr;
  for( itr=m_map.begin(); itr!=m_map.end(); ++itr ){
    delete itr->second;
    itr->second = 0;
  }
  m_map.clear();
}

//______________________________________________________________________________
Bool_t
DCDriftParamMan::Initialize( void )
{
  const Int_t MaxParam = 10;

  Int_t pid, wid, type, np;
  std::vector<Double_t> q(MaxParam);

  std::ifstream ifs( m_file_name );
  if( !ifs.is_open() ){
    hddaq::cerr << FUNC_NAME << ": file open fail" << std::endl;
    return false;
  }

  TString line;
  while( ifs.good() && line.ReadLine(ifs) ){
    if( line[0] == '#' ) continue;

    if( std::sscanf( line.Data(), "%d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
		     &pid, &wid, &type, &np, &q[0], &q[1], &q[2], &q[3], &q[4],
		     &q[5], &q[6], &q[7], &q[8], &q[9] ) >= 6 ){
      // wid is not used at present
      // reserved for future updates
      UInt_t key = MakeKey(pid,0);
      DCDriftParamRecord *ptr = new DCDriftParamRecord;
      if( ptr ){
	if( np > MaxParam ) np = MaxParam;
	ptr->type=type;
	ptr->np=np;
	ptr->p.resize(np);
	for( Int_t i=0; i<np; ++i ) ptr->p[i] = q[i];
	if( m_map[key] ) delete m_map[key];
	m_map[key] = ptr;
      } else{
	hddaq::cerr << FUNC_NAME << ": new fail. "
		    << " Plane=" << std::setw(3) << pid
		    << " Wire=" << std::setw(3) << wid << std::endl;
      }
    } else {
      hddaq::cerr << FUNC_NAME << ": Bad format => " << std::endl
		  << line << std::endl;
    } /* if( sscanf... */
  } /* while( ifs... */

  hddaq::cout << FUNC_NAME << ": Initialization finished" << std::endl;
  m_is_ready = true;
  return true;
}

//______________________________________________________________________________
DCDriftParamRecord*
DCDriftParamMan::getParameter( Int_t PlaneId, Double_t WireId ) const
{
  WireId=0;
  UInt_t key = MakeKey(PlaneId,WireId);
  std::map<UInt_t, DCDriftParamRecord*>::const_iterator itr = m_map.find(key);
  if( itr != m_map.end() )
    return itr->second;
  else
    return nullptr;
}

//______________________________________________________________________________
Double_t
DCDriftParamMan::DriftLength1( Double_t dt, Double_t vel )
{
  return dt*vel;
}

//______________________________________________________________________________
Double_t
DCDriftParamMan::DriftLength2( Double_t dt, Double_t p1, Double_t p2, Double_t p3,
			       Double_t st, Double_t p5, Double_t p6 )
{
  Double_t dtmax=10.+p2+1./p6;
  Double_t dl;
  Double_t alph=-0.5*p5*st+0.5*st*p3*p5*(p1-st)
    +0.5*p3*p5*(p1-st)*(p1-st);
  if( dt<-10. || dt>dtmax+10. )
    dl = -500.;
  else if( dt<st )
    dl = 0.5*(p5+p3*p5*(p1-st))/st*dt*dt;
  else if( dt<p1 )
    dl = alph+p5*dt-0.5*p3*p5*(p1-dt)*(p1-dt);
  else if( dt<p2 )
    dl = alph+p5*dt;
  else
    dl = alph+p5*dt-0.5*p6*p5*(dt-p2)*(dt-p2);
  return dl;
}

//______________________________________________________________________________
//DL = a0 + a1*Dt + a2*Dt^2
Double_t
DCDriftParamMan::DriftLength3( Double_t dt, Double_t p1, Double_t p2 ,Int_t PlaneId)
{
  if (PlaneId>=1 && PlaneId <=4) {
    if (dt > 130.)
      return 999.9;
  } else if (PlaneId == 5) {
    if (dt > 500.)
      ;
  } else if (PlaneId>=6 && PlaneId <=11) {
    if (dt > 80.)
      return 999.9;
  } else if (PlaneId>=101 && PlaneId <=124) {
    if (dt > 80.)
      return 999.9;
  }

    return dt*p1+dt*dt*p2;
}

//______________________________________________________________________________
Double_t
DCDriftParamMan::DriftLength4( Double_t dt, Double_t p1, Double_t p2 ,Double_t p3)
{
  if (dt < p2)
    return dt*p1;
  else
    return p3*(dt-p2) + p1*p2;
}

//______________________________________________________________________________
Double_t
DCDriftParamMan::DriftLength5( Double_t dt, Double_t p1, Double_t p2 ,Double_t p3, Double_t p4, Double_t p5)
{
  if (dt < p2)
    return dt*p1;
  else if (dt >= p2 && dt < p4)
    return p3*(dt-p2) + p1*p2;
  else
    return p5*(dt-p4) + p3*(p4-p2) + p1*p2;
}

//______________________________________________________________________________
////////////////Now using
//DL = a0 + a1*Dt + a2*Dt^2 + a3*Dt^3 + a4*Dt^4 + a5*Dt^5
Double_t
DCDriftParamMan::DriftLength6( Int_t PlaneId, Double_t dt,
			       Double_t p1, Double_t p2 ,Double_t p3, Double_t p4, Double_t p5 )
{
  //For SDC1
  if (PlaneId>=5 && PlaneId<=10) {
    if (dt<-20 || dt>70) // Tight drift time selection
      //if (dt<-10 || dt>120) // Loose drift time selection
      return 999.9;

    if(dt>65) dt=65.;
    Double_t dl =  dt*p1+dt*dt*p2+p3*pow(dt, 3.0)+p4*pow(dt, 4.0)+p5*pow(dt, 5.0);
    if (dl > 2.5 )
      return 2.5;
    if (dl < 0)
      return 0;
    else
      return dl;
  }//For SDC3&4
  else if (PlaneId>=31 && PlaneId<=42) {
    if ( dt<-20 || dt>400 )
      return 999.9;

    Double_t dl = dt*p1+dt*dt*p2+p3*pow(dt, 3.0)+p4*pow(dt, 4.0)+p5*pow(dt, 5.0);
    if (dl > 10.0 || dt>250 )
      return 10.0;
    if (dl < 0)
      return 0;
    else
      return dl;
  }//For BC3
  else if (PlaneId>=113 && PlaneId<=124) {
    if (dt<-10 || dt>50) // Tight drift time selection
      //if (dt<-10 || dt>80) // Loose drift time selection
      return 999.9;

    if(dt>30) dt=30.;
    Double_t dl =  dt*p1+dt*dt*p2+p3*pow(dt, 3.0)+p4*pow(dt, 4.0)+p5*pow(dt, 5.0);
    if (dl > 1.5 )
      return 1.5;
    if (dl < 0)
      return 0;
    else
      return dl;
  }
  else {
    std::cerr << "DCDriftParamMan::DriftLength6 Invalid planeId="
              << PlaneId << std::endl;
    exit(1);
  }
}

//______________________________________________________________________________
Bool_t
DCDriftParamMan::CalcDrift( Int_t PlaneId, Double_t WireId, Double_t ctime,
			    Double_t & dt, Double_t & dl ) const
{
  DCDriftParamRecord *p = getParameter( PlaneId, WireId );
  if(p){
    dt=p->p[0]-ctime;
    switch(p->type){
    case 1:
      dl=DriftLength1( dt, p->p[1] );
      return true;
    case 2:
      dl=DriftLength2( dt, p->p[1], p->p[2], p->p[3], p->p[4],
		       p->p[5], p->p[6] );
      return true;
    case 3:
      dl=DriftLength3( dt, p->p[1], p->p[2], PlaneId );
      return true;
    case 4:
      dl=DriftLength4( dt, p->p[1], p->p[2], p->p[3] );
      return true;
    case 5:
      dl=DriftLength5( dt, p->p[1], p->p[2], p->p[3], p->p[4], p->p[5] );
      return true;
    case 6:
      dl=DriftLength6( PlaneId, dt, p->p[1], p->p[2], p->p[3], p->p[4], p->p[5] );
      return true;
    default:
      std::cerr << FUNC_NAME << ": No Type. Type="
		<< p->type << std::endl;
      return false;
    }
  }
  else{
#if 0
    std::cerr << FUNC_NAME << ": No record. "
              << " PlaneId=" << std::setw(3) << PlaneId
              << " WireId=" << std::setw(3) << WireId << std::endl;
#endif
    return false;
  }
}
