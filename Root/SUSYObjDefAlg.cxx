#include <pennSoftLepton/SUSYObjDefAlg.h>
#ifndef SKIP_D3PD_COMPILATION

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::SUSYObjDefAlg)
  
//-----------------------------------------------------------------------------
PSL::SUSYObjDefAlg::SUSYObjDefAlg() : AlgBase()
  ,ele_ptmin              (7.     )
  ,mu_ptmin               (6.     ) // 6 GeV to avoid the overlap with the Eflow
  ,tau_ptmin              (20.    )
  ,jet_ptmin              (20.    )
  ,ele_eta_max            (2.47   )
  ,mu_eta_max             (2.5    )
  ,tau_eta_max            (2.47   )
  ,jet_eta_max            (999999.)
  ,ele_d0_max             (999999.)
  ,ele_z0_max             (999999.)
  ,ele_trkiso_max         (999999.)
  ,ele_caloiso_max        (999999.)
  ,ele_id_base            (     1.) // loose = 0, medium = 1, tight = 2
  ,ele_id                 (     2.) // loose = 0, medium = 1, tight = 2
  ,mu_d0_max              (999999.)
  ,mu_z0_max              (999999.)
  ,mu_trkiso_max          (999999.)
  ,mu_caloiso_max         (999999.)
  ,jet_forward_eta_min    (2.4    )
  ,jet_forward_eta_max    (4.5    )
  ,jet_forward_ptmin      (30.    )
  ,jet_bjet_ptmin         (20.    )
  ,jet_bjet_mv1           (0.3511 ) // corresponds to op point 80. MV1_60 = 0.980; MV1_85 = 0.122; MV1_80 = 0.3511;
  // see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/BtagAnalysis172#LC_Jets
  ,jet_central_jvfcut     (0.     )
  ,jet_central_jvfeta_max (2.4    )
  ,jet_central_eta_max    (2.4    )
  ,jet_central_ptmin      (20.    )
  ,jet_central_mv1        (0.3511 )
  ,do_overlapremove       (true)
  ,m_SUSYObjDef           (0)
{
  MSG_DEBUG("Constructor");
}

//-----------------------------------------------------------------------------
bool PSL::SUSYObjDefAlg::init(void)
{
//   MSG_DEBUG("Likelihood algbase name is " << configured_name);
//   MSG_DEBUG("Likelihood name is " << lh_name);
  
  // Make sure (in EventLoop) that the new call and the setHistogramOutput bit are in the same step
  //   myHist = new TH1F("myHist","myHist",200,-2,2);
//   setHistogramOutput(myHist);

  m_SUSYObjDef = new SUSYObjDef();
  bool isdata = false;
  bool isatlfast = false;
  bool ismc12b = true;
  std::string muon_sf_dir = "TrigMuonEfficiency/share/muon_trigger_sf_mc11c.root";
  m_SUSYObjDef->initialize(isdata,isatlfast,ismc12b,false
// 			 ,"" // muon_momentum_dir
// 			 ,"" // muon_sf_dir
// 			 ,"efficiencySF.offline.RecoTrk.2012.8TeV.rel17p2.GEO20.v08.root" // electron_reco_file
// 			 ,"efficiencySF.offline.Tight.2012.8TeV.rel17p2.v07.root" // electron_id_file
// 			 ,"efficiencySF.e24vhi_medium1_e60_medium1.Tight.2012.8TeV.rel17p2.v07.root" // electron_trigger_file
// 			 ,"muon_trigger_sf_2012_AtoL.p1328.root" // muon_trigger_file
			 ); // last thing is "useLeptonTrigger"

  m_toolsd3pd = (ToolsD3PDEra*)connectToAnotherAlgorithm("TOOLS_D3PD");
  if (!m_toolsd3pd) {
    MSG_INFO("Error! Could not find ToolsD3PDEra object. Exiting.");
    return false;
  }

  PrintConfiguration();
  return true;
}

//-----------------------------------------------------------------------------
void PSL::SUSYObjDefAlg::loop(void){
  MSG_VERBOSE("loop.");
  m_evtdef.Reset(m_EDM->el_n,m_EDM->mu_staco_n,m_EDM->tau_n,m_EDM->jet_AntiKt4LCTopo_n);
  m_SUSYObjDef->Reset();
  m_EDM->Reset();

  if (m_EDM->el_MET_Egamma10NoTau_n != m_EDM->el_n){
    MSG_INFO("Size of el_MET_Egamma10NoTau_n != el_n (" 
             << m_EDM->el_MET_Egamma10NoTau_n << " vs " << m_EDM->el_n << ".");
  }

  // get met electrons
  std::vector<int> metElectrons;
  for (int i=0; i<m_EDM->el_MET_Egamma10NoTau_n; ++i) {
    if(m_EDM->Get(D3PD::el_MET_Egamma10NoTau_wet)->at(i).at(0) != 0){
      metElectrons.push_back(i);
    }
  }

  // get_muons_baseline from SusyCommon
  std::vector<int> metMuons;
  for (int i=0;i<m_EDM->mu_staco_n;++i){
    // treat pt separately, on behalf of MET
    bool passes = true;
    passes = passes & 
      m_SUSYObjDef->FillMuon(i,m_EDM->Get(D3PD::mu_staco_pt                 )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_eta                )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_phi                )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_me_qoverp_exPV     )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_id_qoverp_exPV     )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_me_theta_exPV      )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_id_theta_exPV      )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_id_theta           )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_charge             )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_isCombinedMuon     )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_isSegmentTaggedMuon)->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_loose              )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_nPixHits           )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_nPixelDeadSensors  )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_nPixHoles          )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_nSCTHits           )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_nSCTDeadSensors    )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_nSCTHoles          )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_nTRTHits           )->at(i)
                             ,m_EDM->Get(D3PD::mu_staco_nTRTOutliers       )->at(i)
                             ,0. // mu_ptmin*1000. DO LATER
                             ,mu_eta_max
                             ,SystErr::NONE
                             );
    // get met muons - same as pre muons except with a pt cut of 10
    if (passes && (m_SUSYObjDef->GetMuonTLV(i).Pt() > 10000.)) metMuons.push_back(i);
    passes = passes & (m_SUSYObjDef->GetMuonTLV(i).Pt() > mu_ptmin*1000.);
    if (passes){
      m_evtdef.mu.passed_so_far.push_back(i);
      m_evtdef.mu.i_pre.push_back(i);
    }
    else m_evtdef.mu.removed_pre.push_back(i);

    m_evtdef.mu.tlv[i] = m_SUSYObjDef->GetMuonTLV(i);
    m_evtdef.mu.charge[i] = m_EDM->Get(D3PD::mu_staco_charge)->at(i);
    MSG_DEBUG("Muon pt: " << m_SUSYObjDef->GetMuonTLV(i).Pt());
  }

  // get_electrons_baseline from SusyCommon
  for (int i=0;i<m_EDM->el_n;++i){
    bool passes = true;
    passes = passes & 
      m_SUSYObjDef->FillElectron(i,m_EDM->Get(D3PD::el_eta                  )->at(i)
                                 ,m_EDM->Get(D3PD::el_phi                  )->at(i)
                                 ,m_EDM->Get(D3PD::el_cl_eta               )->at(i)
                                 ,m_EDM->Get(D3PD::el_cl_phi               )->at(i)
                                 ,m_EDM->Get(D3PD::el_cl_E                 )->at(i)
                                 ,m_EDM->Get(D3PD::el_tracketa             )->at(i)
                                 ,m_EDM->Get(D3PD::el_trackphi             )->at(i)
                                 ,m_EDM->Get(D3PD::el_author               )->at(i)
                                 ,m_EDM->Get(D3PD::el_mediumPP             )->at(i) // acts on this!
                                 ,m_EDM->Get(D3PD::el_OQ                   )->at(i)
                                 ,m_EDM->Get(D3PD::el_nPixHits             )->at(i)
                                 ,m_EDM->Get(D3PD::el_nSCTHits             )->at(i)
                                 ,m_EDM->Get(D3PD::el_MET_Egamma10NoTau_wet)->at(i).at(0)
                                 ,ele_ptmin*1000.
                                 ,ele_eta_max
                                 ,SystErr::NONE
                                 );
    if (passes) {
      m_evtdef.ele.passed_so_far.push_back(i);
      m_evtdef.ele.i_pre.push_back(i);
    }
    else m_evtdef.ele.removed_pre.push_back(i);

    m_evtdef.ele.tlv[i] = m_SUSYObjDef->GetElecTLV(i);
    m_evtdef.ele.charge[i] = (int)m_EDM->Get(D3PD::el_charge)->at(i);
    MSG_DEBUG("Electron pt: " << m_SUSYObjDef->GetElecTLV(i).Pt());
  }

  // get_taus_baseline from SusyCommon
  for (int i=0;i<m_EDM->tau_n;++i){
    if (m_SUSYObjDef->FillTau(i,m_EDM->Get(D3PD::tau_pt              )->at(i)
			      ,m_EDM->Get(D3PD::tau_eta             )->at(i)
			      ,m_EDM->Get(D3PD::tau_leadTrack_eta   )->at(i)
			      ,m_EDM->Get(D3PD::tau_phi             )->at(i)
			      ,m_EDM->Get(D3PD::tau_Et              )->at(i)
			      ,m_EDM->Get(D3PD::tau_charge          )->at(i)
			      ,m_EDM->Get(D3PD::tau_numTrack        )->at(i)
			      ,m_EDM->Get(D3PD::tau_JetBDTSigLoose  )->at(i)
			      ,m_EDM->Get(D3PD::tau_JetBDTSigMedium )->at(i)
			      ,m_EDM->Get(D3PD::tau_JetBDTSigTight  )->at(i)
			      ,m_EDM->Get(D3PD::tau_EleBDTLoose     )->at(i)
			      ,m_EDM->Get(D3PD::tau_EleBDTMedium    )->at(i)
			      ,m_EDM->Get(D3PD::tau_EleBDTTight     )->at(i)
			      ,m_EDM->Get(D3PD::tau_BDTEleScore     )->at(i)
			      ,m_EDM->Get(D3PD::tau_muonVeto        )->at(i)
			      ,SUSYTau::TauLoose
			      ,SUSYTau::TauLoose
			      ,SUSYTau::TauLoose
			      ,tau_ptmin*1000.
			      ,tau_eta_max
			      ,SystErr::NONE
			      ,false // isD3PD1512
			      )) {
      m_evtdef.tau.passed_so_far.push_back(i);
      m_evtdef.tau.i_pre.push_back(i);
    }
    else m_evtdef.tau.removed_pre.push_back(i);
    m_evtdef.tau.tlv[i] = m_SUSYObjDef->GetTauTLV(i);
    m_evtdef.tau.charge[i] = (int)m_EDM->Get(D3PD::tau_charge)->at(i);
    MSG_DEBUG("Tau pt: " << m_SUSYObjDef->GetTauTLV(i).Pt());
  }

  // get_jet_baseline from SusyCommon
  for (int i=0;i<m_EDM->jet_AntiKt4LCTopo_n;++i){
    // FillJet:
    // - sets jet tlv
    // - ApplyJetAreaOffsetEtaJES (by default - m_doJetCalib)
    // - Fills m_jet_D3PD
    // - ALWAYS returns true
    m_SUSYObjDef->FillJet(i,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_pt)->at(i)
                          ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_eta)->at(i)
                          ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_phi)->at(i)
                          ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_E)->at(i)
                          ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_constscale_eta)->at(i)
                          ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_constscale_phi)->at(i)
                          ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_constscale_E)->at(i)
                          ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_constscale_m)->at(i)
                          ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_ActiveAreaPx)->at(i)
                          ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_ActiveAreaPy)->at(i)
                          ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_ActiveAreaPz)->at(i)
                          ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_ActiveAreaE)->at(i)
                          ,m_EDM->Eventshape_rhoKt4LC
                          ,m_EDM->averageIntPerXing
                          ,m_EDM->Get(D3PD::vxp_nTracks)
                          );
    bool passes = true;
    // local truth label?
    // pt and eta cut
    passes = passes & (m_SUSYObjDef->GetJetTLV(i).Pt() > jet_ptmin*1000.);
    passes = passes & (m_EDM->Get(D3PD::jet_AntiKt4LCTopo_E)->at(i) > 0);
    passes = passes & (fabs(m_EDM->Get(D3PD::jet_AntiKt4LCTopo_eta)->at(i)) < jet_eta_max);
    if (passes) {
      m_evtdef.jet.passed_so_far.push_back(i);
      m_evtdef.jet.i_pre.push_back(i);
    }
    else m_evtdef.jet.removed_pre.push_back(i);
    m_evtdef.jet.tlv[i] = m_SUSYObjDef->GetJetTLV(i);
    MSG_DEBUG("Jet pt: " << m_SUSYObjDef->GetJetTLV(i).Pt());
  }

  //m_SUSYObjDef GetMet. Must be done after jets.
  MSG_DEBUG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  MSG_DEBUG("jetwet                       " << m_EDM->Get(D3PD::jet_AntiKt4LCTopo_MET_Egamma10NoTau_wet));
  MSG_DEBUG("jetwpx                       " << m_EDM->Get(D3PD::jet_AntiKt4LCTopo_MET_Egamma10NoTau_wpx));
  MSG_DEBUG("jetwpy                       " << m_EDM->Get(D3PD::jet_AntiKt4LCTopo_MET_Egamma10NoTau_wpy));
  MSG_DEBUG("jetstatus                    " << m_EDM->Get(D3PD::jet_AntiKt4LCTopo_MET_Egamma10NoTau_statusWord));
  MSG_DEBUG("velidx                       " << metElectrons.size());
  MSG_DEBUG("elwet                        " << m_EDM->Get(D3PD::el_MET_Egamma10NoTau_wet));
  MSG_DEBUG("elwpx                        " << m_EDM->Get(D3PD::el_MET_Egamma10NoTau_wpx));
  MSG_DEBUG("elwpy                        " << m_EDM->Get(D3PD::el_MET_Egamma10NoTau_wpy));
  MSG_DEBUG("elstatus                     " << m_EDM->Get(D3PD::el_MET_Egamma10NoTau_statusWord));
  MSG_DEBUG("MET_Egamma10NoTau_CellOut_etx   " << m_EDM->MET_Egamma10NoTau_CellOut_etx);
  MSG_DEBUG("MET_Egamma10NoTau_CellOut_ety   " << m_EDM->MET_Egamma10NoTau_CellOut_ety);
  MSG_DEBUG("MET_Egamma10NoTau_CellOut_sumet " << m_EDM->MET_Egamma10NoTau_CellOut_sumet);
  MSG_DEBUG("MET_CellOut_Eflow_STVF_etx   " << m_EDM->MET_CellOut_Eflow_STVF_etx);
  MSG_DEBUG("MET_CellOut_Eflow_STVF_ety   " << m_EDM->MET_CellOut_Eflow_STVF_ety);
  MSG_DEBUG("MET_CellOut_Eflow_STVF_sumet " << m_EDM->MET_CellOut_Eflow_STVF_sumet);
  MSG_DEBUG("MET_Egamma10NoTau_RefGamma_etx   " << m_EDM->MET_Egamma10NoTau_RefGamma_etx);
  MSG_DEBUG("MET_Egamma10NoTau_RefGamma_ety   " << m_EDM->MET_Egamma10NoTau_RefGamma_ety);
  MSG_DEBUG("MET_Egamma10NoTau_RefGamma_sumet " << m_EDM->MET_Egamma10NoTau_RefGamma_sumet);
  MSG_DEBUG("vmuidx                       " << metMuons.size());
  MSG_DEBUG("mu_staco_ms_qoverp           " << m_EDM->Get(D3PD::mu_staco_ms_qoverp));
  MSG_DEBUG("mu_staco_ms_theta            " << m_EDM->Get(D3PD::mu_staco_ms_theta));
  MSG_DEBUG("mu_staco_ms_phi              " << m_EDM->Get(D3PD::mu_staco_ms_phi));
  MSG_DEBUG("mu_staco_charge              " << m_EDM->Get(D3PD::mu_staco_charge));
  MSG_DEBUG("mu_staco_energyLossPar       " << m_EDM->Get(D3PD::mu_staco_energyLossPar));
  MSG_DEBUG("averageIntPerXing            " << m_EDM->averageIntPerXing);
  MSG_DEBUG("SUSYMet::met_definition      " << SUSYMet::Default);
  MSG_DEBUG("SystErr::Syste               " << SystErr::NONE);

  m_evtdef.met_tv2 =
    m_SUSYObjDef->GetMET(m_EDM->Get(D3PD::jet_AntiKt4LCTopo_MET_Egamma10NoTau_wet)
                         ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_MET_Egamma10NoTau_wpx)
                         ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_MET_Egamma10NoTau_wpy)
                         ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_MET_Egamma10NoTau_statusWord)
                         ,metElectrons
                         ,m_EDM->Get(D3PD::el_MET_Egamma10NoTau_wet)
                         ,m_EDM->Get(D3PD::el_MET_Egamma10NoTau_wpx)
                         ,m_EDM->Get(D3PD::el_MET_Egamma10NoTau_wpy)
                         ,m_EDM->Get(D3PD::el_MET_Egamma10NoTau_statusWord)
                         ,m_EDM->MET_Egamma10NoTau_CellOut_etx
                         ,m_EDM->MET_Egamma10NoTau_CellOut_ety
                         ,m_EDM->MET_Egamma10NoTau_CellOut_sumet
                         ,m_EDM->MET_CellOut_Eflow_STVF_etx // probably wrong? do not use STVF
                         ,m_EDM->MET_CellOut_Eflow_STVF_ety // probably wrong? do not use STVF
                         ,m_EDM->MET_CellOut_Eflow_STVF_sumet // probably wrong? do not use STVF
                         ,m_EDM->MET_Egamma10NoTau_RefGamma_etx
                         ,m_EDM->MET_Egamma10NoTau_RefGamma_ety
                         ,m_EDM->MET_Egamma10NoTau_RefGamma_sumet
                         ,metMuons
                         ,m_EDM->Get(D3PD::mu_staco_ms_qoverp)
                         ,m_EDM->Get(D3PD::mu_staco_ms_theta)
                         ,m_EDM->Get(D3PD::mu_staco_ms_phi)
                         ,m_EDM->Get(D3PD::mu_staco_charge)
                         ,m_EDM->Get(D3PD::mu_staco_energyLossPar)
                         ,m_EDM->averageIntPerXing
                         ,SUSYMet::Default // SUSYMet::Default STVF STVF_JVF
                         ,SystErr::NONE);

  METUtility* metUtil = m_SUSYObjDef->GetMETUtility();
  m_evtdef.met_sumet      = metUtil->getMissingET(METUtil::RefFinal, METUtil::None).sumet();

  m_evtdef.refEle_tv2     = m_SUSYObjDef->computeMETComponent(METUtil::RefEle   ,METUtil::None);
  m_evtdef.refEle_sumet   = metUtil->getMissingET            (METUtil::RefEle   ,METUtil::None).sumet();
  m_evtdef.refMuo_tv2     = m_SUSYObjDef->computeMETComponent(METUtil::MuonTotal,METUtil::None);
  m_evtdef.refMuo_sumet   = metUtil->getMissingET            (METUtil::MuonTotal,METUtil::None).sumet();
  m_evtdef.refJet_tv2     = m_SUSYObjDef->computeMETComponent(METUtil::RefJet   ,METUtil::None);
  m_evtdef.refJet_sumet   = metUtil->getMissingET            (METUtil::RefJet   ,METUtil::None).sumet();
  m_evtdef.refGamma_tv2   = m_SUSYObjDef->computeMETComponent(METUtil::RefGamma ,METUtil::None);
  m_evtdef.refGamma_sumet = metUtil->getMissingET            (METUtil::RefGamma ,METUtil::None).sumet();
  m_evtdef.softTerm_tv2   = m_SUSYObjDef->computeMETComponent(METUtil::SoftTerms,METUtil::None);
  m_evtdef.softTerm_sumet = metUtil->getMissingET            (METUtil::SoftTerms,METUtil::None).sumet();

  // MSG_INFO("met_Et()      : " << m_evtdef.met_Et()      );
  // MSG_INFO("met_phi()     : " << m_evtdef.met_phi()     );
  // MSG_INFO("met_sumet     : " << m_evtdef.met_sumet     );
  // MSG_INFO("refEle()      : " << m_evtdef.refEle()      );
  // MSG_INFO("refEle_etx()  : " << m_evtdef.refEle_etx()  );
  // MSG_INFO("refEle_ety()  : " << m_evtdef.refEle_ety()  );
  // MSG_INFO("refEle_sumet  : " << m_evtdef.refEle_sumet  );
  // MSG_INFO("refMuo()      : " << m_evtdef.refMuo()      );
  // MSG_INFO("refMuo_etx()  : " << m_evtdef.refMuo_etx()  );
  // MSG_INFO("refMuo_ety()  : " << m_evtdef.refMuo_ety()  );
  // MSG_INFO("refMuo_sumet  : " << m_evtdef.refMuo_sumet  );
  // MSG_INFO("refJet()      : " << m_evtdef.refJet()      );
  // MSG_INFO("refJet_etx()  : " << m_evtdef.refJet_etx()  );
  // MSG_INFO("refJet_ety()  : " << m_evtdef.refJet_ety()  );
  // MSG_INFO("refJet_sumet  : " << m_evtdef.refJet_sumet  );
  // MSG_INFO("refGamma()    : " << m_evtdef.refGamma()    );
  // MSG_INFO("refGamma_etx(): " << m_evtdef.refGamma_etx());
  // MSG_INFO("refGamma_ety(): " << m_evtdef.refGamma_ety());
  // MSG_INFO("refGamma_sumet: " << m_evtdef.refGamma_sumet);
  // MSG_INFO("softTerm()    : " << m_evtdef.softTerm()    );
  // MSG_INFO("softTerm_etx(): " << m_evtdef.softTerm_etx());
  // MSG_INFO("softTerm_ety(): " << m_evtdef.softTerm_ety());
  // MSG_INFO("softTerm_sumet: " << m_evtdef.softTerm_sumet);

  // get_taus_baseline AGAIN but this time with TauLoose ?


  // Because apparently BCH is done before overlap removal, hack to reset signal jets
  // m_evtdef.tau.i_ba
  // Run to get signal-level analysis jets
  //std::vector<int> tmp_jets = m_evtdef.jet.passed_so_far;
  //FindSignalJets();
  //m_evtdef.i_bch_jet = m_evtdef.jet.i_sig; // store these signal jets here
  //HandleBCH(); // needs signal jets already
  //m_evtdef.jet.passed_so_far = tmp_jets;
  //m_evtdef.jet.i_sig.clear();
  //m_evtdef.jet.removed_signal.clear();

  // NOW perform overlap removal
  if (do_overlapremove) OverlapRemoveAll();

  m_evtdef.ele.i_base = m_evtdef.ele.passed_so_far;
  m_evtdef.mu.i_base  = m_evtdef.mu.passed_so_far ;
  m_evtdef.tau.i_base = m_evtdef.tau.passed_so_far;
  m_evtdef.jet.i_base = m_evtdef.jet.passed_so_far;

  //
  // Do bad jet after ONLY overlap removal
  //
  HandleBadJet();
  FindSignalTaus();
  FindSignalJets();
  m_evtdef.i_bch_jet = m_evtdef.jet.passed_so_far; // set bch using jets that passed so far
  HandleBCH(); // runs over the i_bch_jet indices



  //m_evtdef.ele.i_sig = m_evtdef.ele.passed_so_far;
  FindSignalElectrons();
  //m_evtdef.mu.i_sig  = m_evtdef.mu.passed_so_far ;
  FindSignalMuons();

  // signal jets. Decision was stored in i_bch_jet.
  std::vector<int> killed;
  for (unsigned int ii=0;ii<m_evtdef.jet.passed_so_far.size();++ii){
    int i = m_evtdef.jet.passed_so_far[ii];
    if (std::count(m_evtdef.m_passBCH.begin(),m_evtdef.m_passBCH.end(),i))
      m_evtdef.jet.i_sig.push_back(i);
    else killed.push_back(i);
  }
  for(unsigned int i=0; i<killed.size(); i++) {
    TransferIndexFromVecToVec(killed[i],m_evtdef.jet.passed_so_far,m_evtdef.jet.removed_signal);
  }

  m_evtdef.BuildParticles();
  // Trigger matching
  m_toolsd3pd->TriggerMatch(m_evtdef);

  m_EDM->SetEventDefinitionPointer(&m_evtdef);
  m_EDM->AddThinIndices(D3PD::el_n                  ,m_evtdef.ele.i_base);
  m_EDM->AddThinIndices(D3PD::el_MET_Egamma10NoTau_n,m_evtdef.ele.i_base);
  m_EDM->AddThinIndices(D3PD::mu_staco_n            ,m_evtdef.mu.i_base);
  m_EDM->AddThinIndices(D3PD::tau_n                 ,m_evtdef.tau.i_base);
  m_EDM->AddThinIndices(D3PD::jet_AntiKt4LCTopo_n   ,m_evtdef.jet.i_base);

  MSG_VERBOSE("loop end");
}

//-----------------------------------------------------------------------------
void PSL::SUSYObjDefAlg::finish(void){
  MSG_DEBUG("finish.");
}

//-----------------------------------------------------------------------------
void PSL::SUSYObjDefAlg::PrintConfiguration(void){
  MSG_INFO("SUSYObjDef Configuration:");
  MSG_INFO("ele_ptmin              : " << ele_ptmin              );
  MSG_INFO("mu_ptmin               : " << mu_ptmin               );
  MSG_INFO("tau_ptmin              : " << tau_ptmin              );
  MSG_INFO("jet_ptmin              : " << jet_ptmin              );
  MSG_INFO("ele_eta_max            : " << ele_eta_max            );
  MSG_INFO("mu_eta_max             : " << mu_eta_max             );
  MSG_INFO("tau_eta_max            : " << tau_eta_max            );
  MSG_INFO("jet_eta_max            : " << jet_eta_max            );
  MSG_INFO("ele_d0_max             : " << ele_d0_max             );
  MSG_INFO("ele_z0_max             : " << ele_z0_max             );
  MSG_INFO("ele_trkiso_max         : " << ele_trkiso_max         );
  MSG_INFO("ele_caloiso_max        : " << ele_caloiso_max        );
  MSG_INFO("ele_id                 : " << ele_id                 );
  MSG_INFO("mu_d0_max              : " << mu_d0_max              );
  MSG_INFO("mu_z0_max              : " << mu_z0_max              );
  MSG_INFO("mu_trkiso_max          : " << mu_trkiso_max          );
  MSG_INFO("mu_caloiso_max         : " << mu_caloiso_max         );
  MSG_INFO("jet_forward_eta_min    : " << jet_forward_eta_min    );
  MSG_INFO("jet_forward_eta_max    : " << jet_forward_eta_max    );
  MSG_INFO("jet_forward_ptmin      : " << jet_forward_ptmin      );
  MSG_INFO("jet_bjet_ptmin         : " << jet_bjet_ptmin         );
  MSG_INFO("jet_bjet_mv1           : " << jet_bjet_mv1           );
  MSG_INFO("jet_central_jvfcut     : " << jet_central_jvfcut     );
  MSG_INFO("jet_central_jvfeta_max : " << jet_central_jvfeta_max );
  MSG_INFO("jet_central_eta_max    : " << jet_central_eta_max    );
  MSG_INFO("jet_central_ptmin      : " << jet_central_ptmin      );
  MSG_INFO("jet_central_mv1        : " << jet_central_mv1        );
  MSG_INFO("do_overlapremove       : " << (do_overlapremove ? "yes" : "no"));
}

//-----------------------------------------------------------------------------
void PSL::SUSYObjDefAlg::OverlapRemoveAll(void){
  // e-e overlap removal
  // m_evtdef.PrintEleIndices("pre-e-e-overlap removal");
  OverlapRemove(0,0,m_evtdef.ele.passed_so_far,m_evtdef.ele.removed_olap_e
                ,m_evtdef.ele.passed_so_far,0.05);
  // m_evtdef.PrintEleIndices("post-e-e-overlap removal");

  // jet-e overlap removal
  // MSG_INFO("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  // m_evtdef.PrintJetIndices("pre-j-e-overlap removal");
  // m_evtdef.PrintEleIndices("pre-j-e-overlap removal");
  OverlapRemove(3,0,m_evtdef.jet.passed_so_far,m_evtdef.jet.removed_olap_e
        	,m_evtdef.ele.passed_so_far,0.2);
  // m_evtdef.PrintJetIndices("post-j-e-overlap removal");

  // tau-e overlap removal
  // m_evtdef.PrintTauIndices("pre-t-e-overlap removal");
  OverlapRemove(2,0,m_evtdef.tau.passed_so_far,m_evtdef.tau.removed_olap_e
		,m_evtdef.ele.passed_so_far,0.2);
  // m_evtdef.PrintTauIndices("post-t-e-overlap removal");
  
  // tau-mu overlap removal  
  // m_evtdef.PrintTauIndices("pre-t-mu-overlap removal");
  OverlapRemove(2,1,m_evtdef.tau.passed_so_far,m_evtdef.tau.removed_olap_mu
                ,m_evtdef.mu.passed_so_far,0.2);
  // m_evtdef.PrintTauIndices("post-t-mu-overlap removal");

  // e-jet overlap removal
  // m_evtdef.PrintEleIndices("pre-e-jet-overlap removal");
  // m_evtdef.PrintJetIndices("pre-e-jet-overlap removal");
  OverlapRemove(0,3,m_evtdef.ele.passed_so_far,m_evtdef.ele.removed_olap_jet
                ,m_evtdef.jet.passed_so_far,0.4);
  // m_evtdef.PrintEleIndices("post-e-jet-overlap removal");

  // m-jet overlap removal
  // m_evtdef.PrintMuIndices("pre-mu-jet-overlap removal");
  OverlapRemove(1,3,m_evtdef.mu.passed_so_far,m_evtdef.mu.removed_olap_jet
                ,m_evtdef.jet.passed_so_far,0.4);
  // m_evtdef.PrintMuIndices("post-mu-jet-overlap removal");

  std::vector<int> ele_static_indices = m_evtdef.ele.passed_so_far;
  // e-m overlap removal
  // m_evtdef.PrintEleIndices("pre-e-mu-overlap removal");
  OverlapRemove(0,1,m_evtdef.ele.passed_so_far,m_evtdef.ele.removed_olap_mu
                ,m_evtdef.mu.passed_so_far,0.01);
  // m_evtdef.PrintEleIndices("post-e-mu-overlap removal");

  // m-e overlap removal
  // m_evtdef.PrintMuIndices("pre-mu-e-overlap removal");
  OverlapRemove(1,0,m_evtdef.mu.passed_so_far,m_evtdef.mu.removed_olap_e
                ,ele_static_indices,0.01);
  // m_evtdef.PrintMuIndices("post-mu-e-overlap removal");

  // m-m overlap removal
  // m_evtdef.PrintMuIndices("pre-mu-mu-overlap removal");
  OverlapRemove(1,1,m_evtdef.mu.passed_so_far,m_evtdef.mu.removed_olap_mu
                ,m_evtdef.mu.passed_so_far,0.05,false);
  // m_evtdef.PrintMuIndices("post-mu-mu-overlap removal");

  // jet-tau overlap removal
  // m_evtdef.PrintJetIndices("pre-j-t-overlap removal");
  OverlapRemove(3,2,m_evtdef.jet.passed_so_far,m_evtdef.jet.removed_olap_tau
        	,m_evtdef.ele.passed_so_far,0.2);
  // m_evtdef.PrintJetIndices("post-j-t-overlap removal");
}

//-----------------------------------------------------------------------------
void PSL::SUSYObjDefAlg::OverlapRemove(int type_removal // 0 == ele, 1 == mu, 2 == tau, 3 == jet
				       ,int type_static
				       ,std::vector<int>& indices_removal
				       ,std::vector<int>& indices_dumpto
				       ,std::vector<int> indices_static
				       ,double dr
				       ,bool remove_softer
				       ){
  std::vector<int> killed;
  for(unsigned int i=0; i<indices_removal.size(); i++) {
    bool is_overlap = false;
    for(unsigned int j=0; j<indices_static.size(); j++) {
      // Don't remove an object against itself (for electron-electron overlap)
      if((type_removal == type_static) && (i == j)) continue;

      TLorentzVector object1;
      TLorentzVector object2;
      // Fill the TLV with the corrected values. Different functions for each type of particle.
      if      (type_removal == 0) object1 = m_SUSYObjDef->GetElecTLV(indices_removal[i]);
      else if (type_removal == 1) object1 = m_SUSYObjDef->GetMuonTLV(indices_removal[i]);
      else if (type_removal == 3) object1 = m_SUSYObjDef->GetJetTLV (indices_removal[i]);
      else                        object1 = m_SUSYObjDef->GetTauTLV (indices_removal[i]);

      if      (type_static  == 0) object2 = m_SUSYObjDef->GetElecTLV(indices_static [j]);
      else if (type_static  == 1) object2 = m_SUSYObjDef->GetMuonTLV(indices_static [j]);
      else if (type_static  == 3) object2 = m_SUSYObjDef->GetJetTLV (indices_static [j]);
      else                        object2 = m_SUSYObjDef->GetTauTLV (indices_static [j]);

      if (type_removal == type_static && remove_softer) {
	if ( (object1.E()/cosh(object1.Eta())) > (object2.E()/cosh(object2.Eta()))) {
	  continue;    // remove lowest Et
	}
	if(object1.DeltaR(object2) <= dr) {
          MSG_DEBUG("Killer deltaR for index " << indices_removal[i] 
                    << " is " << object1.DeltaR(object2) << " (softer object)");
	  is_overlap = true;
          break;
	}
      } else {
	if(object1.DeltaR(object2) <= dr) {
          MSG_DEBUG("Killer deltaR for index " << indices_removal[i] 
                   << " is " << object1.DeltaR(object2));
	  is_overlap = true;
          break;
	}
      }
    }
    if(is_overlap) {
      killed.push_back(indices_removal[i]);
    }
  }
  for(unsigned int i=0; i<killed.size(); i++) {
    MSG_DEBUG("Removing object type " << type_removal << " index " 
             << killed[i] << " due to object type " << type_static);
    // pop from indices_removal
    TransferIndexFromVecToVec(killed[i],indices_removal,indices_dumpto);
  }
  return;
}

//-----------------------------------------------------------------------------
bool PSL::SUSYObjDefAlg::isSignalJet(int jet_i){
    return (isCentralLightJet(jet_i)
            || isCentralBJet(jet_i)
            || isForwardJet(jet_i));
}

//-----------------------------------------------------------------------------
bool PSL::SUSYObjDefAlg::isCentralLightJet(int i){
  if(m_SUSYObjDef->GetJetTLV(i).Pt() < jet_central_ptmin) return false;
  if(fabs(m_EDM->Get(D3PD::jet_AntiKt4LCTopo_constscale_eta)->at(i)) > jet_central_eta_max)
    return false;
  if(m_EDM->Get(D3PD::jet_AntiKt4LCTopo_flavor_weight_MV1)->at(i) > jet_central_mv1)
    return false;
  if(!JetPassesJvfRequirement(i)) return false;
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::SUSYObjDefAlg::isForwardJet(int i){
  if(m_SUSYObjDef->GetJetTLV(i).Pt() < jet_forward_ptmin) return false;
  if(fabs(m_EDM->Get(D3PD::jet_AntiKt4LCTopo_constscale_eta)->at(i)) < jet_forward_eta_min)
    return false;
  if(fabs(m_EDM->Get(D3PD::jet_AntiKt4LCTopo_constscale_eta)->at(i)) > jet_forward_eta_max)
    return false;
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::SUSYObjDefAlg::isCentralBJet(int i)
{
  if(m_SUSYObjDef->GetJetTLV(i).Pt() < jet_bjet_ptmin) return false;
  if(fabs(m_EDM->Get(D3PD::jet_AntiKt4LCTopo_constscale_eta)->at(i)) > jet_forward_eta_max)
    return false;
  if(m_EDM->Get(D3PD::jet_AntiKt4LCTopo_flavor_weight_MV1)->at(i) < jet_bjet_mv1) return false;
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::SUSYObjDefAlg::JetPassesJvfRequirement(int i){
  // See https://twiki.cern.ch/twiki/bin/view/AtlasProtected/JVFUncertaintyTool
  TLorentzVector jtlv = m_SUSYObjDef->GetJetTLV(i);
  float detector_eta = m_EDM->Get(D3PD::jet_AntiKt4LCTopo_constscale_eta)->at(i);
  if (jtlv.Pt() > 50000.) return true;
  if(fabs(detector_eta) > jet_central_jvfeta_max)
    return true;
  float jvfcut_down = 0; 
  if (jet_central_jvfcut != 0.) // For jvf cut wp 0 there is no down variation.
    m_toolsd3pd->m_jvfTool->getJVFcut(jet_central_jvfcut
                                      ,false /* isPU, always set to false */
                                      ,jtlv.Pt()
                                      ,detector_eta
                                      ,false /* is up */
                                      );
  float jvfcut_up = m_toolsd3pd->m_jvfTool->getJVFcut(jet_central_jvfcut
                                                      ,false /* isPU, always set to false */
                                                      ,jtlv.Pt()
                                                      ,detector_eta
                                                      ,true /* is up */
                                                      );
  MSG_VERBOSE("To avoid warnings: jvf down: " << jvfcut_down << " up: " << jvfcut_up);
  // Currently only the nominal variation is returned.
  return fabs(m_EDM->Get(D3PD::jet_AntiKt4LCTopo_jvtxf)->at(i)) > jet_central_jvfcut;
}

//-----------------------------------------------------------------------------
void PSL::SUSYObjDefAlg::FindSignalJets(void){
  std::vector<int> killed;
  for(unsigned int ii=0;ii<m_evtdef.jet.passed_so_far.size();++ii){
    int i = m_evtdef.jet.passed_so_far[ii];
    bool iscentrallight = isCentralLightJet(i);
    bool isb = isCentralBJet(i);
    bool isforward = isForwardJet(i);
    if (!(iscentrallight || isb || isforward)){
      killed.push_back(i);
      continue;
    } else { 
      //m_evtdef.jet.i_sig.push_back(i); // done elsewhere!
      m_evtdef.m_iscentrallight[i] = iscentrallight;
      m_evtdef.m_isbjet[i] = isb;
      m_evtdef.m_isforward[i] = isforward;
    }
  }
  for(unsigned int i=0; i<killed.size(); i++) {
    TransferIndexFromVecToVec(killed[i],m_evtdef.jet.passed_so_far,m_evtdef.jet.removed_signal);
  }
}

//-----------------------------------------------------------------------------
void PSL::SUSYObjDefAlg::FindSignalElectrons(void){
  std::vector<int> killed;
  for(unsigned int ii=0;ii<m_evtdef.ele.passed_so_far.size();++ii){
    int i = m_evtdef.ele.passed_so_far[ii];
    bool passed = true;
    if (ele_id == 0 && !m_EDM->Get(D3PD::el_loosePP)->at(i)) passed = false;
    else if (ele_id == 1 && !m_EDM->Get(D3PD::el_mediumPP)->at(i)) passed = false;
    else if (ele_id == 2 && !m_EDM->Get(D3PD::el_tightPP)->at(i)) passed = false;
    else if (m_EDM->EleD0Significance(i) > ele_d0_max) passed = false;
    else if (m_EDM->EleZ0(i)/sin(m_evtdef.ele.tlv[i].Theta()) > ele_z0_max) passed = false;
    else if (m_EDM->EleTrkIsolation(i) > ele_trkiso_max) passed = false;
    else if (m_EDM->EleCaloIsolation(i) > ele_caloiso_max) passed = false;
    if (!passed) killed.push_back(i);
    else m_evtdef.ele.i_sig.push_back(i);
  }
  for(unsigned int i=0; i<killed.size(); i++) {
    TransferIndexFromVecToVec(killed[i],m_evtdef.ele.passed_so_far,m_evtdef.ele.removed_signal);
  }
  return;
}

//-----------------------------------------------------------------------------
void PSL::SUSYObjDefAlg::FindSignalMuons(void){
  std::vector<int> killed;
  for(unsigned int ii=0;ii<m_evtdef.mu.passed_so_far.size();++ii){
    int i = m_evtdef.mu.passed_so_far[ii];
    bool passed = true;
    if (m_EDM->MuonD0Significance(i) > mu_d0_max) passed = false;
    else if (m_EDM->MuonZ0(i)/sin(m_evtdef.mu.tlv[i].Theta()) > mu_z0_max) passed = false;
    else if (m_EDM->MuonTrkIsolation(i) > mu_trkiso_max) passed = false;
    else if (m_EDM->MuonCaloIsolation(i) > mu_caloiso_max) passed = false;
    if (!passed) killed.push_back(i);
    else m_evtdef.mu.i_sig.push_back(i);
  }
  for(unsigned int i=0; i<killed.size(); i++) {
    TransferIndexFromVecToVec(killed[i],m_evtdef.mu.passed_so_far,m_evtdef.mu.removed_signal);
  }
  return;
}

//-----------------------------------------------------------------------------
void PSL::SUSYObjDefAlg::FindSignalTaus(void){
  std::vector<int> killed;
  for(unsigned int ii=0;ii<m_evtdef.tau.passed_so_far.size();++ii){
    int i = m_evtdef.tau.passed_so_far[ii];
    if (!m_SUSYObjDef->FillTau(i,m_EDM->Get(D3PD::tau_pt              )->at(i)
                               ,m_EDM->Get(D3PD::tau_eta             )->at(i)
                               ,m_EDM->Get(D3PD::tau_leadTrack_eta   )->at(i)
                               ,m_EDM->Get(D3PD::tau_phi             )->at(i)
                               ,m_EDM->Get(D3PD::tau_Et              )->at(i)
                               ,m_EDM->Get(D3PD::tau_charge          )->at(i)
                               ,m_EDM->Get(D3PD::tau_numTrack        )->at(i)
                               ,m_EDM->Get(D3PD::tau_JetBDTSigLoose  )->at(i)
                               ,m_EDM->Get(D3PD::tau_JetBDTSigMedium )->at(i)
                               ,m_EDM->Get(D3PD::tau_JetBDTSigTight  )->at(i)
                               ,m_EDM->Get(D3PD::tau_EleBDTLoose     )->at(i)
                               ,m_EDM->Get(D3PD::tau_EleBDTMedium    )->at(i)
                               ,m_EDM->Get(D3PD::tau_EleBDTTight     )->at(i)
                               ,m_EDM->Get(D3PD::tau_BDTEleScore     )->at(i)
                               ,m_EDM->Get(D3PD::tau_muonVeto        )->at(i)
                               ,SUSYTau::TauMedium
                               ,SUSYTau::TauLoose
                               ,SUSYTau::TauMedium
                               ,tau_ptmin*1000.
                               ,tau_eta_max
                               ,SystErr::NONE
                               ,false // isD3PD1512
                               ))
    {
      killed.push_back(i);
      continue;
    }
    else m_evtdef.tau.i_sig.push_back(i);
  }
  for(unsigned int i=0; i<killed.size(); i++) {
    TransferIndexFromVecToVec(killed[i],m_evtdef.tau.passed_so_far,m_evtdef.tau.removed_signal);
  }
}

//-----------------------------------------------------------------------------
void PSL::SUSYObjDefAlg::HandleBCH(void){
  // This runs over all of the i_bch_jet indices
  // If no runs have the given mu value, 0 is returned.
  // But the corresponding pileup weight for such events should be 0 as well...
  int RunNumber = 0;
  if (m_EDM->isMC()) {
    int seed = 314159+m_EDM->EventNumber+2718*(m_EDM->mc_channel_number);
    m_toolsd3pd->m_pileup->SetRandomSeed(seed);
    RunNumber = m_toolsd3pd->m_pileup->GetRandomRunNumber(m_EDM->RunNumber,m_EDM->averageIntPerXing);
  } else {
    RunNumber = m_EDM->RunNumber;
  }
  int lbn = 0;
  // The RunNumber could be set to 0 if the data is unrepresented. In which case the pileup tool
  // throws an error. But if the RunNumber is 0 then we do not need the lbn.
  if (RunNumber != 0){
    if (m_EDM->isMC()) lbn = m_toolsd3pd->m_pileup->GetRandomLumiBlockNumber(RunNumber);
    else lbn = m_EDM->lbn;
  }
  // I think doug checks this no matter what
  //m_EDM->SetEventVariable(vMuIsValid,(RunNumber == 0 ? 0 : 1));

  for(unsigned int ii=0;ii<m_evtdef.i_bch_jet.size();++ii){
    int i = m_evtdef.i_bch_jet[ii];
    TLorentzVector jtlv = m_SUSYObjDef->GetJetTLV(i);

    // below, will be killed by unrepresented mu above anyway.
    if (RunNumber == 0) m_evtdef.m_passBCH[i] = false; 
    else { // signal jets must pass bch
      // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/BCHCleaningTool
      // Options : 
      // passBCHCleaningTight
      // passBCHCleaningMedium (needs systematic uncertainties - last argument)
      m_evtdef.m_passBCH[i] =
        m_SUSYObjDef->passBCHCleaningMedium(RunNumber
                                            ,lbn
                                            ,jtlv.Eta()
                                            ,jtlv.Phi()
                                            ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_BCH_CORR_CELL)->at(i)
                                            ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_emfrac)->at(i)
                                            ,jtlv.Pt()
                                            ,0 // systematic for medium (-1, 0, 1)
                                            );
    }
  }
}

//-----------------------------------------------------------------------------
void PSL::SUSYObjDefAlg::HandleBadJet(void){
  for(unsigned int ii=0;ii<m_evtdef.jet.passed_so_far.size();++ii){
    int i = m_evtdef.jet.passed_so_far[ii];
    TLorentzVector jtlv = m_SUSYObjDef->GetJetTLV(i);
    bool is_bad = JetID::isBadJet(JetID::VeryLooseBad
                                  ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_emfrac        )->at(i)
                                  ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_hecf          )->at(i)
                                  ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_LArQuality    )->at(i)
                                  ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_HECQuality    )->at(i)
                                  ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_Timing        )->at(i)
                                  ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_sumPtTrk_pv0_500MeV)->at(i)/1000.
                                  ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_constscale_eta)->at(i)
                                  ,m_SUSYObjDef->GetJetTLV(i).Pt()/1000.
                                  ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_fracSamplingMax)->at(i)
                                  ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_NegativeE)->at(i)
                                  ,m_EDM->Get(D3PD::jet_AntiKt4LCTopo_AverageLArQF)->at(i)
                                  //,m_EDM->Get(D3PD::)->at(i)
                                  );
    MSG_DEBUG("Jet pt: " << jtlv.Pt());
    m_evtdef.m_isbadjet[i] = is_bad; // .push_back(is_bad);
  }
  // std::cout << "isBadJet:";
  // for(unsigned int i=0;i<m_jet_isbadjet.size();++i){
  //   std::cout << (m_jet_isbadjet[i] ? " true" : " false");
  // }
  // std::cout << std::endl;
}

#endif // ifndef SKIP_D3PD_COMPILATION
