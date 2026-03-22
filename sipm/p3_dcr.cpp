#include <iostream>
using namespace std;
const int L=1024*8; //----- TO BE MODIFIED

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

  /* --------- LEGEND: VBias (intensity) [V] -> Threshold [ADC]
     54.51 (250) -> 30
     55.11 (40) -> 30
     55.81 (70) -> 50
     56.31 (200) -> 60
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

  TH1I *h_dce = new TH1I("h_dce", "DC Events Hist;DC Events;Events", 12, -1, 11); //-1.5, 10.5
  for(int i=0; i<n; i++) {
    outTree->GetEntry(i);
    h_dce->Fill(dce);
  }


  TF1 *pp = new TF1("pp","[1]*TMath::PoissonI(x,[0])", 0, 11);
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
  dcr_err=dceN_err/t; //SENSIBILITà NEL DOMINIO DELLE FREQUENZE?????????????????????????????????????????????
  cout<<endl<<"VBias: "<<vbias<<" (t = "<<t<<" s) -> DC Events: "<<dceN<<" +- "<<dceN_err<<endl;
  cout<<"    -----> DCR (Hz) = "<<dcr<<" +- "<<dcr_err<<endl<<endl;

  
  /*
  TO DO: PRENDI DATI PER ALTRI VBIAS con giusta time window 
  TO DO: GRAFICO LINEARE - Residui?
  TO DO: PROPAGA L'ERRORE SUL DCR - capisci in che rapporto si trova con la sensibilità nel dominio delle frequenze
  */

  cout<<endl<<"Digitare 0 se si vuole fare il grafico lineare ---> "<<endl;
  int p;
  double dcr_array[4], dcrerr_array[4], vbias_array[4]={54.51, 55.11, 55.81, 56.31},
    vbiaserr_array[4]={0.01/sqrt(12), 0.01/sqrt(12), 0.01/sqrt(12), 0.01/sqrt(12)}; //BOH, CERCHIAMO DI CAPIRE SE HA SENSO
  cin>>p;
  cout<<endl;

  if(p==0){
    for(int i=0; i<4; i++){
      cout<<"VBias (V): "<<vbias_array[i]<<" => DCR = ";
      cin>>dcr_array[i];
      cout<<" DCR_err = ";
      cin>>dcrerr_array[i];
    }
    cout<<endl;

    TGraphErrors *dc_lin= new TGraphErrors(4, vbias_array, dcr_array, vbiaserr_array, dcrerr_array);
    dc_lin->SetTitle("Gain VS Vbias;Vbias (V);Gain");
    dc_lin->Draw("AP");

    TF1 *fit_lin = new TF1("fit_lin", "[0]+[1]*x", 54.4, 56.4);
    dc_lin->Fit(fit_lin, "R");
    TFitResultPtr rg = dc_lin->Fit(fit_lin, "RS");
    double A = fit_lin->GetParameter(0);
    double eA = fit_lin->GetParError(0);
    double B = fit_lin->GetParameter(1);
    double eB = fit_lin->GetParError(1);
    double covAB = rg->CovMatrix(0,1);

    TGraph *g_fitg = new TGraph(4, vbias_array, dcr_array);
    fit_lin->SetLineColor(kRed-7);
    fit_lin->SetLineWidth(2);
    fit_lin->Draw("SAME");
  }
  cout<<endl;
  
  f->Write();

}
