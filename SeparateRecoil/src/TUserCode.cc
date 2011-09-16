//
// ==================================================================== 
//  
//      Project: sconsNS4
//               ~~~~~~
//  
//      File: TUserCode.cc
//            ~~~~~~~~~~~~
//
//      Description:    This is the code altered by the front end user.
//                      User will be required to change type of object 
//                      array to match example type they wish to run.
//                      This new example type should be a daughter class
//                      of the NSObject abstract class.logWidth
//                      
//                      Prior is also set here, and all methods from 
//                      TNestedSampling will be called here.
//  
//      Author: Stefanie Lewis <s.lewis@physics.gla.ac.uk>
//
//      Date: 01/11/10
//
//      Copyright: 2010 (C) Stefanie Lewis
//
//      $Id$ 
//
// ==================================================================== 

// #include "Rtypes.h"
// #include "Riostream.h"
// #include "TROOT.h"
// #include "TRandom.h"
// #include "TMath.h"
// #include <TFile.h>
#include "TSimplePhysics_CPU.h"
#include "TRecoil.h"
#include "TPlotter.h"
#include "TRecoil_Plotter.h"
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
//#define NOPLOT
//#define PLOT_ONLY
//#define RECOIL
using namespace std;

/***********************************************************************
 *                    Variable Declarations                            *
 ***********************************************************************/


const int noSamples = 3000;  //Set number of objects.



//Set filenames to be used for posterior file.  Not to exceed
//50 characters, should include .root and should conform to all filename rules.

char posterior[] = "test_newLogLhood2.root";
char recoil_post[] = "recoil_posterior.root";

double logWidth = log(1.0 - exp(-1.0 / noSamples));

char datafile[] = "full_data.txt";

int testing = 0;   //Change to 1 when testing.
int verbose = 1;

bool Updated = false;  // Set to true if using previous posterior 
                       // to generate new prior.
                       
char plotBsave[] = "500000_iterates_all_TEST2.png";
char plotPBsave[] = "PB_prior_post_TPlotter_test.png";
char prior_file[] = "oldprior.txt";
char recoil_prior[] = "recoilprior.txt";
char plotRsave[] = "40000_iterates_R_only.png";

Float_t realValues [6];

// Enter values used in event generator for comparison:
float B_real = 0.5566;
float R_real = -0.3444;
float T_real = -0.5697;
float Ox_real = -0.6638;
float Oz_real = -0.0249;
float Pg_real = 0.8;


// Recoil stuff:
float mean;
float std_dev;


/************************************************************************
 *                        Main Function                                 *
 *                                                                      *
 ************************************************************************/ 

int main(void)
{
  
    if (verbose == 1)
    printf("Start of main function.  logWidth is %g\n",logWidth);
    
    
    // This is just to initialise the posterior files.  They get overwritten,
    // but ROOT complains if they are used but don't exist at compile-time.
    ofstream initial;
    initial.open(recoil_post);
    initial << "firstline\n";
    initial.close();
    initial.clear();
    
    initial.open(posterior);
    initial << "firstline\n";
    initial.close();
    
    
    

  
  // TRecoil:
  
  #ifndef PLOT_ONLY 
  
  TRecoil *recoil = new TRecoil(noSamples,logWidth,datafile);
  
  if (Updated == false){
    for (int i = 0; i < noSamples; i++){
      recoil->Prior(i);
    }
  }
  
  recoil->Iterate();
  recoil->FinalCorrection();
  recoil->PrintSummary(recoil_post);
  printf("Print summary complete\n");
  

 
  
  TRecoil_Plotter *Rplot = new TRecoil_Plotter(recoil_prior,recoil_post,noSamples);
    
  float Rval[1];
  Rval[0] = R_real;
  Rplot->PlotPriorPost(1,plotRsave,Rval);
  
  //printf("Mean: %lf\t Std Dev: %lf\n",Rplot->mean, Rplot->sigma);
  
  #endif
  
  mean = Rplot->mean;
  std_dev = Rplot->sigma;
  
  printf("Mean: %lf\t Std Dev: %lf\n",Rplot->mean, Rplot->sigma);
  
  
  
  
  // TSimplePhysics:
 #ifndef RECOIL 
  
  TSimplePhysics_CPU *LH = new TSimplePhysics_CPU(noSamples,logWidth, datafile);
  printf("Passed TSimplePhysics constructor\n");
//  LH->SetMeanStdDev(mean,std_dev);
  LH->SetMeanStdDev(-0.3444, 0.1);
  printf("Set mean and standard deviation \n");
  printf("Mean: %lf\t Std Dev: %lf\n",LH->Rmean,LH->Rstddev);
    if (Updated == false){
    for (int i = 0; i < noSamples; i++){
      LH->Prior(i);
    }
printf("Set prior\n");
printf("Accepted: %d\t Rejected: %d\n",LH->accepted,LH->rejected);
  }
  else if (Updated == true){
    LH->UpdatedPrior();
  }

  printf("About to run Iterate\n");
  LH->Iterate();                  //Run
  printf("Finished running Iterate\n");

  LH->FinalCorrection();          //Optional final correction
  printf("Final Correction complete\n");

  LH->PrintSummary(posterior);
  printf("Print Summary complete\n"); 

  
  
#endif
 
  
  
  
  
  // Plotting:
  
realValues[0] = B_real; //0.5566;   // Sigma (B) value
realValues[1] = R_real; //-0.3444;  // Recoil (P) value
realValues[2] = T_real; //-0.5697;  // T
realValues[3] = Ox_real; //-0.6638;  // Ox
realValues[4] = Oz_real; //-0.0249;  // Oz
realValues[5] = Pg_real; //0.8;      // Pgamma

  



     

  // Make plots
#ifndef RECOIL
  TPlotter *plot = new TPlotter(prior_file,posterior,noSamples);
  plot->PlotPriorPost(true,plotBsave,realValues);
#endif
  //plot->PlotPB(true,plotPBsave);

  return 0;

}
