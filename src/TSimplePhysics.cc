//
// ==================================================================== 
//  
//      Project: sconsNS2
//               ~~~~~~
//  
//      File: TSimplePhysics.cc
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
#ifndef sconsNS2_TSimplePhysics
#include "TSimplePhysics.h"
#endif
using namespace std;
//____________________________________________________________________

//____________________________________________________________________
TSimplePhysics::TSimplePhysics()
{
  
  // Default constructor

}
//_____________________________________________________________________
// Second constructor
TSimplePhysics::TSimplePhysics(int numberOfObjects, double logWidth, char* name):TNestedSample(numberOfObjects, logWidth, name)
,nEvents(0), NewPrior(false), testPrior(true),fIndex(0), sampleIndex(0)
{

  
  // Determine number of events:
  //const char* name = "/home/stefl/NestedSampling/CPU_GPU/text_files/datatest.txt";
  
  nEvents = 0;
  
  nEvents = GetEvents(fDataFile);

  CreateArrays();
  
  ReadData(fDataFile);
/*
  testPrior    = true;
  NewPrior     = false; 
  fIndex       = 0;
  sampleIndex  = 0;
  Log2e        = log2(TMath::E());*/

/*  // Set up OpenCL kernel stuff
  // Need to instantiate an instance of the OclWrapper class - do this in TSimplePhysics constructor.
  OclWrapper *wrapper = new OclWrapper(true);
  // Platform is created in constructor, context and devices are found/created.
  
  // Load and Build Kernel
  wrapper->loadKernel("Likelihood2.cl",likelihood);*/
  
  
  
}
//____________________________________________________________________
void
TSimplePhysics::Prior(int fIndex)
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


  // a_1[fIndex] = TComplex((Double_t)x[0][fIndex],(Double_t)x[1][fIndex]);
  // a_2[fIndex] = TComplex((Double_t)x[2][fIndex],(Double_t)x[3][fIndex]);
  // a_3[fIndex] = TComplex((Double_t)x[4][fIndex],(Double_t)x[5][fIndex]);
  // a_4[fIndex] = TComplex((Double_t)x[6][fIndex],(Double_t)x[7][fIndex]);

  a_1[fIndex] = TComplex(x[0][fIndex],x[1][fIndex]);
  a_2[fIndex] = TComplex(x[2][fIndex],x[3][fIndex]);
  a_3[fIndex] = TComplex(x[4][fIndex],x[5][fIndex]);
  a_4[fIndex] = TComplex(x[6][fIndex],x[7][fIndex]);


  B[fIndex]   = (a_1[fIndex].Rho2()) 
              + (a_2[fIndex].Rho2()) 
              - (a_3[fIndex].Rho2()) 
              - (a_4[fIndex].Rho2());



  //========================================================================//
  // P_gamma set-up
  TRandom *pol_gen = new TRandom();

  //for(int i = 0; i < fNSamples; i++){
    P_gamma[fIndex] = pol_gen->Gaus(0.7, 0.01);
    //P_gamma[i] = pol_gen->Uniform(0.0, 1.0);
  //}

  //========================================================================//

  fLogL[fIndex] = LogLhood(B[fIndex],P_gamma[fIndex]);

  //printf("Point: %d\t LogL: %lf\n",fIndex,fLogL[fIndex]);

  ofstream oldprior;
  if (fIndex == 0){
    oldprior.open("oldprior.txt");
    //    cout << "Outputting initial prior text file" << endl;
  }

  else {
    oldprior.open("oldprior.txt",ios::app);
  }

  oldprior << B[fIndex];
  oldprior << " ";
  oldprior << fLogL[fIndex];
  oldprior << " ";
  oldprior << P_gamma[fIndex];
  oldprior << "\n";

  oldprior.close();
   
}
//____________________________________________________________________
void
TSimplePhysics::UpdatedPrior()
{

  // This will be called when using previous posterior to generate prior.

  TRandom *gen  = new TRandom();
  Double_t pick = 0;

  TFile    prior("BposteriorTest.root");

  TTree   *tree = (TTree*)gDirectory->Get("tree");

  Double_t a1_Re;
  Double_t a1_Im;
  Double_t a2_Re;
  Double_t a2_Im;
  Double_t a3_Re;
  Double_t a3_Im;
  Double_t a4_Re;
  Double_t a4_Im;
  Double_t B_obs;
  Double_t logL_tree;
  Double_t logWt;
  Double_t P_gam;

  //Set branch addresses:
  tree->SetBranchAddress("a1_Re",&a1_Re);
  tree->SetBranchAddress("a1_Im",&a1_Im);
  tree->SetBranchAddress("a2_Re",&a2_Re);
  tree->SetBranchAddress("a2_Im",&a2_Im);
  tree->SetBranchAddress("a3_Re",&a3_Re);
  tree->SetBranchAddress("a3_Im",&a3_Im);
  tree->SetBranchAddress("a4_Re",&a4_Re);
  tree->SetBranchAddress("a4_Im",&a4_Im);
  tree->SetBranchAddress("B_obs",&B_obs);
  tree->SetBranchAddress("logL",&logL_tree);
  tree->SetBranchAddress("logWt",&logWt);
  tree->SetBranchAddress("P_gam",&P_gam);
    
  Long64_t  nentries    = tree->GetEntries();
  Double_t  sumweights  =  0;       // 03/05/2011 - Changed from Float_t to Double_t
  Double_t  maxLogWt    = -10000;   // ''

  for (Long64_t i=0; i<nentries;i++) {
    
    tree->GetEntry(i);
    if (logWt > maxLogWt){
      maxLogWt = logWt;
    }
  }

  Double_t  addedVal    = TMath::Abs(maxLogWt);
  
  for (Long64_t i = 0; i < nentries; i++){
 
    tree->GetEntry(i);
    sumweights += TMath::Exp(logWt + addedVal);
 
  }
   
    
  // Main event loop:
  // Pick random number between 0 and sumweights...
    
  Double_t runningsum = 0;
  Double_t logNu;
  Double_t threshold;
  Double_t stairHeight;
  Double_t old_floor;
  Double_t new_floor;

  Double_t Pgam_sum;
  Double_t Pg_stairHeight;
  
  logNu     = log(fNSamples);
  threshold = -maxLogWt;
  
  // Threshold comes from equation:
  // nu <= 1/maxweights
  // Need to use similar log expression.

    if (logNu > threshold){

      printf("Too many samples! Quit program now.\n");

    }

    else {

      for (int j = 0; j < fNSamples; j++){
	pick = gen->Uniform(0.0,1.0);
	
	Pgam_sum           = 0;
	old_floor          = 0;
	new_floor          = 0;

	for (Long64_t i = 0; i <nentries; i++){
	  
	  tree->GetEntry(i);
	  Pgam_sum        += P_gam;
	  Pg_stairHeight   = pick + fNSamples*Pgam_sum;
	  new_floor        = floor(Pg_stairHeight);
	  
	  if (new_floor > old_floor){
	    P_gamma[j] = P_gam;
	    break;
	  }
	  else {
	    old_floor = new_floor;
	  }

	}

      }
 
      for (int j = 0; j < fNSamples; j++){
      	pick = gen->Uniform(0.0,1.0);
   
 
	runningsum         = 0;
	old_floor          = 0;
	new_floor          = 0;

	for (Long64_t i = 0; i < nentries; i++){

	  // Get the current entry in the tree
	  tree->GetEntry(i);

	  runningsum += TMath::Exp(logWt+addedVal);
	  stairHeight = pick + fNSamples*runningsum;
	  new_floor   = floor(stairHeight);
	  
	  if (new_floor > old_floor){
	   
	    // Use entry i - first time an integer was exceeded.
	   

	    x[0][j]   = a1_Re;
	    x[1][j]   = a1_Im;
	    x[2][j]   = a2_Re;
	    x[3][j]   = a2_Im;
	    x[4][j]   = a3_Re;
	    x[5][j]   = a3_Im;
	    x[6][j]   = a4_Re;
	    x[7][j]   = a4_Im;
	    break;
	    
	  }
	  else {
	    old_floor = new_floor;
	  }
	}
    
	// Calcuate the 'B' observable from Dave's 'spin observables' paper
	// a_1[j] = TComplex((Double_t)x[0][j],(Double_t)x[1][j]);
	// a_2[j] = TComplex((Double_t)x[2][j],(Double_t)x[3][j]);
	// a_3[j] = TComplex((Double_t)x[4][j],(Double_t)x[5][j]);
	// a_4[j] = TComplex((Double_t)x[6][j],(Double_t)x[7][j]);

	a_1[j] = TComplex(x[0][j],x[1][j]);
	a_2[j] = TComplex(x[2][j],x[3][j]);
	a_3[j] = TComplex(x[4][j],x[5][j]);
	a_4[j] = TComplex(x[6][j],x[7][j]);

	B[j] = a_1[j].Rho2() 
	     + a_2[j].Rho2() 
             - a_3[j].Rho2() 
             - a_4[j].Rho2();

	fLogL[j] = LogLhood(B[j],P_gamma[j]);
      }
  
     
      ofstream UPrior;
      
      UPrior.open("newBvals-1.txt");
      
      for (int j = 0; j < fNSamples; j++) {
	UPrior << B[j];
	UPrior << " ";
	UPrior << fLogL[j];
	UPrior << " ";
	UPrior << P_gamma[j];
	UPrior << "\n";
      }

      UPrior.close();
      
      
    }
}
//____________________________________________________________________
float
TSimplePhysics::LogLhood (float B_loc, float Pg_loc)
{
 return B_loc*Pg_loc;
  
}

//____________________________________________________________________
void
TSimplePhysics::Explore (double fLogLstar, int sampleIndex)
{       
  // Evolve object within likelihood constraint
  double   step;
  double   fwhm           = 0.1;           // Full width half maximum
  double   sig;                            // Sigma - to be used in random from Gaussian
  int      accept         = 0;
  int      reject         = 0;
  
  float   trialB;  
  float   trialLogL;
  float   trialx[8];
  float   trial_rSquared = 0;

  TComplex trial_a1;
  TComplex trial_a2;
  TComplex trial_a3;
  TComplex trial_a4;

  int      m              = 20;

//printf("Entered explore function, iterate: %d\n",fNest);
  // Everything to do with the new P_gamma array:
  float   PB;
  float   trial_PB;
  float   trial_Pgam;
  float   step_gamma;
  float   sig_gamma      = 0.05;
  float   fwhm_gamma     = 2*(TMath::Sqrt( 2*log(2)))*sig_gamma;
  bool     flag           = true;

  for( ; m > 0; m--)
    {
      sig       = fwhm / ( 2*( TMath::Sqrt( 2*log(2) ) ) );
      flag = true;

      for (int i = 0; i < 8; i++){
	step = gRandom->Gaus(0,sig);

	trialx[i] = x[i][sampleIndex] + step;
	trial_rSquared += trialx[i]*trialx[i];
      }

      for (int i = 0; i < 8; i++){
	trialx[i] /= TMath::Sqrt(trial_rSquared);
      }

      trial_a1   = TComplex((double)trialx[0],(double)trialx[1]);
      trial_a2   = TComplex((double)trialx[2],(double)trialx[3]);
      trial_a3   = TComplex((double)trialx[4],(double)trialx[5]);
      trial_a4   = TComplex((double)trialx[6],(double)trialx[7]);
    
      trialB     = trial_a1.Rho2() + trial_a2.Rho2() - trial_a3.Rho2() - trial_a4.Rho2();
      
      // P_gamma piece
      PB         = trialB * P_gamma[sampleIndex];
      sig_gamma  = fwhm_gamma / ( 2*( TMath::Sqrt( 2*log(2) ) ) );
      // step_gamma = gRandom->Gaus(0,sig_gamma);
      //while(flag == true){
      //step_gamma = gRandom->Gaus(0,sig_gamma);
/*      if(sig_gamma > 1)
	sig_gamma = 1/sig_gamma;
      step_gamma = gRandom->Uniform(0.0,sig_gamma);*/

while(flag == true){
 step_gamma = gRandom->Gaus(0,sig_gamma);
 trial_PB = PB + step_gamma;
 trial_Pgam = trial_PB/trialB;
       if((trial_Pgam > 0.65) && (trial_Pgam < 0.75))
	 flag = false;
}     
      //trial_PB = PB + step_gamma;

      //trial_Pgam = trial_PB / trialB;

	//if((trial_Pgam > 0) && (trial_Pgam < 1)){
	//  flag = false;
	//}
	//}
//printf("Explore function, about to call likelihood. fNest: %d, m: %d\n",fNest,m);
	//cout <<sampleIndex<<','<< m <<endl;
      trialLogL  = LogLhood(trialB,trial_Pgam);
 //printf("Explore function, finished calling likelihood. fNest: %d, m: %d\n",fNest,m);
      if(trialLogL > fLogLstar)
      	{

	  for (int i =0; i < 8; i++){
	    x[i][sampleIndex]  = trialx[i];
	  }

	  a_1[sampleIndex]     = trial_a1;
	  a_2[sampleIndex]     = trial_a2;
	  a_3[sampleIndex]     = trial_a3;
	  a_4[sampleIndex]     = trial_a4;

	  B[sampleIndex]       = trialB;
	  P_gamma[sampleIndex] = trial_Pgam;
      	  fLogL[sampleIndex]   = trialLogL;
	  fLogWt[sampleIndex]  = 0.0;
	  
	  
      	  accept++;
      	}

      else
      	reject++;

      if(accept > reject){
      	fwhm       *= exp(1.0/accept);
	// fwhm_gamma *= exp(1.0/accept);
      }
      if(accept < reject){
      	fwhm       /= exp(1.0/reject);
	// fwhm_gamma /= exp(1.0/reject);
      }
      // if((sampleIndex % 1000) == 0){
      //  	printf("Sample index: %d\t Pgamma: %lf\t Sigma: %lf\t FWHM: %lf\n",sampleIndex,trial_Pgam,sig_gamma,fwhm_gamma);
      // }
    }
//printf("Leaving explore, fNest: %d\n",fNest);
}

//________________________________________________________________________
void TSimplePhysics::PrintSummary(char fPost[])
{
  
  //Print values to screen
  printf("Number of objects: %d\n",            fNSamples                   );
  printf("Information H: %g\n",                fH                          );
  printf("H with final correction: %g\n",      fHFC                        );
  printf("Log(Z): %g +/- %g\n",                fLogZ, sqrt(fH/fNSamples)   );
  printf("Log(Z) with final correction: %g\n", fLogZFC                     );
  printf("Number of iterates: %d\n",           fNoIterates                 );

  /*
 
  fPosterior.open ("test2.txt");

  for (int z = 0; z < fNoIterates; z++)
    {
      fPosterior << post_a1_Re[z];  
      fPosterior << " ";
      fPosterior << post_a1_Im[z];  
      fPosterior << " ";
      fPosterior << post_a2_Re[z];  
      fPosterior << " ";
      fPosterior << post_a2_Im[z];
      fPosterior << " ";
      fPosterior << post_a3_Re[z];  
      fPosterior << " ";
      fPosterior << post_a3_Im[z];
      fPosterior << " ";
      fPosterior << post_a4_Re[z];  
      fPosterior << " ";
      fPosterior << post_a4_Im[z];
      fPosterior << " ";
      fPosterior << post_B[z];
      fPosterior << " ";
      fPosterior << fUsedLogL[z];  
      fPosterior << " ";
      fPosterior << fUsedLogWt[z];
      fPosterior << "\n";
    }      
  fPosterior.close();
  printf("Posterior File created.\n");
  */

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
  Double_t B_obs;
  Double_t logL;
  Double_t logWt;
  Double_t P_gam;

  tree.Branch("a1_Re",&a1_Re,"a1_Re/D");
  tree.Branch("a1_Im",&a1_Im,"a1_Im/D");
  tree.Branch("a2_Re",&a2_Re,"a2_Re/D");
  tree.Branch("a2_Im",&a2_Im,"a2_Im/D");
  tree.Branch("a3_Re",&a3_Re,"a3_Re/D");
  tree.Branch("a3_Im",&a3_Im,"a3_Im/D");
  tree.Branch("a4_Re",&a4_Re,"a4_Re/D");
  tree.Branch("a4_Im",&a4_Im,"a4_Im/D");
  tree.Branch("B_obs",&B_obs,"B_obs/D");
  tree.Branch("logL",&logL,"logL/D");
  tree.Branch("logWt",&logWt,"logWt/D");
  tree.Branch("P_gam",&P_gam,"P_gam/D");

  for (int r = 0; r < fNoIterates; r++){
    a1_Re = post_a1_Re[r];
    a1_Im = post_a1_Im[r];
    a2_Re = post_a2_Re[r];
    a2_Im = post_a2_Im[r];
    a3_Re = post_a3_Re[r];
    a3_Im = post_a3_Im[r];
    a4_Re = post_a4_Re[r];
    a4_Im = post_a4_Im[r];
    B_obs = post_B[r];
    logL  = fUsedLogL[r];
    logWt = fUsedLogWt[r];
    P_gam = post_Pgam[r];
    
    tree.Fill();
  }

  tree.Write();
}
//____________________________________________________________________

//____________________________________________________________________

void
TSimplePhysics::StorePost(int index, int nest, double LogL, double LogWt )
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

  post_B[nest]     = B[index];
  post_Pgam[nest]  = P_gamma[index];

  fUsedLogL[nest]  = LogL;
  fUsedLogWt[nest] = LogWt;
  
  

}
//____________________________________________________________________


void
TSimplePhysics::SetToCopy(int worst, int copyIndex)
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

  B[worst]       = B[copyIndex];
  P_gamma[worst] = P_gamma[copyIndex];


}

//____________________________________________________________________
int TSimplePhysics::GetEvents(const char* name){
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
void TSimplePhysics::CreateArrays(){
  
  angles  = new float[nEvents];
  pol     = new float[nEvents];
  B       = new float[fNSamples];
  P_gamma = new float[fNSamples];
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

  post_B     = new double[MAX_SAMPLES];
  post_Pgam  = new double[MAX_SAMPLES];
 
  for(int n=0;n<fNSamples;n++){

    B[n]       = 0.0;
    a_1[n]     = TComplex(0.0,0.0);
    a_2[n]     = TComplex(0.0,0.0);
    a_3[n]     = TComplex(0.0,0.0);
    a_4[n]     = TComplex(0.0,0.0);

    P_gamma[n] = 0.0;

  }

  
}
//____________________________________________________________________
void TSimplePhysics::ReadData(const char* name){
 eventgen.open(name);
  

  for(int i = 0; i<nEvents; i++){
    eventgen >> angles[i];
    eventgen >> pol[i];
    }
 
  eventgen.close();
  eventgen.clear();
 printf("Data file test!  angles[7] = %lf\n",angles[7]);
  
}

//      End-of-file
// ====================================================================
