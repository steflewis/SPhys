//
// ==================================================================== 
//  
//      Project: sconsNS2
//               ~~~~~~
//  
//      File: TNestedSample.cc
//            ~~~~~~~~~~~~
//
//      Description:    See TNestedSample.h
//  
//      Author: Stefanie Lewis <s.lewis@physics.gla.ac.uk>
//
//      Date: 01/11/10
//
//      Copyright: 2010 (C) Stefanie Lewis
//
//      $Id$ 
//
// ==================================================================== 

#include "TNestedSample.h"

//_____________________________________________________________________

//_____________________________________________________________________
TNestedSample::TNestedSample()
{
  //Default constructor
  fH              = 0;
}


//_____________________________________________________________________
 TNestedSample::TNestedSample(int numberOfSamples, double logWidth)
 {

   fNSamples       = numberOfSamples;
   fLogL           = new double[fNSamples];
   fLogWt          = new double[fNSamples];
   fUsedLogL       = new double[MAX_SAMPLES];
   fUsedLogWt      = new double[MAX_SAMPLES];



   fLogW           = logWidth;
   fLogZ           = -DBL_MAX;
   fH              = 0;


 }

//_____________________________________________________________________
void TNestedSample::Eval(int fNest, int fWorst)
{
  
  int copy;               
 
  fLogZnew        = PLUS(fLogZ, fLogWt[fWorst]);

  // if(fNest < 10){
  //   printf("fLogZ: %lf\t fLogWt[fWorst]: %lf\t fLogZnew: %lf\n",fLogZ,fLogWt[fWorst],fLogZnew);
  // }
  
  fH              = exp(fLogWt[fWorst] - fLogZnew) * fLogL[fWorst] + exp(fLogZ - fLogZnew) * (fH + fLogZ) - fLogZnew;
  
  fLogZ           = fLogZnew;

  //   if(fNest < 20){
  //   double Sec1 = exp(fLogWt[fWorst] - fLogZnew);
  //   double Sec2 = fLogL[fWorst];
  //   double Sec3 = exp(fLogZ - fLogZnew) * (fH + fLogZ);

  //   printf("fH: %lf\t Sec1: %lf\t Sec2: %lf\t Sec3: %lf\t LogZ: %lf\n",fH,Sec1,Sec2,Sec3,fLogZ);
  // }

  if ((DEBUG == 1)&&(fNest%1000 == 0)){
    printf("Eval - Before Storing Posterior Progress: %d\n",fNest);
  }

  //Store posterior results
  StorePost(fWorst,fNest,fLogL[fWorst],fLogWt[fWorst]);

  if ((DEBUG == 1)&&(fNest%1000 == 0)){
    printf("Eval - After Storing Posterior Progress: %d\n",fNest);
  }
 
  //Overwrite worst object with a copy of random survivor

  do copy          = (int)(fNSamples * UNIFORM) % fNSamples;
  
  while ((copy == fWorst) && (fNSamples > 1));
 
  fLogLstar        = fLogL[fWorst];
 
  SetToCopy(fWorst,copy);
  if ((DEBUG == 1)&&(fNest%1000 == 0)){
    printf("Eval - Before 'Explore' Progress: %d\n",fNest);
  }

  Explore(fLogLstar, fWorst);

  if ((DEBUG == 1)&&(fNest%1000 == 0)){
    printf("Eval - After 'Explore' Progress: %d\n",fNest);
  }


  //Reduce interval
  fLogW          -= (1.0 / double(fNSamples));

}

//_____________________________________________________________________
void TNestedSample::Iterate()
{
  
  fNoIterates     = 0;


  //Test file generated to compare LogL values.
  ofstream testfile;
  testfile.open("testfile.txt");
  for(int f = 0; f < fNSamples; f++){
    testfile << fLogL[f];
    testfile << "\n";
  }
  testfile.close();
  

  //TERMINATION CONDITION
  //for(fNest = 0; fNest <= 2*fNSamples*fH; fNest++)

   
  for (fNest = 0; fNest < ITERATES; fNest++)    
    {
      if ((DEBUG == 1)&&(fNest%1000 == 0)){
	printf("Iterate Progress: %d\n",fNest);
      }
      //Select initial point
      fWorst      = 0;

      for(int i   = 1; i < fNSamples; i++)
	{

	  if((fLogL[i]) < (fLogL[fWorst]))
	    fWorst = i;

	 
	}
	  fLogWt[fWorst] = fLogW + fLogL[fWorst];



      //Call Eval function
      Eval(fNest, fWorst); 

      fNoIterates++;
	  
      //Keeping fNoIterates, despite currently being equivalent to fNest 
      //because fNest increases in FinalCorrection() and I need the 
      //number of iterations before calculating the final correction.

    }
  
  
}

//_______________________________________________________________________

void TNestedSample::FinalCorrection()
{
  //Initialise FC variables to current values from Eval()
  fLogWFC               = -(double)fNest / (double)fNSamples - log((double)fNSamples);
  fLogZFC               = fLogZ;
  fHFC                  = fH;

  for(int i = 0; i < fNSamples; i++){
    
      fLogWt[i]         = fLogWFC + fLogL[i]; //width*likelihood

      fLogZnewFC        = PLUS(fLogZFC, fLogWt[i]);

      
      fHFC              = exp( fLogWt[i] - fLogZnewFC ) * (fLogL[i]) 
                        	+ exp(fLogZFC - fLogZnewFC) * (fHFC + fLogZFC) - fLogZnewFC;

      fLogZFC           = fLogZnewFC;

      int index         = fNest + 1;
      fUsedLogL[index]  = fLogL[i];
      fUsedLogWt[index] = fLogWt[i];
      fNest++;
    }

}

//________________________________________________________________________
void TNestedSample::PrintSummary(char fPost[])
{
 
  int m = 2;

}

//________________________________________________________________________
void TNestedSample::Prior(TNestedSample*,int)
{

  int x = 3;
  
}

//________________________________________________________________________
double TNestedSample::LogLhood(double,double)
{

  double y = 3.*4.;
  return y;
  
}
//________________________________________________________________________
void TNestedSample::Introduce()
{

  std::cout << "Yay!" << std::endl;
  
}
//________________________________________________________________________
void TNestedSample::Explore(double,int)
{

  std::cout << "Yay!" << std::endl;
  
}
//________________________________________________________________________
void TNestedSample::StorePost(int,int,double,double)
{

  std::cout << "Yay!" << std::endl;
  
}
//________________________________________________________________________
void TNestedSample::SetToCopy(int,int)
{

  std::cout << "Yay!" << std::endl;
  
}
//________________________________________________________________________
