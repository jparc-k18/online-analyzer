#include <vector>

#if defined(__MAKECINT__) || defined(__MAKECLING__)

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;
#pragma link C++ class std::vector< std::vector<double> >;
#pragma link C++ class std::vector< std::vector< std::vector<double> > >;
#pragma link C++ class MatrixParamMan;

#endif
