/*
 */

#ifndef MST_PARAM_MAN_HH
#define MST_PARAM_MAN_HH

#include <vector>
#include <map>

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
};

// getInstance
inline MsTParamMan&
MsTParamMan::GetInstance( void )
{
  static MsTParamMan object;
  return object;
}

#endif
