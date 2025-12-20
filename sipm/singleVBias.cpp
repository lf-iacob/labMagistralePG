// --- Analysis of single VBias (3 events at a time) ---

#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <TGraph.h>
#include <TCanvas.h>
using namespace std;
const int N=100; //data for baseline

void analizza(
              double VBias,
	      TString file_name1, TString file_name2, TString file_name3,
	      TString output="output.root"
             ){
  cout<<endl<<"------> ANALYSIS VBias (V) = "<<VBias<<endl<<endl;
    
  //---- File ROOT
  if(output=="output.root"){
    TString name;
    name.Form("%.2f", VBias);
    name.ReplaceAll(".", "_");
    output = "VBias" + name + ".root";
  }
  TFile *f = new TFile(output, "RECREATE");

  //---- Create TTree
  int a[1024];
  double b, c;
  TTree* dd = new TTree("dd", "dd");
  dd->Branch("A", a, "a[1024]/I");
  dd->Branch("B", &b, "b/D");
  dd->Branch("C", &c, "c/D");

  //---- TTree->Fill()
  ifstream file;
  TString files[5] = {file_name1, file_name2, file_name3};
  for(int k=0; k<5; k++){
    file.open(files[k]);
    while(file.good()){
      b=0.; c=0.;
      for(int i=0;i<1024;i++){
	file>>a[i];
	if(i<N) b+=double(a[i])/N;
	else if(i>=450 && i<=550) c+=(b-a[i])*2000./4096*4/50;
      }
      dd->Fill();
    }
    file.close();
  }

  int n = dd->GetEntries(); //#events collected

  //---- Charge THistogram
  TH1F *hc = new TH1F("hc", "Charge Histogram;Charge (pC);Counts",500,-50,300);
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    hc->Fill(c);
  }

  TH1F *hcf = (TH1F*)hc->Clone("hc_f");
  
  TCanvas *hc_fit = new TCanvas();
  gPad->SetTicks(1,1);
  hcf->Draw();
  hcf->SetLineColor(kBlack);
  hcf->SetLineWidth(2);
  hcf->Sumw2();

  
  //---- Gaussian Fit
  cout<<"--- Gaussian fit ---"<<endl;
  double ch[7], ech[7], m[7], em[7], chirGauss[7], pv[7];
  double pe[7]={0,1,2,3,4,5,6}, epe[7]={0,0,0,0,0,0,0};

  double hfit_i=-20, hfit_f=140; //HARD_CODED
  vector<double> range_i = {-3.4, 12, 28, 44, 59, 75, 90}, //VBias==55.11
    range_f = { 3.4, 18, 34, 49, 66, 81, 96};

  if(VBias==54.51){ //Da aggiustare per fit migliori
    hfit_i= -20; hfit_f = 110;
    range_i = {-3,  8, 23.5, 34, 48, 60, 74};
    range_f = { 3, 17, 28  , 40, 54, 67, 80};
  }
  else if(VBias==56.31) { //DA AGGIUSTARE
    hfit_i= -20; hfit_f = 200;
    range_i = {-5, 18, 39, 65, 86, 108, 150};
    range_f = { 4, 25, 48, 70, 94, 114, 160};
  }
  
  TF1 *gfit[7];
  for(int j=0; j<7; j++){
    gfit[j] = new TF1("gfit", "gaus", hfit_i, hfit_f); //hard-coded
    hcf->Fit(gfit[j], "QS", " ", range_i[j], range_f[j]); //j_pe
    ch[j] = gfit[j]->GetParameter(1);
    ech[j] = gfit[j]->GetParError(1);
    m[j] = ch[j]/1.6e-7/pow(10, 32./20.); //gain
    em[j] = ech[j]/1.6e-7/pow(10, 32./20.); //error gain
    chirGauss[j] = gfit[j]->GetChisquare()/gfit[j]->GetNDF();
    pv[j] = TMath::Prob(gfit[j]->GetChisquare(),gfit[j]->GetNDF());
    cout<<j<<"pe => Charge (pC): "<<ch[j]<<"+-"<<ech[j]<<", Gain: "<<m[j]<<"+-"<<em[j]<<", X2r: "<<chirGauss[j]<<" -> p_value: "<<pv[j]<<" -- ["<<range_i[j]<<";"<<range_f[j]<<"] --"<<endl;
    //drawings
    gfit[j]->SetLineColor(kTeal + j);
    gfit[j]->SetLineWidth(3);
    gfit[j]->SetNpx(750); //numero di punti in cui sono valutate le gaussiane
    gfit[j]->Draw("same");
    hc_fit->Update();
  }
  hc_fit->Write();
  cout<<endl;
  

  //---- Plot (linear)
  cout<<"--- Linear fit ---"<<endl;
  TGraphErrors *gr = new TGraphErrors(7, pe, m, epe, em);
  gr->SetTitle(";pe;Gain");
  gr->SetMarkerStyle(8);
  gr->SetMarkerSize(0.1);
  gr->SetMarkerColor(kAzure-3);

  TCanvas *cgain = new TCanvas();
  cgain->Divide(1,2); //2 pads: one for residual plot

  cgain->cd(1);
  gStyle->SetOptFit(1111);
  gPad->SetPad(0,0.32,1,1);     
  gPad->SetBottomMargin(0.08);
  gPad->SetTopMargin(0.06);
  gPad->SetTicks(1,1);
  gPad->SetGrid(1,1);
  gr->Draw("AP");

  //---- Fit plot (linear)
  TF1 *l = new TF1("l", "[0]+[1]*x", -0.5, 6.5);
  l->SetLineColor(kPink-1);
  l->SetLineWidth(2);
  TFitResultPtr r = gr->Fit(l, "RS"); // Return fit result (important)
  double A = r->Parameter(0);
  double B = r->Parameter(1);
  double eA = r->ParError(0);
  double eB = r->ParError(1);
  cout<<endl<<"Coefficiente angolare (Gain) = "<<B<<"+-"<<eB<<endl;
  cout<<"Intercetta = "<<A<<"+-"<<eA<<endl<<endl;
  double covAB = r->CovMatrix(0, 1);

  double res[7], eres[7];
  for(int i=0; i<7; i++){
    res[i]  = m[i]-l->Eval(pe[i]);
    eres[i] = em[i];
  }

  TGraphErrors *gres = new TGraphErrors(7, pe, res, epe, eres);
  gres->SetMarkerStyle(8);
  gres->SetMarkerSize(0.5);
  gres->SetMarkerColor(kAzure-3);
  gres->SetTitle(";pe;Residuals");
  
  cgain->cd(2);
  gPad->SetPad(0,0,1,0.32); 
  gPad->SetTopMargin(0.08);
  gPad->SetBottomMargin(0.28);
  gPad->SetTicks(1,1);
  gres->Draw("AP");


  // residual fit: region
  const int NB = 700;
  double xb[NB], yb[NB], eyb[NB];

  for(int i=0; i<NB; i++){
    xb[i] = 6.5 * i / (NB-1);
    yb[i] = 0;
    double sigma = sqrt(eA*eA + xb[i]*xb[i]*eB*eB + 2*xb[i]*covAB);
    eyb[i] = sigma;
  }

  TGraphErrors *band = new TGraphErrors(NB, xb, yb, 0, eyb);
  band->SetFillColorAlpha(kPink-1, 0.35);
  band->SetLineColor(kPink-3);
  band->SetFillStyle(1001);

  cgain->cd(2);
  gPad->SetPad(0,0,1,0.32);
  gPad->SetTopMargin(0.08);
  gPad->SetBottomMargin(0.28);
  gPad->SetTicks(1,1);
  gPad->SetGrid(1,1);

  double minY = 1e9;
  double maxY = -1e9;
  for(int i=0; i<7; i++){
    double rmin = res[i] - eres[i];
    double rmax = res[i] + eres[i];
    if(rmin < minY) minY = rmin;
    if(rmax > maxY) maxY = rmax;
  }
  double margin = 0.1*(maxY - minY);
  band->GetYaxis()->SetRangeUser(minY - margin, maxY + margin);

  band->Draw("3");
  gres->Draw("P SAME");

  TLine *zero = new TLine(0,0,6.5,0);
  zero->SetLineColor(kPink-1);
  zero->SetLineWidth(1);
  zero->Draw("same");

  cgain->Write();
  
  //---- Saving in TFile
  f->Write();
  //f->Close();

}
