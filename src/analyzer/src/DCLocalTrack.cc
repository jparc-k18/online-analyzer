// -*- C++ -*-

#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>

#include <std_ostream.hh>

#include "DCAnalyzer.hh"
#include "DCLTrackHit.hh"
#include "DCLocalTrack.hh"
#include "DCGeomMan.hh"
#include "DebugCounter.hh"
#include "DetectorID.hh"
#include "FuncName.hh"
#include "MathTools.hh"
#include "PrintHelper.hh"

ClassImp(DCLocalTrack);

namespace
{
  const DCGeomMan& gGeom = DCGeomMan::GetInstance();
  const Int_t ReservedNumOfHits  = 16;
  const Int_t DCLocalMinNHits    =  4;
  const Int_t DCLocalMinNHitsVXU =  2;// for SSD
  const Int_t MaxIteration       = 100;// for Honeycomb
  const Double_t MaxChisqrDiff   = 1.0e-3;
}

//______________________________________________________________________________
DCLocalTrack::DCLocalTrack( void )
  : TObject(),
    m_is_fitted(false),
    m_is_calculated(false),
    m_Ax(0.), m_Ay(0.), m_Au(0.), m_Av(0.),
    m_Chix(0.), m_Chiy(0.), m_Chiu(0.), m_Chiv(0.),
    m_x0(0.), m_y0(0.),
    m_u0(0.), m_v0(0.),
    m_a(0.),  m_b(0.),
    m_chisqr(1.e+10),
    m_good_for_tracking(true),
    m_de(0.),
    m_chisqr1st(1.e+10),
    m_n_iteration(0)
{
  m_hit_array.reserve( ReservedNumOfHits );
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
DCLocalTrack::~DCLocalTrack( void )
{
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
void
DCLocalTrack::AddHit( DCLTrackHit *hitp )
{
  if( hitp )
    m_hit_array.push_back( hitp );
}

//______________________________________________________________________________
void
DCLocalTrack::Calculate( void )
{
  if( IsCalculated() ){
    hddaq::cerr << "#W " << FUNC_NAME << " "
		<< "already called" << std::endl;
    return;
  }

  const Int_t n = m_hit_array.size();
  for( Int_t i=0; i<n; ++i ){
    DCLTrackHit *hitp = m_hit_array[i];
    Double_t z0 = hitp->GetZ();
    hitp->SetCalPosition( GetX(z0), GetY(z0) );
    hitp->SetCalUV( m_u0, m_v0 );

    // for Honeycomb
    if( !hitp->IsHoneycomb() )
      continue;
    Double_t scal = hitp->GetLocalCalPos();
    Double_t wp   = hitp->GetWirePosition();
    Double_t dl   = hitp->GetDriftLength();
    Double_t ss   = scal-wp>0 ? wp+dl : wp-dl;
    hitp->SetLocalHitPos( ss );
  }

  m_is_calculated = true;
}

//______________________________________________________________________________
Int_t
DCLocalTrack::GetNDF( void ) const
{
  Int_t ndf = 0;
  for( Int_t i=0, n=m_hit_array.size(); i<n; ++i ){
    if( m_hit_array[i] ) ++ndf;
  }
  return ndf-4;
}

//______________________________________________________________________________
DCLTrackHit*
DCLocalTrack::GetHit( Int_t i ) const
{
  return m_hit_array[i];
}

//______________________________________________________________________________
DCLTrackHit*
DCLocalTrack::GetHitOfLayerNumber( Int_t lnum ) const
{
  for( Int_t i=0, n=m_hit_array.size(); i<n; ++i ){
    if( m_hit_array[i]->GetLayer()==lnum )
      return m_hit_array[i];
  }
  return 0;
}

//______________________________________________________________________________
void
DCLocalTrack::DeleteNullHit( void )
{
  for( Int_t i=0, n=m_hit_array.size(); i<n; ++i ){
    DCLTrackHit *hitp = m_hit_array[i];
    if( !hitp ){
      hddaq::cout << FUNC_NAME << " "
		  << "null hit is deleted" << std::endl;
      m_hit_array.erase( m_hit_array.begin()+i );
      --i;
    }
  }
}

//______________________________________________________________________________
bool
DCLocalTrack::DoFit( void )
{
  if( IsFitted() ){
    hddaq::cerr << "#W " << FUNC_NAME << " "
		<< "already called" << std::endl;
    return false;
  }

  DeleteNullHit();

  const Int_t n = m_hit_array.size();
  if( n < DCLocalMinNHits ){
    hddaq::cout << "#D " << FUNC_NAME << " "
		<< "the number of layers is too small : " << n << std::endl;
    return false;
  }

  const Int_t nItr = HasHoneycomb() ? MaxIteration : 1;

  Double_t prev_chisqr = m_chisqr;
  std::vector <Double_t> zw(n), z(n), sw(n),
    w(n), s(n), ct(n), st(n), coss(n);
  std::vector<bool> honeycomb(n);
  for( Int_t iItr=0; iItr<nItr; ++iItr ){
    for( Int_t i=0; i<n; ++i ){
      DCLTrackHit *hitp = m_hit_array[i];
      Int_t lnum = hitp->GetLayer();
      honeycomb[i] = hitp->IsHoneycomb();
      sw[i] = hitp->GetWirePosition();
      zw[i] = hitp->GetZ();
      Double_t ww = gGeom.GetResolution( lnum );
      w[i] = 1./(ww*ww);
      Double_t aa = hitp->GetTiltAngle()*math::Deg2Rad();
      ct[i] = TMath::Cos(aa); st[i] = TMath::Sin(aa);
      Double_t ss = hitp->GetLocalHitPos();
      // Double_t dl = ss-sw[i];
      Double_t dl = hitp->GetDriftLength();
      Double_t dsdz = m_u0*TMath::Cos(aa)+m_v0*TMath::Sin(aa);
      Double_t dcos = 1./TMath::Sqrt( 1. + dsdz*dsdz );
      Double_t dsin = dsdz/TMath::Sqrt( 1. + dsdz*dsdz );
      // Double_t dcos = TMath::Cos( TMath::ATan(dsdz) );
      // Double_t dsin = TMath::Sin( TMath::ATan(dsdz) );
      Double_t ds = dl * dcos;
      Double_t dz = dl * dsin;
      Double_t scal = iItr==0 ? ss : GetS(z[i],aa);
      if( honeycomb[i] ){
	coss[i] = dcos;
	// s[i] = sw[i]+ds;
	// z[i] = zw[i]-dz;
	s[i] = scal-sw[i]>0 ? sw[i]+ds : sw[i]-ds;
	z[i] = scal-sw[i]>0 ? zw[i]-dz : zw[i]+dz;
      }else{
	coss[i] = 1.;
	s[i] = ss;
	z[i] = zw[i];
      }
    }

    Double_t x0, u0, y0, v0;
    if( !math::SolveGaussJordan( z, w, s, ct, st,
				 x0, u0, y0, v0 ) ){
      hddaq::cerr << FUNC_NAME << " Fitting failed" << std::endl;
      return false;
    }

    Double_t chisqr = 0.;
    Double_t de     = 0.;
    for( Int_t i=0; i<n; ++i ){
      Double_t scal = (x0+u0*z[i])*ct[i]+(y0+v0*z[i])*st[i];
      Double_t res = (s[i]-scal)*coss[i];
      //Double_t res = (s[i]-sw[i]) - (scal-sw[i])*coss[i];
      //Double_t ss   = sw[i]+(s[i]-sw[i])/coss[i];
      //Double_t res  = honeycomb[i] ? (ss-scal)*coss[i] : s[i]-scal;
      chisqr += w[i]*res*res;
    }
    chisqr /= GetNDF();

    /* for SSD */
    if( n==4 ){
      chisqr = 0.;
      for( Int_t i=0; i<n; ++i ){
	DCLTrackHit *hitp = m_hit_array[i];
	if( !hitp ) continue;
	chisqr += hitp->GetChisquare();
	de     += hitp->GetDe();
      }
      chisqr /= 4;
    }

    if( iItr==0 )
      m_chisqr1st = chisqr;

    // if worse, not update
    if( prev_chisqr-chisqr>0. ){
      m_x0 = x0;
      m_y0 = y0;
      m_u0 = u0;
      m_v0 = v0;
      m_chisqr = chisqr;
      m_de     = de;
    }

    // judge convergence
    if( prev_chisqr-chisqr<MaxChisqrDiff ){
#if 0
      // if( chisqr<200. && GetTheta()>4. )
      if( chisqr<20. )
      {
	if( iItr==0 ) hddaq::cout << "=============" << std::endl;
	hddaq::cout << FUNC_NAME << " NIteration : " << iItr << " "
		    << "chisqr = " << std::setw(10) << std::setprecision(4)
		    << m_chisqr << " "
		    << "diff = " << std::setw(20) << std::left
		    << m_chisqr-m_chisqr1st << " ndf = " << GetNDF() << std::endl;
      }
#endif
      m_n_iteration = iItr;
      break;
    }

    prev_chisqr = chisqr;
  }

  m_is_fitted = true;
  return true;
}

//______________________________________________________________________________
bool
DCLocalTrack::DoFitBcSdc( void )
{
  static const Double_t zK18tgt = gGeom.GetLocalZ("K18Target");
  static const Double_t zTgt    = gGeom.GetLocalZ("Target");

  if( IsFitted() ){
    hddaq::cerr << "#W " << FUNC_NAME << " "
		<< "already called" << std::endl;
    return false;
  }

  DeleteNullHit();

  Int_t n = m_hit_array.size();
  if( n < DCLocalMinNHits ) return false;

  std::vector <Double_t> z, w, s, ct, st;
  z.reserve(n); w.reserve(n); s.reserve(n);
  ct.reserve(n); st.reserve(n);

  for( Int_t i=0; i<n; ++i ){
    DCLTrackHit *hitp = m_hit_array[i];
    Int_t lnum = hitp->GetLayer();
    Double_t ww = gGeom.GetResolution( lnum );
    Double_t zz = hitp->GetZ();
    if ( lnum>=113&&lnum<=124 ){ // BcOut
      zz -= zK18tgt - zTgt;
    }
    if ( lnum>=11&&lnum<=18 ){ // SsdIn/Out
      zz -= zK18tgt - zTgt;
    }
    Double_t aa = hitp->GetTiltAngle()*math::Deg2Rad();
    z.push_back( zz ); w.push_back( 1./(ww*ww) );
    s.push_back( hitp->GetLocalHitPos() );
    ct.push_back( cos(aa) ); st.push_back( sin(aa) );
  }

  if( !math::SolveGaussJordan( z, w, s, ct, st,
			       m_x0, m_u0, m_y0, m_v0 ) ){
    hddaq::cerr << FUNC_NAME << " Fitting fails" << std::endl;
    return false;
  }

  Double_t chisqr = 0.;
  for( Int_t i=0; i<n; ++i ){
    Double_t ww=w[i], zz=z[i];
    Double_t scal=GetX(zz)*ct[i]+GetY(zz)*st[i];
    chisqr += ww*(s[i]-scal)*(s[i]-scal);
  }
  chisqr /= GetNDF();
  m_chisqr = chisqr;

  for( Int_t i=0; i<n; ++i ){
    DCLTrackHit *hitp = m_hit_array[i];
    if( hitp ){
      Int_t lnum = hitp->GetLayer();
      Double_t zz = hitp->GetZ();
      if ( (lnum>=113&&lnum<=124) ){ // BcOut
        zz -= zK18tgt - zTgt;
      }
      if ( (lnum>=11&&lnum<=18) ){ // SsdIn/Out
        zz -= zK18tgt - zTgt;
      }
      hitp->SetCalPosition( GetX(zz), GetY(zz) );
    }
  }

  m_is_fitted = true;
  return true;
}

//______________________________________________________________________________
bool
DCLocalTrack::DoFitVXU( void )
{
  if( IsFitted() ){
    hddaq::cerr << "#W " << FUNC_NAME << " "
		<< "already called" << std::endl;
    return false;
  }

  DeleteNullHit();

  const Int_t n = m_hit_array.size();
  if( n<DCLocalMinNHitsVXU ) return false;

  Double_t w[n+1],z[n+1],x[n+1];

  for( Int_t i=0; i<n; ++i ){
    DCLTrackHit *hitp = m_hit_array[i];
    Int_t lnum = hitp->GetLayer();
    w[i] = gGeom.GetResolution( lnum );
    z[i] = hitp->GetZ();
    x[i] = hitp->GetLocalHitPos();
#if 0
    hddaq::cout << "" << std::endl;
    hddaq::cout << "**********" << std::endl;
    hddaq::cout << std::setw(10) << "layer = " << lnum
		<< std::setw(10) << "wire  = " << hitp->GetWire() << " "
		<< std::setw(20) << "WirePosition = "<<hitp->GetWirePosition() << " "
		<< std::setw(20) << "DriftLength = "<<hitp->GetDriftLength() << " "
		<< std::setw(20) << "hit position = "<<hitp->GetLocalHitPos()<< " "
		<< std::endl;
    hddaq::cout << "**********" << std::endl;
    hddaq::cout << "" << std::endl;
#endif
  }

  Double_t A=0, B=0, C=0, D=0, E=0;// <-Add!!
  for( Int_t i=0; i<n; ++i ){
    A += z[i]/(w[i]*w[i]);
    B += 1/(w[i]*w[i]);
    C += x[i]/(w[i]*w[i]);
    D += z[i]*z[i]/(w[i]*w[i]);
    E += x[i]*z[i]/(w[i]*w[i]);
  }

  m_a = (E*B-C*A)/(D*B-A*A);
  m_b = (D*C-E*A)/(D*B-A*A);

  Double_t chisqr = 0.;
  for( Int_t i=0; i<n; ++i ){
    chisqr += (x[i]-m_a*z[i]-m_b)*(x[i]-m_a*z[i]-m_b)/(w[i]*w[i]);
  }

  if(n==2) chisqr  = 0.;
  else     chisqr /= n-2.;
  m_chisqr = chisqr;

  for( Int_t i=0; i<n; ++i ){
    DCLTrackHit *hitp = m_hit_array[i];
    if( hitp ){
      Double_t zz = hitp->GetZ();
      hitp->SetLocalCalPosVXU( m_a*zz+m_b );
    }
  }

  m_is_fitted = true;
  return true;
}

//______________________________________________________________________________
bool
DCLocalTrack::FindLayer( Int_t layer ) const
{
  for( Int_t i=0, n=m_hit_array.size(); i<n; ++i ){
    DCLTrackHit *hitp = m_hit_array[i];
    if( hitp && layer == hitp->GetLayer() )
      return true;
  }
  // hddaq::cerr << "#W " << FUNC_NAME << " "
  // 	      << "no such layer : " << layer << std::endl;
  return false;
}

//______________________________________________________________________________
Double_t
DCLocalTrack::GetWire( Int_t layer ) const
{
  for( Int_t i=0, n=m_hit_array.size(); i<n; ++i ){
    DCLTrackHit *hitp = m_hit_array[i];
    if( hitp && layer == hitp->GetLayer() )
      return hitp->GetWire();
  }
  // hddaq::cerr << "#W " << FUNC_NAME << " "
  // 	      << "no such wire : " << layer << std::endl;
  return math::nan();
}

//______________________________________________________________________________
Double_t
DCLocalTrack::GetDifVXU( void ) const
{
  static const Double_t Cu = cos(  15.*math::Deg2Rad() );
  static const Double_t Cv = cos( -15.*math::Deg2Rad() );
  static const Double_t Cx = cos(   0.*math::Deg2Rad() );

  return
    pow( m_Av/Cv - m_Ax/Cx, 2 ) +
    pow( m_Ax/Cx - m_Au/Cu, 2 ) +
    pow( m_Au/Cu - m_Av/Cv, 2 );
}

//______________________________________________________________________________
Double_t
DCLocalTrack::GetDifVXUSDC34( void ) const
{
  static const Double_t Cu = cos(  30.*math::Deg2Rad() );
  static const Double_t Cv = cos( -30.*math::Deg2Rad() );
  static const Double_t Cx = cos(   0.*math::Deg2Rad() );

  return
    pow( m_Av/Cv - m_Ax/Cx, 2 ) +
    pow( m_Ax/Cx - m_Au/Cu, 2 ) +
    pow( m_Au/Cu - m_Av/Cv, 2 );
}

//______________________________________________________________________________
Double_t
DCLocalTrack::GetTheta( void ) const
{
  Double_t cost = 1./TMath::Sqrt(1.+m_u0*m_u0+m_v0*m_v0);
  return std::acos(cost)*math::Rad2Deg();
}

//______________________________________________________________________________
bool
DCLocalTrack::HasHoneycomb( void ) const
{
  for( Int_t i=0, n = m_hit_array.size(); i<n; ++i ){
    DCLTrackHit *hitp = m_hit_array[i];
    if( !hitp ) continue;
    if( hitp->IsHoneycomb() ) return true;
  }
  return false;
}

//______________________________________________________________________________
bool
DCLocalTrack::ReCalc( bool applyRecursively )
{
  Int_t n = m_hit_array.size();
  for( Int_t i=0; i<n; ++i ){
    DCLTrackHit *hitp = m_hit_array[i];
    if( hitp ) hitp->ReCalc( applyRecursively );
  }

  bool status = DoFit();
  if( !status ){
    hddaq::cerr << "#W " << FUNC_NAME << " "
		<< "Recalculation fails" << std::endl;
  }

  return status;
}

//______________________________________________________________________________
void
DCLocalTrack::Print( Option_t* option ) const
{
  PrintHelper helper( 3, std::ios::fixed, hddaq::cout );

  const Int_t w = 8;
  hddaq::cout << FUNC_NAME << " " << std::endl
	      << " X0 : " << std::setw(w) << std::left << m_x0
	      << " Y0 : " << std::setw(w) << std::left << m_y0
	      << " U0 : " << std::setw(w) << std::left << m_u0
	      << " V0 : " << std::setw(w) << std::left << m_v0;
  helper.setf( std::ios::scientific );
  hddaq::cout << " Chisqr : " << std::setw(w) << m_chisqr << std::endl;
  helper.setf( std::ios::fixed );
  const Int_t n = m_hit_array.size();
  for( Int_t i=0; i<n; ++i ){
    DCLTrackHit *hitp = m_hit_array[i];
    if( !hitp ) continue;
    Int_t lnum = hitp->GetLayer();
    Double_t zz = hitp->GetZ();
    Double_t s  = hitp->GetLocalHitPos();
    Double_t res = hitp->GetResidual();
    // Double_t aa = hitp->GetTiltAngle()*math::Deg2Rad();
    // Double_t scal=GetX(zz)*cos(aa)+GetY(zz)*sin(aa);
    const std::string& h = hitp->IsHoneycomb() ? "+" : "-";
    hddaq::cout << "[" << std::setw(2) << i << "]"
		<< " #"  << std::setw(2) << lnum << h
		<< " S " << std::setw(w) << s
		<< " ( " << std::setw(w) << GetX(zz)
		<< ", "  << std::setw(w) << GetY(zz)
		<< ", "  << std::setw(w) << zz
		<< " )"
		<< " " << std::setw(w) << s
		<< " -> " << std::setw(w) << res << std::endl;
    // << " -> " << std::setw(w) << s-scal << std::endl;
  }
  hddaq::cout << std::endl;
}

//______________________________________________________________________________
void
DCLocalTrack::PrintVXU( const std::string& arg ) const
{
  PrintHelper helper( 3, std::ios::fixed );

  const Int_t w = 10;
  hddaq::cout << FUNC_NAME << " " << arg << std::endl
	      << "(Local X = A*z + B) "
	      << " A : " << std::setw(w) << m_a
	      << " B : " << std::setw(w) << m_b;
  helper.setf( std::ios::scientific );
  hddaq::cout << " Chisqr : " << std::setw(w) << m_chisqr << std::endl;
  helper.setf( std::ios::fixed );

  const Int_t n = m_hit_array.size();
  for( Int_t i=0; i<n; ++i ){
    const DCLTrackHit * const hitp = m_hit_array[i];
    if( !hitp ) continue;
    Int_t    lnum = hitp->GetLayer();
    Double_t zz   = hitp->GetZ();
    Double_t s    = hitp->GetLocalHitPos();
    Double_t res  = hitp->GetResidualVXU();
    const std::string& h = hitp->IsHoneycomb() ? "+" : "-";
    hddaq::cout << "[" << std::setw(2) << i << "] "
		<< " #"  << std::setw(2) << lnum << h
		<< " S " << std::setw(w) << s
		<< " ( " << std::setw(w) << (m_a*zz+m_b)
		<< ", "  << std::setw(w) << zz
		<< " )"
		<< " " << std::setw(w) << s
		<< " -> " << std::setw(w) << res << std::endl;
  }
  hddaq::cout << std::endl;
}
