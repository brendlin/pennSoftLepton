#include "pennSoftLepton/NtupleWrapper.h"

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

#include "TTree.h"

//-----------------------------------------------------------------------------
PSL::NtupleWrapper::NtupleWrapper() : EL::Algorithm()
                                    ,m_evtdef(0)
                                    ,m_el_defaultcaloiso(AnaIso::None)
                                    ,m_el_defaulttrkiso (AnaIso::None)
                                    ,m_mu_defaultcaloiso(AnaIso::None)
                                    ,m_mu_defaulttrkiso (AnaIso::None)
{
  MSG_DEBUG("Constructor");
  SetName("PSL_EDM_CLASS");
}

//-----------------------------------------------------------------------------
EL::StatusCode PSL::NtupleWrapper::setupJob (EL::Job& job){
  MSG_DEBUG("setupJob");
  (void)job;
  return EL::StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
EL::StatusCode PSL::NtupleWrapper::histInitialize(){
  MSG_DEBUG("histInitialize");
  n_processed = 0;
  return EL::StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
EL::StatusCode PSL::NtupleWrapper::fileExecute(){
  MSG_DEBUG("fileExecute");
  m_CurrentSample = knone;
  return EL::StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
EL::StatusCode PSL::NtupleWrapper::changeInput (bool firstFile){
  MSG_DEBUG("changeInput");
  (void)firstFile;
  m_CurrentSample = knone;

  TTree *tree = wk()->tree();
  tree->SetBranchStatus ("*", 0);

  MSG_DEBUG("Setting branch statuses");
  SetBranchStatusAddress(tree,"EventNumber",&m_eventnumber);

  // Variable
  m_missing_variables.clear();
  for (unsigned int i=0;i<N_VARIABLES;++i){
    Variable v = static_cast<PSL::Variable>(i);
    if (v == vNone) continue;
    bool exists = false;
    if (VariableIsInt(v))
      exists = SetBranchStatusAddress(tree,ConvertVarToStr(v).c_str(),&m_EventVariableHolder_int[i],false);
    else exists = SetBranchStatusAddress(tree,ConvertVarToStr(v).c_str(),&m_EventVariableHolder[i],false);
    if (!exists) m_missing_variables.push_back(v);
  }  
  // LepVariable
  m_missing_lepvariables.clear();
  for (unsigned int i=0;i<N_LEPVARIABLES;++i){
    m_LepVariableHolder.push_back(new std::vector<float>());
  }
  for (unsigned int i=0;i<N_LEPVARIABLES;++i){
    LepVariable v = static_cast<PSL::LepVariable>(i);
    if (v == lepNone) continue;
    bool exists = SetBranchStatusAddress(tree,ConvertLepVarToStr(v).c_str(),&m_LepVariableHolder[i],false);
    if (!exists) m_missing_lepvariables.push_back(v);
  }
  // Lepton ID
  m_missing_eleid.clear();
  for (unsigned int i=0;i<ElectronID::N_ELECTRONID;++i){
    m_ElectronIDHolder.push_back(new std::vector<int>());
  }
  for (unsigned int i=0;i<ElectronID::N_ELECTRONID;++i){
    ElectronID::ElectronID e = static_cast<PSL::ElectronID::ElectronID>(i);
    if (e == ElectronID::None) continue;
    bool exists = SetBranchStatusAddress(tree,ConvertElectronIDToStr(e).c_str(),&m_ElectronIDHolder[i],false);
    if (!exists) m_missing_eleid.push_back(e);
  }  
  // muon ID
  m_missing_muonid.clear();
  for (unsigned int i=0;i<xAOD::Muon::VeryLoose+1;++i){
    m_MuonIDHolder.push_back(new std::vector<int>());
  }
  for (unsigned int i=0;i<xAOD::Muon::VeryLoose+1;++i){
    xAOD::Muon::Quality q = static_cast<xAOD::Muon::Quality>(i);
    bool exists = SetBranchStatusAddress(tree,ConvertMuonIDToStr(q).c_str(),&m_MuonIDHolder[i],false);
    if (!exists) m_missing_muonid.push_back(q);
  }  

  // Lepton Isolation
  m_missing_iso.clear();
  for (unsigned int i=0;i<AnaIso::N_ISOLATION;++i){
    m_LepIsoHolder.push_back(new std::vector<float>());
  }
  for (unsigned int i=0;i<AnaIso::N_ISOLATION;++i){
    AnaIso::AnaIso a = static_cast<PSL::AnaIso::AnaIso>(i);
    if (a == AnaIso::None) continue;
    if (a == AnaIso::N_NONRELATIVE) continue;
    if (a == AnaIso::N_NONWP) continue;
    if (a == AnaIso::UserDefined) continue;
    bool exists = SetBranchStatusAddress(tree,ConvertAnaIsoToStr(a).c_str(),&m_LepIsoHolder[i],false);
    if (!exists) m_missing_iso.push_back(a);
  }  
  
  // Lepton scale factors
  m_missing_elesf.clear();
  for (unsigned int i=0;i<N_ELESF;++i){
    m_EleSFHolder.push_back(new std::vector<float>());
  }
  bool exists = SetBranchStatusAddress(tree,"lepEleFullSF_Reco_LooseAndBLayerLLH_d0z0_isolLooseTrackOnly",&m_EleSFHolder[EleFullSF_Reco_LooseAndBLayerLLH_d0z0_isolLooseTrackOnly],false);
  if (!exists) m_missing_elesf.push_back(EleFullSF_Reco_LooseAndBLayerLLH_d0z0_isolLooseTrackOnly);
  exists      = SetBranchStatusAddress(tree,"lepEleFullSF_Reco_MediumLLH_d0z0_v8_isolGradientLoose",&m_EleSFHolder[EleFullSF_Reco_MediumLLH_d0z0_v8_isolGradientLoose],false);
  if (!exists) m_missing_elesf.push_back(EleFullSF_Reco_MediumLLH_d0z0_v8_isolGradientLoose);
  exists      = SetBranchStatusAddress(tree,"lepEleFullSF_Reco_TightLLH_d0z0_v8_isolGradient",&m_EleSFHolder[EleFullSF_Reco_TightLLH_d0z0_v8_isolGradient],false);
  if (!exists) m_missing_elesf.push_back(EleFullSF_Reco_TightLLH_d0z0_v8_isolGradient);
  //
  m_missing_muonsf.clear();
  for (unsigned int i=0;i<N_MUSF;++i){
    m_MuonSFHolder.push_back(new std::vector<float>());
  }
  exists      = SetBranchStatusAddress(tree,"lepMuFullSF_Medium_d0z0_isolGradientLoose",&m_MuonSFHolder[MuFullSF_Medium_d0z0_isolGradientLoose],false);
  if (!exists) m_missing_muonsf.push_back(MuFullSF_Medium_d0z0_isolGradientLoose);
  exists      = SetBranchStatusAddress(tree,"lepMuFullSF_Loose_d0z0_isolLooseTrackOnly",&m_MuonSFHolder[MuFullSF_Loose_d0z0_isolLooseTrackOnly],false);
  if (!exists) m_missing_muonsf.push_back(MuFullSF_Loose_d0z0_isolLooseTrackOnly);

  // JetVariable
  m_missing_jetvariables.clear();
  for (unsigned int i=0;i<N_JETVARIABLES;++i){
    m_JetVariableHolder.push_back(new std::vector<float>());
  }
  for (unsigned int i=0;i<N_JETVARIABLES;++i){
    JetVariable v = static_cast<PSL::JetVariable>(i);
    if (v == jetNone) continue;
    bool exists = SetBranchStatusAddress(tree,ConvertJetVarToStr(v).c_str(),&m_JetVariableHolder[i],false);
    if (!exists) m_missing_jetvariables.push_back(v);
  }
  // Trigger branches
  m_missing_trig.clear();
  for (unsigned int i=0;i<N_TRIGGERS;++i){
    Trigger2015 t = static_cast<PSL::Trigger2015>(i);
    if (t == tNone) continue;
    bool exists = SetBranchStatusAddress(tree,ConvertTrigToStr(t).c_str(),&m_TrigHolder[i],false);
    if (!exists) m_missing_trig.push_back(t);
  }  
  MSG_DEBUG("Setting branch statuses done");

  return EL::StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
EL::StatusCode PSL::NtupleWrapper::initialize(){
  MSG_DEBUG("initialize");
  return EL::StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
EL::StatusCode PSL::NtupleWrapper::execute(){
  MSG_DEBUG("execute");
  MSG_DEBUG("Before GetEntry");
  wk()->tree()->GetEntry(wk()->treeEntry());
  MSG_DEBUG("After GetEntry");
  if (false) PrintAllVariablesForDebugging();
  //m_CurrentSample = static_cast<PSL::Sample>(GetEventVariable(vSample));
  m_CurrentSample = ConvertToSample(wk()->metaData()->castString("sample_name"));
  MSG_DEBUG("Sample is " << ConvertSampleToStr(m_CurrentSample));
  n_processed++;
  m_evtdef = 0;
  tree_evtdef.Reset();
  MSG_DEBUG("execute end");
  return EL::StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
void PSL::NtupleWrapper::ResetSample(){
  //m_CurrentSample = static_cast<PSL::Sample>(m_EventVariableHolder_int[vSample]);
  m_CurrentSample = ConvertToSample(wk()->metaData()->castString("sample_name"));
}

//-----------------------------------------------------------------------------
EL::StatusCode PSL::NtupleWrapper::postExecute(){
  MSG_DEBUG("postExecute");
  return EL::StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
EL::StatusCode PSL::NtupleWrapper::finalize(){
  MSG_INFO("finalize. Nevents processed: " << n_processed);
  return EL::StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
EL::StatusCode PSL::NtupleWrapper::histFinalize()
{
  MSG_DEBUG("histFinalize");
 return EL::StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::GetEventVariable(PSL::Variable v){
  if (!VariableExists(v)){
    MSG_INFO("Error! " << ConvertVarToStr(v) << " is missing! Exiting.");
    exit(1);
  }
  if (m_evtdef) {
    // some variables are not accessed via the EventDefinition. So access them from the tree.
    if (v == vGRL         ) return m_EventVariableHolder_int[v];
    if (v == vpassLarError) return m_EventVariableHolder_int[v];
    if (v == vpassTileError        ) return m_EventVariableHolder_int[v];
    if (v == vpassSctError         ) return m_EventVariableHolder_int[v];
    if (v == vpassCoreBit          ) return m_EventVariableHolder_int[v];
    if (v == vpassPrimaryVtx       ) return m_EventVariableHolder_int[v];
    if (v == vNPV                  ) return m_EventVariableHolder_int[v];
    if (v == vAverageInteractionsPerCrossing) return m_EventVariableHolder[v];
    if (v == vActualInteractionsPerCrossing) return m_EventVariableHolder[v];
    if (v == vpassCosmicMuon       ) return m_EventVariableHolder_int[v];
    if (v == vpassTileTripReader   ) return m_EventVariableHolder_int[v];
    if (v == vpassIncompleteEvents ) return m_EventVariableHolder_int[v];
    if (v == vpassTileCalHotSpot   ) return m_EventVariableHolder_int[v];
    if (v == vpassBadJet           ) return m_EventVariableHolder_int[v];
    if (v == vpassCaloJet          ) return m_EventVariableHolder_int[v];
    if (v == vpassBadMuon          ) return m_EventVariableHolder_int[v];
    if (v == vpassHFOR             ) return m_EventVariableHolder_int[v];
    if (v == vpassMCOverlap        ) return m_EventVariableHolder_int[v];
    if (v == vpassBCH              ) return m_EventVariableHolder_int[v];
    if (v == vMuIsValid            ) return m_EventVariableHolder_int[v];
    // if (v == vjet0Pt               ) return m_EventVariableHolder[v]; // fix!
    // if (v == vjet0Eta              ) return m_EventVariableHolder[v]; // fix!
    // if (v == vjet0Phi              ) return m_EventVariableHolder[v]; // fix!
    // if (v == vjet1Pt               ) return m_EventVariableHolder[v]; // fix!
    // if (v == vjet1Eta              ) return m_EventVariableHolder[v]; // fix!
    // if (v == vjet1Phi              ) return m_EventVariableHolder[v]; // fix!
    if (v == vMWjj                 ) return m_EventVariableHolder[v]; // fix!
    if (v == vMjjMin               ) return m_EventVariableHolder[v]; // fix!
    if (v == vMEToverPtj0          ) return m_EventVariableHolder[v]; // fix!
    if (v == vPassTrigger          ) return m_EventVariableHolder_int[v];
    if (v == vLumiBlockMu          ) return m_EventVariableHolder[v];
    if (v == vSample               ) return m_CurrentSample;
    if (v == vtruthjet0Pt           ) return m_EventVariableHolder[v];
    if (v == vtruthjet1Pt           ) return m_EventVariableHolder[v];
    if (v == vTotalWeight           ) return m_EventVariableHolder[v];
    if (v == vTotalWeightNoSF       ) return m_EventVariableHolder[v];
    if (v == vTotalWeightNoPupNoSF  ) return m_EventVariableHolder[v];
    if (v == vmc_channel_number     ) return m_EventVariableHolder_int[v];
    if (v == vptAvg                 ) return m_EventVariableHolder[v]; // fix!
    if (v == vtruthptAvg            ) return m_EventVariableHolder[v];
    if (v == vptAvgOverTruthjet0Pt  ) return m_EventVariableHolder[v]; // fix!
    if (v == vWlepPassWSelection    ) return 0; // fix!
    // if a variable in EventDefinition, get it from an EventDefinition if it exists
    if (v == vWlepPassWSelection    ) return 0; // fix!
//     if (v == vDeltaPhiLeadZWLep     ) return 0; // fix!
//     if (v == vDeltaPhiZLeps         ) return 0; // fix!
//     if (v == vDeltaPhiZWLep         ) return 0; // fix!
//     if (v == vDeltaPhiZMet          ) return 0; // fix!
    if (v == vRunNumber             ) return m_EventVariableHolder_int[v];
    if (v == vEvtTrigSF             ) return m_EventVariableHolder[v];
    if (v == vAntiIDSF_lep3       ) return m_EventVariableHolder[v];
    if (v == vAntiIDSF_lep3_pt    ) return m_EventVariableHolder[v];
    if (v == vAntiIDSF_lep3_eta   ) return m_EventVariableHolder[v];
    if (v == vAntiIDSF_lep3_phi   ) return m_EventVariableHolder[v];
    if (v == vAntiIDSF_lep3_flavor) return m_EventVariableHolder_int[v];
    return m_evtdef->GetEventVariable(v);
  }
  //NO if (tree_evtdef.valid) return tree_evtdef.GetEventVariableInt(v);
  // otherwise take directly from tree
  if (v == vMll_alt) return 0;
  if (VariableIsInt(v)) return m_EventVariableHolder_int[v];
  return m_EventVariableHolder[v];
}

//-----------------------------------------------------------------------------
bool PSL::NtupleWrapper::ptorderedLikelihood(unsigned int iptordered,ElectronID::ElectronID id){
  int icontainer = iptordered;
  if (m_evtdef) icontainer = m_evtdef->PtOrderedLepton(iptordered).i_cont;
  return passLikelihood(icontainer,id);
}

//-----------------------------------------------------------------------------
bool PSL::NtupleWrapper::ptorderedPassMuonQuality(unsigned int iptordered,xAOD::Muon::Quality id){
  int icontainer = iptordered;
  if (m_evtdef) icontainer = m_evtdef->PtOrderedLepton(iptordered).i_cont;
  return passMuonQuality(icontainer,id);
}

//-----------------------------------------------------------------------------
TLorentzVector PSL::NtupleWrapper::GetSignalLeptonTLV(int iptordered){
  // MEV!!!
  if (m_evtdef) return m_evtdef->PtOrderedLepton(iptordered).tlv;
  return GetContainerLeptonTLV(iptordered);
}

//-----------------------------------------------------------------------------
TLorentzVector PSL::NtupleWrapper::GetContainerLeptonTLV(int icontainer){
  // MEV!!!
  TLorentzVector lep;
  lep.SetPtEtaPhiM(1000.*m_LepVariableHolder[lepPt]->at(icontainer)
                   ,m_LepVariableHolder[lepEta]->at(icontainer)
                   ,m_LepVariableHolder[lepPhi]->at(icontainer)
                   ,(m_LepVariableHolder[lepFlavor]->at(icontainer) == (int)ObjType::Electron) ? 0.000511 : 0.105658 );
  return lep;
}
//-----------------------------------------------------------------------------
TLorentzVector PSL::NtupleWrapper::GetContainerEleTLV(int icontainer){
  AssertContainerLeptonIsElectron(icontainer);
  TLorentzVector lep;
  lep.SetPtEtaPhiM(1000.*m_LepVariableHolder[lepPt]->at(icontainer)
                   ,m_LepVariableHolder[lepEta]->at(icontainer)
                   ,m_LepVariableHolder[lepPhi]->at(icontainer)
                   ,(m_LepVariableHolder[lepFlavor]->at(icontainer) == (int)ObjType::Electron) ? 0.000511 : 0.105658 );
  return lep;
}
//-----------------------------------------------------------------------------
TLorentzVector PSL::NtupleWrapper::GetContainerJetTLV(int icontainer){
  TLorentzVector jettlv;
  jettlv.SetPtEtaPhiE(1000.*m_JetVariableHolder[jetPt]->at(icontainer),
                      m_JetVariableHolder[jetEta]->at(icontainer),
                      m_JetVariableHolder[jetPhi]->at(icontainer),
                      1000.*m_JetVariableHolder[jetE]->at(icontainer));
  return jettlv;
}
//-----------------------------------------------------------------------------
bool PSL::NtupleWrapper::AssertContainerLeptonIsElectron(int icontainer,bool fatal){
  bool is_ok = true;
  if (icontainer < 0) is_ok = false;
  else if (m_LepVariableHolder[lepFlavor]->at(icontainer) != (int)ObjType::Electron)
    is_ok = false;
  if (fatal && !is_ok) {
    MSG_INFO("Error! Tried to access an electron variable for a non-electron!");
    exit(1);
  }
  return is_ok;
}

//-----------------------------------------------------------------------------
bool PSL::NtupleWrapper::AssertContainerLeptonIsMuon(int icontainer,bool fatal){
  bool is_ok = true;
  if (icontainer < 0) is_ok = false;
  else if (m_LepVariableHolder[lepFlavor]->at(icontainer) != (int)ObjType::Muon)
    is_ok = false;
  if (fatal && !is_ok) {
    MSG_INFO("Error! Tried to access a muon variable for a non-muon!");
    exit(1);
  }
  return is_ok;
}

//-----------------------------------------------------------------------------
TLorentzVector PSL::NtupleWrapper::GetContainerMuonTLV(int icontainer){
  AssertContainerLeptonIsMuon(icontainer);
  TLorentzVector lep;
  lep.SetPtEtaPhiM(1000.*m_LepVariableHolder[lepPt]->at(icontainer)
                   ,m_LepVariableHolder[lepEta]->at(icontainer)
                   ,m_LepVariableHolder[lepPhi]->at(icontainer)
                   ,(m_LepVariableHolder[lepFlavor]->at(icontainer) == (int)ObjType::Electron) ? 0.000511 : 0.105658 );
  return lep;
}

//-----------------------------------------------------------------------------
int PSL::NtupleWrapper::GetSignalLeptonContainerIndex(int iptordered){
  if (m_evtdef) return m_evtdef->PtOrderedLepton(iptordered).i_cont;
  return iptordered;
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::GetSignalLeptonVariable(PSL::LepVariable lepv,int iptordered,bool isAntiId){
  (void)isAntiId;
  //if (m_evtdef) MSG_INFO("Getting lepton variable from evtdef");
  if (m_evtdef) return GetContainerLeptonVariable(lepv,m_evtdef->PtOrderedLepton(iptordered).i_cont);
  return GetContainerLeptonVariable(lepv,iptordered);
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::GetContainerLeptonVariable(PSL::LepVariable lepv,int icontainer){
  if (icontainer < 0) return -999.;
  if (icontainer >= m_EventVariableHolder_int[vnSignalLeptons]) return -999.;
  
  if (lepv == lepElectronID){
    if (passLikelihood(icontainer,ElectronID::TightLLH )) return 0.;
    if (passLikelihood(icontainer,ElectronID::MediumLLH)) return 1.;
    if (passLikelihood(icontainer,ElectronID::LooseLLH )) return 2.;
    return 3;
  }

//   if (lepv == lepTruthDetailed) {
//     return IsRealLepton(static_cast<PSL::LeptonTruthLabel>(m_LepVariableHolder[lepTruthDetailed]->at(icontainer))) ? 1 : 0;
//   }

//   if (lepv == lepMt) {
//     TLorentzVector lep = GetLeptonTLV(iptordered);
//     TLorentzVector met;
//     met.SetPtEtaPhiM(GetEventVariable(vmet_Et),0,GetEventVariable(vmet_phi),0);
//     return sqrt(2*lep.Pt()*met.Pt()*(1-cos(met.DeltaPhi(lep))));
//   }

  if (lepv == lepMll){
    double min_delta = 999999999;
    double mll = -999.;
    double closest_to=91.188;
    TLorentzVector a = GetContainerLeptonTLV(icontainer);
    
    for (int jcontainer=0;jcontainer<m_EventVariableHolder_int[vnSignalLeptons];++jcontainer){
      if (icontainer == jcontainer) continue;
      if (GetContainerLeptonVariable(lepFlavor,icontainer) != GetContainerLeptonVariable(lepFlavor,jcontainer)) continue;
      if (GetContainerLeptonVariable(lepCharge,icontainer) == GetContainerLeptonVariable(lepCharge,jcontainer)) continue;
      if (GetContainerLeptonVariable(lepFlavor,icontainer) == (int)ObjType::Electron
          && !passLikelihood(jcontainer,ElectronID::MediumLLH)) continue;
      TLorentzVector b = GetContainerLeptonTLV(jcontainer);
      double tmp = (a+b).M();
      if (fabs(tmp-closest_to) < min_delta){
        min_delta = fabs(tmp-closest_to);
        mll = tmp;
      }
    }
    return mll;
  }

  if (!LepVariableExists(lepv)){
    MSG_INFO("Error! " << ConvertLepVarToStr(lepv) << " is missing! Exiting.");
    exit(1);
  }
  if (lepv == lepTrkIso) return GetContainerIsolation(icontainer,m_el_defaulttrkiso,m_mu_defaulttrkiso);
  if (lepv == lepCaloIso) return GetContainerIsolation(icontainer,m_el_defaultcaloiso,m_mu_defaultcaloiso);
  // I checked - integers come out correctly too. Luckily.
  return m_LepVariableHolder[lepv]->at(icontainer);
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::GetSignalLeptonVariable(PSL::LepVariable v,PSL::Variable lepindex){
  if (branchMissing(v,m_missing_lepvariables)){
    MSG_INFO("Error! " << ConvertLepVarToStr(v) << " is missing! Exiting.");
    exit(1);
  }
  return GetSignalLeptonVariable(v,GetEventVariable(lepindex),false);
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::GetSignalIsolation(int iptordered,AnaIso::AnaIso isotype_ele,AnaIso::AnaIso isotype_mu,bool isAntiId){
  (void)isAntiId;
  int icontainer = iptordered;
  if (m_evtdef) icontainer = m_evtdef->PtOrderedLepton(iptordered).i_cont;
  return GetContainerIsolation(icontainer,isotype_ele,isotype_mu);
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::GetContainerIsolation(int icontainer,AnaIso::AnaIso isotype_ele,AnaIso::AnaIso isotype_mu){
  if (branchMissing(isotype_ele,m_missing_iso)){
    MSG_INFO("Error! " << ConvertAnaIsoToStr(isotype_ele) << " is missing! Exiting.");
    exit(1);
  }
  if (branchMissing(isotype_mu,m_missing_iso)){
    MSG_INFO("Error! " << ConvertAnaIsoToStr(isotype_mu) << " is missing! Exiting.");
    exit(1);
  }
  if ((int)GetContainerLeptonVariable(lepFlavor,icontainer) == (int)ObjType::Electron){
    if (isotype_ele == AnaIso::None) return 0;
    return m_LepIsoHolder[isotype_ele]->at(icontainer);
  }
  if (isotype_mu == AnaIso::None) return 0;  
  return m_LepIsoHolder[isotype_mu]->at(icontainer);
}

//-----------------------------------------------------------------------------
bool PSL::NtupleWrapper::passLikelihood(int icontainer,ElectronID::ElectronID id){
  if (branchMissing(id,m_missing_eleid)){
    MSG_INFO("Error! " << ConvertElectronIDToStr(id) << " is missing! Exiting.");
    exit(1);
  } 
  if (m_LepVariableHolder[lepFlavor]->at(icontainer) == (int)ObjType::Muon){
    return 0;
  }
  return m_ElectronIDHolder[id]->at(icontainer);
}

//-----------------------------------------------------------------------------
bool PSL::NtupleWrapper::passMuonQuality(int icontainer,xAOD::Muon::Quality id){
  if (branchMissing(id,m_missing_muonid)){
    MSG_INFO("Error! " << ConvertMuonIDToStr(id) << " is missing! Exiting.");
    exit(1);
  } 
  if (m_LepVariableHolder[lepFlavor]->at(icontainer) == (int)ObjType::Electron){
    return false;
  }
  return m_MuonIDHolder[id]->at(icontainer);
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::GetJetVariable(PSL::JetVariable v,int iptordered){
  if((int)GetEventVariable(vNJets) <= iptordered){
    MSG_INFO("Jet Range Issue!");
    exit(1);
  }
  if (iptordered < 0) return -999.;
  if (branchMissing(v,m_missing_jetvariables)){
    MSG_INFO("Error! " << ConvertJetVarToStr(v) << " is missing! Exiting.");
    exit(1);
  }
  return m_JetVariableHolder[v]->at(iptordered);
}

//-----------------------------------------------------------------------------
bool PSL::NtupleWrapper::PassTrigger(Trigger2015 t){
  if (branchMissing(t,m_missing_trig)){
    MSG_INFO("Error! " << ConvertTrigToStr(t) << " is missing! Exiting.");
    exit(1);
  } 
  return (bool)m_TrigHolder[t];
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::EleIsolation(int icontainer,AnaIso::AnaIso isoenum){
  AssertContainerLeptonIsElectron(icontainer);
  if ((int)m_LepIsoHolder[isoenum]->size() < icontainer+1) {
    MSG_INFO("Ele isolation vector Error for iso wp " << ConvertAnaIsoToStr(isoenum)); exit(1);
  }
  return m_LepIsoHolder[isoenum]->at(icontainer);
}
//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::MuonIsolation(int icontainer,AnaIso::AnaIso isoenum){
  AssertContainerLeptonIsMuon(icontainer);
  return m_LepIsoHolder[isoenum]->at(icontainer);
}
//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::EleD0Significance(int icontainer){
  AssertContainerLeptonIsElectron(icontainer);
  return m_LepVariableHolder[lepD0Significance]->at(icontainer);
}
//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::EleEtaBE(int icontainer){
  AssertContainerLeptonIsElectron(icontainer);
  return m_LepVariableHolder[lepEleEtaBE]->at(icontainer);
}
//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::MuonD0Significance(int icontainer){
  AssertContainerLeptonIsMuon(icontainer);
  return m_LepVariableHolder[lepD0Significance]->at(icontainer);
}
//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::EleD0(int icontainer){
  AssertContainerLeptonIsElectron(icontainer);
  return m_LepVariableHolder[lepD0]->at(icontainer);
}
//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::MuonD0(int icontainer){
  AssertContainerLeptonIsMuon(icontainer);
  return m_LepVariableHolder[lepD0]->at(icontainer);
}
//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::ElePt(int icontainer){
  AssertContainerLeptonIsElectron(icontainer);
  // MeV
  return m_LepVariableHolder[lepPt]->at(icontainer)*1000.;
}
//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::MuonPt(int icontainer){
  AssertContainerLeptonIsMuon(icontainer);
  // MeV
  return m_LepVariableHolder[lepPt]->at(icontainer)*1000.;
}
//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::EleZ0(int icontainer){
  AssertContainerLeptonIsElectron(icontainer);
  double sintheta = sin(GetContainerLeptonTLV(icontainer).Theta());
  if (sintheta == 0) return 0;
  return m_LepVariableHolder[lepZ0SinTheta]->at(icontainer)/sintheta;
}
//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::MuonZ0(int icontainer){
  AssertContainerLeptonIsMuon(icontainer);
  double sintheta = sin(GetContainerLeptonTLV(icontainer).Theta());
  if (sintheta == 0) return 0;
  return m_LepVariableHolder[lepZ0SinTheta]->at(icontainer)/sintheta;
}

//-----------------------------------------------------------------------------
bool PSL::NtupleWrapper::hasBranch(TTree* tree,std::string br){
  TObjArray* branches = tree->GetListOfBranches();
  MSG_DEBUG("Got object " << br << " " << branches->GetEntries());
  for (int i=0;i<branches->GetEntries();i++) {
    MSG_VERBOSE("---" << branches->At(i)->GetName());
  }
  if (branches->FindObject(br.c_str())) {
    MSG_DEBUG("Tree has the branch " << br);
    return true;
  }
  MSG_DEBUG("Tree does not have the branch " << br);
  return false;
}

//-----------------------------------------------------------------------------
template <class T> bool PSL::NtupleWrapper::SetBranchStatusAddress(TTree* tree,const char* s,T addr,bool printWarning){
  //std::cout << "Setting branch address AND status!" << std::endl;
  if (!hasBranch(tree,std::string(s))) {
    if (std::string(s) == "TightLH" && hasBranch(tree,"TightLLH")) {
      MSG_INFO("Switching from TightLH to TightLLH");
      s = "TightLLH";
    }
    else if (std::string(s) == "MediumLH" && hasBranch(tree,"MediumLLH")) {
      MSG_INFO("Switching from MediumLH to MediumLLH");
      s = "MediumLLH";
    }
    else if (std::string(s) == "LooseLH" && hasBranch(tree,"LooseLLH")) {
      MSG_INFO("Switching from LooseLH to LooseLLH");
      s = "LooseLLH";
    }
    else if (std::string(s) == "VeryLooseLH" && hasBranch(tree,"VeryLooseLLH")) {
      MSG_INFO("Switching from VeryLooseLH to VeryLooseLLH");
      s = "VeryLooseLLH";
    }
    else {
      if (printWarning) MSG_INFO("Warning: Branch missing: " << s);
      return false;
    }
  }
  tree->SetBranchStatus(s,1);
  tree->SetBranchAddress(s,addr);
  MSG_DEBUG("Successfully set branch address AND status of " << s);
  return true;
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::GetContainerMuonSF_IDIsoTTVA(int icont,xAOD::Muon::Quality q,AnaIso::AnaIso iso_wp){
  if (IsData()) return 1;
  if      (q == xAOD::Muon::Loose  && iso_wp == AnaIso::LooseTrackOnly) {
    if (!MuonSFExists(MuFullSF_Loose_d0z0_isolLooseTrackOnly)) { MSG_INFO("Error! MuFullSF_Loose_d0z0_isolLooseTrackOnly is missing! Exiting."); exit(1); }
    return m_MuonSFHolder[MuFullSF_Loose_d0z0_isolLooseTrackOnly]->at(icont);
  }
  else if (q == xAOD::Muon::Medium && iso_wp == AnaIso::GradientLoose ) {
    if (!MuonSFExists(MuFullSF_Medium_d0z0_isolGradientLoose)) { MSG_INFO("Error! MuFullSF_Medium_d0z0_isolGradientLoose is missing! Exiting."); exit(1); }
    return m_MuonSFHolder[MuFullSF_Medium_d0z0_isolGradientLoose]->at(icont);
  }
  else {
    MSG_INFO("Error - bad scale factor.");
    exit(1);
  }
  return 1;
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::GetContainerEleSF_RecoIDIso(int icont,ElectronID::ElectronID e,bool is_d0z0,AnaIso::AnaIso iso_wp){
  if (IsData()) return 1;
  if      (e == ElectronID::LooseAndBLayerLLH && is_d0z0 && iso_wp == AnaIso::LooseTrackOnly) {
    if (!EleSFExists(EleFullSF_Reco_LooseAndBLayerLLH_d0z0_isolLooseTrackOnly)) { MSG_INFO("Error! EleFullSF_Reco_LooseAndBLayerLLH_d0z0_isolLooseTrackOnly is missing! Exiting."); exit(1); }
    return m_EleSFHolder[EleFullSF_Reco_LooseAndBLayerLLH_d0z0_isolLooseTrackOnly]->at(icont);
  }
  else if (e == ElectronID::MediumLLH         && is_d0z0 && iso_wp == AnaIso::GradientLoose ) {
    if (!EleSFExists(EleFullSF_Reco_MediumLLH_d0z0_v8_isolGradientLoose)) { MSG_INFO("Error! EleFullSF_Reco_MediumLLH_d0z0_v8_isolGradientLoose is missing! Exiting."); exit(1); }
    return m_EleSFHolder[EleFullSF_Reco_MediumLLH_d0z0_v8_isolGradientLoose      ]->at(icont);
  }
  else if (e == ElectronID::TightLLH          && is_d0z0 && iso_wp == AnaIso::Gradient      ) {
    if (!EleSFExists(EleFullSF_Reco_TightLLH_d0z0_v8_isolGradient)) { MSG_INFO("Error! EleFullSF_Reco_TightLLH_d0z0_v8_isolGradient is missing! Exiting."); exit(1); }
    return m_EleSFHolder[EleFullSF_Reco_TightLLH_d0z0_v8_isolGradient            ]->at(icont);
  }
  else {
    MSG_INFO("Error - bad scale factor.");
    exit(1);
  }
  return 1;
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::GetSignalEleSF_RecoIDIso(int iptordered,ElectronSF sf){
  if (IsData()) return 1;
  int icontainer = iptordered;
  if (m_evtdef) icontainer = m_evtdef->PtOrderedLepton(iptordered).i_cont;
  AssertContainerLeptonIsElectron(icontainer);
  return GetContainerEleSF_RecoIDIso(icontainer,sf);
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::GetSignalMuonSF_IDIsoTTVA(int iptordered,MuonSF sf){
  if (IsData()) return 1;
  int icontainer = iptordered;
  if (m_evtdef) icontainer = m_evtdef->PtOrderedLepton(iptordered).i_cont;
  AssertContainerLeptonIsMuon(icontainer);
  return GetContainerMuonSF_IDIsoTTVA(icontainer,sf);
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::GetSignalEleSF_RecoIDIso(int iptordered,ElectronID::ElectronID e,bool is_d0z0,AnaIso::AnaIso iso_wp){
  if (IsData()) return 1;
  int icontainer = iptordered;
  if (m_evtdef) icontainer = m_evtdef->PtOrderedLepton(iptordered).i_cont;
  AssertContainerLeptonIsElectron(icontainer);
  return GetContainerEleSF_RecoIDIso(icontainer,e,is_d0z0,iso_wp);
}

//-----------------------------------------------------------------------------
double PSL::NtupleWrapper::GetSignalMuonSF_IDIsoTTVA(int iptordered,xAOD::Muon::Quality q,AnaIso::AnaIso iso_wp){
  if (IsData()) return 1;
  int icontainer = iptordered;
  if (m_evtdef) icontainer = m_evtdef->PtOrderedLepton(iptordered).i_cont;
  AssertContainerLeptonIsMuon(icontainer);
  return GetContainerMuonSF_IDIsoTTVA(icontainer,q,iso_wp);
}

//-----------------------------------------------------------------------------
void PSL::NtupleWrapper::PrintAllVariablesForDebugging(){
  MSG_INFO("Printing all variables.");
  std::vector<PSL::evtvar_props> props = GetAllEvtVariableProperties();
  for (unsigned int i=0;i<props.size();++i){
    //MSG_INFO(props[i].first);
    if (props[i].first == vNone) continue;
    if (branchMissing(props[i].first,m_missing_variables)) continue;
    MSG_INFO(props[i].second << ": " << GetEventVariable(props[i].first));
  }  
  MSG_INFO("Looping over vnSignalLeptons: " << (unsigned)GetEventVariable(vnSignalLeptons));
  for (unsigned int i=0;i<(unsigned)GetEventVariable(vnSignalLeptons);++i){
    MSG_INFO("Lepton " << i << ":");
    for (unsigned int j=0;j<N_LEPVARIABLES;++j){
      LepVariable v = static_cast<PSL::LepVariable>(j);
      if (v == lepNone) continue;
      if (branchMissing(v,m_missing_lepvariables)) continue;
      MSG_INFO(" - " << ConvertLepVarToStr(v) << ": " << GetSignalLeptonVariable(v,i));
    }
    for (unsigned int j=0;j<AnaIso::N_ISOLATION;++j){
      AnaIso::AnaIso a = static_cast<PSL::AnaIso::AnaIso>(j);
      if (a == AnaIso::None) continue;
      if (a == AnaIso::N_NONRELATIVE) continue;
      if (a == AnaIso::N_NONWP) continue;
      if (a == AnaIso::UserDefined) continue;
      if (branchMissing(a,m_missing_iso)) continue;
      MSG_INFO(" - " << ConvertAnaIsoToStr(a) << ": " << GetSignalIsolation(i,a,a));
    }
  }
  MSG_INFO("Printing all variables done.");
}

//-----------------------------------------------------------------------------
void PSL::NtupleWrapper::CopyEventDefinitionFromEDM(PSL::EventDefinition& evtdef){
  if (m_evtdef) return m_evtdef->MakeCopy(evtdef);
  if (tree_evtdef.valid) return tree_evtdef.MakeCopy(evtdef);
  // Fill event definition EXPLICITLY FROM THE TREE!
  tree_evtdef.n_baseline_ele = m_EventVariableHolder_int[vnBaselineElectrons];
  tree_evtdef.n_baseline_mu  = m_EventVariableHolder_int[vnBaselineMuons];
  //tree_evtdef.n_baseline_tau = 0;
  //tree_evtdef.n_baseline_jet = 0;
  tree_evtdef.m_passBadJet   = m_EventVariableHolder_int[vpassBadJet];

  tree_evtdef.chan = static_cast<PSL::LeptonChannel::LeptonChannel>(m_EventVariableHolder_int[vChanFlavor]);
  //tree_evtdef.chan_antiid    = static_cast<PSL::LeptonChannel>(0);
  for (int i=0;i<m_EventVariableHolder_int[vnSignalLeptons];++i){
    float sf = 1.;
    if (LepVariableExists(lepSFRecoIdIso)) sf = GetContainerLeptonVariable(lepSFRecoIdIso,i);
    tree_evtdef.leps.push_back(Particle(GetContainerLeptonTLV(i)
                                        ,static_cast<PSL::ObjType::ObjType>(GetContainerLeptonVariable(lepFlavor,i))
                                        ,i // i_cont
                                        ,GetContainerLeptonVariable(lepCharge,i)
                                        ,sf
                                        ,1 // trigger sf
                                        ));
    if (LepVariableExists(lepPassOR)) tree_evtdef.leps[i].passOR = m_LepVariableHolder[lepPassOR];
  }

  //tree_evtdef.antileps       = ;
  //tree_evtdef.vetoleps       = ;

  // NEED TO ADD JETS!
  for (int i=0;i<m_EventVariableHolder_int[vNJets];++i){
    tree_evtdef.jets.push_back(Particle(GetContainerJetTLV(i)
                                        ,PSL::ObjType::Jet
                                        ,i // i_cont
                                        ,0
                                        ));
  }

  //tree_evtdef.taus           = ;

  tree_evtdef.leadZlep_index       = m_EventVariableHolder_int[vlepleadZ_index];
  tree_evtdef.subleadZlep_index    = m_EventVariableHolder_int[vlepsubleadZ_index];
  tree_evtdef.Wlep_index           = m_EventVariableHolder_int[vlepW_index];

  tree_evtdef.leadMinSFOS_index    = m_EventVariableHolder_int[vlepleadMinSFOS_index];
  tree_evtdef.subleadMinSFOS_index = m_EventVariableHolder_int[vlepsubleadMinSFOS_index];
  tree_evtdef.WlepMinSFOS_index    = m_EventVariableHolder_int[vlepWMinSFOS_index];

//   tree_evtdef.leadYlep_index       = ;
//   tree_evtdef.subleadYlep_index    = ;
//   tree_evtdef.WlepYpsilon_index    = ;
  
  tree_evtdef.met_tv2        = TVector2();
  tree_evtdef.met_tv2.SetMagPhi(m_EventVariableHolder[vmet_Et]*1000.,m_EventVariableHolder[vmet_phi]);
  tree_evtdef.met_sumet      = m_EventVariableHolder[vmet_sumet]*1000.;
  tree_evtdef.refEle_tv2     = TVector2(m_EventVariableHolder[vrefEle_etx]*1000.,m_EventVariableHolder[vrefEle_ety]*1000.);
  tree_evtdef.refEle_sumet   = m_EventVariableHolder[vrefEle_sumet]*1000.;
  tree_evtdef.refMuo_tv2     = TVector2(m_EventVariableHolder[vrefMuo_etx]*1000.,m_EventVariableHolder[vrefMuo_ety]*1000.);
  tree_evtdef.refMuo_sumet   = m_EventVariableHolder[vrefMuo_sumet]*1000.;
  tree_evtdef.refJet_tv2     = TVector2(m_EventVariableHolder[vrefJet_etx]*1000.,m_EventVariableHolder[vrefJet_ety]*1000.);
  tree_evtdef.refJet_sumet   = m_EventVariableHolder[vrefJet_sumet]*1000.;
  tree_evtdef.refGamma_tv2   = TVector2(m_EventVariableHolder[vrefGamma_etx]*1000.,m_EventVariableHolder[vrefGamma_ety]*1000.);
  tree_evtdef.refGamma_sumet = m_EventVariableHolder[vrefGamma_sumet]*1000.;
  tree_evtdef.softTerm_tv2   = TVector2(m_EventVariableHolder[vsoftTerm_etx]*1000.,m_EventVariableHolder[vsoftTerm_ety]*1000.);
  tree_evtdef.softTerm_sumet = m_EventVariableHolder[vsoftTerm_sumet]*1000.;

  tree_evtdef.BuildEvent();
  tree_evtdef.valid = true;
  tree_evtdef.MakeCopy(evtdef);

  return;
}
