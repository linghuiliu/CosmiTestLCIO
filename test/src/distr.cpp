void distr(int evenum, int ihod, int axis) {
  TString fileName;
  fileName.Form("../result/test.root");
  TFile *input = new TFile(fileName.Data(), "read");
  TTree *tree = (TTree*)input->Get("bigtree");
  double nph[64];
  TH1F *h1 = new TH1F("h1","h1",16,0,16);
  TH1F *h2 = new TH1F("h1","h1",16,0,16);
  h2->SetLineColor(2);

  TString nphname;
  nphname.Form("hod%d_nph",ihod+1);
  tree->SetBranchAddress(nphname,nph);

  tree->GetEntry(evenum);
  for (int i=0;i<16;i++) {
    int ch[2][2];
    ch[0][0]=32+(i+12)%16;
    ch[0][1]=15-(i+12)%16;
    ch[1][0]=i;
    ch[1][1]=47-i;
    h1->Fill(i,nph[ch[ihod][axis]]);
    ch[0][0]=63-(i+12)%16;
    ch[0][1]=16+(i+12)%16;
    ch[1][0]=16+(i+12)%16;
    ch[1][1]=48+i;
    h2->Fill(i,nph[ch[ihod][axis]]);
  }
  h1->Draw();
  h2->Draw("same");
}
