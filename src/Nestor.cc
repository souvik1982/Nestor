#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "../CSVInterface/CSVReaderInterface.h"
#include "../interface/NestorInvestment.h"

#include "StocksPage.cc"
#include "FundsPage.cc"

using namespace std;

int main()
{
  StocksPage("Stocks");
  // FundsPage("FundsValic");
  FundsPage("FundsFidelity");
  
  return 0;
} 
