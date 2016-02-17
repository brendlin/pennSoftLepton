#include "pennSoftLepton/D3PDVariables.h"
#ifndef SKIP_D3PD_COMPILATION

//-----------------------------------------------------------------------------
std::string PSL::ConvertD3PDVarToStr(D3PD::Var_UInt_t v){
  switch(v) 
  {
  case D3PD::EventNumber        : return "EventNumber"       ;
  case D3PD::RunNumber          : return "RunNumber"         ;
  case D3PD::mc_channel_number  : return "mc_channel_number" ;
  case D3PD::larError           : return "larError"          ;
  case D3PD::tileError          : return "tileError"         ;
  case D3PD::lbn                : return "lbn"               ;
  case D3PD::trig_DB_SMK        : return "trig_DB_SMK"       ;
  default: break; // N_UInt_t
  }
  return "none";
}
//-----------------------------------------------------------------------------
std::string PSL::ConvertD3PDVarToStr(D3PD::Var_Int v){
  switch(v)
  {
  case D3PD::el_n                : return "el_n"                ;
  case D3PD::mu_staco_n          : return "mu_staco_n"          ;
  case D3PD::jet_AntiKt4LCTopo_n : return "jet_AntiKt4LCTopo_n" ;
  case D3PD::tau_n  : return "tau_n" ;
  case D3PD::el_MET_Egamma10NoTau_n  : return "el_MET_Egamma10NoTau_n" ;
  case D3PD::trig_EF_el_n  : return "trig_EF_el_n" ;
  case D3PD::trig_Nav_n  : return "trig_Nav_n" ;
  case D3PD::trig_EF_trigmuonef_n  : return "trig_EF_trigmuonef_n" ;
  default: break; // N_int
  }
  return "none";
}
//-----------------------------------------------------------------------------
std::string PSL::ConvertD3PDVarToStr(D3PD::Var_Bool_t v){
  switch(v)
  {
  case D3PD::EF_xe80_tclcw  : return "EF_xe80_tclcw" ;
  case D3PD::EF_e24vhi_medium1  : return "EF_e24vhi_medium1" ;
  case D3PD::EF_e60_medium1  : return "EF_e60_medium1" ;
  case D3PD::EF_2e12Tvh_loose1  : return "EF_2e12Tvh_loose1" ;
  case D3PD::EF_e24vh_medium1_e7_medium1  : return "EF_e24vh_medium1_e7_medium1" ;
  case D3PD::EF_e18vh_medium1_2e7T_medium1  : return "EF_e18vh_medium1_2e7T_medium1" ;
  case D3PD::EF_mu24i_tight  : return "EF_mu24i_tight" ;
  case D3PD::EF_2mu13  : return "EF_2mu13" ;
  case D3PD::EF_mu18_tight_mu8_EFFS  : return "EF_mu18_tight_mu8_EFFS" ;
  case D3PD::EF_3mu6  : return "EF_3mu6" ;
  case D3PD::EF_mu18_tight_2mu4_EFFS  : return "EF_mu18_tight_2mu4_EFFS" ;
  case D3PD::EF_mu18_tight_e7_medium1  : return "EF_mu18_tight_e7_medium1" ;
  case D3PD::EF_e12Tvh_medium1_mu8  : return "EF_e12Tvh_medium1_mu8" ;
  case D3PD::EF_2e7T_medium1_mu6  : return "EF_2e7T_medium1_mu6" ;
  case D3PD::EF_e7T_medium1_2mu6  : return "EF_e7T_medium1_2mu6" ;
  default: break; // N_Bool_t
  }
  return "none";
}
//-----------------------------------------------------------------------------
std::string PSL::ConvertD3PDVarToStr(D3PD::Var_float v){
  switch(v)
  {
  case D3PD::MET_RefFinal_et                   : return "MET_RefFinal_et"                  ;
  case D3PD::MET_RefFinal_etx                  : return "MET_RefFinal_etx"                 ;
  case D3PD::MET_RefFinal_ety                  : return "MET_RefFinal_ety"                 ;
  case D3PD::MET_RefFinal_phi                  : return "MET_RefFinal_phi"                 ;
  case D3PD::MET_RefFinal_sumet                : return "MET_RefFinal_sumet"               ;
  case D3PD::actualIntPerXing                  : return "actualIntPerXing"                 ;
  case D3PD::averageIntPerXing                 : return "averageIntPerXing"                ;
  case D3PD::Eventshape_rhoKt4LC               : return "Eventshape_rhoKt4LC"              ;
  case D3PD::MET_CellOut_Eflow_STVF_etx        : return "MET_CellOut_Eflow_STVF_etx"       ;
  case D3PD::MET_CellOut_Eflow_STVF_ety        : return "MET_CellOut_Eflow_STVF_ety"       ;
  case D3PD::MET_CellOut_Eflow_STVF_sumet      : return "MET_CellOut_Eflow_STVF_sumet"     ;
  case D3PD::MET_Egamma10NoTau_RefGamma_etx    : return "MET_Egamma10NoTau_RefGamma_etx"   ;
  case D3PD::MET_Egamma10NoTau_RefGamma_ety    : return "MET_Egamma10NoTau_RefGamma_ety"   ;
  case D3PD::MET_Egamma10NoTau_RefGamma_sumet  : return "MET_Egamma10NoTau_RefGamma_sumet" ;
  case D3PD::MET_Egamma10NoTau_CellOut_etx     : return "MET_Egamma10NoTau_CellOut_etx"    ;
  case D3PD::MET_Egamma10NoTau_CellOut_ety     : return "MET_Egamma10NoTau_CellOut_ety"    ;
  case D3PD::MET_Egamma10NoTau_CellOut_sumet   : return "MET_Egamma10NoTau_CellOut_sumet"  ;
  case D3PD::mc_event_weight  : return "mc_event_weight" ;
  default: break; // N_float
  }
  return "none";
}
//-----------------------------------------------------------------------------
std::string PSL::ConvertD3PDVarToStr(D3PD::Var_vector_float v){
  switch(v) 
  {
  case D3PD::el_E               : return "el_E"              ;
  case D3PD::mu_staco_z0_exPV   : return "mu_staco_z0_exPV"  ;
  case D3PD::mu_staco_d0_exPV   : return "mu_staco_d0_exPV"  ;
  case D3PD::jet_AntiKt4LCTopo_pt  : return "jet_AntiKt4LCTopo_pt" ;
  case D3PD::jet_AntiKt4LCTopo_eta  : return "jet_AntiKt4LCTopo_eta" ;
  case D3PD::jet_AntiKt4LCTopo_phi  : return "jet_AntiKt4LCTopo_phi" ;
  case D3PD::jet_AntiKt4LCTopo_E  : return "jet_AntiKt4LCTopo_E" ;
  case D3PD::jet_AntiKt4LCTopo_constscale_eta  : return "jet_AntiKt4LCTopo_constscale_eta" ;
  case D3PD::jet_AntiKt4LCTopo_constscale_phi  : return "jet_AntiKt4LCTopo_constscale_phi" ;
  case D3PD::jet_AntiKt4LCTopo_constscale_E  : return "jet_AntiKt4LCTopo_constscale_E" ;
  case D3PD::jet_AntiKt4LCTopo_constscale_m  : return "jet_AntiKt4LCTopo_constscale_m" ;
  case D3PD::jet_AntiKt4LCTopo_ActiveAreaPx  : return "jet_AntiKt4LCTopo_ActiveAreaPx" ;
  case D3PD::jet_AntiKt4LCTopo_ActiveAreaPy  : return "jet_AntiKt4LCTopo_ActiveAreaPy" ;
  case D3PD::jet_AntiKt4LCTopo_ActiveAreaPz  : return "jet_AntiKt4LCTopo_ActiveAreaPz" ;
  case D3PD::jet_AntiKt4LCTopo_ActiveAreaE  : return "jet_AntiKt4LCTopo_ActiveAreaE" ;
  case D3PD::jet_AntiKt4LCTopo_emfrac  : return "jet_AntiKt4LCTopo_emfrac" ;
  case D3PD::jet_AntiKt4LCTopo_hecf  : return "jet_AntiKt4LCTopo_hecf" ;
  case D3PD::jet_AntiKt4LCTopo_LArQuality  : return "jet_AntiKt4LCTopo_LArQuality" ;
  case D3PD::jet_AntiKt4LCTopo_HECQuality  : return "jet_AntiKt4LCTopo_HECQuality" ;
  case D3PD::jet_AntiKt4LCTopo_AverageLArQF  : return "jet_AntiKt4LCTopo_AverageLArQF" ;
  case D3PD::jet_AntiKt4LCTopo_Timing  : return "jet_AntiKt4LCTopo_Timing" ;
  case D3PD::jet_AntiKt4LCTopo_fracSamplingMax  : return "jet_AntiKt4LCTopo_fracSamplingMax" ;
  case D3PD::jet_AntiKt4LCTopo_NegativeE  : return "jet_AntiKt4LCTopo_NegativeE" ;
  case D3PD::jet_AntiKt4LCTopo_jvtxf  : return "jet_AntiKt4LCTopo_jvtxf" ;
  case D3PD::jet_AntiKt4LCTopo_BCH_CORR_CELL  : return "jet_AntiKt4LCTopo_BCH_CORR_CELL" ;
  case D3PD::jet_AntiKt4LCTopo_flavor_weight_MV1  : return "jet_AntiKt4LCTopo_flavor_weight_MV1" ;
  case D3PD::jet_AntiKt4LCTopo_sumPtTrk_pv0_500MeV  : return "jet_AntiKt4LCTopo_sumPtTrk_pv0_500MeV" ;
  case D3PD::el_eta  : return "el_eta" ;
  case D3PD::el_phi  : return "el_phi" ;
  case D3PD::el_cl_eta  : return "el_cl_eta" ;
  case D3PD::el_cl_phi  : return "el_cl_phi" ;
  case D3PD::el_cl_E  : return "el_cl_E" ;
  case D3PD::el_tracketa  : return "el_tracketa" ;
  case D3PD::el_trackphi  : return "el_trackphi" ;
  case D3PD::mu_staco_pt  : return "mu_staco_pt" ;
  case D3PD::mu_staco_eta  : return "mu_staco_eta" ;
  case D3PD::mu_staco_phi  : return "mu_staco_phi" ;
  case D3PD::mu_staco_me_qoverp_exPV  : return "mu_staco_me_qoverp_exPV" ;
  case D3PD::mu_staco_id_qoverp_exPV  : return "mu_staco_id_qoverp_exPV" ;
  case D3PD::mu_staco_me_theta_exPV  : return "mu_staco_me_theta_exPV" ;
  case D3PD::mu_staco_id_theta_exPV  : return "mu_staco_id_theta_exPV" ;
  case D3PD::mu_staco_id_theta  : return "mu_staco_id_theta" ;
  case D3PD::mu_staco_charge  : return "mu_staco_charge" ;
  case D3PD::tau_pt  : return "tau_pt" ;
  case D3PD::tau_eta  : return "tau_eta" ;
  case D3PD::tau_leadTrack_eta  : return "tau_leadTrack_eta" ;
  case D3PD::tau_phi  : return "tau_phi" ;
  case D3PD::tau_Et  : return "tau_Et" ;
  case D3PD::tau_charge  : return "tau_charge" ;
  case D3PD::tau_BDTEleScore  : return "tau_BDTEleScore" ;
  case D3PD::jet_AntiKt4LCTopo_BCH_CORR_JET  : return "jet_AntiKt4LCTopo_BCH_CORR_JET" ;
  case D3PD::el_charge  : return "el_charge" ;
  case D3PD::mu_staco_ms_qoverp  : return "mu_staco_ms_qoverp" ;
  case D3PD::mu_staco_ms_theta  : return "mu_staco_ms_theta" ;
  case D3PD::mu_staco_ms_phi  : return "mu_staco_ms_phi" ;
  case D3PD::mu_staco_energyLossPar  : return "mu_staco_energyLossPar" ;
  case D3PD::trig_EF_el_eta  : return "trig_EF_el_eta" ;
  case D3PD::trig_EF_el_phi  : return "trig_EF_el_phi" ;
  case D3PD::trig_EF_el_Et  : return "trig_EF_el_Et" ;
  case D3PD::trig_L2_combmuonfeature_eta  : return "trig_L2_combmuonfeature_eta" ;
  case D3PD::trig_L2_combmuonfeature_phi  : return "trig_L2_combmuonfeature_phi" ;
  case D3PD::trig_L2_muonfeature_eta  : return "trig_L2_muonfeature_eta" ;
  case D3PD::trig_L2_muonfeature_phi  : return "trig_L2_muonfeature_phi" ;
  case D3PD::trig_L1_mu_eta  : return "trig_L1_mu_eta" ;
  case D3PD::trig_L1_mu_phi  : return "trig_L1_mu_phi" ;
  case D3PD::el_trackIPEstimate_d0_unbiasedpvunbiased  : return "el_trackIPEstimate_d0_unbiasedpvunbiased" ;
  case D3PD::el_trackIPEstimate_sigd0_unbiasedpvunbiased  : return "el_trackIPEstimate_sigd0_unbiasedpvunbiased" ;
  case D3PD::el_trackIPEstimate_z0_unbiasedpvunbiased  : return "el_trackIPEstimate_z0_unbiasedpvunbiased" ;
  case D3PD::mu_staco_trackIPEstimate_d0_unbiasedpvunbiased  : return "mu_staco_trackIPEstimate_d0_unbiasedpvunbiased" ;
  case D3PD::mu_staco_trackIPEstimate_sigd0_unbiasedpvunbiased  : return "mu_staco_trackIPEstimate_sigd0_unbiasedpvunbiased" ;
  case D3PD::mu_staco_trackIPEstimate_z0_unbiasedpvunbiased  : return "mu_staco_trackIPEstimate_z0_unbiasedpvunbiased" ;
  case D3PD::el_ptcone30  : return "el_ptcone30" ;
  case D3PD::mu_staco_ptcone30  : return "mu_staco_ptcone30" ;
  case D3PD::mu_staco_ptcone30_trkelstyle  : return "mu_staco_ptcone30_trkelstyle" ;
  case D3PD::el_Etcone30  : return "el_Etcone30" ;
  case D3PD::el_Etcone40  : return "el_Etcone40" ;
  case D3PD::el_Etcone40_ED_corrected  : return "el_Etcone40_ED_corrected" ;
  case D3PD::el_ED_median  : return "el_ED_median" ;
  case D3PD::el_etas2  : return "el_etas2" ;
  case D3PD::el_etap  : return "el_etap" ;
  case D3PD::mu_staco_etcone30  : return "mu_staco_etcone30" ;
  default: break; // N_vector_float
  }
  return "none";
}
//-----------------------------------------------------------------------------
std::string PSL::ConvertD3PDVarToStr(D3PD::Var_vector_int v){
  switch(v) 
  {
  case D3PD::vxp_nTracks : return "vxp_nTracks";
  case D3PD::jet_AntiKt4LCTopo_flavor_truth_label  : return "jet_AntiKt4LCTopo_flavor_truth_label" ;
  case D3PD::jet_AntiKt4LCTopo_SamplingMax  : return "jet_AntiKt4LCTopo_SamplingMax" ;
  case D3PD::el_author  : return "el_author" ;
  case D3PD::el_mediumPP  : return "el_mediumPP" ;
  case D3PD::el_nPixHits  : return "el_nPixHits" ;
  case D3PD::el_nSCTHits  : return "el_nSCTHits" ;
  case D3PD::mu_staco_isCombinedMuon  : return "mu_staco_isCombinedMuon" ;
  case D3PD::mu_staco_isSegmentTaggedMuon  : return "mu_staco_isSegmentTaggedMuon" ;
  case D3PD::mu_staco_loose  : return "mu_staco_loose" ;
  case D3PD::mu_staco_nPixHits  : return "mu_staco_nPixHits" ;
  case D3PD::mu_staco_nPixelDeadSensors  : return "mu_staco_nPixelDeadSensors" ;
  case D3PD::mu_staco_nPixHoles  : return "mu_staco_nPixHoles" ;
  case D3PD::mu_staco_nSCTHits  : return "mu_staco_nSCTHits" ;
  case D3PD::mu_staco_nSCTDeadSensors  : return "mu_staco_nSCTDeadSensors" ;
  case D3PD::mu_staco_nSCTHoles  : return "mu_staco_nSCTHoles" ;
  case D3PD::mu_staco_nTRTHits  : return "mu_staco_nTRTHits" ;
  case D3PD::mu_staco_nTRTOutliers  : return "mu_staco_nTRTOutliers" ;
  case D3PD::tau_numTrack  : return "tau_numTrack" ;
  case D3PD::tau_JetBDTSigLoose  : return "tau_JetBDTSigLoose" ;
  case D3PD::tau_JetBDTSigMedium  : return "tau_JetBDTSigMedium" ;
  case D3PD::tau_JetBDTSigTight  : return "tau_JetBDTSigTight" ;
  case D3PD::tau_EleBDTLoose  : return "tau_EleBDTLoose" ;
  case D3PD::tau_EleBDTMedium  : return "tau_EleBDTMedium" ;
  case D3PD::tau_EleBDTTight  : return "tau_EleBDTTight" ;
  case D3PD::tau_muonVeto  : return "tau_muonVeto" ;
  case D3PD::trig_EF_el_EF_e24vhi_medium1  : return "trig_EF_el_EF_e24vhi_medium1" ;
  case D3PD::trig_EF_el_EF_e60_medium1  : return "trig_EF_el_EF_e60_medium1" ;
  case D3PD::trig_EF_el_EF_2e12Tvh_loose1  : return "trig_EF_el_EF_2e12Tvh_loose1" ;
  case D3PD::trig_EF_el_EF_e24vh_medium1_e7_medium1  : return "trig_EF_el_EF_e24vh_medium1_e7_medium1" ;
  case D3PD::trig_EF_el_EF_e18vh_medium1_2e7T_medium1  : return "trig_EF_el_EF_e18vh_medium1_2e7T_medium1" ;
  case D3PD::trig_EF_el_EF_e12Tvh_medium1_mu8  : return "trig_EF_el_EF_e12Tvh_medium1_mu8" ;
  case D3PD::trig_EF_el_EF_2e7T_medium1_mu6  : return "trig_EF_el_EF_2e7T_medium1_mu6" ;
  case D3PD::trig_EF_el_EF_e7T_medium1_2mu6  : return "trig_EF_el_EF_e7T_medium1_2mu6" ;
  case D3PD::trig_EF_trigmuonef_track_n  : return "trig_EF_trigmuonef_track_n" ;
  case D3PD::trig_EF_trigmuonef_EF_mu24i_tight  : return "trig_EF_trigmuonef_EF_mu24i_tight" ;
  case D3PD::trig_EF_trigmuonef_EF_2mu13  : return "trig_EF_trigmuonef_EF_2mu13" ;
  case D3PD::trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS  : return "trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS" ;
  case D3PD::trig_EF_trigmuonef_EF_mu6  : return "trig_EF_trigmuonef_EF_mu6" ;
  case D3PD::trig_EF_trigmuonef_EF_2mu6  : return "trig_EF_trigmuonef_EF_2mu6" ;
  case D3PD::trig_EF_trigmuonef_EF_mu18_tight_2mu4_EFFS  : return "trig_EF_trigmuonef_EF_mu18_tight_2mu4_EFFS" ;
  case D3PD::trig_EF_trigmuonef_EF_mu18_tight_e7_medium1  : return "trig_EF_trigmuonef_EF_mu18_tight_e7_medium1" ;
  case D3PD::trig_RoI_EF_mu_Muon_ROI  : return "trig_RoI_EF_mu_Muon_ROI" ;
  case D3PD::trig_RoI_L2_mu_CombinedMuonFeature  : return "trig_RoI_L2_mu_CombinedMuonFeature" ;
  case D3PD::trig_RoI_L2_mu_CombinedMuonFeatureStatus  : return "trig_RoI_L2_mu_CombinedMuonFeatureStatus" ;
  case D3PD::trig_RoI_L2_mu_MuonFeature  : return "trig_RoI_L2_mu_MuonFeature" ;
  case D3PD::trig_RoI_L2_mu_Muon_ROI  : return "trig_RoI_L2_mu_Muon_ROI" ;
  case D3PD::trig_EF_el_EF_e24vh_medium1  : return "trig_EF_el_EF_e24vh_medium1" ;
  case D3PD::trig_EF_el_EF_e7T_medium1  : return "trig_EF_el_EF_e7T_medium1" ;
  case D3PD::trig_EF_el_EF_e18vh_medium1  : return "trig_EF_el_EF_e18vh_medium1" ;
  case D3PD::el_loosePP  : return "el_loosePP" ;
  case D3PD::el_tightPP  : return "el_tightPP" ;
  default: break; // N_vector_int
  }
  return "none";
}
//-----------------------------------------------------------------------------
std::string PSL::ConvertD3PDVarToStr(D3PD::Var_vector_vector_UInt_t v){
  switch(v) 
  {
  case D3PD::jet_AntiKt4LCTopo_MET_Egamma10NoTau_statusWord  : return "jet_AntiKt4LCTopo_MET_Egamma10NoTau_statusWord" ;
  case D3PD::el_MET_Egamma10NoTau_statusWord  : return "el_MET_Egamma10NoTau_statusWord" ;
  default: break; // N_vector_vector_UInt_t
  }
  return "none";
}
//-----------------------------------------------------------------------------
std::string PSL::ConvertD3PDVarToStr(D3PD::Var_vector_vector_int v){
  switch(v) 
  {
  case D3PD::trig_EF_trigmuonef_track_MuonType  : return "trig_EF_trigmuonef_track_MuonType" ;
  case D3PD::trig_EF_trigmuonef_track_CB_hasCB  : return "trig_EF_trigmuonef_track_CB_hasCB" ;
  case D3PD::trig_Nav_chain_RoIType  : return "trig_Nav_chain_RoIType" ;
  case D3PD::trig_Nav_chain_RoIIndex  : return "trig_Nav_chain_RoIIndex" ;
  case D3PD::trig_RoI_EF_e_egammaContainer_egamma_Electrons  : return "trig_RoI_EF_e_egammaContainer_egamma_Electrons" ;
  case D3PD::trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus  : return "trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus" ;
  case D3PD::trig_RoI_EF_mu_TrigMuonEFInfoContainer  : return "trig_RoI_EF_mu_TrigMuonEFInfoContainer" ;
  case D3PD::trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus  : return "trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus" ;
  case D3PD::trig_RoI_EF_mu_TrigMuonEFIsolationContainer  : return "trig_RoI_EF_mu_TrigMuonEFIsolationContainer" ;
  case D3PD::trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus  : return "trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus" ;
  default: break; // N_vector_vector_int
  }
  return "none";
}
//-----------------------------------------------------------------------------
std::string PSL::ConvertD3PDVarToStr(D3PD::Var_vector_short v){
  switch(v) 
  {
  case D3PD::trig_Nav_chain_ChainId  : return "trig_Nav_chain_ChainId" ;
  default: break; // N_vector_short
  }
  return "none";
}
//-----------------------------------------------------------------------------
std::string PSL::ConvertD3PDVarToStr(D3PD::Var_vector_string v){
  switch(v) 
  {
  case D3PD::trig_L1_mu_thrName  : return "trig_L1_mu_thrName" ;
  default: break; // N_vector_string
  }
  return "none";
}
//-----------------------------------------------------------------------------
std::string PSL::ConvertD3PDVarToStr(D3PD::Var_vector_vector_float v){
  switch(v) 
  {
  case D3PD::jet_AntiKt4LCTopo_MET_Egamma10NoTau_wet  : return "jet_AntiKt4LCTopo_MET_Egamma10NoTau_wet" ;
  case D3PD::jet_AntiKt4LCTopo_MET_Egamma10NoTau_wpx  : return "jet_AntiKt4LCTopo_MET_Egamma10NoTau_wpx" ;
  case D3PD::jet_AntiKt4LCTopo_MET_Egamma10NoTau_wpy  : return "jet_AntiKt4LCTopo_MET_Egamma10NoTau_wpy" ;
  case D3PD::el_MET_Egamma10NoTau_wet  : return "el_MET_Egamma10NoTau_wet" ;
  case D3PD::el_MET_Egamma10NoTau_wpx  : return "el_MET_Egamma10NoTau_wpx" ;
  case D3PD::el_MET_Egamma10NoTau_wpy  : return "el_MET_Egamma10NoTau_wpy" ;
  case D3PD::trig_EF_trigmuonef_track_CB_pt  : return "trig_EF_trigmuonef_track_CB_pt" ;
  case D3PD::trig_EF_trigmuonef_track_CB_eta  : return "trig_EF_trigmuonef_track_CB_eta" ;
  case D3PD::trig_EF_trigmuonef_track_CB_phi  : return "trig_EF_trigmuonef_track_CB_phi" ;
  case D3PD::trig_EF_trigmuonef_track_SA_pt  : return "trig_EF_trigmuonef_track_SA_pt" ;
  case D3PD::trig_EF_trigmuonef_track_SA_eta  : return "trig_EF_trigmuonef_track_SA_eta" ;
  case D3PD::trig_EF_trigmuonef_track_SA_phi  : return "trig_EF_trigmuonef_track_SA_phi" ;
  case D3PD::trig_EF_trigmugirl_track_CB_pt  : return "trig_EF_trigmugirl_track_CB_pt" ;
  case D3PD::trig_EF_trigmugirl_track_CB_eta  : return "trig_EF_trigmugirl_track_CB_eta" ;
  case D3PD::trig_EF_trigmugirl_track_CB_phi  : return "trig_EF_trigmugirl_track_CB_phi" ;
  default: break; // N_vector_vector_float
  }
  return "none";
}
//-----------------------------------------------------------------------------
std::string PSL::ConvertD3PDVarToStr(D3PD::Var_vector_UInt_t v){
  switch(v) 
  {
  case D3PD::el_OQ  : return "el_OQ" ;
  default: break; // N_vector_UInt_t
  }
  return "none";
}

#endif // ifndef SKIP_D3PD_COMPILATION
