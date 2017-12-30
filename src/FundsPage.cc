#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "../CSVInterface/CSVReaderInterface.h"
#include "../interface/NestorInvestment.h"

using namespace std;

void FundsPage(string fundsToMonitor)
{
  // Read configuration file for stock data
  CSVReaderInterface monitor(fundsToMonitor+".txt");
  vector<string> v_row;
  monitor.readRow(&v_row);
  string getData=v_row.at(0);
  
  NestorInvestment *fund_INX = new NestorInvestment(getData, "INX", fundsToMonitor);
  
  ofstream outfile((fundsToMonitor+"/index.html").c_str());
  outfile<<"<html>"<<endl;
  outfile<<"<body>"<<endl;
  outfile<<"<table border='1'>"<<endl;
  
  while (monitor.readRow(&v_row))
  {
    string fundname=v_row[0];    
    if (fundname[0]=='#') continue; // comment
    
    NestorInvestment *fund = new NestorInvestment(getData, fundname, fundsToMonitor);
    fund->plotRatioToINX(fund_INX);
    fund->saveCanvas();
    delete fund;
    
    outfile<<"<tr>"<<endl;
    outfile<<" <td>"<<endl;
    outfile<<"  <img src='"<<("c_"+fundname+".png")<<"'/>"<<endl;
    outfile<<" </td>"<<endl;
    outfile<<"</tr>"<<endl;
  }
  
  outfile<<"</table>"<<endl;
  outfile<<"</body>"<<endl;
  outfile<<"</html>"<<endl;
  outfile.close();
}   
