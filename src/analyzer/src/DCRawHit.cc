// -*- C++ -*-

#include <iostream>
#include <iterator>

#include "DebugCounter.hh"
#include "DCRawHit.hh"
#include "FuncName.hh"

ClassImp(DCRawHit);

//______________________________________________________________________________
DCRawHit::DCRawHit( Int_t plane_id, Int_t wire_id )
  : TObject(),
    m_plane_id(plane_id),
    m_wire_id(wire_id),
    m_tdc(),
    m_trailing()
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
DCRawHit::~DCRawHit( void )
{
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
void
DCRawHit::Print( Option_t* ) const
{
  std::cerr << FUNC_NAME << std::endl
	    << "plane_id = " << m_plane_id    << std::endl
	    << "wire_id  = " << m_wire_id     << std::endl;

  std::vector<Int_t>::const_iterator itr, end;
  std::cout << "tdc      = " << m_tdc.size() << " ";
  std::copy( m_tdc.begin(), m_tdc.end(),
	     std::ostream_iterator<Int_t>(std::cout," ") );
  std::cout << std::endl
	    << "trailing = " << m_trailing.size() << " ";
  std::copy( m_trailing.begin(), m_trailing.end(),
	     std::ostream_iterator<Int_t>(std::cout," ") );
  std::cout << std::endl;
}
