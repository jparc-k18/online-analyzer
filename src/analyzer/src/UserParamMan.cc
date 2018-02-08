// -*- C++ -*-

#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>

#include <std_ostream.hh>

#include "ConfMan.hh"
#include "FuncName.hh"
#include "UserParamMan.hh"

ClassImp(UserParamMan);

//______________________________________________________________________________
void
ConfMan::InitializeUserParamMan( void )
{
  if(name_file_["USER:"] != ""){
    UserParamMan& gUserParam = UserParamMan::GetInstance();
    flag_[kIsGood] = gUserParam.Initialize(name_file_["USER:"]);
  }else{
    std::cout << "#E ConfMan::"
	      << " File path does not exist in " << name_file_["USER:"]
	      << std::endl;
    flag_.reset(kIsGood);
  }
}

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
    std::cerr << "#E " << FUNC_NAME
	      << " No such parameter name (" << name << ")"
	      << std::endl;
    return -1;
  }
}

//______________________________________________________________________________
Double_t
UserParamMan::GetParameter( const TString& pName, Int_t index ) const
{
  mapType::const_iterator itr = param_container_.find(pName);
  if( itr != param_container_.end() ){
    return itr->second.at(index);
  } else {
    std::cerr << "#E " << FUNC_NAME
	      << " No such parameter name (" << pName << ")"
	      << std::endl;
    return -9999.;
  }
}

//______________________________________________________________________________
Bool_t
UserParamMan::Initialize( const TString& filename )
{
  std::ifstream ifs( filename );
  if( !ifs.is_open() ){
    hddaq::cerr << "#E " << FUNC_NAME
		<< " No such parameter file (" << filename << ")"
		<< std::endl;
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

  std::cout << "#D " << FUNC_NAME << " Initialized" << std::endl;

  return true;
}
