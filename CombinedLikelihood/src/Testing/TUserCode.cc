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
//#include "TPlotter.h"
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
//#define PLOT_ONLY
#define NOPLOT
using namespace std;

/***********************************************************************
 *                    Variable Declarations                            *
 ***********************************************************************/


const int noSamples = 3000;  //Set number of objects.



//Set filenames to be used for posterior file.  Not to exceed
//50 characters, should include .root and should conform to all filename rules.

char posterior[] = "test_newLogLhood3_sum_100000.root";

double logWidth = log(1.0 - exp(-1.0 / noSamples));

char datafile[] = "full_data.txt";

int testing = 0;   //Change to 1 when testing.
int verbose = 1;

bool Updated = false;  // Set to true if using previous posterior 
                       // to generate new prior.
                       
char plotBsave[] = "100000_iterates_all_TEST_sum.png";
char plotPBsave[] = "PB_prior_post_TPlotter_test.png";
char prior_file[] = "oldprior.txt";

Float_t realValues [6];

float B_real = 0.5566;
float R_real = -0.3444;
float T_real = -0.5697;
float Ox_real = -0.6638;
float Oz_real = -0.0249;
float Pg_real = 0.8;
// Enter values used in event generator for comparison:




/************************************************************************
 *                        Main Function                                 *
 *          Do not change anything below this comment box.              *
 ************************************************************************/ 

int main(void)
{
realValues[0] = B_real; //0.5566;   // Sigma (B) value
realValues[1] = R_real; //-0.3444;  // Recoil (P) value
realValues[2] = T_real; //-0.5697;  // T
realValues[3] = Ox_real; //-0.6638;  // Ox
realValues[4] = Oz_real; //-0.0249;  // Oz
realValues[5] = Pg_real; //0.8;      // Pgamma
#ifndef PLOT_ONLY 
  TSimplePhysics_CPU *LH = new TSimplePhysics_CPU(noSamples,logWidth, datafile);
#endif
  
#ifndef NOPLOT
  //TPlotter *plot = new TPlotter(prior_file,posterior,noSamples);
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
  

float logl[10];
float B[10];
float R[10];
float T[10];
float Ox[10];
float Oz[10];
float Pg[10];

for (int i = 0; i < 10; i++){
  Pg[i] = 0.8;
 if (i < 4){
   B[i] = B_real - 0.05*(i+1);
   R[i] = R_real - 0.05*(i+1);
   T[i] = T_real - 0.05*(1+i);
   Ox[i] = Ox_real - 0.05*(1+i);
   Oz[i] = Oz_real - 0.05*(1+i);
   //Pg[i] = Pg_real - 0.005*(i+1);
   
   logl[i] = LH->LogLhood(B[i],R[i],T[i],Ox[i],Oz[i],Pg[i]);
 }
 else if (i == 4){
  B[i] = B_real;
  R[i] = R_real;
  T[i] = T_real;
  Ox[i] = Ox_real;
  Oz[i] = Oz_real;
  //Pg[i] = Pg_real;
  
    logl[i] = LH->LogLhood(B[i],R[i],T[i],Ox[i],Oz[i],Pg[i]);
    
 }
 else{
  B[i] = B_real + 0.05*(i-4);
    R[i] = R_real - 0.05*(i+1);
   T[i] = T_real - 0.05*(1+i);
   Ox[i] = Ox_real - 0.05*(1+i);
   Oz[i] = Oz_real - 0.05*(1+i);
  //Pg[i] = Pg_real + 0.005*(i-4);
  
     logl[i] = LH->LogLhood(B[i],R[i],T[i],Ox[i],Oz[i],Pg[i]);
 }
  //printf("B: %lf\n",B[i]);
}

for (int i = 0; i < 10; i++){
 printf("i: %d\t logl: %lf\n",i,logl[i]); 
  
}

int max = 0;
float testlog = logl[0];
for (int i = 0; i < 10; i++){
  if (logl[i] > testlog){
   testlog = logl[i];
   max = i;
  }
}
printf("Index of max: %d\t Max logl: %lf\t B value: %lf\n",max,logl[max],B[max]);
  return 0;

}
