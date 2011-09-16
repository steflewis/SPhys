//
// ==================================================================== 
//  
//      Project: sconsNS2
//               ~~~~~~
//  
//      File: TRecoil.cc
//            ~~~~~~~~~~~~~~~~~~~
//
//      Description:     See TRecoil.h
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
#ifndef sconsNS2_TRecoil
#include "TRecoil.h"
#endif
using namespace std;
//using namespace TComplex;
//____________________________________________________________________

//____________________________________________________________________
TRecoil::TRecoil()
{
  
  // Default constructor

}
//_____________________________________________________________________
// Second constructor
TRecoil::TRecoil(int numberOfObjects, double logWidth, char* name):TNestedSample(numberOfObjects, logWidth, name)
,nEvents(0), NewPrior(false), testPrior(true),fIndex(0), sampleIndex(0)
{
  
  
  // Determine number of events:
  //const char* name = "/home/stefl/NestedSampling/CPU_GPU/text_files/datatest.txt";
  
  nEvents = 0;
  
  nEvents = GetEvents(fDataFile);
  
  CreateArrays();
  
  ReadData(fDataFile);
  /*
   t estPrior    = true;                                 *
   NewPrior     = false; 
   fIndex       = 0;
   sampleIndex  = 0;
   Log2e        = log2(TMath::E());*/
  
  /*  // Set up OpenCL kernel stuff
   / / Need to instantiate an instance of the OclWrapper *class - do this in TSimplePhysics constructor.
   OclWrapper *wrapper = new OclWrapper(true);
   // Platform is created in constructor, context and devices are found/created.
   
   // Load and Build Kernel
   wrapper->loadKernel("Likelihood2.cl",likelihood);*/
  
  pi = M_PI;
  weak = 0.642;       // weak decay constant
  dilution = -0.256;  // sigma dilution
  Log2e = log2(TMath::E());
  FWHM_SIG = 2*(TMath::Sqrt(2*log(2)) );
  
  
  
  
}
//____________________________________________________________________
void
TRecoil::Prior(int fIndex)
{
 
  // Set samples according to prior
  // This method will be called if no evolving prior is used. 

  rSquared     = 0;
  for (int i = 0; i < 8; i++) {

    x[i][fIndex] = gRandom->Gaus();
    rSquared    += x[i][fIndex]*x[i][fIndex];

  }

  for (int i = 0; i < 8; i++) {
    x[i][fIndex] /= TMath::Sqrt(rSquared);
  }



  a_1[fIndex] = TComplex(x[0][fIndex],x[1][fIndex]);
  a_2[fIndex] = TComplex(x[2][fIndex],x[3][fIndex]);
  a_3[fIndex] = TComplex(x[4][fIndex],x[5][fIndex]);
  a_4[fIndex] = TComplex(x[6][fIndex],x[7][fIndex]);

  R[fIndex] = Calculate_R(a_1[fIndex], a_2[fIndex], a_3[fIndex], a_4[fIndex]);

  fLogL[fIndex] = LogLhood(R[fIndex]);



  ofstream oldprior;
  if (fIndex == 0){
    oldprior.open("recoilprior.txt");
    //    cout << "Outputting initial prior text file" << endl;
  }

  else {
    oldprior.open("recoilprior.txt",ios::app);
  }

  oldprior << R[fIndex];
  oldprior << " ";
  oldprior << fLogL[fIndex];
  oldprior << "\n";
  
  oldprior.close();
   
}
//____________________________________________________________________

float
TRecoil::LogLhood (float R_loc)
{
 float logL;
 float LogL_2 = 0;
 float prob = 0;
 for (int i = 0; i < nEvents; i++){
   prob = 0.5*(1 + weak*csy[i]*R_loc);
   LogL_2 += log2(prob);
 }
 logL = LogL_2 / Log2e;
 return logL;
}

//____________________________________________________________________
void
TRecoil::Explore (double fLogLstar, int sampleIndex)
{       
  // Evolve object within likelihood constraint
  double   step;
  double   fwhm           = 0.1;           // Full width half maximum
  double   sig;                            // Sigma - to be used in random from Gaussian
  int      accept         = 0;
  int      reject         = 0;
  

  float    trialR;

  double   trialLogL;
  double   trialx[8];
  double   trial_rSquared = 0;

  TComplex trial_a1;
  TComplex trial_a2;
  TComplex trial_a3;
  TComplex trial_a4;

  int      m              = 20;


 
  

  bool     flag           = true;

  for( ; m > 0; m--)
    {

      sig       = fwhm / FWHM_SIG;
      flag = true;

      for (int i = 0; i < 8; i++){
	step = gRandom->Gaus(0,sig);

	trialx[i] = x[i][sampleIndex] + step;
	trial_rSquared += trialx[i]*trialx[i];
      }

      for (int i = 0; i < 8; i++){
	trialx[i] /= TMath::Sqrt(trial_rSquared);
      }

      trial_a1   = TComplex(trialx[0],trialx[1]);
      trial_a2   = TComplex(trialx[2],trialx[3]);
      trial_a3   = TComplex(trialx[4],trialx[5]);
      trial_a4   = TComplex(trialx[6],trialx[7]);
    
      trialR     = Calculate_R(trial_a1, trial_a2, trial_a3, trial_a4);

      
      trialLogL  = LogLhood(trialR);
 
      if(trialLogL > fLogLstar)
      	{

	  for (int i =0; i < 8; i++){
	    x[i][sampleIndex]  = trialx[i];
	  }

	  a_1[sampleIndex]     = trial_a1;
	  a_2[sampleIndex]     = trial_a2;
	  a_3[sampleIndex]     = trial_a3;
	  a_4[sampleIndex]     = trial_a4;

      	  fLogL[sampleIndex]   = trialLogL;
	  R[sampleIndex]       = trialR;
	  fLogWt[sampleIndex]  = 0.0;
	  
	  
      	  accept++;
      	}

      else
      	reject++;

      if(accept > reject){
      	fwhm       *= exp(1.0/accept);
	//fwhm_gamma *= exp(1.0/accept);
      }
      if(accept < reject){
      	fwhm       /= exp(1.0/reject);
	//fwhm_gamma /= exp(1.0/reject);
      }
      // if((sampleIndex % 1000) == 0){
      //  	printf("Sample index: %d\t Pgamma: %lf\t Sigma: %lf\t FWHM: %lf\n",sampleIndex,trial_Pgam,sig_gamma,fwhm_gamma);
      // }
    }

}

//________________________________________________________________________
void TRecoil::PrintSummary(char fPost[])
{
  
  //Print values to screen
  printf("Number of objects: %d\n",            fNSamples                   );
  printf("Information H: %g\n",                fH                          );
  printf("H with final correction: %g\n",      fHFC                        );
  printf("Log(Z): %g +/- %g\n",                fLogZ, sqrt(fH/fNSamples)   );
  printf("Log(Z) with final correction: %g\n", fLogZFC                     );
  printf("Number of iterates: %d\n",           fNoIterates                 );

  // Output posterior to ROOT file
  TFile    hfile(fPost,"RECREATE");

  // TFile    hfile("BposteriorTest.root","RECREATE");
  TTree    tree("tree", "Beam recoil posterior");


  Double_t a1_Re;
  Double_t a1_Im;
  Double_t a2_Re;
  Double_t a2_Im;
  Double_t a3_Re;
  Double_t a3_Im;
  Double_t a4_Re;
  Double_t a4_Im;

  Double_t R_obs;
  Double_t logL;
  Double_t logWt;



  tree.Branch("a1_Re",&a1_Re,"a1_Re/D");
  tree.Branch("a1_Im",&a1_Im,"a1_Im/D");
  tree.Branch("a2_Re",&a2_Re,"a2_Re/D");
  tree.Branch("a2_Im",&a2_Im,"a2_Im/D");
  tree.Branch("a3_Re",&a3_Re,"a3_Re/D");
  tree.Branch("a3_Im",&a3_Im,"a3_Im/D");
  tree.Branch("a4_Re",&a4_Re,"a4_Re/D");
  tree.Branch("a4_Im",&a4_Im,"a4_Im/D");
  tree.Branch("R_obs",&R_obs,"R_obs/D");
  tree.Branch("logL",&logL,"logL/D");
  tree.Branch("logWt",&logWt,"logWt/D");

  for (int r = 0; r < fNoIterates; r++){
    a1_Re = post_a1_Re[r];
    a1_Im = post_a1_Im[r];
    a2_Re = post_a2_Re[r];
    a2_Im = post_a2_Im[r];
    a3_Re = post_a3_Re[r];
    a3_Im = post_a3_Im[r];
    a4_Re = post_a4_Re[r];
    a4_Im = post_a4_Im[r];
    R_obs = post_R[r];
    logL  = fUsedLogL[r];
    logWt = fUsedLogWt[r];
    
    tree.Fill();
  }

  tree.Write();
}
//____________________________________________________________________

//____________________________________________________________________

void
TRecoil::StorePost(int index, int nest, double LogL, double LogWt )
{
  //Used to store the posterior results in arrays.

  post_a1_Re[nest]      = a_1[index].Re();
  post_a1_Im[nest]      = a_1[index].Im();
  post_a2_Re[nest]      = a_2[index].Re();
  post_a2_Im[nest]      = a_2[index].Im();
  post_a3_Re[nest]      = a_3[index].Re();
  post_a3_Im[nest]      = a_3[index].Im();
  post_a4_Re[nest]      = a_4[index].Re();
  post_a4_Im[nest]      = a_4[index].Im();

  for (int i = 0; i < 8; i++){
    postX[i][nest]      = x[i][index];
  }

  post_R[nest]     = R[index];

  fUsedLogL[nest]  = LogL;
  fUsedLogWt[nest] = LogWt;
  
  

}
//____________________________________________________________________


void
TRecoil::SetToCopy(int worst, int copyIndex)
{
  //Used to set copied sample before going through Explore()
  fLogL[worst]   = fLogL[copyIndex];
  fLogWt[worst]  = fLogWt[copyIndex];

  for (int i = 0; i < 8; ++i){
    x[i][worst]  = x[i][copyIndex];
  }

  a_1[worst]     = a_1[copyIndex];
  a_2[worst]     = a_2[copyIndex];
  a_3[worst]     = a_3[copyIndex];
  a_4[worst]     = a_4[copyIndex];

  R[worst]       = R[copyIndex];


}

//____________________________________________________________________
int TRecoil::GetEvents(const char* name){
    // Read in azimuthal angles from file:
  string line;
  eventgen.open(name);
  int events = 0;
  while( getline(eventgen, line)){
    events++;
  }

  eventgen.close();
  eventgen.clear();

  return events;
  
}
//____________________________________________________________________
void TRecoil::CreateArrays(){
  
  angles  = new float[nEvents];
  pol     = new float[nEvents];
  csx     = new float[nEvents];
  csy     = new float[nEvents];
  csz     = new float[nEvents];
  R       = new float[fNSamples];
  
  // Declare 2D arrays for x values.  
  // Essentially will be x[8][fNSamples].

  x       = new double*[8];
 
 for (int i = 0; i < 8; i++){
    x[i] = new double[fNSamples];
  }

  

  a_1     = new TComplex[fNSamples];
  a_2     = new TComplex[fNSamples];
  a_3     = new TComplex[fNSamples];
  a_4     = new TComplex[fNSamples];

  // Declare & Initialise posterior arrays:
  
  postX      = new double*[8];
  for (int i = 0; i < 8; i++){
    postX[i] = new double[MAX_SAMPLES];
  }

  post_a1_Re = new double[MAX_SAMPLES];
  post_a1_Im = new double[MAX_SAMPLES];
  post_a2_Re = new double[MAX_SAMPLES];
  post_a2_Im = new double[MAX_SAMPLES];
  post_a3_Re = new double[MAX_SAMPLES];
  post_a3_Im = new double[MAX_SAMPLES];
  post_a4_Re = new double[MAX_SAMPLES];
  post_a4_Im = new double[MAX_SAMPLES];

  post_R     = new double[MAX_SAMPLES];
 
  for(int n=0;n<fNSamples;n++){

    a_1[n]     = TComplex(0.0,0.0);
    a_2[n]     = TComplex(0.0,0.0);
    a_3[n]     = TComplex(0.0,0.0);
    a_4[n]     = TComplex(0.0,0.0);

    R[n]       = 0.0;

  }

  
}
//____________________________________________________________________
void TRecoil::ReadData(const char* name){
 eventgen.open(name);
  

  for(int i = 0; i<nEvents; i++){
    eventgen >> angles[i];
    eventgen >> pol[i];
    eventgen >> csx[i];
    eventgen >> csy[i];
    eventgen >> csz[i];
    }
 
  eventgen.close();
  eventgen.clear();
 
  
}
//____________________________________________________________________

float TRecoil::Calculate_R(TComplex a1, TComplex a2, TComplex a3, TComplex a4){
  
  float R_calc;
  R_calc    = (a1.Rho2()) - (a2.Rho2()) + (a3.Rho2()) - (a4.Rho2());
  
  return R_calc;
  
  
}

//      End-of-file
// ====================================================================
