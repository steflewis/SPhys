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
  
  test = 2;
  indep = false;

}
//_____________________________________________________________________
TSimplePhysics_CPU::TSimplePhysics_CPU(int numberOfObjects, double logWidth, char* name):TSimplePhysics(numberOfObjects, logWidth, name)
{
  
  
  
}
//____________________________________________________________________
float
TSimplePhysics_CPU::LogLhood (float B_loc, float R_loc, float T_loc, float Ox_loc, float Oz_loc, float Pg_loc)
{

  
  delta_L         = 0;
  float  LogL     = 0.0;
  double costerm  = 0.0;
  double prob     = 0.0;   // Probability
  double A_tilde;
  double localpol = 0.0;
  
  // Recoil likelihood:
  double R_logL = 0.0;
  double R_logl_2 = 0.0;
  double R_prob = 0.0;
  

  // For use of log base 2:
  double LogL_2   = 0.0;
  double A_temp;
  float  angle_term;
  
  for (int i = 0; i < nEvents; i++){
    // Additional attributes used for combined likelihood function
    // So far for all linear beam recoil variables
    
    /*   NEW LOGLIKELIHOOD METHOD!!! */
    
    // Recoil only:
    R_prob = 0.5*(1 + weak*csy[i]*R_loc);
    R_logl_2 += log2(R_prob);
    
    
    // Everything
    angle_term = 2*angles[i];
    
    float f = 1 + weak*csy[i]*R_loc;   //NOT diluted, could multiply by dilution though.
 //   float f = 1 + weak*3*R_loc;
    float g = (B_loc+weak*csy[i]*T_loc)*TMath::Cos(angle_term) + weak*((csx[i]*Ox_loc)+(csz[i]*Oz_loc))*TMath::Sin(angle_term); //Assuming no offset initially.
    
    
    //costerm = Pg*B*cos(2*angles[i]);
    localpol = pol[i];

    // Calculate A_tilde for each angle
    //A_tilde = ( costerm + delta_L )  / ( 1 + (costerm*delta_L) ) ;
    
    //NEW LOGLHOOD
    A_temp = (Pg_loc*g) / f;
    
    A_tilde = (A_temp + delta_L) / (1 + A_temp*delta_L);
    
    // Polarisation of -1 corresponds to PERP
    if ( localpol < 0 ){
        prob = 0.5*(1 + A_tilde);
    }
    
    // Polarisation of 1 corresponds to PARA
    else if ( localpol >= 0 ){
      prob = 0.5*(1 - A_tilde);
    }
    
    LogL_2 += log2(prob);
    
    
  }
  R_logL = R_logl_2 / Log2e;
  LogL = LogL_2 / Log2e;
  return (R_logL+LogL);
  
  
}

//____________________________________________________________________