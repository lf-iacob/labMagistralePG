// --- Multiple event analysis: gainVSvoltage ---
#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <vector>
using namespace std;
const int N=100; //data for baseline

struct gain_res {
  double vbias;
  double a_gain[6];
  double a_egain[6];
  double gain[4];
  double noise[2];
};


gain_res gain(double vbias, TString file1, TString file2, TString file3){

  //---- Create TTree
  int a[1024];
  double b, c;
  TTree* dd = new TTree("dd", "dd");
  dd->Branch("A", a, "a[1024]/I");
  dd->Branch("B", &b, "b/D");
  dd->Branch("C", &c, "c/D");

  //---- TTree->Fill()
  ifstream file;
  TString files[3] = {file1, file2, file3};
  for(int k=0; k<3; k++){
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
  
  //---- Gaussian Fit
  double m[6], em[6], ch[2], s[2];
  double pe[6]={0,1,2,3,4,5}, epe[6]={0,0,0,0,0,0};
  
  vector<double> range_i = {-3.4, 12, 28, 44, 59, 75}, //VBias==55.11
    range_f = { 3.4, 18, 34, 49, 66, 81};
  if(vbias==54.51){       //Da aggiustare per fit migliori
    range_i = {-3,  8, 23.5, 34, 48, 60};
    range_f = { 3, 17, 28  , 40, 54, 66};
  }
  else if(vbias==56.31) { //Da aggiustare per fit migliori
    range_i = {-5, 18, 39, 65, 86, 108};
    range_f = { 4, 25, 48, 70, 94, 114};
  }
  else if(vbias==55.81) { //Da aggiustare per fit migliori
    range_i = { -4, 14, 34, 55, 75, 95};
    range_f = {2.5, 21, 41, 60, 78, 99};
  }

  TF1 *gfit = new TF1("gfit", "gaus");
  for(int j=0; j<6; j++){
    hc->Fit(gfit, "QS", " ", range_i[j], range_f[j]);
    if(j==0 || j==1){
      ch[j] = gfit->GetParameter(1);
      s[j] = gfit->GetParameter(2);
    }
    m[j] = gfit->GetParameter(1)/1.6e-7/pow(10, 32./20.); 
    em[j] = gfit->GetParError(1)/1.6e-7/pow(10, 32./20.);
  }

  
  //---- Plot (linear)
  TGraphErrors *gr = new TGraphErrors(6, pe, m, epe, em);
  TF1 *l = new TF1("l", "[0]+[1]*x", -0.5, 5.5);
  gr->Fit(l, "R");
  double A = l->GetParameter(0);
  double eA = l->GetParError(0);
  double B = l->GetParameter(1);
  double eB = l->GetParError(1);

  cout<<"VBias = "<<vbias<<" -> Gain 1pe = "<<B<<"+-"<<eB<<"; 0pe = "<<A<<"+-"<<eA<<endl;

  gain_res ris;

  ris.noise[0] = (ch[1]-ch[0])/s[0];
  ris.noise[1] = (ch[1]-ch[0])/ sqrt(s[0]*s[0]+s[1]*s[1]);

  ris.vbias=vbias;
  for(int i=0; i<6; i++){
    ris.a_gain[i]=m[i];
    ris.a_egain[i]=em[i];
  }
  ris.gain[0]=B;
  ris.gain[1]=eB;
  ris.gain[2]=A;
  ris.gain[3]=eA;

  return ris;

}


void totale(
	    TString file1, TString file2, TString file3, //vbias1
	    TString file4, TString file5, TString file6, //vbias2
	    TString file7, TString file8, TString file9, //vbias3
	    TString file10, TString file11, TString file12, //vbias4
	    TString output="output.cpp"
){
  cout<<endl<<"------ Gain VS Voltage ------"<<endl<<endl;

  gStyle->SetOptFit(111);
  
  TFile *f = new TFile(output, "RECREATE");

  gain_res v2 = gain(55.11, file1, file2, file3);
  gain_res v1 = gain(54.51, file4, file5, file6);
  gain_res v4 = gain(56.31, file7, file8, file9);
  gain_res v3 = gain(55.81, file10, file11, file12);

  double pe[6]  = {0,1,2,3,4,5};
  double epe[6] = {0,0,0,0,0,0};

  double vbias[4] = {v1.vbias, v2.vbias, v3.vbias, v4.vbias};
  double gain1pe[4] = {v1.gain[0], v2.gain[0], v3.gain[0], v4.gain[0]};
  double egain[4] = {v1.gain[1], v2.gain[1], v3.gain[1], v4.gain[1]}; 

  double y1[6], y2[6], y3[6], y4[6];
  for(int j=0; j<6; j++){
    y1[j] = v1.gain[2] + gain1pe[0]*pe[j];
    y2[j] = v2.gain[2] + gain1pe[1]*pe[j];
    y3[j] = v3.gain[2] + gain1pe[2]*pe[j];
    y4[j] = v4.gain[2] + gain1pe[3]*pe[j];
  }

  //--- Linear plots gainVSpe
  TCanvas *r = new TCanvas();
  r->SetGrid(); 

  TGraphErrors *g1 = new TGraphErrors(6, pe, v1.a_gain, epe, v1.a_egain);
  g1->SetMarkerColor(kRed);
  g1->SetMarkerStyle(20);
  g1->SetMarkerSize(0.5);
  TGraphErrors *g2 = new TGraphErrors(6, pe, v2.a_gain, epe, v2.a_egain);
  g2->SetMarkerColor(kBlue);
  g2->SetMarkerStyle(20);
  g2->SetMarkerSize(0.5);
  TGraphErrors *g3 = new TGraphErrors(6, pe, v3.a_gain, epe, v3.a_egain);
  g3->SetMarkerColor(kGreen+2);
  g3->SetMarkerStyle(20);
  g3->SetMarkerSize(0.5);
  TGraphErrors *g4 = new TGraphErrors(6, pe, v4.a_gain, epe, v4.a_egain);
  g4->SetMarkerColor(kMagenta+2);
  g4->SetMarkerStyle(20);
  g4->SetMarkerSize(0.5);
  
  TGraph *fit1 = new TGraph(6, pe, y1);
  fit1->SetLineColor(kRed);
  fit1->SetLineWidth(2);
  TGraph *fit2 = new TGraph(6, pe, y2);
  fit2->SetLineColor(kBlue);
  fit2->SetLineWidth(2);
  TGraph *fit3 = new TGraph(6, pe, y3);
  fit3->SetLineColor(kGreen+2);
  fit3->SetLineWidth(2);
  TGraph *fit4 = new TGraph(6, pe, y4);
  fit4->SetLineColor(kMagenta+2);
  fit4->SetLineWidth(2);

  g4->SetTitle("Gain vs PE;PE;Gain");
  g4->GetXaxis()->SetRangeUser(-0.5, 5.5);
  g4->Draw("AP");        
  g2->Draw("P SAME");
  g3->Draw("P SAME");
  g1->Draw("P SAME");
  fit1->Draw("L SAME"); 
  fit2->Draw("L SAME");
  fit3->Draw("L SAME");
  fit4->Draw("L SAME");

  TLegend *leg = new TLegend(0.13, 0.6, 0.6, 0.88);
  leg -> AddEntry(g1, Form("54.51 V => A=%.0f+-%.0f, B=%.0f+-%.0f", v1.gain[2], v1.gain[3], v1.gain[0], v1.gain[1]), "lp");
  leg -> AddEntry(g2, Form("55.11 V => A=%.0f+-%.0f, B=%.0f+-%.0f", v2.gain[2], v2.gain[3], v2.gain[0], v2.gain[1]), "lp");
  leg -> AddEntry(g3, Form("55.81 V => A=%.0f+-%.0f, B=%.0f+-%.0f", v3.gain[2], v3.gain[3], v3.gain[0], v3.gain[1]), "lp");
  leg -> AddEntry(g4, Form("56.31 V => A=%.0f+-%.0f, B=%.0f+-%.0f", v4.gain[2], v4.gain[3], v4.gain[0], v4.gain[1]), "lp");
  leg -> Draw();
  r->Update();


  //---- Residui
  TCanvas *cres = new TCanvas("cres","Residuals per Vbias");
  cres->Divide(1,4);

  TGraphErrors* gres_arr[4];
  TGraphErrors* gband_arr[4];
  gain_res v_arr[4] = {v1,v2,v3,v4};
  TString labels[4] = {"54.51 V","55.11 V","55.81 V","56.31 V"};
  int colors[4] = {kRed, kBlue, kGreen+2, kMagenta+2};

  for(int k=0; k<4; k++){
    cres->cd(k+1);
    gPad->SetGrid(1,1);
    double res[6];
    for(int j=0; j<6; j++){
      res[j] = v_arr[k].a_gain[j] - (v_arr[k].gain[2] + v_arr[k].gain[0]*j);
    }

    gres_arr[k] = new TGraphErrors(6, pe, res, epe, v_arr[k].a_egain);
    gres_arr[k]->SetMarkerStyle(20);
    gres_arr[k]->SetMarkerSize(0.5);
    gres_arr[k]->SetMarkerColor(colors[k]);
    gres_arr[k]->SetTitle(Form("Residuals %s;PE;Residui", labels[k].Data()));
    gres_arr[k]->GetXaxis()->SetRangeUser(-0.5, 5.5);
    gres_arr[k]->Draw("AP");

    TLine *lzero = new TLine(0,0,5.5,0);
    lzero->SetLineColor(kBlack);
    lzero->SetLineStyle(2);
    lzero->Draw("SAME");

    const int NB = 100;
    double xb[NB], yb[NB], eyb[NB];

    double A = v_arr[k].gain[2];
    double B = v_arr[k].gain[0];
    double eA = v_arr[k].gain[3];
    double eB = v_arr[k].gain[1];
    double covAB = 0; 

    for(int i=0; i<NB; i++){
      xb[i] = i*5.5/(NB-1);
      yb[i] = 0; 
      double sigma = sqrt(eA*eA + xb[i]*xb[i]*eB*eB + 2*xb[i]*covAB);
      eyb[i] = sigma;
    }

    gband_arr[k] = new TGraphErrors(NB, xb, yb, 0, eyb);
    gband_arr[k]->SetFillColorAlpha(colors[k],0.35);
    gband_arr[k]->SetLineColor(colors[k]);
    gband_arr[k]->SetFillStyle(1001);
    gband_arr[k]->Draw("3 SAME");
  }

  cres->Update();
  cres->Write();


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
  TGraphErrors *g = new TGraphErrors(4, vbias, gain1pe, 0, egain);
  g->SetTitle("Gain VS Vbias;Vbias (V);Gain");
  g->SetMarkerColor(kPink);
  g->Draw("AP");

  TF1 *fitg = new TF1("fitg", "[0]+[1]*x", 54.4, 56.4);
  g->Fit(fitg, "R");
  TFitResultPtr rg = g->Fit(fitg, "RS");
  double C = fitg->GetParameter(0);
  double eC = fitg->GetParError(0);
  double D = fitg->GetParameter(1);
  double eD = fitg->GetParError(1);
  double covCD = rg->CovMatrix(0,1);
  
  TGraph *g_fitg = new TGraph(4, vbias, gain1pe);
  fitg->SetLineColor(kPink);
  fitg->SetLineWidth(2);
  fitg->Draw("SAME");
  
  double res[4], eres[4];
  for(int i=0; i<4; i++){
    res[i]  = gain1pe[i]-fitg->Eval(vbias[i]);
    eres[i] = egain[i];
  }

  TGraphErrors *gres = new TGraphErrors(4, vbias, res, 0, eres);
  gres->SetMarkerStyle(8);
  gres->SetMarkerSize(0.5);
  gres->SetMarkerColor(kAzure-3);
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

  TGraphErrors *band = new TGraphErrors(NB, xb, yb, 0, eyb);
  band->SetFillColorAlpha(kPink-1, 0.35);
  band->SetLineColor(kPink-3);
  band->SetFillStyle(1001);

  double minY = 1e9;
  double maxY = -1e9;
  for(int i=0; i<4; i++){
    double rmin = res[i] - eres[i];
    double rmax = res[i] + eres[i];
    if(rmin < minY) minY = rmin;
    if(rmax > maxY) maxY = rmax;
  }
  double margin = 0.1*(maxY - minY);
  band->GetYaxis()->SetRangeUser(minY - margin, maxY + margin);

  band->Draw("3");
  gres->Draw("P SAME");

  TLine *zero = new TLine(54.4,0,56.4,0);
  zero->SetLineColor(kPink-1);
  zero->SetLineWidth(1);
  zero->Draw("SAME");

  c->Write();
  
  f->Write();
  //f->Close();

}

