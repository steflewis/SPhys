//
// ==================================================================== 
//  
//      Project: sconsNS2
//               ~~~~~~
//  
//      File: TPlotter.cc
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
#include "TPlotter.h"
#endif

using namespace std;



// Constructor:
//______________________________________________________________________________
TPlotter::TPlotter()
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
TPlotter::TPlotter(char* priorFile, char* postFile, int points)
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
  Bprior    = new double[nPoints];
  Rprior    = new double[nPoints];
  Tprior    = new double[nPoints];
  Oxprior   = new double[nPoints];
  Ozprior   = new double[nPoints];
  Pprior    = new double[nPoints];
  LogLprior = new double[nPoints];
  
  
  for (int i = 0; i < nPoints; i++){
    Bprior[i] = 0.0;
    Rprior[i] = 0.0;
    Tprior[i] = 0.0;
    Oxprior[i] = 0.0;
    Ozprior[i] = 0.0;
    Pprior[i] = 0.0;
    LogLprior[i] = 0.0;
  }
  
  inPrior.open(priorFile);
  for (int i = 0; i < nPoints; i++){
    inPrior >> Bprior[i];
    inPrior >> LogLprior[i];
    inPrior >> Pprior[i];
    inPrior >> Rprior[i];
    inPrior >> Tprior[i];
    inPrior >> Oxprior[i];
    inPrior >> Ozprior[i];
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
  T->SetBranchAddress("B_obs",  &B_obs);
  T->SetBranchAddress("R_obs",  &R_obs);
  T->SetBranchAddress("T_obs",  &T_obs);
  T->SetBranchAddress("Ox_obs", &Ox_obs);
  T->SetBranchAddress("Oz_obs", &Oz_obs);
  T->SetBranchAddress("logL",   &logL );
  T->SetBranchAddress("logWt",  &logWt);
  T->SetBranchAddress("P_gam",  &P_gam);
  
  nentries = T->GetEntries();
  maxLogWt = -1000000000;

  // Calculate max Log(weight)
  for (Long64_t i=0; i<nentries;i++) 
  {
    T->GetEntry(i);
    if ((logL +logWt) > maxLogWt)
      //       cout << LogWt[j] << endl;
    maxLogWt = (logL + logWt);
  }
  cout << "Max log weight: " << maxLogWt << endl;
  
}
//______________________________________________________________________________
//
void TPlotter::PlotPriorPost(bool norm, char* saveFile, float* lines)
{
  // This method plots the prior and posterior on the same canvas.
  // Inputs: norm = true -> distributions will be normalised
  //         saveFile = filename for plot to be saved as.
  
  priorB    = new TH1F("priorB"  , "B" , 100, -1.1, 1.1);
  priorR    = new TH1F("priorR"  , "R" , 100, -1.1, 1.1);
  priorT    = new TH1F("priorT"  , "T" , 100, -1.1, 1.1);
  priorOx   = new TH1F("priorOx" , "Ox", 100, -1.1, 1.1);
  priorOz   = new TH1F("priorOz" , "Oz", 100, -1.1, 1.1);
  priorPg   = new TH1F("priorPg" , "Pg", 100, -1.1, 1.1);
  
  postB     = new TH1F("postB"   , "B" , 100, -1.1, 1.1);
  postR     = new TH1F("postR"   , "R" , 100, -1.1, 1.1);
  postT     = new TH1F("postT"  , "T" , 100, -1.1, 1.1);
  postOx    = new TH1F("postOx" , "Ox", 100, -1.1, 1.1);
  postOz    = new TH1F("postOz" , "Oz", 100, -1.1, 1.1);
  postPg    = new TH1F("postPg" , "Pg", 100, -1.1, 1.1);
  

  
  // Fill prior histogram
  for (int i = 0; i < MAX_PRIOR; ++i){
    priorB->Fill(Bprior[i]);
    priorR->Fill(Rprior[i]);
    priorT->Fill(Tprior[i]);
    priorOx->Fill(Oxprior[i]);
    priorOz->Fill(Ozprior[i]);
    priorPg->Fill(Pprior[i]);
    
  }
  
  // Fill posterior histogram
  for (Long64_t i = 0; i<nentries; i++){
    T->GetEntry(i); 
    Double_t other = TMath::Exp((logL + logWt + TMath::Abs(maxLogWt)));
    //Double_t other = TMath::Exp((logL + logWt)*(pow(10,-7.0)));
    postB->Fill(B_obs, other);
    postR->Fill(R_obs, other);
    postT->Fill(T_obs, other);
    postOx->Fill(Ox_obs, other);
    postOz->Fill(Oz_obs, other);
    postPg->Fill(P_gam, other);
    
  }
  
  if (norm){
    double initB_integral = priorB->Integral();
    double postB_integral = postB->Integral();
    
    double initR_integral = priorR->Integral();
    double postR_integral = postR->Integral();
    
    double initT_integral = priorT->Integral();
    double postT_integral = postT->Integral();
    
    double initOx_integral = priorOx->Integral();
    double postOx_integral = postOx->Integral();
    
    double initOz_integral = priorOz->Integral();
    double postOz_integral = postOz->Integral();
    
    double initPg_integral = priorPg->Integral();
    double postPg_integral = postPg->Integral();
    
    priorB->Scale(1.0 / initB_integral);
    postB->Scale(1.0 / postB_integral);
    
    priorR->Scale(1.0 / initR_integral);
    postR->Scale(1.0 / postR_integral);
    
    priorT->Scale(1.0 / initT_integral);
    postT->Scale(1.0 / postT_integral);
    
    priorOx->Scale(1.0 / initOx_integral);
    postOx->Scale(1.0 / postOx_integral);
    
    priorOz->Scale(1.0 / initOz_integral);
    postOz->Scale(1.0 / postOz_integral);
    
    priorPg->Scale(1.0 / initPg_integral);
    postPg->Scale(1.0 / postPg_integral);
       
  }
  
  
  //Lines:
  
   // B:
  Float_t yvals[10];
  Float_t Bxvals[10];
  Float_t Rxvals[10];
  Float_t Txvals[10];
  Float_t Oxxvals[10];
  Float_t Ozxvals[10];
  Float_t Pgvals[10];
  
  for (int i = 0; i < 10; i++){
    yvals[i] = i*0.05;
    Bxvals[i] = lines[0];
    Rxvals[i] = lines[1];
    Txvals[i] = lines[2];
    Oxxvals[i] = lines[3];
    Ozxvals[i] = lines[4];
    Pgvals[i] = lines[5];
  }
  
  TGraph *Bgr = new TGraph(10, Bxvals, yvals);
  Bgr->SetLineColor(kRed);
  Bgr->SetLineWidth(1);
  Bgr->SetMarkerSize(0);

  TGraph *Rgr = new TGraph(10, Rxvals, yvals);
  Rgr->SetLineColor(kRed);
  Rgr->SetLineWidth(1);
  Rgr->SetMarkerSize(0);
  
  TGraph *Tgr = new TGraph(10, Txvals, yvals);
  Tgr->SetLineColor(kRed);
  Tgr->SetLineWidth(1);
  Tgr->SetMarkerSize(0);
  
  TGraph *Oxgr = new TGraph(10, Oxxvals, yvals);
  Oxgr->SetLineColor(kRed);
  Oxgr->SetLineWidth(1);
  Oxgr->SetMarkerSize(0);
  
  TGraph *Ozgr = new TGraph(10, Ozxvals, yvals);
  Ozgr->SetLineColor(kRed);
  Ozgr->SetLineWidth(1);
  Ozgr->SetMarkerSize(0);

  TGraph *Pgr = new TGraph(10, Pgvals, yvals);
  Pgr->SetLineColor(kRed);
  Pgr->SetLineWidth(1);
  Pgr->SetMarkerSize(0);

  
//   TCanvas *B = new TCanvas("B","B");
//   B->SetFillStyle(0);
//   B->SetFillColor(0);
//   B->SetFrameFillStyle(0);
//   B->cd(1);
//   priorB->GetXaxis()->SetTitle("B");
//   //newP->SetLineColor(kBlue);
//   priorB->SetFillColor(kGreen);
//   priorB->SetStats(0);
//   //newP->Draw();
//   postB->SetFillColor(kBlue);
//   postB->SetStats(0);
//   // priorB->Draw("");
//   postB->Draw("");
//   priorB->Draw("SAME");
//   
//   
//   B->SaveAs(saveFile);

    TCanvas *observables = new TCanvas("Observables","Observables",1000,700);
    observables->SetFillStyle(0);
    observables->SetFillColor(0);
    observables->SetFrameFillStyle(0);
    observables->Divide(3,2);
  
    // B:
    observables->cd(1);
    priorB->GetXaxis()->SetTitle("B");
    priorB->SetFillColor(kGreen);
    postB->SetFillColor(kBlue);
    postB->Draw("");
    Bgr->Draw("SAME");
    priorB->Draw("SAME");
    
    // R:
    observables->cd(2);
    priorR->GetXaxis()->SetTitle("R");
    priorR->SetFillColor(kGreen);
    postR->SetFillColor(kBlue);
    postR->Draw("");
    Rgr->Draw("SAME");
    priorR->Draw("SAME");
    
    // T:
    observables->cd(3);
    priorT->GetXaxis()->SetTitle("T");
    priorT->SetFillColor(kGreen);
    postT->SetFillColor(kBlue);
    postT->Draw("");
    Tgr->Draw("SAME");
    priorT->Draw("SAME");

    // Ox:
    observables->cd(4);
    priorOx->GetXaxis()->SetTitle("Ox");
    priorOx->SetFillColor(kGreen);
    postOx->SetFillColor(kBlue);
    postOx->Draw("");
    Oxgr->Draw("SAME");
    priorOx->Draw("SAME");
    
    // Oz:
    observables->cd(5);
    priorOz->GetXaxis()->SetTitle("Oz");
    priorOz->SetFillColor(kGreen);
    postOz->SetFillColor(kBlue);
    postOz->Draw("");
    Ozgr->Draw("SAME");
    priorOz->Draw("SAME");
    
    // Pgamma:
    observables->cd(6);
    priorPg->GetXaxis()->SetTitle("Pg");
    priorPg->SetFillColor(kGreen);
    postPg->SetFillColor(kBlue);
    postPg->Draw("");

    priorPg->Draw("SAME");
    Pgr->Draw("SAME");
    
    observables->SaveAs(saveFile);
  
}
//______________________________________________________________________________
//
void TPlotter::PlotPriorPost(bool norm, float redLine, char* saveFile)
{
  // Same as previous method, but will plot a vertical line on the float given
  // Useful for comparing results of simulation to input value(s).
  
  TH1F *priorB   = new TH1F("priorB"  , "B" , 100, -1.1, 1.1);
  TH1F *postB    = new TH1F("postB"   , "B" , 100, -1.1, 1.1);
    // Fill prior histogram
  for (int i = 0; i < MAX_PRIOR; ++i){
    priorB->Fill(Bprior[i]);
  }
  
  // Fill posterior histogram
  for (Long64_t i = 0; i<nentries; i++){
    T->GetEntry(i); 
    
    postB->Fill(B_obs, TMath::Exp(logL + logWt + TMath::Abs(maxLogWt)));
  }
  
  if (norm){
    double initB_integral = priorB->Integral();
    double postB_integral = postB->Integral();
    
    priorB->Scale(1.0 / initB_integral);
    postB->Scale(1.0 / postB_integral);
        
  }
  
  // For vertical line:
  Float_t yvals[10];
  Float_t xvals[10];
  for (int i = 0; i < 10; i++){
    yvals[i] = i*0.1;
    xvals[i] = redLine;
  }
  
  TGraph *gr = new TGraph(10, xvals, yvals);
  gr->SetLineColor(kRed);
  gr->SetLineWidth(3);
  gr->SetMarkerSize(0);
  
  
  
  TCanvas *B = new TCanvas("B","B");
  B->SetFillStyle(0);
  B->SetFillColor(0);
  B->SetFrameFillStyle(0);
  B->cd(1);
  priorB->GetXaxis()->SetTitle("B");
  //newP->SetLineColor(kBlue);
  priorB->SetFillColor(kGreen);
  priorB->SetStats(0);
  //newP->Draw();
  postB->SetFillColor(kBlue);
  postB->SetStats(0);
  // priorB->Draw("");
  postB->Draw("");
  priorB->Draw("SAME");
  gr->Draw("SAME");
  
  
  B->SaveAs(saveFile);
 
  
}
//______________________________________________________________________________
//
void TPlotter::PlotPB(bool norm, char* saveFile)
{
  // Will plot Pgamma*B.
  TH1F *priorPB  = new TH1F("priorPB" , "B*Pgamma",100,-1.1, 1.1);
  TH1F *postPB = new TH1F("postPB", "B*Pgamma",100, -1.1,1.1);
  
  // Fill prior histograms
  for (int i = 0; i < nPoints; ++i){
    priorPB->Fill(Bprior[i]*Pprior[i]);
  }
  
  // Fill posterior histograms
  for (Long64_t i = 0; i<nentries; i++){
    T->GetEntry(i); 
    postPB->Fill((B_obs*P_gam), TMath::Exp(logL + logWt + TMath::Abs(maxLogWt)) );
  }
  
  if (norm) {
    double initPB_integral = priorPB->Integral();
    double postPB_integral = postPB->Integral();
    
    priorPB->Scale(1.0 / initPB_integral);
    postPB->Scale(1.0 / postPB_integral);
  }
  
    TCanvas *PB = new TCanvas("PB","PB");
  PB->SetFillStyle(0);
  PB->SetFillColor(0);
  PB->SetFrameFillStyle(0);
  PB->cd(1);
  priorPB->GetXaxis()->SetTitle("P*B");
  //newP->SetLineColor(kBlue);
  priorPB->SetFillColor(kGreen);
  postPB->SetFillColor(kBlue);
  //newP->Draw();
  // priorPB->Draw("");
  postPB->Draw("");
  priorPB->Draw("SAME");
  

  PB->SaveAs(saveFile);
}
//______________________________________________________________________________
//
void TPlotter::BetaFit()
{
  // To be developed later.
}
//______________________________________________________________________________
//
void TPlotter::AllHists2D(float* lines)
{
  // Declare prior histograms
  priorBR = new TH2F("priorBR","",200,-1,1,200,-1,1);
  priorBT = new TH2F("priorBT","",200,-1,1,200,-1,1);  
  priorBOx = new TH2F("priorBOx","",200,-1,1,200,-1,1);  
  priorBOz = new TH2F("priorBOz","",200,-1,1,200,-1,1);
  
  priorRT = new TH2F("priorRT","",200,-1,1,200,-1,1);  
  priorROx = new TH2F("priorROx","",200,-1,1,200,-1,1);  
  priorROz = new TH2F("priorROz","",200,-1,1,200,-1,1);
  
  priorTOx = new TH2F("priorTOx","",200,-1,1,200,-1,1);  
  priorTOz = new TH2F("priorTOz","",200,-1,1,200,-1,1);
  
  priorOxOz = new TH2F("priorOxOz","",200,-1,1,200,-1,1);
  
  // Declare posterior histograms
  postBR = new TH2F("postBR","",200,-1,1,200,-1,1);
  postBT = new TH2F("postBT","",200,-1,1,200,-1,1);  
  postBOx = new TH2F("postBOx","",200,-1,1,200,-1,1);  
  postBOz = new TH2F("postBOz","",200,-1,1,200,-1,1);
  
  postRT = new TH2F("postRT","",200,-1,1,200,-1,1);  
  postROx = new TH2F("postROx","",200,-1,1,200,-1,1);  
  postROz = new TH2F("postROz","",200,-1,1,200,-1,1);
  
  postTOx = new TH2F("postTOx","",200,-1,1,200,-1,1);  
  postTOz = new TH2F("postTOz","",200,-1,1,200,-1,1);
  
  postOxOz = new TH2F("postOxOz","",200,-1,1,200,-1,1);
  
  // Declare stacks
  BR = new THStack("BR","BR");
  BT = new THStack("BT","BT");
  BOx = new THStack("BOx","BOx");
  BOz = new THStack("BOz","BOz");
  RT = new THStack("RT","RT");
  ROx = new THStack("ROx","ROx");
  ROz = new THStack("ROz","ROz");
  TOx = new THStack("TOx","TOx");
  TOz = new THStack("TOz","TOz");
  OxOz = new THStack("OxOz","OxOz");

  
  // Fill prior histograms
  for (int i = 0; i < nPoints; i++){
    
    priorBR->Fill(Bprior[i],Rprior[i]);
    priorBT->Fill(Bprior[i],Tprior[i]);
    priorBOx->Fill(Bprior[i],Oxprior[i]);
    priorBOz->Fill(Bprior[i],Ozprior[i]);
    priorRT->Fill(Rprior[i],Tprior[i]);
    priorROx->Fill(Rprior[i],Oxprior[i]);
    priorROz->Fill(Rprior[i],Ozprior[i]);
    priorTOx->Fill(Tprior[i],Oxprior[i]);
    priorTOz->Fill(Tprior[i],Ozprior[i]);
    priorOxOz->Fill(Oxprior[i],Ozprior[i]);
        
  }
  
  // Fill posterior histograms
  for (int i = 0; i < nentries; i++){
    T->GetEntry(i);
    postBR->Fill(B_obs,R_obs);
    postBT->Fill(B_obs,T_obs);
    postBOx->Fill(B_obs,Ox_obs);
    postBOz->Fill(B_obs,Oz_obs);
    postRT->Fill(R_obs,T_obs);
    postROx->Fill(R_obs,Ox_obs);
    postROz->Fill(R_obs,Oz_obs);
    postTOx->Fill(T_obs,Ox_obs);
    postTOz->Fill(T_obs,Oz_obs);
    postOxOz->Fill(Ox_obs,Oz_obs);
        
  }
  
  // Set fill colours
  priorBR->SetMarkerColor(kGreen);
  postBR->SetMarkerColor(kBlue);
  priorBR->SetMarkerStyle(kDot);
  postBR->SetMarkerStyle(kDot);
   
  priorBT->SetMarkerColor(kGreen);
  postBT->SetMarkerColor(kBlue);
  priorBT->SetMarkerStyle(kDot);
  postBT->SetMarkerStyle(kDot);
  
  priorBOx->SetMarkerColor(kGreen);
  postBOx->SetMarkerColor(kBlue);
  priorBOx->SetMarkerStyle(kDot);
  postBOx->SetMarkerStyle(kDot);
  
  priorBOz->SetMarkerColor(kGreen);
  postBOz->SetMarkerColor(kBlue);
  priorBOz->SetMarkerStyle(kDot);
  postBOz->SetMarkerStyle(kDot);
  
  priorRT->SetMarkerColor(kGreen);
  postRT->SetMarkerColor(kBlue);
  priorRT->SetMarkerStyle(kDot);
  postRT->SetMarkerStyle(kDot);
  
  priorROx->SetMarkerColor(kGreen);
  postROx->SetMarkerColor(kBlue);
  priorROx->SetMarkerStyle(kDot);
  postROx->SetMarkerStyle(kDot);
  
  priorROz->SetMarkerColor(kGreen);
  postROz->SetMarkerColor(kBlue);
  priorROz->SetMarkerStyle(kDot);
  postROz->SetMarkerStyle(kDot);
  
  priorTOx->SetMarkerColor(kGreen);
  postTOx->SetMarkerColor(kBlue);
  priorTOx->SetMarkerStyle(kDot);
  postTOx->SetMarkerStyle(kDot);
  
  priorTOz->SetMarkerColor(kGreen);
  postTOz->SetMarkerColor(kBlue);
  priorTOz->SetMarkerStyle(kDot);
  postTOz->SetMarkerStyle(kDot);
  
  priorOxOz->SetMarkerColor(kGreen);
  postOxOz->SetMarkerColor(kBlue);
  priorOxOz->SetMarkerStyle(kDot);
  postOxOz->SetMarkerStyle(kDot);
  
  // Add histograms to stacks
    BR->Add(priorBR);
  BR->Add(postBR);

    
  BT->Add(priorBT);
  BT->Add(postBT);
  
  BOx->Add(priorBOx);
  BOx->Add(postBOx);
  
  BOz->Add(priorBOz);
  BOz->Add(postBOz);
  
  RT->Add(priorRT);
  RT->Add(postRT);
  
  ROx->Add(priorROx);
  ROx->Add(postROx);
  
  ROz->Add(priorROz);
  ROz->Add(postROz);
  
  TOx->Add(priorTOx);
  TOx->Add(postTOx);
  
  TOz->Add(priorTOz);
  TOz->Add(postTOz);
  
  OxOz->Add(priorOxOz);
  OxOz->Add(postOxOz);
  
  // Graphs
  Float_t yvals[10];
  Float_t Bxvals[10];
  Float_t Rxvals[10];
  Float_t Txvals[10];
  Float_t Oxxvals[10];
  Float_t Ozxvals[10];
  Float_t Pgvals[10];
  
  for (int i = 0; i < 10; i++){
    yvals[i] = i*0.05;
    Bxvals[i] = lines[0];
    Rxvals[i] = lines[1];
    Txvals[i] = lines[2];
    Oxxvals[i] = lines[3];
    Ozxvals[i] = lines[4];
    Pgvals[i] = lines[5];
  }
  
  TGraph *Bgr = new TGraph(10, Bxvals, yvals);
  Bgr->SetLineColor(kRed);
  Bgr->SetLineWidth(1);
  Bgr->SetMarkerSize(0);

  TGraph *Rgr = new TGraph(10, Rxvals, yvals);
  Rgr->SetLineColor(kRed);
  Rgr->SetLineWidth(1);
  Rgr->SetMarkerSize(0);
  
  TGraph *Tgr = new TGraph(10, Txvals, yvals);
  Tgr->SetLineColor(kRed);
  Tgr->SetLineWidth(1);
  Tgr->SetMarkerSize(0);
  
  TGraph *Oxgr = new TGraph(10, Oxxvals, yvals);
  Oxgr->SetLineColor(kRed);
  Oxgr->SetLineWidth(1);
  Oxgr->SetMarkerSize(0);
  
  TGraph *Ozgr = new TGraph(10, Ozxvals, yvals);
  Ozgr->SetLineColor(kRed);
  Ozgr->SetLineWidth(1);
  Ozgr->SetMarkerSize(0);

  TGraph *Pgr = new TGraph(10, Pgvals, yvals);
  Pgr->SetLineColor(kRed);
  Pgr->SetLineWidth(1);
  Pgr->SetMarkerSize(0);

  
  // Draw histograms (stacks) on one canvas
  TCanvas *hists2D = new TCanvas("All_Observables","All_Observables");
      //TCanvas *observables = new TCanvas("Observables","Observables",1000,700);
  hists2D->SetFillStyle(0);
  hists2D->SetFillColor(0);
  hists2D->SetFrameFillStyle(0);
  hists2D->Divide(5,5);
  
  hists2D->cd(1);
  //priorB->GetXaxis()->SetTitle("B");
  priorB->SetFillColor(kGreen);
  postB->SetFillColor(kBlue);
  postB->Draw("");
  Bgr->Draw("SAME");
  priorB->Draw("SAME");
  
  hists2D->cd(2);
  BR->Draw("nostack");
  //postBR->Draw("");
  
  hists2D->cd(3);
  BT->Draw("nostack");
  
  hists2D->cd(4);
  BOx->Draw("nostack");
  
  hists2D->cd(5);
  BOz->Draw("nostack");
  
  hists2D->cd(7);
  priorR->SetFillColor(kGreen);
  postR->SetFillColor(kBlue);
  postR->Draw("");
  Rgr->Draw("SAME");
  priorR->Draw("SAME");
  
  hists2D->cd(8);
  RT->Draw("nostack");
  
  hists2D->cd(9);
  ROx->Draw("nostack");
  
  hists2D->cd(10);
  ROz->Draw("nostack");
  
  hists2D->cd(13);
  priorT->SetFillColor(kGreen);
  postT->SetFillColor(kBlue);
  postT->Draw("");
  Tgr->Draw("SAME");
  priorT->Draw("SAME");
  
  hists2D->cd(14);
  TOx->Draw("nostack");
  
  hists2D->cd(15);
  TOz->Draw("nostack");
  
  hists2D->cd(19);
  priorOx->SetFillColor(kGreen);
  postOx->SetFillColor(kBlue);
  postOx->Draw("");
  Oxgr->Draw("SAME");
  priorOx->Draw("SAME");
  
  hists2D->cd(20);
  OxOz->Draw("nostack");
  
  hists2D->cd(25);
  priorOz->SetFillColor(kGreen);
  postOz->SetFillColor(kBlue);
  postOz->Draw("");
  Ozgr->Draw("SAME");
  priorOz->Draw("SAME");
  
  
  hists2D->SaveAs("2d_test.root");
  hists2D->SaveAs("2d_test.png");
  
  
}
//______________________________________________________________________________

//      End-of-file
// ====================================================================

















