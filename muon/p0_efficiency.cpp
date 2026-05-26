// ROOT method for Effinciency estimation

#include <iostream>
#include <vector>
using namespace std;

void eff(TString output="eff.root"){
  //gStyle->SetOptFit(111);
  TFile *f = new TFile(output, "RECREATE");
  f->cd();

  // ---- Dataset
      // -> PROVA 1:
      // vector<double> vset      = {1108, 1209, 1304, 1406, 1501, 1604, 1712, 1806};  //correggi unità di misura
      // vector<double> countAC   = {552,  489,  545,  565,  546, 559,  544,  3272};
      // vector<double> countABC  = {3,    19,   126,  363,  342,  335,  339, 1928};
      // -> PROVA 2:
      // vector<double> vset   = {1108, 1209, 1240, 1273, 1304, 1340, 1371, 1406, 1440, 1470, 1501, 1544, 1573, 1604, 1642, 1794, 1900, 2006, 2099};
      // vector<double> countAC  = {552,  489,  669,  617,  545,  605,  685,  656,  656,  651,  685,  629,  735,   637,  646,  628, 634,  614,  660};
      // vector<double> countABC = {3,    19,   31,   71,   126,  223,  331,  347,  356,  327,  349,  328,  409,   378,  391,  427, 413,  412,  468};
      // -> PROVA 3: mantenuti quelli di prima fino a 1304
  vector<double> vset     = {1200, 1250, 1300, 1323, 1350, 1411, 1502, 1602, 1702, 1802, 1904, 2005, 2100};
  vector<double> countAC  = {386,  346,  371,  393,  355,  350,  363,  312,  387,  353,  379,  354,  384};
  vector<double> countABC = {4,    24,   78,   117,  154,  158,  187,  170,  212,  195,  201,  205,  214};
  
  int n = vset.size();
  vector<double> err_vset, eff, err_eff, eff0, err_eff0;

  // ---- Efficiency estimation
  for(int j=0 ; j<n ; j++){
    //binomial handmade
    eff.push_back(countABC[j]/countAC[j]);
    err_eff.push_back(sqrt(eff[j]*(1-eff[j])/countAC[j]));
    //bayesian handmade
    eff0.push_back((countABC[j]+1)/(countAC[j]+2));
    err_eff0.push_back(sqrt(eff[j]*(1-eff[j])/(countAC[j]+3)));
    err_vset.push_back(vset[j]*0.01);    //(Somma in quadratura con errore voltmetro, ma pesa poco)
  }

  // ---- TGraph Data
  TCanvas *c1 = new TCanvas();
  c1->SetGrid();
  TGraph *AC = new TGraph(n, &vset[0], &countAC[0]);
  AC->SetTitle("Counts AC;VSet (V);A and C");
  AC->SetMarkerStyle(20);
  AC->SetMarkerColor(kViolet-3);
  AC->Draw("AP");
  c1->Write();

  TCanvas *c2 = new TCanvas();
  c2->SetGrid();
  TGraph *ABC = new TGraph(n, &vset[0], &countABC[0]);
  ABC->SetTitle("Counts ABC;VSet (V);(A and C) and B");
  ABC->SetMarkerStyle(20);
  ABC->SetMarkerColor(kTeal-5);
  ABC->Draw("AP");
  c2->Write();

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
  ch1->Write();
  TCanvas *ch2 = new TCanvas();
  hABC->SetLineColor(kTeal-5);
  hABC->SetLineWidth(2);
  hABC->SetFillColor(kTeal-5);
  hABC->SetFillStyle(3005);
  hABC->SetBarWidth(0.8);
  hABC->Sumw2();
  hABC->Draw("bar");
  ch2->Write();

  // ---- Efficiency VS VSet
     // Binomial error by hand
  TCanvas *c3 = new TCanvas();
  TGraphErrors *G_eff = new TGraphErrors(n, &vset[0], &eff[0], &err_vset[0], &err_eff[0]);
  G_eff->SetTitle("Efficiency handmade binomial;VSet (V);Efficiency");
  G_eff->SetMarkerStyle(22);
  G_eff->SetMarkerColor(kPink-8);
  c3->SetGrid();
  G_eff->SetMinimum(-0.05);
  G_eff->Draw("AP");
  
        // Bayesian method by hand
  TCanvas *c30 = new TCanvas();
  TGraphErrors *G_eff0 = new TGraphErrors(n, &vset[0], &eff0[0], &err_vset[0], &err_eff0[0]);
  G_eff0->SetTitle("Efficiency handmade bayesian;VSet (V);Efficiency");
  G_eff0->SetMarkerStyle(22);
  G_eff0->SetMarkerColor(kGreen+2);
  c30->SetGrid();
  G_eff0->SetMinimum(-0.05);
  G_eff0->Draw("AP");
  
  /* ROOT default method
  TCanvas *c4 = new TCanvas();
  TGraphAsymmErrors *m_eff = new TGraphAsymmErrors();
  m_eff->Divide(hABC, hAC);
         //3^ parametro in ingresso = stringa con il metodo -> "B" binomiale, "b(.,.)" bayesian, "cp" Clopper–Pearson esplicito, "w" Wilson, "ac" Agresti–Coull
  m_eff->SetTitle("Efficiency methods: Binomial;VSet (V);Efficiency");
  m_eff->SetMarkerStyle(21);
  m_eff->SetMarkerColor(kGreen+2);
  c4->SetGrid();
  m_eff->SetMinimum(-0.05);
  m_eff->Draw("AP");
  c4->Write();*/

  /*
  TCanvas* c330 = new TCanvas();
  c330->SetGrid();
  G_eff->Draw("AP");
  G_eff0->Draw("SAME P");
  */
  
  // ---- Efficiency VS VSet (tutti i metodi ROOT sovrapposti)
  TCanvas *c5 = new TCanvas();
  c5->SetGrid();
  vector<string> methods = {"", "cp", "ac", "w"}; 
  vector<int> colors = {kGreen+2, kRed, kBlue, kOrange-3};
  TLegend *leg = new TLegend(0.65,0.15,0.88,0.3);
  vector<TGraphAsymmErrors*> graphs;
  for (int i = 0; i < methods.size(); i++) {
    TGraphAsymmErrors *g = new TGraphAsymmErrors();
    if(methods[i] == "")
      g->Divide(hABC, hAC);        // default ROOT (binomiale)
    else
      g->Divide(hABC, hAC, methods[i].c_str());
    for (int l=0; l<n; l++) { //errore su asse x
      g->SetPointEXlow(l, err_vset[l]);
      g->SetPointEXhigh(l, err_vset[l]);
    }
    g->SetMarkerStyle(20 + i);
    g->SetMarkerColor(colors[i]);
    g->SetLineColor(colors[i]);
    if(i == 0){
      g->SetTitle("Efficiency methods: Comparison;VSet (V);Efficiency");
      g->SetMinimum(-0.05);
      g->Draw("AP");
    } else {
      g->Draw("P SAME");
    }
    string label;
    if(methods[i] == "") label = "Binomial (default)";
    else if(methods[i] == "cp") label = "Clopper-Pearson";
    else if(methods[i] == "w") label = "Wilson";
    else if(methods[i] == "ac") label = "Agresti-Coull";
    leg->AddEntry(g, label.c_str(), "p");
    graphs.push_back(g);
  }
  leg->Draw();
  c5->Write();


  // ---- Fit con Erf sugli handmade
  TF1 *f_erf = new TF1("f_erf",
		       "[2]*0.5*(1+TMath::Erf((x-[0])/(sqrt(2)*[1])))",
		       vset.front(), vset.back()
		       );
  f_erf->SetParameters(100, 1400, 100);
  f_erf->SetParNames("Mean", "Sigma", "N");
     // Fit su binomiale
  c3->cd();
  G_eff->Fit(f_erf, "R");
  f_erf->SetLineColor(kRed);
  f_erf->Draw("SAME");
  double chi2=f_erf->GetChisquare(), par0=f_erf->GetParameter(2), err_par0=f_erf->GetParError(2);
  
  int ndf=f_erf->GetNDF();
  TLegend *leg_1= new TLegend(0.6, 0.15, 0.88, 0.4);
  leg_1->SetTextSize(0.03);
  leg_1->AddEntry(f_erf, "Method: binomial");
  leg_1->AddEntry(f_erf, Form("#chi^{2}/Ndf = %.2f/%d", chi2, ndf), "");
  leg_1->AddEntry(f_erf, Form("N = %.2f #pm %.2f", par0, err_par0), "");
  leg_1->Draw();
  c3->Update();
  c3->Write();
     // Fit su bayesiano
  TF1 *f_erf2 = (TF1*)f_erf->Clone("f_erf2");
  c30->cd();
  G_eff0->Fit(f_erf2, "R");
  f_erf2->SetLineColor(kBlue);
  f_erf2->Draw("SAME");
  double chi2_2=f_erf2->GetChisquare(), par0_2=f_erf2->GetParameter(2), err_par0_2=f_erf2->GetParError(2);
  int ndf_2=f_erf2->GetNDF();
  TLegend *leg2 = new TLegend(0.6, 0.15, 0.88, 0.4);
  leg2->SetTextSize(0.03);
  leg2->AddEntry(f_erf2, "Method: bayesian");
  leg2->AddEntry(f_erf2, Form("#chi^{2}/Ndf = %.2f/%d", chi2_2, ndf_2), "");
  leg2->AddEntry(f_erf2, Form("N = %.2f #pm %.2f", par0_2, err_par0_2), "");
  leg2->Draw();
  c30->Update();
  c30->Write();
  
  f->Write();
}
