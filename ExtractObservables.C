#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TArray.h"
#include "TF1.h"
#include "TF2.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TStyle.h"

#include <iostream>

void ExtractObservables()
{

  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);
  

  //	Getting generated histograms...
  TFile*  fi1    = new TFile("outpara.root");
  TH1F*   hpara  = (TH1F*)gDirectory->Get("hcosya")->Clone("hpara");
  TH2F*   hparaX = (TH2F*)gDirectory->Get("hxpa")->Clone("hparaX");
  TH2F*   hparaY = (TH2F*)gDirectory->Get("hypa")->Clone("hparaY");
  TH2F*   hparaZ = (TH2F*)gDirectory->Get("hzpa")->Clone("hparaZ");
  TFile*  fi2    = new TFile("outperp.root");
  TH1F*   hperp  = (TH1F*)gDirectory->Get("hcosye")->Clone("hperp");
  TH2F*   hperpX = (TH2F*)gDirectory->Get("hxpe")->Clone("hperpX");
  TH2F*   hperpY = (TH2F*)gDirectory->Get("hype")->Clone("hperpY");
  TH2F*   hperpZ = (TH2F*)gDirectory->Get("hzpe")->Clone("hperpZ");


  //	Create histograms to be fitted...
//   TH1F* htot = (TH1F*)hpara->Clone("htot");
//   htot->Add(hperp);
  hpara->Add(hperp);
  hpara->SetTitle("Fitting Recoil Polarization");
  hpara->GetXaxis()->SetTitle("cos #theta_{y}");
  TH2F*  asymmX  = (TH2F*)hperpX->GetAsymmetry(hparaX);
  asymmX->GetXaxis()->SetTitle("#phi (radians)");
  asymmX->GetYaxis()->SetTitle("cos #theta_{x}");
  TH2F*  asymmY  = (TH2F*)hperpY->GetAsymmetry(hparaY);
  asymmY->GetXaxis()->SetTitle("#phi (radians)");
  asymmY->GetYaxis()->SetTitle("cos #theta_{y}");
  TH2F*  asymmZ  = (TH2F*)hperpZ->GetAsymmetry(hparaZ);
  asymmZ->GetXaxis()->SetTitle("#phi (radians)");
  asymmZ->GetYaxis()->SetTitle("cos #theta_{z}");

  
  //	Display the histograms...
  TCanvas* c1 = new TCanvas("c1","",899,699);
  c1->Divide(2,2);
  c1->cd(1);
  hpara->DrawCopy();

  c1->cd(2);
  asymmX->SetTitle("Double Polarization Asymmetry X");
  asymmX->Draw("COLZ");

  c1->cd(3);
  asymmY->SetTitle("Double Polarization Asymmetry Y");
  asymmY->Draw("COLZ");

  c1->cd(4);
  asymmZ->SetTitle("Double Polarization Asymmetry Z");
  asymmZ->Draw("COLZ");


  //	Do the fitting
  Float_t PgammaT = 0.7;   
  Float_t aWeak = 0.642;
 
  //	1D fit for recoil...
  TF1* f = new TF1("f","[0]*(1 + [1]*x)", -1, 1 );
  c1->cd(1);
  hpara->Fit(f,"Q");
  Float_t chi2 = f->GetChisquare()/(f->GetNDF());
  Float_t Recoil  = f->GetParameter(1)/aWeak;
  Float_t dRecoil = f->GetParError(1)/aWeak;
  hpara->DrawCopy();

 
  cout <<endl<<endl;
  cout <<"**********************************"<<endl;
  cout <<"values from 1D cross-section fit:"<<endl;
  cout <<"Recoil  = " << Recoil << " +/- " << dRecoil << endl;
  cout <<"Chisq   = " << chi2 <<endl;
  cout <<endl;
  
  //	2D fit for Ox, Oz...
  TF2*  f2DXZ = new TF2("f2DXZ","[0]*cos(2*x) + [1]*sin(2*x)*y");
  c1->cd(2);
  asymmX->Fit("f2DXZ","QN");
  Float_t chi2dX    = f2DXZ->GetChisquare()/(f2DXZ->GetNDF());
  Float_t Sigma2dX  = f2DXZ->GetParameter(0)/PgammaT;
  Float_t dSigma2dX = f2DXZ->GetParError(0)/PgammaT;
  Float_t Ox2d      = f2DXZ->GetParameter(1)/(PgammaT*aWeak);
  Float_t dOx2d     = f2DXZ->GetParError(1)/(PgammaT*aWeak);
  cout <<"values from 2-dimensional X fit:"<<endl;
  cout << "Sigma = " << Sigma2dX << "  +/- " << dSigma2dX << endl;
  cout << "Ox    = " << Ox2d << " +/- " << dOx2d << endl;
  cout << "Chisq 2-d fit = " << chi2dX <<endl;

  c1->cd(4);
  asymmZ->Fit("f2DXZ","QN");
  Float_t chi2dZ    = f2DXZ->GetChisquare()/(f2DXZ->GetNDF());
  Float_t Sigma2dZ  = f2DXZ->GetParameter(0)/PgammaT;
  Float_t dSigma2dZ = f2DXZ->GetParError(0)/PgammaT;
  Float_t Oz2d      = f2DXZ->GetParameter(1)/(PgammaT*aWeak);
  Float_t dOz2d     = f2DXZ->GetParError(1)/(PgammaT*aWeak);
  cout <<endl<<"values from 2-dimensional Z fit:"<<endl;
  cout << "Sigma = " << Sigma2dZ << "  +/- " << dSigma2dZ << endl;
  cout << "Oz    = " << Oz2d << " +/- " << dOz2d << endl;
  cout << "Chisq 2-d fit = " << chi2dZ <<endl;

  //	2D fit for Ox, Oz...
  TF2*  f2DY = new TF2("f2DY","([0]*cos(2*x)+[1]*y*cos(2*x))/(1+[2]*y)");
  //f2DY->FixParameter(0,0.0);
  //f2DY->FixParameter(1,0.0);
  f2DY->FixParameter(2,Recoil*aWeak);

  c1->cd(3);
  asymmY->Fit("f2DY","QN");
  Float_t chi2dY    = f2DY->GetChisquare()/(f2DY->GetNDF());
  Float_t Sigma2dY  = f2DY->GetParameter(0)/PgammaT;
  Float_t dSigma2dY = f2DY->GetParError(0)/PgammaT;
  Float_t T2d       = f2DY->GetParameter(1)/(PgammaT*aWeak);
  Float_t dT2d      = f2DY->GetParError(1)/(PgammaT*aWeak);
  Float_t Recoil2dY = f2DY->GetParameter(2)/aWeak;
  Float_t dRecoil2dY= f2DY->GetParError(2)/aWeak;
  cout <<endl<<"values from 2-dimensional Y fit:"<<endl;
  cout << "Sigma = " << Sigma2dY << "  +/- " << dSigma2dY << endl;
  cout << "T     = " << T2d << " +/- " << dT2d << endl;
  cout << "R     = " <<  Recoil2dY << " +/- " << dRecoil2dY << endl;
  cout << "Chisq 2-d fit = " << chi2dY <<endl;

  cout <<"*********************************"<<endl;

  
  fi1->Close();
  fi2->Close();
  delete fi1;
  delete fi2;
}
