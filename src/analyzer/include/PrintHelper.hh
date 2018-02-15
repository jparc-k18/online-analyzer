/**
 *  file: PrintHelper.hh
 *  date: 2017.03.14
 *
 */

#ifndef PRINT_HELPER_HH
#define PRINT_HELPER_HH

#include <iomanip>
#include <iostream>
#include <string>

#include <escape_sequence.hh>
#include <std_ostream.hh>

//______________________________________________________________________________
class PrintHelper
{
public:
  PrintHelper( std::size_t precision,
	       std::ios::fmtflags flags,
	       std::ostream& ost=hddaq::cout )
    : m_ostream(ost),
      m_precision(ost.precision()),
      m_flags(ost.flags())
  {
    m_ostream.precision(precision);
    m_ostream.flags(flags);
  }

  PrintHelper( const std::string& escape,
	       std::ostream& ost=hddaq::cout )
    : m_ostream(ost),
      m_precision(),
      m_flags()
  {
    m_ostream << escape;
  }

  ~PrintHelper( void )
  {
    m_ostream.precision(m_precision);
    m_ostream.flags(m_flags);
    m_ostream << hddaq::unpacker::esc::k_default_color;
  }

private:
  std::ostream&      m_ostream;
  std::size_t        m_precision;
  std::ios::fmtflags m_flags;

public:
  void precision( std::size_t precision );
  void setf( std::ios::fmtflags flags );
  void set( std::size_t precision, std::ios::fmtflags flags );
  void set( const std::string& escape );

};

//______________________________________________________________________________
inline void
PrintHelper::precision( std::size_t precision )
{
  m_ostream.precision(precision);
}

//______________________________________________________________________________
inline void
PrintHelper::setf( std::ios::fmtflags flags )
{
  m_ostream.setf(flags);
}

//______________________________________________________________________________
inline void
PrintHelper::set( std::size_t precision, std::ios::fmtflags flags )
{
  m_ostream.precision(precision);
  m_ostream.setf(flags);
}

//______________________________________________________________________________
inline void
PrintHelper::set( const std::string& escape )
{
  m_ostream << escape;
}

#endif
