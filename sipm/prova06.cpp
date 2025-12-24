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
    double a_gain[7];
    double a_egain[7];
    double gain[4];
};


gain_res gain(double vbias, TString file1, TString file2, TString file3){
  //SOLVE: HARD-CODED => fai un if in cui definisci i valori del range con la condizione sul valore del vbias
  // => if(vbias==55.11) { range_i[7] = ecc...}

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
	else if(i>=450 && i<=550) c+=(b-a[i])*2000./4096*4/50; //HARD-CODED
      }
      dd->Fill();
    }
    file.close();
  }

  int n = dd->GetEntries(); //#events collected

  //---- Charge THistogram
  TH1F *hc = new TH1F("hc", "Charge Histogram;Charge (pC);Counts",500,-50,300); //HARD-CODED
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    hc->Fill(c);
  }
  
  //---- Gaussian Fit
  double m[7], em[7];
  double pe[7]={0,1,2,3,4,5,6}, epe[7]={0,0,0,0,0,0,0};
  
  double range_i[7] = {-3.4, 12, 28, 44, 59, 75, 90}; //HARD-CODED
  double range_f[7] = { 3.4, 18, 34, 49, 66, 81, 96}; //HARD-CODED

  TF1 *gfit = new TF1("gfit", "gaus");
  for(int j=0; j<7; j++){
    hc->Fit(gfit, "QS", " ", range_i[j], range_f[j]); 
    m[j] = gfit->GetParameter(1)/1.6e-7/pow(10, 32./20.); 
    em[j] = gfit->GetParError(1)/1.6e-7/pow(10, 32./20.);
  }
  
  //---- Plot (linear)
  TGraphErrors *gr = new TGraphErrors(7, pe, m, epe, em);
  TF1 *l = new TF1("l", "[0]+[1]*x", -0.5, 6.5);
  gr->Fit(l, "R");
  double A = l->GetParameter(0);
  double eA = l->GetParError(0);
  double B = l->GetParameter(1);
  double eB = l->GetParError(1);

  cout<<"VBias = "<<vbias<<" -> Gain 1pe = "<<B<<"+-"<<eB<<"; 0pe = "<<A<<"+-"<<eA<<endl;

  gain_res ris;

  ris.vbias=vbias;
  for(int i=0; i<7; i++){
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
	    TString output="output.cpp"
){
  cout<<endl<<"------ Gain VS Voltage ------"<<endl<<endl;

  TFile *f = new TFile(output, "RECREATE");

  gain_res v1 = gain(55.11, file1, file2, file3);
  gain_res v2 = gain(52,    file4, file5, file6);
  gain_res v3 = gain(57,    file7, file8, file9);

  double pe[7]  = {0,1,2,3,4,5,6};
  double epe[7] = {0,0,0,0,0,0,0};

  double vbias[3] = {v1.vbias, v2.vbias, v3.vbias};
  double gain1pe[3] = {v1.gain[0], v2.gain[0], v3.gain[0]};
  double egain[3] = {v1.gain[1], v2.gain[1], v3.gain[1]}; 

  double y1[7], y2[7], y3[7];
  for(int j=0; j<7; j++){
    y1[j] = v1.gain[2] + gain1pe[0]*pe[j];
    y2[j] = v2.gain[2] + gain1pe[1]*pe[j];
    y3[j] = v3.gain[2] + gain1pe[2]*pe[j];
  }

  //--- Linear plots gainVSpe
  TCanvas *r = new TCanvas();

  TGraphErrors *g1 = new TGraphErrors(7, pe, v1.a_gain, epe, v1.a_egain);
  g1->SetMarkerColor(kRed);
  TGraphErrors *g2 = new TGraphErrors(7, pe, v2.a_gain, epe, v2.a_egain);
  g2->SetMarkerColor(kBlue);
  TGraphErrors *g3 = new TGraphErrors(7, pe, v3.a_gain, epe, v3.a_egain);
  g3->SetMarkerColor(kGreen);
  
  TGraph *fit1 = new TGraph(7, pe, y1);
  fit1->SetLineColor(kRed);
  fit1->SetLineWidth(2);
  TGraph *fit2 = new TGraph(7, pe, y2);
  fit2->SetLineColor(kBlue);
  fit2->SetLineWidth(2);
  TGraph *fit3 = new TGraph(7, pe, y3);
  fit3->SetLineColor(kGreen+2);
  fit3->SetLineWidth(2);

  g1->SetTitle("Gain vs PE;PE;Gain");
  g1->Draw("AP");        
  g2->Draw("P SAME");
  g3->Draw("P SAME");
  fit1->Draw("L SAME"); 
  fit2->Draw("L SAME");
  fit3->Draw("L SAME");

  r->BuildLegend();
  r->Update();

  //---- Plot gainVSvbias
  TCanvas *c = new TCanvas();
  TGraphErrors *g = new TGraphErrors(3, vbias, gain1pe, 0, egain);
  g->SetTitle("Gain VS Vbias;Vbias (V);Gain");
  g->SetMarkerColor(kPink);
  g->Draw("AP"); 
  
  f->Write();
  //f->Close();

}
