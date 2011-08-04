//
// ==================================================================== 
//  
//      Project: sconsNS2
//               ~~~~~~
//  
//      File: TSimplePhysics_GPU.cc
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
#ifndef sconsNS2_TSimplePhysics_GPU
#include "TSimplePhysics_GPU.h"
#endif
using namespace std;
//____________________________________________________________________

//____________________________________________________________________
TSimplePhysics_GPU::TSimplePhysics_GPU()
{
  
  // Default constructor

}
//_____________________________________________________________________
TSimplePhysics_GPU::TSimplePhysics_GPU(int numberOfObjects, double logWidth) : TSimplePhysics(numberOfObjects, logWidth)
{
  
  
  
}
//____________________________________________________________________
float
TSimplePhysics::LogLhood (float B, float Pg)
{
  // Variable Declarations
  unsigned int num = 3000;
  
  size_t array_size = sizeof(double)*num;

  // Create OpenCL Arrays
  cl_B = wrapper->makeReadBuffer(1);
  cl_Pg = wrapper->makeReadBuffer(1);
  cl_angles = wrapper->makeReadBuffer(array_size);
  cl_pols = wrapper->makeReadBuffer(array_size);
  cl_LogL = wrapper->makeWriteBuffer(1);
  
  // Set kernel arguments - how?
/*  err = kernel.setArg(0, cl_B);
  err = kernel.setArg(1, cl_Pg);
  err = kernel.setArg(2, cl_LogL);

  // Constant arguments:
  err = kernel.setArg(3, Log2e);

  cl::Event event;*/
  
  // Create command queue
  wrapper->createQueue();
  
  // Push CPU arrays to GPU
  wrapper->writeBuffer(cl_B,1,B);
  wrapper->writeBuffer(cl_Pg,1,Pg);
  wrapper->writeBuffer(cl_angles,array_size,angles);
  wrapper->writeBuffer(cl_pols,array_size,pols);
  
  // Execute kernel
  // really not sure what to do here.
  wrapper->enqueueNDRange();
  // no idea what should be in the parentheses
  
  // Enqueue read buffers
  wrapper->readBuffer(cl_LogL,1,LogL);
  

}

//____________________________________________________________________