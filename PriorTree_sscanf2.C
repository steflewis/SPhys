/*****************************************************************************

A program to plot prior distributions.

Stefanie Lewis

03/05/2011 - s.lewis.glasgow@gmail.com

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

#define  std_title_size 0.05
#define  std_title_offset 0.9

Int_t    nevents_old = 0;
Int_t    nevents_new1 = 0;

Double_t *oldB, *newB1, *oldLogL;

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
  Int_t  entries_old, entries_new1, count_old, count_new1;
  Char_t buffer[11024];

  // Declare histograms

  TH1F *oldP  = new TH1F("oldP" , "Initial Prior" , 100, -1.1, 1.1);
  TH1F *newP  = new TH1F("newP" , "Evolved Prior" , 100, -1.1, 1.1);
  TH2F *B_Log = new TH2F("B_Log", "LogL against B", 100, -1.1, 1.1, 100, -10, 10);
  
 again:
 
  // Initial Prior

  entries_old = 0;
  count_old   = 0;

  if((priorfile = fopen("oldprior.txt", "r")) == NULL){
    printf("Error opening file\n");
    return 1;
  }

  while(fgets(buffer,sizeof(buffer)-1,priorfile) != NULL){
    
    buffer[sizeof(buffer)-1] = '\0';
    if(entries_old >= nevents_old)
      {
	entries_old++;
	continue;
      }
    if( (sscanf(buffer,"%lf", &oldB[count_old], &oldLogL[count_old]) != 2) ){
      printf("Found an error in the data file!\n"); 
      return 2;
    }
   

    oldP->Fill(oldB[count_old]);
    B_Log->Fill(oldLogL[count_old], oldB[count_old]);
    
  }
  fclose(priorfile);


  if(entries_old > nevents_old){
    nevents_old = entries_old;
    if(
       (oldB = (Double_t *) calloc(nevents_old, sizeof(Int_t)))
       || (oldLogL = (Double_t *) calloc(nevents_old, sizeof(Int_t))) == NULL){
      printf("Couldn't allocate the necessary memory 00!\n");
      return 3;
    }	
    goto again;
  }

  // Evolved Prior 1

  entries_new1 = 0; 
  count_new1 = 0;
  if((priorfile_new1 = fopen("newBvals-1.txt", "r")) == NULL){
    printf("Error opening file\n");
    return 1;
  }

  while(fgets(buffer,sizeof(buffer)-1,priorfile_new1) != NULL){
    
    buffer[sizeof(buffer)-1] = '\0';
    if(entries_new1 >= nevents_new1)
      {
	entries_new1++;
	continue;
      }
    if(
       (sscanf(buffer,"%lf", 
	       &newB1[count_new1]) != 1 )
       ){
      printf("Found an error in the data file!\n");        //returns error if sscanf !=9.
      return 2;
    }
   


    newP->Fill(newB1[count_new1]);

  
  }
  fclose(priorfile);


  if(entries_new1 > nevents_new1){
    nevents_new1 = entries_new1;
    if(
       (newB1 = (Double_t *) calloc(nevents_new1, sizeof(Int_t))) == NULL){
      printf("Couldn't allocate the necessary memory 06!\n");
      return 3;
    }	
    goto again;
  }

  // Plot histograms here on same canvas

  TCanvas *priors = new TCanvas("PRIORS","priors");
  priors->SetFillStyle(0);
  priors->SetFillColor(0);
  priors->SetFrameFillStyle(0);

  
  priors->cd(1);
  newP->GetXaxis()->SetTitle("B");
  newP->SetLineColor(kBlue);
  oldP->SetLineColor(kRed);
  //newP->Draw();

  oldP->Draw();

  

  priors->Print("evolving_prior.eps");
 
  TCanvas *likelihood = new TCanvas("LIKELIHOOD","likelihood");
  likelihood->SetFillStyle(0);
  likelihood->SetFillColor(0);
  likelihood->SetFrameFillStyle(0);

  
  likelihood->cd(1);
  B_Log->GetXaxis()->SetTitle("B");
  B_Log->GetYaxis()->SetTitle("LogL");
  //  newP->SetLineColor(kBlue);
  //  oldP->SetLineColor(kRed);
  //newP->Draw();

  B_Log->Draw();



  return 0;
}
    
