// Author: Tomonori Takaahshi

#ifndef USER_ANALYZER_H
#define USER_ANALYZER_H

#include <string>
#include <vector>


namespace analyzer
{
  void checkFileExistence(const std::string& filename);
  void closeTFile(int arg=0);
  int  process_begin(const std::vector<std::string>& argv);
  int  process_end();
  int  process_event();
}

#endif
