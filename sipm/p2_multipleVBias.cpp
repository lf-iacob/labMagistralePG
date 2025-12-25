// -- Multiple VBias Analysis --

#include <iostream>
using namespace std;
int const F=4; //#files

void totale(
	    TString file1, TString file2, TString file3, TString file4,
	    TString output="LinearityVBias.root"
	    ){

  TFile *f = new TFile(output, "RECREATE");
  
  int first, second; double vbias[F];
  TString files[F] = {file1, file2, file3, file4};
  int color[F] = {kRed, kGreen+2, kBlue, kMagenta+1};
  for (int i=0; i<F; i++){
    sscanf(files[i].Data(), "VBias_%d_%d_", &first, &second);
    vbias[i] = first + second / 100.0;
  }
  
  cout<<endl<<"------> Analysis Multiple VBias: Linearity"<<vbias<<endl;
  
  //---- Read files
  double A[F], eA[F], B[F], eB[F], chi[F];
  int ndf[F];
  TGraphErrors *gp[F], *gpr[F], *band[F];
  TF1 *l[F];
  for(int j=0; j<F; j++) {
    TFile* file = TFile::Open(files[j]);
    gp[j] = (TGraphErrors*)file->Get("gp");
    gpr[j] = (TGraphErrors*)file->Get("gpr");
    band[j] = (TGraphErrors*)file->Get("band");
    l[j] = (TF1*)file->Get("l");
    A[j] = l[j]->GetParameter(0);
    eA[j] = l[j]->GetParError(0);
    B[j] = l[j]->GetParameter(1);
    eB[j] = l[j]->GetParError(1);
    chi[j] = l[j]->GetChisquare();
    ndf[j] = l[j]->GetNDF();
  }

  cout<<endl<<"------ Gain VS PE ------"<<endl;
  //---- Gain VS pe
  TCanvas *r = new TCanvas();
  r->SetGrid();
  TLegend *leg = new TLegend(0.13, 0.6, 0.56, 0.88);
  for(int j=F-1; j>=0; j--) {
    gp[j]->SetMarkerStyle(20);
    gp[j]->SetMarkerSize(0.5);
    gp[j]->SetLineColor(color[j]);
    if(j==3) gp[j]->Draw("AP");
    else gp[j]->Draw("SAME P");
    leg->AddEntry(gp[j],
		  Form("%.2f V => A=%.0f+-%.0f, B=%.0f+-%.0f (#chi^{2}=%.0f/%d)",
		       vbias[j], A[j], eA[j], B[j], eB[j], chi[j], ndf[j]),
		  "lp");
  }
  leg -> Draw();
  r->Update();

  //---- Residuals
  TLine *lzero = new TLine(-0.5,0,5.5,0);
  lzero->SetLineColor(kBlack);
  lzero->SetLineStyle(2);
  TCanvas *rr = new TCanvas();
  rr->Divide(1,4);
  TLegend* legr[F];
  for(int j=0; j<F; j++) {
    rr->cd(j+1);
    gPad->SetGrid(1,1);
    gpr[j]->Draw("AP");
    lzero->Draw("SAME");
    band[j]->SetFillColorAlpha(color[j], 0.35);
    band[j]->Draw("3");
    legr[j] = new TLegend(0.13, 0.74, 0.25, 0.86);
    legr[j] -> AddEntry(gpr[j], Form("VBias: %.2f V", vbias[j]));
    legr[j] -> Draw();
  }

  
  cout<<endl<<"------ Gain VS VBias ------"<<endl<<endl;
  gStyle->SetOptFit(111);

  //---- Plot gainVSvbias
  TCanvas *c = new TCanvas();
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
  TGraphErrors *g = new TGraphErrors(4, vbias, B, 0, eB);
  g->SetTitle("Gain VS Vbias;Vbias (V);Gain");
  g->SetMarkerStyle(20);
  g->SetMarkerSize(0.5);
  g->SetMarkerColor(kBlack);
  g->Draw("AP");

  TF1 *fitg = new TF1("fitg", "[0]+[1]*x", 54.4, 56.4);
  g->Fit(fitg, "R");
  TFitResultPtr rg = g->Fit(fitg, "RS");
  double C = fitg->GetParameter(0);
  double eC = fitg->GetParError(0);
  double D = fitg->GetParameter(1);
  double eD = fitg->GetParError(1);
  double covCD = rg->CovMatrix(0,1);
  
  TGraph *g_fitg = new TGraph(4, vbias, B);
  fitg->SetLineColor(kPink+7);
  fitg->SetLineWidth(2);
  fitg->Draw("SAME");
  
  double res[4], eres[4];
  for(int i=0; i<4; i++){
    res[i]  = B[i]-fitg->Eval(vbias[i]);
    eres[i] = eB[i];
  }

  TGraphErrors *gres = new TGraphErrors(4, vbias, res, 0, eres);
  gres->SetMarkerStyle(20);
  gres->SetMarkerSize(0.5);
  gres->SetMarkerColor(kBlack);
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
    double sigma = sqrt(eC*eC + xb[i]*xb[i]*eD*eD + 2*xb[i]*covCD);
    eyb[i] = sigma;
  }

  TGraphErrors *bandv = new TGraphErrors(NB, xb, yb, 0, eyb);
  bandv->SetFillColorAlpha(kPink, 0.35);
  bandv->SetLineColor(kPink-9);
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
  zerov->SetLineColor(kPink-8);
  zerov->SetLineWidth(1);
  zerov->Draw("SAME");

  cout<<endl;
  
  f->cd();
  r->Write();
  rr->Write();
  c->Write();
  f->Write();
  //f->Close();


}
