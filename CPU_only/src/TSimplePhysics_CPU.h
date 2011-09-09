// -*- mode: c++ -*- 
//
// ==================================================================== 
//  
//      Project: sconsNS4
//               ~~~~~~
//  
//      File: TSimplePhysics_CPU.h
//            ~~~~~~~~~~~~~~~~~~
//
//      Description:     
//      This determines the most likely value for the B observable.
//      Uses standard CPU processing power.
//
// ==================================================================== 
#ifndef TSIMPLEPHYSICS_CPU_H
#define TSIMPLEPHYSICS_CPU_H

#include "TSimplePhysics.h"
#include "TNestedSample.h"



//Variable for value of pi:
/*const double pi = M_PI;
const double weak = 0.642;       // weak decay constant
const double dilution = -0.256;  // sigma dilution*/
 

class TSimplePhysics_CPU : public TSimplePhysics {

  
public:
  
  // Methods:
  
  TSimplePhysics_CPU();
  ~TSimplePhysics_CPU() {};
  
  TSimplePhysics_CPU(int numberOfObjects, double logWidth, char* name);
  
  float LogLhood(float B_loc, float Pg_loc);
  
  // Attributes:
  // all should be inherited from TSimplePhysics  
};
#endif


//      End-of-file
// ====================================================================

  