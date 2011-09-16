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
#ifndef TSIMPLEPHYSICS_H
#define TSIMPLEPHYSICS_H

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
//#include "OclWrapper.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>


//Variable for value of pi:
const double pi = M_PI;
const double weak = 0.642;       // weak decay constant
const double dilution = -0.256;  // sigma dilution
const double    Log2e = log2(TMath::E());
const double FWHM_SIG = 2*(TMath::Sqrt(2*log(2)) );


class TSimplePhysics : public TNestedSample {


  // Public stuff
public:
  
  /**************Operations***************/
  
  TSimplePhysics();
  ~TSimplePhysics() {};

  TSimplePhysics(int numberOfObjects, double logWidth, char* name);
    
  // Set object according to uniform prior
  virtual void     Prior (int fIndex);
 // virtual void     TestPrior (int fIndex, double, double); 
  virtual void     UpdatedPrior();

  // logLikelihood function
  virtual float    LogLhood (float B_loc, float R_loc, float T_loc, float Ox_loc, float Oz_loc, float Pg_loc);
  // Evolve object within likelihood constraint
  virtual void     Explore (double logLstar, int sampleIndex);
  virtual void     SetToCopy(int worst, int copyIndex);

  virtual void     PrintSummary(char*);
 // virtual void     TestValues();
  virtual void     StorePost(int, int, double, double);

  /***************Attributes*****************/
  
  // double   *x;	// Easterly position between -2 and +2
  // double   *y;	// Northerly position between 0 and 2
  // double   *u;        // Uniform prior parameter for x
  // double   *v;        // Uniform prior parameter for y

  // Variables required for Prior():
  double  **x;          // Hold random numbers - will be array[8] where each 
                         // are re-defined for each value of B (i.e. each
                         // sample).


  double    rSquared;   // distance squared
  

  TComplex *a_1;  // Complex variables - there are 4 complex amplitudes. 
  TComplex *a_2;  // Need arrays of each amplitude for use in Explore()
  TComplex *a_3;  // method.  I.e. can alter the complex amplitudes to determine
  TComplex *a_4;  // new (slightly changed) value of B.
  
  
  // Polarisation Observables:
  float   *B;          // beam asymmetry
  float   *R;          // recoil polarisation
  float   *T;          // target polarisation
  float   *Ox;
  float   *Oz;
  float   *P_gamma;    // Beam polarisation


  // LogLhood() Variables:
  ifstream  eventgen;   // File containing azimuthal angles for each event
  ifstream  newprior;   // File to read in new prior information.
  float   *angles;     // Extracted angles from file, will be array[nEvents]
  float   *pol;        // Array of polarisation states (-1 or 1) also extracted
  float   *csy;
  float   *csx;
  float   *csz;
  float    delta_L;    // Luminosity asymmetry (essentially). Initially will be 
                        // kept as 0 but will eventually be an array?
                        
  // These attributes are used when extending the LogLhood method to 
  // incorporate all linear beam recoil variables.
 
  

  int       nEvents;    // Number of events.
  
  FILE      *events;
  bool      NewPrior;   // Set to true if using updated prior frrom posterior

  bool      testPrior;
  int       test;
  bool      indep;

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

  double   *post_B;
  double   *post_Pgam;
  double   *post_R;
  double   *post_T;
  double   *post_Ox;
  double   *post_Oz;

  ofstream  fPosterior;    //saves posterior results (fSamples)
  ofstream  fNewPrior;     //To use results as new Prior function.

        
  // Private stuff
protected:
  // Fields
  int       noObjs;  //Number of objects.

  int       fIndex;
  int       sampleIndex;
 
private:
  int       GetEvents(const char* name);
  void      CreateArrays();
  void      ReadData(const char* name);
  float     Calculate_B(TComplex a1, TComplex a2, TComplex a3, TComplex a4);
  float     Calculate_T(TComplex a1, TComplex a2, TComplex a3, TComplex a4);
  float     Calculate_R(TComplex a1, TComplex a2, TComplex a3, TComplex a4);
  float     Calculate_Ox(TComplex a1, TComplex a2, TComplex a3, TComplex a4);
  float     Calculate_Oz(TComplex a1, TComplex a2, TComplex a3, TComplex a4);

  

};
#endif

//      End-of-file
// ====================================================================
