// -*- C++ -*-

#ifndef DEBUG_COUNTER_HH
#define DEBUG_COUNTER_HH

#include <map>
#include <string>
#include <vector>

#include <TObject.h>
#include <TString.h>

//______________________________________________________________________________
namespace debug
{

class ObjectCounter : public TObject
{
public:
  static ObjectCounter& GetInstance( void );
  ~ObjectCounter( void );

private:
  ObjectCounter( void );
  ObjectCounter( const ObjectCounter& );
  ObjectCounter& operator =( const ObjectCounter& );

private:
  typedef std::map<TString,Int_t> ObjectMap;
  typedef ObjectMap::const_iterator ObjectIter;
  ObjectMap m_map;

public:
  void        Check( void ) const;
  void        Print( Option_t* option="" ) const;
  // static method
  static void Decrease( const TString& key );
  static void Increase( const TString& key );

  ClassDef(ObjectCounter,0);
};

//______________________________________________________________________________
inline ObjectCounter&
ObjectCounter::GetInstance( void )
{
  static ObjectCounter g_instance;
  return g_instance;
}

//_____________________________________________________________________
inline void
ObjectCounter::Decrease( const TString& key )
{
#ifdef MemoryLeak
  --(GetInstance().m_map[key]);
#endif
}

//_____________________________________________________________________
inline void
ObjectCounter::Increase( const TString& key )
{
#ifdef MemoryLeak
  ++(GetInstance().m_map[key]);
#endif
}

}

#endif
