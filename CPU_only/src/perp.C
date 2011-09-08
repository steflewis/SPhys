{
//////////////////////////////////////////////////////////
//   This file has been automatically generated 
//     (Wed Mar 23 16:17:12 2011 by ROOT version5.27/02)
//   from TTree h1/Generator Output Tree DGI
//   found on file: Gen_KLambdaPERP_DGI.root
//////////////////////////////////////////////////////////


//Reset ROOT and connect tree file
   gROOT->Reset();
   TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Gen_KLambdaPERP_DGI.root");
   if (!f) {
      f = new TFile("Gen_KLambdaPERP_DGI.root");
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

//     This is the loop skeleton
//       To read only selected branches, Insert statements like:
// h1->SetBranchStatus("*",0);  // disable all branches
// TTreePlayer->SetBranchStatus("branchname",1);  // activate branchname

   Long64_t nentries = h1->GetEntries();
   printf("Number of entries, perp: %d\n",nentries);
   ofstream data;
   
   data.open("datatest.txt", ios::app);

   Long64_t nbytes = 0;
   for (Long64_t i=0; i<nentries;i++) {
     nbytes += h1->GetEntry(i);
     data << P4K->Phi();
     data << " ";
     data << "-1.0";
     data << "\n";
 
   }

   data.close();
}
