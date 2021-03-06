/*****************************************************************************

A program to plot prior distributions and posterior distributions together.

Stefanie Lewis

29/06/2011 - s.lewis.glasgow@gmail.com

Purpose of this program:
-To read in a text file containing the values of B ("Sigma"),          DONE
 P_gamma and LogL for the initial samples                             

-To read in a ROOT file containing the posterior values                DONE

-To generate histograms as follows:

      -On one canvas, display both the prior pdf of B as well as       DONE
       the posterior pdf of B, with different fill colours.

      -On another canvas, display both the prior pdf of P*B as         DONE
       well as the posterior pdf of P*B, again with different
       fill colours.

-This will include normalising the histograms.                         DONE

-Add a beta distribution fit to the posterior pdf.  
  This should probably go in a separate canvas, 
  with the posterior histogram and the beta fit.

****************************************************************************/

// Header files here

#include "Riostream.h"
#include "TROOT.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMinuit.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TMath.h"
#include "TSystem.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TF1.h"
#include "TF2.h"
#include <TFile.h>
#include <cstdlib>
#include <fstream>
#include <string>

#define  std_title_size 0.05
#define  std_title_offset 0.9

using namespace TMath;

int main(){


  // gStyle stuff

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
  

  FILE  *priorfile, *priorfile_new1;

  // Set variables here:
  // Number of samples, filename of prior, filename of posterior.

  const int MAX_SAMPLES = 3000;
  char * prior_filename = "/home/Stefanie/NestedSampling/CPU_GPU/oldprior.txt";
  const char* filename = "/home/Stefanie/NestedSampling/CPU_GPU/SPhys_vCPU_test1.root";



  // Declare histograms

  TH1F *priorB   = new TH1F("priorB"  , "B" , 100, -1.1, 1.1);
  TH1F *priorPB  = new TH1F("priorPB" , "B*Pgamma",100,-1.1, 1.1);
  TH1F *postB  = new TH1F("postB" , "B" , 100, -1.1, 1.1);
  TH1F *postPB = new TH1F("postPB", "B*Pgamma",100, -1.1,1.1);
  TH2F *B_Log = new TH2F("B_Log", "LogL against B", 100, -1.1, 1.1, 100, -10, 10);
  
  

  // Read in data from prior file:
  double Bprior[MAX_SAMPLES];
  double Pprior[MAX_SAMPLES];
  double LogLprior[MAX_SAMPLES];

  ifstream in(prior_filename);

  for (int i = 0; i < MAX_SAMPLES; ++i){
    in >> Bprior[i];
    in >> LogLprior[i];
    in >> Pprior[i];
  }

  //std::cout<<"Test: Index 5.  B: "<<Bprior[5]<<" P: "<<Pprior[5]<<std::endl;


  // Fill prior histograms
  for (int i = 0; i < MAX_SAMPLES; ++i){
    priorB->Fill(Bprior[i]);
    priorPB->Fill(Bprior[i]*Pprior[i]);
  }


  // Read in data from posterior file

   TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
   if (!f) {
      f = new TFile(filename);
   }

   // May need to change 'T' here to relevant tree name.
   TTree *T = (TTree*)gDirectory->Get("tree");

   //Declaration of leaves types

   Double_t     a1_Re;
   Double_t     a1_Im;
   Double_t     a2_Re;
   Double_t     a2_Im;
   Double_t     a3_Re;
   Double_t     a3_Im;
   Double_t     a4_Re;
   Double_t     a4_Im;
   Double_t     B_obs;
   Double_t     logL;
   Double_t     logWt;
   Double_t     P_gam ;

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
   T->SetBranchAddress("logL",   &logL );
   T->SetBranchAddress("logWt",  &logWt);
   T->SetBranchAddress("P_gam",  &P_gam);

   Long64_t         nentries = T->GetEntries();
 
   Double_t         maxLogWt = -100000;
 
 
   // Calculate max Log(weight)
   for (Long64_t i=0; i<nentries;i++) 
     {
       T->GetEntry(i);
       if ((logL +logWt) > maxLogWt)
	   //       cout << LogWt[j] << endl;
	 maxLogWt = (logL + logWt);
     }
 
      

   cout << "Max LogWt: " << maxLogWt << endl;
	 
   // Just to see if I can get results printed to a text file to fit using another program.
   ofstream points;
   points.open("testPoints.txt");
   
   
   // Fill posterior histograms
   for (Long64_t i = 0; i<nentries; i++){
       T->GetEntry(i); 

       postB->Fill(B_obs, TMath::Exp(logL + logWt + TMath::Abs(maxLogWt))        );

       postPB->Fill((B_obs*P_gam), TMath::Exp(logL + logWt + TMath::Abs(maxLogWt)) );
       // From here
       points << B_obs;
       points << " ";
       points << (TMath::Exp(logL + logWt + TMath::Abs(maxLogWt)));
       points << "\n";
       // To here
   }
   // And here.
points.close();

   // Output:
   std::cout<<"Prior file used: "<<prior_filename<<std::endl;
   std::cout<<"Posterior file used: "<<filename<<std::endl;


   // Normalise histograms
   double initB_integral = priorB->Integral();
   double initPB_integral = priorPB->Integral();
   double postB_integral = postB->Integral();
   double postPB_integral = postPB->Integral();
   
   priorB->Scale(1.0 / initB_integral);
   postB->Scale(1.0 / postB_integral);
   priorPB->Scale(1.0 / initPB_integral);
   postPB->Scale(1.0 / postPB_integral);


  // Plot histograms here on same canvas

  TCanvas *B = new TCanvas("B","B");
  B->SetFillStyle(0);
  B->SetFillColor(0);
  B->SetFrameFillStyle(0);
  B->cd(1);
  priorB->GetXaxis()->SetTitle("B");
  //newP->SetLineColor(kBlue);
  priorB->SetFillColor(kGreen);
  //newP->Draw();
  postB->SetFillColor(kBlue);
  // priorB->Draw("");
  postB->Draw("");
  priorB->Draw("SAME");
  

  B->SaveAs("prior_post_B.png");


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
  

  PB->SaveAs("prior_post_PB.png");
  
  
  
  ////********************* BETA DISTR FITTING ********************////
  
  // Declare variables for fit
  float upper = 1.0;  // upper bound of beta distribution
  const float lower = -1.0; // lower bound of beta distribution
  float       alphaOne;     // These two are the shape parameters for 
  float       alphaTwo;     // the beta distribution
  float       alphaOne_err;
  float       alphaTwo_err;
  float       chi2;         // Chi number for goodness of fit
  
  
  // Declare function(s) to be used in fit expression
 
  // Declare fit
  //TF1 *bta = new TF1("bta", "(TMath::Gamma([0]+[1])) / (TMath::Gamma([0]) * TMath::Gamma([1]))", -1,1);
 // TF1 *bta = new TF1("bta", "(1/TMath::Beta([0],[1]))*(x-(-1))^([0]-1)*(1 - x)^([1]-1) / ((1 - (-1))^([0]+[1]-1))", -1, upper);
//   TF1 *bta = new TF1("bta", "[0] + [1]*sin(x)", 0, upper);
// TF1 *bta = new TF1("bta","TMath::BetaDistI(x, [0], [1])", 0, 1);
  TF1 *fit = new TF1("fit", "((TMath::Gamma([0] + [1]))/(TMath::Gamma([0])*TMath::Gamma([1])))*((x-(-1))^([0]-1)*(1 - x)^([1]-1) / ((1 - (-1))^([0]+[1]-1)))");
  // Perform the fit:
 // bta->SetParameters(0,0);   
  fit->SetParameter(0,80);
  fit->SetParameter(1,2);
  postB->Fit("fit","R");
 // postB->Fit("gaus");
  alphaOne = fit->GetParameter(0);
  alphaOne_err = fit->GetParError(0);
  alphaTwo = fit->GetParameter(1);
  alphaTwo_err = fit->GetParError(1);
  chi2 = fit->GetChisquare();
  
  
  // Plot fit on graph:
  TCanvas *graphFit = new TCanvas("GRAPH", "Fit", 200, 10, 1000, 700);
  graphFit->SetFillStyle(0);
  graphFit->SetFillColor(0);
  graphFit->SetFrameFillStyle(0);
  
 // TGraph *betafit = new TGraph(200, )
  graphFit->cd(1);
  postB->GetXaxis()->SetTitle("B");
  postB->Draw();
  
  fit->SetLineColor(kRed);
  fit->Draw("same");
  graphFit->SaveAs("betafit_Test.png");

  return 0;

}
 
  
  double InvBeta(float a, float b){
    
    // Returns 1/B, where B is the Beta function defined as follows:
    // B = Gamma(a)*Gamma(b) / Gamma(a+b)
    
   double bottom = TMath::Gamma(double(a)) * TMath::Gamma(double(b));
   double top = TMath::Gamma(double(a)+ double(b));
   double invB = top / bottom; 
   
   return invB;
   
  }
 
