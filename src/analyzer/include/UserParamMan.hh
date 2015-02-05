#ifndef UPARAMMAN_H
#define UPARAMMAN_H 1

#include<vector>
#include<map>

class UserParamMan{
  // Private parameter declarations ----------------------------------------
  typedef std::vector<double>               arrayType;
  typedef std::map<std::string, arrayType>  mapType;

  // Parameters list instance
  mapType param_container_;

public:
  // User functions --------------------------------------------------------
  ~UserParamMan();
  static UserParamMan& getInstance();

  bool   initialize(const std::string& filename);

  double getParameter(const char* name, int index=0);
  double getParameter(const std::string& name, int index=0);

  int    getSize(const char* name);
  int    getSize(const std::string& name);

  bool   isGood();

private:
  UserParamMan();
  UserParamMan(const UserParamMan& object);
  UserParamMan& operator =(const UserParamMan& object);
};

// getInstance
inline UserParamMan& UserParamMan::getInstance()
{
  static UserParamMan object;
  return object;
}

#endif
