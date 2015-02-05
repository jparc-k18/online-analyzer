/*
  DCTdcCalibMan.hh

  2012/1/24
*/

#ifndef DCTdcCalibMan_h
#define DCTdcCalibMan_h 1

#include <map>
#include <string>
#include <vector>

typedef std::vector <int> IntVec;

struct DCTdcCalMap;

class DCTdcCalibMan
{
public:
  DCTdcCalibMan();
  ~DCTdcCalibMan();
  static DCTdcCalibMan& GetInstance(){static DCTdcCalibMan obj; return obj;}

private:
  DCTdcCalibMan( const DCTdcCalibMan & );
  DCTdcCalibMan & operator = ( const DCTdcCalibMan & );

private:
  std::string MapFileName_;

  mutable std::map <unsigned int, DCTdcCalMap *> Cont_;

public:
  bool Initialize( void );
  void SetFileName(const std::string& filename){MapFileName_ = filename;}

  bool GetTime( int PlaneId, double WireId, int tdc, double & time ) const;
  bool GetTdc( int PlaneId, double WireId, double time, int & tdc ) const;

private:
  DCTdcCalMap * getMap( int PlaneId, double WireId ) const;
  void clearElements( void );
};

#endif 
