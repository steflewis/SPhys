// -*- mode: c++ -*- 
//
// ==================================================================== 
//  
//      Project: sconsNS4
//               ~~~~~~
//  
//      File: TSimplePhysics.h
//            ~~~~~~~~~~~~~~~~~~
//
//      Description:     
//      This determines the most likely value for the B observable.
//
// ==================================================================== 
#ifndef sconsNS4_TSimplePhysics
#define sconsNS4_TSimplePhysics

#include "TROOT.h"
#include "TTree.h"
#include "TMinuit.h"
#include "TMath.h"
#include "TRandom.h"
#include "TComplex.h"
#include "TFile.h"
#include "Riostream.h"
#include "Rtypes.h"

#include "TNestedSample.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>


//Variable for value of pi:
const double pi = M_PI;


class TSimplePhysics : public TNestedSample {


  // Public stuff
public:
  // Operations
  TSimplePhysics();
  ~TSimplePhysics() {};

  TSimplePhysics(int numberOfObjects, double logWidth);
    
  // Set object according to uniform prior
  void     Prior (int fIndex);
  void     TestPrior (int fIndex, double, double); 
  void     UpdatedPrior();

  // logLikelihood function
  double   LogLhood (float B);
        
  // Evolve object within likelihood constraint
  void     Explore (double logLstar, int sampleIndex);
  void     SetToCopy(int worst, int copyIndex);

  void     PrintSummary(char*);
  void     TestValues();
  void     StorePost(int, int, double, double);

  // double   *x;	// Easterly position between -2 and +2
  // double   *y;	// Northerly position between 0 and 2
  // double   *u;        // Uniform prior parameter for x
  // double   *v;        // Uniform prior parameter for y

  // Variables required for Prior():
  float    **x;          // Hold random numbers - will be array[8] where each 
                         // are re-defined for each value of B (i.e. each
                         // sample).


  float     rSquared;   // distance squared
  

  TComplex *a_1;  // Complex variables - there are 4 complex amplitudes. 
  TComplex *a_2;  // Need arrays of each amplitude for use in Explore()
  TComplex *a_3;  // method.  I.e. can alter the complex amplitudes to determine
  TComplex *a_4;  // new (slightly changed) value of B.
  
  float     *B;          // array[nSamples] holding prior values of B

  
  // LogLhood() Variables:
  ifstream  eventgen;   // File containing azimuthal angles for each event
  ifstream  newprior;   // File to read in new prior information.
  double   *angles;     // Extracted angles from file, will be array[nEvents]
  double   *pol;        // Array of polarisation states (-1 or 1) also extracted
  double    P_gamma;     // Initially will be single value, later will be array.
                        // P_gamma is the photon polarisation number

  double    delta_L;     // Luminosity asymmetry (essentially). Initially will be 
                        // kept as 0 but will eventually be an array?

  int       nEvents;    // Number of events.
  
  FILE      *events;
  bool       NewPrior;   // Set to true if using updated prior frrom posterior

  bool       testPrior;

  //Explore() Variables:
  // I don't know what will be required here yet.


  // Variables required for Posterior:

  double   *post_a1_Re;
  double   *post_a1_Im;
  double   *post_a2_Re;
  double   *post_a2_Im;
  double   *post_a3_Re;
  double   *post_a3_Im;
  double   *post_a4_Re;
  double   *post_a4_Im;

  double   **postX;

  float   *post_B;

  ofstream  fPosterior;    //saves posterior results (fSamples)
  ofstream  fNewPrior;     //To use results as new Prior function.

        
  // Private stuff
private:
  // Fields
  int       noObjs;  //Number of objects.

  int       fIndex;
  int       sampleIndex;
 

};
#endif

//      End-of-file
// ====================================================================
