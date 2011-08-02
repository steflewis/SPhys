{
//////////////////////////////////////////////////////////
//   This file has been automatically generated 
//     (Wed Mar 23 15:07:09 2011 by ROOT version5.27/02)
//   from TTree h1/Generator Output Tree DGI
//   found on file: Gen_KLambdaPARA_DGI.root
//////////////////////////////////////////////////////////


//Reset ROOT and connect tree file
   gROOT->Reset();
   TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Gen_KLambdaPARA_DGI.root");
   if (!f) {
      f = new TFile("Gen_KLambdaPARA_DGI.root");
   }
   TTree *h1 = (TTree*)gDirectory->Get("h1");


   //Declaration of leaves types
   Float_t         Egam;
   Float_t         Pgam;
   TLorentzVector  *P4K = 0;
   TLorentzVector  *P4P = 0;

   // Set branch addresses.
   h1->SetBranchAddress("Egam",&Egam);
   h1->SetBranchAddress("Pgam",&Pgam);
   h1->SetBranchAddress("P4K",&P4K);
   h1->SetBranchAddress("P4P",&P4P);

   Long64_t nentries = h1->GetEntries();

   printf("Number of entries, para: %d\n",nentries);
   
   ofstream data;

   data.open("datatest.txt");
   

   Long64_t nbytes = 0;
   for (Long64_t i=0; i<nentries;i++) {
     nbytes += h1->GetEntry(i);
     
     data << P4K->Phi();
     data << " ";
     data << "1.0";
     data << "\n";
     
//      cout << P4K->Phi() << endl;
   }



   data.close();

   
   

   
}
