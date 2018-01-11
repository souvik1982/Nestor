#pragma once

#include <string>
#include <vector>
#include <fstream>

class CSVWriterInterface
{
  public:
    CSVWriterInterface(std::string filename, std::ios::openmode mode = std::ios::out | std::ios::app, std::string delimiter=",");
    ~CSVWriterInterface();
    bool writeRow(std::vector<std::string> *v_row);
    
  private:
    std::ofstream *outFile_;
    std::string delimiter_;
};
