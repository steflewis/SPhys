/***************************************************************************
 *                                                                         *
 *                      Simplified Physics Problem                         *
 *              ROOT Macro to create ROOT tree of posterior results        *
 *                                                                         *
 *              Stefanie Lewis, 31/01/2011                                 *
 *                                                                         *
 ***************************************************************************/

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
#include "TSystem.h"
#include "TPad.h"
#include "TVirtualPad.h"
#include "TAxis.h"
#include "TH1.h"



TFile *NewPriorTree(){
 
  Double_t     B;
  Double_t     LogL;

  

  Char_t buffer[11024];

  TString filename = "newprior.root";
  FILE    *input   = fopen("newBvals.txt","r");
  
  TFile *hfile = new TFile("newprior.root","recreate");
  TTree *tree = new TTree("New","New Prior");
  tree->Branch("B",      &B,      "B/D"    );
  tree->Branch("LogL",   &LogL,   "LogL/D" );

  // while (fgets(&buffer, sizeof(buffer)-1, input)) {
  //   sscanf(buffer, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &a1_Re, &a1_Im,&a2_Re, &a2_Im,&a3_Re, &a3_Im,&a4_Re, &a4_Im, &B, &LogL, &LogWt);
  //   tree->Fill();
  // }

  tree->ReadFile("newBvals.txt","B/D:LogL");

  
  tree->Write();
  //hfile->Write("tree");

  fclose(input);
  




  return 0;
}
