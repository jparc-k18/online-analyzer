
#if defined(__MAKECINT__) || defined(__MAKECLING__)

#pragma link C++ function a2s(const std::string& arg);
#pragma link C++ function a2us(const std::string& arg);
#pragma link C++ function a2i(const std::string& arg);
#pragma link C++ function a2ll(const std::string& arg);
#pragma link C++ function a2ui(const std::string& arg);
#pragma link C++ function a2ull(const std::string& arg);
#pragma link C++ function i2a(short);
#pragma link C++ function i2a(unsigned short);
#pragma link C++ function i2a(int);
#pragma link C++ function i2a(long long);
#pragma link C++ function i2a(unsigned int);
#pragma link C++ function i2a(unsigned long long);
#pragma link C++ function d2a(double);

#endif
