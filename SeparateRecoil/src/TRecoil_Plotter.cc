//
// ==================================================================== 
//  
//      Project: sconsNS2
//               ~~~~~~
//  
//      File: TRecoil_Plotter.cc
//            ~~~~~~~~~~~~~~~~~~~
//
//      Description:     Class to make plots of results from TNestedSample
//  
//      Author: Stefanie Lewis <s.lewis@physics.gla.ac.uk>
//      Update: 2011-07-26 15:36:46z
//
//      Copyright: 2011 Stefanie Lewis
//
//      $Id$ 
//
//
//
//
// ==================================================================== 

#ifndef sconsNS2_TSimplePhysics
#include "TRecoil_Plotter.h"
#endif

using namespace std;



// Constructor:
//______________________________________________________________________________
TRecoil_Plotter::TRecoil_Plotter()
{
  
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetStatX(0.9); 
  gStyle->SetStatY(0.9);
  gStyle->SetStatW(0.4);
  gStyle->SetStatH(0.26);
  gStyle->SetStatBorderSize(1);
  gStyle->SetStatColor(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(1);
  gStyle->SetFrameBorderMode(0);
 
  // default constructor
}
//______________________________________________________________________________
// second constructor
TRecoil_Plotter::TRecoil_Plotter(char* priorFile, char* postFile, int points)
{
  
  //gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetStatX(0.9); 
  gStyle->SetStatY(0.9);
  gStyle->SetStatW(0.4);
  gStyle->SetStatH(0.26);
  gStyle->SetStatBorderSize(1);
  gStyle->SetStatColor(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(1);
  gStyle->SetFrameBorderMode(0);
  
  // Put variable initialisations here. 
  nPoints = points;
  // Start with prior:

  Rprior    = new double[nPoints];
  LogLprior = new double[nPoints];
  
  
  for (int i = 0; i < nPoints; i++){
    Rprior[i] = 0.0;
    LogLprior[i] = 0.0;
  }
  
  inPrior.open(priorFile);
  for (int i = 0; i < nPoints; i++){
    inPrior >> Rprior[i];
    inPrior >> LogLprior[i];
  }
  
  inPrior.close();
  
  // Posterior:
  f = (TFile*)gROOT->GetListOfFiles()->FindObject(postFile);
  if (!f) {
    f = new TFile(postFile);
  }
  
  T = (TTree*)gDirectory->Get("tree");
  
    // Set branch addresses.
  T->SetBranchAddress("a1_Re",  &a1_Re);
  T->SetBranchAddress("a1_Im",  &a1_Im);
  T->SetBranchAddress("a2_Re",  &a2_Re);
  T->SetBranchAddress("a2_Im",  &a2_Im);
  T->SetBranchAddress("a3_Re",  &a3_Re);
  T->SetBranchAddress("a3_Im",  &a3_Im);
  T->SetBranchAddress("a4_Re",  &a4_Re);
  T->SetBranchAddress("a4_Im",  &a4_Im);
  T->SetBranchAddress("R_obs",  &R_obs);
  T->SetBranchAddress("logL",   &logL );
  T->SetBranchAddress("logWt",  &logWt);
  
  nentries = T->GetEntries();
  maxLogWt = -100000000;

  // Calculate max Log(weight)
  for (Long64_t i=0; i<nentries;i++) 
  {
    T->GetEntry(i);
    if ((logL +logWt) > maxLogWt)
      //       cout << LogWt[j] << endl;
    maxLogWt = (logL + logWt);
  }
  
  
}
//______________________________________________________________________________
//
void TRecoil_Plotter::PlotPriorPost(bool norm, char* saveFile, float* lines)
{
  // This method plots the prior and posterior on the same canvas.
  // Inputs: norm = true -> distributions will be normalised
  //         saveFile = filename for plot to be saved as.
  
  priorR    = new TH1F("priorR"  , "R" , 100, -1.1, 1.1);
  postR     = new TH1F("postR"   , "R" , 100, -1.1, 1.1);
  
  TF1 *fp1 = new TF1("fp1","gaus",-1.0,1.0);
  
  // Fill prior histogram
  for (int i = 0; i < MAX_PRIOR; ++i){
    priorR->Fill(Rprior[i]);
    
  }
  
  // Fill posterior histogram
  for (Long64_t i = 0; i<nentries; i++){
    T->GetEntry(i); 
    Double_t other = TMath::Exp(logL + logWt + TMath::Abs(maxLogWt));
    postR->Fill(R_obs, other);
    
  }
  
  if (norm){
    
    double initR_integral = priorR->Integral();
    double postR_integral = postR->Integral();

    priorR->Scale(1.0 / initR_integral);
    postR->Scale(1.0 / postR_integral);
       
  }
  
  
  // Fitting:
  postR->Fit("fp1","R");
  mean = fp1->GetParameter(1);
  mean_err = fp1->GetParError(1);
  sigma = fp1->GetParameter(2);
  sigma_err = fp1->GetParError(2);
  chi2 = fp1->GetChisquare();

//printf("Mean: %lf\t Std Dev: %lf\n",mean,sigma);
  
  //Lines:
  
   // B:
  Float_t yvals[10];
  Float_t Rxvals[10];
  
  for (int i = 0; i < 10; i++){
    yvals[i] = i*0.05;
    Rxvals[i] = lines[0];
  }
  

  TGraph *Rgr = new TGraph(10, Rxvals, yvals);
  Rgr->SetLineColor(kRed);
  Rgr->SetLineWidth(1);
  Rgr->SetMarkerSize(0);
  

    TCanvas *Robservable = new TCanvas("R_Observable","R_Observable");
    Robservable->SetFillStyle(0);
    Robservable->SetFillColor(0);
    Robservable->SetFrameFillStyle(0);
    
  
    
    // R:
    Robservable->cd(1);
    priorR->GetXaxis()->SetTitle("R");
    priorR->SetFillColor(kGreen);
    postR->SetFillColor(kBlue);
    postR->Draw("");
    Rgr->Draw("SAME");
    priorR->Draw("SAME");
    
   
    Robservable->SaveAs(saveFile);
  
}

//      End-of-file
// ====================================================================


