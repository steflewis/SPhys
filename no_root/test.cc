#include <math.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include "OclWrapper.h"
#include "TSimplePhysics_GPU.h"

//____________________________________________________________________
//  Dummy program to test / compile kernel function.
//____________________________________________________________________


//____________________________________________________________________
int main(void)
{
  int size = 5; 
  TSimplePhysics_GPU *gpu = new TSimplePhysics_GPU(size);
  
  srand(time(NULL));
  
  float B[size];
  float Pg[size];
  float LogLs[size];
  float cpu_logs[size];
  
  printf("Using GPU programming:\n");
  for (int i = 0; i < size; i++){
    B[i] = rand()/(float)RAND_MAX;
    Pg[i] = rand()/(float)RAND_MAX;
    LogLs[i] = gpu->LogLhood(B[i],Pg[i]);
   printf("Element: %d\t LogL: %lf\n",i,LogLs[i]);
  }
  
  
  // Now use CPU version of likelihood function to compare results.
  printf("Using CPU version:\n");
  for (int i = 0; i < size; i++){

    cpu_logs[i] = gpu->CPU_LogLhood(B[i],Pg[i]);
    printf("Element: %d\t LogL: %lf\n",i,cpu_logs[i]);
    
  }
  return 0;
 
}
//EOF__________________________________________________________________
