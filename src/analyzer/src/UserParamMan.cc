// -*- C++ -*-

#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>

#include <std_ostream.hh>

#include "ConfMan.hh"
#include "Exception.hh"
#include "FuncName.hh"
#include "UserParamMan.hh"

namespace
{
  // throw exception when no parameter is found
  const Bool_t ThrowException = true;
}

ClassImp(UserParamMan);

//______________________________________________________________________________
UserParamMan::UserParamMan( void )
  : TObject()
{
}

//______________________________________________________________________________
UserParamMan::~UserParamMan( void )
{
}

//______________________________________________________________________________
Int_t
UserParamMan::GetSize( const TString& name ) const
{
  mapType::const_iterator itr = param_container_.find(name);
  if( itr != param_container_.end() ){
    return itr->second.size();
  } else {
    if( ThrowException ){
      throw Exception( FUNC_NAME+" No such parameter : "+name );
    } else {
      std::cerr << "#W " << FUNC_NAME
		<< " No such parameter : " << name << std::endl;
      return -1;
    }
  }
}

//______________________________________________________________________________
Double_t
UserParamMan::GetParameter( const TString& name, Int_t index ) const
{
  static const Double_t default_value = -9999.;

  mapType::const_iterator itr = param_container_.find(name);
  if( itr != param_container_.end() ){
    return itr->second.at(index);
  } else {
    if( ThrowException ){
      throw Exception( FUNC_NAME+" No such parameter : "+name );
    } else {
      std::cerr << "#W " << FUNC_NAME
		<< " No such parameter : " << name << " -> "
		<< default_value << std::endl;
      return default_value;
    }
  }
}

//______________________________________________________________________________
Bool_t
UserParamMan::Initialize( const TString& filename )
{
  std::ifstream ifs( filename );
  if( !ifs.is_open() ){
    hddaq::cerr << "#E " << FUNC_NAME
		<< " No such file : " << filename << std::endl;
    return false;
  }

  TString line;
  while( ifs.good() && line.ReadLine(ifs) ){
    if( line[0] == '#' ) continue;

    // get first word
    std::istringstream LineTo( line.Data() );
    TString buf_word;
    LineTo >> buf_word;

    // This is parameter line
    TString   name_param = buf_word;
    arrayType param_array;
    Double_t  param_val;
    while( LineTo >> param_val ){
      param_array.push_back( param_val );
    }

    // insert to map
    param_container_[name_param] = param_array;
  }

  return true;
}
