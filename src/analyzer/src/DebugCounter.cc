// -*- C++ -*-

#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>

#include "FuncName.hh"
#include "DebugCounter.hh"

ClassImp(debug::ObjectCounter);

namespace debug
{

//______________________________________________________________________________
ObjectCounter::ObjectCounter( void )
  : TObject(),
    m_map()
{
}

//______________________________________________________________________________
ObjectCounter::~ObjectCounter( void )
{
}

//______________________________________________________________________________
void
ObjectCounter::CheckCounter( void ) const
{
#ifdef MemoryLeak
  Bool_t has_leak = false;
  ObjectIter itr, end=m_map.end();
  for( itr=m_map.begin(); itr!=end; ++itr ){
    if( itr->second!=0 ) has_leak = true;
  }
  if( has_leak ) Print();
#endif
}

//______________________________________________________________________________
void
ObjectCounter::PrintCounter( void ) const
{
  std::cout << "#DCounter " << FUNC_NAME << std::endl;
  ObjectIter itr, end=m_map.end();
  for( itr=m_map.begin(); itr!=end; ++itr ){
    std::cout << std::setw(20) << std::left
	      << itr->first << " : " << itr->second << std::endl;
  }
}

}
