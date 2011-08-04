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

#include "TSimplePhysics_CPU.h"


using namespace std;
//____________________________________________________________________

//____________________________________________________________________
TSimplePhysics_CPU::TSimplePhysics_CPU()
{
  
  // Default constructor

}
//_____________________________________________________________________
TSimplePhysics_CPU::TSimplePhysics_CPU(int numberOfObjects, double logWidth):TSimplePhysics(numberOfObjects, logWidth)
{
  
  
  
}
//____________________________________________________________________
float
TSimplePhysics_CPU::LogLhood (float B, float Pg)
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
  delta_L         = 0;
  double LogL     = 0.0;
  double costerm  = 0.0;
  double prob     = 0.0;   // Probability
  double A_tilde;
  double localpol = 0.0;

  // For use of log base 2:
  double LogL_2   = 0.0;
  //double A_temp;

  for (int i = 0; i < nEvents; i++){
    
    costerm = Pg*B*cos(2*angles[i]);
    localpol = pol[i];

    // Calculate A_tilde for each angle
    A_tilde = ( costerm + delta_L )  / ( 1 + (costerm*delta_L) ) ;
    
   // Polarisation of 0 corresponds to PERP
    if ( localpol < 0 ){
        prob = 0.5*(1 + A_tilde);
    }

    // Polarisation of 1 corresponds to PARA
    else if ( localpol >= 0 ){
      prob = 0.5*(1 - A_tilde);
    }
    LogL_2 += log2(prob);

 
  }
  LogL = LogL_2 / Log2e;
  return LogL;

}

//____________________________________________________________________