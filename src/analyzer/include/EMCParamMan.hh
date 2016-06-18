/*
 */

#ifndef EMC_PARAM_MAN_HH
#define EMC_PARAM_MAN_HH

#include <vector>
#include <map>

#include <TROOT.h>

class EMCParamMan
{
  int                 m_nspill;
  std::vector<int>    m_spill;
  std::vector<double> m_x;
  std::vector<double> m_y;

public:
  ~EMCParamMan( void );
  static EMCParamMan& GetInstance( void );

  bool   Initialize( const std::string& filename );
  int    NSpill( void ){ return m_nspill; }
  int    Pos2Spill( double x, double y );
  double Spill2PosX( int spill ){ return m_x[spill]; }
  double Spill2PosY( int spill ){ return m_y[spill]; }

private:
  EMCParamMan( void );
  EMCParamMan( const EMCParamMan& object );
  EMCParamMan& operator =( const EMCParamMan& object );

};

// getInstance
inline EMCParamMan&
EMCParamMan::GetInstance( void )
{
  static EMCParamMan object;
  return object;
}

#endif
