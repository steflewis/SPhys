// -*- mode: c++ -*- 
//
// ==================================================================== 
//  
//      Project: sconsNS4
//               ~~~~~~
//  
//      File: TLightHouseNestedSample.h
//            ~~~~~~~~~~~~~~~~~~
//
//      Description:     
// This is used to show how an application can be developed. It is
// based on the lighthouse problem in Sivia's book:- 
// 
//                -------------------------------------
//           y=2 |:::::::::::::::::::::::::::::::::::::| 
//               |::::::::::::::::::::::LIGHT::::::::::|
//          north|::::::::::::::::::::::HOUSE::::::::::|
//               |:::::::::::::::::::::::::::::::::::::|
//               |:::::::::::::::::::::::::::::::::::::|
//           y=0 |:::::::::::::::::::::::::::::::::::::| 
//  --*--------------*----*--------*-**--**--*-*-------------*--------
//              x=-2          coastline -->east      x=2
//  Problem:
//   Lighthouse at (x,y) emitted n flashes observed at D[.] on coast.
// 
//  
//      Author: Dave Ireland <d.ireland@physics.gla.ac.uk>, based on C
//      code by John Skilling
//
//      Update: 2007-08-03 15:36:46z
//
//      Copyright: 2007 (C) Dave Ireland
//
//      UPDATE: 11/2010 Stefanie Lewis
//              SCONSNS4 - 4th Attempt
//
//      $Id$ 
//
// ==================================================================== 
#ifndef sconsNS4_TLightHouseNestedSample
#define sconsNS4_TLightHouseNestedSample

// #include "Riostream.h"
// #include "TROOT.h"
// #include "TMath.h"
// #include "TRandom"
// #include "Rtypes.h"
// #include <TFile.h>
#include "TNestedSample.h"

#include <math.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

//Variable for value of pi:
const double pi = M_PI;


class TLightHouseNestedSample : public TNestedSample {


  // Public stuff
public:
  // Operations
  TLightHouseNestedSample();
  ~TLightHouseNestedSample() {};

  TLightHouseNestedSample(int numberOfObjects, double logWidth);
    
  // Set object according to uniform prior
  void     Prior (int fIndex);
  void     TestPrior (int fIndex, double, double);     

  // logLikelihood function
  double   LogLhood (double x, double y);
        
  // Evolve object within likelihood constraint
  void     Explore (double logLstar, int sampleIndex);
  void     SetToCopy(int worst, int copyIndex);

  void     PrintSummary(char*);
  void     TestValues();
  void     StorePost(int, int, double, double);

  double   *x;	// Easterly position between -2 and +2
  double   *y;	// Northerly position between 0 and 2
  double   *u;        // Uniform prior parameter for x
  double   *v;        // Uniform prior parameter for y

  double   *postX;
  double   *postY;
  ofstream  fPosterior;    //saves posterior results (fSamples)


        
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
