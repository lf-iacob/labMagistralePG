
// --- Multiple event analysis: FIT for gain with residuals ---

#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <algorithm>
using namespace std;
const int N=100; //data for baseline

void analizza(
              TString file_name1, TString file_name2, TString file_name3, TString file_name4, TString file_name5,
              TString output="analysis.root" //TFile name as output
             ){
  cout<<endl<<"------ MULTIPLE EVENT ANALYSIS ------"<<endl<<endl;
    
  //---- File ROOT
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
  TString files[5] = {file_name1, file_name2, file_name3, file_name4, file_name5};
  for(int k=0; k<5; k++){
    file.open(files[k]);
    while(file.good()){
      b=0.; c=0.;
      for(int i=0;i<1024;i++){
	file>>a[i];
	if(i<N) b+=double(a[i])/N;
	else if(i>=450 && i<=550) c+=(b-a[i])*2000./4096*4/50; //hard-coded
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

  
  
  double pe[7]={0,1,2,3,4,5,6}, epe[7]={0,0,0,0,0,0,0};
  
  /*
  double range_i[7] = {-3.4, 12, 28, 44, 59, 75, 90};
  double range_f[7] = { 3.4, 18, 34, 49, 66, 81, 96};

  TF1 *gfit[7];
  for(int j=0; j<7; j++){
    gfit[j] = new TF1("gfit", "gaus", -20, 140); //hard-coded
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
  */

 
  //---- Sum of Gaussians Fit

  
  int p=7; // peaks istogramma considerati

  double xlow  = hc->GetXaxis()->GetXmin();
  double xhigh = 100; //prendo gli estremi per il fit
  auto sum_gaus = new TF1("sum_gaus",[p](double *x, double *par){
    double y=0;
    for(int i=0; i<p;i++){
      const double A= par[3*i+0];
      const double mu=par[3*i+1];
      const double sigma=par[3*i+2];
      y +=  A * TMath::Gaus(x[0], mu, sigma, false); //norm=false implica che A is the peak height
    }
    return y;
  },
    xlow, xhigh, 3*p );
  
  for(int i=0; i<p; i++){
    sum_gaus->SetParLimits(3*i+2, 1e-3, 1e6); //costrain sigma in a positive range
  }
  sum_gaus->SetNpx(1000);
  
  //(not so) optional: peak search, boh questo è copiato
  
  double approxSigma = hc->GetXaxis()->GetBinWidth(1)*2;
  TSpectrum s(p); //TSpectrum is a ROOT class fatta apposta per trovare i picchi di un istogramma
  const int nfound = s.Search(hc, 1, "", 0.01); // peak search; adjust if needed
  cout<<"picchi trovati"<<nfound<<endl;
  auto peaks = s.GetPositionX();
  for (int i = 0; i < p; ++i) {
  double mu, amp;
  if (i < nfound) {
    mu  = peaks[i];
    amp = hc->GetBinContent(hc->FindBin(mu));
    cout<<"x del picco "<<peaks[i]<<endl;
  } else {
    mu  = hc->GetMean()+approxSigma*i;
    amp = hc->GetMaximum()/p;
  }
  sum_gaus->SetParameter(3*i + 0, amp);
  sum_gaus->SetParameter(3*i + 1, mu);
  sum_gaus->SetParameter(3*i + 2, approxSigma);
}

  

  hc->Fit(sum_gaus, "R");  //perform the fit
  /*
  double muu[9];
  for(int i=0; i<3*p; i++){
    muu[i] =r->Parameter(3*i+1);
  }
  sort(muu,muu+p);
  double gain[9];
  for(int i=0;i<p;i++){
    gain[i]=muu[i]*2000/4096*4/50;
    cout<<gain[i]<<endl;
    } */
  double Amp[7];
  double mu[7];
  double sigma[7];

  double mu_c[7];
  double sigma_c[7];

  
  for (int g = 0; g < p; ++g) {
    Amp[g]     = sum_gaus->GetParameter(3*g + 0);
    mu[g]    = sum_gaus->GetParameter(3*g + 1)/1.6e-7/pow(10,32./20); //rescale for gain
    sigma[g] = sum_gaus->GetParError(3*g + 1)/1.6e-7/pow(10,32./20);
    cout << "Gaussiana " << g+1
              << ": A=" << Amp[g]
	 << ", mu=" << mu[g]
	 << ", errore mu=" << sigma[g]<<endl;
}

  for (int g = 0; g < p; ++g) {
     mu_c[g]    = sum_gaus->GetParameter(3*g + 1);
   sigma_c[g] = sum_gaus->GetParError(3*g + 1);
    cout << "Gaussiana " << g+1
              << ": A=" << Amp[g]
	 << ", mu=" << mu_c[g]
	 << ", errore mu=" << sigma_c[g]<<endl;
}
  

  //DRAW 
  TCanvas* canv = new TCanvas("canv", "Sum of Gaussians fit", 900, 700);
  hc->Draw();

  sum_gaus->SetLineColor(kRed+1);
  sum_gaus->SetLineWidth(2);
  sum_gaus->Draw("SAME");

 //---- Plot (linear)
  cout<<"--- Linear fit ---"<<endl;
  TGraphErrors *gr = new TGraphErrors(7, pe, mu, epe, sigma);
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
    res[i]  = mu[i]-l->Eval(pe[i]);
    eres[i] = sigma[i];
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
