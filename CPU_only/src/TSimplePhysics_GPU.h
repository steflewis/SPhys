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

#include "OclWrapper.h"

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

#include <math.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>


//Variable for value of pi:

class TSimplePhysics_GPU : public TSimplePhysics {

  
public:
  // Attributes:
  OclWrapper *wrapper;
  cl::Buffer cl_B, 
  cl_Pg, 
  cl_angles, 
  cl_pols,
  cl_LogL; 
  // Methods:
  
  TSimplePhysics_GPU();
  ~TSimplePhysics_GPU() {};
  
  TSimplePhysics_GPU(int numberOfObjects, double logWidth, char* filename);
  
  float LogLhood(float B, float Pg);
  
  
private:
  
  void InitOpenCL();
  
};
#endif


//      End-of-file
// ====================================================================

  