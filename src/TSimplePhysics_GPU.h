// -*- mode: c++ -*- 
//
// ==================================================================== 
//  
//      Project: sconsNS4
//               ~~~~~~
//  
//      File: TSimplePhysics_GPU.h
//            ~~~~~~~~~~~~~~~~~~
//
//      Description:     
//      This determines the most likely value for the B observable.
//      Uses GPU programming (OpenCL).
//
// ==================================================================== 
#ifndef TSIMPLEPHYSICS_GPU_H
#define TSIMPLEPHYSICS_GPU_H

#include "TROOT.h"
#include "TTree.h"
#include "TMinuit.h"
#include "TMath.h"
#include "TRandom.h"
#include "TComplex.h"
#include "TFile.h"
#include "Riostream.h"
#include "Rtypes.h"

#include "TNestedSample.h"
#include "TSimplePhysics.h"
#include "OclWrapper.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>


//Variable for value of pi:
const double pi = M_PI;
const double weak = 0.642;       // weak decay constant
const double dilution = -0.256;  // sigma dilution
 

class TSimplePhysics_GPU : public TSimplePhysics {

  
public:
  
  // Methods:
  
  TSimplePhysics_GPU();
  ~TSimplePhysics_GPU() {};
  
  TSimplePhysics_GPU(int numberOfObjects, double logWidth);
  
  float LogLhood(float B, float Pg);
  
  // Attributes:
  OclWrapper *wrapper;
  
private:
  
  void InitOpenCL();
  
};
#endif


//      End-of-file
// ====================================================================

  