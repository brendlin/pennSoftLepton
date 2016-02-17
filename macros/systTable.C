double addInQuad(double num1, double num2)
{
  return TMath::Sqrt(num1*num1 + num2*num2);
}

void systTable(TString outFolder = "SMWZ_AllSys_Run1") {

  gStyle->SetOptStat(0000);
  gStyle->SetOptTitle(0); //this will disable the title for all coming histograms
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  TString evtWeight="TotalWeightNoSF*lepSFID[lep0_index]*lepSFReco[lep0_index]*lepSFIso[lep0_index]*lepSFID[lep1_index]*lepSFReco[lep1_index]*lepSFIso[lep1_index]*lepSFID[lep2_index]*lepSFReco[lep2_index]*lepSFIso[lep2_index]";

  std::vector<TString> systVar;
  systVar.push_back("NOMINAL");
  systVar.push_back("EG_RESOLUTION_ALL__1down");
  systVar.push_back("EG_RESOLUTION_ALL__1up");
  systVar.push_back("EG_SCALE_ALL__1down");
  systVar.push_back("EG_SCALE_ALL__1up");
  systVar.push_back("EL_EFF_ID_TotalCorrUncertainty__1down");
  systVar.push_back("EL_EFF_ID_TotalCorrUncertainty__1up");
  systVar.push_back("EL_EFF_Iso_TotalCorrUncertainty__1down");
  systVar.push_back("EL_EFF_Iso_TotalCorrUncertainty__1up");
  systVar.push_back("EL_EFF_Reco_TotalCorrUncertainty__1down");
  systVar.push_back("EL_EFF_Reco_TotalCorrUncertainty__1up");
  //systVar.push_back("EL_EFF_Trigger_TotalCorrUncertainty__1down");
  //systVar.push_back("EL_EFF_Trigger_TotalCorrUncertainty__1up");
  //systVar.push_back("JET_BJES_Response__1up");
  //systVar.push_back("JET_BJES_Response__1down");
  //systVar.push_back("JET_EffectiveNP_1__1up");
  //systVar.push_back("JET_EffectiveNP_1__1down");
  //systVar.push_back("JET_EffectiveNP_2__1up");
  //systVar.push_back("JET_EffectiveNP_2__1down");
  //systVar.push_back("JET_EffectiveNP_3__1up");
  //systVar.push_back("JET_EffectiveNP_3__1down");
  //systVar.push_back("JET_EffectiveNP_4__1up");
  //systVar.push_back("JET_EffectiveNP_4__1down");
  //systVar.push_back("JET_EffectiveNP_5__1up");
  //systVar.push_back("JET_EffectiveNP_5__1down");
  //systVar.push_back("JET_EffectiveNP_6restTerm__1up");
  //systVar.push_back("JET_EffectiveNP_6restTerm__1down");
  //systVar.push_back("JET_EtaIntercalibration_Modelling__1up");
  //systVar.push_back("JET_EtaIntercalibration_Modelling__1down");
  //systVar.push_back("JET_EtaIntercalibration_TotalStat__1up");
  //systVar.push_back("JET_EtaIntercalibration_TotalStat__1down");
  //systVar.push_back("JET_Flavor_Composition__1up");
  //systVar.push_back("JET_Flavor_Composition__1down");
  //systVar.push_back("JET_Flavor_Response__1up");
  //systVar.push_back("JET_Flavor_Response__1down");
  //systVar.push_back("JET_GroupedNP_1__1up");
  //systVar.push_back("JET_GroupedNP_1__1down");
  //systVar.push_back("JET_JER_SINGLE_NP__1up");
  //systVar.push_back("JET_Pileup_OffsetMu__1up");
  //systVar.push_back("JET_Pileup_OffsetMu__1down");
  //systVar.push_back("JET_Pileup_OffsetNPV__1up");
  //systVar.push_back("JET_Pileup_OffsetNPV__1down");
  //systVar.push_back("JET_Pileup_PtTerm__1up");
  //systVar.push_back("JET_Pileup_PtTerm__1down");
  //systVar.push_back("JET_Pileup_RhoTopology__1up");
  //systVar.push_back("JET_Pileup_RhoTopology__1down");
  //systVar.push_back("JET_PunchThrough_MC15__1up");
  //systVar.push_back("JET_PunchThrough_MC15__1down");
  //systVar.push_back("JET_SingleParticle_HighPt__1up");
  //systVar.push_back("JET_SingleParticle_HighPt__1down");
  //systVar.push_back("MET_SoftTrk_ResoPara");
  //systVar.push_back("MET_SoftTrk_ResoPerp");
  //systVar.push_back("MET_SoftTrk_ScaleDown");
  //systVar.push_back("MET_SoftTrk_ScaleUp");
  //systVar.push_back("MUONS_ID__1down");
  //systVar.push_back("MUONS_ID__1up");
  //systVar.push_back("MUONS_MS__1down");
  //systVar.push_back("MUONS_MS__1up");
  //systVar.push_back("MUONS_SCALE__1down");
  //systVar.push_back("MUONS_SCALE__1up");
  //systVar.push_back("MUON_EFF_STAT__1down");
  //systVar.push_back("MUON_EFF_STAT__1up");
  //systVar.push_back("MUON_EFF_SYS__1down");
  //systVar.push_back("MUON_EFF_SYS__1up");
  //systVar.push_back("MUON_EFF_TrigStatUncertainty__1down");
  //systVar.push_back("MUON_EFF_TrigStatUncertainty__1up");
  //systVar.push_back("MUON_EFF_TrigSystUncertainty__1down");
  //systVar.push_back("MUON_EFF_TrigSystUncertainty__1up");
  //systVar.push_back("MUON_ISO_STAT__1down");
  //systVar.push_back("MUON_ISO_STAT__1up");
  //systVar.push_back("MUON_ISO_SYS__1down");
  //systVar.push_back("MUON_ISO_SYS__1up");
  //systVar.push_back("PRW_DATASF__1down");
  //systVar.push_back("PRW_DATASF__1up");

  int nbSyst = systVar.size();

  std::vector<TString> flavExp;
  flavExp.push_back("(ChanFlavor==7)");
  flavExp.push_back("(ChanFlavor==8)");
  flavExp.push_back("(ChanFlavor==9)");
  flavExp.push_back("(ChanFlavor==10)");
  flavExp.push_back("(GRL>0)");

  int nbFlav = flavExp.size();

  std::vector<TString> flavour;
  flavour.push_back("eee");
  flavour.push_back("eem");
  flavour.push_back("mme");
  flavour.push_back("mmm");
  flavour.push_back("all");

  std::vector<TFile*> files;
  files.resize(nbSyst);
  std::vector<TTree*> trees;
  trees.resize(nbSyst);

  std::vector<std::vector<TH1F*>> hists;
  hists.resize(nbSyst);

  std::vector<std::vector<double>> yieldChange;
  yieldChange.resize(nbSyst);

  std::vector<std::pair<double,double>> totalElectron;
  totalElectron.resize(nbFlav);
  std::vector<std::pair<double,double>> totalMuon;
  totalMuon.resize(nbFlav);
  std::vector<std::pair<double,double>> totalJet;
  totalJet.resize(nbFlav);
  std::vector<std::pair<double,double>> totalMet;
  totalMet.resize(nbFlav);
  std::vector<std::pair<double,double>> totalPrw;
  totalPrw.resize(nbFlav);

  TString elLatex = "Total electron ";
  TString muLatex = "Total muon ";
  TString jetLatex = "Total jet ";
  TString metLatex = "Total MET ";
  TString prwLatex = "Total pile-up ";

  std::vector<TString> latexString;
  latexString.resize(nbSyst);

  for (int iSyst = 0; iSyst < nbSyst; iSyst++){

    files[iSyst] = TFile::Open(outFolder+"_"+systVar[iSyst]+"/fetch/data-SKIM/wz_361601.root" ,"");
    trees[iSyst] = (TTree*) files[iSyst]->Get("physics");

    hists[iSyst].resize(nbFlav);
    yieldChange[iSyst].resize(nbFlav);

    for(int iFlav = 0; iFlav < nbFlav; iFlav++){

      trees[iSyst]->Draw(Form("GRL>>"+flavour[iFlav]+"_%i(3,0,3)",iSyst),""+flavExp[iFlav]+"*("+evtWeight+")","e");
      hists[iSyst][iFlav] = (TH1F*)gDirectory->Get(Form(""+flavour[iFlav]+"_%i",iSyst));
      yieldChange[iSyst][iFlav] = (( hists[iSyst][iFlav]->GetBinContent(2) - hists[0][iFlav]->GetBinContent(2) ) / (hists[0][iFlav]->GetBinContent(2)) )*100.;

      if (iSyst == 0) std::cout << "Total WZ yield " << flavour[iFlav] << "\t" << hists[iSyst][iFlav]->GetBinContent(2) << " +/- " << hists[iSyst][iFlav]->GetBinError(2) << std::endl;

      if( (systVar[iSyst].Contains("EG_") || systVar[iSyst].Contains("EL_")) && systVar[iSyst].Contains("__1up")) {
        totalElectron[iFlav].first = addInQuad( totalElectron[iFlav].first, yieldChange[iSyst][iFlav] );
      } else if( (systVar[iSyst].Contains("EG_") || systVar[iSyst].Contains("EL_")) && systVar[iSyst].Contains("__1down")) {
        totalElectron[iFlav].second = addInQuad( totalElectron[iFlav].second, yieldChange[iSyst][iFlav] );
      } else if( systVar[iSyst].Contains("JET_") && systVar[iSyst].Contains("__1up")) {
        totalJet[iFlav].first = addInQuad( totalJet[iFlav].first, yieldChange[iSyst][iFlav] );
      } else if( systVar[iSyst].Contains("JET_") && systVar[iSyst].Contains("__1down")) {
        totalJet[iFlav].second = addInQuad( totalJet[iFlav].second, yieldChange[iSyst][iFlav] );
      } else if( systVar[iSyst].Contains("MET_") && ( systVar[iSyst].Contains("up") || systVar[iSyst].Contains("para") || systVar[iSyst].Contains("perp")) ){
        totalMet[iFlav].first = addInQuad( totalMet[iFlav].first, yieldChange[iSyst][iFlav] );
      } else if( systVar[iSyst].Contains("MET_") && systVar[iSyst].Contains("down")) {
        totalMet[iFlav].second = addInQuad( totalMet[iFlav].second, yieldChange[iSyst][iFlav] );
      } else if( systVar[iSyst].Contains("MUON_") && systVar[iSyst].Contains("__1up")) {
        totalMuon[iFlav].first = addInQuad( totalMuon[iFlav].first, yieldChange[iSyst][iFlav] );
      } else if( systVar[iSyst].Contains("MUON_") && systVar[iSyst].Contains("__1down")) {
        totalMuon[iFlav].second = addInQuad( totalMuon[iFlav].second, yieldChange[iSyst][iFlav] );
      } else if( systVar[iSyst].Contains("PRW_") && systVar[iSyst].Contains("__1up")) {
        totalPrw[iFlav].first = addInQuad( totalPrw[iFlav].first, yieldChange[iSyst][iFlav] );
      } else if( systVar[iSyst].Contains("PRW_") && systVar[iSyst].Contains("__1down")) {
        totalPrw[iFlav].second = addInQuad( totalPrw[iFlav].second, yieldChange[iSyst][iFlav] );
      }

      latexString[iSyst] = Form(""+latexString[iSyst]+" & %4.2f",yieldChange[iSyst][iFlav]);

    }//loop flavors

  }//loop systs

  for(int iFlav = 0; iFlav < nbFlav; iFlav++){
    elLatex = elLatex+Form("& $^{+%4.2f}_{-%4.2f}$ ",totalElectron[iFlav].first,totalElectron[iFlav].second);
    muLatex = muLatex+Form("& $^{+%4.2f}_{-%4.2f}$ ",totalMuon[iFlav].first,totalMuon[iFlav].second);
    jetLatex = jetLatex+Form("& $^{+%4.2f}_{-%4.2f}$ ",totalJet[iFlav].first,totalJet[iFlav].second);
    metLatex = metLatex+Form("& $^{+%4.2f}_{-%4.2f}$ ",totalMet[iFlav].first,totalMet[iFlav].second);
    prwLatex = prwLatex+Form("& $^{+%4.2f}_{-%4.2f}$ ",totalPrw[iFlav].first,totalPrw[iFlav].second);
  }

  //make electron table
  std::ofstream electronOutput;
  electronOutput.open((outFolder+"_electron.tex").Data());
  std::ofstream muonOutput;
  muonOutput.open((outFolder+"_muon.tex").Data());
  std::ofstream jetOutput;
  jetOutput.open((outFolder+"_jet.tex").Data());
  std::ofstream metOutput;
  metOutput.open((outFolder+"_met.tex").Data());
  std::ofstream prwOutput;
  prwOutput.open((outFolder+"_prw.tex").Data());

  std::string header = "\\resizebox{\\linewidth}{!}{\n\\begin{tabular}{l c c c c c}\n\\toprule\n Systematic variation & $eee$ & $ee\\mu$ & $\\mu\\mu e$ & $\\mu\\mu\\mu$ & all \\\\\\midrule";

  electronOutput <<  header << std::endl;
  muonOutput <<  header << std::endl;
  jetOutput <<  header << std::endl;
  metOutput <<  header << std::endl;
  prwOutput <<  header << std::endl;

  for (int iSyst = 0; iSyst < nbSyst; iSyst++){
    if( (systVar[iSyst].Contains("EG_") || systVar[iSyst].Contains("EL_")) ) {
      electronOutput << systVar[iSyst].ReplaceAll("_","\\_") <<  latexString[iSyst] << "\\\\\n";
    } else if (systVar[iSyst].Contains("JET_")){
      jetOutput << systVar[iSyst].ReplaceAll("_","\\_") <<  latexString[iSyst] << "\\\\\n";
    } else if (systVar[iSyst].Contains("MUON_")){
      muonOutput << systVar[iSyst].ReplaceAll("_","\\_") <<  latexString[iSyst] << "\\\\\n";
    } else if (systVar[iSyst].Contains("MET_")){
      metOutput << systVar[iSyst].ReplaceAll("_","\\_") <<  latexString[iSyst] << "\\\\\n";
    } else if (systVar[iSyst].Contains("PRW_")){
      prwOutput << systVar[iSyst].ReplaceAll("_","\\_") <<  latexString[iSyst] << "\\\\\n";
    }

  }
  
  electronOutput << "\\midrule\n" << elLatex << "\\\\\n\\bottomrule\n \\end{tabular}}" << std::endl;
  electronOutput.close();

  muonOutput << "\\midrule\n" << muLatex << "\\\\\n\\bottomrule\n \\end{tabular}}" << std::endl;
  muonOutput.close();

  jetOutput << "\\midrule\n" << jetLatex << "\\\\\n\\bottomrule\n \\end{tabular}}" << std::endl;
  jetOutput.close();

  metOutput << "\\midrule\n" << metLatex << "\\\\\n\\bottomrule\n \\end{tabular}}" << std::endl;
  metOutput.close();

  prwOutput << "\\midrule\n" << prwLatex << "\\\\\n\\bottomrule\n \\end{tabular}}" << std::endl;
  prwOutput.close();

}

