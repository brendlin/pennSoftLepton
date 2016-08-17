#include "pennSoftLepton/FakeFactorTool.h"
#include "pennSoftLepton/UtilCore.h"

#include <fstream>

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::FakeFactorTool)

//-----------------------------------------------------------------------------
PSL::FakeFactorTool::FakeFactorTool() : ObjectSelectionBase()
  ,SavedFakeFactorFile    ("")
  ,MtCut                  (-1)
  ,MzCutLow               (-1)
  ,MzCutHigh              (-1)
  ,Region                 ("dijet")
  ,do_dijet               (true)
  ,do_zjet                (false)
  ,do_ttt                 (false)
  ,do_ltt                 (false)
  ,do_tlt                 (false)
  ,do_ttl                 (false)
  ,do_SR                  (false)
  ,do_wjet                (false)
  ,do_ttbar               (false)
  ,do_wplus               (false)
  ,do_wminus              (false)
//   ,do_zee                 (true)
//   ,do_zmm                 (true)
  ,m_ZWindow              (20.)
  ,m_mcclosure            (false)
  ,m_includeZdesert       (false)
  ,m_nvariation           (0)
  ,m_saved_muFakeFactor   (0)
  ,m_saved_eleFakeFactor  (0)
  ,ParentEventSelector    (0)
{
  MSG_DEBUG("Constructor");
}

//-----------------------------------------------------------------------------
void PSL::FakeFactorTool::loop(void){
  //MSG_INFO("loop start for " << GetName());
  if (ParentEventSelector){
    // Hacky thing to make sure the algorithms are talking about the same event.
    if (!ParentEventSelector->passed(m_EDM->get_EventNumber())) return;
  }

  if     (do_dijet) loopDijetRegion();
  else if (do_zjet || do_ttbar) loopZjetRegion();
  else if (do_wjet) loopWjetRegion();
  //MSG_INFO("loop end for " << GetName());
}

//-----------------------------------------------------------------------------
void PSL::FakeFactorTool::loopTtbarRegion(void){
  m_currenteventnumber = m_EDM->get_EventNumber();
  m_eventpassed = 0; // reset "event passed."
  return;
}

//-----------------------------------------------------------------------------
void PSL::FakeFactorTool::loopZjetRegion(void){
  m_EDM->m_evtdef = 0;
  m_EDM->ResetSample();
  m_currenteventnumber = m_EDM->get_EventNumber();  
  m_eventpassed = 0; // reset "event passed."
  //double weight = m_EDM->GetEventWeightNoPupNoSF();
  double weight = m_EDM->GetEventWeight();
  // Multiply WZ by 15% as the nominal
  if (MatchSample(kwz,m_EDM->m_CurrentSample)) weight *= 1.15;

  m_eventpassed_EleNum = 0;
  m_eventpassed_EleDen = 0;
  m_eventpassed_MuNum  = 0;
  m_eventpassed_MuDen  = 0;

  if (!(
      (m_EDM->PassTrigger(tHLT_e24_lhmedium_L1EM20VH) && m_EDM->IsData()) || 
      (m_EDM->PassTrigger(tHLT_e24_lhmedium_L1EM18VH) && m_EDM->IsMC()) ||
      m_EDM->PassTrigger(tHLT_e60_lhmedium         ) ||
      m_EDM->PassTrigger(tHLT_e120_lhloose         ) ||
      m_EDM->PassTrigger(tHLT_mu20_iloose_L1MU15   ) ||
      m_EDM->PassTrigger(tHLT_mu50                 )
        )) return;
  
//   unsigned int nlep = m_EDM->GetEventVariable(vnSignalLeptons);
//   int nlep_forveto_den = nlep;
  
  if (m_EDM->GetEventVariable(vnBaselineLeptons) >= 4) return;
  
  bool use_ff = false;
  // zjet: If data, then do data-driven
  if (do_zjet && do_SR) {
    if (!m_mcclosure) {
      if (m_EDM->m_CurrentSample == kdata && !SavedFakeFactorFile.empty()) {
        m_EDM->m_CurrentSample = kzjetdd;
        use_ff = true;
      }
      if (MatchSample(kwz,m_EDM->m_CurrentSample) && !SavedFakeFactorFile.empty()) use_ff = true;
      if (MatchSample(kzz,m_EDM->m_CurrentSample) && !SavedFakeFactorFile.empty()) use_ff = true;
      if (MatchSample(kttbar,m_EDM->m_CurrentSample) && !SavedFakeFactorFile.empty()) use_ff = true;
      if (MatchSample(ktw,m_EDM->m_CurrentSample) && !SavedFakeFactorFile.empty()) use_ff = true;
      if (MatchSample(kqqww,m_EDM->m_CurrentSample) && !SavedFakeFactorFile.empty()) use_ff = true;
    }
    // zjet: If MC and doing closure, then send to data-driven
    else if (m_mcclosure){
      // only run zjet mcclosure on zjet and ttbar
      if (!MatchSample(kzjet,m_EDM->m_CurrentSample) && !MatchSample(kttbar,m_EDM->m_CurrentSample) && !MatchSample(kzgam,m_EDM->m_CurrentSample)) return;
      if (!SavedFakeFactorFile.empty()) use_ff = true;
      if (MatchSample(kzjet,m_EDM->m_CurrentSample) && !SavedFakeFactorFile.empty()) {
        m_EDM->m_CurrentSample = kzjetdd;
      }
      if (MatchSample(kzgam,m_EDM->m_CurrentSample) && !SavedFakeFactorFile.empty()) {
        m_EDM->m_CurrentSample = kzjetdd;
      }
      if (MatchSample(kttbar,m_EDM->m_CurrentSample) && !SavedFakeFactorFile.empty()) {
        m_EDM->m_CurrentSample = kttdblct;
      }
      if (MatchSample(ktw,m_EDM->m_CurrentSample) && !SavedFakeFactorFile.empty()) {
        m_EDM->m_CurrentSample = kttdblct;
      }
    }
  }

  // ttbar: If data, then do data-driven
  if (do_ttbar && !do_SR) {
    if (!m_mcclosure) {
      // instead of data we use MC
      if (m_EDM->m_CurrentSample == kttbar && !SavedFakeFactorFile.empty()) {
        m_EDM->m_CurrentSample = kttfix;
        use_ff = true;
      }
    }
    // ttbar: If MC and doing closure, then send to data-driven
    else if (m_mcclosure) {
      // only run zjet mcclosure on zjet and ttbar
      if (!MatchSample(kttbar,m_EDM->m_CurrentSample)) return;
      if (MatchSample(kttbar,m_EDM->m_CurrentSample) && !SavedFakeFactorFile.empty()) {
        m_EDM->m_CurrentSample = kttfix;
        use_ff = true;
      }
    }
  }

  if (do_zjet && !do_SR) {
    if (m_EDM->GetEventVariable(vmet_Et) > 40) return; // THIS CUT IS ALSO LATER IN THE CODE!!
  }

  //
  // Z-lead, Z-sublead, W version
  // Find the signal leptons closest to the Z-peak
  // These indices correspond to EventDefinition indices!
  int SFOS_lep1index = -1;
  int SFOS_lep2index = -1;
  int index_wcand = -1;
  int index_fake = -1;
  int index_numobj = -1;

  unsigned int nlep = m_EDM->GetEventVariable(vnSignalLeptons);
  std::vector<bool> lep_pass_WSel(nlep,false);
  std::vector<bool> lep_pass_ZSel(nlep,false);
  std::vector<bool> lep_pass_Antiid(nlep,false);
  std::vector<bool> lep_remove(nlep,false);
  int n_total = 0;
  int n_wsignal = 0;
  int n_antiid = 0;
  int n_zsignal = 0;

  for (int i=(int)nlep-1;i>=0;--i){
    bool object_of_interest = false;
    bool passesOR = (m_EDM->LepVariableExists(lepPassOR) ? m_EDM->GetSignalLeptonVariable(lepPassOR,i) : true);
    if (!passesOR && do_ttt) {
      lep_remove[i] = true;
    }
    if (passesOR && SignalLeptonPassesAdditionalSignalCuts(i)) {
      lep_pass_WSel[i] = true;
      object_of_interest = true;
      n_wsignal++;
    }
    if (passesOR && SignalLeptonPassesAdditionalZBosonCuts(i)) {
      lep_pass_ZSel[i] = true;
      object_of_interest = true;
      n_zsignal++;
    }
    if (SignalLeptonPassesAntiIDCuts(i)) {
      lep_pass_Antiid[i] = true;
      object_of_interest = true;
      n_antiid++;
    }
    if (object_of_interest) n_total++;
    if (!object_of_interest) lep_remove[i] = true;
  }
  // currently we assume that you want to apply a 4th lepton veto - to match
  // what you would do for the signal region.

  if (n_total != 3) return;
  if (n_wsignal == 0 && n_antiid == 0) return;
  if (n_zsignal < 2) return;

  m_EDM->CopyEventDefinitionFromEDM(m_evtdef);
  for (int i=(int)nlep-1;i>=0; --i){
    if (!lep_remove[i]) continue;
    m_evtdef.RemoveSignalLepton(i);
    lep_pass_WSel  .erase(lep_pass_WSel  .begin()+i);
    lep_pass_ZSel  .erase(lep_pass_ZSel  .begin()+i);
    lep_pass_Antiid.erase(lep_pass_Antiid.begin()+i);
    nlep--;
  }
  if (nlep < 3) return;
  m_evtdef.BuildEvent();
  m_EDM->SetEventDefinitionPointer(&m_evtdef);

  //MSG_INFO("#######################");

  //
  // First sort the indices of the three leptons.
  //

  int SFOS_alt1index = -1;
  int SFOS_alt2index = -1;

  double min_delta = 99999999999;
  double min_delta2 = 99999999999;
  //double min_delta = 999999.;
  double tmp_mass = -999999.;
  for (unsigned int i=0;i<nlep;++i){
    for (unsigned int j=0;j<nlep;++j){
      if (i == j) continue;
      int k = (i+j == 1) ? 2 : ((i+j == 2) ? 1 : 0);
      //MSG_INFO("i: " << i << " j: " << j << " k: " << k);

      // Enforce leading/subleading Z leptons
      if (m_EDM->GetSignalLeptonVariable(lepPt,i) < m_EDM->GetSignalLeptonVariable(lepPt,j)) continue;

      // Require SFOS Z-lepton pair (i, j)
      if (do_zjet || (do_ttbar && do_SR)) {
        if (m_EDM->GetSignalLeptonVariable(lepCharge,i) == m_EDM->GetSignalLeptonVariable(lepCharge,j)) continue;
        if (m_EDM->GetSignalLeptonVariable(lepFlavor,i) != m_EDM->GetSignalLeptonVariable(lepFlavor,j)) continue;
      }

      // Or, require DFOS pair, excluding any SFOS pair
      if (do_ttbar && !do_SR) {
        // different charge
        if (m_EDM->GetSignalLeptonVariable(lepCharge,i) == m_EDM->GetSignalLeptonVariable(lepCharge,j)) continue;
        // different flavor
        if (m_EDM->GetSignalLeptonVariable(lepFlavor,i) == m_EDM->GetSignalLeptonVariable(lepFlavor,j)) continue;
        if (m_EDM->GetSignalLeptonVariable(lepFlavor,k) == m_EDM->GetSignalLeptonVariable(lepFlavor,i) &&
            m_EDM->GetSignalLeptonVariable(lepCharge,k) != m_EDM->GetSignalLeptonVariable(lepCharge,i)) continue;
        if (m_EDM->GetSignalLeptonVariable(lepFlavor,k) == m_EDM->GetSignalLeptonVariable(lepFlavor,j) &&
            m_EDM->GetSignalLeptonVariable(lepCharge,k) != m_EDM->GetSignalLeptonVariable(lepCharge,j)) continue;
      }

      tmp_mass = (m_EDM->GetSignalLeptonTLV(i)+m_EDM->GetSignalLeptonTLV(j)).M()/1000.;
      //MSG_INFO(tmp_mass);
      if (fabs(tmp_mass - 91.188) < min_delta) {
        min_delta2 = min_delta;
        min_delta = fabs(tmp_mass - 91.188);
        SFOS_alt1index = SFOS_lep1index;
        SFOS_alt2index = SFOS_lep2index;
        SFOS_lep1index = i;
        SFOS_lep2index = j;
        index_wcand = k;
      }
      else if (fabs(tmp_mass - 91.188) < min_delta2) {
        min_delta2 = fabs(tmp_mass-91.188);
        SFOS_alt1index = i;
        SFOS_alt2index = j;
      }
    }
  }   

  if (min_delta > m_ZWindow/2.) return;
  //if (do_zjet && min_delta < )
  if (SFOS_lep1index < 0) return;
  if (SFOS_lep2index < 0) return;
  if (index_wcand < 0) return;
  // reject event if a lepton is not above 25 GeV
  if (m_evtdef.LeadingLepton().Pt() < 25000.) return;
  if (m_EDM->GetSignalLeptonVariable(lepPt,vlep0_index) < 25) return;

  // ttbar: exclude ANY sfos pair. This part might be redundant.
  if (do_ttbar && !do_SR) {
    if (m_EDM->GetSignalLeptonVariable(lepFlavor,SFOS_lep1index) == m_EDM->GetSignalLeptonVariable(lepFlavor,index_wcand) &&
        m_EDM->GetSignalLeptonVariable(lepCharge,SFOS_lep1index) != m_EDM->GetSignalLeptonVariable(lepCharge,index_wcand)) return;
    if (m_EDM->GetSignalLeptonVariable(lepFlavor,SFOS_lep2index) == m_EDM->GetSignalLeptonVariable(lepFlavor,index_wcand) &&
        m_EDM->GetSignalLeptonVariable(lepCharge,SFOS_lep2index) != m_EDM->GetSignalLeptonVariable(lepCharge,index_wcand)) return;
  }

  LeptonChannel::LeptonChannel chan = static_cast<PSL::LeptonChannel::LeptonChannel>(m_EDM->GetEventVariable(vChanFlavor));

  // truth information.
  LeptonTruthLabel truth_leadz    = static_cast<PSL::LeptonTruthLabel>(m_EDM->GetSignalLeptonVariable(lepTruthDetailed,SFOS_lep1index));
  LeptonTruthLabel truth_subleadz = static_cast<PSL::LeptonTruthLabel>(m_EDM->GetSignalLeptonVariable(lepTruthDetailed,SFOS_lep2index));
  LeptonTruthLabel truth_w        = static_cast<PSL::LeptonTruthLabel>(m_EDM->GetSignalLeptonVariable(lepTruthDetailed,index_wcand   ));
  int index_realfake = -1;
  int type_fake_fortopestimate = -1;
  if (IsRealLepton(truth_leadz) && IsRealLepton(truth_subleadz) && IsRealLepton(truth_w)) {
    //MSG_INFO("Error! three real leptons.");
    index_realfake = index_wcand;
  }
  else if ((int)IsRealLepton(truth_leadz) + (int)IsRealLepton(truth_subleadz) + (int)IsRealLepton(truth_w) == 1) {
    //MSG_INFO("Error! one real lepton.");
    index_realfake = index_wcand;
  }
  else if ((int)IsRealLepton(truth_leadz) + (int)IsRealLepton(truth_subleadz) + (int)IsRealLepton(truth_w) == 0) {
    //MSG_INFO("Error! zero real leptons.");
    index_realfake = index_wcand;
  }
  else {
    //MSG_INFO("ok!");
    if (!IsRealLepton(truth_leadz   )) {
      //MSG_INFO("Lead z is fake");
      if (std::string(GetName()) == "FFTool_forsyst_wid_ttt") return; // HACK for systematics test
      index_realfake = SFOS_lep1index;
      type_fake_fortopestimate = m_EDM->GetSignalLeptonVariable(lepFlavor,SFOS_lep1index);
    }
    if (!IsRealLepton(truth_subleadz)) {
      //MSG_INFO("sublead z is fake");
      if (std::string(GetName()) == "FFTool_forsyst_wid_ttt") return; // HACK for systematics test
      index_realfake = SFOS_lep2index;
      type_fake_fortopestimate = m_EDM->GetSignalLeptonVariable(lepFlavor,SFOS_lep2index);
    }
    if (!IsRealLepton(truth_w       )) {
      //MSG_INFO("W is fake");
      // WRONG //if (std::string(GetName()) == "FFTool_forsyst_zid_ttt") return; // HACK for systematics test
      index_realfake = index_wcand;
      type_fake_fortopestimate = m_EDM->GetSignalLeptonVariable(lepFlavor,index_wcand);
    }
  }

  // Additional trigger requirement for the systematic tests. For ZID this does not need a hack.
  // for "SR fake factor," must require one of the reals to fire the trigger
  if (do_ttt && 
      (std::string(GetName()) == "FFTool_forsyst_wid_ttt" || 
       std::string(GetName()) == "FFTool_forsyst_zid_ttt")) {
    int index_reala = 1;
    int index_realb = 2;
    if (index_realfake == 1) index_reala = 0;
    if (index_realfake == 2) index_realb = 0;
    if (m_EDM->LepVariableExists(lepMatchesTrigger) &&
        !m_EDM->GetSignalLeptonVariable(lepMatchesTrigger,index_reala) &&
        !m_EDM->GetSignalLeptonVariable(lepMatchesTrigger,index_realb)) return;
  }

  // ttbar truth assignment.
  if ((MatchSample(kttbar,m_EDM->m_CurrentSample) || MatchSample(ktw, m_EDM->m_CurrentSample) || MatchSample(kqqww, m_EDM->m_CurrentSample))
      && std::string(GetName()) == "FFTool_ttt") {
    if (type_fake_fortopestimate < 0) {
      if (chan == LeptonChannel::eee) type_fake_fortopestimate = 1;
      else if (chan == LeptonChannel::mmm) type_fake_fortopestimate = 1;
      else {
        MSG_INFO("Error! Ambiguity in which lepton is fake! Taking the flavor of the lowest-pt lepton. " << 
                 m_EDM->GetSignalLeptonVariable(lepPt,SFOS_lep1index) << " " <<
                 m_EDM->GetSignalLeptonVariable(lepPt,SFOS_lep2index) << " " <<
                 m_EDM->GetSignalLeptonVariable(lepPt,index_wcand) << "    " << 
                 m_EDM->GetSignalLeptonVariable(lepTruthDetailed,SFOS_lep1index) << " " <<
                 m_EDM->GetSignalLeptonVariable(lepTruthDetailed,SFOS_lep2index) << " " <<
                 m_EDM->GetSignalLeptonVariable(lepTruthDetailed,index_wcand) << "    " << 
                 m_EDM->GetSignalLeptonVariable(lepFlavor,SFOS_lep1index) << " " <<
                 m_EDM->GetSignalLeptonVariable(lepFlavor,SFOS_lep2index) << " " <<
                 m_EDM->GetSignalLeptonVariable(lepFlavor,index_wcand) << "    " <<
                 m_EDM->GetSignalLeptonVariable(lepCharge,SFOS_lep1index) << " " <<
                 m_EDM->GetSignalLeptonVariable(lepCharge,SFOS_lep2index) << " " <<
                 m_EDM->GetSignalLeptonVariable(lepCharge,index_wcand) << "    "  << 
                 " weight " << weight << "  " <<
                 " lowest pt flavor: " << m_EDM->GetSignalLeptonVariable(lepFlavor,vlep2_index)
                 );
        type_fake_fortopestimate = m_EDM->GetSignalLeptonVariable(lepFlavor,vlep2_index);
      }
    }
    if (type_fake_fortopestimate == 1) weight *= 0.81;
    if (type_fake_fortopestimate == 2) weight *= 1.53;
    // HACK
    //if (type_fake_fortopestimate != 2) return; // run mu-fake
    //if (type_fake_fortopestimate != 1) return; // run e-fake
    // comment both to run all fakes
  }

  //
  // Now assess the quality of the leptons, only after they have been assigned an index.
  //
  // first check if it's a signal event
  if (do_ttt) {
    // trigger requirements
    if ((do_SR && do_zjet) || (!do_SR && do_ttbar)) { // SR, or ttcrptSR. Doesn't hurt to have in syst.
      if (m_EDM->LepVariableExists(lepMatchesTrigger) &&
          !m_EDM->GetSignalLeptonVariable(lepMatchesTrigger,index_wcand) &&
          !m_EDM->GetSignalLeptonVariable(lepMatchesTrigger,SFOS_lep1index) &&
          !m_EDM->GetSignalLeptonVariable(lepMatchesTrigger,SFOS_lep2index)) return;
    }
    if (!do_SR && do_zjet) { // Z+Jet FF Region
      if (m_EDM->LepVariableExists(lepMatchesTrigger) &&
          // SKIP WCAND FOR FF!!!
          !m_EDM->GetSignalLeptonVariable(lepMatchesTrigger,SFOS_lep1index) &&
          !m_EDM->GetSignalLeptonVariable(lepMatchesTrigger,SFOS_lep2index)) return;
    }
    if (!lep_pass_ZSel[SFOS_lep1index]) return;
    if (!lep_pass_ZSel[SFOS_lep2index]) return;
    if (!lep_pass_WSel[index_wcand]) return;
    //if  (m_EDM->GetSignalLeptonVariable(lepPt,index_wcand) < 30.) return;
    index_numobj = index_wcand;
  }
  else if (do_tlt) {
    if (m_EDM->LepVariableExists(lepMatchesTrigger) &&
        !m_EDM->GetSignalLeptonVariable(lepMatchesTrigger,index_wcand) &&
        !m_EDM->GetSignalLeptonVariable(lepMatchesTrigger,SFOS_lep2index)) return;
    if (!lep_pass_Antiid[SFOS_lep1index]) return;
    if (!lep_pass_ZSel[SFOS_lep2index]) return;
    if (!lep_pass_WSel[index_wcand]) return;
    // Below: require an additional SFOS pair to be consistent with a Z boson in some cases
    if (do_zjet 
        && m_EDM->GetSignalLeptonVariable(lepFlavor,SFOS_lep1index) != m_EDM->GetSignalLeptonVariable(lepFlavor,index_wcand)
        && !m_includeZdesert ) return;
    index_fake = SFOS_lep1index;
    if (m_EDM->GetSignalLeptonVariable(lepPt,index_fake) < 15.) return; // HACKED
  } 
  else if (do_ttl) {
    if (m_EDM->LepVariableExists(lepMatchesTrigger) &&
        !m_EDM->GetSignalLeptonVariable(lepMatchesTrigger,index_wcand) &&
        !m_EDM->GetSignalLeptonVariable(lepMatchesTrigger,SFOS_lep1index)) return;
    if (!lep_pass_ZSel[SFOS_lep1index]) return;
    if (!lep_pass_Antiid[SFOS_lep2index]) return;
    if (!lep_pass_WSel[index_wcand]) return;
    // Below: require an additional SFOS pair to be consistent with a Z boson in some cases
    if (do_zjet 
        && m_EDM->GetSignalLeptonVariable(lepFlavor,SFOS_lep1index) != m_EDM->GetSignalLeptonVariable(lepFlavor,index_wcand)
        && !m_includeZdesert ) return;
    index_fake = SFOS_lep2index;
    if (m_EDM->GetSignalLeptonVariable(lepPt,index_fake) < 15.) return; // HACKED
  } 
  else if (do_ltt) {
    if (m_EDM->LepVariableExists(lepMatchesTrigger) &&
        !m_EDM->GetSignalLeptonVariable(lepMatchesTrigger,SFOS_lep1index) &&
        !m_EDM->GetSignalLeptonVariable(lepMatchesTrigger,SFOS_lep2index)) return;
    if (!lep_pass_ZSel[SFOS_lep1index]) return;
    if (!lep_pass_ZSel[SFOS_lep2index]) return;
    if (!lep_pass_Antiid[index_wcand]) return;
    index_fake = index_wcand;
    // bit of a hack
    if (do_SR && m_EDM->GetSignalLeptonVariable(lepPt,index_fake) < 20. 
        && std::string(GetName()) != "FFTool_forsyst_ltt") return; // HACKED // should only affect zid case. wid has a cut in the conf file.
  }
  else return;

  if (do_wplus  && m_EDM->GetSignalLeptonVariable(lepCharge,index_wcand) == -1) return;
  if (do_wminus && m_EDM->GetSignalLeptonVariable(lepCharge,index_wcand) ==  1) return;

  // reweight ttbar in data-driven FF
  if ((MatchSample(kttbar,m_EDM->m_CurrentSample) || MatchSample(ktw, m_EDM->m_CurrentSample) || MatchSample(kqqww, m_EDM->m_CurrentSample)) &&
      (std::string(GetName()).find("FFTool_z_data") != std::string::npos || std::string(GetName()).find("FFTool_z_est_tt") != std::string::npos)){
    if (m_EDM->GetSignalLeptonVariable(lepFlavor,index_fake) == (int)ObjType::Muon    ) weight *= 1.03;
    if (m_EDM->GetSignalLeptonVariable(lepFlavor,index_fake) == (int)ObjType::Electron) weight *= 1.05;
  }

  //MSG_INFO("here. lep1 " << SFOS_lep1index << " lep2 " << SFOS_lep2index << " wcand " << index_wcand);
  if (SFOS_lep1index < 0) return;
  m_evtdef.leadZlep_index = SFOS_lep1index;
  m_evtdef.subleadZlep_index = SFOS_lep2index;
  m_evtdef.leadlep_Zalt_index = SFOS_alt1index;
  m_evtdef.subleadlep_Zalt_index = SFOS_alt2index;
  m_evtdef.Wlep_index = index_wcand;
  //if (index_wcand != index_realfake) return; // HACK!
  m_evtdef.antilep0_index = index_fake;
  if (do_ttt) m_evtdef.antilep0_index = index_realfake;

  // scale factors
  int tmp_index = m_evtdef.leadZlep_index;
  // Leading Z lepton SF
  if (do_ttt || do_ltt || do_ttl) {
    if (m_evtdef.SignalLeptonIsElectron(tmp_index)) weight *= m_EDM->GetSignalEleSF_RecoIDIso(tmp_index,zele_id,true,zele_isowp);
    else weight *= m_EDM->GetSignalMuonSF_IDIsoTTVA(tmp_index,zmu_id,zmu_isowp);
  }
  // Subleading Z lepton
  if (do_ttt || do_ltt || do_tlt) {
    tmp_index = m_evtdef.subleadZlep_index;
    if (m_evtdef.SignalLeptonIsElectron(tmp_index)) weight *= m_EDM->GetSignalEleSF_RecoIDIso(tmp_index,zele_id,true,zele_isowp);
    else weight *= m_EDM->GetSignalMuonSF_IDIsoTTVA(tmp_index,zmu_id,zmu_isowp);
  }
  // W lepton SF
  if (do_ttt || do_tlt || do_ttl) {  
    tmp_index = m_evtdef.Wlep_index;
    if (m_evtdef.SignalLeptonIsElectron(tmp_index)) weight *= m_EDM->GetSignalEleSF_RecoIDIso(tmp_index,ele_id,true,ele_isowp);
    else weight *= m_EDM->GetSignalMuonSF_IDIsoTTVA(tmp_index,mu_id,mu_isowp);
  }

  // double-check
  double mll_now = m_EDM->GetEventVariable(vMll);
  if (fabs(mll_now-91.188) > m_ZWindow/2.) {
    MSG_INFO("Some strange error! Mass was apparently: " << tmp_mass 
             << " but mll is " << mll_now << " and nleps is " << m_evtdef.leps.size()
             << " indices " << m_evtdef.leadZlep_index << " " << m_evtdef.subleadZlep_index
             << " indices " << SFOS_lep1index << " " << SFOS_lep2index
             << " wlep index " << index_wcand
             );
    exit(1);
  }

  // Cuts on CR
  //
  if (do_zjet && do_SR) {
    if (m_EDM->GetEventVariable(vMTW) < 30) return;
  } else if (do_zjet) {
    // old cuts - more signal contamination
    if (m_EDM->GetEventVariable(vmet_Et) > 40) return; // THIS CUT IS ALSO EARLIER IN THE CODE!!
    if (m_EDM->GetEventVariable(vMTW) > 30) return;
    // New, cleaner cuts 
//     if (m_EDM->GetEventVariable(vMTW) > 25) return;
//     if (m_EDM->GetEventVariable(vmet_Et)+1.2*m_EDM->GetEventVariable(vMTW) > 60) return;
//     if (m_EDM->GetEventVariable(vDeltaPhi3LMET) < 1.40) return;
    //
  } else if (do_ttbar) {
    if (m_EDM->GetEventVariable(vMTW) < 30) return;
  }

  
  //MSG_DEBUG(GetName());
  //m_EDM->PrintAllVariablesForDebugging();
  
  if (index_fake >= 0) {
    double pt = m_EDM->GetSignalLeptonVariable(lepPt,index_fake);
    if (m_EDM->GetSignalLeptonVariable(lepFlavor,index_fake) == (int)ObjType::Electron){
      MSG_DEBUG("Filling antiid ele with weight " << weight << " with sample type " << m_EDM->m_CurrentSample);
      m_eventpassed_EleDen = 1;
      if (!m_mcclosure) {
        FillStuff(weight,m_VariablesBySample_EleDen,m_LepVariablesBySample_EleDen);
        FillStuff(weight,m_VariablesBySample_allDen,m_LepVariablesBySample_allDen);
        if (chan == LeptonChannel::eee) FillStuff(weight,m_VariablesBySample_eeeDen,m_LepVariablesBySample_eeeDen);
        if (chan == LeptonChannel::emm) FillStuff(weight,m_VariablesBySample_euuDen,m_LepVariablesBySample_euuDen);
        if (chan == LeptonChannel::eem) FillStuff(weight,m_VariablesBySample_ueeDen,m_LepVariablesBySample_ueeDen);
      }
      if (use_ff){
        MSG_DEBUG("here!");
        double ff          = m_saved_eleFakeFactor->GetBinContent(m_saved_eleFakeFactor->FindBin(fmin(pt,99)));
        FillStuff(weight*ff,m_VariablesBySample_EleNum,m_LepVariablesBySample_EleNum);
        FillStuff(weight*ff,m_VariablesBySample_allNum,m_LepVariablesBySample_allNum);
        if (chan == LeptonChannel::eee) FillStuff(weight*ff,m_VariablesBySample_eeeNum,m_LepVariablesBySample_eeeNum);
        if (chan == LeptonChannel::emm) FillStuff(weight*ff,m_VariablesBySample_euuNum,m_LepVariablesBySample_euuNum);
        if (chan == LeptonChannel::eem) FillStuff(weight*ff,m_VariablesBySample_ueeNum,m_LepVariablesBySample_ueeNum);
      }
    }
    else {
      MSG_DEBUG("Filling antiid muon with weight " << weight);
      m_eventpassed_MuDen = 1;
      if (!m_mcclosure) {
        FillStuff(weight,m_VariablesBySample_MuDen ,m_LepVariablesBySample_MuDen );
        FillStuff(weight,m_VariablesBySample_allDen,m_LepVariablesBySample_allDen);
        if (chan == LeptonChannel::mmm) FillStuff(weight,m_VariablesBySample_uuuDen,m_LepVariablesBySample_uuuDen);
        if (chan == LeptonChannel::emm) FillStuff(weight,m_VariablesBySample_euuDen,m_LepVariablesBySample_euuDen);
        if (chan == LeptonChannel::eem) FillStuff(weight,m_VariablesBySample_ueeDen,m_LepVariablesBySample_ueeDen);
      }
      if (use_ff){
        double ff          = m_saved_muFakeFactor->GetBinContent(m_saved_muFakeFactor->FindBin(fmin(pt,99)));
        MSG_DEBUG("Fake factor is " << ff);
        FillStuff(weight*ff,m_VariablesBySample_MuNum ,m_LepVariablesBySample_MuNum );
        FillStuff(weight*ff,m_VariablesBySample_allNum,m_LepVariablesBySample_allNum);
        if (chan == LeptonChannel::mmm) FillStuff(weight*ff,m_VariablesBySample_uuuNum,m_LepVariablesBySample_uuuNum);
        if (chan == LeptonChannel::emm) FillStuff(weight*ff,m_VariablesBySample_euuNum,m_LepVariablesBySample_euuNum);
        if (chan == LeptonChannel::eem) FillStuff(weight*ff,m_VariablesBySample_ueeNum,m_LepVariablesBySample_ueeNum);
      }
    }
  }
  
  // Numerators are blind for data.
  //if (do_SR && m_EDM->m_CurrentSample == kdata) return;


  if (index_numobj >= 0 && !use_ff) {
    if (m_EDM->GetSignalLeptonVariable(lepFlavor,index_numobj) == (int)ObjType::Electron){
      MSG_DEBUG("Filling id ele with weight " << weight);
      m_eventpassed_EleNum = 1;
                                      FillStuff(weight,m_VariablesBySample_EleNum,m_LepVariablesBySample_EleNum);
                                      FillStuff(weight,m_VariablesBySample_allNum,m_LepVariablesBySample_allNum);
      if (chan == LeptonChannel::eee) FillStuff(weight,m_VariablesBySample_eeeNum,m_LepVariablesBySample_eeeNum);
      if (chan == LeptonChannel::emm) FillStuff(weight,m_VariablesBySample_euuNum,m_LepVariablesBySample_euuNum);
      if (chan == LeptonChannel::eem) FillStuff(weight,m_VariablesBySample_ueeNum,m_LepVariablesBySample_ueeNum);
    }
    else { //muon
      MSG_DEBUG("Filling id muon with weight " << weight);
      m_eventpassed_MuNum = 1;
                                      FillStuff(weight,m_VariablesBySample_MuNum ,m_LepVariablesBySample_MuNum );
                                      FillStuff(weight,m_VariablesBySample_allNum,m_LepVariablesBySample_allNum);
      if (chan == LeptonChannel::mmm) FillStuff(weight,m_VariablesBySample_uuuNum,m_LepVariablesBySample_uuuNum);
      if (chan == LeptonChannel::emm) FillStuff(weight,m_VariablesBySample_euuNum,m_LepVariablesBySample_euuNum);
      if (chan == LeptonChannel::eem) FillStuff(weight,m_VariablesBySample_ueeNum,m_LepVariablesBySample_ueeNum);
    }
  }

  m_eventpassed = 1;
  return;
}

//-----------------------------------------------------------------------------
void PSL::FakeFactorTool::loopWjetRegion(void){
  m_EDM->m_evtdef = 0;
  m_EDM->ResetSample();
  m_currenteventnumber = m_EDM->get_EventNumber();
  m_eventpassed = 0; // reset "event passed."
  double weight = m_EDM->GetEventWeight();
  // Multiply WZ by 15% as the nominal
  if (MatchSample(kwz,m_EDM->m_CurrentSample)) weight *= 1.15;

  if (!(
      (m_EDM->PassTrigger(tHLT_e24_lhmedium_L1EM20VH) && m_EDM->IsData()) || 
      (m_EDM->PassTrigger(tHLT_e24_lhmedium_L1EM18VH) && m_EDM->IsMC()) ||
      m_EDM->PassTrigger(tHLT_e60_lhmedium         ) ||
      m_EDM->PassTrigger(tHLT_e120_lhloose         ) ||
      m_EDM->PassTrigger(tHLT_mu20_iloose_L1MU15   ) ||
      m_EDM->PassTrigger(tHLT_mu50                 )
        )) return;
  
  if (m_EDM->GetEventVariable(vnBaselineLeptons) >= 3) return;

  unsigned int nlep = m_EDM->GetEventVariable(vnSignalLeptons);
  std::vector<bool> lep_pass_Sel(nlep,false);
  std::vector<bool> lep_pass_Antiid(nlep,false);
  std::vector<bool> lep_remove(nlep,false);
  int n_total = 0;
  int n_signal = 0;
  int n_antiid = 0;

  for (int i=(int)nlep-1;i>=0;--i){
    bool object_of_interest = false;
    bool passesOR = (m_EDM->LepVariableExists(lepPassOR) ? m_EDM->GetSignalLeptonVariable(lepPassOR,i) : true);
    if (passesOR && SignalLeptonPassesAdditionalSignalCuts(i)) {
      lep_pass_Sel[i] = true;
      object_of_interest = true;
      n_signal++;
    }
    if (SignalLeptonPassesAntiIDCuts(i)) {
      lep_pass_Antiid[i] = true;
      object_of_interest = true;
      n_antiid++;
    }
    if (object_of_interest) n_total++;
    if (!object_of_interest) lep_remove[i] = true;
  }
  // currently we assume that you want to apply a 3rd lepton veto - to match
  // what you would do for the signal region.

  if (n_total != 3) return;
  if (n_signal == 0 && n_antiid == 0) return;


  // We do not rebuild the event!

  return;
}

//-----------------------------------------------------------------------------
void PSL::FakeFactorTool::loopDijetRegion(void){
  m_currenteventnumber = m_EDM->get_EventNumber();
  m_eventpassed = 0; // reset "event passed."
  // Do the nominal selection
  //MSG_INFO("Number of signal leptons: " << m_EDM->GetEventVariable(vnSignalLeptons));

  bool apply_prescale = (m_EDM->m_CurrentSample == kdata) ? true : false;
  if (m_EDM->m_CurrentSample == kdata && !SavedFakeFactorFile.empty()) 
    m_EDM->m_CurrentSample = kdijetdd;

  // 
  if (m_EDM->m_CurrentSample == kdijet) {
    if (vNJets < 2) return;
    if (m_EDM->GetEventVariable(vptAvgOverTruthjet0Pt) > 1.4) return;
    if (m_EDM->GetEventVariable(vptAvgOverTruthjet0Pt) < 0.6) return;
  }

  int n_passing_leptons = 0;

  unsigned int nlep = m_EDM->GetEventVariable(vnSignalLeptons);
  //unsigned int nlep = m_EDM->GetEventVariable(vnSignalElectrons)+m_EDM->GetEventVariable(vnSignalMuons);

  for (unsigned int i=0;i<nlep; ++i){
    double pt = m_EDM->GetSignalLeptonVariable(lepPt,i);
    MSG_DEBUG(pt << ", " << m_EDM->GetSignalLeptonVariable(lepFlavor,i));
    if (pt < 9) continue;
    if (m_EDM->GetSignalLeptonVariable(lepMt,i) > MtCut) continue;

    double mll_medium_pair = m_EDM->GetSignalLeptonVariable(lepMll,i);
    if (MzCutLow < mll_medium_pair && mll_medium_pair < MzCutHigh) continue;

    MSG_DEBUG("here!");
    n_passing_leptons++;

    // see if it passes the trigger (just the bit first)
    bool pass_trigger = false;
    double weight = m_EDM->GetEventWeight();
    double prescale = 1;

    if (m_EDM->GetSignalLeptonVariable(lepFlavor,i) == (int)ObjType::Electron){
      if     (m_EDM->PassTrigger(tHLT_e5_lhvloose          ) &&   9 < pt && pt <  12) {pass_trigger = true; prescale = m_prescales[tHLT_e5_lhvloose          ];}
      else if(m_EDM->PassTrigger(tHLT_e10_lhvloose_L1EM7   ) &&  12 < pt && pt <  17) {pass_trigger = true; prescale = m_prescales[tHLT_e10_lhvloose_L1EM7   ];}
      else if(m_EDM->PassTrigger(tHLT_e15_lhvloose_L1EM7   ) &&  17 < pt && pt <  28) {pass_trigger = true; prescale = m_prescales[tHLT_e15_lhvloose_L1EM7   ];}
      else if(m_EDM->PassTrigger(tHLT_e24_lhvloose_L1EM18VH) &&  28 < pt && pt <  55) {pass_trigger = true; prescale = m_prescales[tHLT_e24_lhvloose_L1EM18VH];}
      else if(m_EDM->PassTrigger(tHLT_e50_lhvloose_L1EM15  ) &&  55 < pt && pt <  75) {pass_trigger = true; prescale = m_prescales[tHLT_e50_lhvloose_L1EM15  ];}
      else if(m_EDM->PassTrigger(tHLT_e70_lhvloose         ) &&  75 < pt && pt < 105) {pass_trigger = true; prescale = m_prescales[tHLT_e70_lhvloose         ];}
      else if(m_EDM->PassTrigger(tHLT_e100_lhvloose        ) && 105 < pt            ) {pass_trigger = true; prescale = m_prescales[tHLT_e100_lhvloose        ];}

      MSG_DEBUG("ele pass trigger: " << pass_trigger);

      if (apply_prescale) weight *= prescale;

      if (pass_trigger) {
        if (IsAntiIDElectron(i)) {
          MSG_DEBUG("Filling antiid ele with weight " << weight << " with sample type " << m_EDM->m_CurrentSample);
          FillStuff(weight,m_VariablesBySample_EleDen,m_LepVariablesBySample_EleDen);
          if (!SavedFakeFactorFile.empty()){
            MSG_DEBUG("here!");
            double ff = m_saved_eleFakeFactor->GetBinContent(m_saved_eleFakeFactor->FindBin(fmin(pt,99)));
            FillStuff(weight*ff,m_VariablesBySample_EleNum,m_LepVariablesBySample_EleNum);
          }
        }
        if (IsSignalElectronLocal(i) && SavedFakeFactorFile.empty()) {
          MSG_DEBUG("Filling id ele with weight " << weight);
          FillStuff(weight,m_VariablesBySample_EleNum,m_LepVariablesBySample_EleNum);
        }
      }
    }
    else { // muon
      if     (m_EDM->PassTrigger(tHLT_mu4 ) &&  9 < pt && pt < 14) {pass_trigger = true; prescale = m_prescales[tHLT_mu4 ];}
      else if(m_EDM->PassTrigger(tHLT_mu14) && 14 < pt && pt < 18) {pass_trigger = true; prescale = m_prescales[tHLT_mu14];}
      else if(m_EDM->PassTrigger(tHLT_mu18) && 18 < pt && pt < 50) {pass_trigger = true; prescale = m_prescales[tHLT_mu18];}
      else if(m_EDM->PassTrigger(tHLT_mu50) && 50 < pt           ) {pass_trigger = true; prescale = m_prescales[tHLT_mu50];}

      if (apply_prescale) weight *= prescale;

      MSG_DEBUG("muon pass trigger: " << pass_trigger);
      
      if (pass_trigger) {
        if (IsAntiIDMuon(i)) {
          MSG_DEBUG("Filling antiid muon with weight " << weight);
          FillStuff(weight,m_VariablesBySample_MuDen,m_LepVariablesBySample_MuDen);
          if (!SavedFakeFactorFile.empty()){
            MSG_DEBUG("here!");
            double ff = m_saved_muFakeFactor->GetBinContent(m_saved_muFakeFactor->FindBin(fmin(pt,99)));
            FillStuff(weight*ff,m_VariablesBySample_MuNum,m_LepVariablesBySample_MuNum);
          }
        }
        if (IsSignalMuonLocal(i) && SavedFakeFactorFile.empty()) {
          MSG_DEBUG("Filling id muon with weight " << weight);
          FillStuff(weight,m_VariablesBySample_MuNum,m_LepVariablesBySample_MuNum);
        }
      }
    }
  }
  MSG_DEBUG("made it through");

  m_eventpassed = 1;
  return;
}

//-----------------------------------------------------------------------------
void PSL::FakeFactorTool::FillStuff(double weight,std::vector<std::vector<TH1*> >& h_all_variables
                                    ,std::vector<std::vector<std::vector<TH1*> > >& h_all_lepvariables){
  for (unsigned int j=0;j<Samples.size();++j){
    MSG_DEBUG("Seeing if " << ConvertSampleToStr(Samples[j]) << " should be filled. Note that current sample is " << ConvertSampleToStr(m_EDM->m_CurrentSample));
    if (!MatchSample(Samples[j],m_EDM->m_CurrentSample)) continue; 
    MSG_DEBUG("Indeed, " << ConvertSampleToStr(Samples[j]) << " will be filled");
    for (unsigned int k=0;k<m_variables.size();++k){
      if (!m_EDM->VariableExists(m_variables[k])) continue;
      MSG_DEBUG(ConvertVarToStr(m_variables[k]) << " " << ConvertSampleToStr(Samples[j]));
      h_all_variables[j][k]->Fill(m_EDM->GetEventVariable(m_variables[k]),weight);
      MSG_DEBUG(ConvertVarToStr(m_variables[k]) << " " << ConvertSampleToStr(Samples[j]) << "Filled.");
    }
    for (unsigned int k=0;k<m_lepvariables.size();++k){
      if (!m_EDM->LepVariableExists(m_lepvariables[k])) continue;
      for (unsigned int l=0;l<m_lepindices.size();++l){
        MSG_DEBUG(ConvertLepVarToStr(m_lepvariables[k]) << " " << ConvertSampleToStr(Samples[j]));
        if (m_EDM->GetEventVariable(m_lepindices[l]) < 0) continue;
        h_all_lepvariables[j][k][l]->Fill(m_EDM->GetSignalLeptonVariable(m_lepvariables[k],m_lepindices[l]),weight);
      }
    }
  }
  return;
}

//-----------------------------------------------------------------------------
bool PSL::FakeFactorTool::init(void){

  if (!ObjectSelectionBase::init()) return false;
  
  if (!SavedFakeFactorFile.empty()){
    TFile* f_tmp = TFile::Open((std::string(getenv("ROOTCOREBIN"))+"/../"+SavedFakeFactorFile).c_str(),"READ");
    if (!f_tmp) return false;
    m_saved_muFakeFactor = (TH1F*)((TH1F*)f_tmp->Get("Muons"))->Clone();
    m_saved_muFakeFactor->SetDirectory(0);
    m_saved_eleFakeFactor = (TH1F*)((TH1F*)f_tmp->Get("Electrons"))->Clone();
    m_saved_eleFakeFactor->SetDirectory(0);
    f_tmp->Close();
    
    // statistical variations.
    MSG_DEBUG("nvariation is " << m_nvariation);
    if (m_nvariation > 0) {
      int bin = -1;
      if (m_nvariation < 5) { // 1, 2, 3, 4
        if (m_nvariation == 1) bin = m_saved_eleFakeFactor->FindBin(15.1);
        if (m_nvariation == 2) bin = m_saved_eleFakeFactor->FindBin(20.1);
        if (m_nvariation == 3) bin = m_saved_eleFakeFactor->FindBin(30.1);
        if (m_nvariation == 4) bin = m_saved_eleFakeFactor->FindBin(50.1);
        //
        m_saved_eleFakeFactor->SetBinContent(bin,
                                             m_saved_eleFakeFactor->GetBinContent(bin)+
                                             m_saved_eleFakeFactor->GetBinError(bin));
        for (int i=0;i<m_saved_eleFakeFactor->GetNbinsX();++i){
          MSG_DEBUG(m_saved_eleFakeFactor->GetBinContent(i+1));
        }
      } else if (5 <= m_nvariation && m_nvariation < 8) { // 5, 6, 7
        if (m_nvariation == 5) bin = m_saved_muFakeFactor->FindBin(15.1);
        if (m_nvariation == 6) bin = m_saved_muFakeFactor->FindBin(20.1);
        if (m_nvariation == 7) bin = m_saved_muFakeFactor->FindBin(30.1);
        m_saved_muFakeFactor->SetBinContent(bin,
                                            m_saved_muFakeFactor->GetBinContent(bin)+
                                            m_saved_muFakeFactor->GetBinError(bin));
        for (int i=0;i<m_saved_muFakeFactor->GetNbinsX();++i){
          MSG_DEBUG(m_saved_muFakeFactor->GetBinContent(i+1));
        }
      } else if (m_nvariation == 8) {
        // wz syst
        TFile* f_tmp = TFile::Open((std::string(getenv("ROOTCOREBIN"))+"/../"+SavedFakeFactorFile).c_str(),"READ");
        if (!f_tmp) return false;
        m_saved_muFakeFactor = (TH1F*)((TH1F*)f_tmp->Get("Muons_wzsyst"))->Clone();
        m_saved_muFakeFactor->SetDirectory(0);
        m_saved_eleFakeFactor = (TH1F*)((TH1F*)f_tmp->Get("Electrons_wzsyst"))->Clone();
        m_saved_eleFakeFactor->SetDirectory(0);
        f_tmp->Close();
      } else if (m_nvariation == 9) {
        // e composition
        if (do_ltt) { // WID Systematic
          int bin; double bc;
          bin = m_saved_eleFakeFactor->FindBin(20.1); bc = m_saved_eleFakeFactor->GetBinContent(bin); m_saved_eleFakeFactor->SetBinContent(bin,m_saved_eleFakeFactor->GetBinContent(bin),bc*1.30);
          bin = m_saved_eleFakeFactor->FindBin(30.1); bc = m_saved_eleFakeFactor->GetBinContent(bin); m_saved_eleFakeFactor->SetBinContent(bin,m_saved_eleFakeFactor->GetBinContent(bin),bc*1.30);
          bin = m_saved_eleFakeFactor->FindBin(50.1); bc = m_saved_eleFakeFactor->GetBinContent(bin); m_saved_eleFakeFactor->SetBinContent(bin,m_saved_eleFakeFactor->GetBinContent(bin),bc*1.60/*1.80*/);
        }
        if (do_tlt || do_ttl) { // 
          int bin; double bc;
          bin = m_saved_eleFakeFactor->FindBin(15.1); bc = m_saved_eleFakeFactor->GetBinContent(bin); m_saved_eleFakeFactor->SetBinContent(bin,m_saved_eleFakeFactor->GetBinContent(bin),bc*1.60);
          bin = m_saved_eleFakeFactor->FindBin(20.1); bc = m_saved_eleFakeFactor->GetBinContent(bin); m_saved_eleFakeFactor->SetBinContent(bin,m_saved_eleFakeFactor->GetBinContent(bin),bc*1.30/*1.35*/);
          bin = m_saved_eleFakeFactor->FindBin(30.1); bc = m_saved_eleFakeFactor->GetBinContent(bin); m_saved_eleFakeFactor->SetBinContent(bin,m_saved_eleFakeFactor->GetBinContent(bin),bc*1.35);
          bin = m_saved_eleFakeFactor->FindBin(50.1); bc = m_saved_eleFakeFactor->GetBinContent(bin); m_saved_eleFakeFactor->SetBinContent(bin,m_saved_eleFakeFactor->GetBinContent(bin),bc*1.55/*1.50*/);
        }
      } else if (m_nvariation == 10) {
        // mu composition
        if (do_ltt) { // WID Systematic
          int bin; double bc;
          bin = m_saved_muFakeFactor->FindBin(20.1); bc = m_saved_muFakeFactor->GetBinContent(bin); m_saved_muFakeFactor->SetBinContent(bin,m_saved_muFakeFactor->GetBinContent(bin),bc*1.20);
          bin = m_saved_muFakeFactor->FindBin(30.1); bc = m_saved_muFakeFactor->GetBinContent(bin); m_saved_muFakeFactor->SetBinContent(bin,m_saved_muFakeFactor->GetBinContent(bin),bc*1.30/*1.25*/);
        }
        if (do_tlt || do_ttl) { // 
          int bin; double bc;
          bin = m_saved_muFakeFactor->FindBin(15.1); bc = m_saved_muFakeFactor->GetBinContent(bin); m_saved_muFakeFactor->SetBinContent(bin,m_saved_muFakeFactor->GetBinContent(bin),bc*1.30);
          bin = m_saved_muFakeFactor->FindBin(20.1); bc = m_saved_muFakeFactor->GetBinContent(bin); m_saved_muFakeFactor->SetBinContent(bin,m_saved_muFakeFactor->GetBinContent(bin),bc*1.20);
          bin = m_saved_muFakeFactor->FindBin(30.1); bc = m_saved_muFakeFactor->GetBinContent(bin); m_saved_muFakeFactor->SetBinContent(bin,m_saved_muFakeFactor->GetBinContent(bin),bc*1.20);
        }
      }
    }
  }
  
  // Unless otherwise specified, save all of the variables from Variables.h
  if (!m_variables.size()) m_variables = GetAllVariables();
  
  m_VariablesBySample_allNum.clear();
  m_VariablesBySample_allDen.clear();
  MakeVariableTH1FVector(m_VariablesBySample_allNum ,"PassEvent",std::string(GetName())+"_allNum" ,Samples,m_variables);
  MakeVariableTH1FVector(m_VariablesBySample_allDen ,"PassEvent",std::string(GetName())+"_allDen" ,Samples,m_variables);

  if (do_zjet || do_ttbar) {
    m_VariablesBySample_MuNum.clear();
    m_VariablesBySample_MuDen.clear();
    m_VariablesBySample_EleNum.clear();
    m_VariablesBySample_EleDen.clear();
    MakeVariableTH1FVector(m_VariablesBySample_MuNum ,"PassEvent",std::string(GetName())+"_MuNum" ,Samples,m_variables);
    MakeVariableTH1FVector(m_VariablesBySample_MuDen ,"PassEvent",std::string(GetName())+"_MuDen" ,Samples,m_variables);
    MakeVariableTH1FVector(m_VariablesBySample_EleNum,"PassEvent",std::string(GetName())+"_EleNum",Samples,m_variables);
    MakeVariableTH1FVector(m_VariablesBySample_EleDen,"PassEvent",std::string(GetName())+"_EleDen",Samples,m_variables);
    
    m_VariablesBySample_eeeNum.clear();
    m_VariablesBySample_eeeDen.clear();
    MakeVariableTH1FVector(m_VariablesBySample_eeeNum ,"PassEvent",std::string(GetName())+"_eeeNum" ,Samples,m_variables);
    MakeVariableTH1FVector(m_VariablesBySample_eeeDen ,"PassEvent",std::string(GetName())+"_eeeDen" ,Samples,m_variables);
    
    m_VariablesBySample_euuNum.clear();
    m_VariablesBySample_euuDen.clear();
    MakeVariableTH1FVector(m_VariablesBySample_euuNum ,"PassEvent",std::string(GetName())+"_euuNum" ,Samples,m_variables);
    MakeVariableTH1FVector(m_VariablesBySample_euuDen ,"PassEvent",std::string(GetName())+"_euuDen" ,Samples,m_variables);
    
    m_VariablesBySample_ueeNum.clear();
    m_VariablesBySample_ueeDen.clear();
    MakeVariableTH1FVector(m_VariablesBySample_ueeNum ,"PassEvent",std::string(GetName())+"_ueeNum" ,Samples,m_variables);
    MakeVariableTH1FVector(m_VariablesBySample_ueeDen ,"PassEvent",std::string(GetName())+"_ueeDen" ,Samples,m_variables);
    
    m_VariablesBySample_uuuNum.clear();
    m_VariablesBySample_uuuDen.clear();
    MakeVariableTH1FVector(m_VariablesBySample_uuuNum ,"PassEvent",std::string(GetName())+"_uuuNum" ,Samples,m_variables);
    MakeVariableTH1FVector(m_VariablesBySample_uuuDen ,"PassEvent",std::string(GetName())+"_uuuDen" ,Samples,m_variables);
  }
  if (do_wjet) {
    m_VariablesBySample_uuNum.clear();
    m_VariablesBySample_uuDen.clear();
    MakeVariableTH1FVector(m_VariablesBySample_uuNum ,"PassEvent",std::string(GetName())+"_uuNum" ,Samples,m_variables);
    MakeVariableTH1FVector(m_VariablesBySample_uuDen ,"PassEvent",std::string(GetName())+"_uuDen" ,Samples,m_variables);

    m_VariablesBySample_ueNum.clear();
    m_VariablesBySample_ueDen.clear();
    MakeVariableTH1FVector(m_VariablesBySample_ueNum ,"PassEvent",std::string(GetName())+"_ueNum" ,Samples,m_variables);
    MakeVariableTH1FVector(m_VariablesBySample_ueDen ,"PassEvent",std::string(GetName())+"_ueDen" ,Samples,m_variables);

    m_VariablesBySample_euNum.clear();
    m_VariablesBySample_euDen.clear();
    MakeVariableTH1FVector(m_VariablesBySample_euNum ,"PassEvent",std::string(GetName())+"_euNum" ,Samples,m_variables);
    MakeVariableTH1FVector(m_VariablesBySample_euDen ,"PassEvent",std::string(GetName())+"_euDen" ,Samples,m_variables);

    m_VariablesBySample_ueuoNum.clear();
    m_VariablesBySample_ueuoDen.clear();
    MakeVariableTH1FVector(m_VariablesBySample_ueuoNum ,"PassEvent",std::string(GetName())+"_ueuoNum" ,Samples,m_variables);
    MakeVariableTH1FVector(m_VariablesBySample_ueuoDen ,"PassEvent",std::string(GetName())+"_ueuoDen" ,Samples,m_variables);
  }

  if (!m_lepindices.size()) {
    m_lepindices.push_back(vlep0_index             );
    m_lepindices.push_back(vlep1_index             );
    m_lepindices.push_back(vlep2_index             );
    m_lepindices.push_back(vlepleadZ_index         );
    m_lepindices.push_back(vlepsubleadZ_index      );
    m_lepindices.push_back(vlepW_index             );
    m_lepindices.push_back(vantilep0_index         );
  }

  if (!m_lepvariables.size()) {
    m_lepvariables.push_back(lepFlavor        );
    m_lepvariables.push_back(lepPt            );
    m_lepvariables.push_back(lepEta           );
    m_lepvariables.push_back(lepCharge        );
    m_lepvariables.push_back(lepD0Significance);
    m_lepvariables.push_back(lepZ0SinTheta    );
    m_lepvariables.push_back(lepTrkIso        );
    m_lepvariables.push_back(lepTruthDetailed );
    m_lepvariables.push_back(lepElectronID    );
    m_lepvariables.push_back(lepMuonID        );
    m_lepvariables.push_back(lepMatchesTrigger);
  }

  m_LepVariablesBySample_allNum.clear();
  m_LepVariablesBySample_allDen.clear();
  MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_allNum ,"PassEvent",std::string(GetName())+"_allNum" ,Samples,m_lepvariables,m_lepindices);
  MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_allDen ,"PassEvent",std::string(GetName())+"_allDen" ,Samples,m_lepvariables,m_lepindices);

  if (do_zjet || do_ttbar) {
    m_LepVariablesBySample_MuNum.clear();
    m_LepVariablesBySample_MuDen.clear();
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_MuNum ,"PassEvent",std::string(GetName())+"_MuNum" ,Samples,m_lepvariables,m_lepindices);
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_MuDen ,"PassEvent",std::string(GetName())+"_MuDen" ,Samples,m_lepvariables,m_lepindices);
    
    m_LepVariablesBySample_EleNum.clear();
    m_LepVariablesBySample_EleDen.clear();
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_EleNum,"PassEvent",std::string(GetName())+"_EleNum",Samples,m_lepvariables,m_lepindices);
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_EleDen,"PassEvent",std::string(GetName())+"_EleDen",Samples,m_lepvariables,m_lepindices);
    
    m_LepVariablesBySample_eeeNum.clear();
    m_LepVariablesBySample_eeeDen.clear();
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_eeeNum ,"PassEvent",std::string(GetName())+"_eeeNum" ,Samples,m_lepvariables,m_lepindices);
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_eeeDen ,"PassEvent",std::string(GetName())+"_eeeDen" ,Samples,m_lepvariables,m_lepindices);
    
    m_LepVariablesBySample_euuNum.clear();
    m_LepVariablesBySample_euuDen.clear();
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_euuNum ,"PassEvent",std::string(GetName())+"_euuNum" ,Samples,m_lepvariables,m_lepindices);
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_euuDen ,"PassEvent",std::string(GetName())+"_euuDen" ,Samples,m_lepvariables,m_lepindices);

    m_LepVariablesBySample_ueeNum.clear();
    m_LepVariablesBySample_ueeDen.clear();
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_ueeNum ,"PassEvent",std::string(GetName())+"_ueeNum" ,Samples,m_lepvariables,m_lepindices);
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_ueeDen ,"PassEvent",std::string(GetName())+"_ueeDen" ,Samples,m_lepvariables,m_lepindices);
    
    m_LepVariablesBySample_uuuNum.clear();
    m_LepVariablesBySample_uuuDen.clear();
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_uuuNum ,"PassEvent",std::string(GetName())+"_uuuNum" ,Samples,m_lepvariables,m_lepindices);
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_uuuDen ,"PassEvent",std::string(GetName())+"_uuuDen" ,Samples,m_lepvariables,m_lepindices);
  }
  if (do_wjet) {
    m_LepVariablesBySample_uuNum.clear();
    m_LepVariablesBySample_uuDen.clear();
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_uuNum ,"PassEvent",std::string(GetName())+"_uuNum" ,Samples,m_lepvariables,m_lepindices);
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_uuDen ,"PassEvent",std::string(GetName())+"_uuDen" ,Samples,m_lepvariables,m_lepindices);

    m_LepVariablesBySample_ueNum.clear();
    m_LepVariablesBySample_ueDen.clear();
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_ueNum ,"PassEvent",std::string(GetName())+"_ueNum" ,Samples,m_lepvariables,m_lepindices);
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_ueDen ,"PassEvent",std::string(GetName())+"_ueDen" ,Samples,m_lepvariables,m_lepindices);

    m_LepVariablesBySample_euNum.clear();
    m_LepVariablesBySample_euDen.clear();
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_euNum ,"PassEvent",std::string(GetName())+"_euNum" ,Samples,m_lepvariables,m_lepindices);
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_euDen ,"PassEvent",std::string(GetName())+"_euDen" ,Samples,m_lepvariables,m_lepindices);

    m_LepVariablesBySample_ueuoNum.clear();
    m_LepVariablesBySample_ueuoDen.clear();
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_ueuoNum ,"PassEvent",std::string(GetName())+"_ueuoNum" ,Samples,m_lepvariables,m_lepindices);
    MakeVectorOfLepVariableTH1FVectors(m_LepVariablesBySample_ueuoDen ,"PassEvent",std::string(GetName())+"_ueuoDen" ,Samples,m_lepvariables,m_lepindices);
  }

  if (MtCut                     <= 0) MtCut = FLT_MAX;
  if (MzCutLow > 0 && MzCutHigh <= 0) MzCutHigh = FLT_MAX;

  m_prescales.clear();
  std::string line;
  std::ifstream lumifile (std::string(getenv("ROOTCOREBIN"))+"/../"+PrescalesFile);
  if (lumifile.is_open()) {
    while ( getline (lumifile,line) ) {
      std::cout << line << '\n';
      if (line.find("#") != std::string::npos) continue;
      std::vector<std::string> tmp;
      PSL::StringTok(tmp,line," ");
      if (tmp.size() < 4) {
        MSG_INFO("File error. Exiting.");
        return false;
      }
      m_prescales[ConvertToTrigger(tmp[0])] = std::stod(tmp[3]);
    }
    lumifile.close();
  }
  else {
    MSG_INFO("Unable to open FakeFactorTool prescales file " << PrescalesFile << ". Exiting."); 
    return false;
  }
  for (auto it=m_prescales.begin();it != m_prescales.end();++it){
    MSG_INFO(ConvertTrigToStr(static_cast<PSL::Trigger2015>(it->first)) << ": " << it->second);
  }

  PrintConfiguration();
  return true;
}

//-----------------------------------------------------------------------------
void PSL::FakeFactorTool::finish(void){
  MSG_INFO(GetName() << " Finish");

  hist_PassN = new TH2F();
  hist_PassW = new TH2F();
  
  for (unsigned int i=0;i<Samples.size();++i){
    for (unsigned int j=0;j<m_variables.size();++j){
      SaveIfNotEmpty(m_VariablesBySample_MuNum[i][j],false);
      delete m_VariablesBySample_MuNum[i][j];
      SaveIfNotEmpty(m_VariablesBySample_MuDen[i][j],false);
      delete m_VariablesBySample_MuDen[i][j];
      SaveIfNotEmpty(m_VariablesBySample_EleNum[i][j],false);
      delete m_VariablesBySample_EleNum[i][j];
      SaveIfNotEmpty(m_VariablesBySample_EleDen[i][j],false);
      delete m_VariablesBySample_EleDen[i][j];

      SaveIfNotEmpty(m_VariablesBySample_allNum[i][j],false);
      delete m_VariablesBySample_allNum[i][j];
      SaveIfNotEmpty(m_VariablesBySample_allDen[i][j],false);
      delete m_VariablesBySample_allDen[i][j];

      SaveIfNotEmpty(m_VariablesBySample_eeeNum[i][j],false);
      delete m_VariablesBySample_eeeNum[i][j];
      SaveIfNotEmpty(m_VariablesBySample_eeeDen[i][j],false);
      delete m_VariablesBySample_eeeDen[i][j];

      SaveIfNotEmpty(m_VariablesBySample_euuNum[i][j],false);
      delete m_VariablesBySample_euuNum[i][j];
      SaveIfNotEmpty(m_VariablesBySample_euuDen[i][j],false);
      delete m_VariablesBySample_euuDen[i][j];

      SaveIfNotEmpty(m_VariablesBySample_ueeNum[i][j],false);
      delete m_VariablesBySample_ueeNum[i][j];
      SaveIfNotEmpty(m_VariablesBySample_ueeDen[i][j],false);
      delete m_VariablesBySample_ueeDen[i][j];

      SaveIfNotEmpty(m_VariablesBySample_uuuNum[i][j],false);
      delete m_VariablesBySample_uuuNum[i][j];
      SaveIfNotEmpty(m_VariablesBySample_uuuDen[i][j],false);
      delete m_VariablesBySample_uuuDen[i][j];
    }
    for (unsigned int j=0;j<m_lepvariables.size();++j){    
      for (unsigned int k=0;k<m_lepindices.size();++k){    
        SaveIfNotEmpty(m_LepVariablesBySample_MuNum[i][j][k],false);
        delete m_LepVariablesBySample_MuNum[i][j][k];
        SaveIfNotEmpty(m_LepVariablesBySample_MuDen[i][j][k],false);
        delete m_LepVariablesBySample_MuDen[i][j][k];
        SaveIfNotEmpty(m_LepVariablesBySample_EleNum[i][j][k],false);
        delete m_LepVariablesBySample_EleNum[i][j][k];
        SaveIfNotEmpty(m_LepVariablesBySample_EleDen[i][j][k],false);
        delete m_LepVariablesBySample_EleDen[i][j][k];

        SaveIfNotEmpty(m_LepVariablesBySample_allNum[i][j][k],false);
        delete m_LepVariablesBySample_allNum[i][j][k];
        SaveIfNotEmpty(m_LepVariablesBySample_allDen[i][j][k],false);
        delete m_LepVariablesBySample_allDen[i][j][k];
        
        SaveIfNotEmpty(m_LepVariablesBySample_eeeNum[i][j][k],false);
        delete m_LepVariablesBySample_eeeNum[i][j][k];
        SaveIfNotEmpty(m_LepVariablesBySample_eeeDen[i][j][k],false);
        delete m_LepVariablesBySample_eeeDen[i][j][k];
        
        SaveIfNotEmpty(m_LepVariablesBySample_euuNum[i][j][k],false);
        delete m_LepVariablesBySample_euuNum[i][j][k];
        SaveIfNotEmpty(m_LepVariablesBySample_euuDen[i][j][k],false);
        delete m_LepVariablesBySample_euuDen[i][j][k];
        
        SaveIfNotEmpty(m_LepVariablesBySample_ueeNum[i][j][k],false);
        delete m_LepVariablesBySample_ueeNum[i][j][k];
        SaveIfNotEmpty(m_LepVariablesBySample_ueeDen[i][j][k],false);
        delete m_LepVariablesBySample_ueeDen[i][j][k];
        
        SaveIfNotEmpty(m_LepVariablesBySample_uuuNum[i][j][k],false);
        delete m_LepVariablesBySample_uuuNum[i][j][k];
        SaveIfNotEmpty(m_LepVariablesBySample_uuuDen[i][j][k],false);
        delete m_LepVariablesBySample_uuuDen[i][j][k];
      }
    }
  }

  MSG_INFO(GetName() << " Finish done.");

}

//-----------------------------------------------------------------------------
void PSL::FakeFactorTool::PrintConfiguration(void){
  MSG_INFO("-------------------------------FakeFactorTool Configuration:---------------------------");
  MSG_INFO("This Instance Name      : " << GetName()              );
  MSG_INFO("ParentEventSelectorName : " << ParentEventSelectorName);
  MSG_INFO("SavedFakeFactorFile    : " << SavedFakeFactorFile    );
  MSG_INFO("MtCut                  : " << MtCut                  );
  MSG_INFO("MzCutLow               : " << MzCutLow               );
  MSG_INFO("MzCutHigh              : " << MzCutHigh              );
  MSG_INFO("ZWindow                : " << m_ZWindow              );
  MSG_INFO("");
//   MSG_INFO("ele_ptmin              : " << ele_ptmin              );
//   MSG_INFO("ele_id                 : " << ConvertElectronIDToStr(ele_id));
//   MSG_INFO("ele_d0_max             : " << ele_d0_max             );
//   MSG_INFO("ele_z0_max             : " << ele_z0_max             );
//   MSG_INFO("ele_isowp              : " << ConvertAnaIsoToStr(ele_isowp));
//   MSG_INFO("ele_trkisotype         : " << ConvertAnaIsoToStr(ele_trkisotype));
//   MSG_INFO("ele_caloisotype        : " << ConvertAnaIsoToStr(ele_caloisotype));
//   MSG_INFO("ele_caloiso_max        : " << ele_caloiso_max        );
//   MSG_INFO("ele_trkiso_max         : " << ele_trkiso_max         );
//   MSG_INFO("");
//   MSG_INFO("ele_ptmin_antiid       : " << ele_ptmin_antiid       );
//   MSG_INFO("ele_id_fail_antiid     : " << ConvertElectronIDToStr(ele_id_fail_antiid));
//   MSG_INFO("ele_id_pass_antiid     : " << ConvertElectronIDToStr(ele_id_pass_antiid));
//   MSG_INFO("ele_d0_max_antiid      : " << ele_d0_max_antiid      );
//   MSG_INFO("ele_d0_min_antiid      : " << ele_d0_min_antiid      );
//   MSG_INFO("ele_z0_max_antiid      : " << ele_z0_max_antiid      );
//   MSG_INFO("ele_z0_min_antiid      : " << ele_z0_min_antiid      );
//   MSG_INFO("ele_isowp_fail_antiid   : " << ConvertAnaIsoToStr(ele_isowp_fail_antiid));
//   MSG_INFO("ele_isowp_pass_antiid   : " << ConvertAnaIsoToStr(ele_isowp_pass_antiid));
//   MSG_INFO("ele_trkiso_max_antiid  : " << ele_trkiso_max_antiid  );
//   MSG_INFO("ele_trkiso_min_antiid  : " << ele_trkiso_min_antiid  );
//   MSG_INFO("ele_caloiso_max_antiid : " << ele_caloiso_max_antiid );
//   MSG_INFO("ele_caloiso_min_antiid : " << ele_caloiso_min_antiid );
//   MSG_INFO("");
//   MSG_INFO("mu_ptmin               : " << mu_ptmin               );
//   MSG_INFO("mu_id                  : " << ConvertMuonIDToStr(mu_id));
//   MSG_INFO("mu_d0_max              : " << mu_d0_max              );
//   MSG_INFO("mu_z0_max              : " << mu_z0_max              );
//   MSG_INFO("mu_isowp               : " << ConvertAnaIsoToStr(mu_isowp));
//   MSG_INFO("mu_trkisotype          : " << ConvertAnaIsoToStr(mu_trkisotype));
//   MSG_INFO("mu_caloisotype         : " << ConvertAnaIsoToStr(mu_caloisotype));
//   MSG_INFO("mu_trkiso_max          : " << mu_trkiso_max          );
//   MSG_INFO("mu_caloiso_max         : " << mu_caloiso_max         );
//   MSG_INFO("");
//   MSG_INFO("zmu_ptmin              : " << zmu_ptmin);
//   MSG_INFO("zmu_id                 : " << ConvertMuonIDToStr(zmu_id));
//   MSG_INFO("zmu_d0_max             : " << zmu_d0_max);
//   MSG_INFO("zmu_z0_max             : " << zmu_z0_max);
//   MSG_INFO("zmu_isowp              : " << ConvertAnaIsoToStr(zmu_isowp));
//   MSG_INFO("zmu_trkisotype         : " << ConvertAnaIsoToStr(zmu_trkisotype));
//   MSG_INFO("zmu_caloisotype        : " << ConvertAnaIsoToStr(zmu_caloisotype));
//   MSG_INFO("zmu_trkiso_max         : " << zmu_trkiso_max);
//   MSG_INFO("zmu_caloiso_max        : " << zmu_caloiso_max);
//   MSG_INFO("");
//   MSG_INFO("mu_ptmin_antiid        : " << mu_ptmin_antiid        );
//   MSG_INFO("mu_d0_max_antiid       : " << mu_d0_max_antiid       );
//   MSG_INFO("mu_d0_min_antiid       : " << mu_d0_min_antiid       );
//   MSG_INFO("mu_z0_max_antiid       : " << mu_z0_max_antiid       );
//   MSG_INFO("mu_z0_min_antiid       : " << mu_z0_min_antiid       );
//   MSG_INFO("mu_isowp_fail_antiid    : " << ConvertAnaIsoToStr(mu_isowp_fail_antiid));
//   MSG_INFO("mu_isowp_pass_antiid    : " << ConvertAnaIsoToStr(mu_isowp_pass_antiid));
//   MSG_INFO("mu_trkiso_max_antiid   : " << mu_trkiso_max_antiid   );
//   MSG_INFO("mu_trkiso_min_antiid   : " << mu_trkiso_min_antiid   );
//   MSG_INFO("mu_caloiso_max_antiid  : " << mu_caloiso_max_antiid  );
//   MSG_INFO("mu_caloiso_min_antiid  : " << mu_caloiso_min_antiid  );
//   MSG_INFO("");
  MSG_INFO("do_dijet               : " << (do_dijet ? "yes" : "no"));
  MSG_INFO("do_zjet                : " << (do_zjet ? "yes" : "no"));
  MSG_INFO("do_ttt                 : " << (do_ttt ? "yes" : "no"));
  MSG_INFO("do_ltt                 : " << (do_ltt ? "yes" : "no"));
  MSG_INFO("do_tlt                 : " << (do_tlt ? "yes" : "no"));
  MSG_INFO("do_ttl                 : " << (do_ttl ? "yes" : "no"));
  MSG_INFO("do_SR                  : " << (do_SR ? "yes" : "no"));
  MSG_INFO("do_wjet                : " << (do_wjet ? "yes" : "no"));
  MSG_INFO("do_ttbar               : " << (do_ttbar ? "yes" : "no"));
  MSG_INFO("do_ttbar               : " << (do_ttbar ? "yes" : "no"));
  MSG_INFO("wplus                  : " << (do_wplus ? "yes" : "no"));
  MSG_INFO("wminus                 : " << (do_wminus ? "yes" : "no"));
  MSG_INFO("mcclosure              : " << (m_mcclosure ? "yes" : "no"));
  MSG_INFO("nvariation             : " << m_nvariation           );
  MSG_INFO("includeZdesert         : " << (m_includeZdesert ? "yes" : "no"));
  std::string tmp;
  for (unsigned int i=0;i<Samples.size();++i){
    tmp += ConvertSampleToStr(Samples[i]);
    tmp += " ";
  }
  MSG_INFO("Samples                 : " << tmp                    );
  tmp = "";
  for (unsigned int i=0;i<m_variables.size();++i){
    tmp += ConvertVarToStr(m_variables[i]);
    tmp += " ";
  }
  MSG_INFO("Variables               : " << tmp                    );
  tmp = "";
  for (unsigned int i=0;i<m_lepvariables.size();++i){
    tmp += ConvertLepVarToStr(m_lepvariables[i]);
    tmp += " ";
  }
  MSG_INFO("LepVariables            : " << tmp                    );
  tmp = "";
  for (unsigned int i=0;i<m_lepindices.size();++i){
    tmp += ConvertVarToStr(m_lepindices[i]);
    tmp += " ";
  }
  MSG_INFO("LepIndices              : " << tmp                    );
  MSG_INFO("---------------------------------------------------------------------------------------");
}

//-----------------------------------------------------------------------------
void PSL::FakeFactorTool::ReconnectHistograms(TFile* file){
  (void)file;
  return;
}

//-----------------------------------------------------------------------------
double PSL::FakeFactorTool::TtbarMultiplier(void) {
  return 1;
  // For now let us assume that we are looking at lle channels together
//   double factor_ee = 1;
//   double factor_uu = 1;
//   double ttbar_global_etou_factor = 1;
//   double ttbar_global_utoe_factor = 1;

  // maybe later
  //if (IsMuon(m_evtdef.leps[m_evtdef.leadZlep_index])) factor_ee = factor*ttbar_global_utoe_factor;

//   if (do_zee) {
//     if (do_ltt) {
//       factor_ee = factor_ee*ttbar_global_utoe_factor;
//     } 
//     else if (do_tlt) {
//       if (IsElectron(m_evtdef.leps[m_evtdef.Wlep_index])) factor_ee = factor_ee*ttbar_global_utoe_factor;
//       else factor_ee = 0;
//     }
//     else if (do_ttl) { // same
//       if (IsElectron(m_evtdef.leps[m_evtdef.Wlep_index])) factor_ee = factor_ee*ttbar_global_utoe_factor;
//       else factor_ee = 0;
//     }
//   }
//   if (do_zmm) {
//     if (do_ltt) {
//       factor_uu = factor_uu*ttbar_global_etou_factor;
//     }
//     else if (do_tlt) {
//       if (IsMuon(m_evtdef.leps[m_evtdef.Wlep_index])) factor_uu = factor_uu*ttbar_global_etou_factor;
//       else factor_uu = 0;
//     }
//     else if (do_ttl) { // same
//       if (IsMuon(m_evtdef.leps[m_evtdef.Wlep_index])) factor_uu = factor_uu*ttbar_global_etou_factor;
//       else factor_uu = 0;
//     }
//   }
//   // window....
//   return (factor_ee+factor_uu)*(m_ZWindow/20.);
}

//-----------------------------------------------------------------------------
void PSL::FakeFactorTool::SetOutgoingNtupleBranches(TTree* tree){
  std::string branchname = "PassEvent_"+std::string(GetName())+"_EleNum";
  tree->Branch(branchname.c_str(),&m_eventpassed_EleNum,(branchname+"/I").c_str());
  branchname             = "PassEvent_"+std::string(GetName())+"_EleDen";
  tree->Branch(branchname.c_str(),&m_eventpassed_EleDen,(branchname+"/I").c_str());
  branchname             = "PassEvent_"+std::string(GetName())+"_MuNum";
  tree->Branch(branchname.c_str(),&m_eventpassed_MuNum,(branchname+"/I").c_str());
  branchname             = "PassEvent_"+std::string(GetName())+"_MuDen";
  tree->Branch(branchname.c_str(),&m_eventpassed_MuDen,(branchname+"/I").c_str());
  return;
}

//-----------------------------------------------------------------------------
void PSL::FakeFactorTool::set_LepIndices(std::string v){
  MSG_DEBUG("setting algorithm " << GetName() << " lep indices to " << v);
  std::vector<std::string> tmp;
  PSL::StringTok(tmp,v," ");
  m_lepindices.clear();
  for(unsigned int i=0;i<tmp.size();i++){
    m_lepindices.push_back(ConvertToVariable(tmp[i]));
  }
}

