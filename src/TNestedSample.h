// -*- mode: c++ -*- 
//
// ==================================================================== 
//  
//      Project: sconsNS4
//               ~~~~~~
//  
//      File: TNestedSample.h
//            ~~~~~~~~~~~~~~~~~~
//
//      Description: General class for running Nested Sampling applications
//  
//      Author: Stefanie Lewis <s.lewis@physics.gla.ac.uk> 
//              based on class diagram by Dave Ireland
//
//      
//
//      Copyright: 2010 (C) Stefanie Lewis
//
//      $Id$ 
//
// ==================================================================== 
#ifndef TNESTEDSAMPLE_H
#define TNESTEDSAMPLE_H

#define MAX_SAMPLES 1200000
using namespace std;
// #include "Riostream.h"
// #include "TROOT.h"
// #include "TMath.h"
// #include "TRandom.h"
// #include "Rtypes.h"
// #include <TFile.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <float.h>
#include <complex.h>

#define PLUS(x,y) (x>y ? x+log(1+exp(y-x)) : y+log(1+exp(x-y)))
#define logZERO   (-DBL_MAX*DBL_EPSILON) //log(0) - smallest value possible.
#define UNIFORM ((rand()+0.5)/(RAND_MAX+1.0)) //uniform inside (0,1)

const int ITERATES = 50000;//40000; 
const int DEBUG    = 0;//0;   //Set to 0 to turn off DEBUG mode.

enum EBoolean {kFalse, kTrue};

//Class TNestedSample
class TNestedSample {
 public: 
  // Constructors
  TNestedSample();

  TNestedSample(int numberOfSamples, double logWidth, char* name);

  // Operations
  virtual void PrintSummary(char*); //Prints results

  //void Initialize();

  //void Banner();

  virtual void Iterate(); //This contains the Eval function. It determines whether
                  //the nested sampling calculation should terminate or not
                  //and selects the next starting point.
                  //Also counts number of iterations.

  virtual void FinalCorrection(); //Finds the difference between 'guess' value of
                          //evidence and calculated value.


  //These methods defined in derived classes. Here they are pure virtual.
  //Compiler complains.  What happens if not pure virtual?
  virtual void Prior(TNestedSample*,int);

  virtual float LogLhood(float,float);

  virtual void Introduce();

  virtual void Explore(double, int);

  virtual void SetToCopy(int,int);

  virtual void StorePost(int,int,double,double);


  //Private
 protected:

  int               fNSamples;
  double            *fLogL;
  double            *fLogWt;
  double            *fUsedLogL;
  double            *fUsedLogWt;
  


  double            fLogW;
  double            fLogWFC;       //Width used in FinalCorrection()
  double            fLogLstar;
  double            fH;
  double            fHFC;          //H to be used in FinalCorrection()
  double            fLogZ;
  double            fLogZFC;       //LogZ used in FinalCorrection()
  double            fLogZnew;
  double            fLogZnewFC;    //LogZnew used in FinalCorrection()
  int               fNest;
  int               fWorst;
  int               fNoIterates;
  int               fIncrement;

  char              fPost[60];     //Filename for posterior file
  double            fBig;          //used in termination condition for 
                                   //'significantly exceeds' 
                                   
  char              *fDataFile;
 
 private:
  virtual void Eval(int, int);     //This is the main nested sampling loop.  
                           //It is contained within the Iterate function.
};

#endif
