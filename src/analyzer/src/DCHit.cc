// -*- C++ -*-

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>

#include <TF1.h>
#include <TGraph.h>

#include <std_ostream.hh>

#include "DCDriftParamMan.hh"
#include "DCGeomMan.hh"
#include "DCHit.hh"
#include "DCParameters.hh"
#include "DCTdcCalibMan.hh"
#include "DCLTrackHit.hh"
#include "DebugCounter.hh"
#include "DetectorID.hh"
#include "FuncName.hh"
#include "MathTools.hh"
// #include "SsdParamMan.hh"

ClassImp(DCHit);

namespace
{
  const DCGeomMan&       gGeom  = DCGeomMan::GetInstance();
  const DCTdcCalibMan&   gTdc   = DCTdcCalibMan::GetInstance();
  const DCDriftParamMan& gDrift = DCDriftParamMan::GetInstance();
  // const SsdParamMan&     gSsd   = SsdParamMan::GetInstance();
  const Bool_t SelectTDC1st = true;
}

//______________________________________________________________________________
DCHit::DCHit( void )
  : TObject(),
    m_layer(-1), m_wire(-1),
    m_wpos(-9999.), m_angle(0.),
    m_cluster_size(0.),
    m_mwpc_flag(false),
    // m_is_ssd(false),
    m_zero_suppressed(false),
    m_time_corrected(false),
    m_good_waveform(false),
    m_pedestal(-999),
    m_peak_height(-999),
    m_peak_position(-999),
    m_deviation(-999.),
    m_amplitude(-999.),
    m_peak_time(-999.),
    m_adc_sum(-999.),
    m_de(-999.),
    m_rms(-999.),
    m_chisqr(9999.),
    m_belong_kaon(false)
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
DCHit::DCHit( Int_t layer )
  : TObject(),
    m_layer( layer ), m_wire(-1),
    m_wpos(-9999.), m_angle(0.),
    m_cluster_size(0.),
    m_mwpc_flag(false),
    // m_is_ssd(false),
    m_zero_suppressed(false),
    m_time_corrected(false),
    m_good_waveform(false),
    m_pedestal(-999),
    m_peak_height(-999),
    m_peak_position(-999),
    m_deviation(-999.),
    m_amplitude(-999.),
    m_peak_time(-999.),
    m_adc_sum(-999.),
    m_de(-999.),
    m_rms(-999.),
    m_chisqr(-999.),
    m_belong_kaon(false)
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
DCHit::DCHit( Int_t layer, Double_t wire )
  : TObject(),
    m_layer(layer), m_wire(wire),
    m_wpos(-9999.), m_angle(0.),
    m_cluster_size(0.),
    m_mwpc_flag(false),
    // m_is_ssd(false),
    m_zero_suppressed(false),
    m_time_corrected(false),
    m_good_waveform(false),
    m_pedestal(-999),
    m_peak_height(-999),
    m_peak_position(-999),
    m_deviation(-999.),
    m_amplitude(-999.),
    m_peak_time(-999.),
    m_adc_sum(-999.),
    m_de(-999.),
    m_rms(-999.),
    m_chisqr(-999.),
    m_belong_kaon(false)
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
DCHit::~DCHit( void )
{
  ClearRegisteredHits();
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
void
DCHit::SetTdcVal( Int_t tdc )
{
  m_tdc.push_back(tdc);
  m_belong_track.push_back(false);
  m_dl_range.push_back(false);
}

//______________________________________________________________________________
void
DCHit::SetAdcVal( Int_t adc )
{
  m_adc.push_back(adc);
}

//______________________________________________________________________________
void
DCHit::ClearRegisteredHits( void )
{
  for( Int_t i=0, n=m_register_container.size(); i<n; ++i ){
    delete m_register_container[i];
  }
}

//______________________________________________________________________________
Bool_t
DCHit::CalcDCObservables( void )
{
  if( !gGeom.IsReady() ||
      !gTdc.IsReady()  ||
      !gDrift.IsReady() )
    return false;

  m_wpos  = gGeom.CalcWirePosition( m_layer,m_wire );
  m_angle = gGeom.GetTiltAngle( m_layer );
  m_z     = gGeom.GetLocalZ( m_layer );

  Bool_t status = true;
  Int_t  nhtdc = m_tdc.size();
  for ( Int_t i=0; i<nhtdc; ++i ) {
    Double_t ctime;
    if( !gTdc.GetTime( m_layer, m_wire, m_tdc[i], ctime ) )
      return false;

    Double_t dtime, dlength;
    if( !gDrift.CalcDrift( m_layer, m_wire, ctime, dtime, dlength ) )
      status = false;

    m_dt.push_back( dtime );
    m_dl.push_back( dlength );

    switch( m_layer ){
      // BC3,4
    case 113: case 114: case 115: case 116: case 117: case 118:
    case 119: case 120: case 121: case 122: case 123: case 124:
      if( MinDLBc[m_layer-100]<m_dl[i] && m_dl[i]<MaxDLBc[m_layer-100] )
	m_dl_range[i] = true;
      break;

      // SDC1,2,3
    case 1: case 2: case 3: case 4: case 5: case 6:
    case 31: case 32: case 33: case 34:
    case 35: case 36: case 37: case 38:
      if( MinDLSdc[m_layer]<m_dl[i] && m_dl[i]<MaxDLSdc[m_layer] )
	m_dl_range[i] = true;
      break;
    default:
      hddaq::cout << "#E " << FUNC_NAME << " "
		  << "invalid layer id : " << m_layer << std::endl;
      status = false;
      break;
    }
  }

  if( SelectTDC1st && status ){
    Int_t    tdc1st = 0;
    Double_t dl1st  = -1.;
    Double_t dt1st  = -1.;
    for( Int_t i=0; i<nhtdc; ++i ){
      if( tdc1st < m_tdc[i]
	  /* && m_dl_range[i] */ ){
	tdc1st = m_tdc[i];
	dl1st  = m_dl[i];
	dt1st  = m_dt[i];
      }
    }
    if( tdc1st>0 ){
      m_tdc.clear();
      m_dt.clear();
      m_dl.clear();
      m_tdc.push_back( tdc1st );
      m_dt.push_back( dt1st );
      m_dl.push_back( dl1st );
    } else {
      m_tdc.clear();
      m_dt.clear();
      m_dl.clear();
    }
  }

  return status;
}

//______________________________________________________________________________
Bool_t
DCHit::CalcMWPCObservables( void )
{
  if( !gGeom.IsReady() || !gTdc.IsReady() )
    return false;

  m_angle = gGeom.GetTiltAngle( m_layer );
  m_wpos  = gGeom.CalcWirePosition( m_layer, m_wire );
  m_z     = gGeom.GetLocalZ( m_layer );

  Bool_t status = true;
  for( std::size_t i=0, nhtdc=m_tdc.size(); i<nhtdc; ++i ){
    if( m_dt.size()<nhtdc ){
      Double_t ctime;
      if( !gTdc.GetTime( m_layer, m_wire, m_tdc[i], ctime ) ) return false;

      m_dt.push_back( ctime );
      m_dl.push_back( 0. );

      Double_t ctt; // tdc value of trailing edge --> time
      if( !gTdc.GetTime( m_layer, m_wire, m_trailing[i], ctt ) ) return false;
      m_trailing_time.push_back( ctt );
    }

    if( m_dt[i]>MinDLBc[m_layer-100] && m_dt[i]<MaxDLBc[m_layer-100] )
      m_dl_range[i] = true;
    else
      status = false;
  }

  return status;
}

//______________________________________________________________________________
Bool_t
DCHit::CalcFiberObservables( void )
{
  if( !gGeom.IsReady() ) return false;

  m_angle = gGeom.GetTiltAngle( m_layer );
  m_z     = gGeom.GetLocalZ( m_layer );

  for( std::size_t i=0, n=m_tdc.size(); i<n; ++i ){
    if ( m_dt.size()<n ){
      m_dt.push_back( m_tdc[i] );
      m_dl.push_back( 0. );
    }
    m_dl_range[i] = true;
  }

  return true;
}

//______________________________________________________________________________
Bool_t
DCHit::CalcSsdObservables( void )
{
  if( !gGeom.IsReady()
      // || !gSsd.IsReady()
      )
    return false;

  m_angle = gGeom.GetTiltAngle( m_layer );
  m_wpos  = gGeom.CalcWirePosition( m_layer, m_wire );
  m_z     = gGeom.GetLocalZ( m_layer );

  std::size_t nhadc = m_adc.size();
  if( nhadc!=NumOfSamplesSSD ){
    hddaq::cerr << "#W " << FUNC_NAME << " the number of sample is wrong " << std::endl
		<< "   layer#" << m_layer << " segment#" << m_wire
		<< " [" << nhadc << "/" << NumOfSamplesSSD << "]" << std::endl;
    return false;
  }

  if( m_trailing.size()>0 ) m_zero_suppressed = true;

  Double_t pedestal = m_adc[0];
  std::vector<Double_t> dE(nhadc);
  std::vector<Double_t> rms(nhadc);
  for( std::size_t i=0; i<nhadc; ++i ){
    if ( m_dt.size()<nhadc ){
      m_dt.push_back( m_tdc[i] );
      m_dl.push_back( 0. );
    }
    m_dl_range[i] = true;

    // if( m_adc[i]<pedestal )
    //   pedestal = m_adc[i];

    // if( !gSsd.GetDe( m_layer, m_wire, i, m_adc[i], dE[i] ) ){
    //   hddaq::cerr << FUNC_NAME << " : something is wrong at GetDe("
    // 		  << m_layer   << ", " << m_wire << ", " << i << ", "
    // 		  << m_adc[i]  << ", " << dE[i] << ")"
    // 		  << std::endl;
    //   return false;
    // }
    dE[i] = m_adc[i];

    // if( !gSsd.GetRms( m_layer,m_wire, i, rms[i] ) ){
    //   hddaq::cerr << FUNC_NAME << " : something is wrong at GetRms("
    // 		  << m_layer   << ", " << m_wire << ", " << i << ", "
    // 		  << rms[i]   << ")"
    // 		  <<std::endl;
    //   return false;
    // }
    rms[i] = 1.;

    dE[i] -= pedestal;
    m_waveform.push_back( dE[i] );
    m_time.push_back( m_tdc[i]*SamplingIntervalSSD );

    if( m_adc[i]>m_peak_height ){
      m_peak_time     = m_time[i];
      m_rms           = rms[i];
      m_peak_height   = m_adc[i];
      m_peak_position = m_tdc[i];
    }
  }

  m_pedestal  = pedestal;
  m_adc_sum   = math::Accumulate(dE);
  m_deviation = math::Deviation(dE);

  /*** SSD Waveform Fitting ***************************
   *
   *  f(x) = a * (x-b) * exp(-(x-b)/c)
   *    a : scale factor
   *    b : start timing
   *    c : decay constant
   *
   *  f'(x)          = a/c * (-x+b+c) * exp(-(x-b)/c)
   *  f'(x)|x=b+c    = 0.
   *  f(b+c)         = a * c * exp(-1)
   *  Sf(x)dx|b->inf = a * c^2
   *
   *  peak time = b + c
   *  amplitude = a * c * exp(-1)
   *  integral  = a * c^2
   *
   ****************************************************/

  // TGraphErrors graph( NumOfSamplesSSD, &m_time[0], dE, 0, rms );
  TGraph graph( NumOfSamplesSSD, &(m_time[0]), &(dE[0]) );

  Double_t xmin =  40.;
  Double_t xmax = 210.;
  TF1 func( "func", "[0]*(x-[1])*exp(-(x-[1])/[2])", xmin, xmax );
  func.SetParameter( 0, dE[3]*std::exp(1)/60. );
  func.SetParLimits( 0, 0., 50000.*std::exp(-1.) );
  func.SetParameter( 1, 40. );
  func.SetParLimits( 1, 10., 100. );
  func.FixParameter( 2, 50. );

  graph.Fit("func", "RQ");
  Double_t p[3];
  func.GetParameters(p);

  m_peak_time = p[1] + p[2];
  m_amplitude = p[0] * p[2] * std::exp(-1.);
  // m_de        = func.Integral( p[1], math::Infinity() );
  m_de        = p[0]*p[2]*p[2];
  m_chisqr    = func.GetChisquare() / func.GetNDF();

  m_is_ssd        = true;
  m_good_waveform = true;

  if( m_de<0.1 ) m_good_waveform = false;

  return true;
}

//______________________________________________________________________________
// Bool_t
// DCHit::CalcObservablesSimulation( Double_t dlength )
// {
//   if( !gGeom.IsReady() ) return false;

//   m_wpos=gGeom.CalcWirePosition(m_layer,m_wire);
//   m_angle=gGeom.GetTiltAngle(m_layer);

//   m_dl = dlength;
//   Bool_t status=true;

//   if(m_layer>=100){
//     if( m_dl>MinDLBc[m_layer-100] && m_dl<MaxDLBc[m_layer-100] )
//       m_dl_range=true;
//   }
//   else {
//     if( m_dl>MinDLSdc[m_layer] && m_dl<MaxDLSdc[m_layer] )
//       m_dl_range=true;
//   }
//   return status;
// }

//______________________________________________________________________________
Bool_t
DCHit::DoTimeCorrection( Double_t offset )
{
  if( m_time_corrected ){
    Print(FUNC_NAME+" already corrected!");
    return false;
  }

#if 0
  Print("Before Correction");
#endif

  std::vector<Double_t> ctime;
  for( Int_t i=0, n=m_time.size(); i<n; ++i ){
    ctime.push_back( m_time[i] + offset );
  }

  m_time            = ctime;
  m_peak_time      += offset;
  m_time_corrected  = true;

#if 0
  Print("After Correction");
#endif

  return true;
}

//______________________________________________________________________________
Double_t
DCHit::GetResolution( void ) const
{
  return gGeom.GetResolution(m_layer);
}

//______________________________________________________________________________
void
DCHit::Print( Option_t* option ) const
{
  const Int_t w = 16;
  hddaq::cout << "#D " << FUNC_NAME << std::endl
	      << std::setw(w) << std::left << "layer" << m_layer << std::endl
	      << std::setw(w) << std::left << "wire"  << m_wire  << std::endl
	      << std::setw(w) << std::left << "wpos"  << m_wpos  << std::endl
	      << std::setw(w) << std::left << "angle" << m_angle << std::endl
	      << std::setw(w) << std::left << "z"     << m_z     << std::endl
	      << std::setw(w) << std::left << "kaon"  << m_belong_kaon << std::endl;

  if(m_is_ssd){
    hddaq::cout << std::setw(w) << std::left << "zero_suppressed"
		<< m_zero_suppressed << std::endl;
    hddaq::cout << std::setw(w) << std::left << "time_corrected"
		<< m_time_corrected << std::endl;
    hddaq::cout << std::setw(w) << std::left << "good_waveform"
		<< m_good_waveform << std::endl;
    hddaq::cout << std::setw(w) << std::left << "adc" << m_adc.size() << " : ";
    std::copy(m_adc.begin(), m_adc.end(),
	      std::ostream_iterator<Int_t>(hddaq::cout, " "));
    hddaq::cout << std::endl;
    hddaq::cout << std::setw(w) << std::left << "peak_height"
		<< m_peak_height << std::endl;
    hddaq::cout << std::setw(w) << std::left << "peak_position"
		<< m_peak_position << std::endl;
  }

  hddaq::cout << std::setw(w) << std::left << "tdc" << m_tdc.size() << " : ";
  std::copy( m_tdc.begin(), m_tdc.end(),
	     std::ostream_iterator<Int_t>(hddaq::cout, " ") );
  hddaq::cout << std::endl;

  if(!m_is_ssd){
    hddaq::cout << std::endl << std::setw(w) << std::left
		<< "trailing" << m_trailing.size() << " : ";
    std::copy(m_trailing.begin(), m_trailing.end(),
	      std::ostream_iterator<Int_t>(hddaq::cout, " "));
    hddaq::cout << std::endl << std::setw(w) << std::left
		<< "drift time" << m_dt.size() << " : ";
    std::copy(m_dt.begin(), m_dt.end(),
	      std::ostream_iterator<Double_t>(hddaq::cout, " "));
    hddaq::cout << std::endl << std::setw(w) << std::left
		<< "drift length" << m_dl.size() << " : ";
    std::copy(m_dl.begin(), m_dl.end(),
	      std::ostream_iterator<Double_t>(hddaq::cout, " "));
    hddaq::cout << std::endl << std::setw(w) << std::left
		<< "trailing time" << m_trailing_time.size() << " : ";
    std::copy(m_trailing_time.begin(), m_trailing_time.end(),
	      std::ostream_iterator<Double_t>(hddaq::cout, " "));
    hddaq::cout << std::endl;
    hddaq::cout << std::setw(w) << std::left
		<< "belongTrack" << m_belong_track.size() << " : ";
    std::copy(m_belong_track.begin(), m_belong_track.end(),
	      std::ostream_iterator<Bool_t>(hddaq::cout, " "));
    hddaq::cout << std::endl << std::setw(w) << std::left
		<< "dlRange" << m_dl_range.size() << " : ";
    std::copy(m_dl_range.begin(), m_dl_range.end(),
	      std::ostream_iterator<Bool_t>(hddaq::cout, " "));
    hddaq::cout << std::endl;
  }

  if(m_is_ssd){
    hddaq::cout << std::setw(w) << std::left << "waveform" << m_waveform.size() << " : ";
    std::copy(m_waveform.begin(), m_waveform.end(),
	      std::ostream_iterator<Int_t>(hddaq::cout, " "));
    hddaq::cout << std::endl;
    hddaq::cout << std::setw(w) << std::left
		<< "time" << m_time.size() << " : ";
    std::copy(m_time.begin(), m_time.end(),
	      std::ostream_iterator<Int_t>(hddaq::cout, " "));
    hddaq::cout << std::endl;
    hddaq::cout << std::setw(w) << std::left << "chisqr"    << m_chisqr    << std::endl;
    hddaq::cout << std::setw(w) << std::left << "deviation" << m_deviation << std::endl;
    hddaq::cout << std::setw(w) << std::left << "amplitude" << m_amplitude << std::endl;
    hddaq::cout << std::setw(w) << std::left << "delta E"   << m_de        << std::endl;
  }

  if(m_mwpc_flag){
    hddaq::cout << std::endl
		<< std::setw(w) << std::left << "clsize" << m_cluster_size << std::endl
		<< std::setw(w) << std::left << "mean wire" << m_mwpc_wire << std::endl
		<< std::setw(w) << std::left << "mean pos"  << m_mwpc_wpos << std::endl;
  }
}
