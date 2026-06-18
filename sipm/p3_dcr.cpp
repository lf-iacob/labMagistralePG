#include <iostream>
using namespace std;
const int L=1024*8;

void dcr(TString file1, //.root (alberi)
	 double th, TString output="output.root"
	 ){

  cout<<endl<<"------> Dark Count Rate Analysis "<<endl<<endl;

  int first, second, third;
  sscanf(file1.Data(), "alberi/alb_dcr_%d_%d_%d", &first, &second, &third);
  double vbias = first + second / 100.0;
  cout<<"--> VBias (V): "<<vbias<<endl<<endl;

  if(output == "output.root"){
    output = file1;
    output.ReplaceAll("alberi/alb_dcr", "DCR");
  }

  /* --------- VBias (intensity) [V] -> Threshold [ADC]
     54.51 (250) -> 30
     55.11 (40) -> 48
     55.81 (70) -> 60
     56.31 (200, 230, 280) -> 65
     57.01 (a sentimento) -> 75
  */        
  cout<<"Threshold: "<<th<<endl<<endl;

  TFile *file1_aperto = TFile::Open(file1);
  TTree *dd = (TTree*)file1_aperto->Get("dd");
  int a[L];
  double b,c,amp;
  dd->SetBranchAddress("A", a);
  dd->SetBranchAddress("B", &b);
  dd->SetBranchAddress("C", &c);
  int n = dd->GetEntries();

  
  TFile *f = new TFile(output, "RECREATE");
  TTree *outTree = new TTree("dcr", "DCR analysis");
  int dce;
  outTree->Branch("dce", &dce, "dce/I");

  for(int i=0; i<n; i++){
    dd->GetEntry(i);
    dce = 0;
    for(int j=0; j<L; j++){
      if((b-a[j]) > th){
	dce += 1;
	j+=100; //jump
      }
    }
    outTree->Fill();
  }

  TH1I *h_dce = new TH1I("h_dce", "DC Events Hist;DC Events;Events", 13, -1, 12); //-1.5, 10.5
  for(int i=0; i<n; i++) {
    outTree->GetEntry(i);
    h_dce->Fill(dce);
  }


  TF1 *pp = new TF1("pp","[1]*TMath::PoissonI(x,[0])", 0, 12);
  //TF1 *pp = new TF1("pp","[1]*([0]^int(x)/TMath::Factorial(int(x)))*exp([0])",-0.5, 10.5);
  gStyle->SetOptFit(0111);
  pp->SetNpx(10000);
  pp->SetParameters(2,1000);
  double dceN, dceN_err, t, dcr, dcr_err;
  h_dce -> Fit(pp); 
  dceN=pp->GetParameter(0);
  dceN_err=pp->GetParError(0);
  t=L*4*1e-9; //s
  dcr=dceN/t;
  dcr_err=dceN_err/t;
  cout<<endl<<"VBias: "<<vbias<<" (t = "<<t<<" s) -> DC Events: "<<dceN<<" +- "<<dceN_err<<endl;
  cout<<"    -----> DCR (Hz) = "<<dcr<<" +- "<<dcr_err<<endl<<endl;


  double dcr_array[5]={47463.5, 60144.4, 75709.1, 89238.5, 103455},
    dcrerr_array[5]={367.361, 391.314, 445.049, 480.41, 506.939},
    vbias_array[5]={54.51, 55.11, 55.81, 56.31, 57.01},
    vbiaserr_array[5]={0.01/sqrt(12), 0.01/sqrt(12), 0.01/sqrt(12), 0.01/sqrt(12), 0.01/sqrt(12)};

  for(int i=0; i<5; i++)
    cout<<"VBias (V): "<<vbias_array[i]<<" => DCR = "<<dcr_array[i]<<" +- "<<dcrerr_array[i]<<endl;

  TCanvas *c1 = new TCanvas();
  c1->SetGrid();
  TGraphErrors *dc_lin= new TGraphErrors(5, vbias_array, dcr_array, vbiaserr_array, dcrerr_array);
  dc_lin->SetTitle("DCR VS Vbias;Vbias (V);DCR (Hz)");
  dc_lin->Draw("AP");

  TF1 *fit_lin = new TF1("fit_lin", "[0]+[1]*x", 54.4, 57.2);
  dc_lin->Fit(fit_lin, "R");
  TFitResultPtr rg = dc_lin->Fit(fit_lin, "RS");
  double A = fit_lin->GetParameter(0);
  double eA = fit_lin->GetParError(0);
  double B = fit_lin->GetParameter(1);
  double eB = fit_lin->GetParError(1);
  double covAB = rg->CovMatrix(0,1);
  double chi2 = fit_lin->GetChisquare();
  double gdl = fit_lin->GetNDF();

  TGraph *g_fitg = new TGraph(5, vbias_array, dcr_array);
  fit_lin->SetLineColor(kAzure-5);
  fit_lin->SetLineWidth(3);
  fit_lin->Draw("SAME");

  TLegend *leg = new TLegend(0.15, 0.75, 0.5, 0.88);
  leg->AddEntry(fit_lin, Form("Slope: %.0fe+02 #pm %.0e", B/100, eB), "l");
  leg->AddEntry(fit_lin, Form("Intercept: %.0fe+04 #pm %.0e", A/10000, eA), "");
  leg->AddEntry(fit_lin, Form("#chi^{2}/NDf = %.2f/%.0f", chi2, gdl), "");
  leg -> Draw();
  c1->Update();
  
  cout<<endl;
  
  f->Write();

}
