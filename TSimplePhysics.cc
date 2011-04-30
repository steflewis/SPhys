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
//      Author: Dave Ireland <d.ireland@physics.gla.ac.uk>
//      edited by Stefanie Lewis <s.lewis@physics.gla.ac.uk>
//      Update: 2011-01-017 15:36:46z
//
//      Copyright: 2007 (C) Dave Ireland
//
//      $Id$ 
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
TSimplePhysics::TSimplePhysics(int numberOfObjects, double logWidth):TNestedSample(numberOfObjects, logWidth)
{
  //printf("Entering constructor.\n");

  // Determine number of events:
  //nEvents = 10113;
  nEvents = 0;
  string line;
  // Read in azimuthal angles from file:
  eventgen.open("datatest.txt");

  while( getline(eventgen, line)){
    nEvents++;
  }

  eventgen.close();
  eventgen.clear();
  printf("Have found number of events: %d\n",nEvents);
  angles = new double[nEvents];
  pol = new double[nEvents];
  B = new float[fNSamples];

  // Declare 2D arrays for x values.  
  // Essentially will be x[8][fNSamples].
  x = new float*[8];
  for (int i = 0; i < 8; i++){
    x[i] = new float[fNSamples];
  }

  
  //printf("Initialising arrays.\n");
  a_1     = new TComplex[fNSamples];
  a_2     = new TComplex[fNSamples];
  a_3     = new TComplex[fNSamples];
  a_4     = new TComplex[fNSamples];

  // Declare / Initialise posterior arrays:
  
  postX = new double*[8];
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

  post_B     = new float[MAX_SAMPLES];
  //printf("About to read from file\n");
  
  eventgen.open("datatest.txt");
  
  // int g = 0;
  
  // while(!eventgen.eof()){
  //   eventgen >> angles[g] >> pol[g];
  //   g++;
  //   printf("Iteration: %d\n",g);
  // }

  for(int i = 0; i<nEvents; i++){
    eventgen >> angles[i];
    eventgen >> pol[i];
    //printf("iteration: %d\t angle: %lf\t polarisation: %lf\n",i,angles[i],pol[i]);
  }
  printf("Finished loop\n");
  eventgen.close();
  eventgen.clear();
  printf("file closed\n");
  // int j = 0;
  // Char_t tempString[111024];
  // while(fgets(tempString,sizeof(tempString),events)!=NULL){
  //   if(sscanf(tempString,"%lf %lf\n", &angles[j], &pol[j] ) != 2){
  //     fprintf(stderr, "There was a problem with the format of the file.\n");
  //     //return 0;
  //   }
  //   j++;
  // }
  // fclose(events);
  // int g = 0;
  // ifstream eventfile ("angles.txt");
  // if (eventfile.is_open()){
  //   while(eventfile.good()){
  //     eventfile >> angles[g];
  //     eventfile >> pol[g];
  //     g++;
  //   }
  // }
  // eventfile.close();

  // FILE *events = fopen("angles.txt","r");
  // char buffer[11024];
  // int g = 0;
  //   while (fgets(&buffer, sizeof(buffer)-1, events)) {
  //   sscanf(buffer, "%lf %lf", &angles[g], &pol[g]);
  //   g++;
  // }

  printf("Reached initialisation of arrays pt 2\n");

  for(int n=0;n<fNSamples;n++){

    B[n]      = 0.0;
    a_1[n]    = TComplex(0.0,0.0);
    a_2[n]    = TComplex(0.0,0.0);
    a_3[n]    = TComplex(0.0,0.0);
    a_4[n]    = TComplex(0.0,0.0);
    // printf("iterate: %d\n",n);

  }
  testPrior = true;
  NewPrior = false; 
  fIndex       = 0;
  sampleIndex  = 0;
  printf("Leaving Constructor\n"); 

}
//____________________________________________________________________
void
TSimplePhysics::Prior(int fIndex)
{
  // Set samples according to prior
  // printf("Entering Prior, %d\n",fIndex);

   

  rSquared = 0;
  for (int i = 0; i < 8; i++) {
    x[i][fIndex] = gRandom->Gaus();
    rSquared += x[i][fIndex]*x[i][fIndex];
  }
  // printf("After first loop\n");
  for (int i = 0; i < 8; i++) {
    x[i][fIndex] /= TMath::Sqrt(rSquared);
  }

  // printf("after second loop\n");
  a_1[fIndex] = TComplex((Double_t)x[0][fIndex],(Double_t)x[1][fIndex]);
  a_2[fIndex] = TComplex((Double_t)x[2][fIndex],(Double_t)x[3][fIndex]);
  a_3[fIndex] = TComplex((Double_t)x[4][fIndex],(Double_t)x[5][fIndex]);
  a_4[fIndex] = TComplex((Double_t)x[6][fIndex],(Double_t)x[7][fIndex]);
  // printf("set complex numbers\n");

  // printf("test of rho thing: %lf\n",a_1[fIndex].Rho2());
  B[fIndex] = (a_1[fIndex].Rho2()) + (a_2[fIndex].Rho2()) - (a_3[fIndex].Rho2()) - (a_4[fIndex].Rho2());
  // printf("found B, about to find log l\n");
  fLogL[fIndex] = LogLhood(B[fIndex]);
  // printf("B: %lf\t LogL: %lf\n",B[fIndex],fLogL[fIndex]);

  if (testPrior == true){
    ofstream oldprior;
    if (fIndex == 0){
      oldprior.open("oldprior.txt");
      cout << "Outputting initial prior text file" << endl;
    }
    else {
      oldprior.open("oldprior.txt",ios::app);
    }
    oldprior << B[fIndex];
    // oldprior << " ";
    // oldprior << fLogL[fIndex];
    oldprior << "\n";

    oldprior.close();
  }

   
}
//____________________________________________________________________
void
TSimplePhysics::UpdatedPrior()
{
  TRandom *gen = new TRandom();
  Double_t pick = 0;

  TFile prior("BposteriorTest.root");

  TTree *tree = (TTree*)gDirectory->Get("tree");

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
    
  Long64_t nentries = tree->GetEntries();
  cout << "No entries: \t" << nentries << endl;

  Float_t sumweights = 0;
  Float_t maxLogWt =-10000;
  for (Long64_t i=0; i<nentries;i++) 
    {
      tree->GetEntry(i);
      if (logWt > maxLogWt)
	maxLogWt = logWt;
    }

    
  for (Long64_t i = 0; i < nentries; i++){
    tree->GetEntry(i);
    // if (i > (nentries - 20)){
    //   if(logWt != 0)cout << logWt <<" I AM LOGWT \t"<<"minLogWt: "<<maxLogWt<< endl;
    //   if(sumweights != 0)cout<<" I AM SUMWEIGHTS "<<sumweights<<endl;
    //   cout << "What to exp: " << TMath::Exp(logWt + TMath::Abs(maxLogWt)) << endl;
    // }

    // sumweights += TMath::Exp((logWt+TMath::Abs(minLogWt))/1000000);
    sumweights += TMath::Exp(logWt + TMath::Abs(maxLogWt));


  }
  cout << "Sumweights: " << sumweights << endl;  
    
  // Main event loop:
  // Pick random number between 0 and sumweights...
    
  Double_t runningsum = 0;
  bool     exitloop = false;
  Double_t nu;
  Double_t threshold;


  for (int j = 0; j < fNSamples; j++){
      
    // This is incorrect XXX  See first eqn on 198 (sivia)

    /************************************
Need pick = Uniform(0,1)
nu is number of samples in sequence
nu <= 1/max(weights)
StairHeight = pick + nu*sumweights

     ************************************/
    pick = gen->Uniform(0.0,1.0);

    nu = fNSamples;
    threshold = 1/(TMath::Exp(maxLogWt));
    printf("Threshold: %lf\n",maxLogWt);
    //This value is too small.... equals zero essentially.  Need to fix.
   
    runningsum = 0;
    
    for (Long64_t i = 0; i < nentries; i++){
      // Get the current entry in the tree
      tree->GetEntry(i);
      // runningsum used as a threshold value 
      runningsum += TMath::Exp(logWt+TMath::Abs(maxLogWt));

      
      if (runningsum > pick){
	// Assign values from the corresponding entry in tree 'tree'
	// where values in tree are priors and x's are used to calculate
	// posterior values
	x[0][j] = a1_Re;
	x[1][j] = a1_Im;
	x[2][j] = a2_Re;
	x[3][j] = a2_Im;
	x[4][j] = a3_Re;
	x[5][j] = a3_Im;
	x[6][j] = a4_Re;
	x[7][j] = a4_Im;
	break;
	
      }
    }
    
    // Calcuate the 'B' observable from Dave's 'spin observables' paper
    a_1[j] = TComplex((Double_t)x[0][j],(Double_t)x[1][j]);
    a_2[j] = TComplex((Double_t)x[2][j],(Double_t)x[3][j]);
    a_3[j] = TComplex((Double_t)x[4][j],(Double_t)x[5][j]);
    a_4[j] = TComplex((Double_t)x[6][j],(Double_t)x[7][j]);

    B[j] = a_1[j].Rho2() + 
           a_2[j].Rho2() - 
           a_3[j].Rho2() - 
           a_4[j].Rho2();

    fLogL[j] = LogLhood(B[j]);
  }

  if (testPrior == true){
    ofstream UPrior;
    
    UPrior.open("newBvals-1.txt");
    
    for (int j = 0; j < fNSamples; j++) {
      UPrior << B[j];
      // UPrior << " ";
      // UPrior << fLogL[j];
      UPrior << "\n";
    }
    UPrior.close();
  }

}

//____________________________________________________________________
double
TSimplePhysics::LogLhood (float B)
{
  // logLikelihood function
  P_gamma = 1;
  delta_L = 0;
  double LogL   = 0;
  
  double prob = 0;   // Probability
  double A_tilde;

  for (int i = 0; i < nEvents; i++){

    // Calculate A_tilde for each angle
    A_tilde = ( (P_gamma*B*cos(2*angles[i])) + delta_L ) 
              / ( 1 + (P_gamma*B*cos(2*angles[i])*delta_L) );
    // printf("A_tilde: %lf\n",A_tilde);
  
    // Polarisation of 0 corresponds to PERP
    if ( pol[i] < 0 ){
      // printf("a-tilde: %lf\n",A_tilde);
      prob = 0.5*(1 + A_tilde);
      // printf("prob: %lf\n",prob);
    }

    // Polarisation of 1 corresponds to PARA
    else if ( pol[i] >= 0 ){
      // printf("a-tilde (para): %lf\n",A_tilde);
      prob = 0.5*(1 - A_tilde);
      // printf("prob (para): %lf\n",prob);
    }
    
    LogL += log(prob);
    // if (i < 20){
    //   printf("Prob: %lf\n",prob);
    //   printf("Log(prob): %lf\n",log(prob));
    //   printf("LogL: %lf\n",LogL);
    // }
  }
  // printf("In logLhood fn: %lf\n",LogL);
  return LogL;

}

//____________________________________________________________________
void
TSimplePhysics::Explore (double fLogLstar, int sampleIndex)
{       
  // Evolve object within likelihood constraint
  double step;
  double fwhm        = 0.1;
  double sig;
  int    accept      = 0;
  int    reject      = 0;
  
  double trialB;  
  double trialLogL;
  float trialx[8];
  float trial_rSquared = 0;

  TComplex trial_a1;
  TComplex trial_a2;
  TComplex trial_a3;
  TComplex trial_a4;

  int    m           = 20;


  for( ; m > 0; m--)
    {

      sig = fwhm / ( 2*( TMath::Sqrt( 2*log(2) ) ) );

      for (int i = 0; i < 8; i++){
	step = gRandom->Gaus(0,sig);

	trialx[i] = x[i][sampleIndex] + step;
	trial_rSquared += trialx[i]*trialx[i];
      }

      for (int i = 0; i < 8; i++){
	trialx[i] /= TMath::Sqrt(trial_rSquared);
      }

      trial_a1 = TComplex((Double_t)trialx[0],(Double_t)trialx[1]);
      trial_a2 = TComplex((Double_t)trialx[2],(Double_t)trialx[3]);
      trial_a3 = TComplex((Double_t)trialx[4],(Double_t)trialx[5]);
      trial_a4 = TComplex((Double_t)trialx[6],(Double_t)trialx[7]);
     
      trialB = trial_a1.Rho2() + trial_a2.Rho2() - trial_a3.Rho2() - trial_a4.Rho2();
      
      trialLogL  = LogLhood(trialB);
 
      if(trialLogL > fLogLstar)
      	{
	  for (int i =0; i < 8; i++){
	    x[i][sampleIndex] = trialx[i];
	  }

	  a_1[sampleIndex] = trial_a1;
	  a_2[sampleIndex] = trial_a2;
	  a_3[sampleIndex] = trial_a3;
	  a_4[sampleIndex] = trial_a4;

	  B[sampleIndex]   = trialB;

      	  fLogL[sampleIndex]  = trialLogL;
	  fLogWt[sampleIndex] = 0.0;
	  
	  
      	  accept++;
      	}
      else
      	reject++;

      if(accept > reject)
      	fwhm *= exp(1.0/accept);
      if(accept < reject)
      	fwhm /= exp(1.0/reject);
    }

}

//________________________________________________________________________
void TSimplePhysics::PrintSummary(char fPost[])
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

  // fNewPrior.open ("newprior.txt");
  // for (int w = 0; w < fNoIterates; w++){
  //   for (int q = 0; q < 8; q++){
  //     fNewPrior << postX[q][w];
  //     fNewPrior << " ";
  //   }
  //   fNewPrior << "\n";


  // }
  // fNewPrior.close();

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
  
  TFile hfile("BposteriorTest.root","RECREATE");
  TTree tree("tree", "Beam recoil posterior");

  printf("Have recreated file.\n");

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
  printf("Have declared branches.\n");
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
    
    tree.Fill();
  }
  printf("Have filled tree.\n");
  tree.Write();
  printf("Root tree file written.\n");
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

  post_B[nest]      = B[index];

  fUsedLogL[nest]  = LogL;
  fUsedLogWt[nest] = LogWt;
  
  

}
//____________________________________________________________________


void
TSimplePhysics::SetToCopy(int worst, int copyIndex)
{
  //Used to set copied sample before going through Explore()
  fLogL[worst]  = fLogL[copyIndex];
  fLogWt[worst] = fLogWt[copyIndex];

  for (int i = 0; i < 8; ++i){
    x[i][worst] = x[i][copyIndex];
  }

  a_1[worst]    = a_1[copyIndex];
  a_2[worst]    = a_2[copyIndex];
  a_3[worst]    = a_3[copyIndex];
  a_4[worst]    = a_4[copyIndex];

  B[worst]      = B[copyIndex];



}
//      End-of-file
// ====================================================================
