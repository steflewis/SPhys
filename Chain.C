{
  TChain  *fROOTChain;


  fROOTChain = new TChain("tree");
  fROOTChain->Add("BposteriorTest.root");

  const ULong64_t NEvents = fROOTChain->GetEntries();
  cout << NEvents << endl;
  Double_t fLogWt;
  fROOTChain->SetBranchAddress("logWt", &fLogWt);

  for(ULong64_t i = 0; i < 20; i++) {
    cout << fLogWt << endl;
  }
}
