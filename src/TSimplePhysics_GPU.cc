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
TSimplePhysics_GPU::TSimplePhysics_GPU(int numberOfObjects, double logWidth, char* filename) : TSimplePhysics(numberOfObjects, logWidth, filename),
    ocl(true,"Likelihood2.cl","Likelihood2","-cl-fast-relaxed-math")
{

  InitOpenCL();

}
//____________________________________________________________________
float
TSimplePhysics_GPU::LogLhood (float B_obs, float Pg_obs)
{
  
  // Determine number of cores present on machine
  // Can use any number, but no advantage when nunits is set > number of cores.
  float* tmpLogL=new float[nunits];
  // Push CPU arrays to GPU
  B_Pg_array[0]=B_obs;
  B_Pg_array[1]=Pg_obs;
  int flt_sz=(int)sizeof(float);
  // Enqueue write buffer
  ocl.writeBuffer(cl_B_Pg,2*flt_sz,B_Pg_array);
  // The arguments to the enqueueNDRange function essentially determine grid over which the data is divided into sections
  int ncalls = ocl.enqueueNDRange(cl::NDRange(nunits*NTH), cl::NDRange(NTH));
  // Execute kernel
  // WV: the KernFunctor () operator requires references, even for constants!
  const float& Log2e_r = Log2e;
  const int& nEvents_r = nEvents;
  const int& nunits_r = nunits;
  ocl.kernel_functor( cl_B_Pg,cl_angles,cl_pols,cl_LogL,Log2e_r,nEvents_r,nunits_r ).wait();
  // Enqueue read buffer
  ocl.readBuffer(cl_LogL,flt_sz*nunits,tmpLogL);
//std::cout << ncalls << "\n";
  float sumLog = 0;
  for (int i = 0; i < nunits; i++){
//std::cout <<tmpLogL[i]<<"\n"; 
      sumLog += tmpLogL[i];    
  }
//if (ncalls>=10) {
//exit(1);
//}
  return sumLog; 

}

//____________________________________________________________________
void TSimplePhysics_GPU::InitOpenCL()
{ 
  array_size = sizeof(float)*nEvents;
//  wrapper = new OclWrapper(true,"Likelihood2.cl","Likelihood2");
  nunits=ocl.getMaxComputeUnits();
/*
  wrapper = new OclWrapper(true);
  nunits = ocl.deviceInfo.max_compute_units(ocl.devices[ocl.deviceIdx]);
  ocl.loadKernel("Likelihood2.cl","Likelihood2");
*/
	
  printf("angle test: %lf\n",angles[7]);
  printf("thread test: %d\n",NTH);
  // Create the Command Queue
  //  ocl.createQueue();
  // Create OpenCL Arrays
  int flt_size=(int)sizeof(float);
  cl_B_Pg = ocl.makeReadBuffer(2*flt_size);
  cl_angles = ocl.makeReadBuffer(array_size,angles);
  cl_pols = ocl.makeReadBuffer(array_size,pol);
  cl_LogL = ocl.makeWriteBuffer(nunits*flt_size);
// printf("nunits test: %d\n",ocl.deviceInfo.max_compute_units(ocl.devices[ocl.deviceIdx]));
  printf("nEvents test: %d\n",nEvents);
  
}
//____________________________________________________________________
