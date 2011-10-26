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
#include "TSimplePhysics_GPU.h"
using namespace std;

//____________________________________________________________________

//____________________________________________________________________
TSimplePhysics_GPU::TSimplePhysics_GPU()
{
  
  // Default constructor

}
//_____________________________________________________________________
TSimplePhysics_GPU::TSimplePhysics_GPU(int numberOfObjects)
{
  printf("There are %d samples\n",numberOfObjects);
     const unsigned int aSize=10093;//1024*1024;
   nEvents = aSize;
   angles = new float[aSize];
   pol = new float[aSize];
   std::cout << "Angles address: " << angles << std::endl;
   for (int i = 0; i < aSize; i++){
    angles[i] = pow(-1.0,i)*0.01*i;
    pol[i] = pow(-1.0,i);
   }
InitOpenCL();
}
//_____________________________________________________________________
float
TSimplePhysics_GPU::LogLhood (float B, float Pg)
{
  nunits = wrapper->deviceInfo.max_compute_units(wrapper->devices[wrapper->deviceIdx]);
//  nunits=4;
  float LogL[nunits];
  
  // Push CPU arrays to GPU
  float B_array[1];
  B_array[0]=B;
  
  float Pg_array[1];
  Pg_array[0]=Pg;
  cl_B = wrapper->makeReadBuffer((int)sizeof(float),B_array,CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR);
  cl_Pg = wrapper->makeReadBuffer((int)sizeof(float),Pg_array,CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR);
cl_LogL = wrapper->makeWriteBuffer((int)sizeof(float)*nunits);

  
 // wrapper->doNothing(); //debugging  
  //wrapper->writeBuffer(cl_B,sizeof(float),B_array);
  //wrapper->writeBuffer(cl_Pg,sizeof(float),Pg_array);
//  wrapper->writeBuffer(cl_angles,array_size,angles);
//  wrapper->writeBuffer(cl_pols,array_size,pols);
  
  // Execute kernel
  // instead of nEvents, use number of compute units.
  const unsigned int NTH=16;
  wrapper->enqueueNDRange(cl::NDRange(nunits*NTH), cl::NDRange(NTH));//cl::NullRange);
  wrapper->kernel_functor(cl_B, cl_Pg, cl_angles, cl_pols, cl_LogL, Log2e, nEvents, nunits).wait();
   
  // Enqueue read buffers
  wrapper->readBuffer(cl_LogL,sizeof(float)*nunits,LogL);
  
  //for debugging:
   
  //LogL[0] = 1;
  float sumLog = 0;
  for (int i = 0; i < nunits;i++){
  //  printf("LogL: %lf\n",LogL[i]);
   sumLog += LogL[i]; 
  }
  return sumLog;

}

//____________________________________________________________________
void TSimplePhysics_GPU::InitOpenCL()
{
  wrapper = new OclWrapper(true);
  array_size = sizeof(float)*nEvents;
  wrapper->loadKernel("Likelihood2.cl","Likelihood2");
    // Create OpenCL Arrays
  cl_angles = wrapper->makeReadBuffer(array_size,angles,CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR);
  cl_pols = wrapper->makeReadBuffer(array_size,pol,CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR);

//  std::cout << "cl_angles address: " << &cl_angles << std:: endl;
// The line below is causing the seg fault.
  //wrapper->writeBuffer(cl_angles,array_size,angles);
  //wrapper->writeBuffer(cl_pols,array_size,pol);

 // cl_B = wrapper->makeReadBuffer((int)sizeof(float));//,NULL, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR);
//     cl_B = wrapper->makeReadBuffer((int)sizeof(float),B,CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR);
//     cl_Pg = wrapper->makeReadBuffer((int)sizeof(float));
//   cl_angles = wrapper->makeReadBuffer(array_size);
//   cl_pols = wrapper->makeReadBuffer(array_size);
//   cl_LogL = wrapper->makeWriteBuffer(sizeof(float));
// 
//   wrapper->writeBuffer(cl_angles,array_size,angles);
//   wrapper->writeBuffer(cl_pols,array_size,pol);

 
}
//____________________________________________________________________
float TSimplePhysics_GPU::CPU_LogLhood(float B, float Pg){
  float logl = 0;
  float prob = 0;
  float costerm = 0;
  float delta_L = 0;
  float A_tilde;
  
  float LogL_2 = 0.0;
  float probsum = 0;
  for (int i = 0; i < nEvents; i++){
    costerm = Pg * B * cos(2*angles[i]);

    A_tilde = (costerm + delta_L) / (1 + (costerm*delta_L));
    //printf("event: %d\t Pg: %lf\t B: %lf\t costerm: %lf\t A_tilde: %lf\t pol: %lf\n",i,Pg,B,costerm,A_tilde,pol[i]);
    
    //A_tilde = angles[i]*Pg;
    
    if (pol[i] < 0)
      prob = 0.5*(1+A_tilde);
    else if (pol[i] >= 0)
      prob = 0.5*(1-A_tilde);
    
    //probsum +=prob;
    LogL_2 += log2(prob);
    //logl += log(prob);
    //printf("prob: %lf\t logl: %lf\n",prob,logl);
  }
  logl = LogL_2 / Log2e;
  return logl;
  
  
}
//____________________________________________________________________
// ====================================================================
