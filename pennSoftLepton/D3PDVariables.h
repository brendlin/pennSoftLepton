#include "pennSoftLepton/switch_d3pd_xaod.h"
#ifndef SKIP_D3PD_COMPILATION

#ifndef pennSoftLepton_D3PDVARIABLE_H
#define pennSoftLepton_D3PDVARIABLE_H

/**********************************************************************************
 * @Package: MonojetSoftLepton
 * @Class  : D3PDVariable
 * @Author : Rustem Ospanov, Doug Schaefer
 *
 * @Brief  :
 * 
 *  Enum for dataset keys (samples) and helper class for a set of samples
 * 
 **********************************************************************************/

// C/C++
#include <vector>
#include <string>
#include <iomanip>
#include <iostream>

#include <pennSoftLepton/Messaging.h>

/** \brief The PSL namespace */
namespace PSL
{
  //* \brief Enums for unique dataset samples. Used for naming and stuff. After a point they are overlapping. */
  namespace D3PD{
    enum Var_UInt_t { // these are not actually filled automatically
      EventNumber
      ,RunNumber
      ,mc_channel_number
      ,larError
      ,tileError
      ,lbn
      ,trig_DB_SMK
      ,N_UInt_t
    };
    enum Var_Int {
      el_n
      ,mu_staco_n
      ,jet_AntiKt4LCTopo_n
      ,tau_n
      ,el_MET_Egamma10NoTau_n
      ,trig_EF_el_n
      ,trig_Nav_n
      ,trig_EF_trigmuonef_n
      ,N_int
    };
    enum Var_Bool_t {
      EF_xe80_tclcw
      ,EF_e24vhi_medium1
      ,EF_e60_medium1
      ,EF_2e12Tvh_loose1
      ,EF_e24vh_medium1_e7_medium1
      ,EF_e18vh_medium1_2e7T_medium1
      ,EF_mu24i_tight
      ,EF_2mu13
      ,EF_mu18_tight_mu8_EFFS
      ,EF_3mu6
      ,EF_mu18_tight_2mu4_EFFS
      ,EF_mu18_tight_e7_medium1
      ,EF_e12Tvh_medium1_mu8
      ,EF_2e7T_medium1_mu6
      ,EF_e7T_medium1_2mu6
      ,N_Bool_t
    };
    enum Var_float { // these are not actually filled automatically
      MET_RefFinal_et
      ,MET_RefFinal_etx
      ,MET_RefFinal_ety
      ,MET_RefFinal_phi
      ,MET_RefFinal_sumet
      ,actualIntPerXing
      ,averageIntPerXing 
      ,Eventshape_rhoKt4LC
      ,MET_CellOut_Eflow_STVF_etx // probably wrong? do not use STVF
      ,MET_CellOut_Eflow_STVF_ety // probably wrong? do not use STVF
      ,MET_CellOut_Eflow_STVF_sumet // probably wrong? do not use STVF
      ,MET_Egamma10NoTau_RefGamma_etx
      ,MET_Egamma10NoTau_RefGamma_ety
      ,MET_Egamma10NoTau_RefGamma_sumet
      ,MET_Egamma10NoTau_CellOut_etx
      ,MET_Egamma10NoTau_CellOut_ety
      ,MET_Egamma10NoTau_CellOut_sumet
      ,mc_event_weight
      ,N_float
    };
    enum Var_vector_float {
      el_E
      ,mu_staco_z0_exPV
      ,mu_staco_d0_exPV
      ,jet_AntiKt4LCTopo_pt
      ,jet_AntiKt4LCTopo_eta
      ,jet_AntiKt4LCTopo_phi
      ,jet_AntiKt4LCTopo_E
      ,jet_AntiKt4LCTopo_constscale_eta
      ,jet_AntiKt4LCTopo_constscale_phi
      ,jet_AntiKt4LCTopo_constscale_E
      ,jet_AntiKt4LCTopo_constscale_m
      ,jet_AntiKt4LCTopo_ActiveAreaPx
      ,jet_AntiKt4LCTopo_ActiveAreaPy
      ,jet_AntiKt4LCTopo_ActiveAreaPz
      ,jet_AntiKt4LCTopo_ActiveAreaE
      ,jet_AntiKt4LCTopo_emfrac
      ,jet_AntiKt4LCTopo_hecf
      ,jet_AntiKt4LCTopo_LArQuality
      ,jet_AntiKt4LCTopo_HECQuality
      ,jet_AntiKt4LCTopo_AverageLArQF
      ,jet_AntiKt4LCTopo_Timing
      ,jet_AntiKt4LCTopo_fracSamplingMax
      ,jet_AntiKt4LCTopo_NegativeE
      ,jet_AntiKt4LCTopo_jvtxf
      ,jet_AntiKt4LCTopo_BCH_CORR_CELL
      ,jet_AntiKt4LCTopo_flavor_weight_MV1
      ,jet_AntiKt4LCTopo_sumPtTrk_pv0_500MeV
      ,el_eta
      ,el_phi
      ,el_cl_eta
      ,el_cl_phi
      ,el_cl_E
      ,el_tracketa
      ,el_trackphi
      ,mu_staco_pt
      ,mu_staco_eta
      ,mu_staco_phi
      ,mu_staco_me_qoverp_exPV
      ,mu_staco_id_qoverp_exPV
      ,mu_staco_me_theta_exPV
      ,mu_staco_id_theta_exPV
      ,mu_staco_id_theta
      ,mu_staco_charge
      ,tau_pt
      ,tau_eta
      ,tau_leadTrack_eta
      ,tau_phi
      ,tau_Et
      ,tau_charge
      ,tau_BDTEleScore
      ,jet_AntiKt4LCTopo_BCH_CORR_JET
      ,el_charge
      ,mu_staco_ms_qoverp
      ,mu_staco_ms_theta
      ,mu_staco_ms_phi
      ,mu_staco_energyLossPar
      ,trig_EF_el_eta
      ,trig_EF_el_phi
      ,trig_EF_el_Et
      ,trig_L2_combmuonfeature_eta
      ,trig_L2_combmuonfeature_phi
      ,trig_L2_muonfeature_eta
      ,trig_L2_muonfeature_phi
      ,trig_L1_mu_eta
      ,trig_L1_mu_phi
      ,el_trackIPEstimate_d0_unbiasedpvunbiased
      ,el_trackIPEstimate_sigd0_unbiasedpvunbiased
      ,el_trackIPEstimate_z0_unbiasedpvunbiased
      ,mu_staco_trackIPEstimate_d0_unbiasedpvunbiased
      ,mu_staco_trackIPEstimate_sigd0_unbiasedpvunbiased
      ,mu_staco_trackIPEstimate_z0_unbiasedpvunbiased
      ,el_ptcone30
      ,mu_staco_ptcone30
      ,mu_staco_ptcone30_trkelstyle
      ,el_Etcone30
      ,el_Etcone40
      ,el_Etcone40_ED_corrected
      ,el_ED_median
      ,el_etas2
      ,el_etap
      ,mu_staco_etcone30
      ,N_vector_float
    };
    enum Var_vector_int {
      vxp_nTracks
      ,jet_AntiKt4LCTopo_flavor_truth_label
      ,jet_AntiKt4LCTopo_SamplingMax
      ,el_author
      ,el_mediumPP
      ,el_nPixHits
      ,el_nSCTHits
      ,mu_staco_isCombinedMuon
      ,mu_staco_isSegmentTaggedMuon
      ,mu_staco_loose
      ,mu_staco_nPixHits
      ,mu_staco_nPixelDeadSensors
      ,mu_staco_nPixHoles
      ,mu_staco_nSCTHits
      ,mu_staco_nSCTDeadSensors
      ,mu_staco_nSCTHoles
      ,mu_staco_nTRTHits
      ,mu_staco_nTRTOutliers
      ,tau_numTrack
      ,tau_JetBDTSigLoose
      ,tau_JetBDTSigMedium
      ,tau_JetBDTSigTight
      ,tau_EleBDTLoose
      ,tau_EleBDTMedium
      ,tau_EleBDTTight
      ,tau_muonVeto
      ,trig_EF_el_EF_e24vhi_medium1
      ,trig_EF_el_EF_e60_medium1
      ,trig_EF_el_EF_2e12Tvh_loose1
      ,trig_EF_el_EF_e24vh_medium1_e7_medium1
      ,trig_EF_el_EF_e18vh_medium1_2e7T_medium1
      ,trig_EF_el_EF_e12Tvh_medium1_mu8
      ,trig_EF_el_EF_2e7T_medium1_mu6
      ,trig_EF_el_EF_e7T_medium1_2mu6
      ,trig_EF_trigmuonef_track_n
      ,trig_EF_trigmuonef_EF_mu24i_tight
      ,trig_EF_trigmuonef_EF_2mu13
      ,trig_EF_trigmuonef_EF_mu18_tight_mu8_EFFS
      ,trig_EF_trigmuonef_EF_mu6
      ,trig_EF_trigmuonef_EF_2mu6
      ,trig_EF_trigmuonef_EF_mu18_tight_2mu4_EFFS
      ,trig_EF_trigmuonef_EF_mu18_tight_e7_medium1
      ,trig_RoI_EF_mu_Muon_ROI
      ,trig_RoI_L2_mu_CombinedMuonFeature
      ,trig_RoI_L2_mu_CombinedMuonFeatureStatus
      ,trig_RoI_L2_mu_MuonFeature
      ,trig_RoI_L2_mu_Muon_ROI
      ,trig_EF_el_EF_e24vh_medium1
      ,trig_EF_el_EF_e7T_medium1
      ,trig_EF_el_EF_e18vh_medium1
      ,el_loosePP
      ,el_tightPP
      ,N_vector_int
    };
    enum Var_vector_vector_UInt_t {
      jet_AntiKt4LCTopo_MET_Egamma10NoTau_statusWord
      ,el_MET_Egamma10NoTau_statusWord
      ,N_vector_vector_UInt_t
    };
    enum Var_vector_vector_int {
      trig_EF_trigmuonef_track_MuonType
      ,trig_EF_trigmuonef_track_CB_hasCB
      ,trig_Nav_chain_RoIType
      ,trig_Nav_chain_RoIIndex
      ,trig_RoI_EF_e_egammaContainer_egamma_Electrons
      ,trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus
      ,trig_RoI_EF_mu_TrigMuonEFInfoContainer
      ,trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus
      ,trig_RoI_EF_mu_TrigMuonEFIsolationContainer
      ,trig_RoI_EF_mu_TrigMuonEFIsolationContainerStatus
      ,N_vector_vector_int
    };
    enum Var_vector_short {
      trig_Nav_chain_ChainId
      ,N_vector_short
    };
    enum Var_vector_string {
      trig_L1_mu_thrName
      ,N_vector_string
    };
    enum Var_vector_vector_float {
      jet_AntiKt4LCTopo_MET_Egamma10NoTau_wet
      ,jet_AntiKt4LCTopo_MET_Egamma10NoTau_wpx
      ,jet_AntiKt4LCTopo_MET_Egamma10NoTau_wpy
      ,el_MET_Egamma10NoTau_wet
      ,el_MET_Egamma10NoTau_wpx
      ,el_MET_Egamma10NoTau_wpy
      ,trig_EF_trigmuonef_track_CB_pt
      ,trig_EF_trigmuonef_track_CB_eta
      ,trig_EF_trigmuonef_track_CB_phi
      ,trig_EF_trigmuonef_track_SA_pt
      ,trig_EF_trigmuonef_track_SA_eta
      ,trig_EF_trigmuonef_track_SA_phi
      ,trig_EF_trigmugirl_track_CB_pt
      ,trig_EF_trigmugirl_track_CB_eta
      ,trig_EF_trigmugirl_track_CB_phi
      ,N_vector_vector_float
    };
    enum Var_vector_UInt_t { // these are not actually filled automatically
      el_OQ
      ,N_vector_UInt_t
    };
  }

  // fyi the jet prefix options are:
/* tau_jet_ */
/* jet_AntiKt4TopoEM_ */
/* jet_AntiKt6TopoEM_ */
/* jet_AntiKt4LCTopo_ */
/* jet_AntiKt6LCTopo_ */
/* eljet_ */
/* jet_AntiKt5LCTopo_ */
/* jet_AntiKt7LCTopo_ */
/* jet_AntiKt2LCTopo_ */
/* jet_AntiKt3LCTopo_ */
/* jet_AntiKt8LCTopo_ */
/* jet_AntiKt10LCTopo_ */
/* jet_AntiKt10LCTopoTrimmedPtFrac5SmallR30_ */
/* jet_AntiKt10LCTopoTrimmedSubjetsPtFrac5SmallR30_ */
/* jet_CamKt12LCTopo_ */
/* jet_CamKt12LCTopoTrimmedPtFrac5SmallR30_ */
/* jet_CamKt12LCTopoTrimmedSubjetsPtFrac5SmallR30_ */
/* jet_CamKt12LCTopoSplitFilteredmassFraction67minSplitR0_ */
/* jet_CamKt12LCTopoSplitFiltSubjetsmassFraction67minSplitR0_ */

  //* \brief map for dataset samples to Sample bitmap */
  //const std::map<PSL::Sample,std::bitset>& SampleBits(void);
  
  std::string ConvertD3PDVarToStr(D3PD::Var_UInt_t v);
  std::string ConvertD3PDVarToStr(D3PD::Var_Int v);
  std::string ConvertD3PDVarToStr(D3PD::Var_Bool_t v);
  std::string ConvertD3PDVarToStr(D3PD::Var_float v);
  std::string ConvertD3PDVarToStr(D3PD::Var_vector_float v);
  std::string ConvertD3PDVarToStr(D3PD::Var_vector_int v);
  std::string ConvertD3PDVarToStr(D3PD::Var_vector_vector_UInt_t v);
  std::string ConvertD3PDVarToStr(D3PD::Var_vector_vector_int v);
  std::string ConvertD3PDVarToStr(D3PD::Var_vector_short v);
  std::string ConvertD3PDVarToStr(D3PD::Var_vector_string v);
  std::string ConvertD3PDVarToStr(D3PD::Var_vector_vector_float v);
  std::string ConvertD3PDVarToStr(D3PD::Var_vector_UInt_t v);
  
  /** \brief Match vars to each other */
  /* void PrintD3PDVariable(D3PDVariable v); */
  /* void PrintD3PDVariables(std::vector<D3PDVariable> v); */
  
}

#endif

#endif // ifndef SKIP_D3PD_COMPILATION
