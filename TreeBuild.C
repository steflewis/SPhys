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



TFile *TreeBuild(){
 
  Double_t     a1_Re;
  Double_t     a1_Im;
  Double_t     a2_Re;
  Double_t     a2_Im;
  Double_t     a3_Re;
  Double_t     a3_Im;
  Double_t     a4_Re;
  Double_t     a4_Im;
  Double_t     B;
  Double_t     LogL;
  Double_t     LogWt;

  

  Char_t buffer[11024];

  TString filename = "simplephysics.root";
  FILE    *input   = fopen("test.txt","r");
  
  TFile *hfile = new TFile("beamrecoil.root","recreate");
  TTree *tree = new TTree("T","Simple Beam Recoil Posterior");
  tree->Branch("a1_Re",  &a1_Re,  "a1_Re/D");
  tree->Branch("a1_Im",  &a1_Im,  "a1_Im/D");
  tree->Branch("a2_Re",  &a2_Re,  "a2_Re/D");
  tree->Branch("a2_Im",  &a2_Im,  "a2_Im/D");
  tree->Branch("a3_Re",  &a3_Re,  "a3_Re/D");
  tree->Branch("a3_Im",  &a3_Im,  "a3_Im/D");
  tree->Branch("a4_Re",  &a4_Re,  "a4_Re/D");
  tree->Branch("a4_Im",  &a4_Im,  "a4_Im/D");
  tree->Branch("B",      &B,      "B/D"    );
  tree->Branch("LogL",   &LogL,   "LogL/D" );
  tree->Branch("LogWt",  &LogWt,  "LogWt/D");

  // while (fgets(&buffer, sizeof(buffer)-1, input)) {
  //   sscanf(buffer, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &a1_Re, &a1_Im,&a2_Re, &a2_Im,&a3_Re, &a3_Im,&a4_Re, &a4_Im, &B, &LogL, &LogWt);
  //   tree->Fill();
  // }

  tree.ReadFile("test.txt","a1_Re/D:a1_Im:a2_Re:a2_Im:a3_Re:a3_Im:a4_Re:a4_Im:B:LogL:LogWt");

  
  tree->Write();
  //hfile->Write("tree");

  fclose(input);
  
  return 0;
}
