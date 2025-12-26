// --- Dark count rate (single VBias) ---

#include <iostream>
using namespace std;
int const M=4;

void dcr(
	 TString file1, TString file2="", TString file3="", TString file4="", //.root (alberi)
	 TString fl="LinearityVBias.root", //.root (Linearity)
	 TString output="DCR.root"
	 ){

  TFile *f = new TFile(output, "RECREATE");
  
  TString files[M] = {file1, file2, file3, file4};
  double vbias[M];
  int first, second;
  for(int i=0; i<M; i++){
    sscanf(files[i].Data(), "alberi/alb_%d_%d_", &first, &second);
    vbias[i] = first + second / 100.0;
  }

  cout<<endl<<"------> Dark Count Rate Analysis "<<endl<<endl;

  //---- SPE charge
  TFile* file_lin = TFile::Open(fl);
  double cspe[M], ecspe[M];
  TGraphErrors *g = (TGraphErrors*)file_lin->Get("Graph");
  for(int j=0; j<M; j++) {
    cspe[j] = g->GetY()[j]*1.6e-7*pow(10, 32./20.);    //check la conversione
    ecspe[j] = g->GetEY()[j]*1.6e-7*pow(10, 32./20.);
  }
  file_lin->Close();

  //---- Histograms -> #PE

  int nEv[M];
  double c, npe[M]={0., 0., 0., 0.};
  TTree *alb[M];
  for(int i=0; i<M; i++){
    if(files[i]=="") continue;
    TFile* file_dd = TFile::Open(files[i]);
    alb[i] = (TTree*)file_dd->Get("dd");
    alb[i]->SetBranchAddress("C", &c);
    nEv[i] = alb[i]->GetEntries();
    for(int k=0; k<nEv[i]; k++){
      alb[i]->GetEntry(k);
      npe[i]+=c/cspe[i];
    }
  }

  cout<<" Vbias  => Spe charge      , #pe"<<endl;
  for(int j=0; j<M; j++)
    cout<<Form("%.2f V => %.2f +- %.2f pC, %.2f", vbias[j], cspe[j], ecspe[j], npe[j])<<endl;
  cout<<endl;

  
  //---- Time (???????)
  double time[M];
  for(int i=0; i<M; i++) time[i] = nEv[i]*1024*4; //ns
  
  //---- DCR (VS VBias)
  double dcr[M], edcr[M], evbias[M];
  for(int i=0; i<M; i++){
    dcr[i] = npe[i]/time[i];
    edcr[i] = 0.00002; //messo a caso - da trattare ancora
    evbias[i] = 0.01/sqrt(12); //da discutere ancora e aggiustare in altri codici
  }

  cout<<"--> DCR vs VBias"<<endl;
  gStyle->SetOptFit(111);

  TCanvas *ca = new TCanvas();
  TPad *pad1 = new TPad("pad1","pad1",0.0,0.33,1.0,1.0);
  pad1->SetBottomMargin(0.04);
  pad1->SetGrid(1,1);
  pad1->Draw();
  TPad *pad2 = new TPad("pad2","pad2",0.0,0.0,1.0,0.30);
  pad2->SetTopMargin(0.02);
  pad2->SetBottomMargin(0.3);
  pad2->SetGrid(1,1);
  pad2->SetTicks(1,1);
  pad2->Draw();
  
  pad1->cd();
  gPad->SetGrid(1,1);
  TGraphErrors *gr = new TGraphErrors(4, vbias, dcr, evbias, edcr);
  gr->SetTitle("DCR VS Vbias;Vbias (V);DCR (#pe/ns)");
  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(0.5);
  gr->SetMarkerColor(kTeal+3);
  gr->Draw("AP");

  TF1 *fitg = new TF1("fitg", "[0]+[1]*x", 54.4, 56.4);
  gr->Fit(fitg, "R");
  TFitResultPtr rg = gr->Fit(fitg, "RS");
  double C = fitg->GetParameter(0);
  double eC = fitg->GetParError(0);
  double D = fitg->GetParameter(1);
  double eD = fitg->GetParError(1);
  double chi2 = fitg->GetChisquare();
  int ndf = fitg->GetNDF();
  double covCD = rg->CovMatrix(0,1);
  
  TGraph *g_fitg = new TGraph(4, vbias, dcr);
  fitg->SetLineColor(kTeal-5);
  fitg->SetLineWidth(2);
  fitg->Draw("SAME");

  TLegend *leg = new TLegend(0.13, 0.6, 0.5, 0.88);
  leg -> AddEntry(gr, "Data", "p");
  leg -> AddEntry(fitg, Form("Fit (#chi^{2}/ndf=%.2f/%d)", chi2, ndf), "l");
  leg -> AddEntry(fitg, Form("=> Intercept = %.6f +- %.6f", C, eC), "");
  leg -> AddEntry(fitg, Form("=> Slope (Gain) = %.6f +- %.6f", D, eD), "");
  leg -> Draw();
  ca->Update();
  
  double res[4], eres[4];
  for(int i=0; i<4; i++){
    res[i]  = dcr[i]-fitg->Eval(vbias[i]);
    eres[i] = edcr[i];
  }

  TGraphErrors *gres = new TGraphErrors(4, vbias, res, evbias, eres);
  gres->SetMarkerStyle(20);
  gres->SetMarkerSize(0.5);
  gres->SetMarkerColor(kTeal+3);
  gres->SetTitle(";VBias;Residuals");
  
  pad2->cd();
  gPad->SetGrid(1,1);
  gPad->SetTicks(1,1);
  gres->Draw("AP");

  // residual fit: region
  const int NB = 700;
  double xb[NB], yb[NB], eyb[NB];

  for(int i=0; i<NB; i++){
    xb[i] = 54.4 + 2 * i / (NB-1);
    yb[i] = 0;
    eyb[i] = sqrt(eC*eC + xb[i]*xb[i]*eD*eD + 2*xb[i]*covCD);;
  }

  TGraphErrors *bandv = new TGraphErrors(NB, xb, yb, 0, eyb);
  bandv->SetFillColorAlpha(kTeal+2, 0.35);
  bandv->SetLineColor(kCyan+3);
  bandv->SetFillStyle(1001);

  double minY = 1e9;
  double maxY = -1e9;
  for(int i=0; i<4; i++){
    double rmin = res[i] - eres[i];
    double rmax = res[i] + eres[i];
    if(rmin < minY) minY = rmin;
    if(rmax > maxY) maxY = rmax;
  }
  double margin = 0.1*(maxY - minY);
  bandv->GetYaxis()->SetRangeUser(minY - margin, maxY + margin);

  bandv->Draw("3");
  gres->Draw("P SAME");

  TLine *zerov = new TLine(54.4,0,56.4,0);
  zerov->SetLineColor(kCyan+3);
  zerov->SetLineWidth(1);
  zerov->Draw("SAME");

  cout<<endl;

  f->cd();
  ca->Write();
  f->Write();
  //f->Close();

}
