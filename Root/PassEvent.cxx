#include <pennSoftLepton/PassEvent.h>
//#include <EventLoop/Worker.h>

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::PassEvent)

//-----------------------------------------------------------------------------
PSL::PassEvent::PassEvent() : AlgBase()
{
  MSG_DEBUG("Constructor");
}

//-----------------------------------------------------------------------------
bool PSL::PassEvent::init(void)
{
  MSG_DEBUG("PassEvent algbase name is " << configured_name);

  // connect to cut algorithms
  if (!Cuts.size()) Cuts.push_back("Start");
  for (unsigned int i=0;i<Cuts.size();++i){
    CutItem* tmp = (CutItem*)connectToAnotherAlgorithm(Cuts[i]);
    if (!tmp){
      MSG_INFO("Error! Could not find CutItem " << Cuts[i] << ". Exiting.");
      return false;
    }
    CutItemPointers.push_back(tmp);
    MSG_DEBUG("Successfully connected " << Cuts[i] << " to " << GetName() << ".");
  }
  if (!ParentEventSelectorName.empty()){
    ParentEventSelector = (PassEvent*)connectToAnotherAlgorithm(ParentEventSelectorName);
    if (!ParentEventSelector) {
      MSG_INFO("Error! Could not find parent PassEvent " << ParentEventSelectorName << ". Exiting");
      return false;
    }
    MSG_DEBUG("Successfully connected " << ParentEventSelectorName << " to " << GetName() << ".");
  }

  //
  // even if there are no cuts, we need one dummy bin.
  // Likewise, we want an "all" bin for samples
  // Yah ok this is sort-of done twice. It is ok though.
  //
  bool has_all = false;
  bool has_data = false;
  for(unsigned int i=0;i<Samples.size();i++){
    if (Samples[i] == kall) has_all = true;
    if (Samples[i] == kdata) has_data = true;
  }
  if(!has_all) Samples.push_back(kall);
  if(!has_data) Samples.push_back(kdata);

  // Set up cut-sample matrix 
  m_CountsByCutAndSample.clear();
  for (unsigned int i=0;i<Cuts.size();++i){
    std::vector<EventCounter> ecvec;
    for (unsigned int j=0;j<Samples.size();++j){
      EventCounter tmp = {0,0,0};
      ecvec.push_back(tmp);
    }
    m_CountsByCutAndSample.push_back(ecvec);
  }

  // Unless otherwise specified, save all of the variables from Variables.h
  if (!m_variables.size()) m_variables = GetAllVariables();

  // Set up variable histograms by sample
  m_VariablesBySample.clear();
  std::string name;
  for (unsigned int i=0;i<Samples.size();++i){
    std::vector<TH1*> varvec;
    for (unsigned int j=0;j<m_variables.size();++j){
      name = GetHistogramLocation("PassEvent",GetName(),Samples[i],m_variables[j]);
      TH1F* tmphist = MakeVariableTH1F(name,m_variables[j]);
      //tmphist->SetDirectory(0); // done in MakeVariableTH1F
      //tmphist->Sumw2(); // done in MakeVariableTH1F
      //tmphist->GetXaxis()->SetRange(0,tmphist->GetNbinsX()+1);
      varvec.push_back(tmphist);
      // Make sure (in EventLoop) that the new call and the setHistogramOutput bit are in the same step
      //setHistogramOutput(tmphist);
    }
    m_VariablesBySample.push_back(varvec);
  }

  // Set up LepVariable histograms by sample
  m_LepVariablesBySample.clear();
  m_lepindices.clear();
  m_lepindices.push_back(vlep0_index             );
  m_lepindices.push_back(vlep1_index             );
  m_lepindices.push_back(vlep2_index             );
  m_lepindices.push_back(vlepleadZ_index         );
  m_lepindices.push_back(vlepsubleadZ_index      );
  m_lepindices.push_back(vlepW_index             );
  m_lepindices.push_back(vlepleadMinSFOS_index   );
  m_lepindices.push_back(vlepsubleadMinSFOS_index);
  m_lepindices.push_back(vlepWMinSFOS_index      );
  m_lepindices.push_back(vantilep0_index         );

  if (!m_lepvariables.size()) m_lepvariables = GetAllLepVariables();

  for (unsigned int i=0;i<Samples.size();++i){
    std::vector<std::vector<TH1*> > lepvarvec_1deep;
    for (unsigned int j=0;j<m_lepvariables.size();++j){
      std::vector<TH1*> lepvarvec_2deep;
      for (unsigned int k=0;k<m_lepindices.size();++k){
        name = GetHistogramLocation("PassEvent",GetName()
                                    ,Samples[i],m_lepvariables[j],m_lepindices[k]);
        TH1F* tmphist = MakeVariableTH1F(name,m_lepvariables[j]);
        tmphist->SetDirectory(0);
        //tmphist->Sumw2(); // YES CALL THIS
        lepvarvec_2deep.push_back(tmphist);
      }
      lepvarvec_1deep.push_back(lepvarvec_2deep);
    }
    m_LepVariablesBySample.push_back(lepvarvec_1deep);
  }

  PrintConfiguration();
  return true;
}

//-----------------------------------------------------------------------------
void PSL::PassEvent::loop(void){
  MSG_DEBUG("loop");
  m_currenteventnumber = m_EDM->get_EventNumber();
  m_eventpassed = 0;
  bool pass_all = true;
  if (ParentEventSelector){
    // Hacky thing to make sure the algorithms are talking about the same event.
    if (!ParentEventSelector->passed(m_EDM->get_EventNumber())) return;
  }
  double weight = m_EDM->GetEventWeight();
  for (unsigned int i=0;i<CutItemPointers.size();++i){
    if (!CutItemPointers[i]->pass()){
      MSG_DEBUG("Failed");
      pass_all = false;
    }
    else {
      MSG_DEBUG("Passed");
    }

    //
    // Break out if it fails
    //
    if (!pass_all) break;

    //
    // Fill with weights
    //
    for (unsigned int j=0;j<Samples.size();++j){
      if (!MatchSample(Samples[j],m_EDM->m_CurrentSample)) continue;
      m_CountsByCutAndSample[i][j].PassN  += 1;
      m_CountsByCutAndSample[i][j].PassW  += weight;
      m_CountsByCutAndSample[i][j].Passw2 += weight*weight;
    }
  }
  if (pass_all) {
    FillVariableHistograms(weight);
    FillLepVariableHistograms(weight);
    // major hack here (keep this line though)
    // if (std::string(GetName()) == "eventcleaning3l") ((XSUSYObjDefAlgExpert*)(wk()->getAlg("susytools_alg")))->CountOR();
    m_eventpassed = 1;
  }
  MSG_DEBUG("loop done");
}

//-----------------------------------------------------------------------------
void PSL::PassEvent::finish(void){
  MSG_DEBUG("finish.");
  SaveCountsByCutAndSampleToHist();
  //
  // Only save a histogram if it has entries
  //
  for (unsigned int i=0;i<Samples.size();++i){
    for (unsigned int j=0;j<m_variables.size();++j){
      // GetSumw2N is for whether Sumw2 has been called.
      // We set the range already to include first and last bins
      SaveIfNotEmpty(m_VariablesBySample[i][j]);
      delete m_VariablesBySample[i][j];
    }
  }

  for (unsigned int i=0;i<Samples.size();++i){
    for (unsigned int k=0;k<m_lepindices.size();++k){
      for (unsigned int j=0;j<m_lepvariables.size();++j){
        SaveIfNotEmpty(m_LepVariablesBySample[i][j][k]);
        delete m_LepVariablesBySample[i][j][k];
      }
    }
  }
  
}

//-----------------------------------------------------------------------------
void PSL::PassEvent::set_Cuts(std::string c){
  MSG_DEBUG("setting cuts to " << c);
  std::vector<std::string> tmp;
  Cuts.clear();
  PSL::StringTok(tmp,c," ");
  Cuts.push_back("Start");
  for(unsigned int i=0;i<tmp.size();i++){
    Cuts.push_back(tmp[i]);
    //MSG_INFO(" " << Cuts.at(i));
  }
}

//-----------------------------------------------------------------------------
std::vector<std::string> PSL::PassEvent::get_Samples(void){
  std::vector<std::string> tmp;
  for (unsigned int i=0;i<Samples.size();++i){
    tmp.push_back(ConvertSampleToStr(Samples[i]));
  }
  return tmp;
}

//-----------------------------------------------------------------------------
std::vector<std::vector<double> > PSL::PassEvent::get_CountsByCutAndSample(std::string type){
  std::vector<std::vector<double> > tmp;
  for (unsigned int i=0;i<Cuts.size();++i){
    tmp.push_back(std::vector<double>());
    for (unsigned int j=0;j<Samples.size();++j){
      if (type == "N") tmp[i].push_back(m_CountsByCutAndSample[i][j].PassN);
      if (type == "err") tmp[i].push_back(pow(m_CountsByCutAndSample[i][j].PassN,0.5));
      if (type == "W") tmp[i].push_back(m_CountsByCutAndSample[i][j].PassW);
      if (type == "Werr") tmp[i].push_back(pow(m_CountsByCutAndSample[i][j].Passw2,0.5));
    }
  }
  return tmp;
}

//-----------------------------------------------------------------------------
void PSL::PassEvent::SaveCountsByCutAndSampleToHist(void){
  std::string tmpname;
  tmpname = GetHistogramLocation("PassEvent",GetName(),"PassN");
  int ssize = Samples.size();
  int csize = Cuts.size();
  MSG_DEBUG(GetName() << " SaveCountsByCutAndSampleToHist: ssize: " << ssize << "  csize: " << csize);
  hist_PassN = new TH2F(tmpname.c_str(),tmpname.c_str(),ssize,0,ssize,csize,0,csize);
  tmpname = GetHistogramLocation("PassEvent",GetName(),"PassW");
  hist_PassW = new TH2F(tmpname.c_str(),tmpname.c_str(),ssize,0,ssize,csize,0,csize);
  for (unsigned int i=0;i<Samples.size();++i){
    std::string samplename = ConvertSampleToStr(Samples[i]);
    hist_PassN->GetXaxis()->SetBinLabel(i+1,samplename.c_str());
    hist_PassW->GetXaxis()->SetBinLabel(i+1,samplename.c_str());
    for (unsigned int j=0;j<Cuts.size();++j){
      if (i==0) hist_PassN->GetYaxis()->SetBinLabel(j+1,Cuts[j].c_str());
      if (i==0) hist_PassW->GetYaxis()->SetBinLabel(j+1,Cuts[j].c_str());
      hist_PassN->SetBinContent(i+1,j+1,m_CountsByCutAndSample[j][i].PassN);
      hist_PassN->SetBinError(i+1,j+1,pow(m_CountsByCutAndSample[j][i].PassN,0.5));
      hist_PassW->SetBinContent(i+1,j+1,m_CountsByCutAndSample[j][i].PassW);
      hist_PassW->SetBinError(i+1,j+1,pow(m_CountsByCutAndSample[j][i].Passw2,0.5));
    }
  }
  setHistogramOutput(hist_PassN);
  setHistogramOutput(hist_PassW);    
  MSG_DEBUG("Saved counts to histogram");
}

//-----------------------------------------------------------------------------
void PSL::PassEvent::FillVariableHistograms(double weight){
  MSG_DEBUG("FillVariableHistograms begin");
  //Sample s = m_EDM->m_CurrentSample;
  for (unsigned int i=0;i<Samples.size();++i){
    if (!MatchSample(Samples[i],m_EDM->m_CurrentSample)) 
      continue;
    for (unsigned int j=0;j<m_variables.size();++j){
      if (!m_EDM->VariableExists(m_variables[j])) continue;
      MSG_DEBUG("FillVariableHistograms: Filling " << 
               ConvertSampleToStr(Samples[i]) << " " << ConvertVarToStr(m_variables[j])
               << " with value " << m_EDM->GetEventVariable(m_variables[j]));
      m_VariablesBySample[i][j]->Fill(m_EDM->GetEventVariable(m_variables[j]),weight);
    }
  }
  MSG_DEBUG("FillVariableHistograms end");
}

//-----------------------------------------------------------------------------
void PSL::PassEvent::FillLepVariableHistograms(double weight){
  MSG_DEBUG("FillLepVariableHistograms begin");

  for (unsigned int i=0;i<Samples.size();++i){
    if (!MatchSample(Samples[i],m_EDM->m_CurrentSample)) 
      continue;
    for (unsigned int j=0;j<m_lepvariables.size();++j){
      for (unsigned int k=0;k<m_lepindices.size();++k){
        if (!m_EDM->VariableExists(m_lepindices[k])) continue;
        if (!m_EDM->LepVariableExists(m_lepvariables[j])) continue;
        double val = m_EDM->GetSignalLeptonVariable(m_lepvariables[j],m_lepindices[k]);
        MSG_DEBUG("FillVariableHistograms: Filling " << 
                  ConvertSampleToStr(Samples[i]) << " " << ConvertLepVarToStr(m_lepvariables[j])
                  << " index " << m_lepindices[k]
                  << " with value " << val);
        m_LepVariablesBySample[i][j][k]->Fill(val,weight);
      }
    }
  }
  MSG_DEBUG("FillLepVariableHistograms end");
}

//-----------------------------------------------------------------------------
void PSL::PassEvent::ReconnectHistograms(TFile* file){
  //
  // Variable histograms
  //
  std::string tmpname;
  m_VariablesBySample.clear();
  for (unsigned int i=0;i<Samples.size();++i){
    std::vector<TH1*> varvec;
    for (unsigned int j=0;j<GetAllVariables().size();++j){
      tmpname = GetHistogramLocation("PassEvent",GetName(),Samples[i],GetAllVariables()[j]);
      TH1F* tmphist = (TH1F*)file->Get(tmpname.c_str());
      if (!tmphist) MSG_DEBUG("Warning! Did not pick up " << tmpname);
      varvec.push_back(tmphist);
    }
    m_VariablesBySample.push_back(varvec);
  }
  //
  // Count histograms
  //
  tmpname = GetHistogramLocation("PassEvent",GetName(),"PassN");
  hist_PassN = (TH2F*)file->Get(tmpname.c_str());
  if (!hist_PassN) MSG_INFO("Error! Did not pick up " << tmpname);
  tmpname = GetHistogramLocation("PassEvent",GetName(),"PassW");
  hist_PassW = (TH2F*)file->Get(tmpname.c_str());
  if (!hist_PassW) MSG_INFO("Error! Did not pick up " << tmpname);
  return;
}

//-----------------------------------------------------------------------------
void PSL::PassEvent::SetOutgoingNtupleBranches(TTree* tree){
  std::string branchname = "PassEvent_"+std::string(GetName());
  tree->Branch(branchname.c_str(),&m_eventpassed,(branchname+"/I").c_str());
  return;
}

//-----------------------------------------------------------------------------
void PSL::PassEvent::PrintConfiguration(void){
  MSG_INFO("-------------------------------PassEvent Configuration:--------------------------------");
  MSG_INFO("This Instance Name      : " << GetName()              );
  MSG_INFO("ParentEventSelectorName : " << ParentEventSelectorName);
  for (unsigned int i=0;i<Cuts.size();++i){
    if (i == 0) {
      MSG_INFO("Cuts                    : " << Cuts[i]);
    }
    else MSG_INFO("                          " << Cuts[i]);
  }
  for (unsigned int i=0;i<Samples.size();++i){
    if (i == 0) {
      MSG_INFO("Samples                 : " << ConvertSampleToStr(Samples[i]));
    }
    else MSG_INFO("                          " << ConvertSampleToStr(Samples[i]));
  }
  std::string tmp;
  for (unsigned int i=0;i<m_variables.size();++i){
    tmp += ConvertVarToStr(m_variables[i]);
    tmp += " ";
  }
  MSG_INFO("Variables               : " << tmp                    );
  // now lep variables
  tmp = "";
  for (unsigned int i=0;i<m_lepvariables.size();++i){
    tmp += ConvertLepVarToStr(m_lepvariables[i]);
    tmp += " ";
  }
  MSG_INFO("LepVariables            : " << tmp                    );
  MSG_INFO("---------------------------------------------------------------------------------------");
}
