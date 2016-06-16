#include <pennSoftLepton/xAODWrapper.h>
#ifndef SKIP_XAOD_COMPILATION

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

#include "pennSoftLepton/EventDefinition.h"
#include "pennSoftLepton/HelpersXaod.h"
#include <xAODRootAccess/TEvent.h>

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigConfxAOD/xAODConfigTool.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"

#include <xAODCaloEvent/CaloClusterContainer.h>
#include <xAODCaloEvent/CaloClusterAuxContainer.h>

//#include <gperftools/profiler.h>

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::xAODWrapper)

bool PSL::xAODWrapper::passLikelihood(int icontainer,ElectronID::ElectronID id){
  if (!electrons) return false;
  if (icontainer < 0 || icontainer+1 > (int)electrons->size()) return false;
  if (!m_lhtools.count(id)) { MSG_DEBUG("Error! LH not initialized!"); return false;}
  m_lhtools[id]->calculate(getElectron(icontainer));
  return static_cast<bool>(m_lhtools[id]->accept(getElectron(icontainer)));
}

double PSL::xAODWrapper::getLikelihoodResult(int i,ElectronID::ElectronID id){
  if (!electrons) return 0.;
  //return getElectron(i)->passSelection(passesMenu,"LHTight");
  if (!m_lhtools.count(id)) { MSG_DEBUG("Error! LH not initialized!"); return 0.;} 
  m_lhtools[id]->calculate(getElectron(i)); 
  return m_lhtools[id]->getTResult().getResult(0);
}

int PSL::xAODWrapper::ContinuousElectronID(int iptordered){
#ifdef BEFORE_SUSYTOOLS_000611
  if (ptorderedLikelihood(iptordered,ElectronID::TightLLH )) return 0.;
  if (ptorderedLikelihood(iptordered,ElectronID::MediumLLH)) return 1.;
  if (ptorderedLikelihood(iptordered,ElectronID::LooseLLH )) return 2.;
  if (ptorderedLikelihood(iptordered,ElectronID::VeryLooseLLH )) return 3.;
  return 4;
#else // BEFORE_SUSYTOOLS_000611
  if (ptorderedLikelihood(iptordered,ElectronID::TightLLH         )) return 0.;
  if (ptorderedLikelihood(iptordered,ElectronID::MediumLLH        )) return 1.;
  if (ptorderedLikelihood(iptordered,ElectronID::LooseAndBLayerLLH)) return 2.;
  if (ptorderedLikelihood(iptordered,ElectronID::LooseLLH         )) return 3.;
  if (ptorderedLikelihood(iptordered,ElectronID::VeryLooseLLH     )) return 4.;
  return 5;
#endif // BEFORE_SUSYTOOLS_000611
}

int PSL::xAODWrapper::ContinuousMuonID(int iptordered){
  if (!muons) return false;
  int icontainer = m_evtdef->GetContainerIndex(iptordered);
  const xAOD::Muon* muon = getMuon(icontainer);
  if (!m_muonSelection->passedIDCuts(*muon)) return 4;
  return m_muonSelection->getQuality(*muon);
}

bool PSL::xAODWrapper::ptorderedLikelihood(unsigned int i,ElectronID::ElectronID id){
  if (!electrons) return false;
  if (!m_evtdef) {MSG_INFO("Error: m_evtdef not set. You made a mistake in your code."); exit(1);}
  if (m_evtdef->leps.size() <= i) return 0;
  Particle p = m_evtdef->leps[i];
  if (p.i_cont < 0) return 0;
  if (p.obj_type != ObjType::Electron) return 0;
  return passLikelihood(p.i_cont,id);
}

bool PSL::xAODWrapper::passMuonQuality(int icontainer,xAOD::Muon::Quality q){
  if (!muons) return false;
  if (icontainer < 0 || icontainer+1 > (int)muons->size()) return false;
  const xAOD::Muon* muon = getMuon(icontainer);
  if (!m_muonSelection->passedIDCuts(*muon)) return false;
  if (m_muonSelection->getQuality(*muon) > q) return false;
  return true;
}

bool PSL::xAODWrapper::ptorderedPassMuonQuality(int i,xAOD::Muon::Quality q){
  if (!muons) return false;
  if (!m_evtdef) {MSG_INFO("Error: m_evtdef not set. You made a mistake in your code."); exit(1);}
  if ((int)m_evtdef->leps.size() <= i) return 0;
  Particle p = m_evtdef->leps[i];
  if (p.i_cont < 0) return 0;
  if (p.obj_type != ObjType::Muon) return 0;
  return passMuonQuality(p.i_cont,q);
}

double PSL::xAODWrapper::GetEventVariable(PSL::Variable v){
  MSG_DEBUG("v is " << PSL::ConvertVarToStr(v));
  if (m_EventVariables.count(v)) return m_EventVariables[v];
  if (!m_evtdef) {MSG_INFO("Error: m_evtdef not set. You made a mistake in your code."); exit(1);}
  double d = -999;
  // begin all of the variable definitions
  if (v == vGRL){
    d = IsMC() ? 1 : (int)(m_grl->passRunLB(eventInfo->runNumber(),eventInfo->lumiBlock()));
  }
  else if (v == vpassLarError         ){
    d = IsMC() ? 1 : (int)(eventInfo->errorState(xAOD::EventInfo::LAr)!=xAOD::EventInfo::Error);
  }
  else if (v == vpassTileError        ){
    d = IsMC() ? 1 : (int)(eventInfo->errorState(xAOD::EventInfo::Tile)!=xAOD::EventInfo::Error);
  }
  else if (v == vpassSctError        ){
    d = IsMC() ? 1 : (int)(eventInfo->errorState(xAOD::EventInfo::SCT)!=xAOD::EventInfo::Error);
  }
  else if (v == vpassCoreBit          ){ 
    d = IsMC() ? 1 : (int)(!eventInfo->isEventFlagBitSet(xAOD::EventInfo::Core, 18));
  }
  else if (v == vpassPrimaryVtx       ) {
    const xAOD::Vertex* pv = GetPrimaryVertex(); d = (pv ? 1 : 0);
  }
  else if (v == vNPV                  ) {
    int npv = GetNPV(); d = npv;
  }
  else if (v == vAverageInteractionsPerCrossing) d = eventInfo->averageInteractionsPerCrossing();
  else if (v == vActualInteractionsPerCrossing) d = eventInfo->actualInteractionsPerCrossing();
  else if (v == vpassTileTripReader   ) d = 1;
  else if (v == vpassIncompleteEvents ) d = 1;
  else if (v == vpassTileCalHotSpot   ) d = 1;
  else if (v == vpassCaloJet          ) d = 1;
  else if (v == vpassHFOR             ) d = 1;
  else if (v == vpassMCOverlap        ) d = 1;
  else if (v == vpassBCH              ) d = 1;
  else if (v == vMuIsValid            ) d = 1;
  else if (v == vPassTrigger          ) d = PassTrigger() ? 1. : 0.;
  else if (v == vLumiBlockMu          ) d = m_LumiBlockMu;
  else if (v == vSample               ) d = m_CurrentSample;
  else if (v == vtruthjet0Pt          ) d = LeadingTruthJet().Pt()/1000.;
  else if (v == vtruthjet1Pt          ) d = SubleadingTruthJet().Pt()/1000.;
  else if (v == vTotalWeight          ) d = 0; // Keep this 0! see implementation in Ntupler,PassEvent
  else if (v == vTotalWeightNoSF      ) d = m_weight_pu;
  else if (v == vTotalWeightNoPupNoSF ) d = m_weight;
  else if (v == vmc_channel_number    ) d = IsData() ? 0 : eventInfo->mcChannelNumber();
  else if (v == vptAvg                ) d = (m_evtdef->LeadingJet().Pt()+m_evtdef->SubleadingJet().Pt())/2000.;
  else if (v == vtruthptAvg           ) d = (LeadingTruthJet().Pt()+SubleadingTruthJet().Pt())/2000.;
  else if (v == vptAvgOverTruthjet0Pt ) d = GetEventVariable(vptAvg)/GetEventVariable(vtruthjet0Pt);
  else if (v == vDeltaPhill           ) d = m_evtdef->DeltaPhill();
  else if (v == vWlepPassWSelection   ) d = m_evtdef->WlepPassWSelection();
  else if (v == vPassTriggerEle       ) d = m_evtdef->PassTriggerEle();
  else if (v == vPassTriggerMu        ) d = m_evtdef->PassTriggerMu();
  else if (v == vPassTriggerMatch     ) d = m_evtdef->PassTriggerMatch();
  else if (v == vRunNumber            ) d = eventInfo->runNumber();
  else if (v == vEvtTrigSF            ) d = getTriggerSF_Winter2016WZAnalysis();
  else if (v == vAntiIDSF_lep3        ) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 1.;
    else if(m_doRecoAntiID) d = CalculateZZAntiIDScaleFactor(GetMatchingRecoLepton(GetZZAntiIDParticle()));
    else if(m_doXCheckAntiID) d= CalculateZZAntiIDScaleFactor(GetZZAntiIDParticleFromReco());
    else d = CalculateZZAntiIDScaleFactor(GetZZAntiIDParticle());
  }
  else if (v == vAntiIDSF_lep3_pt      ) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else d = GetZZAntiIDParticle().tlv.Pt();
  }
  else if (v == vAntiIDSF_lep3_eta     ) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else d = GetZZAntiIDParticle().tlv.Eta();
  }
  else if (v == vAntiIDSF_lep3_phi     ) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else d = GetZZAntiIDParticle().tlv.Phi();
  }
  else if (v == vAntiIDSF_lep3_flavor  ) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else if (GetZZAntiIDParticle().obj_type == ObjType::Electron) d = 1;
    else if (GetZZAntiIDParticle().obj_type == ObjType::Muon) d = 2;
    else d = 0;
  }
  else if (v == vAntiIDSF_lep3_pt_reco ) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else d = GetMatchingRecoLepton(GetZZAntiIDParticle()).tlv.Pt();
  }
  else if (v == vAntiIDSF_lep3_eta_reco) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else d = GetMatchingRecoLepton(GetZZAntiIDParticle()).tlv.Eta();
  }
  else if (v == vAntiIDSF_lep3_phi_reco) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else d = GetMatchingRecoLepton(GetZZAntiIDParticle()).tlv.Phi();
  }
  else if (v == vAntiIDSF_lep3_flavor_reco) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else if (GetMatchingRecoLepton(GetZZAntiIDParticle()).obj_type == ObjType::Electron) d = 1;
    else if (GetMatchingRecoLepton(GetZZAntiIDParticle()).obj_type == ObjType::Muon) d = 2;
    else d = 0;
  }
  else if (v == vAntiIDSF_lep3_match_DR) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else d = GetMatchingRecoLepton(GetZZAntiIDParticle()).tlv.DeltaR(GetZZAntiIDParticle().tlv);
  }
   else if (v == vAntiIDSF_lep3_origin){
     if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else if (GetMatchingRecoLepton(GetZZAntiIDParticle()).obj_type == ObjType::Electron){
      const xAOD::Electron* ele = getElectron(GetMatchingRecoLepton(GetZZAntiIDParticle()).i_cont);
      d = TruthOrigin(*ele);
    }
    else if (GetMatchingRecoLepton(GetZZAntiIDParticle()).obj_type == ObjType::Muon){
      if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
      const xAOD::TrackParticle* track = getMuon(GetMatchingRecoLepton(GetZZAntiIDParticle()).i_cont)->primaryTrackParticle();
      if (!track) return -1;
      const xAOD::TruthParticle *tp = xAOD::TruthHelpers::getTruthParticle(*track);
      if (m_acc_truthType.isAvailable(*track)) d = TruthOrigin(*track);
      else if (tp && m_acc_particleType.isAvailable(*tp)) d = m_acc_particleOrigin(*tp);
    }
    else d = -1;
  }
  else if (v == vAntiIDSF_lep3_type){
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else if (GetMatchingRecoLepton(GetZZAntiIDParticle()).obj_type == ObjType::Electron){
      const xAOD::Electron* ele = getElectron(GetMatchingRecoLepton(GetZZAntiIDParticle()).i_cont);
      d = TruthType(*ele);
    }
    else if (GetMatchingRecoLepton(GetZZAntiIDParticle()).obj_type == ObjType::Muon){
      const xAOD::TrackParticle* track = getMuon(GetMatchingRecoLepton(GetZZAntiIDParticle()).i_cont)->primaryTrackParticle();
      if (!track) return -1;
      const xAOD::TruthParticle *tp = xAOD::TruthHelpers::getTruthParticle(*track);
      if (m_acc_truthType.isAvailable(*track)) d = TruthType(*track);
      else if (tp && m_acc_particleType.isAvailable(*tp)) d = m_acc_particleType(*tp);
    }
    else d = -1;
  }
  else if (v == vNlepsFromZ){
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else d = GetNLepsfromZ(GetZZAntiIDParticle());
  }
  else if (v == vAntiIDSF_lep3_truth_origin){
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else if (GetZZAntiIDParticle().obj_type == ObjType::Electron){
      const xAOD::TruthParticle* tmp_ele = getTruthElectron(GetZZAntiIDParticle().i_cont);
      d = ParticleOrigin(*tmp_ele);
    }
    else if (GetZZAntiIDParticle().obj_type == ObjType::Muon){
      const xAOD::TruthParticle* tmp_mu = getTruthMuon(GetZZAntiIDParticle().i_cont);
      d = ParticleOrigin(*tmp_mu);
    }
    else d = -1;
  }
  else if (v == vAntiIDSF_lep3_truth_type){
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else if (GetZZAntiIDParticle().obj_type == ObjType::Electron){
      const xAOD::TruthParticle* tmp_ele = getTruthElectron(GetZZAntiIDParticle().i_cont);
      d = ParticleType(*tmp_ele);
    }
    else if (GetZZAntiIDParticle().obj_type == ObjType::Muon){
      const xAOD::TruthParticle* tmp_mu = getTruthMuon(GetZZAntiIDParticle().i_cont);
      d = ParticleType(*tmp_mu);
    }
    else d = -1;
  }
  else if (v == vXCheckAntiIDSF_lep3_pt      ) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else d = GetZZAntiIDParticleFromReco().tlv.Pt();
  }
  else if (v == vXCheckAntiIDSF_lep3_eta     ) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else d = GetZZAntiIDParticleFromReco().tlv.Eta();
  }
  else if (v == vXCheckAntiIDSF_lep3_phi     ) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else d = GetZZAntiIDParticleFromReco().tlv.Phi();
  }
  else if (v == vXCheckAntiIDSF_lep3_flavor  ) {
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else if (GetZZAntiIDParticleFromReco().obj_type == ObjType::Electron) d = 1;
    else if (GetZZAntiIDParticleFromReco().obj_type == ObjType::Muon) d = 2;
    else d = 0;
  }
  else if (v == vXCheckAntiIDSF_lep3_origin){
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else if (GetZZAntiIDParticleFromReco().obj_type == ObjType::Electron) d = TruthOrigin(*getElectron(GetZZAntiIDParticleFromReco().i_cont));
    else if (GetZZAntiIDParticle().obj_type == ObjType::Muon){
      const xAOD::TrackParticle* track = getMuon(GetZZAntiIDParticleFromReco().i_cont)->primaryTrackParticle();
      if (!track) return -1;
      const xAOD::TruthParticle *tp = xAOD::TruthHelpers::getTruthParticle(*track);
      if (m_acc_truthType.isAvailable(*track)) d = TruthOrigin(*track);
      else if (tp && m_acc_particleType.isAvailable(*tp)) d = m_acc_particleOrigin(*tp);
    }
    else d = -1;
  }
  else if (v == vXCheckAntiIDSF_lep3_type){
    if (m_CurrentSample != kzz || !m_CalculateZZAntiid) d = 0;
    else if (GetZZAntiIDParticleFromReco().obj_type == ObjType::Electron)d = TruthType(*getElectron(GetZZAntiIDParticleFromReco().i_cont));
    else if (GetZZAntiIDParticleFromReco().obj_type == ObjType::Muon){
      const xAOD::TrackParticle* track = getMuon(GetZZAntiIDParticleFromReco().i_cont)->primaryTrackParticle();
      if (!track) return -1;
      const xAOD::TruthParticle *tp = xAOD::TruthHelpers::getTruthParticle(*track);
      if (m_acc_truthType.isAvailable(*track)) d = TruthType(*track);
      else if (tp && m_acc_particleType.isAvailable(*tp)) d = m_acc_particleType(*tp);
    }
    else d = -1;
  }

  // Add_new_variables_here to GetEventVariable
  //
  // If it is a object-definition-dependent variable, get it from the EventDefinition.
  //
  else d = m_evtdef->GetEventVariable(v);
  // end all of the variable definitions
  m_EventVariables[v] = d;
  return d;
}

double PSL::xAODWrapper::GetSignalIsolation(int iptordered,AnaIso::AnaIso isotype_ele,AnaIso::AnaIso isotype_mu,bool isAntiId){
  if (!m_evtdef) {MSG_INFO("Error: m_evtdef not set. You made a mistake in your code."); exit(1);}
  Particle p;
  if (isAntiId) p = m_evtdef->PtOrderedAntiLepton(iptordered);
  else p = m_evtdef->PtOrderedLepton(iptordered);
  if (!p.tlv.Pt()) return 999.;
  if (p.obj_type == ObjType::Muon) {
    return MuonIsolation(p.i_cont,isotype_mu);
  }
  // otherwise it's an electron
  return EleIsolation(p.i_cont,isotype_ele);
}

TLorentzVector PSL::xAODWrapper::GetSignalLeptonTLV(int iptordered){
  if (!m_evtdef) {MSG_INFO("Error: m_evtdef not set. You made a mistake in your code."); exit(1);}
  return m_evtdef->PtOrderedLepton(iptordered).tlv;
}

int PSL::xAODWrapper::GetSignalLeptonContainerIndex(int iptordered){
  if (!m_evtdef) {MSG_INFO("Error: m_evtdef not set. You made a mistake in your code."); exit(1);}
  return m_evtdef->PtOrderedLepton(iptordered).i_cont;
}

double PSL::xAODWrapper::GetSignalLeptonVariable(PSL::LepVariable lepv,int iptordered,bool isAntiId){
  if (!m_evtdef) {MSG_INFO("Error: m_evtdef not set. You made a mistake in your code."); exit(1);}
  if (m_LepVariables.count(lepv) && m_LepVariables[lepv].count(iptordered)) return m_LepVariables[lepv][iptordered];

  if (iptordered < 0) return -999.;

  Particle p;
  if (isAntiId) p = m_evtdef->PtOrderedAntiLepton(0);
  else p = m_evtdef->PtOrderedLepton(iptordered);

  MSG_DEBUG("Particle has momentum " << p.tlv.Pt() << " and index " << p.i_cont);
  if (!p.tlv.Pt()) return -999.;
  double d = -999;
  if      (lepv == lepFlavor          ) d = p.obj_type; // 1 = ele, 2 = mu0;
  else if (lepv == lepPt              ) d = p.Pt()/1000.;
  else if (lepv == lepEta             ) d = p.Eta();
  else if (lepv == lepPhi             ) d = p.Phi();
  else if (lepv == lepTrkIso          ) d = GetSignalIsolation(iptordered,m_el_defaulttrkiso,m_mu_defaulttrkiso);
  else if (lepv == lepCaloIso         ) d = GetSignalIsolation(iptordered,m_el_defaultcaloiso,m_mu_defaultcaloiso);
  else if (lepv == lepSFRecoIdIso     ) d = p.sf_recoIdIso;
  else if (lepv == lepSFTrigger       ) d = p.sf_trig;
  else if (lepv == lepCharge          ) d = p.charge;
  else if (lepv == lepMt              ) d = m_evtdef->Mt_singleLepton(p.tlv)/1000.;
  else if (lepv == lepMll             ) d = MllForEwkRejection(iptordered);
  else if (lepv == lepPassOR          ) d = p.passOR;
  else if (lepv == lepMatchesTrigger  ) d = p.is_trigMatched;
  else if (p.obj_type == ObjType::Muon) {
    if      (lepv == lepD0Significance) d = MuonD0Significance(p.i_cont);
    else if (lepv == lepZ0SinTheta    ) d = fabs(MuonZ0(p.i_cont)*sin(p.tlv.Theta()));
    else if (lepv == lepTruthDetailed ) d = getMuonTruthLabel(p.i_cont);
    else if (lepv == lepD0              ) d = MuonD0(p.i_cont);
    else if (lepv == lepElectronID      ) d = 5;
    else if (lepv == lepMuonID          ) d = ContinuousMuonID(iptordered);
    else if (lepv == lepPassBlayer      ) d = 0;
    else if (lepv == lepEleEtaBE        ) d = 0;
    else if (lepv == lepOrigin          ){
      if (IsData()) d = -999;
      else{
	const xAOD::TrackParticle* track = getMuon(p.i_cont)->primaryTrackParticle();
	const xAOD::TruthParticle *tp = xAOD::TruthHelpers::getTruthParticle(*track);
	if (m_acc_truthType.isAvailable(*track)) d = TruthOrigin(*track);
	else if (tp && m_acc_particleType.isAvailable(*tp)) d = m_acc_particleOrigin(*tp);    
      }
    }
    else if (lepv == lepType            ){
      if (IsData()) d = -999;
      else{
	const xAOD::TrackParticle* track = getMuon(p.i_cont)->primaryTrackParticle();
	const xAOD::TruthParticle *tp = xAOD::TruthHelpers::getTruthParticle(*track);
	if (m_acc_truthType.isAvailable(*track)) d  = TruthType(*track);
	else if (tp && m_acc_particleType.isAvailable(*tp)) d = m_acc_particleType(*tp);
      }
    }
    else MSG_INFO("Error! LepVariable not implemented in xAODWrapper::GetSignalLeptonVariable: " << ConvertLepVarToStr(lepv));
  }
  else if (p.obj_type == ObjType::Electron) {
    if      (lepv == lepD0Significance) d =  EleD0Significance(p.i_cont);
    else if (lepv == lepZ0SinTheta    ) d = fabs(EleZ0(p.i_cont)*sin(p.tlv.Theta()));
    else if (lepv == lepTruthDetailed ) d =  getElectronTruthLabel(p.i_cont);
    else if (lepv == lepD0              ) d = EleD0(p.i_cont);
    else if (lepv == lepElectronID      ) d = ContinuousElectronID(iptordered);
    else if (lepv == lepMuonID          ) d = 4;
    else if (lepv == lepPassBlayer      ) d = EleBlayer(p.i_cont);
    else if (lepv == lepEleEtaBE        ) d = (electrons) ? getElectron(p.i_cont)->caloCluster()->etaBE(2) : -999;
    else if (lepv == lepOrigin          ){
      if (IsData()) d = -999;
      else{
	const xAOD::Electron* ele = getElectron(p.i_cont);
	d = TruthOrigin(*ele);
      }
    }
    else if (lepv == lepType            ){
      if (IsData()) d = -999;
      else{
        const xAOD::Electron* ele = getElectron(p.i_cont);
        d = TruthType(*ele);
      }
    }
    else MSG_INFO("Error! LepVariable not implemented in xAODWrapper::GetSignalLeptonVariable: " << ConvertLepVarToStr(lepv));
  }
  if (!m_LepVariables.count(lepv)) {
    std::map<int,double> inner;
    inner.insert(std::make_pair(iptordered,d));
    m_LepVariables.insert(std::make_pair(lepv,inner));
  }
  else {
    m_LepVariables[lepv][iptordered] = d;
  }
  return d;
}

double PSL::xAODWrapper::GetSignalLeptonVariable(PSL::LepVariable v,PSL::Variable lepindex){
  if (lepindex == vantilep0_index) return GetSignalLeptonVariable(v,0,true);
  return GetSignalLeptonVariable(v,GetEventVariable(lepindex),false);
}

double PSL::xAODWrapper::GetJetVariable(PSL::JetVariable v,int iptordered){
  if (!m_evtdef) {MSG_INFO("Error: m_evtdef not set. You made a mistake in your code."); exit(1);}
  if (iptordered < 0) return -999.;
  Particle p = m_evtdef->PtOrderedJet(iptordered);
  if (!p.tlv.Pt()) return -999.;
  if      (v == jetPt              ) return p.Pt()/1000.;
  else if (v == jetEta             ) return p.Eta();
  else if (v == jetPhi             ) return p.Phi();
  else if (v == jetE               ) return p.E()/1000.;
  return -999.;
}

void PSL::xAODWrapper::ConfigureRun(unsigned int runNumber,std::string samp,float xsec_k_f,float sumw){
  Sample s = ConvertToSample(samp);
  m_run_sample_map[runNumber] = s;
  m_run_xsec_map[runNumber] = xsec_k_f;
  m_run_sumw_map[runNumber] = sumw;
  return;
}

void PSL::xAODWrapper::PrintConfiguration(void){
  MSG_INFO("-------------------------------xAODWrapper Configuration:------------------------------");
  MSG_INFO("This Instance Name      : " << GetName()              );
  MSG_INFO("luminosity (pb)         : " << m_lumi_pb);
  MSG_INFO("grlfile                 : " << m_grlfile);
  //MSG_INFO("prwConfFiles            : " << m_prwConfFiles);
  MSG_INFO("prwLcalcFiles           : " << m_prwLcalcFiles);
  MSG_INFO("pu_rescaling            : " << m_pu_rescaling);
  MSG_INFO("allowUnlabeledSamples   : " << (m_allowUnlabeledSamples ? "yes" : "no"));
  MSG_INFO("el_defaultcaloiso       : " << ConvertAnaIsoToStr(m_el_defaultcaloiso));
  MSG_INFO("el_defaulttrkiso        : " << ConvertAnaIsoToStr(m_el_defaulttrkiso ));
  MSG_INFO("mu_defaultcaloiso       : " << ConvertAnaIsoToStr(m_mu_defaultcaloiso));
  MSG_INFO("mu_defaulttrkiso        : " << ConvertAnaIsoToStr(m_mu_defaulttrkiso ));
  MSG_INFO("RemoveZgammaFromZjet    : " << (m_RemoveZgammaFromZjet ? "yes" : "no"));
  for (unsigned int i=0;i<m_trigger_bitmask.size();++i){
    if (i == 0) {
      MSG_INFO("Triggers                : " << ConvertTrigToStr(m_trigger_bitmask[i]));
    }
    else MSG_INFO("                          " << ConvertTrigToStr(m_trigger_bitmask[i]));
  }
  MSG_INFO("---------------------------------------------------------------------------------------");
}

void PSL::xAODWrapper::PrintRunConfiguration(void){
  for (std::map<unsigned int,unsigned int>::const_iterator it=m_run_sample_map.begin();
       it!=m_run_sample_map.end();++it){
    MSG_INFO("Run " << it->first << ": " << ConvertSampleToStr(static_cast<PSL::Sample>(it->second))
             << " xsec : " << m_run_xsec_map[it->first]
             << " sumw : " << m_run_sumw_map[it->first]);
  }
}

PSL::xAODWrapper::xAODWrapper() : EL::Algorithm()
                                ,m_lumi_pb(20276.9)
                                ,m_grlfile("SUSYTools/data/GRL/Summer2013/data12_8TeV.periodAllYear_DetStatus-v61-pro14-02_DQDefects-00-01-00_PHYS_StandardGRL_All_Good.xml")
                                // ,m_prwDefaultChannel(0)
                                // ,m_prwConfFiles("")
                                ,m_prwLcalcFiles("")
                                ,m_muUncert(0.2)
                                ,m_pu_rescaling(1.0)
                                ,m_el_defaultcaloiso(AnaIso::None)
                                ,m_el_defaulttrkiso (AnaIso::None)
                                ,m_mu_defaultcaloiso(AnaIso::None)
                                ,m_mu_defaulttrkiso (AnaIso::None)
                                ,m_RemoveZgammaFromZjet(false)
                                ,m_CalculateZZAntiid(false)
                                ,m_doRecoAntiID(false)
                                ,m_EgammaToys(false)
                                ,m_CurrentSample(knone)
                                ,m_evtdef(0)
                                  // start of xAOD initializers
                                ,m_event(0)
                                ,eventInfo(0)
                                ,jets(0)
                                ,jets_aux(0)
                                ,electrons(0)
                                ,electrons_aux(0)
                                ,muons(0)
                                ,muons_aux(0)
                                ,taus(0)
                                ,taus_aux(0)
                                ,photons(0)
                                ,photons_aux(0)
                                ,met(0)
                                ,met_aux(0)
                                ,truthjets(0)
                                ,truthmet(0)
                                ,truthparticles(0)
                                ,truthelectrons(0)
                                ,truthmuons(0)
                                ,truthtaus(0)
                                ,truthneutrinos(0)
                                ,truthphotons(0)
                                ,m_trigConfTool(0)
                                ,m_trigDecTool(0)
                                ,m_grl(0)
                                ,m_prwTool(0)
                                ,m_muonTriggerSFTool(0)
                                ,m_muonTriggerSFToolNOSYST(0)
                                ,m_acc_particleType      ("classifierParticleType")
                                ,m_acc_particleOrigin    ("classifierParticleOrigin")
                                ,m_acc_particleOutCome   ("classifierParticleOutCome")
                                ,m_acc_motherID          ("motherID")
                                ,m_acc_truthType("truthType")
                                ,m_acc_truthOrigin("truthOrigin")
                                //,m_acc_truthType("classifierParticleType")
                                //,m_acc_truthOrigin("classifierParticleOrigin")
                                ,m_acc_e_dressed("e_dressed")
                                ,m_acc_pt_dressed("pt_dressed")
                                ,m_acc_eta_dressed("eta_dressed")
                                ,m_acc_phi_dressed("phi_dressed")
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
  MSG_DEBUG("Constructor");
  SetName("PSL_EDM_CLASS");
}

EL::StatusCode PSL::xAODWrapper::setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.
  MSG_DEBUG("setupJob");
  (void)job;
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::xAODWrapper::histInitialize()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.
  MSG_INFO("histInitialize - initializing internal tools.");
  MSG_DEBUG("my name is " << GetName());

  m_EventVariables.clear();
  m_LepVariables.clear();
  m_xaod_warnings.clear();
  n_processed = 0;
  m_sumw_thisjob = 0;

  std::string lhBaseDir = "ElectronPhotonSelectorTools/offline/";
  std::map<PSL::ElectronID::ElectronID,std::string> lhconfs;
#ifndef ISREL20
  lhconfs[ElectronID::TightLLH    ] = lhBaseDir+"dc14b_20150121/ElectronLikelihoodTightOfflineConfig2015.conf"    ;
  lhconfs[ElectronID::MediumLLH   ] = lhBaseDir+"dc14b_20150121/ElectronLikelihoodMediumOfflineConfig2015.conf"   ;
  lhconfs[ElectronID::LooseLLH    ] = lhBaseDir+"dc14b_20150121/ElectronLikelihoodLooseOfflineConfig2015.conf"    ;
  lhconfs[ElectronID::VeryLooseLLH] = lhBaseDir+"dc14b_20150121/ElectronLikelihoodVeryLooseOfflineConfig2015.conf";
#elif defined(BEFORE_SUSYTOOLS_000624)
  lhconfs[ElectronID::TightLLH    ] = lhBaseDir+"mc15_20150429/ElectronLikelihoodTightOfflineConfig2015.conf"    ;
  lhconfs[ElectronID::MediumLLH   ] = lhBaseDir+"mc15_20150429/ElectronLikelihoodMediumOfflineConfig2015.conf"   ;
  lhconfs[ElectronID::LooseLLH    ] = lhBaseDir+"mc15_20150429/ElectronLikelihoodLooseOfflineConfig2015.conf"    ;
  lhconfs[ElectronID::VeryLooseLLH] = lhBaseDir+"mc15_20150429/ElectronLikelihoodVeryLooseOfflineConfig2015.conf";
#else // SUSYTools-00-06-25 and later
  lhconfs[ElectronID::TightLLH         ] = lhBaseDir+"mc15_20150712/ElectronLikelihoodTightOfflineConfig2015.conf"    ;
  lhconfs[ElectronID::MediumLLH        ] = lhBaseDir+"mc15_20150712/ElectronLikelihoodMediumOfflineConfig2015.conf"   ;
  lhconfs[ElectronID::LooseLLH         ] = lhBaseDir+"mc15_20150712/ElectronLikelihoodLooseOfflineConfig2015.conf"    ;
  lhconfs[ElectronID::VeryLooseLLH     ] = lhBaseDir+"mc15_20150712/ElectronLikelihoodVeryLooseOfflineConfig2015.conf";
  lhconfs[ElectronID::LooseAndBLayerLLH] = lhBaseDir+"mc15_20150712/ElectronLikelihoodLooseOfflineConfig2015_CutBL.conf";
#endif
  
  m_lhtools[ElectronID::TightLLH         ] = new AsgElectronLikelihoodTool("xAODWrapper_TightLLH");
  m_lhtools[ElectronID::MediumLLH        ] = new AsgElectronLikelihoodTool("xAODWrapper_MediumLLH");
  m_lhtools[ElectronID::LooseLLH         ] = new AsgElectronLikelihoodTool("xAODWrapper_LooseLLH");
  m_lhtools[ElectronID::VeryLooseLLH     ] = new AsgElectronLikelihoodTool("xAODWrapper_VeryLooseLLH");
  m_lhtools[ElectronID::LooseAndBLayerLLH] = new AsgElectronLikelihoodTool("xAODWrapper_LooseAndBLayerLLH");
  for (auto it=m_lhtools.begin();it != m_lhtools.end();++it){
    it->second->setProperty("primaryVertexContainer","PrimaryVertices" ).isSuccess();
    MSG_INFO("Setting up ElectronLikelihood with config file " << lhconfs[it->first]);
    it->second->setProperty("ConfigFile",lhconfs[it->first]).isSuccess();
    it->second->msg().setLevel( MSG::WARNING );
    if (!it->second->initialize().isSuccess()) return EL::StatusCode::FAILURE;
  }

  //
  // Electron Scale factors!
  //
  std::map<PSL::ElectronSF,std::string> sf_pairs;
  sf_pairs[PhilipAntiID                                     ] = "efficiencySF.offline.LooseAndBLayerLLH_d0z0";
  sf_pairs[ElectronSF_RecoTrk                               ] = "efficiencySF.offline.RecoTrk"                              ;
#ifndef BEFORE_SUSYTOOLS_000709
  sf_pairs[ElectronSF_LooseAndBLayerLLH                     ] = "efficiencySF.offline.LooseAndBLayerLLH"                    ;
  sf_pairs[ElectronSF_LooseAndBLayerLLH_d0z0                ] = "efficiencySF.offline.LooseAndBLayerLLH_d0z0"               ;
  sf_pairs[ElectronSF_TightLLH_d0z0                         ] = "efficiencySF.offline.TightLLH_d0z0"                        ;
  sf_pairs[ElectronSF_MediumLLH_d0z0                        ] = "efficiencySF.offline.MediumLLH_d0z0"                       ;
  sf_pairs[ElectronIsoSF_MediumLLH_d0z0_v8_isolGradientLoose] = "efficiencySF.Isolation.MediumLLH_d0z0_v8_isolGradientLoose";
  sf_pairs[ElectronIsoSF_TightLLH_d0z0_v8_isolGradient      ] = "efficiencySF.Isolation.TightLLH_d0z0_v8_isolGradient"      ;
  sf_pairs[ElectronIsoSF_LooseAndBLayerLLH_d0z0_v8_isolLooseTrackOnly] = "efficiencySF.Isolation.LooseAndBLayerLLH_d0z0_v8_isolLooseTrackOnly";
  sf_pairs[ElectronSF_Trigger_MediumLLH_isolGradientLoose]    = "efficiencySF.e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose.MediumLLH_d0z0_v8_isolGradient"; // smwz z-electron (data), Gradient for now
  sf_pairs[ElectronSF_TriggerMC_MediumLLH_isolGradientLoose]  = "efficiency.e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose.MediumLLH_d0z0_v8_isolGradient"; // smwz z-electron (mc), Gradient for now
#ifdef BEFORE_ANALYSISBASE_2p3p41 // some operating points just recently became available
  sf_pairs[ElectronSF_Trigger_TightLLH_isolGradient]          = "efficiencySF.e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose.MediumLLH_d0z0_v8_isolGradient"; // Wrong but the only option before 2p3p41
  sf_pairs[ElectronSF_TriggerMC_TightLLH_isolGradient]        = "efficiency.e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose.MediumLLH_d0z0_v8_isolGradient"; // Wrong but the only option before 2p3p41
#else
  sf_pairs[ElectronSF_Trigger_TightLLH_isolGradient]          = "efficiencySF.e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose.TightLLH_d0z0_v8_isolGradient"; // Correct for Moriond
  sf_pairs[ElectronSF_TriggerMC_TightLLH_isolGradient]        = "efficiency.e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose.TightLLH_d0z0_v8_isolGradient"; // Correct for Moriond
#endif // BEFORE_ANALYSISBASE_2p3p41
#endif // BEFORE_SUSYTOOLS_000709

#ifdef BEFORE_SUSYTOOLS_000611
  std::string eff_version = "2015.13TeV.rel19.GEO21.v01";
#elif defined(BEFORE_SUSYTOOLS_000709)
  std::string eff_version = "2015.13TeV.rel20p0.v01";
#elif defined(BEFORE_ANALYSISBASE_2p3p41)
  std::string eff_version = "2015.13TeV.rel20p0.25ns.v02";
#else
  std::string eff_version = "2015.13TeV.rel20p0.25ns.v04";
#endif

  PATCore::ParticleDataType::DataType data_type(PATCore::ParticleDataType::Data);
  if (wk()->metaData()->castString("Simulation") == "atlfast") data_type = PATCore::ParticleDataType::Fast;
  if (wk()->metaData()->castString("Simulation") == "fullsim") data_type = PATCore::ParticleDataType::Full;

  for (auto it=sf_pairs.begin();it != sf_pairs.end();++it) {
    MSG_INFO("Setting up Electron SF " << it->second);
    std::string tool_name = it->second;
    // The next lines are a really dumb hack intended to
    // stop SUSYTools from initializing duplicate tools. Should not affect performance.
    if (it->first == PhilipAntiID                                              ) tool_name = "AsgElectronEfficiencyCorrectionTool_trigEff_singleLep_TightLLH";          
    if (it->first == ElectronSF_RecoTrk                                        ) tool_name = "AsgElectronEfficiencyCorrectionTool_trigEff_diLep_TightLLH";              
    if (it->first == ElectronSF_LooseAndBLayerLLH                              ) tool_name = "AsgElectronEfficiencyCorrectionTool_trigEff_mixLep_TightLLH";             
    if (it->first == ElectronSF_LooseAndBLayerLLH_d0z0                         ) tool_name = "AsgElectronEfficiencyCorrectionTool_trigEff_singleLep_fallback_MediumLLH";
    if (it->first == ElectronSF_MediumLLH_d0z0                                 ) tool_name = "AsgElectronEfficiencyCorrectionTool_trigEff_diLep_fallback_MediumLLH";    
    if (it->first == ElectronIsoSF_MediumLLH_d0z0_v8_isolGradientLoose         ) tool_name = "AsgElectronEfficiencyCorrectionTool_trigEff_mixLep_fallback_MediumLLH";   
    if (it->first == ElectronIsoSF_TightLLH_d0z0_v8_isolGradient               ) tool_name = "AsgElectronEfficiencyCorrectionTool_id_fallback_MediumLLH";               
    if (it->first == ElectronIsoSF_LooseAndBLayerLLH_d0z0_v8_isolLooseTrackOnly) tool_name = "AsgElectronEfficiencyCorrectionTool_trig_singleLep_fallback_MediumLLH";   
    if (it->first == ElectronSF_Trigger_MediumLLH_isolGradientLoose            ) tool_name = "AsgElectronEfficiencyCorrectionTool_trig_diLep_fallback_MediumLLH";       
    if (it->first == ElectronSF_TriggerMC_MediumLLH_isolGradientLoose          ) tool_name = "AsgElectronEfficiencyCorrectionTool_trig_mixLep_fallback_MediumLLH";
    if (it->first == ElectronSF_Trigger_TightLLH_isolGradient                  ) tool_name = "AsgElectronEfficiencyCorrectionTool_iso_fallback_MediumLLHGradientLoose";

    m_elesfs[it->first] = new AsgElectronEfficiencyCorrectionTool(tool_name);
    std::vector<std::string> tmp_list;
    TString s = Form("ElectronEfficiencyCorrection/%s.%s.root",(it->second).c_str(),eff_version.c_str());
    if ( (it->first) == PhilipAntiID ) s = Form("pennSoftLepton/%s.%s.root",(it->second).c_str(),eff_version.c_str());
    tmp_list.push_back(std::string(s));
    m_elesfs[it->first]->msg().setLevel(MSG::ERROR);
    m_elesfs[it->first]->setProperty("CorrectionFileNameList",tmp_list).isSuccess();
    m_elesfs[it->first]->setProperty("ForceDataType",(int) data_type).isSuccess();
    if (m_EgammaToys) {
      m_elesfs[it->first]->setProperty("CorrelationModel", "COMBMCTOYS").isSuccess();
      m_elesfs[it->first]->setProperty("NumberOfToys", 40).isSuccess();
      m_elesfs[it->first]->setProperty("MCToySeeD", 10).isSuccess();
    }
    if (!m_elesfs[it->first]->initialize().isSuccess()) return EL::StatusCode::FAILURE;
  }

  //
  // Muon Scale Factors!
  // Medium
  /* m_muonsfs[MuonSF_Medium                  ] = new CP::MuonEfficiencyScaleFactors("MuonSF_Medium");
  m_muonsfs[MuonSF_Medium                  ]->setProperty("WorkingPoint","Medium").isSuccess();
  if (!m_muonsfs[MuonSF_Medium             ]->initialize().isSuccess()) return EL::StatusCode::FAILURE;
  // Loose
  m_muonsfs[MuonSF_Loose                   ] = new CP::MuonEfficiencyScaleFactors("MuonSF_Loose");
  m_muonsfs[MuonSF_Loose                   ]->setProperty("WorkingPoint","Loose").isSuccess();
  if (!m_muonsfs[MuonSF_Loose              ]->initialize().isSuccess()) return EL::StatusCode::FAILURE;
  // Loose NOSYST
  m_muonsfs[MuonSF_Loose_NOSYST            ] = new CP::MuonEfficiencyScaleFactors("MuonSF_Loose_NOSYST");
  m_muonsfs[MuonSF_Loose_NOSYST            ]->setProperty("WorkingPoint","Loose").isSuccess();
  if (!m_muonsfs[MuonSF_Loose_NOSYST       ]->initialize().isSuccess()) return EL::StatusCode::FAILURE;
  // isolation scale factor - GradientLoose
  m_muonsfs[MuonSF_isolGradientLoose] = new CP::MuonEfficiencyScaleFactors("MuonSF_isolGradientLoose");
  m_muonsfs[MuonSF_isolGradientLoose]->setProperty("WorkingPoint","GradientLooseIso").isSuccess();
  if (!m_muonsfs[MuonSF_isolGradientLoose]->initialize().isSuccess()) return EL::StatusCode::FAILURE;
  // isolation scale factor - LooseTrackOnly
  m_muonsfs[MuonSF_isolLooseTrackOnly] = new CP::MuonEfficiencyScaleFactors("MuonSF_isolLooseTrackOnly");
  m_muonsfs[MuonSF_isolLooseTrackOnly]->setProperty("WorkingPoint","LooseTrackOnlyIso").isSuccess();
  if (!m_muonsfs[MuonSF_isolLooseTrackOnly]->initialize().isSuccess()) return EL::StatusCode::FAILURE;
  // isolation scale factor - LooseTrackOnly - NOSYST
  m_muonsfs[MuonSF_isolLooseTrackOnly_NOSYST] = new CP::MuonEfficiencyScaleFactors("MuonSF_isolLooseTrackOnly_NOSYST");
  m_muonsfs[MuonSF_isolLooseTrackOnly_NOSYST]->setProperty("WorkingPoint","LooseTrackOnlyIso").isSuccess();
  if (!m_muonsfs[MuonSF_isolLooseTrackOnly_NOSYST]->initialize().isSuccess()) return EL::StatusCode::FAILURE;
  // TTVA scale factors
  m_muonsfs[MuonSF_TTVA] = new CP::MuonEfficiencyScaleFactors("MuonSF_TTVA");
  m_muonsfs[MuonSF_TTVA]->setProperty("WorkingPoint","TTVA").isSuccess();
  if (!m_muonsfs[MuonSF_TTVA]->initialize().isSuccess()) return EL::StatusCode::FAILURE;
  // TTVA scale factors - NOSYST
  m_muonsfs[MuonSF_TTVA_NOSYST] = new CP::MuonEfficiencyScaleFactors("MuonSF_TTVA_NOSYST");
  m_muonsfs[MuonSF_TTVA_NOSYST]->setProperty("WorkingPoint","TTVA").isSuccess();
  if (!m_muonsfs[MuonSF_TTVA_NOSYST]->initialize().isSuccess()) return EL::StatusCode::FAILURE;
  */
  // Initialize trigger scale factors tools
  // Do this before setting systematics!
  m_muonTriggerSFTool = new CP::MuonTriggerScaleFactors("muTrigSF");
  m_muonTriggerSFTool->setProperty("MuonQuality","Medium").ignore();
  //m_muonTriggerSFTool->setProperty("filename","muon_trigger_eff_nov17.root");
  //m_muonTriggerSFTool->setProperty("Isolation","").ignore();
  if( m_muonTriggerSFTool->initialize().isFailure() ){
    MSG_INFO("Failed to initialize muon trigger scale factors tool!");
    return EL::StatusCode::FAILURE;
  }

  // Initialize trigger scale factors tools
  // Do this before setting systematics!
  m_muonTriggerSFToolNOSYST = new CP::MuonTriggerScaleFactors("muTrigSF_NOSYST");
  m_muonTriggerSFToolNOSYST->setProperty("MuonQuality","Medium").ignore();
  if( m_muonTriggerSFToolNOSYST->initialize().isFailure() ){
    MSG_INFO("Failed to initialize muon trigger (non-systematics) scale factors tool!");
    return EL::StatusCode::FAILURE;
  }

  m_grl = new GoodRunsListSelectionTool("GoodRunsListSelectionTool");
  std::vector<std::string> mygrls;
  MSG_INFO("$ROOTCOREBIN/../"+m_grlfile);
  MSG_INFO("$ROOTCOREDIR/../"+m_grlfile);
  if (m_grlfile.empty()) {
    MSG_INFO("Info: No GRL file specified.");
  } else if (FILE *file = fopen((std::string(getenv("ROOTCOREBIN"))+"/../"+m_grlfile).c_str(), "r")) {
    fclose(file);
    mygrls.push_back("$ROOTCOREBIN/../"+m_grlfile);
  } else if (FILE *file = fopen((std::string(getenv("ROOTCOREDIR"))+"/../"+m_grlfile).c_str(), "r")) {
    fclose(file);
    mygrls.push_back("$ROOTCOREDIR/../"+m_grlfile);
  } else {
    MSG_INFO("Error! Cannot find GRL file " << m_grlfile << ". Exiting.");
    return EL::StatusCode::FAILURE;
  }
  m_grl->setProperty( "GoodRunsListVec", mygrls).ignore();
  m_grl->setProperty("PassThrough", false).ignore();
  if (!m_grl->initialize().isSuccess()) return EL::StatusCode::FAILURE;

  // Make the tool here, but configure it once the first file has been loaded
  m_prwTool = new CP::PileupReweightingTool( "PrwTool" );

#ifdef BEFORE_SUSYTOOLS_000611
  m_isotools[PSL::AnaIso::VeryLooseTrackOnly] = new CP::IsolationSelectionTool("VeryLooseTrackOnly");
  m_isotools[PSL::AnaIso::VeryLoose         ] = new CP::IsolationSelectionTool("VeryLoose"         );
#endif
  m_isotools[PSL::AnaIso::Loose             ] = new CP::IsolationSelectionTool("Loose"             );
  m_isotools[PSL::AnaIso::Tight             ] = new CP::IsolationSelectionTool("Tight"             );
  m_isotools[PSL::AnaIso::Gradient          ] = new CP::IsolationSelectionTool("Gradient"          );
  m_isotools[PSL::AnaIso::GradientLoose     ] = new CP::IsolationSelectionTool("GradientLoose"     );
  m_isotools[PSL::AnaIso::LooseTrackOnly    ] = new CP::IsolationSelectionTool("LooseTrackOnly"    );

  for (auto it=m_isotools.begin();it != m_isotools.end();++it){
#ifdef BEFORE_SUSYTOOLS_000611
    it->second->setProperty("WorkingPoint",ConvertAnaIsoToStr(it->first)).isSuccess();
#else
    it->second->setProperty("MuonWP"    ,ConvertAnaIsoToStr(it->first)).isSuccess();
    it->second->setProperty("ElectronWP",ConvertAnaIsoToStr(it->first)).isSuccess();
#endif // BEFORE_SUSYTOOLS_000611
    if (!it->second->initialize().isSuccess()) return EL::StatusCode::FAILURE;
  }

  MSG_INFO("histInitialize - initializing internal tools done.");
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::xAODWrapper::fileExecute()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  MSG_DEBUG("fileExecute");
  m_CurrentSample = knone;
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::xAODWrapper::changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  MSG_DEBUG("changeInput");
  (void)firstFile;
  m_CurrentSample = knone;
  mc_channel_number = 0;
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::xAODWrapper::initialize()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.
  MSG_DEBUG("initialize");
  m_event = wk()->xaodEvent();

  /////////////////////////
  /// PRW tool was made new in histInitialize, but we need metadata in order to initialize it.
  /////////////////////////
  if(/*m_prwConfFiles.empty() &&*/ m_prwLcalcFiles.empty()){
    MSG_WARNING( "No configuration or iLumiCalc files provided for pileup reweighting! Will not configure PRW tool." );
  } else {
    std::vector<std::string> prwConfFiles;
    //
    // FROM NOW ON THE PRW CONF FILES WILL BE HARDCODED
    //
    // std::vector<std::string> m_prwConfFiles_vector;
    // PSL::StringTok(m_prwConfFiles_vector,m_prwConfFiles,",");
    // for(uint i = 0; i<m_prwConfFiles_vector.size();i++){
    //   prwConfFiles.push_back("$ROOTCOREBIN/../"+m_prwConfFiles_vector[i]);
    // }
    std::string pileup_profile = wk()->metaData()->castString("PileupProfile");
    if (pileup_profile == "mc15a") {
      //prwConfFiles.push_back("dev/PileupReweighting/mc15a_defaults.NotRecommended.prw.root"); //put this as back-up but it's not recommended
      prwConfFiles.push_back("$ROOTCOREBIN/../pennSoftLepton/data/merged_prw.root");
      m_prwTool->setProperty( "DefaultChannel",410000).isSuccess();//when channel info not present in config file, use this channel instead
      MSG_INFO("Setting PRW default channel to " << 410000);
    }
    else if (pileup_profile == "mc15b") {
      // load this for data just to prevent tools from possibly crashing.
      prwConfFiles.push_back("$ROOTCOREBIN/../pennSoftLepton/data/merged_prw_mc15b.root");
      prwConfFiles.push_back("$ROOTCOREBIN/../pennSoftLepton/data/my.prw_410000_mc15b.root");
      m_prwTool->setProperty( "DefaultChannel",410000).isSuccess();//when channel info not present in config file, use this channel instead      
      MSG_INFO("Setting PRW default channel to " << 410000);
    }
    else if (pileup_profile == "mc15c" || pileup_profile == "data"){
      prwConfFiles.push_back("dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root");
      m_prwTool->setProperty( "DefaultChannel",3).isSuccess();//when channel info not present in config file, use this channel instead
      MSG_INFO("Setting PRW default channel to " << 3);
    }
    else {
      MSG_INFO("Error! Something went wrong. PileupProfile was not set correctly: " << pileup_profile);
      return EL::StatusCode::FAILURE;
    }

    //int default_channel = wk()->metaData()->castInteger("DefaultChannel",m_prwDefaultChannel);
    
    std::vector<std::string> prwLcalcFiles;
    prwLcalcFiles.push_back("$ROOTCOREBIN/../"+m_prwLcalcFiles);
    m_prwTool->setProperty( "DataScaleFactor", 1./m_pu_rescaling ).isSuccess();
    m_prwTool->setProperty( "ConfigFiles", prwConfFiles ).isSuccess();
    m_prwTool->setProperty( "LumiCalcFiles", prwLcalcFiles ).isSuccess();
    m_prwTool->setProperty( "DataScaleFactorUP",   1.0/1.0 ).isSuccess();
    m_prwTool->setProperty( "DataScaleFactorDOWN",  1.0/1.23 ).isSuccess();
    if (!m_prwTool->initialize().isSuccess()) {
      MSG_INFO("Please do ln -s /home/mjoana/merged_prw.root $ROOTCOREBIN/../pennSoftLepton/data/. (mc15a)");
      MSG_INFO("Please do ln -s /home/mjoana/merged_prw_mc15b.root $ROOTCOREBIN/../pennSoftLepton/data/. (mc15b)");
      return EL::StatusCode::FAILURE;
    }
    MSG_INFO( "PileupReweightingTool initialized with rescaling of 1./" << m_pu_rescaling );
  }
  /////////////////////////
  /////////////////////////

  // Used for test of whether TriggerMenu stuff exists, just below:
  const DataVector<xAOD::TriggerMenu>* tmp_triggerMenu; 
  if ( asg::ToolStore::contains<TrigConf::xAODConfigTool>("xAODConfigTool") ) {
    m_trigConfTool = asg::ToolStore::get<TrigConf::xAODConfigTool>("xAODConfigTool");
    m_trigDecTool = asg::ToolStore::get<Trig::TrigDecisionTool>("TrigDecisionTool");
    ToolHandle< TrigConf::ITrigConfigTool > handle(m_trigConfTool);
    m_trigDecTool->setProperty("ConfigTool", handle).ignore();
    MSG_INFO("Getting xAODConfigTool and TrigDecisionTool from SUSYTools");
  }
  else if (m_event->retrieveMetaInput(tmp_triggerMenu,"TriggerMenu").isSuccess()) {
    MSG_INFO("Making xAODConfigTool");
    m_trigConfTool = new TrigConf::xAODConfigTool("xAODConfigTool");
    m_trigConfTool->initialize().ignore();
    m_trigConfTool->msg().setLevel(MSG::ERROR);
    ToolHandle< TrigConf::ITrigConfigTool > handle(m_trigConfTool);
    
    MSG_INFO("Making TrigDecisionTool");
    m_trigDecTool = new Trig::TrigDecisionTool("TrigDecisionTool");
    m_trigDecTool->setProperty("ConfigTool", handle).ignore();
    m_trigDecTool->setProperty("OutputLevel", MSG::ERROR).ignore(); // suppress MSG::WARNING
    m_trigDecTool->setProperty("TrigDecisionKey","xTrigDecision").ignore();
    m_trigDecTool->initialize().ignore();
  }
  else {
    MSG_WARNING("Warning! Trigger info does not exist. Skipping.");
  }

  // MuonSelectionTool
  m_muonSelection = new CP::MuonSelectionTool("MuonSelection");
  m_muonSelection->msg().setLevel( MSG::WARNING );
  if( !m_muonSelection->initialize().isSuccess() )
    MSG_ERROR("Failed to initialize MuonSelectionTool!");

  //ProfilerStart("unitTest");

  PrintConfiguration();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::xAODWrapper::execute()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.
  MSG_DEBUG("execute");
  m_EventVariables.clear();
  m_LepVariables.clear();
  if (jets         ){ delete jets         ; jets          = 0 ;}
  if (jets_aux     ){ delete jets_aux     ; jets_aux      = 0 ;}
  if (electrons    ){ delete electrons    ; electrons     = 0 ;}
  if (electrons_aux){ delete electrons_aux; electrons_aux = 0 ;}
  if (muons        ){ delete muons        ; muons         = 0 ;}
  if (muons_aux    ){ delete muons_aux    ; muons_aux     = 0 ;}
  if (taus         ){ delete taus         ; taus          = 0 ;}
  if (taus_aux     ){ delete taus_aux     ; taus_aux      = 0 ;}
  if (photons      ){ delete photons      ; photons       = 0 ;}
  if (photons_aux  ){ delete photons_aux  ; photons_aux   = 0 ;}
  if (met          ){ delete met          ; met           = 0 ;}
  if (met_aux      ){ delete met_aux      ; met_aux       = 0 ;}

  if (!m_event->retrieve(eventInfo,"EventInfo" ).isSuccess()) {
    MSG_INFO("Error! EventInfo not found!");
    return EL::StatusCode::FAILURE;
  } else MSG_DEBUG("Info: EventInfo found.");
  // in the event you want to make your object selection here.
  // if (!GetObjects()){
  //   MSG_INFO("Error! Objects failed to load. Exiting.");
  //   return EL::StatusCode::FAILURE;
  // }

  // if (!m_event->retrieve(electrons,"ElectronCollection" ).isSuccess()) {
  //   MSG_INFO("Error! ElectronCollection not found!");
  // } else MSG_DEBUG("Info: ElectronCollection found.");
  // if (!m_event->retrieve(electrons_aux,"ElectronCollectionAux").isSuccess())
  //   MSG_INFO("Error! ElectronCollectionAux not found!");

  if (m_CurrentSample == knone) {
    m_sumw = 1.;
    m_xsec = 1.;
    m_kfac = 1.;
    m_feff = 1.;

    m_CurrentSample = kunlabeled;
    if(!eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)){ // not simulation == data
      MSG_INFO("Setting sample to data.");
      m_CurrentSample = kdata;
    }
    else {
      mc_channel_number = eventInfo->mcChannelNumber();
      if (!m_run_sample_map.count(mc_channel_number)) {
        MSG_INFO("Warning! Run " << mc_channel_number << " cannot be converted to sample!");
        if (!m_allowUnlabeledSamples) return EL::StatusCode::FAILURE;
      }
      else {
        m_CurrentSample = static_cast<PSL::Sample>(m_run_sample_map[mc_channel_number]);
        m_sumw = m_run_sumw_map[mc_channel_number];
        m_xsec = m_run_xsec_map[mc_channel_number];
      }
      MSG_INFO("Setting sample (Run " << mc_channel_number 
               << ") to " << ConvertSampleToStr(m_CurrentSample));
      
      MSG_INFO("--- mc_channel_number: " << mc_channel_number);
      MSG_INFO("--- Samples.txt xsec*kfactor*feff: " << m_xsec);
      MSG_INFO("--- kfactor = 1      : " << m_kfac);
      MSG_INFO("--- filt eff = 1     : " << m_feff);        
      MSG_INFO("--- Map_SRM sumw     : " << m_sumw);
      MSG_INFO("--- Local lumi       : " << m_lumi_pb);
      MSG_INFO("--- Metadata Simul.  : " << wk()->metaData()->castString("Simulation"));
      MSG_INFO("--- Metadata pileup profile  : " << wk()->metaData()->castString("PileupProfile"));
      // MSG_INFO("--- Metadata Default_Channel : " << wk()->metaData()->castInteger("DefaultChannel",m_prwDefaultChannel)
      //          << (wk()->metaData()->castInteger("DefaultChannel",m_prwDefaultChannel) != m_prwDefaultChannel ? " (changed to reflect sample metadata)" : ""));
      MSG_INFO("--- eventInfo.runNumber      : " << eventInfo->runNumber());
      MSG_INFO("--- eventInfo.mcChannelNumber: " << eventInfo->mcChannelNumber());
      MSG_INFO("--- Metadata isData  : " << (wk()->metaData()->castString("IsData").empty() ? "False" : "True"));
    }
  }

  if (m_trigDecTool && n_processed == 1) {
    const Trig::ChainGroup* cg_dbg = m_trigDecTool->getChainGroup("HLT_.*");
    //MSG_INFO("HLT Triggers available:");
    MSG_INFO("#####################################");
    // for(auto trig : cg_dbg->getListOfTriggers()){
    //   MSG_INFO(" " << trig);
    // }
    for (unsigned int i=0;i<m_trigger_bitmask.size();++i) {
      bool exists = false;
      std::string mytrig = ConvertTrigToStr(m_trigger_bitmask[i]);
      for(auto trig : cg_dbg->getListOfTriggers()){
        if (trig == mytrig) exists = true;
      }
      if (!exists) {
        MSG_INFO("Warning! " << mytrig << " does not exist!");
      } else {
        MSG_INFO("Info: " << mytrig << " exists.");
      }
    }
    MSG_INFO("#####################################");
  }

  if (IsMC()) {
    if (!m_event->retrieve(truthjets,"AntiKt4TruthJets").isSuccess()) {
      MSG_INFO("Error! AntiKt4TruthJets not found!");
      return EL::StatusCode::FAILURE;
    }
    else {
      truth_jets.clear();
      for(unsigned int i=0;i<truthjets->size();++i){
        const xAOD::Jet* truthjet = truthjets->at(i);
        truth_jets.push_back(Particle(truthjet->p4(),ObjType::Jet,i,0));
        MSG_DEBUG("Truth jet pt: " << truthjet->p4().Pt());
      }
      std::sort(truth_jets.begin(), truth_jets.end(), DescendingPt);
    }

  }

  if (!m_sample_stats.count(mc_channel_number)) {
    TString mcchan;
    mcchan.Form("%d",mc_channel_number);
    m_sample_stats[mc_channel_number] = new TH1F(mcchan,mcchan,6,0,6);
    TString bin_label;
    bin_label.Form("sumw: %5.3f",m_sumw);
    m_sample_stats[mc_channel_number]->GetXaxis()->SetBinLabel(1,bin_label);
    bin_label.Form("nevents");
    m_sample_stats[mc_channel_number]->GetXaxis()->SetBinLabel(2,bin_label); 
    bin_label.Form("TotalWeightNoPupNoSF");
    m_sample_stats[mc_channel_number]->GetXaxis()->SetBinLabel(3,bin_label); 
    bin_label.Form("TotalWeightNoSF");
    m_sample_stats[mc_channel_number]->GetXaxis()->SetBinLabel(4,bin_label); 
    bin_label.Form("Xsec*Kfac*Feff: %5.3f",m_xsec*m_kfac*m_feff);
    m_sample_stats[mc_channel_number]->GetXaxis()->SetBinLabel(5,bin_label);
    bin_label.Form("lumi (pb): %5.3f",m_lumi_pb);
    m_sample_stats[mc_channel_number]->GetXaxis()->SetBinLabel(6,bin_label);
    wk()->addOutput(m_sample_stats[mc_channel_number]);
  }

#ifdef BEFORE_SUSYTOOLS_000611
  // getLumiBlockMu not available before SUSYTools 06-11
#elif defined(BEFORE_SUSYTOOLS_000709)
  m_LumiBlockMu = m_prwTool->getLumiBlockMu( *eventInfo );
#elif defined(BEFORE_ANALYSISBASE_2p3p45)
  m_LumiBlockMu = m_prwTool->getCorrectedMu( *eventInfo );
#else
  m_LumiBlockMu = m_prwTool->getCorrectedMu( *eventInfo , false /* includeDataScaleFactor */);
#endif // BEFORE_SUSYTOOLS_000611

  m_weight = 1;
  m_weight_pu = 1;
  m_purw = 1.;
  float mc_event_weight = 1;
  if (IsMC()){
#ifndef BEFORE_SUSYTOOLS_000611
    m_purw = m_prwTool->getCombinedWeight( *eventInfo );
#endif // BEFORE_SUSYTOOLS_000611
    const std::vector< float > weights = eventInfo->mcEventWeights();
    if( weights.size() > 0 ) mc_event_weight = weights[0];
    m_weight = mc_event_weight*m_xsec*m_kfac*m_feff*m_lumi_pb/m_sumw;
    m_weight_pu = m_weight*m_purw;
  }
  n_processed++;
  m_sumw_thisjob += mc_event_weight;
  m_sample_stats[mc_channel_number]->Fill(0.5,mc_event_weight); // 1: sumw
  m_sample_stats[mc_channel_number]->Fill(1.5,1); // 2: nevents
  m_sample_stats[mc_channel_number]->Fill(2.5,m_weight); // 3: total weight (no pileup, no sf)
  m_sample_stats[mc_channel_number]->Fill(3.5,m_weight_pu); // 4: total weight with pileup (no sf)
  MSG_DEBUG("Events processed: " << n_processed);
  if (!(n_processed%1000)) MSG_INFO("Events processed: " << n_processed);

  // AlgBase will check if the event was killed at this stage. If it is killed here,
  // all other algorithms will be skipped.
  m_event_killed = false;
  if (m_CurrentSample == kzjee || m_CurrentSample == kzjmm || m_CurrentSample == kzjtt){
    if (isZgammaEvent() && m_RemoveZgammaFromZjet) {
      m_event_killed = true;
    }
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::xAODWrapper::postExecute()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  MSG_DEBUG("postExecute");
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::xAODWrapper::finalize()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.
  MSG_INFO("finalize. Nevents processed: " << n_processed);
  //ProfilerStop();
  // if (eventInfo     ){delete eventInfo     ; eventInfo      = 0;}
  // if (vertices      ){delete vertices      ; vertices       = 0;}
  if (m_trigDecTool ){delete m_trigDecTool ; m_trigDecTool  = 0;}
  if (m_trigConfTool){delete m_trigConfTool; m_trigConfTool = 0;}
  if (m_grl         ){delete m_grl         ; m_grl          = 0;}
  if (m_prwTool     ){delete m_prwTool     ; m_prwTool      = 0;}
  if (m_muonTriggerSFTool){delete m_muonTriggerSFTool; m_muonTriggerSFTool = 0;}
  if (m_muonTriggerSFToolNOSYST){delete m_muonTriggerSFToolNOSYST; m_muonTriggerSFToolNOSYST = 0;}
  for (auto it=m_lhtools.begin();it != m_lhtools.end();++it){
    if (it->second){delete it->second;}
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::xAODWrapper::histFinalize()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.
  MSG_DEBUG("histFinalize");
 return EL::StatusCode::SUCCESS;
}

double PSL::xAODWrapper::EleIsolation(int icontainer,AnaIso::AnaIso isoenum){
  MSG_DEBUG("Ele isolation: " << ConvertAnaIsoToStr(isoenum));
  if (isoenum == AnaIso::None) return -999.;
  if (!electrons) return -999.;
  const xAOD::Electron* ele = getElectron(icontainer);
  if (isoenum > AnaIso::N_NONWP) {
    return (double)m_isotools[isoenum]->accept(*ele);
  }

#ifdef BEFORE_SUSYTOOLS_050030
  // most of this is a hack until Rel 20
  std::string acc_name = "";
  if (isoenum == AnaIso::AnaIso::ptvarcone20) acc_name = "ptvarcone20";
  if (isoenum == AnaIso::AnaIso::ptvarcone30) acc_name = "ptvarcone30";
  if (isoenum == AnaIso::AnaIso::ptvarcone40) acc_name = "ptvarcone40";
  if (!acc_name.empty())
  {
    SG::AuxElement::Accessor<float> acc(acc_name);
    if (!acc.isAvailable(*ele)) {
      xaodWarnOnce("Ele"+acc_name,"Warning! electron "+acc_name+" is missing!");
      return 0;
    }
    double iso = acc(*ele);
    MSG_DEBUG("Returning ele isolation.");
    if (isoenum > AnaIso::N_NONRELATIVE) return iso/ele->pt();
    return iso;
  }
#endif //BEFORE_SUSYTOOLS_050030
  SG::AuxElement::Accessor<float>* acc = xAOD::getIsolationAccessor(IsoType(isoenum));
  if (!acc) return 0;
  if (!acc->isAvailable(*ele)) {
    xaodWarnOnce("Ele"+ConvertAnaIsoToStr(isoenum)
                 ,"Warning! electron "+ConvertAnaIsoToStr(isoenum)+" is missing!");
    return 0;
  }
  double iso = (*acc)(*ele);
  MSG_DEBUG("Returning ele isolation.");
  if (isoenum > AnaIso::N_NONRELATIVE) return iso/ele->pt();
  return iso;
}
double PSL::xAODWrapper::EleD0Significance(int icontainer){
  if (!electrons) return -999.;
  const xAOD::TrackParticle* track = getElectron(icontainer)->trackParticle();
  //
  // This was the old way
  //
  //double d0sigma = Amg::error(track->definingParametersCovMatrix(),0);
  //if (d0sigma != 0) return fabs(track->d0()/d0sigma);
  //return 0;
  //
  // New way
  //
  return xAOD::TrackingHelpers::d0significance(track,eventInfo->beamPosSigmaX()
                                               ,eventInfo->beamPosSigmaY()
                                               ,eventInfo->beamPosSigmaXY());
}

double PSL::xAODWrapper::EleEtaBE(int icontainer){
  if (!electrons) return -999.;
  return getElectron(icontainer)->caloCluster()->etaBE(2);
}

double PSL::xAODWrapper::ElePt(int icontainer){
  if (!electrons) return -999.;
  return getElectron(icontainer)->pt();
}
double PSL::xAODWrapper::EleD0(int icontainer){
  if (!electrons) return -999.;
  const xAOD::TrackParticle* track = getElectron(icontainer)->trackParticle();
  return track->d0();
}
double PSL::xAODWrapper::EleZ0(int icontainer){
  if (!electrons) return -999.;
  const xAOD::Vertex* pv = GetPrimaryVertex();
  const xAOD::TrackParticle* track = getElectron(icontainer)->trackParticle();
  double primvertex_z = pv ? pv->z() : 0;
  return track->z0() + track->vz() - primvertex_z;
}

int PSL::xAODWrapper::EleBlayer(int icontainer){
  if (!electrons) return -999.;
  const xAOD::TrackParticle* track  = getElectron(icontainer)->trackParticle();

  uint8_t expectBlayer(true);
  uint8_t nBlayerHits(0);
  uint8_t nBlayerOutliers(0);
  track->summaryValue(expectBlayer, xAOD::expectBLayerHit);
  track->summaryValue(nBlayerHits, xAOD::numberOfBLayerHits);
  track->summaryValue(nBlayerOutliers, xAOD::numberOfBLayerOutliers);

  if (expectBlayer) {
    if (nBlayerHits + nBlayerOutliers < 1) return false;
  }
  return true;
}

double PSL::xAODWrapper::MuonIsolation(int icontainer,AnaIso::AnaIso isoenum){
  MSG_DEBUG("Muon isolation: " << ConvertAnaIsoToStr(isoenum));
  if (isoenum == AnaIso::None) return -999.;
  if (!muons) return -999.;
  const xAOD::Muon* mu = getMuon(icontainer);
  if (isoenum > AnaIso::N_NONWP) {
    return (double)m_isotools[isoenum]->accept(*mu);
  }
#ifdef BEFORE_SUSYTOOLS_050030
  // most of this is a hack until Rel 20
  std::string acc_name = "";
  if (isoenum == AnaIso::AnaIso::ptvarcone20) acc_name = "ptvarcone20";
  if (isoenum == AnaIso::AnaIso::ptvarcone30) acc_name = "ptvarcone30";
  if (isoenum == AnaIso::AnaIso::ptvarcone40) acc_name = "ptvarcone40";
  if (!acc_name.empty())
  {
    SG::AuxElement::Accessor<float> acc(acc_name);
    if (!acc.isAvailable(*mu)) {
      xaodWarnOnce("Muon"+acc_name,"Warning! muon "+acc_name+" is missing!");
      return 0;
    }
    double iso = acc(*mu);
    MSG_DEBUG("Returning muon isolation.");
    if (isoenum > AnaIso::N_NONRELATIVE) return iso/mu->pt();
    return iso;
  }
#endif //BEFORE_SUSYTOOLS_050030
  SG::AuxElement::Accessor<float>* acc = xAOD::getIsolationAccessor(IsoType(isoenum));
  if (!acc) {
    xaodWarnOnce("Muon"+ConvertAnaIsoToStr(isoenum)
                 ,"Warning! muon "+ConvertAnaIsoToStr(isoenum)+" is missing!");
    return 0;
  }
  if (!acc->isAvailable(*mu)) {
    xaodWarnOnce("Muon"+ConvertAnaIsoToStr(isoenum)
                 ,"Warning! muon "+ConvertAnaIsoToStr(isoenum)+" is missing!");
    return 0;
  }
  double iso = (*acc)(*mu);
  MSG_DEBUG("Returning muon isolation.");
  if (isoenum > AnaIso::N_NONRELATIVE) return iso/mu->pt();
  return iso;
}
double PSL::xAODWrapper::MuonD0Significance(int icontainer){
  if (!muons) return -999.;
  const xAOD::TrackParticle* track = getMuon(icontainer)->primaryTrackParticle();
  //
  // Old way
  //
  // double d0sigma = Amg::error(track->definingParametersCovMatrix(),0);
  // if (d0sigma != 0) return fabs(track->d0()/d0sigma);
  // return 0;
  //
  // New way
  //
  return xAOD::TrackingHelpers::d0significance(track,eventInfo->beamPosSigmaX()
                                               ,eventInfo->beamPosSigmaY()
                                               ,eventInfo->beamPosSigmaXY());
}
double PSL::xAODWrapper::MuonPt(int icontainer){
  if (!muons) return -999.;
  return getMuon(icontainer)->pt();
}
double PSL::xAODWrapper::MuonD0(int icontainer){
  if (!muons) return -999.;
  const xAOD::TrackParticle* track = getMuon(icontainer)->primaryTrackParticle();
  return track->d0();
}
double PSL::xAODWrapper::MuonZ0(int icontainer){
  if (!muons) return -999.;
  const xAOD::Vertex* pv = GetPrimaryVertex();
  const xAOD::TrackParticle* track = getMuon(icontainer)->primaryTrackParticle();
  double primvertex_z = pv ? pv->z() : 0;
  return track->z0() + track->vz() - primvertex_z;
}

const xAOD::Vertex* PSL::xAODWrapper::GetPrimaryVertex(void){
  if (!m_event->contains<xAOD::VertexContainer>("PrimaryVertices")) return NULL; 
  const xAOD::VertexContainer* vertices(0);
  if( m_event->retrieve( vertices, "PrimaryVertices" ).isSuccess() ) {
    for (unsigned int i=0;i<vertices->size();++i){
      const xAOD::Vertex* vx = vertices->at(i);
      if (vx->vertexType() == xAOD::VxType::PriVtx) return vx;
    }
  }
  return NULL;
}

int PSL::xAODWrapper::GetNPV(void){
  unsigned int numReqTracks = 2;
  int nvtx = 0;
  const xAOD::VertexContainer* vxContainer = 0;
  if( m_event->retrieve(vxContainer, "PrimaryVertices").isSuccess() ){
    for(unsigned int vx = 0; vx < vxContainer->size(); ++vx){
      const xAOD::Vertex* vxcand = vxContainer->at(vx);
      if(vxcand->nTrackParticles() >= numReqTracks)
        ++nvtx;
    }
  }
  return nvtx;
}

// bool PSL::xAODWrapper::GetObjects(void){
  
//   return true;
// }

void PSL::xAODWrapper::set_trigger_bitmask(std::string bits){
  MSG_DEBUG("setting bitmask to " << bits);
  m_trigger_bitmask.clear();
  if (bits.find("all") != std::string::npos ||
      bits.find("All") != std::string::npos) {
    std::vector<PSL::Trigger2015> all_triggers = GetAllTriggers();
    for (unsigned int i=0;i<all_triggers.size();++i){
      m_trigger_bitmask.push_back(all_triggers[i]);
    }
    return;
  }
  std::vector<std::string> tmp;
  PSL::StringTok(tmp,bits," ");
  for(unsigned int i=0;i<tmp.size();i++){
    m_trigger_bitmask.push_back(ConvertToTrigger(tmp[i]));
  }
}

bool PSL::xAODWrapper::PassTrigger(void){
  if (!m_evtdef->trigger_bits_set) FillTriggerBits();
  return bool(m_evtdef->trigger_bits.count());
}

bool PSL::xAODWrapper::PassTrigger(Trigger2015 t){ // trigger enum
  std::vector<PSL::Trigger2015>::iterator it;
  it = std::find(m_trigger_bitmask.begin(),m_trigger_bitmask.end(),t);
  if (it == m_trigger_bitmask.end()){
    return 0;
  }
  if (!m_evtdef->trigger_bits_set) FillTriggerBits();
  MSG_DEBUG("Trigger " << ConvertTrigToStr(t)
            << " was set to " << m_evtdef->trigger_bits[t] );
  return m_evtdef->trigger_bits[t];
}

void PSL::xAODWrapper::FillTriggerBits(void){
  if (m_evtdef->trigger_bits_set) return;
  m_evtdef->trigger_bits.reset();
  if (!m_trigDecTool) {
    m_evtdef->trigger_bits_set = true;
    return;
  }
  std::string bit_result;
  // We are testing only the triggers in the bitmask
  for (unsigned int i=0;i<m_trigger_bitmask.size();++i){
    MSG_DEBUG("Testing trigger " << ConvertTrigToStr(m_trigger_bitmask[i]));
    if (!TriggerMatch(ConvertTrigToStr(m_trigger_bitmask[i]))) {
      bit_result += "  ";
      continue;
    }
    m_evtdef->trigger_bits.set(m_trigger_bitmask[i]);
    bit_result +=  "1 ";
  }
  bit_result += "~";
  MSG_DEBUG("Trigger bits: " << bit_result << " lbn: " << eventInfo->lumiBlock());
  m_evtdef->trigger_bits_set = true;
  return;
}

bool PSL::xAODWrapper::TriggerMatch(const std::string& trigname){
  if (trigname == "None") return false;
  if (!m_trigDecTool) return false;
  // see if the trigger passed in the first place
  if(!m_trigDecTool->isPassed(trigname) ) {
    MSG_DEBUG("Failed trigger " << trigname);
    return false;
  }
  MSG_DEBUG("Passed trigger " << trigname);
  return true;
}

TLorentzVector PSL::xAODWrapper::GetContainerEleTLV(int icontainer){
  return getElectron(icontainer)->p4();
}

TLorentzVector PSL::xAODWrapper::GetContainerMuonTLV(int icontainer){
  return getMuon(icontainer)->p4();
}

TLorentzVector PSL::xAODWrapper::CorrectedElectronTLV(int icontainer){
  //
  // copied from DerivationFramework/DerivationFrameworkEGamma/trunk/src/ElectronsDirectionTool.cxx
  // This does the track-cluster combination for xAOD. tlv.Pt() will give you the result.
  // Otherwise you should use eCluster->e() to get the cluster energy.
  //
  TLorentzVector result;
  const xAOD::Electron* ele = getElectron(icontainer);
  // Prepare variables
  float mElectron(0.511);
  //float eta(0.0), phi(0.0), et(0.0),e(0.0),pt(0.0);
  float e(0.0),eta(0.0);
  const xAOD::TrackParticle* eTrack(0);
  const xAOD::CaloCluster* eCluster(0);
  if ( ele->nTrackParticles() > 0) {eTrack = ele->trackParticle(0);}
  else {return TLorentzVector();}
  if ( ele->nCaloClusters() > 0) {eCluster = ele->caloCluster(0);}             
  else {return TLorentzVector();}
  uint8_t numberOfPixelHits(0);
  if( !eTrack->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits) ){
    MSG_WARNING("Unable to retrieve numberOfPixelHits");
  }
  uint8_t numberOfSCTHits(0);
  if( !eTrack->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits) ){
    MSG_WARNING("Unable to retrieve numberOfSCTHits");
  }   
  if ( numberOfPixelHits+numberOfSCTHits >= 4.0) {
    e = eCluster->e() ;
    eta = eTrack->eta();
    result.SetPtEtaPhiM(sqrt( e*e - mElectron*mElectron )/cosh(eta)
                        ,eta // track
                        ,eTrack->phi()
                        ,0.511);
    // et = eCluster->e()/cosh(eta) // where eta is from track!
    return result;
  } 
  e = eCluster->e() ;
  eta = eCluster->eta();
  result.SetPtEtaPhiM(sqrt( e*e - mElectron*mElectron )/cosh(eta)
                      ,eta // cluster
                      ,eCluster->phi()
                      ,0.511);
  return result;
  //et = eCluster->e()/cosh(eta); // where eta is from cluster!
}

void PSL::xAODWrapper::xaodWarnOnce(std::string wname,std::string message){
  if (!std::count(m_xaod_warnings.begin(),m_xaod_warnings.end(),wname)) {
    m_xaod_warnings.push_back(wname);
    MSG_INFO("#####################################");
    MSG_INFO(message);
    MSG_INFO("#####################################");
  }
  return;
}

PSL::LeptonTruthLabel PSL::xAODWrapper::getElectronTruthLabel(int i){
  MSG_DEBUG("getElectronTruthLabel");
  if (!electrons) return LeptonTruthLabel::NoTruthLabel;
  if (IsData()) return LeptonTruthLabel::NoTruthLabel;
  const xAOD::Electron* ele = getElectron(i);

  int type = TruthType(*ele);

  int origin = TruthOrigin(*ele);
  // real
  if (type == 1 && origin == 0) return LeptonTruthLabel::LeptonUnknown; //
  if (type == 2 && origin == 10) return LeptonTruthLabel::LeptonTtbar;
  if (type == 2 && origin == 12) return LeptonTruthLabel::LeptonW;
  //egamma recommendations for Z lepton
  if (type == 2 && origin == 13) return LeptonTruthLabel::LeptonZ;
  if (type == 4 && origin == 13) return LeptonTruthLabel::LeptonZ;
  if (type == 4 && origin == 40) return LeptonTruthLabel::LeptonZ;

  if (type == 2 && origin == 22) return LeptonTruthLabel::LeptonSUSY;
  if (type == 2 && origin == 43) return LeptonTruthLabel::LeptonZ; // Diboson...???
  // count as fake...?
  if (type == 2 && origin == 27) return LeptonTruthLabel::IsoElectronFromCCbar;
  if (type == 2 && origin == 29) return LeptonTruthLabel::IsoElectronFromBBbar;
  // bkg electron
  if (type == 3 && origin == 25) return LeptonTruthLabel::CjetLeptonic; // CharmedMeson
  if (type == 3 && origin == 26) return LeptonTruthLabel::BjetLeptonic; // BottomMeson
  if (type == 3 && origin == 32) return LeptonTruthLabel::CjetLeptonic; // CharmedBaryon
  if (type == 3 && origin == 33) return LeptonTruthLabel::BjetLeptonic; // BottomBaryon
  // conversion
  if (type == 4 && origin ==  5) return LeptonTruthLabel::PhotonOrPi0; // PhotonConv
  // hadronic
  if (type == 4 && origin == 23) return LeptonTruthLabel::LightjetLeptonic; // Light jet Leptonic!
  if (type == 4 && origin == 24) return LeptonTruthLabel::LightjetLeptonic; // StrangeMeson
  if (type == 4 && origin == 27) return LeptonTruthLabel::CjetLeptonic; // CCbarMeson
  if (type == 4 && origin == 29) return LeptonTruthLabel::LightjetLeptonic; // StrangeBaryon
  if (type == 4 && origin == 31) return LeptonTruthLabel::LightjetLeptonic; // StrangeMeson
  // hadron
  if (type == 17) return LeptonTruthLabel::LightjetHadronic; // Hadron
  if (type == 20 && origin == 0) return LeptonTruthLabel::NonPrimary;
  
  if (type == 6               ) return LeptonTruthLabel::ElectronAsMuon;
  if (type == 7               ) return LeptonTruthLabel::ElectronAsMuon;
  if (type == 3 && origin == 8) return LeptonTruthLabel::ElectronAsMuon;

  const xAOD::TruthParticle *tp = xAOD::TruthHelpers::getTruthParticle(*ele);

  //
  // photon treatment is difficult.
  //
  if (type == 14 && origin == 3) return LeptonTruthLabel::PhotonOrPi0; // PhotonConv
  if (type == 16 && origin == 42) return LeptonTruthLabel::PhotonOrPi0; // Bkg photon from pi0
  if (type == 16 && origin == 23) return LeptonTruthLabel::PhotonInMeson;
  // if (type == 15 && origin == 40) return LeptonTruthLabel::PhotonOrPi0; // noniso FSR photon...?
  if (((type == 15 && origin == 40) ||
       (type == 16 && origin == 38) ||
       (type == 15 && origin == 39) ||
       (type == 13) ||
       (type == 14) ||
       (type == 15) ||
       (type == 16)
       ) && tp) { // going to label all of these as photons, unless they are collinear with dr<0.1
    if (!m_event->retrieve(truthelectrons,"TruthElectrons").isSuccess()) {
      xaodWarnOnce("tp_ele_retrieve","Warning! TruthElectrons not found!");
    }
    else {
      for(unsigned int j=0;j<truthelectrons->size();++j){
        const xAOD::TruthParticle* tele = getTruthElectron(j);
        if (tele->p4().DeltaR(tp->p4()) < 0.1 && tele->p4().Pt() > 7000.) {
          MSG_INFO("Found electron near an apparent FSR non-iso photon.");
          MSG_INFO("DeltaR: " << tele->p4().DeltaR(tp->p4()));
          MSG_INFO("Origin \t " << ParticleOrigin(*tele));
          MSG_INFO("Type \t " << ParticleType(*tele));
          MSG_INFO("Status \t " << tele->status());
          MSG_INFO("Barcode \t " << tele->barcode());
          MSG_INFO("Outcome \t " << ParticleOutCome(*tele));
          MSG_INFO("Mother \t " << ParticleMother(*tele));
          MSG_INFO("Pt \t " << tele->p4().Pt());
          MSG_INFO("Eta \t " << tele->p4().Eta());
          MSG_INFO("Phi \t " << tele->p4().Phi());
          MSG_INFO("Mass \t " << tele->p4().M());
          MSG_INFO("PDG ID \t " << tele->absPdgId());
          if (ParticleOrigin(*tele) == 10) return LeptonTruthLabel::LeptonTtbar;
          if (ParticleOrigin(*tele) == 12) return LeptonTruthLabel::LeptonW;
          if (ParticleOrigin(*tele) == 13) return LeptonTruthLabel::LeptonZ;
        }
      }
    }
    return LeptonTruthLabel::PhotonOrPi0;
  }
  if ((type == 13) || (type == 14) || (type == 15) || (type == 16)) {
    return LeptonTruthLabel::PhotonOrElectronAmbiguous; // we tried our best.
  }
  
  std::stringstream chain;
  chain << "--";
  if (!tp) chain << "no tp";
  else chain << "nparents: " << tp->nParents() << "\n";
  if (tp) {
    std::string indent = "";
    while (tp->nParents()){
      chain << "--" << indent << tp->absPdgId() << "\n";
      indent += " ";
      tp = tp->parent();
      if (!tp) break;
    }
  }

  if (type == 4) {
    const xAOD::TruthParticle* tp_eg = xAOD::EgammaHelpers::getBkgElectronMother(ele);
    if (tp_eg) {
      if (tp_eg && MC::PID::isTop(tp_eg->pdgId())) return LeptonTruthLabel::LeptonTtbar;
      if (tp_eg && MC::PID::isW(tp_eg->pdgId())) return LeptonTruthLabel::LeptonW;
      if (tp_eg && MC::PID::isZ(tp_eg->pdgId())) return LeptonTruthLabel::LeptonZ;
    }
  }


  if (type == 3 && origin == 9) return LeptonTruthLabel::LeptonTau;
  //if (type == 0 && origin == 0) return LeptonTruthLabel::NoTruthLabel;
  if (type == 4 && origin == 6) return LeptonTruthLabel::ElectronAsMuon;
  if (type == 4 && origin == 7) return LeptonTruthLabel::ElMagProc;

  if (!m_event->retrieve(truthparticles,"TruthParticles").isSuccess()) {
    xaodWarnOnce("tp_cont_retrieve","Warning! TruthParticles not found!");
  } 
  else {
    for(unsigned int j=0;j<truthparticles->size();++j){
      const xAOD::TruthParticle* part = getTruthParticle(j);
      //MSG_INFO("Pt: " << part->pt() << " DeltaR: " << part->p4().DeltaR(ele->p4()));
      //if (part->p4().DeltaR(ele->p4()) < 0.1) {
      if (false){
        MSG_INFO("Found truth particle near a wayward electron.");
        MSG_INFO("DeltaR: " << part->p4().DeltaR(ele->p4()));
        MSG_INFO("Origin \t " << ParticleOrigin(*part));
        MSG_INFO("Type \t " << ParticleType(*part));
        MSG_INFO("Status \t " << part->status());
        MSG_INFO("Barcode \t " << part->barcode());
        MSG_INFO("Outcome \t " << ParticleOutCome(*part));
        //MSG_INFO("Mother \t " << ParticleMother(*part));
        MSG_INFO("Pt \t " << part->p4().Pt());
        MSG_INFO("Eta \t " << part->p4().Eta());
        MSG_INFO("Phi \t " << part->p4().Phi());
        MSG_INFO("Mass \t " << part->p4().M());
        MSG_INFO("PDG ID \t " << part->absPdgId());        
      }
    }
  }

  MSG_INFO("unknown truth electron: type " << type << " and origin " << origin);
  MSG_INFO("chain: ");
  std::cout << chain.str() << std::endl;
  return LeptonTruthLabel::NoTruthLabel;
}

PSL::LeptonTruthLabel PSL::xAODWrapper::getMuonTruthLabel(int i){
  MSG_DEBUG("getMuonTruthLabel");
  if (!muons) return LeptonTruthLabel::NoTruthLabel;
  if (IsData()) return LeptonTruthLabel::NoTruthLabel;
  int type = 0;
  int origin = 0;
  
  std::stringstream chain;
  chain << "--";

  const xAOD::Muon* muon = getMuon(i);
  const xAOD::TrackParticle* track = getMuon(i)->primaryTrackParticle();

  if (!track) {
    MSG_INFO("Error! muon has no track. Returning no truth label.");
    return LeptonTruthLabel::NoTruthLabel;
  }

  const xAOD::TruthParticle *tp = xAOD::TruthHelpers::getTruthParticle(*track);
  if (!tp) chain << "no tp";
  else chain << "nparents: " << tp->nParents() << "\n";

  // If the truth info exists, try it first:
  if (m_acc_truthType.isAvailable(*track)) {
    type   = TruthType(*track);
    origin = TruthOrigin(*track);
  }
  else if (tp && m_acc_particleType.isAvailable(*tp)) { // "classifierParticleType"
    type   = m_acc_particleType(*tp);
    origin = m_acc_particleOrigin(*tp);    
    //MSG_INFO("Truth type and orgin: " << type << " " << origin);
  }

  if (type == 6 && origin == 10) return LeptonTruthLabel::LeptonTtbar;
  if (type == 6 && origin == 12) return LeptonTruthLabel::LeptonW;
  if (type == 6 && origin == 13) return LeptonTruthLabel::LeptonZ;
  if (type == 6 && origin == 43) return LeptonTruthLabel::LeptonZ; // listed as "DiBoson" ...?
  if (type == 6 && origin == 22) return LeptonTruthLabel::LeptonSUSY; // SUSY!
  // 7 == NonIsoMuon
  if (type == 7 && origin == 25) return LeptonTruthLabel::CjetLeptonic; // CharmedMeson
  if (type == 7 && origin == 26) return LeptonTruthLabel::BjetLeptonic; // BottomMeson
  if (type == 7 && origin == 27) return LeptonTruthLabel::CjetLeptonic; // CCbarMeson
  if (type == 7 && origin == 32) return LeptonTruthLabel::CjetLeptonic; // CharmedBaryon
  if (type == 7 && origin == 33) return LeptonTruthLabel::BjetLeptonic;
  // 8 == BkgMuon
  if (type == 8 && origin == 23) return LeptonTruthLabel::LightjetLeptonic; // LightMeson
  if (type == 8 && origin == 24) return LeptonTruthLabel::LightjetLeptonic; // StrangeMeson
  if (type == 8 && origin == 27) return LeptonTruthLabel::CjetLeptonic; // CCbarMeson
  if (type == 6 && origin == 27) return LeptonTruthLabel::CjetLeptonic; // CCbarMeson
  if (type == 8 && origin == 29) return LeptonTruthLabel::BjetLeptonic; // BBbarMeson
  if (type == 6 && origin == 29) return LeptonTruthLabel::BjetLeptonic; // BBbarMeson
  if (type == 8 && origin == 34) return LeptonTruthLabel::PiPlusMinusLeptonic; // muon from pion
  if (type == 8 && origin == 35) return LeptonTruthLabel::KaonLeptonic; // KaonDecay
  if (type == 17) return LeptonTruthLabel::LightjetHadronic; // Hadron

  if (type == 2) {
    int pdgid = -999;
    if (tp) pdgid = tp->absPdgId();
    double deltar = -999;
    if (tp) deltar = muon->p4().DeltaR(tp->p4());
    MSG_INFO("WARNING: Reco object is a muon but the associated truth particle is an iso ele. DeltaR with reco muon: " << deltar
             << ". Origin is " << origin << ". PdgId is " << pdgid << ". Returning anyway.");
    if (origin == 10) return LeptonTruthLabel::LeptonTtbar;
    if (origin == 12) return LeptonTruthLabel::LeptonW;
    // if (origin == 13) return LeptonTruthLabel::LeptonZ;
    // if (origin == 43) return LeptonTruthLabel::LeptonZ; // "DiBoson"
  }

  if (type == 4 || type == 3) {
    int pdgid = -999;
    if (tp) pdgid = tp->absPdgId();
    double deltar = -999;
    if (tp) deltar = muon->p4().DeltaR(tp->p4());
    MSG_INFO("WARNING: Reco object is a muon but the associated truth particle is a bkg ele. DeltaR with reco muon: " << deltar
             << ". Origin is " << origin << ". PdgId is " << pdgid << ". Returning NoTruthLabel.");
    return LeptonTruthLabel::NoTruthLabel;
  }
  
  if (!tp) {
    if (type ==  7 && origin == 9) return LeptonTruthLabel::LeptonTau;
    //if (type ==  0 && origin == 0) return LeptonTruthLabel::NoTruthLabel;
    if (type == 20 && origin == 0) return LeptonTruthLabel::NonPrimary;
    if (type ==  5 && origin == 0) return LeptonTruthLabel::LeptonUnknown; //
  }

  if (tp) {
    std::string indent = "";
    bool has_tau = false;
    while (tp->nParents()){
      chain << "--" << indent << tp->absPdgId() << "\n";
      if (tp->absPdgId() == 15) has_tau = true;
      if (tp->absPdgId() ==  431 && has_tau) return LeptonTruthLabel::CjetLeptonic; // D+s -> tau -> mu
      if (tp->absPdgId() ==  411 && has_tau) return LeptonTruthLabel::CjetLeptonic; // D+ -> tau -> mu

      if (tp->absPdgId() ==  511 && has_tau) return LeptonTruthLabel::BjetLeptonic; // B0 -> tau -> mu
      if (tp->absPdgId() ==  521 && has_tau) return LeptonTruthLabel::BjetLeptonic; // B+ -> tau -> mu
      if (tp->absPdgId() ==  531 && has_tau) return LeptonTruthLabel::BjetLeptonic; // B0s-> tau -> mu
      if (tp->absPdgId() == 5122 && has_tau) return LeptonTruthLabel::BjetLeptonic; // LambdaB -> tau -> mu
      if (tp->absPdgId() == 5132 && has_tau) return LeptonTruthLabel::BjetLeptonic; // XiB- -> tau -> mu
      if (tp->absPdgId() == 5232 && has_tau) return LeptonTruthLabel::BjetLeptonic; // XiB0 -> tau -> mu
      indent += " ";
      tp = tp->parent();
      if (!tp) break;
    }
  }
  
  if (tp) {
    MSG_INFO("unsure about: truth muon: type " << type << " and origin " << origin);
    MSG_INFO("chain: ");
    std::cout << chain.str() << std::endl;
  }
  else {
    MSG_INFO("unknown truth muon: type " << type << " and origin " << origin 
             << ". Truth particle link does not exist.");
  }

  return LeptonTruthLabel::NoTruthLabel;
}

double PSL::xAODWrapper::MllForEwkRejection(int iptordered){
  double min_delta = 999999999;
  double mll = -999.;
  double closest_to=91.188;
  Particle a = m_evtdef->PtOrderedLepton(iptordered);
  if (!a.tlv.Pt()) return -999.;
  
  for (int j=0;j<GetEventVariable(vnSignalLeptons);++j){
    if (iptordered == j) continue;
    if (GetSignalLeptonVariable(lepFlavor,iptordered) != GetSignalLeptonVariable(lepFlavor,j)) continue;
    if (GetSignalLeptonVariable(lepCharge,iptordered) == GetSignalLeptonVariable(lepCharge,j)) continue;
    if (GetSignalLeptonVariable(lepFlavor,iptordered) == (int)ObjType::Electron
        && !ptorderedLikelihood(j,ElectronID::MediumLLH)) continue;
    Particle b = m_evtdef->PtOrderedLepton(j);
    double tmp = (a.tlv+b.tlv).M()/1000.;
    if (fabs(tmp-closest_to) < min_delta){
      min_delta = fabs(tmp-closest_to);
      mll = tmp;
    }
  }
  return mll;
  
}

bool PSL::xAODWrapper::isZgammaEvent(void){

  if (!m_event->retrieve(truthphotons,"TruthPhotons").isSuccess()) {
    xaodWarnOnce("tp_cont_retrieve","Warning! TruthPhotons not found!");
    return false;
  }

  bool hasFSRphoton = false;

  for(unsigned int i=0;i<truthphotons->size();++i){
    const xAOD::TruthParticle* tpho = getTruthPhoton(i);
    if( (ParticleOrigin(*tpho) == 40) && (tpho->p4().Pt() > 10000.) ){
      MSG_DEBUG("Found FSR photon");
      MSG_DEBUG("Origin \t " << ParticleOrigin(*tpho));
      MSG_DEBUG("Type \t " << ParticleType(*tpho));
      MSG_DEBUG("Status \t " << tpho->status());
      MSG_DEBUG("Barcode \t " << tpho->barcode());
      MSG_DEBUG("Outcome \t " << ParticleOutCome(*tpho));
      MSG_DEBUG("Mother \t " << ParticleMother(*tpho));
      MSG_DEBUG("Pt \t " << tpho->p4().Pt());
      MSG_DEBUG("Eta \t " << tpho->p4().Eta());
      MSG_DEBUG("Phi \t " << tpho->p4().Phi());
      MSG_DEBUG("Mass \t " << tpho->p4().M());
      MSG_DEBUG("PDG ID \t " << tpho->absPdgId());
      hasFSRphoton = true;
    }
  }

  return hasFSRphoton;

}

double PSL::xAODWrapper::GetContainerMuonSF(int icontainer,xAOD::Muon::Quality q,AnaIso::AnaIso iso_wp,bool skip_reco,bool skip_iso,bool skip_ttva){
  if (!muons) return 1;
  if (IsData()) return 1;
  if (IsMC()) return 1;
  const xAOD::Muon* mu = getMuon(icontainer);  
  float reco_sf = 1.;
  float iso_sf = 1.;
  float ttva_sf = 1.;

  // reco
  if (skip_reco) {} // do nothing
  else if (q == xAOD::Muon::Loose ) m_muonsfs[MuonSF_Loose ]->getEfficiencyScaleFactor(*mu,reco_sf).ignore();
  else if (q == xAOD::Muon::Medium) m_muonsfs[MuonSF_Medium]->getEfficiencyScaleFactor(*mu,reco_sf).ignore();
  else {
    MSG_INFO("Tried to get a muon ID scale factor that does not exist! " 
             << ConvertMuonIDToStr(q)
             );
    exit(1);
  }
  
  // iso
  if (skip_iso) {} // do nothing
  else if (iso_wp == AnaIso::LooseTrackOnly) m_muonsfs[MuonSF_isolLooseTrackOnly]->getEfficiencyScaleFactor(*mu,iso_sf).ignore();
  else if (iso_wp == AnaIso::GradientLoose ) m_muonsfs[MuonSF_isolGradientLoose ]->getEfficiencyScaleFactor(*mu,iso_sf).ignore();
  else {
    MSG_INFO("Tried to get a muon ID+iso scale factor that does not exist! " 
             << ConvertMuonIDToStr(q)
             << " isolation: " << ConvertAnaIsoToStr(iso_wp)
             );
    exit(1);
  }

  // iso
  if (skip_ttva) {} // do nothing
  else {
    m_muonsfs[MuonSF_TTVA]->getEfficiencyScaleFactor(*mu,ttva_sf).ignore();
  }
  
  return reco_sf * iso_sf * ttva_sf;
}

double PSL::xAODWrapper::GetContainerEleSF(int icontainer,ElectronID::ElectronID e,bool is_d0z0,AnaIso::AnaIso iso_wp,bool skip_reco,bool skip_id,bool skip_iso){
  //
  // Get any combination of scale factors. Options to skip reco or ID (specify None)
  //
  if (!electrons) return 1;
  if (IsData()) return 1;
  const xAOD::Electron* ele = getElectron(icontainer);  
  double reco_sf = 1.;
  double id_sf = 1.;
  double iso_sf = 1.;
  
  // reco
  if (skip_reco) {} // do nothing
  else m_elesfs[ElectronSF_RecoTrk]->getEfficiencyScaleFactor(*ele,reco_sf).ignore();
  
  // id
  if (skip_id) {} // do nothing
  else if (e == ElectronID::None) {} // do nothing
#ifndef BEFORE_SUSYTOOLS_000611
  else if (e == ElectronID::LooseAndBLayerLLH && is_d0z0) m_elesfs[ElectronSF_LooseAndBLayerLLH_d0z0]->getEfficiencyScaleFactor(*ele,id_sf).ignore();
#endif // BEFORE_SUSYTOOLS_000611
  else if (e == ElectronID::MediumLLH && is_d0z0) m_elesfs[ElectronSF_MediumLLH_d0z0]->getEfficiencyScaleFactor(*ele,id_sf).ignore();
  else if (e == ElectronID::TightLLH && is_d0z0) m_elesfs[ElectronSF_TightLLH_d0z0]->getEfficiencyScaleFactor(*ele,id_sf).ignore();
  else {
    MSG_INFO("Tried to get an electron ID scale factor that does not exist! "
             << ConvertElectronIDToStr(e) 
             << (is_d0z0 ? " with d0z0 " : ""));
    exit(1);
  }
  
  // isolation
  if (skip_iso) {} // do nothing
  else if (iso_wp == AnaIso::None) {} // do nothing
  else if (e == ElectronID::TightLLH && is_d0z0 && iso_wp == AnaIso::Gradient) m_elesfs[ElectronIsoSF_TightLLH_d0z0_v8_isolGradient]->getEfficiencyScaleFactor(*ele,iso_sf).ignore();
  else if (e == ElectronID::MediumLLH && is_d0z0 && iso_wp == AnaIso::GradientLoose) m_elesfs[ElectronIsoSF_MediumLLH_d0z0_v8_isolGradientLoose]->getEfficiencyScaleFactor(*ele,iso_sf).ignore();
  else if (e == ElectronID::LooseAndBLayerLLH && is_d0z0 && iso_wp == AnaIso::LooseTrackOnly) m_elesfs[ElectronIsoSF_LooseAndBLayerLLH_d0z0_v8_isolLooseTrackOnly]->getEfficiencyScaleFactor(*ele,iso_sf).ignore();
  else {
    MSG_INFO("Tried to get an electron ID+iso scale factor that does not exist! " 
             << ConvertElectronIDToStr(e) 
              << (is_d0z0 ? " with d0z0 " : "") 
             << " isolation: " << ConvertAnaIsoToStr(iso_wp));
    exit(1);
  }
  // return the product
  return reco_sf * id_sf * iso_sf;
}

bool PSL::xAODWrapper::applySystematicVariation(const CP::SystematicSet& systConfig){
  for (auto it=m_muonsfs.begin();it != m_muonsfs.end();++it){
    // We do not apply the systematic to MuonSF_Loose_NOSYST because it is used for efficiencies.
    if (it->first == MuonSF_Loose_NOSYST) continue;
    if (it->first == MuonSF_isolLooseTrackOnly_NOSYST) continue;
    if (it->first == MuonSF_TTVA_NOSYST) continue;
    if (it->second->applySystematicVariation(systConfig) != CP::SystematicCode::Ok) {
      MSG_INFO("applySystematicVariation: Muon sf systematic failed.");
      return false;
    }
  }
  for (auto it=m_elesfs.begin();it != m_elesfs.end();++it){
    if (it->second->applySystematicVariation(systConfig) != CP::SystematicCode::Ok) {
      MSG_INFO("applySystematicVariation: Electron sf systematic failed.");
      return false;
    }
  }

  if (m_muonTriggerSFTool->applySystematicVariation(systConfig) != CP::SystematicCode::Ok){
    MSG_INFO("applySystematicVariation: Muon trigger SF failed.");
    return false;
  }

  // m_muonTriggerSFToolNOSYST DOES NOT GET SYSTEMATICS APPLIED! USED TO GET EFFICIENCIES!
  
  if (m_prwTool->applySystematicVariation(systConfig) != CP::SystematicCode::Ok) {
    MSG_INFO("applySystematicVariation: PRW systematic failed.");
    return false;
  }

  return true;
}

PSL::Particle PSL::xAODWrapper::GetZZAntiIDParticleFromReco(void){
  TLorentzVector dummy;
  if (!electrons || !muons) return Particle(dummy,ObjType::None,0,0,0);
  if (GetEventVariable(vnSignalLeptons) < 3) return Particle(dummy,ObjType::None,0,0,0);
  if (m_evtdef->leadZlep_index < 0) return Particle(dummy,ObjType::None,0,0,0);
  if (m_evtdef->subleadZlep_index < 0) return Particle(dummy,ObjType::None,0,0,0);
  if (m_evtdef->Wlep_index < 0) return Particle(dummy,ObjType::None,0,0,0);

  Particle p0 = m_evtdef->leps.at(m_evtdef->leadZlep_index);
  Particle p1 =m_evtdef->leps.at(m_evtdef->subleadZlep_index);
  Particle p2 =m_evtdef->leps.at(m_evtdef->Wlep_index);

  int count=0;
  Particle fourth_lep_truth(dummy,ObjType::None,0,0,0);
  Particle tmp_mu(dummy,ObjType::None,0,0,0);
  Particle tmp_ele(dummy,ObjType::None,0,0,0);

  if ( IsMuon(p2) ){ // check if the W lepton is a Muon
    for(unsigned int i=0;i<muons->size();++i){
      const xAOD::Muon* muon = getMuon(i);
      tmp_mu = Particle(muon->p4(),ObjType::Muon,i,muon->charge(),1.0,1.0);
      //reject if looking at one of the signal leptons
      if( ( IsMuon(p0) && (p0.i_cont == (int)i) ) || ( IsMuon(p1) && (p1.i_cont == (int)i) ) || ( IsMuon(p2) && (p2.i_cont == (int)i) ) ) continue;
      if (!passMuonQuality(i,xAOD::Muon::Loose)){
	if (getMuonTruthLabel(i) == LeptonTruthLabel::LeptonZ){
	  if (count == 0) fourth_lep_truth = Particle(muon->p4(),ObjType::Muon,i,muon->charge(),1.0,1.0);
	  else{
	    count++;
	    MSG_ERROR(" !!! " << count << " muons coming from Z");
	  }
	}
      }
    }
  }

  if ( p2.obj_type == ObjType::Electron ) { // check if the W lepton is an electron
    for(unsigned int i=0;i<electrons->size();++i){
      const xAOD::Electron* electron = getElectron(i);
      tmp_ele = Particle(electron->p4(),ObjType::Electron,i,electron->charge(),1.0,1.0);
      //reject if looking at one of the signal leptons
      if( ( IsElectron(p0) && (p0.i_cont == (int)i) ) || ( IsElectron(p1) && (p1.i_cont == (int)i) ) || ( IsElectron(p2) && (p2.i_cont == (int)i) ) ) continue;
      if (!passLikelihood(i,ElectronID::LooseAndBLayerLLH) || fabs(EleD0Significance(i)) > 5.0 || fabs(EleZ0(tmp_ele.i_cont)*sin(tmp_ele.tlv.Theta())) > 0.5 ){ // fail ID or DO or Z0
	  if (getElectronTruthLabel(i) == LeptonTruthLabel::LeptonZ){
	    if (count == 0) fourth_lep_truth = Particle(electron->p4(),ObjType::Electron,i,electron->charge(),1.0,1.0);
	    else{
	      count++;
	      MSG_ERROR(" !!! " << count << " electrons coming from Z");
	    }
	  }
      }
    }
  }

  return fourth_lep_truth;
}

PSL::Particle PSL::xAODWrapper::GetZZAntiIDParticle(void) {
  //Return the 4th Particle

  // load the truth muon and electron containers
  if (!m_event->retrieve(truthelectrons,"TruthElectrons").isSuccess()) {
    MSG_INFO("Error! TruthElectrons not found!");
    exit(1);
  }  
  if (!m_event->retrieve(truthmuons,"TruthMuons").isSuccess()) {
    MSG_INFO("Error! TruthMuons not found!");
    exit(1);
  }

  TLorentzVector dummy;
  if (GetEventVariable(vnSignalLeptons) < 3) return Particle(dummy,ObjType::None,0,0,0);
  if (m_evtdef->leadZlep_index < 0) return Particle(dummy,ObjType::None,0,0,0);
  if (m_evtdef->subleadZlep_index < 0) return Particle(dummy,ObjType::None,0,0,0);
  if (m_evtdef->Wlep_index < 0) return Particle(dummy,ObjType::None,0,0,0);

  int arr_index[3]={-1,-1,-1};
  int arr_type[3]={-1,-1,-1}; // 1 - electron, 2- muon
  double delta_R, tmp_delta_R, tmp_delta_R_match,  particle3_mass, particle4_mass;
  double tmp_mll,delta_mll, muon_mll, ele_mll;
  int match_i;
  int thirdlep_index, thirdlep_type, index_mu_fourth, index_ele_fourth;
  TLorentzVector lep3,lep4;
  
  const xAOD::TruthParticle* fourth_mu_truth = 0;
  const xAOD::TruthParticle* fourth_ele_truth = 0;

  Particle fourth_lep_truth(dummy,ObjType::None,0,0,0);
  
  Particle p0 = m_evtdef->leps.at(m_evtdef->leadZlep_index);
  Particle p1 =m_evtdef->leps.at(m_evtdef->subleadZlep_index);
  Particle p2 =m_evtdef->leps.at(m_evtdef->Wlep_index); 

  /*MSG_ERROR("Event Number " << get_EventNumber());
   MSG_ERROR("P0 charge" << p0.charge << " muon? " << IsMuon(p0) << " eta " << p0.Eta() << " phi " << p0.Phi() );
  if (IsMuon(p0)) MSG_ERROR("truth label " << getMuonTruthLabel(p0.i_cont));
  if (IsElectron(p0)) MSG_ERROR("truth label " << getElectronTruthLabel(p0.i_cont));
  MSG_ERROR("P1 charge" << p1.charge << " muon? " << IsMuon(p1)<< " eta " << p1.Eta() << " phi " << p1.Phi() );
  if (IsMuon(p1)) MSG_ERROR("truth label " << getMuonTruthLabel(p1.i_cont));
  if (IsElectron(p1)) MSG_ERROR("truth label "<< getElectronTruthLabel(p1.i_cont));
  MSG_ERROR("P2 charge" << p2.charge << " muon? " << IsMuon(p2)<< " eta " << p2.Eta() << " phi " << p2.Phi() );
  if (IsMuon(p2)) MSG_ERROR("truth label " << getMuonTruthLabel(p2.i_cont));
  if (IsElectron(p2)) MSG_ERROR("truth label "<< getElectronTruthLabel(p2.i_cont));
  */

  //Elodie changed these to -1, otherwise if they don't get filled it will get the first object which we might not want
  thirdlep_index = -1;
  thirdlep_type = -1;
  index_mu_fourth = -1;
  index_ele_fourth = -1;
  ele_mll=0.0;
  muon_mll=0.0;

  // matching the truth and reco leptons by minimizing deltaR
  for(unsigned int i=0;i<truthmuons->size();++i){
    delta_R = 0.1;
    match_i=10;
    const xAOD::TruthParticle* tmp_mu = getTruthMuon(i);

    TLorentzVector dressed_mu;
    dressed_mu.SetPtEtaPhiE(m_acc_pt_dressed(*tmp_mu),m_acc_eta_dressed(*tmp_mu),m_acc_phi_dressed(*tmp_mu),m_acc_e_dressed(*tmp_mu));

    /* MSG_ERROR("This is the undressed pt " << tmp_mu->pt() << " vs the dressed pt " << m_acc_pt_dressed(*tmp_mu) << " is it different?")
    MSG_ERROR("mu charge" << tmp_mu->charge() << " index " << i<< " eta " << tmp_mu->eta() << " phi " << tmp_mu->phi() );
    MSG_ERROR("dressed phi: " << dressed_mu.Phi() << " eta: " << dressed_mu.Eta() );
    MSG_ERROR(" origin " << ParticleOrigin(*tmp_mu) << " type " << ParticleType(*tmp_mu));*/
    //if ((IsMuon(p0)) && (tmp_mu->charge() == p0.charge)){
    if (IsMuon(p0)){
      tmp_delta_R = sqrt(pow((dressed_mu.Phi()-p0.Phi()),2)+pow((dressed_mu.Eta()-p0.Eta()),2));

      if ((arr_type[0] == 2) && (arr_index[0] > -1)){
  const xAOD::TruthParticle* tmp_mu_match = getTruthMuon(arr_index[0]); 
  TLorentzVector dressed_mu_match;
  dressed_mu_match.SetPtEtaPhiE(m_acc_pt_dressed(*tmp_mu_match),m_acc_eta_dressed(*tmp_mu_match),m_acc_phi_dressed(*tmp_mu_match),m_acc_e_dressed(*tmp_mu_match));
  tmp_delta_R_match = sqrt(pow((dressed_mu_match.Phi()-p0.Phi()),2)+pow((dressed_mu_match.Eta()-p0.Eta()),2));
      }
      else tmp_delta_R_match = 5;

      //MSG_ERROR (" 0: " << tmp_delta_R << " " << tmp_delta_R_match);
      if ((tmp_delta_R<= delta_R) && (tmp_delta_R < tmp_delta_R_match)){
        match_i=0;
        delta_R = tmp_delta_R;
      }
    }

    //if ((IsMuon(p1))&& (tmp_mu->charge() ==p1.charge)){
    if(IsMuon(p1)){  
      tmp_delta_R = sqrt(pow((dressed_mu.Phi()-p1.Phi()),2)+pow((dressed_mu.Eta()-p1.Eta()),2));

      if ((arr_type[1] == 2) && (arr_index[1] > -1)){
  const xAOD::TruthParticle* tmp_mu_match = getTruthMuon(arr_index[1]);
  TLorentzVector dressed_mu_match;
  dressed_mu_match.SetPtEtaPhiE(m_acc_pt_dressed(*tmp_mu_match),m_acc_eta_dressed(*tmp_mu_match),m_acc_phi_dressed(*tmp_mu_match),m_acc_e_dressed(*tmp_mu_match));
  tmp_delta_R_match = sqrt(pow((dressed_mu_match.Phi()-p1.Phi()),2)+pow((dressed_mu_match.Eta()-p1.Eta()),2));
      }
      else tmp_delta_R_match = 5;

      //MSG_ERROR(" 1: "<< tmp_delta_R << " " << tmp_delta_R_match);
      if ((tmp_delta_R<=delta_R) && (tmp_delta_R < tmp_delta_R_match)){
        match_i=1;
        delta_R = tmp_delta_R;
      } 
    }

    //if ((IsMuon(p2))&& (tmp_mu->charge() ==p2.charge)){
    if (IsMuon(p2)){  
      tmp_delta_R = sqrt(pow((dressed_mu.Phi()-p2.Phi()),2)+pow((dressed_mu.Eta()-p2.Eta()),2));

      if ((arr_type[2] == 2) && (arr_index[2] > -1)){
  const xAOD::TruthParticle* tmp_mu_match = getTruthMuon(arr_index[2]);
  TLorentzVector dressed_mu_match;
  dressed_mu_match.SetPtEtaPhiE(m_acc_pt_dressed(*tmp_mu_match),m_acc_eta_dressed(*tmp_mu_match),m_acc_phi_dressed(*tmp_mu_match),m_acc_e_dressed(*tmp_mu_match));
  tmp_delta_R_match = sqrt(pow((dressed_mu_match.Phi()-p2.Phi()),2)+pow((dressed_mu_match.Eta()-p2.Eta()),2));
      }
      else tmp_delta_R_match = 5;

      //MSG_ERROR(" 2: "<< tmp_delta_R << " " << tmp_delta_R_match);
      if ((tmp_delta_R<=delta_R) && (tmp_delta_R < tmp_delta_R_match)){
        match_i=2;
        delta_R = tmp_delta_R;
        thirdlep_index = i;
        thirdlep_type= 2;
      }
    }

    if (match_i<10){
      arr_index[match_i] = i;
      arr_type[match_i] = 2;
      //MSG_ERROR("index " << i << " match " << match_i );
    }
  }// end of muon loop
  //MSG_ERROR("third lep index " << thirdlep_index << " type " << thirdlep_type);
 
  for(unsigned int i=0;i<truthelectrons->size();++i){
    //MSG_ERROR("Back here 8");
    delta_R = 0.1;
    match_i=10;
    const xAOD::TruthParticle* tmp_ele = getTruthElectron(i);

    //MSG_ERROR("This is the undressed pt " << tmp_ele->pt() << " vs the dressed pt " << m_acc_pt_dressed(*tmp_ele) << " is it different?")

    TLorentzVector dressed_ele;
    dressed_ele.SetPtEtaPhiE(m_acc_pt_dressed(*tmp_ele),m_acc_eta_dressed(*tmp_ele),m_acc_phi_dressed(*tmp_ele),m_acc_e_dressed(*tmp_ele));
    /*MSG_ERROR("mu charge" << tmp_ele->charge() << " index " << i<< " eta " << tmp_ele->eta() << " phi " << tmp_ele->phi() );
    MSG_ERROR("dressed phi: " << dressed_ele.Phi() << " eta: " << dressed_ele.Eta() );
    MSG_ERROR(" origin " << ParticleOrigin(*tmp_ele) << " type " << ParticleType(*tmp_ele));*/

    //if ((IsElectron(p0))&& (tmp_ele->charge() == p0.charge)){
    if(IsElectron(p0)){ 
      tmp_delta_R = sqrt(pow((dressed_ele.Phi()-p0.Phi()),2)+pow((dressed_ele.Eta()-p0.Eta()),2));
      
      if ((arr_type[0] == 1) && (arr_index[0] > -1)){
  const xAOD::TruthParticle* tmp_ele_match = getTruthElectron(arr_index[0]);
  TLorentzVector dressed_ele_match;
  dressed_ele_match.SetPtEtaPhiE(m_acc_pt_dressed(*tmp_ele_match),m_acc_eta_dressed(*tmp_ele_match),m_acc_phi_dressed(*tmp_ele_match),m_acc_e_dressed(*tmp_ele_match));
  tmp_delta_R_match = sqrt(pow((dressed_ele_match.Phi()-p0.Phi()),2)+pow((dressed_ele_match.Eta()-p0.Eta()),2));
      }
      else tmp_delta_R_match = 5;

      //MSG_ERROR(" 0: "<< tmp_delta_R << " " << tmp_delta_R);
      if ((tmp_delta_R<= delta_R) && (tmp_delta_R < tmp_delta_R_match)){
        match_i=0;
        delta_R = tmp_delta_R;
      }
    }

    //if ((IsElectron(p1))&& (tmp_ele->charge() ==p1.charge)){
    if (IsElectron(p1)){  
      tmp_delta_R = sqrt(pow((dressed_ele.Phi()-p1.Phi()),2)+pow((dressed_ele.Eta()-p1.Eta()),2));

      if ((arr_type[1] == 1) && (arr_index[1] > -1)){
        const xAOD::TruthParticle* tmp_ele_match = getTruthElectron(arr_index[1]);
        TLorentzVector dressed_ele_match;
        dressed_ele_match.SetPtEtaPhiE(m_acc_pt_dressed(*tmp_ele_match),m_acc_eta_dressed(*tmp_ele_match),m_acc_phi_dressed(*tmp_ele_match),m_acc_e_dressed(*tmp_ele_match));
        tmp_delta_R_match = sqrt(pow((dressed_ele_match.Phi()-p1.Phi()),2)+pow((dressed_ele_match.Eta()-p1.Eta()),2));
      }
      else tmp_delta_R_match = 5;

      //MSG_ERROR(" 1: "<< tmp_delta_R << " " << tmp_delta_R_match);
      if ((tmp_delta_R<=delta_R) && (tmp_delta_R < tmp_delta_R_match)){
        match_i=1;
        delta_R = tmp_delta_R;
      }
    }

    //if ((IsElectron(p2))&& (tmp_ele->charge() ==p2.charge)){
    if (IsElectron(p2)){  
      tmp_delta_R = sqrt(pow((dressed_ele.Phi()-p2.Phi()),2)+pow((dressed_ele.Eta()-p2.Eta()),2));
      
      if ((arr_type[2] == 1) && (arr_index[2] > -1)){
        const xAOD::TruthParticle* tmp_ele_match = getTruthElectron(arr_index[2]);
        TLorentzVector dressed_ele_match;
        dressed_ele_match.SetPtEtaPhiE(m_acc_pt_dressed(*tmp_ele_match),m_acc_eta_dressed(*tmp_ele_match),m_acc_phi_dressed(*tmp_ele_match),m_acc_e_dressed(*tmp_ele_match));
        tmp_delta_R_match = sqrt(pow((dressed_ele_match.Phi()-p2.Phi()),2)+pow((dressed_ele_match.Eta()-p2.Eta()),2));
      }
      else tmp_delta_R_match = 5;

      //MSG_ERROR(" 2: "<< tmp_delta_R << " " << tmp_delta_R_match);
      if ((tmp_delta_R<=delta_R) && (tmp_delta_R < tmp_delta_R_match)){
        match_i=2;
        delta_R = tmp_delta_R;
        thirdlep_index = i;
        thirdlep_type = 1;
      }
    }

    if (match_i<10){
      arr_index[match_i] = i;
      arr_type[match_i] = 1;                                                                                        
      //MSG_ERROR("index: " << i << " match i " << match_i );
    }
  }//end of electron loop

  //MSG_ERROR("third lep index " << thirdlep_index << " type " <<thirdlep_type);
  //MSG_ERROR(" index 0 " << arr_index[0] << " type " << arr_type[0] << " index 1 "<< arr_index[1] << " type " << arr_type[1] << " index 2 "<< arr_index[2] << " type " << arr_type[2]);
  
  if ((arr_index[0] > -1) && (arr_index[1] > -1) && (arr_index[1] > -1) && (thirdlep_index>-1)){ 
    //MSG_ERROR("Back here 9");
    // making the TLV of the W lepton
    particle3_mass = (thirdlep_type == 1)? 0.511:105.658; //MeV

    const xAOD::TruthParticle *tmp_lep;

    if (thirdlep_type == 1) tmp_lep = getTruthElectron(thirdlep_index);
    else tmp_lep = getTruthMuon(thirdlep_index);

    lep3.SetPtEtaPhiM(m_acc_pt_dressed(*tmp_lep),m_acc_eta_dressed(*tmp_lep),m_acc_phi_dressed(*tmp_lep),particle3_mass);
    
    delta_mll = 100.0;

    if (thirdlep_type == 2){
      for (unsigned int i=0;i<truthmuons->size();i++){
  const xAOD::TruthParticle* tmp_mu = getTruthMuon(i);
  //if (m_acc_pt_dressed(*tmp_mu) < 1000.) continue; // do we want this

  if (((arr_index[0] == (int)i) && (arr_type[0] == 2))|| ((arr_index[1] == (int)i) && (arr_type[1] ==2)) || ((arr_index[2] == (int)i) && (arr_type[2] ==2))) continue; 
  if (tmp_lep->charge() == tmp_mu->charge()) continue;
  particle4_mass = 105.658; //MeV
  lep4.SetPtEtaPhiM(m_acc_pt_dressed(*tmp_mu),m_acc_eta_dressed(*tmp_mu),m_acc_phi_dressed(*tmp_mu),particle4_mass);
  tmp_mll = (lep3+lep4).M()/1000.;
  //MSG_ERROR("tmp mll " << tmp_mll) ;
  if (fabs(tmp_mll - 91.188)< delta_mll){
    delta_mll = fabs(tmp_mll - 91.188);
    muon_mll = tmp_mll;
    index_mu_fourth = i;
    //MSG_ERROR("muon mll " << muon_mll << " index 4 " << i);
    //MSG_ERROR("4th eta " << m_acc_eta_dressed(*tmp_mu) << " phi " << m_acc_phi_dressed(*tmp_mu)<< " pt " << m_acc_pt_dressed(*tmp_mu));
    //MSG_ERROR("Back here 10");
  }
      }
      if (index_mu_fourth > -1){
  TLorentzVector fourth_truth_dressed;
        fourth_mu_truth = getTruthMuon(index_mu_fourth);
        fourth_truth_dressed.SetPtEtaPhiM(m_acc_pt_dressed(*fourth_mu_truth),m_acc_eta_dressed(*fourth_mu_truth),m_acc_phi_dressed(*fourth_mu_truth),105.658);
        fourth_lep_truth = Particle(fourth_truth_dressed,ObjType::Muon,index_mu_fourth,fourth_mu_truth->charge(),1.0,1.0);
  //MSG_ERROR("muon mll " << muon_mll);                                                                                             
  //MSG_ERROR("4th eta " << m_acc_eta_dressed(*fourth_mu_truth) << " phi " << m_acc_phi_dressed(*fourth_mu_truth)<< " pt " << m_acc_pt_dressed(*fourth_mu_truth));
      }
    }// end of muon loop

    delta_mll = 100.0;
    
    if (thirdlep_type ==1){
      for (unsigned int i=0; i<truthelectrons->size();++i){
  const xAOD::TruthParticle* tmp_ele = getTruthElectron(i);

  //if (m_acc_pt_dressed(*tmp_ele) < 1000.) continue; //do we want this?

  if (((arr_index[0] == (int)i) && (arr_type[0] == 1))|| ((arr_index[1] == (int)i) && (arr_type[1] ==1)) || ((arr_index[2] == (int)i) && (arr_type[2] ==1))) continue;
  if (tmp_lep->charge() == tmp_ele->charge()) continue;
  particle4_mass = 0.511; //MeV                                                                               
  lep4.SetPtEtaPhiM(m_acc_pt_dressed(*tmp_ele), m_acc_eta_dressed(*tmp_ele),m_acc_phi_dressed(*tmp_ele),particle4_mass);
  tmp_mll = (lep3+lep4).M()/1000.;
  //MSG_ERROR("tmp mll " << tmp_mll);
  if (fabs(tmp_mll - 91.188)< delta_mll){
    delta_mll= fabs(tmp_mll -91.188);
    ele_mll = tmp_mll;
    index_ele_fourth = i;
    //MSG_ERROR("ele mll " << ele_mll << " index 4 " << i);
    // MSG_ERROR("4th eta " << m_acc_eta_dressed(*tmp_ele) <<" pt " << m_acc_pt_dressed(*tmp_ele));
    //MSG_ERROR("Back here 11");
  }
      }
      if (index_ele_fourth > -1){
  TLorentzVector fourth_truth_dressed;
  fourth_ele_truth = getTruthElectron(index_ele_fourth);
  fourth_truth_dressed.SetPtEtaPhiM(m_acc_pt_dressed(*fourth_ele_truth),m_acc_eta_dressed(*fourth_ele_truth),m_acc_phi_dressed(*fourth_ele_truth),0.511);
  fourth_lep_truth = Particle(fourth_truth_dressed,ObjType::Electron,index_ele_fourth,fourth_ele_truth->charge(),1.0,1.0);
  //MSG_ERROR("electron mll " << ele_mll);
        //MSG_ERROR("4th eta " << m_acc_eta_dressed(*fourth_ele_truth) << " phi " << m_acc_phi_dressed(*fourth_ele_truth)<< " pt " << m_acc_pt_dressed(*fourth_ele_truth));
      }
    } //end of electron loop
  }
  return fourth_lep_truth;
}

int PSL::xAODWrapper::GetNLepsfromZ(Particle truthLepton){
  if (truthLepton.obj_type == ObjType::None) return -1;

  Particle p0 = m_evtdef->leps.at(0);
  Particle p1 = m_evtdef->leps.at(1);
  Particle p2 = m_evtdef->leps.at(2);
  
  int count = 0;
  if ( IsMuon(truthLepton) ){
    for(unsigned int i=0;i<muons->size();++i){
 
      if( ( IsMuon(p0) && (p0.i_cont == (int)i) ) || ( IsMuon(p1) && (p1.i_cont == (int)i) ) || ( IsMuon(p2) && (p2.i_cont == (int)i) ) ) continue;
      if (getMuonTruthLabel(i) == LeptonTruthLabel::LeptonZ) count ++;
    }
  }

  if ( IsElectron(truthLepton) ) {
    for(unsigned int i=0;i<electrons->size();++i){

      //reject if looking at one of the signal leptons                                                                                                             
      if( ( IsElectron(p0) && (p0.i_cont == (int)i) ) || ( IsElectron(p1) && (p1.i_cont == (int)i) ) || ( IsElectron(p2) && (p2.i_cont == (int)i) ) ) continue;
      if (getElectronTruthLabel(i) == LeptonTruthLabel::LeptonZ) count ++;
    }
  }
  
  return count;
}
    

PSL::Particle PSL::xAODWrapper::GetMatchingRecoLepton(Particle truthLepton) {

  /*
  if((truthLepton.obj_type == ObjType::Muon)) {
    MSG_ERROR("Investigating truth muon " << truthLepton.tlv.Pt() << "\t" << truthLepton.tlv.Eta() << "\t" << truthLepton.tlv.Phi());
  } else if ((truthLepton.obj_type == ObjType::Electron)) {
    MSG_ERROR("Investigating truth electron " << truthLepton.tlv.Pt() << "\t" << truthLepton.tlv.Eta() << "\t" << truthLepton.tlv.Phi());
  } else {
    MSG_ERROR("Investigating truth particle " << truthLepton.tlv.Pt() << "\t" << truthLepton.tlv.Eta() << "\t" << truthLepton.tlv.Phi());
  }*/

  TLorentzVector dummy;
  if (truthLepton.obj_type == ObjType::None) return Particle(dummy,ObjType::None,0,0,0);

  Particle closestRecoParticle(dummy,ObjType::None,0,0,0);

  Particle p0 = m_evtdef->leps.at(0);
  Particle p1 = m_evtdef->leps.at(1);
  Particle p2 = m_evtdef->leps.at(2);

  //if the truth particle is a muon within acceptance find matching muon in reco container
  if ( IsMuon(truthLepton) && (truthLepton.tlv.Pt()>7000.) && (fabs(truthLepton.tlv.Eta())<2.5) ) {

    float closestRecoMuonDR = 999.;
    float closestRecoMuonIndex = -1;

    for(unsigned int i=0;i<muons->size();++i){

      const xAOD::Muon* muon = getMuon(i);

      //reject if looking at one of the signal leptons or if the charge is not correct
      if( ( IsMuon(p0) && (p0.i_cont == (int)i) ) || ( IsMuon(p1) && (p1.i_cont == (int)i) ) || ( IsMuon(p2) && (p2.i_cont == (int)i) ) ) continue; 
      if(muon->charge() != truthLepton.charge) continue;

      float currentDR = muon->p4().DeltaR(truthLepton.tlv);

      if(currentDR < closestRecoMuonDR){
        closestRecoMuonDR = currentDR;
        closestRecoMuonIndex = i;
      }
    } //loop container muons

    if(closestRecoMuonIndex>-1 && closestRecoMuonDR < 0.1) {
      const xAOD::Muon* closestMuon = getMuon(closestRecoMuonIndex);
      closestRecoParticle = Particle(closestMuon->p4(),ObjType::Muon,closestRecoMuonIndex,closestMuon->charge(),1.0,1.0);
      //MSG_ERROR("Matching reco muon " << closestRecoParticle.tlv.Pt() << "\t" << closestRecoParticle.tlv.Eta() << "\t" <<closestRecoParticle.tlv.Phi());
    }

  }//truth muon in acceptance

  if ( IsElectron(truthLepton) && (truthLepton.tlv.Pt()>7000.) && (fabs(truthLepton.tlv.Eta())<2.5) ) {

    float closestRecoElectronDR = 999.;
    float closestRecoElectronIndex = -1;

    for(unsigned int i=0;i<electrons->size();++i){

      const xAOD::Electron* electron = getElectron(i);

      //reject if looking at one of the signal leptons
      if( ( IsElectron(p0) && (p0.i_cont == (int)i) ) || ( IsElectron(p1) && (p1.i_cont == (int)i) ) || ( IsElectron(p2) && (p2.i_cont == (int)i) ) ) continue; 
      if(electron->charge() != truthLepton.charge) continue;

      float currentDR = electron->p4().DeltaR(truthLepton.tlv);

      if(currentDR < closestRecoElectronDR){
        closestRecoElectronDR = currentDR;
        closestRecoElectronIndex = i;
      }
    } //loop container electrons

    if(closestRecoElectronIndex>-1 && closestRecoElectronDR < 0.1) {
      const xAOD::Electron* closestElectron = getElectron(closestRecoElectronIndex);
      closestRecoParticle = Particle(closestElectron->p4(),ObjType::Electron,closestRecoElectronIndex,closestElectron->charge(),1.0,1.0);
      //MSG_ERROR("Matching reco electron " << closestRecoParticle.tlv.Pt() << "\t" << closestRecoParticle.tlv.Eta() << "\t" <<closestRecoParticle.tlv.Phi());
      //MSG_ERROR("found electron match");
    }
    /*
    else {
      MSG_ERROR("didn't find electron match look for photon");

      const xAOD::PhotonContainer* photons(0);
      if( !m_event->retrieve(photons,"Photons").isSuccess()){
        MSG_ERROR("Could not retrieve Photon container!! That's bad!!");
        return Particle(dummy,ObjType::None,0,0,0);
      }

      float closestRecoPhotonDR = 999.;
      float closestRecoPhotonIndex = -1;

      for(unsigned int i=0;i<photons->size();++i){

        const xAOD::Photon* photon = getPhoton(i);

        float currentDR = photon->p4().DeltaR(truthLepton.tlv);

        if(currentDR < closestRecoPhotonDR){
          closestRecoPhotonDR = currentDR;
          closestRecoPhotonIndex = i;
        }

      } //loop container photons

      if(closestRecoPhotonIndex>-1 && closestRecoPhotonDR < 0.1) {
        const xAOD::Photon* closestPhoton = getPhoton(closestRecoPhotonIndex);
        closestRecoParticle = Particle(closestPhoton->p4(),ObjType::Photon,closestRecoPhotonIndex,0,1.0,1.0);
      }
    }//didn't find any possible electron match, look for photons
    */

  }//truth electron in acceptance
  
  return closestRecoParticle;

}

double PSL::xAODWrapper::CalculateZZAntiIDScaleFactor(Particle forthLepton) {
  //return the SF

  if (forthLepton.obj_type == ObjType::None) return 1.;

  //
  // FOURTH MUON CASE
  //
  float reco_sf_mu = 1.;
  if ( (forthLepton.obj_type == ObjType::Muon) && (forthLepton.tlv.Pt()>7000.) && (fabs(forthLepton.tlv.Eta())<2.5) ) {

    //MSG_ERROR("handling truth muons " << forthLepton.tlv.Pt());
    xAOD::MuonContainer* cont = new xAOD::MuonContainer();
    xAOD::MuonAuxContainer* copyAux = new xAOD::MuonAuxContainer();
    cont->setStore( copyAux );
    xAOD::Muon* tmp_muon = new xAOD::Muon();
    cont->push_back(tmp_muon);
    tmp_muon->setP4(forthLepton.tlv.Pt(),forthLepton.tlv.Eta(),forthLepton.tlv.Phi());
    if (!tmp_muon) {
      MSG_INFO("Error! Failed to cast truth particle to xAOD::Muon");
      exit(1);
    }
    const xAOD::Muon* tmp_muon_const = tmp_muon;
    // Get the scale factors you need, do the calculation.
    float tmp_sf_mu = 1.;
    float tmp_eff_mu = 0.;

    float tmp_sf_mu_reco = 1.;
    float tmp_eff_mu_reco = 0.;

    float tmp_sf_mu_iso = 1.;
    float tmp_eff_mu_iso = 0.;

    float tmp_sf_mu_ttva = 1.;
    float tmp_eff_mu_ttva = 0.;

    m_muonsfs[MuonSF_Loose ]->getEfficiencyScaleFactor(*tmp_muon_const,tmp_sf_mu_reco).ignore();
    m_muonsfs[MuonSF_Loose_NOSYST ]->getRecoEfficiency(*tmp_muon_const,tmp_eff_mu_reco).ignore();

    m_muonsfs[MuonSF_isolLooseTrackOnly ]->getEfficiencyScaleFactor(*tmp_muon_const,tmp_sf_mu_iso).ignore();
    m_muonsfs[MuonSF_isolLooseTrackOnly_NOSYST ]->getRecoEfficiency(*tmp_muon_const,tmp_eff_mu_iso).ignore();

    m_muonsfs[MuonSF_TTVA ]->getEfficiencyScaleFactor(*tmp_muon_const,tmp_sf_mu_ttva).ignore();
    m_muonsfs[MuonSF_TTVA_NOSYST ]->getRecoEfficiency(*tmp_muon_const,tmp_eff_mu_ttva).ignore();

    //tmp_eff_mu = tmp_eff_mu_reco*tmp_eff_mu_iso*tmp_eff_mu_ttva;
    //tmp_sf_mu = tmp_sf_mu_reco*tmp_sf_mu_iso*tmp_sf_mu_ttva;

    tmp_eff_mu = tmp_eff_mu_reco;
    tmp_sf_mu = tmp_sf_mu_reco;

    if ( (fabs(tmp_sf_mu) > 0.00001) && (tmp_eff_mu < 0.99999)) reco_sf_mu = (1. - tmp_eff_mu) / (1. - (tmp_eff_mu/tmp_sf_mu) );

    // Need these or else you have a memory leak
    if (copyAux) delete copyAux;
    if (cont) delete cont;
  }

  //
  // FOURTH ELECTRON CASE
  //
  double reco_sf_ele = 1;
  if (forthLepton.obj_type == ObjType::Electron && (forthLepton.tlv.Pt()>7000.) && (fabs(forthLepton.tlv.Eta())<2.5) ) {
    //
    // To get this recipe see
    // ElectronEfficiencyCorrection/util/testEgEfficiencyCorrWithoutFile.cxx
    //
    xAOD::ElectronContainer* cont = new xAOD::ElectronContainer();
    xAOD::ElectronAuxContainer* copyAux = new xAOD::ElectronAuxContainer();
    cont->setStore( copyAux );
    xAOD::CaloClusterContainer* clus_cont = new xAOD::CaloClusterContainer();
    xAOD::CaloClusterAuxContainer* clus_contAux = new xAOD::CaloClusterAuxContainer();
    clus_cont->setStore(clus_contAux);
    wk()->xaodStore()->record(clus_cont,"ZZVetoClusters").ignore();
    wk()->xaodStore()->record(clus_contAux,"ZZVetoClustersAux").ignore();

    xAOD::CaloCluster* cluster = new xAOD::CaloCluster();
    cluster->makePrivateStore();
    clus_cont->push_back(cluster);
    std::vector< ElementLink< xAOD::CaloClusterContainer > > links_clusters;
    links_clusters.push_back(ElementLink< xAOD::CaloClusterContainer >( cluster, *clus_cont ));
    xAOD::Electron* tmp_ele = new xAOD::Electron();
    cont->push_back(tmp_ele);
    tmp_ele->setCaloClusterLinks(links_clusters);

    // Really annoying setup of the calorimeter cluster
    unsigned sampling_pattern = 0;
    CaloSampling::CaloSample which_calosample;
    if (fabs(forthLepton.tlv.Eta()) < 1.45){
      which_calosample = CaloSampling::EMB2;
      sampling_pattern |= (0x1U << CaloSampling::PreSamplerB);
      sampling_pattern |= (0x1U << CaloSampling::EMB1       );
      sampling_pattern |= (0x1U << CaloSampling::EMB2       );
      sampling_pattern |= (0x1U << CaloSampling::EMB3       );
    } else {
      which_calosample = CaloSampling::EME2;
      sampling_pattern |= (0x1U << CaloSampling::PreSamplerE);
      sampling_pattern |= (0x1U << CaloSampling::EME1       );
      sampling_pattern |= (0x1U << CaloSampling::EME2       );
      sampling_pattern |= (0x1U << CaloSampling::EME3       );
    }
    cluster->setSamplingPattern(sampling_pattern);
    //
    // END Recipe to create electron
    //

    tmp_ele->setP4(forthLepton.tlv.Pt(),forthLepton.tlv.Eta(),forthLepton.tlv.Phi(),0.511);
    cluster->setEta(which_calosample,forthLepton.tlv.Eta());

    if (!tmp_ele) {
      MSG_INFO("Error! Failed to cast truth particle to xAOD::Electron");
      exit(1);
    }
    const xAOD::Electron* tmp_ele_const = tmp_ele;
    MSG_DEBUG(Form("Ele pt, eta, clustereta, phi is %2.3f %2.3f %2.3f %2.3f",tmp_ele_const->pt(),tmp_ele_const->eta(),cluster->etaBE(2),tmp_ele_const->phi()));
    // Get the scale factors you need, do the calculation. This is a dummy calculation!
    m_elesfs[PhilipAntiID]->getEfficiencyScaleFactor(*tmp_ele_const,reco_sf_ele).ignore();
    // Need these or else you have a memory leak
    if (copyAux) delete copyAux;
    if (cont) delete cont;
    // Cluster containers will be deleted by xaodStore.
  }

  MSG_DEBUG("Muon reco sf is: " << reco_sf_mu);
  MSG_DEBUG("Ele reco sf is: " << reco_sf_ele);

  float total_sf = 1;
  total_sf *= reco_sf_mu;
  total_sf *= reco_sf_ele;
  
  return total_sf;
}
// Trigger scale factor functions
double PSL::xAODWrapper::getTriggerSF_Winter2016WZAnalysis(){
  if (IsData()) return 1;
  if (!electrons) return 1;
  if (!muons) return 1;
  /*
  //cross-checking tool result, to be commented out
  int tmp_rdmRN = m_prwTool->getRandomRunNumber( *eventInfo );
  int rdmRN = (tmp_rdmRN == 0) ? 267639 : tmp_rdmRN;
  m_muonTriggerSFTool->setRunNumber(rdmRN);
  xAOD::MuonContainer *SelectedMuon = new xAOD::MuonContainer;
  xAOD::MuonAuxContainer *SelectedMuonAux = new xAOD::MuonAuxContainer;
  SelectedMuon -> setStore(SelectedMuonAux);
  const std::string triggers = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50"; // make this configurable/argument later
  int muCount = 0;
  //cross-checking tool result, to be commented out
  */

  // only want to do this for signal events (2 z-lep, 1 w-lep)
  if(m_evtdef->leps.size() != 3 || m_evtdef->Wlep_index < 0)
    return 1.;
  // get the lepton weights
  std::vector< std::pair<double,double> > eff_leps;

  for(int i=0;i<3;i++){
    int index = m_evtdef->leps.at(i).i_cont;
    if( IsElectron(m_evtdef->leps.at(i)) ){
      eff_leps.push_back( getElectronTriggerWeight(index) );
    }
    else{
      /*
      //cross-checking tool result, to be commented out
      muCount+=1;
      xAOD::Muon* newMuon = new xAOD::Muon();
      SelectedMuon->push_back(newMuon);
      newMuon->setP4(m_evtdef->leps.at(i).tlv.Pt(),m_evtdef->leps.at(i).tlv.Eta(),m_evtdef->leps.at(i).tlv.Phi());
      //cross-checking tool result, to be commented out
      */
      eff_leps.push_back( getMuonTriggerWeight(index) );
    }
  }

  /*
  //cross-checking tool result, to be commented out
  double toolSF = 1.0;
  m_muonTriggerSFTool->getTriggerScaleFactor(*SelectedMuon,toolSF,triggers);
  if(muCount==3) MSG_ERROR(rdmRN << " " << get_EventNumber() << " " << toolSF);
  // Need these or else you have a memory leak
  if (SelectedMuonAux) delete SelectedMuonAux;
  if (SelectedMuon) delete SelectedMuon;
  //cross-checking tool result, to be commented out
  */

  // calculate the event weight
  double numerator = 1 - (1-(eff_leps.at(0).first*eff_leps.at(0).second))*(1-(eff_leps.at(1).first*eff_leps.at(1).second))*(1-(eff_leps.at(2).first*eff_leps.at(2).second));
  double denominator = 1 - (1-eff_leps.at(0).second)*(1-eff_leps.at(1).second)*(1-eff_leps.at(2).second);
  double weight = 1.0;
  if ( fabs(denominator) > 0.0) weight = numerator/denominator;
  MSG_VERBOSE("Event trigger weight: " << weight);

  return weight;
}

std::pair<double,double> PSL::xAODWrapper::getMuonTriggerWeight(int icontainer){

  //get run number and avoid warnings
#ifdef BEFORE_ANALYSISBASE_2p3p45
  int tmp_rdmRN = m_prwTool->getRandomRunNumber( *eventInfo ); 
#else
  int tmp_rdmRN = m_prwTool->getRandomRunNumber( *eventInfo , true ); 
#endif
  int rdmRN = (tmp_rdmRN == 0) ? 267639 : tmp_rdmRN; //this is the internal run of the tool if zero
  m_muonTriggerSFTool->setRunNumber(rdmRN).ignore();
  m_muonTriggerSFToolNOSYST->setRunNumber(rdmRN).ignore();

  const std::string triggers = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50"; // make this configurable/argument later

  //make container of individual muons for SF
  const xAOD::Muon *muon = getMuon(icontainer);
  xAOD::MuonContainer *muonCont = new xAOD::MuonContainer;
  xAOD::MuonAuxContainer *muonContAux = new xAOD::MuonAuxContainer;
  muonCont -> setStore(muonContAux);
  xAOD::Muon* muon_tmp = new xAOD::Muon();
  muonCont->push_back(muon_tmp);
  muon_tmp->setP4(muon->pt(),muon->eta(),muon->phi());
  
  std::pair<double,double> weights; // first = SF; second = MC
  double sf_weight = 1.; double mc_eff = 1.;

  //SFs can vary with systematics
  m_muonTriggerSFTool->getTriggerScaleFactor(*muonCont,sf_weight,triggers).ignore();

  //MC efficiency shouldn't vary with systematics
  m_muonTriggerSFToolNOSYST->getTriggerEfficiency(*muon,
              mc_eff,
              triggers,
              false).ignore();
  /*
  //double mc_eff_syst = 1.; double data_eff_syst = 1.;
  //we actually don't need the data efficiency
  m_muonTriggerSFToolNOSYST->getTriggerEfficiency(*muon,
              data_eff,
              triggers,
              true);

  //just to cross check
  m_muonTriggerSFTool->getTriggerEfficiency(*muon,
              mc_eff_syst,
              triggers,
              false);

  //just to cross check
  m_muonTriggerSFTool->getTriggerEfficiency(*muon,
              data_eff_syst,
              triggers,
              true);

  MSG_ERROR("Individual muon SF " << sf_weight << " MC eff no syst " << mc_eff << " data eff no syst " << data_eff << " mc eff varied " << mc_eff_syst << " data eff varied " << data_eff_syst );
  */

  // Need these or else you have a memory leak
  if (muonContAux) delete muonContAux;
  if (muonCont) delete muonCont;

  weights.first = sf_weight;
  weights.second = mc_eff;
  return weights;
}

std::pair<double,double> PSL::xAODWrapper::getElectronTriggerWeight(int icontainer){
  std::pair<double,double> weights; //first SF, second MC eff
  double data_weight = 1.; double mc_weight = 1.;
  const xAOD::Electron *ele = getElectron(icontainer);
  // if w electron then sf
  int windex_container = m_evtdef->leps.at(m_evtdef->Wlep_index).i_cont; // container index of w lepton
  if(icontainer == windex_container){
    m_elesfs[ElectronSF_Trigger_TightLLH_isolGradient]->getEfficiencyScaleFactor(*ele,data_weight).ignore();
    m_elesfs[ElectronSF_TriggerMC_TightLLH_isolGradient]->getEfficiencyScaleFactor(*ele,mc_weight).ignore();
  }
  // else if z electron then sf
  else{
    m_elesfs[ElectronSF_Trigger_MediumLLH_isolGradientLoose]->getEfficiencyScaleFactor(*ele,data_weight).ignore();
    m_elesfs[ElectronSF_TriggerMC_MediumLLH_isolGradientLoose]->getEfficiencyScaleFactor(*ele,mc_weight).ignore();
  }
  weights.first = data_weight;
  weights.second = mc_weight;
  return weights;
}

#endif // SKIP_XAOD_COMPILATION
