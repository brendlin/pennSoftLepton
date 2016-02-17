#include <pennSoftLepton/ToolsD3PDEra.h>
#ifndef SKIP_D3PD_COMPILATION

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::ToolsD3PDEra)
  
PSL::ToolsD3PDEra::ToolsD3PDEra() : AlgBase()
  ,m_pileup(0)
  ,m_jvfTool(0)
// 				  ,myHist(0)
{
  MSG_DEBUG("Constructor");
  SetName("TOOLS_D3PD");
}

bool PSL::ToolsD3PDEra::init(void)
{
  m_pileup = new Root::TPileupReweighting("PileupReweighting");
  m_pileup->SetDataScaleFactors(1/1.11);
  m_pileup->AddConfigFile("$ROOTCOREBIN/data/PileupReweighting/mc12ab_defaults.prw.root");
  //m_pileup->AddLumiCalcFile("$ROOTCOREBIN/data/SusyCommon/ilumicalc_histograms_EF_2e12Tvh_loose1_200842-215643_grl_v61.root");
  m_pileup->AddLumiCalcFile("$ROOTCOREBIN/../pennSoftLepton/share/ilumicalc_histograms_EF_2e12Tvh_loose1_200842-215643_grl_v61.root");
  m_pileup->SetUnrepresentedDataAction(2);
  m_pileup->Initialize();

  m_jvfTool = new JVFUncertaintyTool("AntiKt4LCTopo");
  //m_jvfTool->UseGeV(true);
  MSG_INFO("Finished init");
  return true;
}

EL::StatusCode PSL::ToolsD3PDEra::changeInput(bool firstFile){

  MSG_INFO("Initialzing Trigger Navigation Variables");
  m_triggerNavigationVariables = new TriggerNavigationVariables();

  m_triggerNavigationVariables->set_trig_DB_SMK(m_EDM->GetPointer(D3PD::trig_DB_SMK));
  m_triggerNavigationVariables->set_trig_Nav_n(m_EDM->GetPointer(D3PD::trig_Nav_n));
  m_triggerNavigationVariables->set_trig_Nav_chain_ChainId(m_EDM->Get(D3PD::trig_Nav_chain_ChainId));
  m_triggerNavigationVariables->set_trig_Nav_chain_RoIType(m_EDM->Get(D3PD::trig_Nav_chain_RoIType));
  m_triggerNavigationVariables->set_trig_Nav_chain_RoIIndex(m_EDM->Get(D3PD::trig_Nav_chain_RoIIndex));

  //electron
  m_triggerNavigationVariables->set_trig_RoI_EF_e_egammaContainer_egamma_Electrons(m_EDM->Get(D3PD::trig_RoI_EF_e_egammaContainer_egamma_Electrons));
  m_triggerNavigationVariables->set_trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus(m_EDM->Get(D3PD::trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus));
  m_triggerNavigationVariables->set_trig_EF_el_n(m_EDM->GetPointer(D3PD::trig_EF_el_n));
  m_triggerNavigationVariables->set_trig_EF_el_Et(m_EDM->Get(D3PD::trig_EF_el_Et));
  m_triggerNavigationVariables->set_trig_EF_el_eta(m_EDM->Get(D3PD::trig_EF_el_eta));
  m_triggerNavigationVariables->set_trig_EF_el_phi(m_EDM->Get(D3PD::trig_EF_el_phi));

  // muon 
  m_triggerNavigationVariables->set_trig_RoI_EF_mu_Muon_ROI(m_EDM->Get(D3PD::trig_RoI_EF_mu_Muon_ROI));
  m_triggerNavigationVariables->set_trig_RoI_EF_mu_TrigMuonEFInfoContainer(m_EDM->Get(D3PD::trig_RoI_EF_mu_TrigMuonEFInfoContainer));
  m_triggerNavigationVariables->set_trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus(m_EDM->Get(D3PD::trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus));
  m_triggerNavigationVariables->set_trig_RoI_L2_mu_CombinedMuonFeature(m_EDM->Get(D3PD::trig_RoI_L2_mu_CombinedMuonFeature));
  m_triggerNavigationVariables->set_trig_RoI_L2_mu_CombinedMuonFeatureStatus(m_EDM->Get(D3PD::trig_RoI_L2_mu_CombinedMuonFeatureStatus));
  m_triggerNavigationVariables->set_trig_RoI_L2_mu_MuonFeature(m_EDM->Get(D3PD::trig_RoI_L2_mu_MuonFeature));
  m_triggerNavigationVariables->set_trig_RoI_L2_mu_Muon_ROI(m_EDM->Get(D3PD::trig_RoI_L2_mu_Muon_ROI));

  m_triggerNavigationVariables->set_trig_EF_trigmuonef_track_MuonType(m_EDM->Get(D3PD::trig_EF_trigmuonef_track_MuonType));
  m_triggerNavigationVariables->set_trig_EF_trigmuonef_track_CB_pt(m_EDM->Get(D3PD::trig_EF_trigmuonef_track_CB_pt));
  m_triggerNavigationVariables->set_trig_EF_trigmuonef_track_CB_eta(m_EDM->Get(D3PD::trig_EF_trigmuonef_track_CB_eta));
  m_triggerNavigationVariables->set_trig_EF_trigmuonef_track_CB_phi(m_EDM->Get(D3PD::trig_EF_trigmuonef_track_CB_phi));
  m_triggerNavigationVariables->set_trig_EF_trigmuonef_track_SA_pt(m_EDM->Get(D3PD::trig_EF_trigmuonef_track_SA_pt));
  m_triggerNavigationVariables->set_trig_EF_trigmuonef_track_SA_eta(m_EDM->Get(D3PD::trig_EF_trigmuonef_track_SA_eta));
  m_triggerNavigationVariables->set_trig_EF_trigmuonef_track_SA_phi(m_EDM->Get(D3PD::trig_EF_trigmuonef_track_SA_phi));

  m_triggerNavigationVariables->set_trig_EF_trigmugirl_track_CB_pt(m_EDM->Get(D3PD::trig_EF_trigmugirl_track_CB_pt));
  m_triggerNavigationVariables->set_trig_EF_trigmugirl_track_CB_eta(m_EDM->Get(D3PD::trig_EF_trigmugirl_track_CB_eta));
  m_triggerNavigationVariables->set_trig_EF_trigmugirl_track_CB_phi(m_EDM->Get(D3PD::trig_EF_trigmugirl_track_CB_phi));

  m_triggerNavigationVariables->set_trig_L2_combmuonfeature_eta(m_EDM->Get(D3PD::trig_L2_combmuonfeature_eta));
  m_triggerNavigationVariables->set_trig_L2_combmuonfeature_phi(m_EDM->Get(D3PD::trig_L2_combmuonfeature_phi));
  m_triggerNavigationVariables->set_trig_L2_muonfeature_eta(m_EDM->Get(D3PD::trig_L2_muonfeature_eta));
  m_triggerNavigationVariables->set_trig_L2_muonfeature_phi(m_EDM->Get(D3PD::trig_L2_muonfeature_phi));

  m_triggerNavigationVariables->set_trig_L1_mu_eta(m_EDM->Get(D3PD::trig_L1_mu_eta));
  m_triggerNavigationVariables->set_trig_L1_mu_phi(m_EDM->Get(D3PD::trig_L1_mu_phi));
  m_triggerNavigationVariables->set_trig_L1_mu_thrName(m_EDM->Get(D3PD::trig_L1_mu_thrName));

  m_triggerNavigationVariables->set_trig_RoI_EF_mu_TrigMuonEFIsolationContainer(m_EDM->Get(D3PD::trig_RoI_EF_mu_TrigMuonEFIsolationContainer)); // for 2012 isolated trigger
  m_triggerNavigationVariables->set_trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus(m_EDM->Get(D3PD::trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus)); // for 2012 isolated trigger

  if (!m_triggerNavigationVariables->isValid()) {
    MSG_INFO("Error! Trigger navigation variables not set correctly!");
    return EL::StatusCode::FAILURE;
  }
  m_muonTriggerMatchTool = new MuonTriggerMatching(m_triggerNavigationVariables);
  m_electronTriggerMatchTool = new ElectronTriggerMatching(m_triggerNavigationVariables);

  return EL::StatusCode::SUCCESS;
}

void PSL::ToolsD3PDEra::finish(void){
  MSG_DEBUG("finish.");
  if (m_pileup) delete m_pileup;
  return;
}

void PSL::ToolsD3PDEra::TriggerMatch(EventDefinition evtdef){
  long long int trig_match = 0;
  bool result1 = false;
  bool result2 = false;
  std::pair<bool,bool> r1; // results of the input muon 1, in scenario <1,2>
  std::pair<bool,bool> r2; // results of the input muon 2, in scenario <2,1> (I understand it's bizarre)
  for (unsigned int i=0;i<evtdef.leps.size();++i){
    if (evtdef.leps[i].obj_type == ObjType::Electron){
      // e24vhi_medium1
      if(m_electronTriggerMatchTool->match(evtdef.leps[i].Eta(),evtdef.leps[i].Phi(),"EF_e24vhi_medium1") && evtdef.leps[i].Pt() > 1000.*24.)
        trig_match = trig_match | 0x1 << Trigger2012::EF_e24vhi_medium1;
      // e60_medium1
      if(m_electronTriggerMatchTool->match(evtdef.leps[i].Eta(),evtdef.leps[i].Phi(),"EF_e60_medium1")    && evtdef.leps[i].Pt() > 1000.*60.)
        trig_match = trig_match | 0x1 << Trigger2012::EF_e60_medium1;
    }
    else if (evtdef.leps[i].obj_type == ObjType::Muon){
      // mu24i_tight
      if(m_muonTriggerMatchTool->match(evtdef.leps[i].Eta(),evtdef.leps[i].Phi(), "EF_mu24i_tight") && evtdef.leps[i].Pt() > 1000.*24.)
        trig_match = trig_match | 0x1 << Trigger2012::EF_mu24i_tight;
    }
    for (unsigned int j=0;j<evtdef.leps.size();++j){
      if (i == j) continue;
      // di-object triggers
      // The way the code works, it will check match(e1,e2) and then match(e2,e1) in different steps of the loop
      if (evtdef.leps[i].obj_type == ObjType::Electron && evtdef.leps[j].obj_type == ObjType::Electron) {
        // 2e12Tvh_loose1
        m_electronTriggerMatchTool->matchDielectron(evtdef.leps[i].tlv,evtdef.leps[j].tlv, "EF_2e12Tvh_loose1", result1, result2);
        if (result1 && result2) trig_match = trig_match | 0x1 << Trigger2012::EF_2e12Tvh_loose1;
        // e24vh_medium1_e7_medium1
        if (m_EDM->EF_e24vh_medium1_e7_medium1 && // CHECK
            matchesToEFTriggerEle(evtdef.leps[i].tlv,m_EDM->Get(D3PD::trig_EF_el_EF_e24vh_medium1)) &&
            matchesToEFTriggerEle(evtdef.leps[j].tlv,m_EDM->Get(D3PD::trig_EF_el_EF_e7T_medium1)))
          trig_match = trig_match | 0x1 << Trigger2012::EF_e24vh_medium1_e7_medium1;
        // broken // m_electronTriggerMatchTool->matchDielectron(evtdef.leps[i].tlv,evtdef.leps[j].tlv, "EF_e24vh_medium1_e7_medium1", result1, result2);
        // broken //if (result1 && result2) trig_match = trig_match | 0x1 << Trigger2012::EF_e24vh_medium1_e7_medium1;
      }
      else if (evtdef.leps[i].obj_type == ObjType::Electron && evtdef.leps[j].obj_type == ObjType::Muon) {
        // e12Tvh_medium1_mu8
        if (m_electronTriggerMatchTool->matchElectronMuon(evtdef.leps[i].tlv,evtdef.leps[j].tlv, "EF_e12Tvh_medium1_mu8"))
          trig_match = trig_match | 0x1 << Trigger2012::EF_e12Tvh_medium1_mu8;
        // mu18_tight_e7_medium1
        if (m_EDM->EF_mu18_tight_e7_medium1 &&
            matchesToEFTriggerEle(evtdef.leps[i].tlv,m_EDM->Get(D3PD::trig_EF_el_EF_e7T_medium1)) &&
            m_muonTriggerMatchTool->match(evtdef.leps[j].Eta(),evtdef.leps[j].Phi(), "EF_mu18_tight"))
          trig_match = trig_match | 0x1 << Trigger2012::EF_mu18_tight_e7_medium1;
        // broken // if (m_electronTriggerMatchTool->matchElectronMuon(evtdef.leps[i].tlv,evtdef.leps[j].tlv, "EF_mu18_tight_e7_medium1"))
        // broken //   trig_match = trig_match | 0x1 << Trigger2012::EF_mu18_tight_e7_medium1;
      }
      else if (evtdef.leps[i].obj_type == ObjType::Muon && evtdef.leps[j].obj_type == ObjType::Muon) {
        // mu18_tight_mu8_EFFS
        m_muonTriggerMatchTool->matchDimuon(evtdef.leps[i].tlv,evtdef.leps[j].tlv, "EF_mu18_tight_mu8_EFFS", r1,r2);
        if ((r1.first && r2.second) || (r2.first && r1.second)) trig_match = trig_match | 0x1 << Trigger2012::EF_mu18_tight_mu8_EFFS;
        // 2mu13
        m_muonTriggerMatchTool->matchDimuon(evtdef.leps[i].tlv,evtdef.leps[j].tlv, "EF_2mu13", r1,r2);
        if ((r1.first && r2.second) || (r2.first && r1.second)) trig_match = trig_match | 0x1 << Trigger2012::EF_2mu13;
      }
      for (unsigned int k=0;k<evtdef.leps.size();++k){
        if (i == k || j == k) continue;
        // tri-object triggers
        if (evtdef.leps[i].obj_type == ObjType::Electron && evtdef.leps[j].obj_type == ObjType::Electron && evtdef.leps[k].obj_type == ObjType::Electron) {
          // e18vh_medium1_2e7T_medium1
          if (m_EDM->EF_e18vh_medium1_2e7T_medium1 &&
              matchesToEFTriggerEle(evtdef.leps[i].tlv,m_EDM->Get(D3PD::trig_EF_el_EF_e18vh_medium1)) &&
              matchesToEFTriggerEle(evtdef.leps[j].tlv,m_EDM->Get(D3PD::trig_EF_el_EF_e7T_medium1)) &&
              matchesToEFTriggerEle(evtdef.leps[k].tlv,m_EDM->Get(D3PD::trig_EF_el_EF_e7T_medium1)))
            trig_match = trig_match | 0x1 << Trigger2012::EF_e18vh_medium1_2e7T_medium1;
        }
        if (evtdef.leps[i].obj_type == ObjType::Electron && evtdef.leps[j].obj_type == ObjType::Electron && evtdef.leps[k].obj_type == ObjType::Muon) {
          // 2e7T_medium1_mu6
          if (m_EDM->EF_2e7T_medium1_mu6 &&
              matchesToEFTriggerEle(evtdef.leps[i].tlv,m_EDM->Get(D3PD::trig_EF_el_EF_e7T_medium1)) &&
              matchesToEFTriggerEle(evtdef.leps[j].tlv,m_EDM->Get(D3PD::trig_EF_el_EF_e7T_medium1)) &&
              //m_muonTriggerMatchTool->match(evtdef.leps[k].Eta(),evtdef.leps[k].Phi(),"EF_mu6"))
              matchMuonTrigger(evtdef.leps[k].tlv,m_EDM->Get(D3PD::trig_EF_trigmuonef_EF_mu6)))
            trig_match = trig_match | 0x1 << Trigger2012::EF_2e7T_medium1_mu6;
        }
        if (evtdef.leps[i].obj_type == ObjType::Electron && evtdef.leps[j].obj_type == ObjType::Muon && evtdef.leps[k].obj_type == ObjType::Muon) {
          // e7T_medium1_2mu6
          if (m_EDM->EF_e7T_medium1_2mu6 &&
              matchesToEFTriggerEle(evtdef.leps[i].tlv,m_EDM->Get(D3PD::trig_EF_el_EF_e7T_medium1)) &&
              //m_muonTriggerMatchTool->match(evtdef.leps[j].Eta(),evtdef.leps[j].Phi(),"EF_mu6") &&
              //m_muonTriggerMatchTool->match(evtdef.leps[k].Eta(),evtdef.leps[k].Phi(),"EF_mu6"))
             matchMuonTrigger(evtdef.leps[j].tlv,m_EDM->Get(D3PD::trig_EF_trigmuonef_EF_mu6)) &&
             matchMuonTrigger(evtdef.leps[k].tlv,m_EDM->Get(D3PD::trig_EF_trigmuonef_EF_mu6)))              
            trig_match = trig_match | 0x1 << Trigger2012::EF_e7T_medium1_2mu6;
        }
        if (evtdef.leps[i].obj_type == ObjType::Muon && evtdef.leps[j].obj_type == ObjType::Muon && evtdef.leps[k].obj_type == ObjType::Muon) {
          // mu18_tight_2mu4_EFFS
          if(m_EDM->EF_mu18_tight_2mu4_EFFS &&
             matchMuonTrigger(evtdef.leps[i].tlv,m_EDM->Get(D3PD::trig_EF_trigmuonef_EF_mu18_tight_2mu4_EFFS)) &&
             matchMuonTrigger(evtdef.leps[j].tlv,m_EDM->Get(D3PD::trig_EF_trigmuonef_EF_mu18_tight_2mu4_EFFS)) &&
             matchMuonTrigger(evtdef.leps[k].tlv,m_EDM->Get(D3PD::trig_EF_trigmuonef_EF_mu18_tight_2mu4_EFFS)))
            trig_match = trig_match | 0x1 << Trigger2012::EF_mu18_tight_2mu4_EFFS;
          // 3mu6
          if(m_EDM->EF_3mu6 &&
             // m_muonTriggerMatchTool->match(evtdef.leps[i].Eta(),evtdef.leps[i].Phi(), "EF_3mu6") &&
             // m_muonTriggerMatchTool->match(evtdef.leps[j].Eta(),evtdef.leps[j].Phi(), "EF_3mu6") &&
             // m_muonTriggerMatchTool->match(evtdef.leps[k].Eta(),evtdef.leps[k].Phi(), "EF_3mu6"))
             matchMuonTrigger(evtdef.leps[i].tlv,m_EDM->Get(D3PD::trig_EF_trigmuonef_EF_mu6)) &&
             matchMuonTrigger(evtdef.leps[j].tlv,m_EDM->Get(D3PD::trig_EF_trigmuonef_EF_mu6)) &&
             matchMuonTrigger(evtdef.leps[k].tlv,m_EDM->Get(D3PD::trig_EF_trigmuonef_EF_mu6)))
            trig_match = trig_match | 0x1 << Trigger2012::EF_3mu6;
        }
      } // k
    } // j
  } // i
  std::string mask = "";
  for (unsigned int i=0;i<Trigger2012::n_Trig;++i){
    mask += ((trig_match & (0x1 << i)) == (0x1 << i) ? "1" : "0");
  }
  MSG_DEBUG("Bitmask: " << mask);
}

bool PSL::ToolsD3PDEra::matchesToEFTriggerEle(TLorentzVector& ele
                                              ,std::vector<int>* EF
                                              ,double EF_thresh
                                              //,bool EF_I=false
                                              //,double EF_Ival=0.1
                                              ,double deltar
                                              ){
  
  //
  // Et threshold
  //
  if (EF_thresh >= 0.){
    MSG_DEBUG("Checking Et " << ele.Pt() << " against " << 1000.*EF_thresh);
    if (ele.Pt() < 1000.*EF_thresh) return false;
  }
  //   //
  //   // Isolation threshold
  //   //
  //   if (EF_I){
  //     m_logger << DEBUG << "Checking iso " << el_ptcone20->at(i)/el_pt->at(i) << " against " << EF_Ival << SLogger::endmsg;
  //     if (el_ptcone20->at(i)/el_pt->at(i) > EF_Ival) return false;
  //   }
  //
  // DeltaR matching - do last! (In case some branches are missing or something.)
  //
  if (!EF->size() && m_EDM->Get(D3PD::trig_EF_el_n)) {
    MSG_INFO("Skipping DeltaR trigger matching! (Passs EF!) Branch missing?");
    return true;
  }
  int iEF = -1;
  double tmpdeltaR;
  double DRmin=999.;
  for (int j=0;j<m_EDM->Get(D3PD::trig_EF_el_n);j++){
    if (!EF->at(j)) continue;
    tmpdeltaR = GetDeltaR(ele.Eta(),ele.Phi(),m_EDM->Get(D3PD::trig_EF_el_eta)->at(j),m_EDM->Get(D3PD::trig_EF_el_phi)->at(j));
    MSG_DEBUG("tmpdeltaR " << tmpdeltaR);
    if (tmpdeltaR < DRmin){
      DRmin = tmpdeltaR;
      iEF = j;
    }
  }
  MSG_DEBUG("Checking DRmin " << DRmin << " against " << deltar);
  if(DRmin > deltar) return false;
  MSG_DEBUG("Passes EF!");
  return true;
}

// Courtesy SusyCommon
bool PSL::ToolsD3PDEra::matchMuonTrigger(const TLorentzVector& lv, std::vector<int>* passTrig)
{
  // loop over muon trigger features
  for(int iTrig=0; iTrig < m_EDM->Get(D3PD::trig_EF_trigmuonef_n); iTrig++){
    // Check to see if this feature passed chain we want
    if(passTrig->at(iTrig)){
      // Loop over muon EF tracks
      TLorentzVector lvTrig;
      for(int iTrk=0; iTrk < m_EDM->Get(D3PD::trig_EF_trigmuonef_track_n)->at(iTrig); iTrk++){
        lvTrig.SetPtEtaPhiM( m_EDM->Get(D3PD::trig_EF_trigmuonef_track_CB_pt)->at(iTrig).at(iTrk),
                             m_EDM->Get(D3PD::trig_EF_trigmuonef_track_CB_eta)->at(iTrig).at(iTrk),
                             m_EDM->Get(D3PD::trig_EF_trigmuonef_track_CB_phi)->at(iTrig).at(iTrk),
                             0 );       // only eta and phi used to compute dR anyway
        // Require combined offline track...?
        if(!(m_EDM->Get(D3PD::trig_EF_trigmuonef_track_CB_hasCB)->at(iTrig).at(iTrk))) continue;
        float dR = lv.DeltaR(lvTrig);
        if(dR < 0.15){
          return true;
        }
      } // loop over EF tracks
    } // trigger object passes chain?
  } // loop over trigger objects
  // matching failed
  return false;
}

#endif // SKIP_D3PD_COMPILATION
