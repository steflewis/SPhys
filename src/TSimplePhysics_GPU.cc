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

  InitOpenCL();
  
}
//____________________________________________________________________
float
TSimplePhysics::LogLhood (float B, float Pg)
{
  
  // Push CPU arrays to GPU
  wrapper->writeBuffer(cl_B,sizeof(float),B);
  wrapper->writeBuffer(cl_Pg,sizeof(float),Pg);
/*  wrapper->writeBuffer(cl_angles,array_size,angles);
  wrapper->writeBuffer(cl_pols,array_size,pols);
 */ 
  
  
  // Execute kernel
  
  wrapper->enqueueNDRange(cl::NDRange(nEvents), cl::NullRange);
  wrapper->kernel_functor(cl_B, cl_Pg, cl_angles, cl_pols, cl_LogL, Log2e, nEvents).wait();
   
  // Enqueue read buffers
  wrapper->readBuffer(cl_LogL,sizeof(float),LogL);
  

}

//____________________________________________________________________
void TSimplePhysics_GPU::InitOpenCL()
{
  wrapper = new OclWrapper(true);
  
  wrapper->loadKernel("Likelihood2.cl",likelihood);
    // Create OpenCL Arrays

  size_t array_size = sizeof(float)*nEvents;

  cl_B = wrapper->makeReadBuffer(sizeof(float));
  cl_Pg = wrapper->makeReadBuffer(sizeof(float));
  cl_angles = wrapper->makeReadBuffer(array_size);
  cl_pols = wrapper->makeReadBuffer(array_size);
  cl_LogL = wrapper->makeWriteBuffer(sizeof(float));

  wrapper->writeBuffer(cl_angles,array_size,angles);
  wrapper->writeBuffer(cl_pols,array_size,pols);
 
 
}
//____________________________________________________________________