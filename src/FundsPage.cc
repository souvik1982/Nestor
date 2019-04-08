#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <unistd.h>

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
  
  NestorInvestment *fund_INX = new NestorInvestment(getData, "INX", fundsToMonitor); // getData
  fund_INX->candleStickGraph();
  fund_INX->saveCanvas();
  
  ofstream outfile((fundsToMonitor+"/index.html").c_str());
  outfile<<"<html>"<<endl;
  outfile<<"<body>"<<endl;
  outfile<<"<table border='1'>"<<endl;
  
  while (monitor.readRow(&v_row))
  {
    string fundname=v_row[0];    
    if (fundname[0]=='#') // Comment, to be displayed if text
    {
      outfile<<"<tr>"<<endl;
      outfile<<" <td>"<<endl;
      outfile<<"  <h3>"<<v_row[1]<<"</h3>"<<endl;
      outfile<<" </td>"<<endl;
      outfile<<"</tr>"<<endl;
      continue;
    }
    
    NestorInvestment *fund = new NestorInvestment(getData, fundname, fundsToMonitor);
    fund->plotRatioToINX(fund_INX);
    fund->saveCanvas();
    delete fund;
    
    outfile<<"<tr>"<<endl;
    outfile<<" <td>"<<endl;
    outfile<<"  <img src='"<<("c_"+fundname+".png")<<"'/>"<<endl;
    outfile<<" </td>"<<endl;
    outfile<<"</tr>"<<endl;
    
    sleep(12);
  }
  
  outfile<<"</table>"<<endl;
  outfile<<"</body>"<<endl;
  outfile<<"</html>"<<endl;
  outfile.close();
}   
