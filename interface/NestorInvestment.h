/* 
==================================================
Nestor
 
An investment analysis program for private use.
 
Copyright (C) 8 December 2017 Souvik Das
ALL RIGHTS RESERVED
=================================================
*/

#pragma once

#include <stdlib.h>
#include <algorithm>

#include "TDatime.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraph.h"

class NestorInvestment
{
  public:
  
    // getData: "update" or "fulldownload"
    NestorInvestment(std::string getData, std::string stockname, std::string directory);
    ~NestorInvestment();
    
    void candleStickGraph();
    void plotRatioToINX(NestorInvestment *inx);
    void movingAverage50();
    void movingAverage100();
    void movingAverage150();
    void movingAverage200();
    void saveCanvas();
  
    // Member data
    std::string name_, directory_;
    TDatime startTime_;
    std::vector<float> *v_dates_=new std::vector<float>;
    std::vector<float> *v_price_open_=new std::vector<float>;
    std::vector<float> *v_price_hi_=new std::vector<float>;
    std::vector<float> *v_price_lo_=new std::vector<float>;
    std::vector<float> *v_price_close_=new std::vector<float>;
    TCanvas *canvas_=new TCanvas();
    TLegend *leg_=new TLegend();
    std::vector<float> *v_price_avg_=new std::vector<float>;
    
    TGraph *g_movingAverage50_=new TGraph();
    TGraph *g_movingAverage100_=new TGraph();
    TGraph *g_movingAverage150_=new TGraph();
    TGraph *g_movingAverage200_=new TGraph();
    
    TGraph *g_dmovingAverage50_=new TGraph();
    TGraph *g_dmovingAverage100_=new TGraph();
    TGraph *g_dmovingAverage150_=new TGraph();
    TGraph *g_dmovingAverage200_=new TGraph();
    
    TGraph *g_ddmovingAverage50_=new TGraph();
    TGraph *g_ddmovingAverage100_=new TGraph();
    TGraph *g_ddmovingAverage150_=new TGraph();
    TGraph *g_ddmovingAverage200_=new TGraph();
    
    bool movingAverage(unsigned int n, TGraph*& g_movingAverage, 
                                       TGraph*& g_dmovingAverage,
                                       TGraph*& g_ddmovingAverage);
};
    
    
