// -- Single VBias Analysis --

#include <iostream>
using namespace std;
int const M=5; //number of files for same vbias


void analizza(
	      TString file1, TString file2, TString file3, TString file4="", TString file5="",
	      TString output = "output.root"
	      ){
  
  //---- TFile as output
  if(output == "output.root"){
    output = file1;
    output.ReplaceAll("alberi/alb", "VBias");
    Ssiz_t last_underscore = output.Last('_');
    Ssiz_t dot_pos = output.Last('.');
    output.Remove(last_underscore, dot_pos - last_underscore);
  }
  
  TFile *f = new TFile(output, "RECREATE");

  int first, second;
  sscanf(file1.Data(), "alberi/alb_%d_%d_", &first, &second);
  double vbias = first + second / 100.0;
  
  cout<<endl<<"------> Analysis Single VBias (V): "<<vbias<<endl<<endl;

  //---- Total data
  int a[1024];
  double b, c;
  TChain* ddchain = new TChain("dd","dd");
  TString files[M] = {file1, file2, file3, file4, file5};
  for(int i=0; i<M; i++){
    if(files[i]=="") continue;
    ddchain->AddFile(files[i]);
  }
  ddchain->GetListOfFiles()->Print();
  cout<<endl;

  ddchain->SetBranchAddress("A", a);
  ddchain->SetBranchAddress("B", &b);
  ddchain->SetBranchAddress("C", &c);  

  int n = ddchain->GetEntries();

  TH1F *hc_t = new TH1F("hc_t", "Charge Histogram;Charge (pC);Entries",500,-50,300);
  for(int i=0; i<n; i++) {
    ddchain->GetEntry(i);
    hc_t->Fill(c);
  }

  TH1F *hcf_t = (TH1F*)hc_t->Clone("hcf_t");
  
  TCanvas *hc_fit_t = new TCanvas("hc_fit_t", "hc_fit_t");
  gPad->SetTicks(1,1);
  hcf_t->Draw();
  hcf_t->SetLineColor(kBlack);
  hcf_t->Sumw2();

  //---- Gaussian Fit
  cout<<"--- Gaussian fit ---"<<endl;
  double ch[6], s[2], ech[6], m[6], em[6], chirGauss[6], pv[6];
  double pe[6]={0,1,2,3,4,5}, epe[6]={0,0,0,0,0,0};

  int color=kGreen+2;
  vector<double> range_i = {-3.4, 12, 28, 44, 59, 75}, //VBias==55.11
    range_f = { 3.4, 18, 34, 49, 66, 81};
  if(vbias==54.51){ //DA AGGIUSTARE
    color=kRed;
    range_i = {-3,  8, 23.5, 34, 48, 61};
    range_f = { 3, 17, 28  , 40, 54, 66};
  }
  else if(vbias==55.81) { //DA AGGIUSTARE
    color=kBlue;
    range_i = { -4, 14, 34, 55, 75, 95};
    range_f = {2.5, 21, 41, 60, 78, 99};
  }
  else if(vbias==56.31) { //DA AGGIUSTARE
    color=kMagenta+1;
    range_i = {-5, 18, 39, 65, 86, 108};
    range_f = { 4, 25, 48, 70, 94, 114};
  }

  
  TF1 *gfit[6];
  for(int j=0; j<6; j++){
    gfit[j] = new TF1("gfit", "gaus", -20, 300);
    hcf_t->Fit(gfit[j], "QS", " ", range_i[j], range_f[j]);
    ch[j] = gfit[j]->GetParameter(1);
    if(j==0 || j==1) s[j] = gfit[j]->GetParameter(2);
    ech[j] = gfit[j]->GetParError(1);
    m[j] = ch[j]/1.6e-7/pow(10, 32./20.); //gain
    em[j] = ech[j]/1.6e-7/pow(10, 32./20.); //error gain
    chirGauss[j] = gfit[j]->GetChisquare()/gfit[j]->GetNDF();
    pv[j] = TMath::Prob(gfit[j]->GetChisquare(),gfit[j]->GetNDF());
    cout<<j<<"pe => Charge (pC): "<<ch[j]<<"+-"<<ech[j]<<", Gain: "<<m[j]<<"+-"<<em[j]<<", X2r: "<<chirGauss[j]<<" -> p_value: "<<pv[j]<<" -- ["<<range_i[j]<<";"<<range_f[j]<<"] --"<<endl;
    gfit[j]->SetLineColor(color - j);
    gfit[j]->SetLineWidth(3);
    gfit[j]->SetNpx(750); //points for gauss function
    gfit[j]->Draw("same");
    hc_fit_t->Update();
  }
  hc_fit_t->Write();
  cout<<endl;

  //---- SNR, SSPE
  double snr = (ch[1]-ch[0])/s[0];
  double sspe = (ch[1]-ch[0])/ sqrt(s[0]*s[0]+s[1]*s[1]);
  cout<<"SNR: "<<snr<<"; SSPE: "<<sspe<<endl<<endl;
  
  //---- Plot (linear)
  cout<<"--- Linear fit ---"<<endl;
  TGraphErrors *gr = new TGraphErrors(6, pe, m, epe, em);
  gr->SetTitle("GainVSpe;pe;Gain");
  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(0.5);
  gr->SetMarkerColor(color+2);

  TCanvas *cgain = new TCanvas("lin","lin");
  cgain->Divide(1,2); 

  cgain->cd(1);
  gStyle->SetOptFit(1111);
  gPad->SetPad(0,0.32,1,1);     
  gPad->SetBottomMargin(0.08);
  gPad->SetTopMargin(0.06);
  gPad->SetTicks(1,1);
  gPad->SetGrid(1,1);
  gr->Draw("AP");
  double minY = 1e9, maxY = -1e9;
  for(int i=0; i<6; i++){
    if(m[i] < minY) minY = m[i];
    if(m[i] > maxY) maxY = m[i];
  }
  double margin = 0.1*(maxY - minY);
  gr->GetYaxis()->SetLimits(minY - margin, maxY + margin);
  gr->GetYaxis()->SetRangeUser(minY - margin, maxY + margin);
  gr->GetXaxis()->SetLimits(-0.5, 5.5);
  gr->GetXaxis()->SetRangeUser(-0.5, 5.5);
  cgain->Update();

  //---- Fit plot (linear)
  TF1 *l = new TF1("l", "[0]+[1]*x", -0.5, 5.5);
  l->SetLineColor(color);
  l->SetLineWidth(2);
  TFitResultPtr r = gr->Fit(l, "RS");
  double A = r->Parameter(0);
  double B = r->Parameter(1);
  double eA = r->ParError(0);
  double eB = r->ParError(1);
  cout<<endl<<"Coefficiente angolare (Gain) = "<<B<<"+-"<<eB<<endl;
  cout<<"Intercetta = "<<A<<"+-"<<eA<<endl<<endl;
  double covAB = r->CovMatrix(0, 1);

  TLegend *leg = new TLegend(0.13, 0.6, 0.5, 0.88);
  leg -> AddEntry(gr, Form("Data %.2f V", vbias), "p");
  leg -> AddEntry(l, "Fit", "l");
  leg -> AddEntry(l, Form("=> Intercept = %.0f +- %.0f", A, eA), "");
  leg -> AddEntry(l, Form("=> Slope (Gain) = %.0f +- %.0f", B, eB), "");
  leg -> Draw();
  cgain->Update();

  double res[6], eres[6];
  for(int i=0; i<6; i++){
    res[i]  = m[i]-l->Eval(pe[i]);
    eres[i] = em[i];
  }

  TGraphErrors *gres = new TGraphErrors(6, pe, res, epe, eres);
  gres->SetMarkerStyle(8);
  gres->SetMarkerSize(0.5);
  gres->SetMarkerColor(color+2);
  gres->SetTitle(";pe;Residuals");
  
  cgain->cd(2);
  gPad->SetPad(0,0,1,0.32); 
  gPad->SetTopMargin(0.08);
  gPad->SetBottomMargin(0.28);
  gPad->SetTicks(1,1);
  double minYr = 1e9, maxYr = -1e9;
  for(int i=0; i<6; i++){
    double rmin = res[i] - eres[i];
    double rmax = res[i] + eres[i];
    if(rmin < minYr) minYr = rmin;
    if(rmax > maxYr) maxYr = rmax;
  }
  double marginr = 0.1*(maxYr - minYr);
  gres->GetYaxis()->SetLimits(minYr - marginr, maxYr + marginr);
  gres->GetYaxis()->SetRangeUser(minYr - marginr, maxYr + marginr);
  gres->GetXaxis()->SetLimits(-0.5,5.5);
  gres->GetXaxis()->SetRangeUser(-0.5,5.5);
  gres->Draw("AP");

  // residual fit: region
  const int NB = 300;
  double xb[NB], yb[NB], eyb[NB];

  for(int i=0; i<NB; i++){
    xb[i] = -0.5 + 6 * i / (NB-1);
    yb[i] = -0.5;
    double sigma = sqrt(eA*eA + xb[i]*xb[i]*eB*eB + 2*xb[i]*covAB);
    eyb[i] = sigma;
  }

  TGraphErrors *band = new TGraphErrors(NB, xb, yb, 0, eyb);
  band->SetFillColorAlpha(color-4, 0.35);
  band->SetLineColor(color-3);
  band->SetFillStyle(1001);

  cgain->cd(2);
  gPad->SetPad(0,0,1,0.32);
  gPad->SetTopMargin(0.08);
  gPad->SetBottomMargin(0.28);
  gPad->SetTicks(1,1);
  gPad->SetGrid(1,1);

  band->Draw("3");
  gres->Draw("P SAME");

  TLine *zero = new TLine(-0.5,0,5.5,0);
  zero->SetLineColor(color-1);
  zero->SetLineWidth(1);
  zero->Draw("same");

  cgain->Write();
  band->SetName("band");
  l->Write();
  gr->SetName("gp");
  gres->SetName("gpr");
  band->Write();
  gr->Write();
  gres->Write();
  
  //---- Saving in TFile
  f->Write();
  //f->Close();

}
