// -*- C++ -*-

#ifndef EVENT_ANALYZER_HH
#define EVENT_ANALYZER_HH

#include <TObject.h>

class RawData;
class DCAnalyzer;
class DCLocalTrack;
class HodoAnalyzer;
class BH2Hit;

namespace analyzer
{

//______________________________________________________________________________
class EventAnalyzer : public TObject
{
public:
  EventAnalyzer( void );
  ~EventAnalyzer( void );

protected:
  RawData*      m_raw_data;
  DCAnalyzer*   m_dc_analyzer;
  HodoAnalyzer* m_hodo_analyzer;

public:
  void          DecodeRawData( void );
  void          DecodeDCAnalyzer( void );
  void          DecodeHodoAnalyzer( void );

  RawData*      GetRawData( void ) const { return m_raw_data; }
  DCAnalyzer*   GetDCAnalyzer( void ) const { return m_dc_analyzer; }
  HodoAnalyzer* GetHodoAnalyzer( void ) const { return m_hodo_analyzer; }

  Bool_t        TrackSearchBcOut( void );

  ClassDef(EventAnalyzer,0);
};

}

#endif
