// -*- C++ -*-

#ifndef KURAMA_TRACK_HH
#define KURAMA_TRACK_HH

#include <vector>
#include <iosfwd>
#include <iostream>
#include <functional>

#include <TObject.h>

#include <std_ostream.hh>

#include "RungeKuttaUtilities.hh"
#include "ThreeVector.hh"

class DCLocalTrack;
class TrackHit;
class DCAnalyzer;
class Hodo2Hit;

//______________________________________________________________________________
class KuramaTrack : public TObject
{
public:
  KuramaTrack( DCLocalTrack *track_in, DCLocalTrack *track_out );
  ~KuramaTrack( void );

private:
  KuramaTrack( const KuramaTrack& );
  KuramaTrack& operator =( const KuramaTrack& );

public:
  enum RKstatus { kInit,
		  kPassed,
		  kExceedMaxPathLength,
		  kExceedMaxStep,
		  kFailedTraceLast,
		  kFailedGuess,
		  kFailedSave,
		  kFatal,
		  nRKstatus };

private:
  std::string             s_status[nRKstatus];
  RKstatus                m_status;
  DCLocalTrack           *m_track_in;
  DCLocalTrack           *m_track_out;
  Double_t                m_tof_seg;
  Double_t                m_initial_momentum;
  std::vector<TrackHit*>  m_hit_array;
  RKHitPointContainer     m_HitPointCont;
  Int_t                   m_n_iteration;
  Int_t                   m_nef_iteration;
  Double_t                m_chisqr;
  Double_t                m_polarity;
  ThreeVector             m_primary_position;
  ThreeVector             m_primary_momentum;
  Double_t                m_path_length_tof;
  Double_t                m_path_length_total;
  ThreeVector             m_tof_pos;
  ThreeVector             m_tof_mom;
  RKCordParameter         m_cord_param;
  Bool_t                  m_gfastatus;
  std::vector<Double_t>   m_ssd_seg;

public:
  DCLocalTrack*      GetLocalTrackIn( void ) const { return m_track_in;}
  DCLocalTrack*      GetLocalTrackOut( void ) const { return m_track_out; }
  Bool_t             DoFit( void );
  Bool_t             DoFit( RKCordParameter iniCord );
  Bool_t             DoFitMinuit( void );
  Bool_t             Status( void ) const { return m_status; }
  Int_t              Niteration( void ) const { return m_n_iteration; }
  void               SetInitialMomentum( Double_t initial_momentum ) { m_initial_momentum = initial_momentum; }
  const ThreeVector& PrimaryPosition( void ) const { return m_primary_position; }
  const ThreeVector& PrimaryMomentum( void ) const { return m_primary_momentum; }
  Double_t           PrimaryMomMag( void )   const { return m_primary_momentum.Mag();}
  Double_t           PathLengthToTOF( void ) const { return m_path_length_tof; }
  Double_t           PathLengthTotal( void ) const { return m_path_length_total; }
  Double_t           TofSeg( void ) const { return m_tof_seg; }
  const ThreeVector& TofPos( void ) const { return m_tof_pos; }
  const ThreeVector& TofMom( void ) const { return m_tof_mom; }
  Double_t           Chisqr( void )          const { return m_chisqr; }
  Double_t           Polarity( void )        const { return m_polarity; }
  Int_t              GetNDF( void )          const;
  Int_t              GetNHits( void )        const { return m_hit_array.size(); }
  TrackHit*          GetHit( Int_t i ) const;
  TrackHit*          GetHitOfLayerNumber( Int_t lnum ) const;
  Bool_t             GoodForAnalysis( void ) const { return m_gfastatus; }
  Bool_t             GoodForAnalysis( Bool_t status )
  { Bool_t ret=m_gfastatus; m_gfastatus=status; return ret; }
  Double_t           GetInitialMomentum( void ) const { return m_initial_momentum; }
  Bool_t             GetTrajectoryLocalPosition( Int_t layer, Double_t & x, Double_t & y ) const;
  // for SSD
  std::vector<Double_t> GetSsdSeg( void ) const { return m_ssd_seg; }

  void               Print( Option_t* option="" ) const;
  Bool_t             ReCalc( Bool_t applyRecursively=false );

private:
  void     FillHitArray( void );
  void     ClearHitArray( void );
  Double_t CalcChiSqr( const RKHitPointContainer &hpCont ) const;
  Bool_t   GuessNextParameters( const RKHitPointContainer &hpCont,
				RKCordParameter &Cord,
				Double_t &estDeltaChisqr,
				Double_t &lambdaCri, Double_t dmp=0. ) const;
  Bool_t   SaveCalcPosition( const RKHitPointContainer &hpCont );
  void     PrintCalcHits( const RKHitPointContainer &hpCont,
			  std::ostream &ost = std::cout ) const;
  Bool_t   SaveTrackParameters( const RKCordParameter &cp );

  ClassDef(KuramaTrack,0);
};

//______________________________________________________________________________
struct KuramaTrackComp
  : public std::binary_function<KuramaTrack*, KuramaTrack*, Bool_t>
{
  Bool_t operator()( const KuramaTrack * const p1,
		     const KuramaTrack * const p2 ) const
  {
    Int_t n1=p1->GetNHits(), n2=p2->GetNHits();
    if( n1>n2+1 ) return true;
    if( n2>n1+1 ) return false;
    return (p1->Chisqr())<(p2->Chisqr());
  }
};

#endif
