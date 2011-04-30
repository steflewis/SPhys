//
// ==================================================================== 
//  
//      Project: sconsNS2
//               ~~~~~~
//  
//      File: TLightHouseNestedSample.cc
//            ~~~~~~~~~~~~~~~~~~~
//
//      Description:     See TLightHouseNestedSample.h
//  
//      Author: Dave Ireland <d.ireland@physics.gla.ac.uk>
//      edited by Stefanie Lewis <s.lewis@physics.gla.ac.uk>
//      Update: 2011-01-017 15:36:46z
//
//      Copyright: 2007 (C) Dave Ireland
//
//      $Id$ 
//
// ==================================================================== 
#ifndef sconsNS2_TLightHouseNestedSample
#include "TLightHouseNestedSample.h"
#endif
using namespace std;
//____________________________________________________________________

//____________________________________________________________________
TLightHouseNestedSample::TLightHouseNestedSample()
{
  
  // Default constructor

}
//_____________________________________________________________________
TLightHouseNestedSample::TLightHouseNestedSample(int numberOfObjects, double logWidth):TNestedSample(numberOfObjects, logWidth)
{
  printf("Entering LH constructor.\n");

  x            = new double[fNSamples];
  y            = new double[fNSamples];
  u            = new double[fNSamples];
  v            = new double[fNSamples];
  postX        = new double[MAX_SAMPLES];
  postY        = new double[MAX_SAMPLES];

  for(int n=0;n<fNSamples;n++){

    x[n]      =0.0;
    y[n]      =0.0;
    u[n]      =0.0;
    v[n]      =0.0;

  }


  fIndex       = 0;
  sampleIndex  = 0;
   

}
//____________________________________________________________________
void
TLightHouseNestedSample::Prior(int fIndex)
{
  // Set samples according to prior
  

  u[fIndex]     = UNIFORM;
    
  v[fIndex]     = UNIFORM;
    
  x[fIndex]     = 4.0*u[fIndex] - 2.0;  //map to x
    
  y[fIndex]     = 2.0*v[fIndex];        //map to y
    

  fLogL[fIndex] = LogLhood(x[fIndex], y[fIndex]);

   
}

//____________________________________________________________________
double
TLightHouseNestedSample::LogLhood (double x, double y)
{
  // logLikelihood function
  const int N  = 64;
  int k;
  double fLogL = 0;
  double D[N]  = { 4.73,  0.45, -1.73,  1.09,  2.19,  0.12,
		   1.31,  1.00,  1.32,  1.07,  0.86, -0.49, -2.59,  1.73,  2.11,
		   1.61,  4.98,  1.71,  2.23,-57.20,  0.96,  1.25, -1.56,  2.45,
		   1.19,  2.17,-10.66,  1.91, -4.16,  1.92,  0.10,  1.98, -2.51,
		   5.55, -0.47,  1.91,  0.95, -0.78, -0.84,  1.72, -0.01,  1.48,
		   2.70,  1.21,  4.41, -4.79,  1.33,  0.81,  0.20,  1.58,  1.29,
		   16.19,  2.75, -2.38, -1.79,  6.50,-18.53,  0.72,  0.94,  3.64,
		   1.94, -0.11,  1.57,  0.57};

  for(k = 0; k < N; k++)
    {
      fLogL   += log( (y/3.1416) / ( (D[k]-x)*(D[k]-x) + y*y ) );
    }
  

}

//____________________________________________________________________
void
TLightHouseNestedSample::Explore (double fLogLstar, int sampleIndex)
{       
  // Evolve object within likelihood constraint
  double step        = 0.1;
  int    accept      = 0;
  int    reject      = 0;
  
  double trialX; 
  double trialY; 
  double trialLogL;
  double a;
  double b;

  int    m           = 20;


  for( ; m > 0; m--)
    {
      
      a          = u[sampleIndex] + step*(2.0*UNIFORM - 1.0);
      b          = v[sampleIndex] + step*(2.0*UNIFORM - 1.0);
      a         -= floor(a);
      b         -= floor(b);
      trialX     = 4.0*a - 2.0;
      trialY     = 2.0*b;
    
      trialLogL  = LogLhood(trialX, trialY);
 
      if(trialLogL > fLogLstar)
      	{
      	  u[sampleIndex]      = a;
      	  v[sampleIndex]      = b;
      	  x[sampleIndex]      = trialX;
      	  y[sampleIndex]      = trialY;
      	  fLogL[sampleIndex]  = trialLogL;
	  fLogWt[sampleIndex] = 0.0;
	  
	  
      	  accept++;
      	}
      else
      	reject++;

      if(accept > reject)
      	step *= exp(1.0/accept);
      if(accept < reject)
      	step /= exp(1.0/reject);
    }

}

//________________________________________________________________________
void TLightHouseNestedSample::PrintSummary(char fPost[])
{
  
  //Print values to screen
  printf("double max: %g\n",                   DBL_MAX                     );
  printf("Number of objects: %d\n",            fNSamples                   );
  printf("Log width: %g\n",                    fLogW                       );
  printf("Information H: %g\n",                fH                          );
  printf("H with final correction: %g\n",      fHFC                        );
  printf("Log(Z): %g +/- %g\n",                fLogZ, sqrt(fH/fNSamples)   );
  printf("Log(Z) with final correction: %g\n", fLogZFC                     );
  printf("Number of iterates: %d\n",           fNoIterates                 );

 
  //Print posterior results to a file
  
  fPosterior.open ("test.txt");

  for (int z = 0; z < ITERATES; z++)
    {
      fPosterior << postX[z];  
      fPosterior << " ";
      fPosterior << postY[z];
      fPosterior << " ";
      fPosterior << fUsedLogL[z];  
      fPosterior << " ";
      fPosterior << fUsedLogWt[z];
      fPosterior << "\n";
    }      
  fPosterior.close();
  printf("Posterior File created.\n");
  
 

}
//____________________________________________________________________
void
TLightHouseNestedSample::TestPrior(int fIndex, double Xnew, double Ynew)
{
  // TEST for Prior() - takes u and v values as arguments.  These
  // are generated by Sivia's version of the code.
  

  u[fIndex]     = Xnew; //UNIFORM;
    
  v[fIndex]     = Ynew; //UNIFORM;
    
  x[fIndex]     = 4.0*u[fIndex] - 2.0;  //map to x
    
  y[fIndex]     = 2.0*v[fIndex];        //map to y
    

  fLogL[fIndex] = LogLhood(x[fIndex], y[fIndex]);

  cout << "Element: " << fIndex << " u v x y: "<<u[fIndex]<<" "<<v[fIndex]<<" "<<x[fIndex]<<" "<<y[fIndex] << " fLogL: " << fLogL[fIndex] << endl;

   
}

//____________________________________________________________________

void
TLightHouseNestedSample::TestValues ()
{
  //Makes data file with initial x,y values - used to make hists.
  ofstream data;
  data.open("priordata.txt");
  for(int h = 0; h < fNSamples; h++){
    data << x[h];
    data << " ";
    data << y[h];
    data << "\n";
  }
  data.close();
  
  

}
//____________________________________________________________________

void
TLightHouseNestedSample::StorePost(int index, int nest, double LogL, double LogWt )
{
  //Used to store the posterior results in arrays.

  postX[nest]      = x[index];
  postY[nest]      = y[index];
  fUsedLogL[nest]  = LogL;
  fUsedLogWt[nest] = LogWt;
  
  

}
//____________________________________________________________________


void
TLightHouseNestedSample::SetToCopy(int worst, int copyIndex)
{
  //Used to set copied sample before going through Explore()
  fLogL[worst]  = fLogL[copyIndex];
  fLogWt[worst] = fLogWt[copyIndex];
  u[worst]      = u[copyIndex];
  v[worst]      = v[copyIndex];
  x[worst]      = x[copyIndex];
  y[worst]      = y[copyIndex];


}
//      End-of-file
// ====================================================================
