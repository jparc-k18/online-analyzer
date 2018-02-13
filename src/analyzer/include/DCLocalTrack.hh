// -*- C++ -*-

#ifndef DC_LOCAL_TRACK_HH
#define DC_LOCAL_TRACK_HH

#include <vector>
#include <functional>

#include <std_ostream.hh>

#include "DCLTrackHit.hh"
#include "ThreeVector.hh"

class DCLTrackHit;
class DCAnalyzer;

//______________________________________________________________________________
class DCLocalTrack : public TObject
{
public:
  DCLocalTrack( void );
  ~DCLocalTrack( void );

private:
  DCLocalTrack( const DCLocalTrack & );
  DCLocalTrack & operator =( const DCLocalTrack & );

private:
  Bool_t                    m_is_fitted;     // flag of DoFit()
  Bool_t                    m_is_calculated; // flag of Calculate()
  std::vector<DCLTrackHit*> m_hit_array;
  Double_t                  m_Ax;
  Double_t                  m_Ay;
  Double_t                  m_Au;
  Double_t                  m_Av;
  Double_t                  m_Chix;
  Double_t                  m_Chiy;
  Double_t                  m_Chiu;
  Double_t                  m_Chiv;
  Double_t                  m_x0;
  Double_t                  m_y0;
  Double_t                  m_u0;
  Double_t                  m_v0;
  Double_t                  m_a;
  Double_t                  m_b;
  Double_t                  m_chisqr;
  Bool_t                    m_good_for_tracking;
  // for SSD
  Double_t                  m_de;
  // for Honeycomb
  Double_t                  m_chisqr1st; // 1st iteration for honeycomb
  Double_t                  m_n_iteration;

public:
  void         AddHit( DCLTrackHit *hitp );
  void         Calculate( void );
  void         DeleteNullHit( void );
  Bool_t       DoFit( void );
  Bool_t       DoFitBcSdc( void );
  Bool_t       FindLayer( Int_t layer ) const;
  Int_t        GetNDF( void ) const;
  Int_t        GetNHit( void ) const { return m_hit_array.size(); }
  DCLTrackHit* GetHit( Int_t i ) const;
  DCLTrackHit* GetHitOfLayerNumber( Int_t lnum ) const;
  Double_t     GetWire( Int_t layer ) const;
  Bool_t       HasHoneycomb( void ) const;
  Bool_t       IsFitted( void ) const { return m_is_fitted; }
  Bool_t       IsCalculated( void ) const { return m_is_calculated; }

  void SetAx( Double_t Ax ) { m_Ax = Ax; }
  void SetAy( Double_t Ay ) { m_Ay = Ay; }
  void SetAu( Double_t Au ) { m_Au = Au; }
  void SetAv( Double_t Av ) { m_Av = Av; }
  void SetChix( Double_t Chix ) { m_Chix = Chix; }
  void SetChiy( Double_t Chiy ) { m_Chiy = Chiy; }
  void SetChiu( Double_t Chiu ) { m_Chiu = Chiu; }
  void SetChiv( Double_t Chiv ) { m_Chiv = Chiv; }
  void SetDe( Double_t de ) { m_de = de; }

  Double_t GetX0( void ) const { return m_x0; }
  Double_t GetY0( void ) const { return m_y0; }
  Double_t GetU0( void ) const { return m_u0; }
  Double_t GetV0( void ) const { return m_v0; }

  //For XUV Tracking
  Bool_t DoFitVXU( void );

  Double_t GetVXU_A( void ) const { return m_a; }
  Double_t GetVXU_B( void ) const { return m_b; }
  Double_t GetVXU( Double_t z ) const { return m_a*z+m_b; }
  Double_t GetAx( void ) const { return m_Ax; }
  Double_t GetAy( void ) const { return m_Ay; }
  Double_t GetAu( void ) const { return m_Au; }
  Double_t GetAv( void ) const { return m_Av; }

  Double_t GetDifVXU( void ) const ;
  Double_t GetDifVXUSDC34( void ) const;
  Double_t GetChiSquare( void ) const { return m_chisqr; }
  Double_t GetChiSquare1st( void ) const { return m_chisqr1st; }
  Double_t GetChiX( void ) const { return m_Chix; }
  Double_t GetChiY( void ) const { return m_Chiy; }
  Double_t GetChiU( void ) const { return m_Chiu; }
  Double_t GetChiV( void ) const { return m_Chiv; }
  Double_t GetX( Double_t z ) const { return m_x0+m_u0*z; }
  Double_t GetY( Double_t z ) const { return m_y0+m_v0*z; }
  Double_t GetS( Double_t z, Double_t tilt ) const { return GetX(z)*std::cos(tilt)+GetY(z)*std::sin(tilt); }
  Int_t    GetNIteration( void ) const { return m_n_iteration; }
  Double_t GetTheta( void ) const;
  Bool_t   GoodForTracking( void ) const { return m_good_for_tracking; }
  Bool_t   GoodForTracking( Bool_t status )
  { Bool_t ret = m_good_for_tracking; m_good_for_tracking = status; return ret; }
  Bool_t   ReCalc( Bool_t ApplyRecursively=false );
  Double_t GetDe( void ) const { return m_de; }
  virtual void Print( Option_t* option="" ) const;
  void   PrintVXU( const std::string& arg="" ) const;

  ClassDef(DCLocalTrack,0);
};

//______________________________________________________________________________
inline
std::ostream&
operator <<( std::ostream& ost, const DCLocalTrack& track )
{
  track.Print();
  return ost;
}

//______________________________________________________________________________
struct DCLTrackComp
  : public std::binary_function <DCLocalTrack *, DCLocalTrack *, Bool_t>
{
  Bool_t operator()( const DCLocalTrack * const p1,
		     const DCLocalTrack * const p2 ) const
  {
    Int_t n1=p1->GetNHit(), n2=p2->GetNHit();
    Double_t chi1=p1->GetChiSquare(), chi2=p2->GetChiSquare();
    if( n1>n2+1 )
      return true;
    if( n2>n1+1 )
      return false;
    if( n1<=4 || n2<=4 )
      return ( n1 >= n2 );

    return ( chi1 <= chi2 );

    // if( n1==n2 )
    //   return ( chi1 <= chi2 );

    // static const Double_t th = 3.; // 5-sigma
    // if( n1>n2 )
    //   return ( chi1-chi2 <= th/(n1-4) );
    // else
    //   return ( chi2-chi1 <= th/(n2-4) );
  }
};

//______________________________________________________________________________
struct DCLTrackComp1
  : public std::binary_function <DCLocalTrack *, DCLocalTrack *, Bool_t>
{
  Bool_t operator()( const DCLocalTrack * const p1,
		     const DCLocalTrack * const p2 ) const
  {
    Int_t n1=p1->GetNHit(), n2=p2->GetNHit();
    Double_t chi1=p1->GetChiSquare(),chi2=p2->GetChiSquare();
    if(n1>n2) return true;
    if(n2>n1) return false;
    return ( chi1<=chi2 );
  }

};

//______________________________________________________________________________
struct DCLTrackComp2
  : public std::binary_function <DCLocalTrack *, DCLocalTrack *, Bool_t>
{
  Bool_t operator()( const DCLocalTrack * const p1,
		     const DCLocalTrack * const p2 ) const
  {
    Int_t n1=p1->GetNHit(), n2=p2->GetNHit();
    Double_t chi1=p1->GetChiSquare(),chi2=p2->GetChiSquare();
    if(n1<n2) return true;
    if(n2<n1) return false;
    return ( chi1<=chi2 );
  }

};

//______________________________________________________________________________
struct DCLTrackComp3
  : public std::binary_function <DCLocalTrack *, DCLocalTrack *, Bool_t>
{
  Bool_t operator()( const DCLocalTrack * const p1,
		     const DCLocalTrack * const p2 ) const
  {
    Int_t n1=p1->GetNHit(), n2=p2->GetNHit();
    Double_t chi1=p1->GetChiSquare(),chi2=p2->GetChiSquare();
    Double_t a1= std::abs(1.-chi1), a2=std::abs(1.-chi2);
    if(a1<a2) return true;
    if(a2<a1) return false;
    return (n1<=n2);
  }

};

//______________________________________________________________________________
struct DCLTrackComp4
  : public std::binary_function <DCLocalTrack *, DCLocalTrack *, Bool_t>
{
  Bool_t operator()( const DCLocalTrack * const p1,
		     const DCLocalTrack * const p2 ) const
  {
    Int_t n1=p1->GetNHit(), n2=p2->GetNHit();
    Double_t chi1=p1->GetChiSquare(),chi2=p2->GetChiSquare();
    if( (n1>n2+1) && (std::abs(chi1-chi2)<2.) )
      return true;
    if( (n2>n1+1) && ( std::abs(chi1-chi2)<2.) )
      return false;

    return (chi1<=chi2);
  }
};

//______________________________________________________________________________
struct DCLTrackCompSdcInFiber
  : public std::binary_function <DCLocalTrack *, DCLocalTrack *, Bool_t>
{
  Bool_t operator()( const DCLocalTrack * const p1,
		     const DCLocalTrack * const p2 ) const
  {
    Int_t n1=p1->GetNHit(), n2=p2->GetNHit();
    Double_t chi1=p1->GetChiSquare(),chi2=p2->GetChiSquare();
    Int_t NofFiberHit1 = 0;
    Int_t NofFiberHit2 = 0;
    for(Int_t ii=0;ii<n1;ii++){
      Int_t layer = p1->GetHit(ii)->GetLayer();
      if( layer <= 4 ) NofFiberHit1++;
    }
    for(Int_t ii=0;ii<n2;ii++){
      Int_t layer = p2->GetHit(ii)->GetLayer();
      if( layer <= 4 ) NofFiberHit2++;
    }

    if( (n1>n2+1) ){
      return true;
    }
    else if( (n2>n1+1)  ){
      return false;
    }
    else if( NofFiberHit1 > NofFiberHit2 ){
      return true;
    }
    else if( NofFiberHit2 > NofFiberHit1 ){
      return false;
    }
    else{
      return (chi1<=chi2);
    }

  }
};

//______________________________________________________________________________
struct DCLTrackCompSsd
  : public std::binary_function <DCLocalTrack *, DCLocalTrack *, Bool_t>
{
  Bool_t operator()( const DCLocalTrack * const p1,
		     const DCLocalTrack * const p2 ) const
  {
    Int_t n1 = p1->GetNHit(), n2 = p2->GetNHit();
    if( (n1>n2) ) return true;
    if( (n2>n1) ) return false;
    // return ( p1->GetDe() >= p2->GetDe() );
    return ( p1->GetChiSquare() <= p2->GetChiSquare() );
  }
};

//______________________________________________________________________________
struct DCLTrackCompSsdSdc
  : public std::binary_function <DCLocalTrack *, DCLocalTrack *, Bool_t>
{
  Bool_t operator()( const DCLocalTrack * const p1,
		     const DCLocalTrack * const p2 ) const
  {
    Int_t n1=p1->GetNHit(), n2=p2->GetNHit();
    Double_t chi1=p1->GetChiSquare(),chi2=p2->GetChiSquare();

    Int_t nSsdXHit1=0, nSsdXHit2=0;
    Int_t nSsdYHit1=0, nSsdYHit2=0;
    for( Int_t i=0; i<n1; ++i ){
      Int_t layer = p1->GetHit(i)->GetLayer();
      switch( layer ){
      case 8: case 10: case 11: case 13:
	nSsdXHit1++;
	break;
      case 7: case 9: case 12: case 14:
	nSsdYHit1++;
	break;
      }
    }
    for( Int_t i=0; i<n2; ++i ){
      Int_t layer = p2->GetHit(i)->GetLayer();
      switch( layer ){
      case 8: case 10: case 11: case 13:
	nSsdXHit2++;
	break;
      case 7: case 9: case 12: case 14:
	nSsdYHit2++;
	break;
      }
    }

    // Bool_t SsdXYHit1 = ( nSsdXHit1>0 && nSsdYHit1>0 );
    // Bool_t SsdXYHit2 = ( nSsdXHit2>0 && nSsdYHit2>0 );
    // Int_t  nSsdHit1  = nSsdXHit1 + nSsdYHit1;
    // Int_t  nSsdHit2  = nSsdXHit2 + nSsdYHit2;

    if( (n1>n2+1) ) return true;
    if( (n2>n1+1) ) return false;
    // if( nSsdXHit1 > nSsdXHit2+1 ) return true;
    // if( nSsdYHit1 > nSsdYHit2+1 ) return true;
    return (chi1<=chi2);
  }
};

//______________________________________________________________________________
struct DCLTrackCompSdcOut
  : public std::binary_function <DCLocalTrack *, DCLocalTrack *, Bool_t>
{
  Bool_t operator()( const DCLocalTrack * const p1,
		     const DCLocalTrack * const p2 ) const
  {
    Int_t n1=p1->GetNHit(), n2=p2->GetNHit();
    Double_t chi1=p1->GetChiSquare(), chi2=p2->GetChiSquare();
    if( n1 > n2 ) return true;
    if( n2 > n1 ) return false;
    return ( chi1 <= chi2 );
  }
};

#endif
