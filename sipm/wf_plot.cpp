// --- Waveform representation ---

const int n=1024;

void wf(
	TString outname, //root file name
	TString filename //file name for the data
       ){

  //--- Read input file
  ifstream file;
  file.open(filename);
  
  //---- File ROOT
  TFile *f = new TFile(outname, "recreate");
  
  //--- TTree
  double v;
  TTree* ch = new TTree("ch", "wf");
  ch->Branch("v", &v, "v/D");
  while(file.good()){
    file>>v;
    ch->Fill();
  }
  file.close();
  //int n = ch -> GetEntries();

  //--- Axis (ticks)
  double x[n], y[n];
  for (int i=0; i<n; i++){
    x[i]=i;
    ch->GetEntry(i);
    y[i]=v;
  }

  //--- Plot
  TGraph *gr = new TGraph(n, x, y);
  gr->SetTitle("Waveform - single event;Time [ticks];Amplitude [ADCs]");
  gr->SetLineColor(kViolet+1);
  gr->SetLineWidth(3);
  gr->SetMarkerStyle(8);
  //gr->SetMarkerSize(0.8);
  gr->SetMarkerColor(kViolet);

  TCanvas *c = new TCanvas();
  gPad->SetTicks(1,1);
  c->SetGrid();
  gr->Draw("AL");

  f->Write();
 
  return;
}
