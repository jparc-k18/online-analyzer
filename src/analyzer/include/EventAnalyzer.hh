// -*- C++ -*-

#ifndef EVENT_ANALYZER_HH
#define EVENT_ANALYZER_HH

#include <TObject.h>

class RawData;
class DCAnalyzer;
class DCLocalTrack;

namespace analyzer
{

//______________________________________________________________________________
class EventAnalyzer : public TObject
{
public:
  EventAnalyzer( void );
  ~EventAnalyzer( void );

protected:
  RawData*    m_raw_data;
  DCAnalyzer* m_dc_analyzer;

public:
  void  DecodeRawData( void );
  void  DecodeDCRawHits( void );
  Int_t TrackSearchBcOut( void );

  RawData*      GetRawData( void ) const { return m_raw_data; }
  DCAnalyzer*   GetDCAnalyzer( void ) const { return m_dc_analyzer; }
  DCLocalTrack* GetTrackBcOut( Int_t i ) const;

  ClassDef(EventAnalyzer,0);
};

}

#endif
