//
// ==================================================================== 
//  
//      Project: sconsNS2
//               ~~~~~~
//  
//      File: TSimplePhysics_CPU.cc
//            ~~~~~~~~~~~~~~~~~~~
//
//      Description:     See TSimplePhysics.h
//  
//      Author: Stefanie Lewis <s.lewis@physics.gla.ac.uk>
//      Update: 2011-05-19 15:36:46z
//
//      Copyright: 2011 Stefanie Lewis
//
//      $Id$ 
//
//      Update: 2011-07-11 13:48:31z 
//              The purpose of this update is to create a likelihood
//              function that includes all linear beam recoil 
//              variables. 
//
//
// ==================================================================== 
#ifdef WITH_OMP
#include <omp.h>
#endif
//#define NTH 8
#include "TSimplePhysics_CPU.h"


using namespace std;
//____________________________________________________________________

//____________________________________________________________________
TSimplePhysics_CPU::TSimplePhysics_CPU()
{
  
  // Default constructor

}
//_____________________________________________________________________
TSimplePhysics_CPU::TSimplePhysics_CPU(int numberOfObjects, double logWidth, char* name):TSimplePhysics(numberOfObjects, logWidth, name)
{
  
  
  
}
//____________________________________________________________________
float
TSimplePhysics_CPU::LogLhood (float B_loc, float Pg_loc)
{

  //===================================================//
  // Method development notes -                        //
  //                                                   //
  // Weak decay constant set to non-diluted value.     //
  // Need to somewhere declare/initialise these:       //
  // -- T                                              //
  // -- R    (guessing prior! and passed as arg)       //
  // -- csy  (think this is from input file)           //
  //                                                   //
  //===================================================//
  
  // logLikelihood function
  // P_gamma         = 0.8;
  float delta_L         = 0; // WV: made this a local rather than class attribute
  double LogL     = 0.0;
//  double costerm  = 0.0;
//  double prob     = 0.0;   // Probability
//  double A_tilde;
//  double localpol = 0.0;
  double *LogL_2_array = new double[NTH];
for (int i = 0; i < NTH; i++){
LogL_2_array[i] = 0.0;
}
//int th_id;
  // For use of log base 2:
  double LogL_2   = 0.0;
  //double A_temp;
#ifdef WITH_OMP
#pragma omp parallel private(delta_L) shared(LogL_2_array) num_threads(NTH) 
// join next line with previous and #define NTH to desired number of threads if required
// num_threads(NTH)
{
  int th_id = omp_get_thread_num();
double LogL_2_th=0.0;
#pragma omp for
#endif
  for (int i = 0; i < nEvents; i++){
    
    double costerm = Pg_loc*B_loc*cos(2*angles[i]);
    double localpol = pol[i];

    // Calculate A_tilde for each angle
    double A_tilde = ( costerm + delta_L )  / ( 1 + (costerm*delta_L) ) ;
    
   double prob = (localpol<0) ? (0.5*(1 + A_tilde)) : ( 0.5*(1 - A_tilde));
/*
   // Polarisation of 0 corresponds to PERP
    if ( localpol < 0 ){
        prob = 0.5*(1 + A_tilde);
    }

    // Polarisation of 1 corresponds to PARA
    else if ( localpol >= 0 ){
      prob = 0.5*(1 - A_tilde);
    }
*/
    LogL_2_th += log2(prob);
  }
    LogL_2_array[th_id] = LogL_2_th;
#ifdef WITH_OMP
}
#endif
for(int i = 0; i < NTH; i++){
LogL_2 += LogL_2_array[i];
}
  LogL = LogL_2 / Log2e;
  return LogL;

}

//____________________________________________________________________
