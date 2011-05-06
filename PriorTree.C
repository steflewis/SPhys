
TFile *PriorTree(){
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
 
  //*************************//
  //    Original Prior       //
  //*************************//

  Double_t   B_orig;
  Double_t   LogL_orig;

  TString filename = "priortree.root";
  FILE  *OrigPrior = fopen("oldprior.txt","r");

  TFile *hfile = new TFile("priortree.root","recreate");
  TTree *tree  = new TTree("T", "Evolving Prior");
  tree->Branch("B_orig", &B_orig, "B_orig/D");
  tree->Branch("LogL_orig", &LogL_orig, "LogL_orig/D");

  tree->ReadFile("oldprior.txt", "B_orig/D:LogL_orig");
  tree->Write();

  fclose(OrigPrior);
  Long64_t         nentries = T->GetEntries();
  printf("Number of entries, Original prior: %d\n",nentries);
  TH1F            *Pr  = new TH1F("Prior",  "", 80, -1.3, 1.3           );

  for (Long64_t i = 0; i<nentries; i++){
    T->GetEntry(i); 

    Pr->Fill(B_orig);

  }

  //*************************//
  //       New Prior       //
  //*************************//

  // New Prior - 1

  Double_t   B_1;

  //  TString filename = "priortree.root";
  FILE  *NewPrior1 = fopen("newBvals-1.txt","r");

  TFile *hfile = new TFile("priortree.root","recreate");
  TTree *New1  = new TTree("NewT1", "Evolving Prior");
  tree->Branch("B_1", &B_1, "B_1/D");

  tree->ReadFile("newBvals-1.txt", "B_1/D");
  tree->Write();

  fclose(NewPrior1);


  //******PLOTS**************//

  
  TH1F            *Pr  = new TH1F("Prior",  "", 80, -1.3, 1.3           );
  TH1F            *PrNew1  = new TH1F("Prior New 1",  "", 80, -1.3, 1.3 );

  // Original Prior
  for (Long64_t i = 0; i<nentries; i++){
    T->GetEntry(i); 
    Pr->Fill(B_orig);
  }

  // New Prior 1
  for (Long64_t i = 0; i<nentries; i++){
    NewT1->GetEntry(i);
    PrNew1->Fill(B_1);
  }


  ///////////////////////////////



   TCanvas *Prior   = new TCanvas("Prior", "Prior, TSimplePhysics", 1200, 900);
   Prior->SetFillStyle(0);
   Prior->SetFillColor(0);
   Prior->SetFrameFillStyle(0);
   Pr->GetXaxis()->SetTitle("B");
   Prior->cd(1);
   Pr->SetFillColor(1);
   Pr->Draw("");
     // c1->SaveAs("~/public_html/nestedsampling/lighthouseXweighted.png");
   //Prior->SaveAs("firstPrior.png");
   PrNew1->SetFillColor(2);
   PrNew1->Draw("SAME");


  return 0;
}
