// -*- C++ -*-

#include <iterator>

#include "DebugCounter.hh"
#include "FuncName.hh"
#include "HodoRawHit.hh"

ClassImp(HodoRawHit);

//______________________________________________________________________________
HodoRawHit::HodoRawHit( Int_t detector_id, Int_t plane_id, Int_t segment_id )
  : TObject(),
    m_detector_id(detector_id),
    m_plane_id(plane_id),
    m_segment_id(segment_id),
    m_adc1(1, -1),
    m_adc2(1, -1),
    m_tdc1(1, -1),
    m_tdc2(1, -1),
    m_nhtdc(0)
{
  debug::ObjectCounter::Increase(ClassName());
}

//______________________________________________________________________________
HodoRawHit::~HodoRawHit( void )
{
  debug::ObjectCounter::Decrease(ClassName());
}

//______________________________________________________________________________
void
HodoRawHit::SetAdc1( Int_t adc )
{
  if( m_adc1.at(0) == -1 )
    m_adc1.at(0) = adc;
  else
    m_adc1.push_back(adc);
}

//______________________________________________________________________________
void
HodoRawHit::SetAdc2( Int_t adc )
{
  if( m_adc2.at(0) == -1 )
    m_adc2.at(0) = adc;
  else
    m_adc2.push_back(adc);
}

//______________________________________________________________________________
void
HodoRawHit::SetTdc1( Int_t tdc )
{
  if( m_tdc1.at(0) == -1 ){
    m_tdc1.at(0) = tdc;
    ++m_nhtdc;
  }else{
    m_tdc1.push_back(tdc);
  }
}

//______________________________________________________________________________
void
HodoRawHit::SetTdc2( Int_t tdc )
{
  if( m_tdc2.at(0) == -1 ){
    m_tdc2.at(0) = tdc;
    ++m_nhtdc;
  }else{
    m_tdc2.push_back(tdc);
  }
}

//______________________________________________________________________________
Int_t
HodoRawHit::SizeAdc1( void ) const
{
  if( m_adc1.at(0) == -1 )
    return 0;
  else
    return m_adc1.size();
}

//______________________________________________________________________________
Int_t
HodoRawHit::SizeAdc2( void ) const
{
  if( m_adc2.at(0) == -1 )
    return 0;
  else
    return m_adc2.size();
}

//______________________________________________________________________________
Int_t
HodoRawHit::SizeTdc1( void ) const
{
  if( m_tdc1.at(0) == -1 )
    return 0;
  else
    return m_tdc1.size();
}

//______________________________________________________________________________
Int_t
HodoRawHit::SizeTdc2( void ) const
{
  if( m_tdc2.at(0) == -1 )
    return 0;
  else
    return m_tdc2.size();
}

//______________________________________________________________________________
void
HodoRawHit::Clear( Option_t* )
{
  m_nhtdc = 0;
  m_adc1.clear();
  m_adc2.clear();
  m_tdc1.clear();
  m_tdc2.clear();
  m_adc1.push_back(-1);
  m_adc2.push_back(-1);
  m_tdc1.push_back(-1);
  m_tdc2.push_back(-1);
}

//______________________________________________________________________________
void
HodoRawHit::Print( Option_t* ) const
{
  hddaq::cerr << FUNC_NAME << std::endl
	      << "detector_id = " << m_detector_id << std::endl
	      << "plane_id    = " << m_plane_id    << std::endl
	      << "segment_id  = " << m_segment_id   << std::endl;

  std::vector<Int_t>::const_iterator itr, end;
  hddaq::cout << "adc1        = " << m_adc1.size() << " ";
  std::copy( m_adc1.begin(), m_adc1.end(),
	     std::ostream_iterator<Int_t>(hddaq::cout," ") );
  hddaq::cout << std::endl
	      << "adc2        = " << m_adc2.size() << " ";
  std::copy( m_adc2.begin(), m_adc2.end(),
	     std::ostream_iterator<Int_t>(hddaq::cout," ") );
  hddaq::cout << std::endl
	      << "tdc1        = " << m_tdc1.size() << " ";
  std::copy( m_tdc1.begin(), m_tdc1.end(),
	     std::ostream_iterator<Int_t>(hddaq::cout," ") );
  hddaq::cout << std::endl
	      << "tdc2        = " << m_tdc2.size() << " ";
  std::copy( m_tdc2.begin(), m_tdc2.end(),
	     std::ostream_iterator<Int_t>(hddaq::cout," ") );
  hddaq::cout << std::endl;
}
