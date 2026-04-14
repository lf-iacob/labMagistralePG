// ROOT method for Effinciency estimation

#include <iostream>
#include <vector>
using namespace std;


void eff(){

  // ---- Dataset (1806V: 15min, altri: 3 min)
  vector<double> vset      = {1108, 1209, 1304, 1406, 1501, 1604, 1712, 1806};  //correggi unità di misura
  vector<double> countAC   = {552,  489,  545,  565,  546, 559,  544,  3272};
  vector<double> countABC  = {3,    19,   126,  363,  342,  335,  339, 1928};
  int n = vset.size();
  vector<double> err_vset, eff, err_eff;

  // ---- Efficiency estimation
  for(int j=0 ; j<n ; j++){
    eff.push_back(countABC[j]/countAC[j]);
    err_eff.push_back(sqrt(eff[j]*(1-eff[j])/countAC[j]));   //binomial error by hand
    err_vset.push_back(0.0);                                 //MODIFICA incertezza tensione!!
  }

  // ---- TGraph Data
  TCanvas *c1 = new TCanvas();
  TGraph *AC = new TGraph(n, &vset[0], &countAC[0]);
  AC->SetTitle("Counts AC;VSet (V);A and C");
  AC->SetMarkerStyle(20);
  AC->SetMarkerColor(kViolet-3);
  AC->Draw("AP");

  TCanvas *c2 = new TCanvas();
  TGraph *ABC = new TGraph(n, &vset[0], &countABC[0]);
  ABC->SetTitle("Counts ABC;VSet (V);(A and C) and B");
  ABC->SetMarkerStyle(20);
  ABC->SetMarkerColor(kTeal-5);
  ABC->Draw("AP");

  // ---- Hist Data
  vector<double> edges(n+1); //bins
  for (int i = 1; i < n; ++i)
    edges[i] = 0.5 * (vset[i] + vset[i-1]);
  edges[0] = vset[0] + (vset[0] - edges[1]);
  edges[n] = vset[n-1] - (edges[n-1] - vset[n-1]);
  TH1D* hAC  = new TH1D("hAC",  "Counts AC;VSet (V);Counts", n, &edges[0]);
  TH1D* hABC = new TH1D("hABC", "Counts ABC;VSet (V);Counts", n, &edges[0]);
  for(int i = 0; i < n; i++){
    hAC->SetBinContent(i+1, countAC[i]);    //hAC->SetBinError(i+1, sqrt(countAC[i]));
    hABC->SetBinContent(i+1, countABC[i]);  //hABC->SetBinError(i+1, sqrt(countABC[i]));
  }
  TCanvas *ch1 = new TCanvas();
  hAC->SetLineColor(kViolet-3);
  hAC->SetLineWidth(2);
  hAC->SetFillColor(kViolet-3);
  hAC->SetFillStyle(3005);
  hAC->SetBarWidth(0.8);
  hAC->Sumw2();
  hAC->Draw("bar");
  TCanvas *ch2 = new TCanvas();
  hABC->SetLineColor(kTeal-5);
  hABC->SetLineWidth(2);
  hABC->SetFillColor(kTeal-5);
  hABC->SetFillStyle(3005);
  hABC->SetBarWidth(0.8);
  hABC->Sumw2();
  hABC->Draw("bar");

  // ---- Efficiency VS VSet
     // Binomial error by hand
  TCanvas *c3 = new TCanvas();
  TGraphErrors *G_eff = new TGraphErrors(n, &vset[0], &eff[0], &err_vset[0], &err_eff[0]);
  G_eff->SetTitle("Efficiency handmade;VSet (V);Efficiency");
  G_eff->SetMarkerStyle(22);
  G_eff->SetMarkerColor(kPink-8);
  c3->SetGrid();
  G_eff->SetMinimum(-0.05);
  G_eff->Draw("AP");
     // ROOT method
  TCanvas *c4 = new TCanvas();
  TGraphAsymmErrors *m_eff = new TGraphAsymmErrors();
  m_eff->Divide(hABC, hAC);
         /* 3^ parametro in ingresso = stringa con il metodo
	    "B" binomiale, "b(.,.)" bayesian, "cp" Clopper–Pearson esplicito, "w" Wilson, "ac" Agresti–Coull */
  m_eff->SetTitle("Efficiency methods;VSet (V);Efficiency");
  m_eff->SetMarkerStyle(21);
  m_eff->SetMarkerColor(kGreen+2);
  c4->SetGrid();
  m_eff->SetMinimum(-0.05);
  m_eff->Draw("AP");
  

}
