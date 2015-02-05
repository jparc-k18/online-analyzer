#include<sstream>
#include<fstream>
#include<iostream>
#include<cstdlib>

#include"UserParamMan.hh"

static const std::string MyName = "UserParamMan::";

// program instance -------------------------------------------------------
// getSize (const char*)
int UserParamMan::getSize(const char* name)
{
  const std::string nameStr = name;
  return this->getSize(nameStr);
}

// getSize (const std::string)
int UserParamMan::getSize(const std::string& name)
{
  static const std::string MyFunc = "getSize ";
  int size_return = -1;

  mapType::iterator itr = param_container_.find(name);
  if(itr == param_container_.end()){
    std::cerr << "#E " << MyName << MyFunc
	      << "No such parameter name (" << name << ")"
	      << std::endl;
  }else{
    size_return = itr->second.size();
  }

  return size_return;
}

// getParameter (const char*)
double UserParamMan::getParameter(const char* pName, int index)
{
  const std::string pNameStr = pName;
  return this->getParameter(pNameStr, index);
}

// getParameter (const std::string)
double UserParamMan::getParameter(const std::string& pName, int index)
{
  static const std::string MyFunc = "getParameter ";
  double param_return = 0;
  
  mapType::iterator itr = param_container_.find(pName);
  if(itr == param_container_.end()){
    std::cerr << "#E " << MyName << MyFunc
	      << "No such parameter name (" << pName << ")"
	      << std::endl;
  }else{
    if(index +1 > static_cast<int>(itr->second.size())){
      std::cerr << "#E " << MyName << MyFunc
		<< "Invalid index (" << index << ")"
		<< std::endl;
    }else{
      // Process normal
      param_return = itr->second.at(index);
    }
  }

  return param_return;
}

// Constructor
UserParamMan::UserParamMan()
{
  
}

// Destructor
UserParamMan::~UserParamMan()
{
  
}

// Initialize
bool UserParamMan::initialize(const std::string& filename)
{
  static const std::string MyFunc = "initialize ";
  
  std::ifstream ifs(filename.c_str());
  if(!ifs.is_open()){
    std::cerr << "#E " << MyName << MyFunc
	      << "No such parameter file (" << filename << ")"
	      << std::endl;
    std::exit(-1);
  }

  std::string bufLine;
  while(getline(ifs, bufLine)){
    // Empty line
    if(bufLine.empty()){ continue; }

    // get first word
    std::istringstream LineTo(bufLine);
    std::string buf_word;
    LineTo >> buf_word;

    // This is comment line
    if(buf_word.at(0) == '#'){ continue; }

    // This is parameter line
    std::string& name_param = buf_word;
    arrayType param_array;
    double    param_val;
    while(LineTo >> param_val){
      param_array.push_back(param_val);
    }

    // insert to map
    param_container_[name_param] = param_array;
  }

  std::cout << "#D " << MyName << MyFunc
	    << "Initialized"
	    << std::endl;

  return true;
}
