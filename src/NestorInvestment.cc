#include <iostream>

#include "TGraphAsymmErrors.h"
#include "TH1F.h"
#include "TAxis.h"
#include "TGraph.h"

#include "../CSVInterface/CSVReaderInterface.h"
#include "../CSVInterface/CSVWriterInterface.h"
#include "../interface/NestorInvestment.h"

using namespace std;

NestorInvestment::NestorInvestment(std::string getData, string stockname, string directory)
{
  name_=stockname;
  directory_=directory;
  
  vector<string> v_headerRow;
  vector<vector<string>> v_v_timeOrderedRows;
  
  bool b_download=false;
  
  // Download last 20 years of daily data
  // Order it by increasing time
  if (getData=="fulldownload") // Get full daily data 
  {
    std::string inputFilename=name_+"_temp.csv";
    std::cout<<"LOG: Downloading up to last 20 years of daily trades for "<<name_<<" in "<<inputFilename<<std::endl;
    std::string systemCommand="curl -o "+inputFilename+" \"https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol="+name_+"&outputsize=full&apikey=48M48LLJEW4TF5SF&datatype=csv\"";
    system(systemCommand.c_str());
    std::cout<<"LOG: Executed full download command."<<std::endl;
    
    // Reverse order of information and write out to Nestor CSV file
    CSVReaderInterface csvFile(inputFilename);
    vector<vector<string>> v_v_row;
    vector<string> v_row;
    if (csvFile.readRow(&v_headerRow)) 
    {
      if (v_headerRow.at(0)=="timestamp")
      {
        while (csvFile.readRow(&v_row)) v_v_row.push_back(v_row);
        
        std::cout<<"LOG: Deleting temporary file "<<inputFilename<<std::endl;
        systemCommand="rm "+inputFilename;
        system(systemCommand.c_str());
    
        std::cout<<"LOG: Creating directory called "<<directory_<<" if it doesn't exist."<<std::endl;
        systemCommand="mkdir -p "+directory_;
        system(systemCommand.c_str());
    
        std::string outputFilename=directory_+"/"+name_+"_Nestor.csv";
        CSVWriterInterface csvOutputFile(outputFilename, std::ofstream::out);
        csvOutputFile.writeRow(&v_headerRow);
        for (int i=v_v_row.size()-1; i>=0; --i) 
        {
          v_v_timeOrderedRows.push_back(v_v_row.at(i));
          csvOutputFile.writeRow(&(v_v_row.at(i)));
        }
        
        b_download=true;
      }
      else
      {
        std::cout<<"ERROR: CSV file "<<inputFilename<<" is corrupted. Incorrect download."<<std::endl;
      }
    }
    else
    {
      std::cout<<"ERROR: CSV file "<<inputFilename<<" could not be opened for reading."<<std::endl;
    }
  }
  
  // Download last 100 trading days
  else if (getData=="update")
  {
    std::string inputFilename=name_+"_temp.csv";
    std::cout<<"LOG: Downloading last 100 days of daily trades for "<<name_<<" in "<<inputFilename<<std::endl;
    std::string systemCommand="curl -o "+inputFilename+" \"https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol="+name_+"&outputsize=compact&apikey=48M48LLJEW4TF5SF&datatype=csv\"";
    system(systemCommand.c_str());
    std::cout<<"LOG: Executed compact download command."<<std::endl;
    
    // Find the last date
    std::string persistentFilename=directory_+"/"+name_+"_Nestor.csv";
    CSVReaderInterface csvPersistentFileInput(persistentFilename);
    if (csvPersistentFileInput.ignoreRow())
    {
      vector<string> v_row;
      while (csvPersistentFileInput.readRow(&v_row)) v_v_timeOrderedRows.push_back(v_row);
      std::cout<<"Last trading date in persistent record is "<<v_row[0]<<std::endl;
      TDatime t_lastPersistentDate((v_row[0]+" 00:00:00").c_str());
      
      CSVReaderInterface csvLatestFileInput(inputFilename);
      csvLatestFileInput.ignoreRow();
      vector<vector<string>> v_v_appendRows;
      while (csvLatestFileInput.readRow(&v_row))
      {
        TDatime t_date((v_row[0]+" 00:00:00").c_str());
        if (t_date<=t_lastPersistentDate) break;
        std::cout<<"v_row[1] = "<<v_row[1]<<std::endl;
        v_v_appendRows.push_back(v_row);
      }
      
      CSVWriterInterface csvPersistentFileOutput(persistentFilename, std::ofstream::app);
      for (int i=v_v_appendRows.size()-1; i>=0; --i)
      {
        v_v_timeOrderedRows.push_back(v_v_appendRows.at(i));
        csvPersistentFileOutput.writeRow(&(v_v_appendRows.at(i)));
      }
      b_download=true;
    }
    else
    {
      std::cout<<"WARNING: Long term trading data has not been downloaded for "<<name_<<" stock. Please use the 'fulldownload' option first."<<std::endl;
    }
  }
  
  else
  {
    // Just read the CSV file and load member data
  }
  
  // Load member data
  if (b_download)
  {
    for (int i=v_v_timeOrderedRows.size()-1; i>=0; --i)
    {
      string sqlDate=v_v_timeOrderedRows.at(i).at(0)+" 00:00:00";
      TDatime t_date(sqlDate.c_str());
      v_dates_->push_back(t_date.Convert());
      float price_open=atof(v_v_timeOrderedRows.at(i).at(1).c_str());
      v_price_open_->push_back(price_open);
      v_price_hi_->push_back(atof(v_v_timeOrderedRows.at(i).at(2).c_str()));
      v_price_lo_->push_back(atof(v_v_timeOrderedRows.at(i).at(3).c_str()));
      float price_close=atof(v_v_timeOrderedRows.at(i).at(4).c_str());
      v_price_close_->push_back(price_close);
      v_price_avg_->push_back((price_open+price_close)/2.);
    }
  }
}

NestorInvestment::~NestorInvestment()
{
  delete v_dates_;
  delete v_price_open_;
  delete v_price_hi_;
  delete v_price_lo_;
  delete v_price_close_;
  delete v_price_avg_;
  
  delete canvas_;
  delete leg_;
  
  delete g_movingAverage50_;
  /*delete g_movingAverage100_;
  delete g_movingAverage150_;
  delete g_movingAverage200_;*/

  delete g_dmovingAverage50_;
  /*delete g_dmovingAverage100_;
  delete g_dmovingAverage150_;
  delete g_dmovingAverage200_;*/

  delete g_ddmovingAverage50_;
  /*delete g_ddmovingAverage100_;
  delete g_ddmovingAverage150_;
  delete g_ddmovingAverage200_;*/
}

void NestorInvestment::candleStickGraph()
{
  vector<float> *v_priceErr_open=new vector<float>;
  vector<float> *v_priceErr_close=new vector<float>;
  vector<float> *v_priceErr_hi=new vector<float>;
  vector<float> *v_priceErr_lo=new vector<float>;
  
  vector<float> *v_Bear_dates=new vector<float>;
  vector<float> *v_Bear_price_avg=new vector<float>;
  vector<float> *v_Bear_priceErr_open=new vector<float>;
  vector<float> *v_Bear_priceErr_close=new vector<float>;
  
  for (unsigned int i=0; i<v_dates_->size(); ++i)
  {
    float price_open=v_price_open_->at(i);
    float price_close=v_price_close_->at(i);
    float price_hi=v_price_hi_->at(i);
    float price_lo=v_price_lo_->at(i);
    
    float price_avg=v_price_avg_->at(i);
    float priceErr_open=fabs(price_open-price_avg);
    float priceErr_close=fabs(price_avg-price_close);
    float priceErr_hi=price_hi-price_avg;
    float priceErr_lo=price_avg-price_lo;
    
    v_priceErr_open->push_back(priceErr_open);
    v_priceErr_close->push_back(priceErr_close);
    v_priceErr_hi->push_back(priceErr_hi);
    v_priceErr_lo->push_back(priceErr_lo);
    
    if (price_close<price_open) // Bear
    {
      v_Bear_dates->push_back(v_dates_->at(i));
      v_Bear_price_avg->push_back(price_avg);
      v_Bear_priceErr_open->push_back(priceErr_open);
      v_Bear_priceErr_close->push_back(priceErr_close);
    }
  }
  
  TGraphAsymmErrors *g_hiLo=new TGraphAsymmErrors(180, 
                                                  &(v_dates_->at(0)), 
                                                  &(v_price_avg_->at(0)),
                                                  0, 
                                                  0, 
                                                  &(v_priceErr_lo->at(0)), 
                                                  &(v_priceErr_hi->at(0)));
  g_hiLo->SetName(("g_"+name_).c_str());
  g_hiLo->SetTitle((name_+"; time (mm/dd/yy); value ($)").c_str());
  g_hiLo->GetXaxis()->SetTimeDisplay(1);
  g_hiLo->GetXaxis()->SetTimeFormat("%m/%d/%y%F2000-02-28");
  g_hiLo->SetLineWidth(1);
  
  TGraphAsymmErrors *g_openClose=new TGraphAsymmErrors(180, 
                                                       &(v_dates_->at(0)), 
                                                       &(v_price_avg_->at(0)),
                                                       0, 
                                                       0, 
                                                       &(v_priceErr_close->at(0)), 
                                                       &(v_priceErr_open->at(0)));
  g_openClose->SetMarkerStyle(6);
  g_openClose->SetLineColor(kGreen);
  g_openClose->SetLineWidth(5);
  g_openClose->SetMarkerColor(kGreen);
  
  TGraphAsymmErrors *g_Bear_openClose=new TGraphAsymmErrors(180, 
                                                            &(v_Bear_dates->at(0)), 
                                                            &(v_Bear_price_avg->at(0)),
                                                            0, 
                                                            0, 
                                                            &(v_Bear_priceErr_close->at(0)), 
                                                            &(v_Bear_priceErr_open->at(0)));
  g_Bear_openClose->SetMarkerStyle(6);
  g_Bear_openClose->SetLineColor(kRed);
  g_Bear_openClose->SetLineWidth(5);
  g_Bear_openClose->SetMarkerColor(kRed);
  
  leg_=new TLegend(0.11, 0.89, 0.35, 0.65);
  leg_->SetFillColor(0); leg_->SetLineColor(0);
  leg_->Draw();
  
  canvas_=new TCanvas(name_.c_str(), name_.c_str(), 3000, 700);
  canvas_->cd();
  g_hiLo->Draw("AP");
  g_openClose->Draw("SAME P");
  g_Bear_openClose->Draw("SAME P");
  canvas_->Update();
}

void NestorInvestment::plotRatioToINX(NestorInvestment *inx)
{
  std::vector<float> v_ratioToINX0;
  std::vector<float> v_ratioToStock0;
  
  float inx0=inx->v_price_avg_->at(0);
  for (unsigned int i=0; i<inx->v_price_avg_->size(); ++i) v_ratioToINX0.push_back(inx->v_price_avg_->at(i)/inx0);
  
  float stock0=v_price_avg_->at(0);
  for (unsigned int i=0; i<v_price_avg_->size(); ++i) v_ratioToStock0.push_back(v_price_avg_->at(i)/stock0);
  
  TGraph *g_inx=new TGraph(inx->v_price_avg_->size(), &(inx->v_dates_->at(0)), &(v_ratioToINX0.at(0)));
  TGraph *g_fund=new TGraph(v_price_avg_->size(), &(v_dates_->at(0)), &(v_ratioToStock0.at(0))); 
  
  g_inx->SetMarkerStyle(6);
  g_fund->SetLineColor(kRed);
  g_fund->SetMarkerStyle(6);
  g_fund->SetMarkerColor(kRed);
  
  leg_=new TLegend(0.65, 0.35, 0.89, 0.11);
  leg_->AddEntry(g_inx, "INX / INX(0)", "lep");
  leg_->AddEntry(g_fund, (name_+" / "+name_+"(0)").c_str(), "lep");
  leg_->SetFillColor(0); leg_->SetLineColor(0);
  
  canvas_=new TCanvas(name_.c_str(), name_.c_str(), 2000, 700);
  g_inx->SetMaximum(std::max(g_inx->GetHistogram()->GetMaximum(), g_fund->GetHistogram()->GetMaximum()));
  g_inx->SetMinimum(std::min(g_inx->GetHistogram()->GetMinimum(), g_fund->GetHistogram()->GetMinimum()));
  g_inx->SetName("g_inx");
  g_inx->SetTitle((name_+" / "+name_+"(0), INX / INX(0); time (mm/dd/yy); value / value today").c_str());
  g_inx->GetXaxis()->SetTimeDisplay(1);
  g_inx->GetXaxis()->SetTimeFormat("%m/%d/%y%F2000-02-28");
  g_inx->Draw("ALP");
  g_fund->Draw("LP");
  leg_->Draw();
  canvas_->Update();
} 

bool NestorInvestment::movingAverage(unsigned int n, TGraph*& g_movingAverage, 
                                                     TGraph*& g_dmovingAverage,
                                                     TGraph*& g_ddmovingAverage)
{
  std::vector<float> v_movingAverage;
  unsigned int nDays=v_dates_->size();
  bool computed=false;
  if (nDays>=n)
  {
    for (unsigned int i=0; i<=nDays-n; ++i)
    {
      float avg=0;
      for (unsigned int j=i; j<i+n; ++j)
      {
        avg+=v_price_avg_->at(j);
      }
      avg/=n;
      v_movingAverage.push_back(avg);
    }
    g_movingAverage=new TGraph(v_movingAverage.size(), &(v_dates_->at(0)), &(v_movingAverage.at(0)));
    computed=true;
  }
  return computed;
}

/*TGraph* derivative(TGraph *g)
{
}
*/

void NestorInvestment::movingAverage50()
{
  if (movingAverage(50, g_movingAverage50_, g_dmovingAverage50_, g_ddmovingAverage50_))
  {
    g_movingAverage50_->SetLineColor(kCyan);
    g_movingAverage50_->Draw("LP");
    leg_->AddEntry(g_movingAverage50_, "SMA50", "lep");
    leg_->Draw();
    canvas_->Update();
  }
}

void NestorInvestment::movingAverage100()
{
  if (movingAverage(100, g_movingAverage100_, g_dmovingAverage100_, g_ddmovingAverage100_))
  {
    g_movingAverage100_->SetLineColor(kBlue);
    g_movingAverage100_->Draw("LP");
    leg_->AddEntry(g_movingAverage100_, "SMA100", "lep");
    leg_->Draw();
    canvas_->Update();
  }
}

void NestorInvestment::movingAverage150()
{
  if (movingAverage(150, g_movingAverage150_, g_dmovingAverage150_, g_ddmovingAverage150_))
  {
    g_movingAverage150_->SetLineColor(kRed);
    g_movingAverage150_->Draw("LP");
    leg_->AddEntry(g_movingAverage150_, "SMA150", "lep");
    leg_->Draw();
    canvas_->Update();
  }
}

void NestorInvestment::movingAverage200()
{
  if (movingAverage(200, g_movingAverage200_, g_dmovingAverage200_, g_ddmovingAverage200_))
  {
    g_movingAverage200_->SetLineColor(kMagenta);
    g_movingAverage200_->Draw("LP");
    leg_->AddEntry(g_movingAverage200_, "SMA200", "lep");
    leg_->Draw();
    canvas_->Update();
  }
}

void NestorInvestment::saveCanvas()
{
  canvas_->SaveAs((directory_+"/c_"+name_+".png").c_str());
}
    
    


