/*
 */

#ifndef MATRIX_PARAM_MAN_HH
#define MATRIX_PARAM_MAN_HH

#include <vector>
#include <map>

#include<TROOT.h>

class MatrixParamMan
{
  std::vector< std::vector<double> > m_enable_2d;
  std::vector< std::vector< std::vector<double> > > m_enable_3d;

public:
  virtual ~MatrixParamMan( void );
  static MatrixParamMan& GetInstance( void );

  bool Initialize( const std::string& filename_2d,
		   const std::string& filename_3d );
  bool IsAccept( int detA, int detB );
  bool IsAccept( int detA, int detB, int detC );

private:
  MatrixParamMan( void );
  MatrixParamMan( const MatrixParamMan& object );
  MatrixParamMan& operator =( const MatrixParamMan& object );

  ClassDef(MatrixParamMan, 0)
};

// getInstance
inline MatrixParamMan&
MatrixParamMan::GetInstance( void )
{
  static MatrixParamMan object;
  return object;
}

#endif
