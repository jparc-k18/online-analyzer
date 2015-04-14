/*
  GeAdcCalibMan.hh
*/

#ifndef GeAdcCalibMan_h

#define GeAdcCalibMan_h 1

#include <map>
#include <string>

struct GeAdcCalibMap;
class GeHit;

class GeAdcCalibMan
{
public:
  GeAdcCalibMan();
  ~GeAdcCalibMan();
  static GeAdcCalibMan& GetInstance(){static GeAdcCalibMan obj; return obj;}

private:
  GeAdcCalibMan( const GeAdcCalibMan & );
  GeAdcCalibMan & operator = ( const GeAdcCalibMan & );

private:
  std::string ParamFileName_;
  mutable std::map <unsigned int, GeAdcCalibMap *> Cont_;

public:
  bool Initialize( const std::string& filename );
  double CalibAdc( int SegId, int adc, double &adccal );

private:
  GeAdcCalibMap * getMap( int SegmentId ) const;
  void clearElements( void );
};

#endif 
