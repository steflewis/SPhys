//____________________________________________________________________ 
//  
// $Id$ 
// Author: Dave Ireland <d.ireland@physics.gla.ac.uk>
// Update: 2009-07-23 10:15:36+0100
// Copyright: 2009 (C) Dave Ireland
//
//
#ifndef __CINT__
#ifndef ROOT_TApplication
#include "TApplication.h"
#endif
// PUT HEADERS HERE
#endif

int DoAll(Int_t nev, string fileName, Int_t opt=0 )
{
    //	Generate G4 output, analyse and extract...
    gROOT->ProcessLine(".L KLambdaGenP_v2.C+");
    KLambdaGenP(0,0,123456,nev,fileName);
    KLambdaGenP(1,0,234567,nev,fileName);
    gROOT->ProcessLine(".L AnaPol_v2.C+");
    AnaPol(0);
    AnaPol(1);
    gROOT->ProcessLine(".L ExtractObservables.C");
    ExtractObservables();
    

    //	Generate DGI output...
    if (opt == 1) {
	KLambdaGenP(0,1,123456,nev,fileName);
	KLambdaGenP(1,1,234567,nev,fileName);
    }
    
    return 0;
}

#ifndef __CINT__
int main(int argc, char** argv)
{
  TApplication DoAllApp("DoAllApp", &argc, argv);
  int retVal = DoAll();
  DoAllApp.Run();
  return retVal;
}
#endif

//____________________________________________________________________ 
//  
// EOF
//
