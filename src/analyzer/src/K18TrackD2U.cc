// -*- C++ -*-

#include "K18TrackD2U.hh"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <std_ostream.hh>

#include "ConfMan.hh"
#include "DCAnalyzer.hh"
#include "DCGeomMan.hh"
#include "DCHit.hh"
#include "DCLocalTrack.hh"
#include "DebugCounter.hh"
#include "DetectorID.hh"
#include "FuncName.hh"
#include "K18Parameters.hh"
#include "K18TransMatrix.hh"
// #include "Minuit.hh"
#include "TrackHit.hh"

ClassImp(K18TrackD2U);

namespace
{
  using namespace K18Parameter;
  const DCGeomMan&      gGeom   = DCGeomMan::GetInstance();
  const K18TransMatrix& gK18Mtx = K18TransMatrix::GetInstance();

  const Double_t LowBand[5] =
    { MinK18InX, MinK18InY, MinK18InU, MinK18InV, MinK18Delta };
  const Double_t UpperBand[5] =
    { MaxK18InX, MaxK18InY, MaxK18InU, MaxK18InV, MaxK18Delta };
  const Double_t LowBandOut[5] =
    { MinK18OutX, MinK18OutY, MinK18OutU, MinK18OutV, MinK18Delta };
  const Double_t UpperBandOut[5] =
    { MaxK18OutX, MaxK18OutY, MaxK18OutU, MaxK18OutV, MaxK18Delta };
}

//______________________________________________________________________________
K18TrackD2U::K18TrackD2U( Double_t local_x, DCLocalTrack *track_out, Double_t p0 )
  : TObject(),
    m_local_x(local_x),
    m_track_out(track_out),
    m_p0( TMath::Abs(p0) ),
    m_Xi(0), m_Yi(0), m_Ui(0), m_Vi(0),
    m_Xo(0), m_Yo(0), m_Uo(0), m_Vo(0),
    m_status(false), m_good_for_analysis(true)
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
K18TrackD2U::~K18TrackD2U( void )
{
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
Bool_t
K18TrackD2U::CalcMomentumD2U( void )
{
  m_status = false;

  if( !gK18Mtx.IsReady() ) return m_status;

  Double_t xi, yi, ui, vi;
  Double_t xo, yo, uo, vo;
  Double_t delta1, delta2;
  xi= -m_track_out->GetX0();
  yi=  m_track_out->GetY0();
  ui=  m_track_out->GetU0();
  vi= -m_track_out->GetV0();
  xo= -m_local_x;

  m_status = gK18Mtx.CalcDeltaD2U( xi, yi, ui, vi,
				   xo, yo, uo, vo,
				   delta1, delta2 );

#if 0
  hddaq::cout << FUNC_NAME << ": after calculation. "
	      << " StatusD2U=" << m_status  << std::endl;
#endif

  if( m_status ){
    // hddaq::cout << "delta1 = " << delta1 << ", delta2 = " << delta2 << std::endl;
    m_delta    = delta1;
    m_delta3rd = delta2;

    m_Xi = -xo;
    m_Yi =  yo;
    m_Ui =  uo;
    m_Vi = -vo;

    m_Xo = m_track_out->GetX0();
    m_Yo = m_track_out->GetY0();
    m_Uo = m_track_out->GetU0();
    m_Vo = m_track_out->GetV0();
  }

  return m_status;
}

//______________________________________________________________________________
ThreeVector
K18TrackD2U::BeamMomentumD2U( void ) const
{
  Double_t u  = m_track_out->GetU0(), v=m_track_out->GetV0();
  Double_t pz = P()/TMath::Sqrt(1.+u*u+v*v);
  return ThreeVector( pz*u, pz*v, pz );
}

//______________________________________________________________________________
Double_t
K18TrackD2U::GetChiSquare( void ) const
{
  return m_track_out->GetChiSquare();
}

//______________________________________________________________________________
Double_t
K18TrackD2U::Xtgt( void ) const
{
  static const Double_t zK18Target = gGeom.GetLocalZ("K18Target");
  return m_track_out->GetU0()*zK18Target+m_track_out->GetX0();
}

//______________________________________________________________________________
Double_t
K18TrackD2U::Ytgt( void ) const
{
  static const Double_t zK18Target = gGeom.GetLocalZ("K18Target");
  return m_track_out->GetV0()*zK18Target+m_track_out->GetY0();
}

//______________________________________________________________________________
Double_t
K18TrackD2U::Utgt( void ) const
{
  return m_track_out->GetU0();
}

//______________________________________________________________________________
Double_t
K18TrackD2U::Vtgt( void ) const
{
  return m_track_out->GetV0();
}

//______________________________________________________________________________
Bool_t
K18TrackD2U::GoodForAnalysis( Bool_t status )
{
  Bool_t pre_status = m_good_for_analysis;
  m_good_for_analysis = status;
  return pre_status;
}

//______________________________________________________________________________
Bool_t
K18TrackD2U::ReCalc( Bool_t applyRecursively )
{
  if( applyRecursively )
    m_track_out->ReCalc(applyRecursively);

  return CalcMomentumD2U();
}
