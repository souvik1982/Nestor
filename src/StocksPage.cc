#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "../CSVInterface/CSVReaderInterface.h"
#include "../interface/NestorInvestment.h"

using namespace std;

void StocksPage(string stocksToMonitor)
{
  // Read configuration file for stock data
  CSVReaderInterface monitor(stocksToMonitor+".txt");
  vector<string> v_row;
  monitor.readRow(&v_row);
  string getData=v_row.at(0);
  
  ofstream outfile((stocksToMonitor+"/index.html").c_str());
  outfile<<"<html>"<<endl;
  outfile<<"<body>"<<endl;
  outfile<<"<table border='1'>"<<endl;
  
  while (monitor.readRow(&v_row))
  {
    string stockname=v_row[0];    
    if (stockname[0]=='#') continue; // comment
    
    NestorInvestment *stock = new NestorInvestment(getData, stockname, stocksToMonitor);
    stock->candleStickGraph();
    stock->movingAverage50();
    stock->movingAverage100();
    stock->movingAverage150();
    stock->movingAverage200();
    stock->saveCanvas();
    delete stock;
    
    outfile<<"<tr>"<<endl;
    outfile<<" <td>"<<endl;
    outfile<<"  <img src='"<<("c_"+stockname+".png")<<"'/>"<<endl;
    outfile<<" </td>"<<endl;
    outfile<<"</tr>"<<endl;
  }
  
  outfile<<"</table>"<<endl;
  outfile<<"</body>"<<endl;
  outfile<<"</html>"<<endl;
  outfile.close();
}   
