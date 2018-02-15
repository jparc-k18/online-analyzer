// -*- C++ -*-

#ifndef K18_TRANS_MATRIX_HH
#define K18_TRANS_MATRIX_HH

#include <Rtypes.h>
#include <TObject.h>
#include <TString.h>

//______________________________________________________________________________
class K18TransMatrix : public TObject
{
public:
  static K18TransMatrix& GetInstance( void );
  ~K18TransMatrix( void );

private:
  K18TransMatrix( void );
  K18TransMatrix( const K18TransMatrix& );
  K18TransMatrix& operator=( const K18TransMatrix& );

private:
  Bool_t  m_is_ready;
  TString m_file_name;

  enum NameX { X, A, T, XX, XA, XT, AA, AT, TT, YY,
	       YB, BB, XXX, XXA, XXT, XAA, XAT, XTT, XYY, XYB,
	       XBB, AAA, AAT, ATT, AYY, AYB, ABB, TTT, TYY, TYB,
	       TBB,
	       size_NameX };
  enum NameY { Y, B, YX, YA, YT, BX, BA, BT,
	       size_NameY };
  Double_t m_X[size_NameX], m_Y[size_NameY];
  Double_t m_U[size_NameX], m_V[size_NameY];

public:
  Bool_t Initialize( void );
  Bool_t Initialize( const TString& file_name );
  Bool_t IsReady( void ) const { return m_is_ready; }
  void   SetFileName( const TString& file_name ) { m_file_name = file_name; }
  Bool_t Transport( Double_t xin, Double_t yin,
		    Double_t uin, Double_t vin,
		    Double_t delta,
		    Double_t& xout, Double_t& yout,
		    Double_t& uout, Double_t& vout ) const;
  Bool_t CalcDeltaD2U( Double_t xin, Double_t yin,
		       Double_t uin, Double_t vin,
		       Double_t xout, Double_t& yout,
		       Double_t& uout, Double_t& vout,
		       Double_t& delta1, Double_t& delta2 ) const;

  ClassDef(K18TransMatrix,0);
};

//______________________________________________________________________________
inline K18TransMatrix&
K18TransMatrix::GetInstance( void )
{
  static K18TransMatrix g_instance;
  return g_instance;
}

#endif
