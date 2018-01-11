#include "CSVWriterInterface.h"

#include <sstream>

CSVWriterInterface::CSVWriterInterface(std::string filename, std::ios::openmode mode, std::string delimiter)
{
  outFile_=new std::ofstream(filename.c_str(), mode);
  delimiter_=delimiter;
}

CSVWriterInterface::~CSVWriterInterface()
{
  outFile_->close();
  delete outFile_;
}

bool CSVWriterInterface::writeRow(std::vector<std::string> *v_row)
{
  if (outFile_->good())
  {
    for (unsigned int i=0; i<v_row->size()-1; ++i)
    {
      *outFile_<<v_row->at(i)<<", ";
    }
    *outFile_<<v_row->at(v_row->size()-1)<<std::endl;
    return true;
  }
  return false;
}
