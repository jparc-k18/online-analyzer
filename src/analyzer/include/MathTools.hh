/**
 *  file: MathTools.hh
 *  date: 2017.08.21
 *
 */

#ifndef MATH_TOOLS_HH
#define MATH_TOOLS_HH

#include <TMath.h>

#include "ThreeVector.hh"

//______________________________________________________________________________
namespace math
{
  //______________________________________________________________________________
  inline Double_t Rad2Deg( void ){ return TMath::RadToDeg(); }
  inline Double_t Deg2Rad( void ){ return TMath::DegToRad(); }
  inline Double_t Rad2Deg( Double_t rad ){ return rad*Rad2Deg(); }
  inline Double_t Deg2Rad( Double_t deg ){ return deg*Deg2Rad(); }

  //______________________________________________________________________________
  inline Double_t nan( void ) { return TMath::QuietNaN(); }
  inline Double_t Infinity( void ) { return TMath::Infinity(); }
  inline Double_t Epsilon( void ) { return TMath::Limits<Double_t>::Epsilon(); }
  inline Int_t    Round( Double_t a ) { return Int_t(a+0.5-(a<0.)); }

  //______________________________________________________________________________
  template <typename T>
  inline T Sign( T a ) { return (a>0) - (a<0); }

  template <typename Container>
  inline Double_t Accumulate( const Container& c )
  {
    Double_t sum = 0.;
    typename Container::const_iterator itr, end=c.end();
    for( itr=c.begin(); itr!=end; ++itr ){
      sum += *itr;
    }
    return sum;
  }

  template <typename Container>
  inline Double_t Average( const Container& c )
  {
    return Accumulate(c)/c.size();
  }

  template <typename Container>
  inline Double_t Deviation( const Container& c )
  {
    Double_t sum  = 0.;
    Double_t sum2 = 0.;
    typename Container::const_iterator itr, end=c.end();
    for( itr=c.begin(); itr!=end; ++itr ){
      sum  += *itr;
      sum2 += (*itr) * (*itr);
    }
    std::size_t n = c.size();
    return TMath::Sqrt( ( sum2-(sum*sum/n) ) / n );
  }

  template <typename Container>
  inline Double_t MaxElement( const Container& c )
  {
    return *std::max_element(c.begin(), c.end());
  }

  template <typename Container>
  inline Double_t MinElement( const Container& c )
  {
    return *std::min_element(c.begin(), c.end());
  }

  //______________________________________________________________________________
  // compare l and r
  template <typename T>
  inline Bool_t Equal( const T& l, const T& r );

  // 3.14159265358979323846
  inline Double_t Pi( void ) { return TMath::Pi(); }
  // velocity of light : 299.792458 mm ns^-1
  inline Double_t C( void ) { return TMath::C()*1e-6; }
  // base of natural log
  inline Double_t E( void ) { return TMath::E(); }

  Bool_t SolveGaussJordan( const std::vector<Double_t>& z,
			   const std::vector<Double_t>& w,
			   const std::vector<Double_t>& s,
			   const std::vector<Double_t>& ct,
			   const std::vector<Double_t>& st,
			   Double_t& x0,
			   Double_t& u0,
			   Double_t& y0,
			   Double_t& v0 );

  Bool_t GaussElim( Double_t **a, Int_t n, Double_t *b, Int_t *indx, Int_t *ipiv );
  Bool_t GaussJordan( Double_t **a, Int_t n, Double_t *b,
		      Int_t *indxc, Int_t *indxd, Int_t *ipiv );
  Bool_t InterpolateRatio( Int_t n, const Double_t *xa, const Double_t *ya,
			   Double_t *w1, Double_t *w2,
			   Double_t x, Double_t &y, Double_t &dy );
  Bool_t InterpolatePol( Int_t n, const Double_t *xa, const Double_t *ya,
			 Double_t *w1, Double_t *w2,
			 Double_t x, Double_t &y, Double_t &dy );
  Bool_t SVDksb( Double_t **u, const Double_t *w, Double_t **v,
		 Int_t m, Int_t n, const Double_t *b, Double_t *x, Double_t *wv );
  Bool_t SVDcmp( Double_t **a, Int_t m, Int_t n, Double_t *w, Double_t **v, Double_t *wv );
  template <typename T>
  void PrintMatrix( T *mat, const std::string& arg="",
		    const Int_t column=4, const Int_t line=4 );
  template <typename T>
  void PrintVector( T *vec, const std::string& arg="",
		    const std::size_t size=4 );
}

//______________________________________________________________________________
template <typename T>
inline Bool_t
math::Equal( const T& l, const T& r )
{
  Double_t al = TMath::Abs(l), ar = TMath::Abs(r);
  return
    ( ( al <= Epsilon() ) || ( ar <= Epsilon() ) )
    ?
    ( TMath::Abs(l-r) <= Epsilon() )
    :
    ( ( TMath::Abs(l-r)/TMath::Max( al, ar ) ) <= Epsilon() );
}


#endif
