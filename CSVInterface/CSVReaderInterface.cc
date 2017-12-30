#include "CSVReaderInterface.h"

#include <fstream>
#include <sstream>

CSVReaderInterface::CSVReaderInterface(std::string filename, std::string delimiter)
{
  inFile_=new std::ifstream(filename.c_str());
  delimiter_=delimiter;
}

CSVReaderInterface::~CSVReaderInterface()
{
  delete inFile_;
}

bool CSVReaderInterface::readRow(std::vector<std::string> *v_row)
{
  v_row->clear();
  if (inFile_->good())
  {
    std::string line;
    std::getline(*inFile_, line);
    std::stringstream s_line(line);
    std::string cell;
    while (std::getline(s_line, cell, delimiter_.c_str()[0]))
    {
      v_row->push_back(cell);
    }
    if (v_row->size()>0) return true;
  }
  return false;
}

bool CSVReaderInterface::ignoreRow()
{
  if (inFile_->good()) 
  {
    std::string line;
    std::getline(*inFile_, line);
    return true;
  }
  else return false;
}
