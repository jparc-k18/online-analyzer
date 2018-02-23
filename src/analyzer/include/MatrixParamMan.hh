// -*- C++ -*-

#ifndef MATRIX_PARAM_MAN_HH
#define MATRIX_PARAM_MAN_HH

#include <vector>
#include <map>

#include <TObject.h>

//______________________________________________________________________________
class MatrixParamMan : public TObject
{
public:
  static MatrixParamMan& GetInstance( void );
  ~MatrixParamMan( void );

private:
  MatrixParamMan( void );
  MatrixParamMan( const MatrixParamMan& );
  MatrixParamMan& operator =( const MatrixParamMan& );

private:
  std::vector< std::vector<Double_t> >                m_enable_2d;
  std::vector< std::vector< std::vector<Double_t> > > m_enable_3d;

public:
  Bool_t Initialize( const TString& filename_2d,
		     const TString& filename_3d );
  Bool_t IsAccept( Int_t detA, Int_t detB );
  Bool_t IsAccept( Int_t detA, Int_t detB, Int_t detC );

  ClassDef(MatrixParamMan,0);
};

//______________________________________________________________________________
inline MatrixParamMan&
MatrixParamMan::GetInstance( void )
{
  static MatrixParamMan g_instance;
  return g_instance;
}

#endif
