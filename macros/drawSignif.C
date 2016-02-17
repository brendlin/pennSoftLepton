void ATLAS_LABEL(Double_t x,Double_t y,Color_t color,float tsize=0.04) 
{

  TLatex l; l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);
  l.DrawLatex(x,y,"ATLAS");
  
}

void myText(Double_t x,Double_t y,Color_t color,const char *text,float tsize=0.04) {

  TLatex l; l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetNDC();
  l.SetTextFont(42);
  l.SetTextColor(color);
  l.DrawLatex(x,y,text);
}

void myBoxText(Double_t x, Double_t y,Double_t boxsize,Int_t mcolor,char *text) 
{

  Double_t tsize=0.06;

  TLatex l; l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetNDC();
  l.DrawLatex(x,y,text);

  Double_t y1=y-0.25*tsize;
  Double_t y2=y+0.25*tsize;
  Double_t x2=x-0.3*tsize;
  Double_t x1=x2-boxsize;

  printf("x1= %f x2= %f y1= %f y2= %f \n",x1,x2,y1,y2);

  TPave *mbox= new TPave(x1,y1,x2,y2,0,"NDC");

  mbox->SetFillColor(mcolor);
  mbox->SetFillStyle(1001);
  mbox->Draw();

  TLine mline;
  mline.SetLineWidth(4);
  mline.SetLineColor(1);
  mline.SetLineStyle(1);
  Double_t y_new=(y1+y2)/2.;
  mline.DrawLineNDC(x1,y_new,x2,y_new);

}

void myMarkerText(Double_t x,Double_t y,Int_t color,Int_t mstyle,char *text,Float_t msize) 
{
  Double_t tsize=0.06;
  TMarker *marker = new TMarker(x-(0.4*tsize),y,8);
  marker->SetMarkerColor(color);  marker->SetNDC();
  marker->SetMarkerStyle(mstyle);
  marker->SetMarkerSize(msize);
  marker->Draw();

  TLatex l; l.SetTextAlign(12); //l.SetTextSize(tsize); 
  l.SetNDC();
  l.DrawLatex(x,y,text);
}

void setHistStyle(TH1F* input_hist, int fillColor,int lineColor, float lineWidth, int lineStyle){
  input_hist->SetFillColor(fillColor);
  //th1->SetMarkerColor();
  input_hist->SetLineColor(lineColor);
  input_hist->SetLineWidth(lineWidth);
  input_hist->SetLineStyle(lineStyle);
}

void fillHistZn(TH1F* Zn_hist, TH1F* ZnRel_hist, TH1F* sig_hist, TH1F* totB_hist, float RelUnc, bool lowerCut){

  using namespace RooStats::NumberCountingUtils;

  int firstBin = 0; //underflow
  int lastBin = Zn_hist->GetSize()-1; //includes overflow

  float totSig = sig_hist->Integral(firstBin, lastBin);
  float totBkg = totB_hist->Integral(firstBin, lastBin);

  float totalZn = BinomialObsZ(totSig+totBkg, totBkg, RelUnc);

  //loop Zn bins
  for(int ibin=firstBin; ibin < (lastBin+1); ibin++){

    float sigEvt = sig_hist->Integral(ibin, lastBin);
    float bkgEvt = totB_hist->Integral(ibin, lastBin);

    if(!lowerCut) {
      sigEvt = sig_hist->Integral(firstBin, ibin);
      bkgEvt = totB_hist->Integral(firstBin, ibin);
    }

    float Zn = 0;
    if (bkgEvt>0.) Zn=BinomialObsZ(sigEvt+bkgEvt, bkgEvt, RelUnc);

    //std::cout << ibin << "    " << sigEvt << "    " << bkgEvt << "    "  << Zn << "     "   << totalZn << "     "    <<  (((Zn-totalZn))/totalZn) <<  std::endl;
    float relativeZn = (((Zn-totalZn))/totalZn);
    if(totalZn<0.) relativeZn = (((totalZn - Zn))/totalZn);

    Zn_hist->SetBinContent(ibin,Zn);
    ZnRel_hist->SetBinContent(ibin,relativeZn);

  }//end loop

}

void drawSignif(TString optVar = "lep0Pt", int nbins = 50, float xmin = 0., float xmax = 50., TString axis_label = "#it{p}_{T}^{lead}", TString unit = "GeV", bool do0j=false, bool dology=true, bool doLowerCut = true, bool doRelImprovement = true, TString selectionCuts="") {

  gStyle->SetOptStat(0000);
  gStyle->SetOptTitle(0); //this will disable the title for all coming histograms
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  TString optCuts = "(NJets>=1)&&(nBjets==0)";
  if(do0j) optCuts = "(NJets==0)&&(nBjets==0)";

  optCuts+="&&"+selectionCuts;
  
  TString evtWeight="TotalWeightNoPupNoSF";
  float totB_RelUnc = 0.3;

  axis_label+=" ["+unit+"]";

  if (doLowerCut) axis_label = "(lower cut on) "+axis_label;
  else axis_label = "(upper cut on) "+axis_label;

  std::vector<TString> bkg_ntuples;
  std::vector<TString> bkg_names;
  std::vector<int> bkg_color;

  bkg_ntuples.push_back("../ntuples/zj.root");
  bkg_names.push_back("Z+jets");
  bkg_color.push_back(kViolet-9);

  bkg_ntuples.push_back("../ntuples/wj.root");
  bkg_names.push_back("W+jets");
  bkg_color.push_back(kViolet-7);

  bkg_ntuples.push_back("../ntuples/ww.root");
  bkg_names.push_back("WW");
  bkg_color.push_back(kMagenta);

  bkg_ntuples.push_back("../ntuples/zz.root");
  bkg_names.push_back("ZZ");
  bkg_color.push_back(kAzure);

  bkg_ntuples.push_back("../ntuples/top1.root");
  bkg_names.push_back("t#bar{t}");
  bkg_color.push_back(kOrange-2);

  bkg_ntuples.push_back("../ntuples/top2.root");
  bkg_names.push_back("single top");
  bkg_color.push_back(kYellow-9);

  bkg_ntuples.push_back("../ntuples/top3.root");
  bkg_names.push_back("t#bar{t}V");
  bkg_color.push_back(kCyan-1);

  bkg_ntuples.push_back("../ntuples/wz.root");
  bkg_names.push_back("WZ");
  bkg_color.push_back(kAzure-4);


  int nb_bkg = bkg_ntuples.size();

  std::vector<TString> sig_ntuples;
  std::vector<TString> sig_names;
  std::vector<int> sig_color;
  /*sig_ntuples.push_back("../samples/susy_205048-0.root");
  sig_names.push_back("(100,0) via WZ");
  sig_ntuples.push_back("../samples/susy_205049-0.root");
  sig_names.push_back("(200,0) via WZ");
  sig_ntuples.push_back("../samples/susy_205051-0.root");
  sig_names.push_back("(300,0) via WZ");
  sig_ntuples.push_back("../samples/susy_205051-0.root");
  sig_names.push_back("(400,0) via WZ");
  sig_ntuples.push_back("../samples/susy_205052-0.root");
  sig_names.push_back("(500,0) via WZ");
  sig_ntuples.push_back("../samples/susy_205053-0.root");
  sig_names.push_back("(150,100) via WZ");
  sig_ntuples.push_back("../samples/susy_205054-0.root");
  sig_names.push_back("(200,100) via WZ");
  sig_ntuples.push_back("../samples/susy_205055-0.root");
  sig_names.push_back("(200,150) via WZ");
  sig_ntuples.push_back("../samples/susy_205056-0.root");
  sig_names.push_back("(300,200) via WZ");*/
  sig_ntuples.push_back("../ntuples/susy_205128-0.root");
  sig_names.push_back("(125,100) via WZ");
  sig_color.push_back(kGray);
  sig_ntuples.push_back("../ntuples/susy_205129-0.root");
  sig_names.push_back("(150,125) via WZ");
  sig_color.push_back(kGray+1);
  sig_ntuples.push_back("../ntuples/susy_205130-0.root");
  sig_names.push_back("(175,150) via WZ");
  sig_color.push_back(kGray+2);
  sig_ntuples.push_back("../ntuples/susy_205131-0.root");
  sig_names.push_back("(125,112.5) via WZ");
  sig_color.push_back(kGray+3);

  int nb_sig = sig_ntuples.size();

  std::vector <TH1F*> sig_hists;
  sig_hists.resize(nb_sig);

  std::vector <TFile*> sig_files;
  sig_files.resize(nb_sig);

  std::vector <TH1F*> sig_cutflow;
  sig_cutflow.resize(nb_sig);

  std::vector <TTree*> sig_trees;
  sig_trees.resize(nb_sig);

  std::vector <TH1F*> bkg_hists;
  bkg_hists.resize(nb_bkg);

  std::vector <TH1F*> bkg_cutflow;
  bkg_cutflow.resize(nb_bkg);

  std::vector <TFile*> bkg_files;
  bkg_files.resize(nb_bkg);

  std::vector <TTree*> bkg_trees;
  bkg_trees.resize(nb_bkg);

  std::vector <TH1F*> Zn_hists;
  Zn_hists.resize(nb_sig);

  std::vector <TH1F*> ZnRel_hists;
  ZnRel_hists.resize(nb_sig);

  TH1F *totB_hists = new TH1F("totB_hists",";"+axis_label+";Events",nbins,xmin,xmax);
  totB_hists->Sumw2();

  TH1F *totB_cutflow = new TH1F("totB_cutflow",";"+axis_label+";Events",1,-100000.,-100000.);
  totB_cutflow->Sumw2();
  
  THStack *bkg_stack = new THStack("bkg_stack","");

  std::cout << "The cuts are: " << optCuts << std::endl;

  //loop bkgs
  for (int ibkg = 0; ibkg < nb_bkg; ibkg++){
    //improve with TChain

    bkg_files[ibkg] = TFile::Open(bkg_ntuples[ibkg],"");

    bkg_trees[ibkg] = (TTree*) bkg_files[ibkg]->Get("physics");

    bkg_trees[ibkg]->Draw(Form(optVar+">>bkg%i(%i,%f,%f)",ibkg,nbins,xmin,xmax),""+evtWeight+"*("+optCuts+")","e");

    bkg_hists[ibkg] = (TH1F*)gDirectory->Get(Form("bkg%i",ibkg));

    setHistStyle(bkg_hists[ibkg],bkg_color[ibkg],1,1.5,1);

    bkg_trees[ibkg]->Draw(Form(optVar+">>bkg_cutflow%i(%i,%f,%f)",ibkg,1,-100000.,100000.),""+evtWeight+"*("+optCuts+")","e");
    bkg_cutflow[ibkg] = (TH1F*)gDirectory->Get(Form("bkg_cutflow%i",ibkg));

    totB_hists->Add(bkg_hists[ibkg]);
    totB_cutflow->Add(bkg_cutflow[ibkg]);

  }

  std::vector<std::pair <float, int> > sortingPairBkg;
  sortingPairBkg.resize(nb_bkg);

  for (int ibkg = 0; ibkg < nb_bkg; ibkg++){
    sortingPairBkg[ibkg] = (make_pair(bkg_hists[ibkg]->Integral(-1,-1),ibkg));
  }

  std::sort(sortingPairBkg.begin(), sortingPairBkg.end());

  for (int ibkg = 0; ibkg < nb_bkg; ibkg++){
    bkg_stack->Add(bkg_hists[sortingPairBkg[ibkg].second]);
  }

  setHistStyle(totB_hists,0,kRed,2.0,1);

  for (int isig = 0; isig < nb_sig; isig++){
    //improve with TChain

    sig_files[isig] = TFile::Open(sig_ntuples[isig],"");
    sig_trees[isig] = (TTree*) sig_files[isig]->Get("physics");

    sig_trees[isig]->Draw(Form(optVar+">>sig%i(%i,%f,%f)",isig,nbins,xmin,xmax),""+evtWeight+"*("+optCuts+")","e");
    sig_hists[isig] = (TH1F*)gDirectory->Get(Form("sig%i",isig));

    setHistStyle(sig_hists[isig],0,sig_color[isig],2.,7);//isig+2

    Zn_hists[isig] = new TH1F(Form("Zn_hists_%i",isig),";"+axis_label+";Z_{n}",nbins,xmin,xmax);
    ZnRel_hists[isig] = new TH1F(Form("ZnRel_hists_%i",isig),";"+axis_label+";Z_{n} variation",nbins,xmin,xmax);

    fillHistZn(Zn_hists[isig], ZnRel_hists[isig], sig_hists[isig], totB_hists, totB_RelUnc, doLowerCut);

    setHistStyle(Zn_hists[isig],0,sig_color[isig],2.,1);
    setHistStyle(ZnRel_hists[isig],0,sig_color[isig],2.,1);

    sig_trees[isig]->Draw(Form(optVar+">>sig_cutflow%i(%i,%f,%f)",isig,1,-100000.,100000.),""+evtWeight+"*("+optCuts+")","e");
    sig_cutflow[isig] = (TH1F*)gDirectory->Get(Form("sig_cutflow%i",isig));

  }

  TCanvas* mycanvas = new TCanvas("mycanvas","mycanvas",0,0,800,800);
  mycanvas->cd();
  
  TPad *mycanvas_1 = new TPad("mycanvas_1", "newpad",0.,0.0,1.0,0.3);
  mycanvas_1->Draw();
  mycanvas_1->cd();
  mycanvas_1->SetTopMargin(0.03);
  mycanvas_1->SetRightMargin(0.05);
  mycanvas_1->SetBottomMargin(0.45);
  mycanvas_1->SetLeftMargin(0.16);
  //mycanvas_1->SetGridy(1);
  mycanvas_1->SetLogy(!doRelImprovement);

  std::vector<float> maxSignif;
  maxSignif.resize(nb_sig);

  std::vector<float> maxSignifCut;
  maxSignifCut.resize(nb_sig);
  
  float mymax = 0.5;
  
  for (int isig = 0; isig < nb_sig; isig++){
    maxSignif[isig] = Zn_hists[isig]->GetBinContent(Zn_hists[isig]->GetMaximumBin());
    if(doLowerCut) maxSignifCut[isig] = Zn_hists[isig]->GetBinLowEdge(Zn_hists[isig]->GetMaximumBin());  
    else maxSignifCut[isig] = Zn_hists[isig]->GetBinLowEdge(Zn_hists[isig]->GetMaximumBin()) +  Zn_hists[isig]->GetBinWidth(Zn_hists[isig]->GetMaximumBin());  

    float currMax = Zn_hists[isig]->GetBinContent(Zn_hists[isig]->GetMaximumBin());
    if(doRelImprovement) currMax = ZnRel_hists[isig]->GetBinContent(ZnRel_hists[isig]->GetMaximumBin());
    if(currMax>mymax) mymax=currMax;
  }

  if(doRelImprovement){
    ZnRel_hists[0]->GetXaxis()->SetTitle(axis_label);
    ZnRel_hists[0]->GetXaxis()->SetTitleSize(0.16);
    ZnRel_hists[0]->GetXaxis()->SetTitleOffset(1.1);
    ZnRel_hists[0]->GetXaxis()->SetLabelSize(0.16);
    ZnRel_hists[0]->GetXaxis()->SetTickLength(0.07);
    ZnRel_hists[0]->GetYaxis()->SetTitle("Z_{n} variation");
    ZnRel_hists[0]->GetYaxis()->SetTitleSize(0.16);
    ZnRel_hists[0]->GetYaxis()->SetTitleOffset(0.42);
    ZnRel_hists[0]->GetYaxis()->SetLabelSize(0.16);
    ZnRel_hists[0]->GetYaxis()->SetTickLength(0.02);
    ZnRel_hists[0]->GetYaxis()->SetNdivisions(4,5,0, true);
    ZnRel_hists[0]->SetMaximum(2.0* mymax );
    ZnRel_hists[0]->SetMinimum(-1.0);
    ZnRel_hists[0]->GetYaxis()->SetLabelOffset(0.01);
    ZnRel_hists[0]->GetXaxis()->SetRangeUser(xmin,xmax);
    ZnRel_hists[0]->Draw("hist");
    for (int isig = 1; isig < nb_sig; isig++){
      ZnRel_hists[isig]->Draw("histsames");
    }
  } else {
    Zn_hists[0]->GetXaxis()->SetTitle(axis_label);
    Zn_hists[0]->GetXaxis()->SetTitleSize(0.16);
    Zn_hists[0]->GetXaxis()->SetTitleOffset(1.1);
    Zn_hists[0]->GetXaxis()->SetLabelSize(0.16);
    Zn_hists[0]->GetXaxis()->SetTickLength(0.07);
    Zn_hists[0]->GetYaxis()->SetTitle("Z_{n}");
    Zn_hists[0]->GetYaxis()->SetTitleSize(0.16);
    Zn_hists[0]->GetYaxis()->SetTitleOffset(0.42);
    Zn_hists[0]->GetYaxis()->SetLabelSize(0.16);
    Zn_hists[0]->GetYaxis()->SetTickLength(0.02);
    Zn_hists[0]->GetYaxis()->SetNdivisions(4,5,0, true);
    Zn_hists[0]->SetMaximum(2.0);
    Zn_hists[0]->SetMinimum(0.0009);
    Zn_hists[0]->GetYaxis()->SetLabelOffset(-0.002);
    Zn_hists[0]->GetXaxis()->SetRangeUser(xmin,xmax);

    Zn_hists[0]->Draw("hist");
    for (int isig = 1; isig < nb_sig; isig++){
      Zn_hists[isig]->Draw("histsames");
    }
  }

  TLine *line = new TLine(xmin,0,xmax,0);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->SetLineStyle(2);
  if(doRelImprovement) line->Draw();

  for (int isig = 0; isig < nb_sig; isig++){
    myText(0.81,0.88-0.1*isig,sig_color[isig],Form("(%4.2f,%4.2f)",maxSignifCut[isig],maxSignif[isig]),0.09);
  }  

  mycanvas->cd();

  TPad *mycanvas_2 = new TPad("mycanvas_2", "newpad",0.0,0.3,1.0,1.0);
  mycanvas_2->Draw();
  mycanvas_2->cd();
  mycanvas_2->SetTopMargin(0.08);
  mycanvas_2->SetRightMargin(0.05);
  mycanvas_2->SetBottomMargin(0.01);
  mycanvas_2->SetLeftMargin(0.16);
  mycanvas_2->SetLogy(dology);

  bkg_stack->Draw("hist");

  bkg_stack->GetYaxis()->SetTitle(Form("Events / %4.2f ", ((xmax-xmin)/float(nbins)) )+unit);//here 
  bkg_stack->GetYaxis()->SetTitleSize(0.07);
  bkg_stack->GetYaxis()->SetTitleOffset(1.0);
  bkg_stack->GetYaxis()->SetLabelSize(0.07);
  bkg_stack->GetXaxis()->SetLabelOffset(1.5);
  bkg_stack->GetXaxis()->SetRangeUser(xmin,xmax);
  if(!dology){
    bkg_stack->SetMinimum(0.0001);
    bkg_stack->SetMaximum(1.2*(totB_hists->GetBinContent(totB_hists->GetMaximumBin())+totB_hists->GetBinError(totB_hists->GetMaximumBin())));
  }
  if(dology){
    bkg_stack->SetMinimum(0.01);
    bkg_stack->SetMaximum(100*(totB_hists->GetBinContent(totB_hists->GetMaximumBin())+totB_hists->GetBinError(totB_hists->GetMaximumBin())));
  }

  //stat errors
  TGraphAsymmErrors *bkg_StatErrors = new TGraphAsymmErrors(nbins);
  for(int i=1; i <=nbins; i++){
    bkg_StatErrors->SetPoint(i,totB_hists->GetBinCenter(i),totB_hists->GetBinContent(i));
    bkg_StatErrors->SetPointError(i,totB_hists->GetBinWidth(i)/2.,totB_hists->GetBinWidth(i)/2.,sqrt(pow(totB_hists->GetBinError(i),2)),sqrt(pow(totB_hists->GetBinError(i),2)));
  }

  bkg_StatErrors->SetFillColor(12);
  bkg_StatErrors->SetFillStyle(3254);
  bkg_StatErrors->SetLineColor(kRed);
  bkg_StatErrors->SetLineWidth(2);
  bkg_StatErrors->Draw("2");

  totB_hists->Draw("histsames");

  for (int isig = 0; isig < nb_sig; isig++){
    sig_hists[isig]->Draw("histsames");
  }
  
  TLegend *leg1 = new TLegend(0.55,0.89-0.035*(nb_bkg),0.70,0.89);
  leg1->SetBorderSize(0);
  leg1->SetTextSize(0.035);
  leg1->SetFillStyle(0);
  //leg1->AddEntry(data,"Data 2011", "p");
  for (int ibkg = 0; ibkg < nb_bkg; ibkg++){
    leg1->AddEntry(bkg_hists[sortingPairBkg[nb_bkg - 1 - ibkg].second],bkg_names[sortingPairBkg[nb_bkg - 1 - ibkg].second], "f");
  }
  leg1->Draw();

  TLegend *leg2 = new TLegend(0.70,0.89-0.035*(nb_sig+1),0.85,0.89);
  leg2->SetBorderSize(0);
  leg2->SetTextSize(0.035);
  leg2->SetFillStyle(0);
  leg2->AddEntry(bkg_StatErrors,"SM (stat)","lf");
  for (int isig = 0; isig < nb_sig; isig++){
    leg2->AddEntry(sig_hists[isig],sig_names[isig], "f");
  }
  leg2->Draw();

  //data->Draw("EPSAMES");
  
  ATLAS_LABEL(0.2,0.87,1);
  myText(0.3,0.87,1,"Internal");
  myText(0.2,0.81,1,"#sqrt{s} = 13 TeV, #lower[-0.15]{#scale[0.7]{#int}} L dt = 10 fb^{-1}");
  myText(0.2,0.75,1,"#tilde{#chi}^{#pm}_{1} #tilde{#chi}^{0}_{2} #rightarrow W^{(*)} #tilde{#chi}^{0}_{1} Z^{(*)} #tilde{#chi}^{0}_{1}");

  mycanvas->cd();
  mycanvas->Modified();

  TString saveName= optVar;
  if(doLowerCut)saveName += "_lowerCut";
  else saveName += "_upperCut";
  if(do0j) saveName += "_0j";
  else saveName += "_ISR";
  saveName +="_"+optCuts;
  //gSystem->Exec(Form("mkdir CutLevel%i",optCutLevel));

  saveName=saveName.ReplaceAll("(NJets>=1)&&(nBjets==0)","");
  saveName=saveName.ReplaceAll("(NJets==0)&&(nBjets==0)","");
  saveName=saveName.ReplaceAll("/","_over_");
  saveName=saveName.ReplaceAll("(","");
  saveName=saveName.ReplaceAll(")","");
  saveName=saveName.ReplaceAll("&&","_");
  saveName=saveName.ReplaceAll("<=","LE");
  saveName=saveName.ReplaceAll(">=","GE");
  saveName=saveName.ReplaceAll("<","LT");
  saveName=saveName.ReplaceAll(">","GT");
  saveName=saveName.ReplaceAll("==","EQ");
  saveName=saveName.ReplaceAll("!=","NE");

  

  if(dology) saveName+="_logy";
  
  mycanvas->SaveAs(saveName+".pdf");

  for (int isig = 0; isig < nb_sig-1; isig++){
  
    std::cout << sig_names[isig] << ": " << std::fixed << std::setprecision(0) << sig_cutflow[isig]->GetBinContent(1) << " +/- " << sig_cutflow[isig]->GetBinError(1) << std::endl;

  }

  for (int ibkg = 0; ibkg < nb_bkg; ibkg++){
  
    std::cout << bkg_names[ibkg] << ": " << std::fixed << std::setprecision(0) << bkg_cutflow[ibkg]->GetBinContent(1) << " +/- " << bkg_cutflow[ibkg]->GetBinError(1) << std::endl;

  }

  using namespace RooStats::NumberCountingUtils;

  std::cout << "Tot. Bkg." << ": " << std::fixed << std::setprecision(0) << totB_cutflow->GetBinContent(1) << " +/- " << totB_cutflow->GetBinError(1) << std::endl;
  std::cout << "Zn" << ": " << std::setprecision(2) << BinomialObsZ(sig_cutflow[0]->GetBinContent(1)+totB_cutflow->GetBinContent(1), totB_cutflow->GetBinContent(1), totB_RelUnc) << std::endl;


}

