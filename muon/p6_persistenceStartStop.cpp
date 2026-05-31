// Persistence oscilloscope start - stop

using namespace std;
const int ARRAY_SIZE = 8*1024;  
const int N = 100;

void persistence(TString file, TString output="persistence_start_stop.root") {

  TFile *file_aperto = TFile::Open(file);
  TTree *dd = (TTree*)file_aperto->Get("dd");
  int a1[1024*8], a2[1024*8], t1, t2, t, s=1200;
  double b1, b2;
  dd->SetBranchAddress("A1", a1);
  dd->SetBranchAddress("B1", &b1);
  dd->SetBranchAddress("A2", a2);
  dd->SetBranchAddress("B2", &b2);
  //int n = dd->GetEntries();
  int n = 10
    ;
  
  TFile *f = new TFile(output, "RECREATE");

  // ---- Persistence
  TCanvas *c1 = new TCanvas();
  c1->SetGrid();
  dd->Draw("a1:Iteration$", "Entry$==0", "AL");
  TGraph *g1 = (TGraph*)gPad->GetListOfPrimitives()->Last();
  g1->SetLineColor(kPink-8);
  g1->SetTitle("Oscilloscopio - persistence plot;Ticks;ADC");
  dd->Draw("a2:Iteration$", "Entry$==0", "L SAME");
  for(int i = 1; i < n; i++){
    dd->Draw("a2:Iteration$", Form("Entry$==%d", i), "L SAME");
  }

  file_aperto->Close();
  
  c1->Write();
  f->Write();
}
