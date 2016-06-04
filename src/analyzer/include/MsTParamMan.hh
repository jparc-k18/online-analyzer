/*
 */

#ifndef MST_PARAM_MAN_HH
#define MST_PARAM_MAN_HH

#include <vector>
#include <map>

#include<TROOT.h>

class MsTParamMan
{
  std::vector< std::vector<double> > m_low_threshold;
  std::vector< std::vector<double> > m_high_threshold;

public:
  ~MsTParamMan( void );
  static MsTParamMan& GetInstance( void );

  bool Initialize( const std::string& filename );
  bool IsAccept( int detA, int detB, int tdc );

private:
  MsTParamMan( void );
  MsTParamMan( const MsTParamMan& object );
  MsTParamMan& operator =( const MsTParamMan& object );

  ClassDef(MsTParamMan, 0)
};

// getInstance
inline MsTParamMan&
MsTParamMan::GetInstance( void )
{
  static MsTParamMan object;
  return object;
}

#endif
