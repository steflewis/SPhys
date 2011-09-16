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
#ifndef TPLOTTER_H
#define TPLOTTER_H

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
#include "THStack.h"
#include <TFile.h>
#include <cstdlib>
#include <fstream>
#include <string>

#define  std_title_size 0.05
#define  std_title_offset 0.9
#define  MAX_PRIOR 3000

class TPlotter {
public:
  TPlotter();
  ~TPlotter() {};
  TPlotter(char* priorFile,char* postFile, int points);            // second constructor
  
  void PlotPriorPost(bool,char*, float*);         // Plots both prior and posterior on same canvas
  void PlotPriorPost(bool,float, char*);   // Like above, but also plots a red vertical line at a given value.
                                    // bools determine if the histograms should be normalised
  
  void PlotPB(bool, char*);
  
  void AllHists2D(float*);
  
  void BetaFit();            // To be developed later...
  
private:
/*  char* prior_filename;
  char* post_filename;*/
  int nPoints;
  // Prior data
  double *Bprior;
  double *Rprior;
  double *Tprior;
  double *Oxprior;
  double *Ozprior;
  double *Pprior;
  double *LogLprior;
  
  ifstream inPrior;
  
  // Posterior data
  TFile *f;
  TTree *T;
  
   Double_t     a1_Re;
   Double_t     a1_Im;
   Double_t     a2_Re;
   Double_t     a2_Im;
   Double_t     a3_Re;
   Double_t     a3_Im;
   Double_t     a4_Re;
   Double_t     a4_Im;
   Double_t     B_obs;
   Double_t     R_obs;
   Double_t     T_obs;
   Double_t     Ox_obs;
   Double_t     Oz_obs;
   Double_t     logL;
   Double_t     logWt;
   Double_t     P_gam ;
   
   Long64_t     nentries;
   Double_t     maxLogWt;
   
   // Histograms
   TH1F        *priorB;
   TH1F        *priorR;
   TH1F        *priorT;
   TH1F        *priorOx;
   TH1F        *priorOz;
   TH1F        *priorPg;
//   TH1F        *priorPB;
   TH1F        *postB;
   TH1F        *postR;
   TH1F        *postT;
   TH1F        *postOx;
   TH1F        *postOz;
   TH1F        *postPg;
//   TH1F        *postPB;
   TH2F        *B_Log;
   
  
   // 2-D histograms
   TH2F        *priorBR;
   TH2F        *priorBT;
   TH2F        *priorBOx;
   TH2F        *priorBOz;
   TH2F        *priorRT;
   TH2F        *priorROx;
   TH2F        *priorROz;
   TH2F        *priorTOx;
   TH2F        *priorTOz;
   TH2F        *priorOxOz;
   
   TH2F        *postBR;
   TH2F        *postBT;
   TH2F        *postBOx;
   TH2F        *postBOz;
   TH2F        *postRT;
   TH2F        *postROx;
   TH2F        *postROz;
   TH2F        *postTOx;
   TH2F        *postTOz;
   TH2F        *postOxOz;
   
   THStack     *BR;
   THStack     *BT;
   THStack     *BOx;
   THStack     *BOz;
   THStack     *RT;
   THStack     *ROx;
   THStack     *ROz;
   THStack     *TOx;
   THStack     *TOz;
   THStack     *OxOz;
   
};
#endif

//      End-of-file
// ====================================================================

  
  
  
  
  
  
  
  
  