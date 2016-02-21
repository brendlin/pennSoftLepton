#include "pennSoftLepton/Ntupler.h"
// this is needed to distribute the algorithm to the workers
ClassImp(PSL::Ntupler)
  
PSL::Ntupler::Ntupler() : AlgBase()
  ,tree(0)
  ,treename("physics")
  ,ParentEventSelector(0)
  ,m_DoTriggers(true)
  ,m_DoLeptonID(true)
  ,fATreeEvent(NULL)
  ,fATreeEvent_int(NULL)
{
  MSG_DEBUG("Constructor");
  SetName("NTUPLER");
}

//-----------------------------------------------------------------------------
void PSL::Ntupler::set_SkipIsoVariables(std::string v){
  MSG_DEBUG("skipping ntupler isolation variables: " << v);
  std::vector<std::string> tmp;
  PSL::StringTok(tmp,v," "); 
  std::vector<AnaIso::AnaIso> isovariables = GetAllIsolationVariables();
  m_lep_iso.clear();
  for(unsigned int i=0;i<isovariables.size();i++){
    if (std::find(tmp.begin(),tmp.end(),ConvertAnaIsoToStr(isovariables[i])) != tmp.end()) continue;
    if (isovariables[i] == AnaIso::UserDefined) continue;
    MSG_DEBUG("Adding " << ConvertAnaIsoToStr(isovariables[i]));
    m_lep_iso.push_back(isovariables[i]);
  }
}

//-----------------------------------------------------------------------------
bool PSL::Ntupler::init(void){
  TFile *file = wk()->getOutputFile("SKIM");
  tree = new TTree(treename.c_str(),treename.c_str());
  tree->SetDirectory(file);

  // Unless otherwise specified, save all of the variables from Variables.h
  if (!m_variables.size()) m_variables = GetAllVariables();
  // save all of the trigger decisions
  if (m_DoTriggers) m_triggers = m_EDM->GetTriggerBitmask();
  // Unless otherwise specified, save all of the lep variables from Variables.h
  if (!m_lepvariables.size()) m_lepvariables = GetAllLepVariables();
  // Unless otherwise specified, save all of the jet variables from Variables.h
  if (!m_jetvariables.size()) m_jetvariables = GetAllJetVariables();

  // save electron ID information in vectors
  m_lep_id.clear();
  m_lep_muon_id.clear();
  if (m_DoLeptonID) {
    m_lep_id.push_back(ElectronID::TightLLH);
    m_lep_id.push_back(ElectronID::MediumLLH);
    m_lep_id.push_back(ElectronID::LooseLLH);
#ifndef BEFORE_SUSYTOOLS_000611
    m_lep_id.push_back(ElectronID::LooseAndBLayerLLH);
#endif
    m_lep_id.push_back(ElectronID::VeryLooseLLH);

    m_lep_muon_id.push_back(xAOD::Muon::Tight    );
    m_lep_muon_id.push_back(xAOD::Muon::Medium   );
    m_lep_muon_id.push_back(xAOD::Muon::Loose    );
    m_lep_muon_id.push_back(xAOD::Muon::VeryLoose);
  }
  // save lepton isolation information in vectors
  if (m_DoLeptonID && !m_lep_iso.size()) {
#ifdef ISREL20      
    m_lep_iso.push_back(AnaIso::ptvarcone20overPt );
    m_lep_iso.push_back(AnaIso::ptvarcone30overPt );
    m_lep_iso.push_back(AnaIso::ptvarcone40overPt );
#endif
    m_lep_iso.push_back(AnaIso::topoetcone20overPt);
    m_lep_iso.push_back(AnaIso::topoetcone30overPt);
    m_lep_iso.push_back(AnaIso::topoetcone40overPt);
    m_lep_iso.push_back(AnaIso::Gradient          );
    m_lep_iso.push_back(AnaIso::GradientLoose     );
    m_lep_iso.push_back(AnaIso::LooseTrackOnly    );
  }

  // book branches
  unsigned int full_size_evt = m_variables.size()+m_triggers.size();
  if (!fATreeEvent) fATreeEvent = new Float_t[full_size_evt];
  if (!fATreeEvent_int) fATreeEvent_int = new Int_t[full_size_evt]; // for now book too many

  // Event number
  tree->Branch("EventNumber",&m_eventnumber,"EventNumber/l");  

  //
  // Event-level variables!
  //
  int j = 0;
  for (unsigned int i=0;i<m_variables.size();++i){
    Variable v = m_variables[i];
    std::string vs = ConvertVarToStr(v);
    if (VariableIsInt(v))
      tree->Branch(vs.c_str(),&(fATreeEvent_int[j]), (vs + "/I").c_str());
    else 
      tree->Branch(vs.c_str(),&(fATreeEvent[j]), (vs + "/F").c_str());
    j++;
  }
  for (unsigned int i=0;i<m_triggers.size();++i){
    std::string ts = ConvertTrigToStr(m_triggers[i]);
    tree->Branch(ts.c_str(),&(fATreeEvent_int[j]), (ts + "/I").c_str());
    j++;
  }

  //
  // lepton-level variables!
  //
  unsigned int full_size_lep = m_lepvariables.size()+m_lep_id.size()+m_lep_muon_id.size()+m_lep_iso.size();
  full_size_lep += 5; // add 4 for scale factors!
  if (!m_lep_vector_floats.size()) 
    m_lep_vector_floats = std::vector<std::vector<float> >(full_size_lep,std::vector<float>());
  if (!m_lep_vector_ints.size())
    m_lep_vector_ints = std::vector<std::vector<int> >(full_size_lep,std::vector<int>());

  int k = 0;
  for (unsigned int i=0;i<m_lepvariables.size();++i){
    LepVariable v = m_lepvariables[i];
    if (VariableIsInt(v))
      tree->Branch(ConvertLepVarToStr(v).c_str(),"vector<int>",&(m_lep_vector_ints[k]));
    else
      tree->Branch(ConvertLepVarToStr(v).c_str(),"vector<float>",&(m_lep_vector_floats[k]));
    k++;
  }
  for (unsigned int i=0;i<m_lep_id.size();++i){
    tree->Branch(ConvertElectronIDToStr(m_lep_id[i]).c_str(),"vector<int>",&(m_lep_vector_ints[k]));
    k++;
  }
  for (unsigned int i=0;i<m_lep_muon_id.size();++i){
    tree->Branch(ConvertMuonIDToStr(m_lep_muon_id[i]).c_str(),"vector<int>",&(m_lep_vector_ints[k]));
    k++;
  }
  for (unsigned int i=0;i<m_lep_iso.size();++i){
    if (m_lep_iso[i] < AnaIso::N_NONWP)
      tree->Branch(ConvertAnaIsoToStr(m_lep_iso[i]).c_str(),"vector<float>",&(m_lep_vector_floats[k]));
    else
      tree->Branch(ConvertAnaIsoToStr(m_lep_iso[i]).c_str(),"vector<int>",&(m_lep_vector_ints[k]));
    k++;
  }
  tree->Branch("lepEleFullSF_Reco_LooseAndBLayerLLH_d0z0_isolLooseTrackOnly","vector<float>",&m_lep_vector_floats[k]); k++;
  tree->Branch("lepEleFullSF_Reco_MediumLLH_d0z0_v8_isolGradientLoose"      ,"vector<float>",&m_lep_vector_floats[k]); k++;
  tree->Branch("lepEleFullSF_Reco_TightLLH_d0z0_v8_isolGradient"            ,"vector<float>",&m_lep_vector_floats[k]); k++;
  tree->Branch("lepMuFullSF_Medium_d0z0_isolGradientLoose"                  ,"vector<float>",&m_lep_vector_floats[k]); k++;
  tree->Branch("lepMuFullSF_Loose_d0z0_isolLooseTrackOnly"                  ,"vector<float>",&m_lep_vector_floats[k]); k++;

  unsigned int full_size_jet = m_jetvariables.size();
  if (!m_jet_vector_floats.size())
    m_jet_vector_floats = std::vector<std::vector<float> >(full_size_jet,std::vector<float>());

  int h = 0;
  for (unsigned int i=0;i<m_jetvariables.size();++i){
    JetVariable v = m_jetvariables[i];
    tree->Branch(ConvertJetVarToStr(v).c_str(),"vector<float>",&(m_jet_vector_floats[h]));
    h++;
  }

  // parent event selector
  if (!ParentEventSelectorName.empty()){
    ParentEventSelector = (AlgBase*)connectToAnotherAlgorithm(ParentEventSelectorName);
    if (!ParentEventSelector) {
      MSG_INFO("Error! Could not find parent event selector (AlgBase type) " << ParentEventSelectorName << ". Exiting");
      return false;
    }
    MSG_DEBUG("Successfully connected " << ParentEventSelectorName << " to " << GetName() << ".");
  }

  for (unsigned int i=0;i<all_algs.size();++i){
    AlgBase* alg = dynamic_cast<AlgBase*>(wk()->getAlg(all_algs[i]));
    if (alg == 0) continue;
    alg->SetOutgoingNtupleBranches(tree);
  }

  PrintConfiguration();
  return true;
}

//-----------------------------------------------------------------------------
void PSL::Ntupler::loop(void){
  MSG_DEBUG("Fill start.");
  if (ParentEventSelector){
    // Hacky thing to make sure the algorithms are talking about the same event.
    if (!ParentEventSelector->passed(m_EDM->get_EventNumber())) return;
  }
  // event number
  m_eventnumber = m_EDM->get_EventNumber();

  //
  // Event-level variables!
  //
  int j = 0;
  for (unsigned int i=0;i<m_variables.size();++i){
    if (!m_EDM->VariableExists(m_variables[i])) continue;
    MSG_DEBUG("Variable: " << ConvertVarToStr(m_variables[i]) 
              << " " << i << " " << m_EDM->GetEventVariable(m_variables[i]));
    if (VariableIsInt(m_variables[i]))
      fATreeEvent_int[j] = (int)m_EDM->GetEventVariable(m_variables[i]);
    else 
      fATreeEvent[j] = m_EDM->GetEventVariable(m_variables[i]);
    j++;
  }
  for (unsigned int i=0;i<m_triggers.size();++i){
    fATreeEvent_int[j] = (int)m_EDM->PassTrigger(m_triggers[i]); // Trigger2015 enum
    j++;
  }
  
  //
  // lepton-level variables!
  //
  int k = 0;
  int n_leps = m_EDM->GetEventVariable(vnSignalLeptons);
  for (unsigned int i=0;i<m_lepvariables.size();++i){
    m_lep_vector_floats[k].clear();
    m_lep_vector_ints[k].clear();
    for (int l=0;l<n_leps;++l){ 
      if (!m_EDM->LepVariableExists(m_lepvariables[i])) continue;
      if (VariableIsInt(m_lepvariables[i]))
        m_lep_vector_ints[k].push_back((int)m_EDM->GetSignalLeptonVariable(m_lepvariables[i],l));
      else
        m_lep_vector_floats[k].push_back(m_EDM->GetSignalLeptonVariable(m_lepvariables[i],l));
    }
    k++;
  }
  for (unsigned int i=0;i<m_lep_id.size();++i){
    m_lep_vector_ints[k].clear();
    for (int l=0;l<n_leps;++l){ 
      m_lep_vector_ints[k].push_back(m_EDM->ptorderedLikelihood(l,m_lep_id[i]));
    }
    k++;
  }
  for (unsigned int i=0;i<m_lep_muon_id.size();++i){
    m_lep_vector_ints[k].clear();
    for (int l=0;l<n_leps;++l){ 
      m_lep_vector_ints[k].push_back(m_EDM->ptorderedPassMuonQuality(l,m_lep_muon_id[i]));
    }
    k++;
  }
  for (unsigned int i=0;i<m_lep_iso.size();++i){
    m_lep_vector_floats[k].clear();
    m_lep_vector_ints[k].clear();
    for (int l=0;l<n_leps;++l){ 
      if (m_lep_iso[i] < AnaIso::N_NONWP)
        m_lep_vector_floats[k].push_back(m_EDM->GetSignalIsolation(l,m_lep_iso[i],m_lep_iso[i]));
      else
        m_lep_vector_ints[k].push_back(m_EDM->GetSignalIsolation(l,m_lep_iso[i],m_lep_iso[i]));
    }
    k++;
  }

  // Supported scale factors!
  MSG_DEBUG("Filling ntuple scale factors.");
  m_lep_vector_floats[k].clear();
  for (int l=0;l<n_leps;++l){
    m_lep_vector_floats[k].push_back(m_EDM->GetSignalEleSF_RecoIDIso(l,EleFullSF_Reco_LooseAndBLayerLLH_d0z0_isolLooseTrackOnly));
  }
  k++;
  MSG_DEBUG("Filling ntuple scale factors. 2");
  m_lep_vector_floats[k].clear();
  for (int l=0;l<n_leps;++l){
    m_lep_vector_floats[k].push_back(m_EDM->GetSignalEleSF_RecoIDIso(l,EleFullSF_Reco_MediumLLH_d0z0_v8_isolGradientLoose));
  }
  k++;
  MSG_DEBUG("Filling ntuple scale factors. 3");
  m_lep_vector_floats[k].clear();
  for (int l=0;l<n_leps;++l){
    m_lep_vector_floats[k].push_back(m_EDM->GetSignalEleSF_RecoIDIso(l,EleFullSF_Reco_TightLLH_d0z0_v8_isolGradient));
  }
  k++;
  MSG_DEBUG("Filling ntuple scale factors. 4");
  m_lep_vector_floats[k].clear();
  for (int l=0;l<n_leps;++l){
    m_lep_vector_floats[k].push_back(m_EDM->GetSignalMuonSF_IDIsoTTVA(l,MuFullSF_Medium_d0z0_isolGradientLoose));
  }
  k++;
  MSG_DEBUG("Filling ntuple scale factors. 5");
  m_lep_vector_floats[k].clear();
  for (int l=0;l<n_leps;++l){
    m_lep_vector_floats[k].push_back(m_EDM->GetSignalMuonSF_IDIsoTTVA(l,MuFullSF_Loose_d0z0_isolLooseTrackOnly));
  }
  k++;
  MSG_DEBUG("Filling ntuple scale factors done.");

  //
  // jet variables!
  //
  int h = 0;
  int n_jets = m_EDM->GetEventVariable(vNJets);
  for (unsigned int i=0;i<m_jetvariables.size();++i){
    m_jet_vector_floats[h].clear();
    for (int l=0;l<n_jets;++l){ 
      m_jet_vector_floats[h].push_back(m_EDM->GetJetVariable(m_jetvariables[i],l));
    }
    h++;
  }

  tree->Fill();
  MSG_DEBUG("Fill complete.");
  return;
}

//-----------------------------------------------------------------------------
EL::StatusCode PSL::Ntupler::setupJob(EL::Job& job){
  EL::OutputStream out("SKIM");
  job.outputAdd(out);
  return EL::StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
void PSL::Ntupler::GetAlgList(EL::Job job){
  all_algs.clear();
  for (EL::Job::algsIter alg = job.algsBegin(), end = job.algsEnd(); alg != end; ++ alg){
    all_algs.push_back((*alg)->GetName());
  }
  return;
}

//-----------------------------------------------------------------------------
void PSL::Ntupler::finish(){
  TFile *file = wk()->getOutputFile("SKIM");
  std::map<unsigned int, TH1F*>::iterator it;
  for (it = m_EDM->m_sample_stats.begin(); it != m_EDM->m_sample_stats.end(); it++)
  {
    TString mcchan;
    TH1F* hist = (TH1F*)it->second->Clone();
    hist->SetDirectory(file);
  }
  AlgBase* tmp_parentSelector = ParentEventSelector;
  while (tmp_parentSelector) {
    if (tmp_parentSelector->hist_PassN) {      
      TH2F* hist = (TH2F*)tmp_parentSelector->hist_PassN->Clone();
      hist->SetDirectory(file);
    }
    if (tmp_parentSelector->hist_PassW) {
      TH2F* hist = (TH2F*)tmp_parentSelector->hist_PassW->Clone();
      hist->SetDirectory(file);
    }
    tmp_parentSelector = tmp_parentSelector->ParentEventSelector;
  }
  return;
}

//-----------------------------------------------------------------------------
void PSL::Ntupler::PrintConfiguration(void){
  MSG_INFO("-------------------------------Ntupler Configuration:----------------------------------");
  MSG_INFO("This Instance Name      : " << GetName()              );
  MSG_INFO("ParentEventSelectorName : " << ParentEventSelectorName);
  MSG_INFO("treename                : " << treename               );
  MSG_INFO("DoTriggers              : " << (m_DoTriggers ? "yes" : "no"));
  MSG_INFO("DoLeptonID              : " << (m_DoLeptonID ? "yes" : "no"));
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
  // lep isolation variables
  tmp = "";
  for (unsigned int i=0;i<m_lep_iso.size();++i){
    tmp += ConvertAnaIsoToStr(m_lep_iso[i]);
    tmp += " ";
  }
  MSG_INFO("Lep Isolation Variables : " << tmp                    );
  MSG_INFO("---------------------------------------------------------------------------------------");
}
