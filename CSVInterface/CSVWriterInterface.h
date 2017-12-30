#pragma once

#include <string>
#include <vector>

class CSVWriterInterface
{
  public:
    CSVWriterInterface(std::string filename, std::string delimiter=",");
    ~CSVWriterInterface();
    bool writeRow(std::vector<std::string> *v_row);
    
  private:
    std::ofstream *outFile_;
    std::string delimiter_;
};
