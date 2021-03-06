//================================================================//
//                Plot Generating Analysis Macro                  //
//                                                                //
//       A ROOT Macro to be used to analyse the output of the     //
//       Nested Sampling program.                                 //
//                                                                //
//       Purposes:                                                //
//           - Read in .root file                                 //
//           - Create histograms                                  //
//           - Save histograms to .png files                      //
//                                                                //
//      This code has been further developed under the filename   //
//      ComparePriorPost.C, which is a standalone program.        //
//                                                                //
//      Date: 05/2011           Author: Stefanie Lewis            //
//      Copyright 2011 - s.lewis@physics.gla.ac.uk                //
//                                                                //
//================================================================//




{

  //Reset ROOT and connect tree file
   gROOT->Reset();
   gStyle->SetPalette(1);
  gStyle->SetFrameFillStyle(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadBorderMode(0);

  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameFillStyle(0);
  gStyle->SetFillStyle(0);

  const char* filename = "/home/Stefanie/NestedSampling/BeamRecoil/SPhys_vCPU_test1.root";

  // change filename in lines below
   TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(filename);
   if (!f) {
      f = new TFile(filename);
   }
   // change "T" below to sivia
   TTree *T = (TTree*)gDirectory->Get("tree");

   //Declaration of leaves types

   Double_t     a1_Re;
   Double_t     a1_Im;
   Double_t     a2_Re;
   Double_t     a2_Im;
   Double_t     a3_Re;
   Double_t     a3_Im;
   Double_t     a4_Re;
   Double_t     a4_Im;
   Double_t     B_obs;
   Double_t     logL;
   Double_t     logWt;
   Double_t     P_gam ;

   // Set branch addresses.
   T->SetBranchAddress("a1_Re",  &a1_Re);
   T->SetBranchAddress("a1_Im",  &a1_Im);
   T->SetBranchAddress("a2_Re",  &a2_Re);
   T->SetBranchAddress("a2_Im",  &a2_Im);
   T->SetBranchAddress("a3_Re",  &a3_Re);
   T->SetBranchAddress("a3_Im",  &a3_Im);
   T->SetBranchAddress("a4_Re",  &a4_Re);
   T->SetBranchAddress("a4_Im",  &a4_Im);
   T->SetBranchAddress("B_obs",  &B_obs);
   T->SetBranchAddress("logL",   &logL );
   T->SetBranchAddress("logWt",  &logWt);
   T->SetBranchAddress("P_gam",  &P_gam);

   Long64_t         nentries = T->GetEntries();
 
   Double_t         maxLogWt = -100000;
 
   TH2F            *a1  = new TH2F("a1",  "", 200, -1, 1,  200, -1, 1);
   TH1F            *h1  = new TH1F("h1",  "", 200, -1, 1             );
   TH2F            *PB  = new TH2F("PB",  "", 200, -1, 1,  200, 0, 1);
  
   // Histogram of P*B:
   TH1F            *PB_prod = new TH1F("PB_prod", "", 200, -1, 1);

   // TH1F            *h2  = new TH1F("h2",  "", 100, 0, 2            );

 
   // Main event loop
   for (Long64_t i=0; i<nentries;i++) 
     {
       T->GetEntry(i);
       if ((logL +logWt) > maxLogWt)
	   //       cout << LogWt[j] << endl;
	 maxLogWt = (logL + logWt);
     }
   
     
       
   // end of main event loop
   cout << "Max LogWt: " << maxLogWt << endl;
	 
   for (i = 0; i<nentries; i++){
       T->GetEntry(i); 

       h1->Fill(B_obs, TMath::Exp(logL + logWt + TMath::Abs(maxLogWt))        );
              // h0->Fill(posX, posY,  TMath::Exp(LogWt + TMath::Abs(maxLogWt)) );
              // h0->Fill(posX,posY, ( TMath::Exp(LogL) )*( TMath::Exp(LogWt + TMath::Abs(maxLogWt) ) ) );
      a1->Fill(a1_Re,a1_Im, TMath::Exp(logL + logWt + TMath::Abs(maxLogWt)));
       // h2->Fill(posY, TMath::Exp(LogL + LogWt + TMath::Abs(maxLogWt))        );
      PB->Fill(B_obs,P_gam, TMath::Exp(logL + logWt + TMath::Abs(maxLogWt)));

      PB_prod->Fill((B_obs*P_gam), TMath::Exp(logL + logWt + TMath::Abs(maxLogWt)) );

   }

   TCanvas *c1   = new TCanvas("c1", "Simple Beam Recoil, TNestedSample", 1200, 900);
   c1->SetFillStyle(0);
   c1->SetFillColor(0);
   c1->SetFrameFillStyle(0);
   h1->GetXaxis()->SetTitle("B");
   c1->cd(1);
   h1->Draw("");
     // c1->SaveAs("~/public_html/nestedsampling/lighthouseXweighted.png");
    c1->SaveAs("BNewPriorTestM-20000.png");

      TCanvas *c2   = new TCanvas("c2", "P_gamma and B, TNestedSample", 1200, 900);
   c2->SetFillStyle(0);
   c2->SetFillColor(0);
   c2->SetFrameFillStyle(0);
   c2->cd(1);
   PB->GetXaxis()->SetTitle("B ('sigma')");
   PB->GetYaxis()->SetTitle("Pgamma");
   PB->Draw("COLZ");
  
     // c2->SaveAs("~/public_html/nestedsampling/lighthouseYweighted.png");
    c2->SaveAs("Pgamma_v_B.png");

   TCanvas *c3   = new TCanvas("c3", "a1, TNestedSample", 1200, 900);
   c3->SetFillStyle(0);
   c3->SetFillColor(0);
   c3->SetFrameFillStyle(0);
   a1->GetXaxis()->SetTitle("Re");
   a1->GetYaxis()->SetTitle("Im");
   c3->cd(1);
   a1->Draw("COLZ");
     // c3->SaveAs("~/public_html/nestedsampling/lighthouseTNSplotweighted.png");
    c3->SaveAs("amp1NewPriorTestM-20000.png");

    TCanvas *c4  = new TCanvas("c4", "P_gamma * B, TNestedSample", 1200, 900);
    c4->SetFillStyle(0);
    c4->SetFillColor(0);
    c4->SetFrameFillStyle(0);
    PB_prod->GetXaxis()->SetTitle("Pgamma*B");
    c4->cd(1);
    PB_prod->Draw("");
    c4->SaveAs("PB_product_pdf.png");
    
}
