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
#ifndef TSIMPLEPHYSICS_GPU_NOROOT_H
#define TSIMPLEPHYSICS_GPU_NOROOT_H

#include "OclWrapper.h"


#include <math.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>


//Variable for value of pi:
const float Log2e = M_LOG2E;
class TSimplePhysics_GPU {

  
public:
  // Attributes:
  OclWrapper *wrapper;
  cl::Buffer cl_B, 
  cl_Pg, 
  cl_angles, 
  cl_pols,
  cl_LogL; 
  
  size_t array_size;
  // Methods:
  
  TSimplePhysics_GPU();
  ~TSimplePhysics_GPU() {};
  
  TSimplePhysics_GPU(int numberOfObjects);
  
  float LogLhood(float B, float Pg);
  float CPU_LogLhood(float B, float Pg);
  
  float *angles;
  float *pol;
  
  int nEvents;
  
private:
  
  void InitOpenCL();
  
};
#endif


//      End-of-file
// ====================================================================

  