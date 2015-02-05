// Author: Tomonori Takahashi

#include "user_analyzer.hh"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

#include <TFile.h>

namespace analyzer
{

//______________________________________________________________________________
void
checkFileExistence(const std::string& filename)
{
  std::ifstream tmp(filename.c_str());
  if (!tmp.fail())
    {
      for (;;)
	{
	  std::string answer;
	  std::cout << "#W file: " << filename 
		    << " exists. overwrite anyway? [y/n]:"
		    << std::flush;
	  std::getline(std::cin, answer);
          if ((answer.find("y")==0) || (answer.find("Y")==0))
            break;
          else if ((answer.find("n")==0) || (answer.find("N")==0))
            {
              std::cout << "\n not overwrite.    exit" << std::endl;
              std::exit(0);
            }
	  std::cout << "\n unknown input: " << answer << "\n";
	}
    }
  return;
}

//______________________________________________________________________________
void
closeTFile(int arg)
{
  std::cout << "#D closeTFile() called" << std::endl;
  if (gFile)
    {
      std::cout << " gFile->Write()" << std::endl;
      gFile->Write();
      std::cout << " gFile->Close()" << std::endl;
      gFile->Close();
    }
  else
    std::cout << " no root file exists" << std::endl;
  return;
}

}
