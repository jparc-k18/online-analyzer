// -*- C++ -*-

#ifndef EVENT_ANALYZER_HH
#define EVENT_ANALYZER_HH

#include <TObject.h>

class RawData;

namespace analyzer
{

//______________________________________________________________________________
class EventAnalyzer : public TObject
{
public:
  EventAnalyzer( void );
  ~EventAnalyzer( void );

protected:
  RawData *m_raw_data;

public:
  void DecodeRawData( void );

  RawData* GetRawData( void ) const { return m_raw_data; }

  ClassDef(EventAnalyzer,0);
};

}

#endif
