#pragma once

#include <string>
#include <vector>

class CSVReaderInterface
{
  public:
    CSVReaderInterface(std::string filename, std::string delimiter=",");
    ~CSVReaderInterface();
    bool readRow(std::vector<std::string> *v_row);
    bool ignoreRow();
    
  private: 
    std::ifstream *inFile_;
    std::string delimiter_;
};
