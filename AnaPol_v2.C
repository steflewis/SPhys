#include "TFile.h"
#include "TChain.h"
#include "TRandom3.h"
#include "TDatabasePDG.h"
#include "TPDGCode.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TF1.h"
#include "TBenchmark.h"
#include "TSystem.h"
#include "TMath.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TStyle.h"

#include <iostream>


void AnaPol(Int_t pol)
{
  gStyle->SetPalette(1);

  Float_t fPx_Gam, fPy_Gam, fPz_Gam, fEn_Gam;
  Float_t fPx_Kp,  fPy_Kp,  fPz_Kp,  fEn_Kp;
  Float_t fPx_P,   fPy_P,   fPz_P,   fEn_P;
  Float_t fPx_Pim, fPy_Pim, fPz_Pim, fEn_Pim;

  TChain* fROOTChain = new TChain("h1");
  if( pol == 0 )
    fROOTChain->Add("Gen_KLambdaPARA_G4.root");
  else
    fROOTChain->Add("Gen_KLambdaPERP_G4.root");
  Int_t fNEvents = fROOTChain->GetEntries();
  cout << "Total number of events to be analysed = " << fNEvents << endl;

  Int_t fNBranches    = fROOTChain->GetNbranches();
  TObjArray* objarray = fROOTChain->GetListOfBranches();

  for( Int_t j = 0; j < fNBranches; j++ ) {
    TBranch *branch = dynamic_cast<TBranch*>     (objarray->At(j));
    TString  bname  = TString( const_cast<char*> (branch->GetName()) );

    if( bname == "Px_Gam" )  branch->SetAddress( &fPx_Gam );
    if( bname == "Py_Gam" )  branch->SetAddress( &fPy_Gam );
    if( bname == "Pz_Gam" )  branch->SetAddress( &fPz_Gam );
    if( bname == "En_Gam" )  branch->SetAddress( &fEn_Gam );

    if( bname == "Px_Kp"  )  branch->SetAddress( &fPx_Kp  );
    if( bname == "Py_Kp"  )  branch->SetAddress( &fPy_Kp  );
    if( bname == "Pz_Kp"  )  branch->SetAddress( &fPz_Kp  );
    if( bname == "En_Kp"  )  branch->SetAddress( &fEn_Kp  );

    if( bname == "Px_P"   )  branch->SetAddress( &fPx_P );
    if( bname == "Py_P"   )  branch->SetAddress( &fPy_P );
    if( bname == "Pz_P"   )  branch->SetAddress( &fPz_P );
    if( bname == "En_P"   )  branch->SetAddress( &fEn_P );

    if( bname == "Px_Pim" )  branch->SetAddress( &fPx_Pim );
    if( bname == "Py_Pim" )  branch->SetAddress( &fPy_Pim );
    if( bname == "Pz_Pim" )  branch->SetAddress( &fPz_Pim );
    if( bname == "En_Pim" )  branch->SetAddress( &fEn_Pim );
  }

  const Float_t   ProtonM     = 0.938272;
  TLorentzVector* fTargetP4   = new TLorentzVector(0,0,0,ProtonM);
  TLorentzVector* fBeamP4     = new TLorentzVector(0,0,0,1);
  TLorentzVector* fKplusP4    = new TLorentzVector(0,0,0,1);
  TLorentzVector* fProtonP4   = new TLorentzVector(0,0,0,1);
  TLorentzVector* fPiminusP4  = new TLorentzVector(0,0,0,1);
  TLorentzVector* fReactionP4 = new TLorentzVector(0,0,0,1);
  TLorentzVector* fLambdaP4   = new TLorentzVector(0,0,0,1);

  TVector3 RBoostv3, LBoostv3;
                      
  TVector3 *x_axis               = new TVector3(0,0,0); 
  TVector3 *y_axis               = new TVector3(0,0,0); 
  TVector3 *y_axis_tmp           = new TVector3(0,0,0); 
  TVector3 *z_axis               = new TVector3(0,0,0); 

  Float_t csx, csy, csz;
  TFile* fOutFile;
  TH2F *hx, *hy, *hz;
  TH1F* hcosy;

  if( pol == 0) {
    fOutFile = new TFile("outpara.root", "RECREATE");
    hx    = new TH2F("hxpa",   "hxpa",   60, -TMath::Pi(), TMath::Pi(), 20, -1.0, 1.0);
    hy    = new TH2F("hypa",   "hypa",   60, -TMath::Pi(), TMath::Pi(), 20, -1.0, 1.0);
    hz    = new TH2F("hzpa",   "hzpa",   60, -TMath::Pi(), TMath::Pi(), 20, -1.0, 1.0);
    hcosy = new TH1F("hcosya", "hcosya", 60, -1, 1);
  }
  else {
    fOutFile = new TFile("outperp.root", "RECREATE");
    hx    = new TH2F("hxpe",   "hxpe",   60, -TMath::Pi(), TMath::Pi(), 20, -1.0, 1.0);
    hy    = new TH2F("hype",   "hype",   60, -TMath::Pi(), TMath::Pi(), 20, -1.0, 1.0);
    hz    = new TH2F("hzpe",   "hzpe",   60, -TMath::Pi(), TMath::Pi(), 20, -1.0, 1.0);
    hcosy = new TH1F("hcosye", "hcosye", 60, -1, 1);
  }
    
  for( Long64_t i = 0; i < fNEvents; i++ ) 
    {
      fROOTChain->GetEvent(i);
      
      fBeamP4->SetXYZT(    fPx_Gam, fPy_Gam, fPz_Gam, fEn_Gam );
      fKplusP4->SetXYZT(   fPx_Kp,  fPy_Kp,  fPz_Kp,  fEn_Kp  );
      fProtonP4->SetXYZT(  fPx_P,   fPy_P,   fPz_P,   fEn_P   );
      fPiminusP4->SetXYZT( fPx_Pim, fPy_Pim, fPz_Pim, fEn_Pim );

      // Get Photon and Kplus in gamma + p CM
      *fReactionP4 = *fTargetP4 + *fBeamP4;
      RBoostv3     = fReactionP4->BoostVector();
      fBeamP4->Boost(-RBoostv3);
      fKplusP4->Boost(-RBoostv3);

      Float_t fKaonPhiCM = fKplusP4->Phi();
      fKplusP4->RotateZ(-fKaonPhiCM);
      fProtonP4->RotateZ(-fKaonPhiCM);
      fPiminusP4->RotateZ(-fKaonPhiCM);
      *fLambdaP4 = *fProtonP4 + *fPiminusP4;
      //      fLambdaP4->Boost(-RBoostv3);

      // Get proton in Lambda Rest
      LBoostv3   = fLambdaP4->BoostVector();
      fProtonP4->Boost(-LBoostv3);

      *z_axis     = fBeamP4->Vect().Unit();
      *y_axis_tmp = z_axis->Cross(fKplusP4->Vect());
      *y_axis     = y_axis_tmp->Unit();
      *x_axis     = y_axis->Cross(*z_axis);

      csx = TMath::Cos(fProtonP4->Vect().Angle(*x_axis));
      csy = TMath::Cos(fProtonP4->Vect().Angle(*y_axis));
      csz = TMath::Cos(fProtonP4->Vect().Angle(*z_axis));

      hx->Fill(fKaonPhiCM, csx);
      hy->Fill(fKaonPhiCM, csy);
      hz->Fill(fKaonPhiCM, csz);
      hcosy->Fill(csy);

      if( i % 1000000 == 0) cout << i/1000000 <<" M" << endl;
    }      
  
  hx->Write();
  hy->Write();
  hz->Write();
  hcosy->Write();

  fOutFile->Close();
}
