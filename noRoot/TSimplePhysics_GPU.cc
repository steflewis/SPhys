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
 // fNSamples = 5;
  nEvents = 10;//1024*1024;
  angles = new float[nEvents];
  pol = new float[nEvents];
  
  for (int i = 0; i < nEvents; i++){
    
   angles[i] = pow(-1.0,i)*0.01*i;
   pol[i] = pow(-1.0,i);
    
  }
  
  
  // Default constructor
InitOpenCL();



}
//_____________________________________________________________________
// TSimplePhysics_GPU::TSimplePhysics_GPU(int numberOfObjects, double logWidth, char* filename) : TSimplePhysics(numberOfObjects, logWidth, filename)  
// {
// 
//   InitOpenCL();
// 
// }
//____________________________________________________________________
float
TSimplePhysics_GPU::LogLhood (float B_obs, float Pg_obs)
{
  
  // Determine number of cores present on machine
  // Can use any number, but no advantage when nunits is set > number of cores.
  float tmpLogL[nunits];
  // Push CPU arrays to GPU
  B_Pg_array[0]=B_obs;
  B_Pg_array[1]=Pg_obs;
cout << "B: "<<B_Pg_array[0]<< "Pg: "<< B_Pg_array[1]<<endl;
  //flt_sz=(int)sizeof(float);
  // Enqueue write buffer
  wrapper->writeBuffer(cl_B_Pg,2*flt_size,B_Pg_array);
  // The arguments to the enqueueNDRange function essentially determine grid over which the data is divided into sections
  int ncalls = wrapper->enqueueNDRange(cl::NDRange(nunits*NTH), cl::NDRange(NTH));
  // Execute kernel
  wrapper->kernel_functor(cl_B_Pg, cl_angles, cl_pols, cl_LogL, Log2e, nEvents,nunits).wait();
//  std::cout << ncalls <<"\n";
  // Enqueue read buffer
  wrapper->readBuffer(cl_LogL,flt_size*nunits,tmpLogL);

  float sumLog = 0;
  for (int i = 0; i < nunits; i++){
    
   sumLog += tmpLogL[i];    
  
    
  }
  //sumLog = tmpLogL[1];
printf("nunits: %d\tLogL: %lf\n",nunits,sumLog);  
  return sumLog; 

}

//____________________________________________________________________
void TSimplePhysics_GPU::InitOpenCL()
{
  array_size = sizeof(float)*nEvents;
  wrapper = new OclWrapper(true,"Likelihood2.cl","Likelihood2");
  nunits=wrapper->getMaxComputeUnits();
  printf("Nunits: %d\n",nunits);
/*
  wrapper = new OclWrapper(true);
  nunits = wrapper->deviceInfo.max_compute_units(wrapper->devices[wrapper->deviceIdx]);

  wrapper->loadKernel("Likelihood2.cl","Likelihood2");
*/
	
  printf("angle test: %lf\n",angles[7]);
  printf("thread test: %d\n",NTH);
//  B_Pg_array = new float[2];
  // Create the Command Queue
//  wrapper->createQueue();
  // Create OpenCL Arrays
  flt_size=(int)sizeof(float);
  cl_B_Pg = wrapper->makeReadBuffer(2*flt_size);
  cl_angles = wrapper->makeReadBuffer(array_size,angles);
  cl_pols = wrapper->makeReadBuffer(array_size,pol);
  cl_LogL = wrapper->makeWriteBuffer(nunits*flt_size);
// printf("nunits test: %d\n",wrapper->deviceInfo.max_compute_units(wrapper->devices[wrapper->deviceIdx]));
  printf("nEvents test: %d\n",nEvents);
  
}
//____________________________________________________________________
float TSimplePhysics_GPU::CPU_LogLhood(float B, float Pg)
{
 float logl = 0;
 float prob = 0;
 float costerm = 0;
 float delta_L = 0;
 float A_tilde;
 
 float LogL_2 = 0.0;
 float probsum = 0;
 for (int i = 0; i < nEvents; i++){
  costerm = Pg*B*cos(2*angles[i]);
  A_tilde = (costerm+delta_L) / (1+(costerm*delta_L));
  
  if (pol[i] < 0)
    prob = 0.5*(1+A_tilde);
  else if (pol[i] >= 0)
    prob = 0.5*(1-A_tilde);
  
  LogL_2 += log2(prob);
  
 }
  
  //logl = LogL_2 / Log2e;
  logl = B+Pg;
  
  return logl;
  
  
  
}