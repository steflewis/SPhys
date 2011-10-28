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

//#define GPU

#include "OclWrapper.h"
#include "TSimplePhysics_GPU.h"
#include "TSimplePhysics_CPU.h"
#ifdef PLOT
#include "TPlotter.h"
#endif
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
using namespace std;

/***********************************************************************
 *                    Variable Declarations                            *
 ***********************************************************************/


const int noSamples = 3000;  //Set number of objects.



//Set filenames to be used for posterior file.  Not to exceed
//50 characters, should include .root and should conform to all filename rules.

char posterior[] = "SPhys_vCPU_test1.root";

double logWidth = log(1.0 - exp(-1.0 / noSamples));

//char datafile[] = "/home/Stefanie/Dropbox/NestedSampling/EventGen/datasets/1000/data_set1.txt";//"/home/Stefanie/Dropbox/NestedSampling/CPU_GPU/text_files/datatest.txt";
char datafile[] = DATAPATH ;

int testing = 0;   //Change to 1 when testing.
int verbose = 1;

bool Updated = false;  // Set to true if using previous posterior 
                       // to generate new prior.
#ifdef PLOT                       
char plotBsave[] = "B_prior_post_TPlotter_test.png";
char plotPBsave[] = "PB_prior_post_TPlotter_test.png";
char prior_file[] = "oldprior.txt";
#endif

/************************************************************************
 *                        Main Function                                 *
 *          Do not change anything below this comment box.              *
 ************************************************************************/ 

int main(void)
{

#ifdef GPU
  TSimplePhysics_GPU *LH = new TSimplePhysics_GPU(noSamples,logWidth, datafile);
#ifdef PLOT
  TPlotter *plot = new TPlotter(prior_file,posterior,noSamples);
#endif
#endif
  
#ifndef GPU
   TSimplePhysics_CPU *LH = new TSimplePhysics_CPU(noSamples,logWidth, datafile);
#ifdef PLOT
  TPlotter *plot = new TPlotter(prior_file,posterior,noSamples);
#endif
#endif

  if (verbose == 1)
    printf("Start of main function.  logWidth is %g\n",logWidth);

  // if (testing == 1){
  //   ifstream fin;
  //   fin.open("/home/stefl/examples/sivia/siviavalues.txt");
  //   double NewX[noSamples];
  //   double NewY[noSamples];
  //   double NewLogL[noSamples];


  //   int y = 0;
  //   while(!fin.eof()){
  //     fin >> NewX[y] >> NewY[y] >> NewLogL[y];
  //     y++;
  //   }
  // }
  

  //Set Prior

  if (Updated == false){
    for (int i = 0; i < noSamples; i++){
//WV cout << i<< endl; 
      LH->Prior(i);
    }

  }
  else if (Updated == true){
    LH->UpdatedPrior();
  }
printf("Finished Prior\n");
   printf("About to run Iterate\n");
  LH->Iterate();                  //Run
   printf("Finished running Iterate\n");

  LH->FinalCorrection();          //Optional final correction
  //printf("Final Correction complete\n");

  LH->PrintSummary(posterior);
  printf("Print Summary complete\n"); 
  
#ifdef PLOT
  // Make plots
  plot->PlotPriorPost(true,plotBsave);
  
  plot->PlotPB(true,plotPBsave);
#endif
  return 0;

}
