#include "pennSoftLepton/ObjectSelectionBase.h"

ClassImp(PSL::ObjectSelectionBase)

//-----------------------------------------------------------------------------
PSL::ObjectSelectionBase::ObjectSelectionBase() : AlgBase()
  ,ele_ptmin_base         (7.     )
  ,ele_ptmin              (20.    )
  ,ele_ptmin_antiid       (20.    )
  ,mu_ptmin_base          (6.     )
  ,mu_ptmin               (20.    )
  ,mu_ptmin_antiid        (20.    )
  ,tau_ptmin              (20.    )
  ,jet_ptmin              (25.    )
  ,ele_eta_max            (2.47   )
  ,mu_eta_max             (2.5    )
  ,tau_eta_max            (2.47   )
  ,jet_eta_max            (4.5    )
  ,ele_d0_max             (5.0    ) // In "NoExp" this is 1 (and refers to d0, not signif.)
  ,ele_d0_max_antiid      (-1.    )
  ,ele_d0_min_antiid      (-1.    )
  ,ele_z0_max             (0.5    ) // In "NoExp" this is 2 (and refers to z0, not z0sintheta)
  ,ele_z0_max_antiid      (-1.    )
  ,ele_z0_min_antiid      (-1.    )
  ,ele_isowp              (PSL::AnaIso::Gradient)
  ,ele_isowp_base         (PSL::AnaIso::GradientLoose)
  ,ele_isowp_fail_antiid  (PSL::AnaIso::None)
  ,ele_isowp_pass_antiid  (PSL::AnaIso::None)
  ,ele_trkisotype         (PSL::AnaIso::None)
  ,ele_caloisotype        (PSL::AnaIso::None)
  ,ele_trkiso_max         (0.16   )
  ,ele_trkiso_max_antiid  (-1.    )
  ,ele_trkiso_min_antiid  (-1.    )
  ,ele_caloiso_max        (0.18   )
  ,ele_caloiso_max_antiid (-1.    )
  ,ele_caloiso_min_antiid (-1.    )
  ,ele_id_base            (ElectronID::LooseLLH)
  ,ele_id                 (ElectronID::TightLLH)
  ,ele_id_fail_antiid     (ElectronID::TightLLH)
  ,ele_id_pass_antiid     (ElectronID::VeryLooseLLH)
  ,ele_antiid_or_d0       (false)
  ,ele_antiid_or_z0       (false)
  ,ele_antiid_or_iso      (false)
  ,ele_antiid_or_id       (false)
  ,mu_antiid_or_d0        (false)
  ,mu_antiid_or_z0        (false)
  ,mu_antiid_or_iso       (false)
  ,zele_ptmin              (15.    )
  ,zele_d0_max             (5.0    )
  ,zele_z0_max             (0.5    )
  ,zele_isowp              (PSL::AnaIso::GradientLoose)
  ,zele_trkisotype         (PSL::AnaIso::None)
  ,zele_caloisotype        (PSL::AnaIso::None)
  ,zele_trkiso_max         (0.16   )
  ,zele_caloiso_max        (0.18   )
  ,zele_id                 (ElectronID::MediumLLH)
  ,mu_d0_max              (3.0    )
  ,mu_d0_max_antiid       (-1.    )
  ,mu_d0_min_antiid       (-1.    )
  ,mu_z0_max              (0.5    )
  ,mu_z0_max_antiid       (-1.    )
  ,mu_z0_min_antiid       (-1.    )
  ,mu_isowp               (PSL::AnaIso::GradientLoose)
  ,mu_isowp_base          (PSL::AnaIso::GradientLoose)
  ,mu_isowp_fail_antiid   (PSL::AnaIso::None)
  ,mu_isowp_pass_antiid   (PSL::AnaIso::None)
  ,mu_trkisotype          (PSL::AnaIso::None)
  ,mu_caloisotype         (PSL::AnaIso::None)
  ,mu_trkiso_max          (0.12   )
  ,mu_trkiso_max_antiid   (-1.    )
  ,mu_trkiso_min_antiid   (-1.    )
  ,mu_caloiso_max         (0.12   )
  ,mu_caloiso_max_antiid  (-1.    )
  ,mu_caloiso_min_antiid  (-1.    )
  ,zmu_ptmin               (15.    )
  ,zmu_d0_max              (3.0    )
  ,zmu_z0_max              (0.5    )
  ,zmu_isowp               (PSL::AnaIso::GradientLoose)
  ,zmu_trkisotype          (PSL::AnaIso::None)
  ,zmu_caloisotype         (PSL::AnaIso::None)
  ,zmu_trkiso_max          (0.12   )
  ,zmu_caloiso_max         (0.12   )
  // --- Muon ID variables
  ,mu_id_base             (xAOD::Muon::Loose)
  ,mu_id_pass_antiid      (xAOD::Muon::Loose)
  ,mu_id                  (xAOD::Muon::Medium)
  ,zmu_id                 (xAOD::Muon::Medium)
  // ---
  ,jet_forward_eta_min    (2.4    )
  ,jet_forward_eta_max    (4.5    )
  ,jet_forward_ptmin      (30.    )
  ,jet_bjet_ptmin         (20.    )
  ,jet_bjet_mv1           (0.7892 ) // 60%: 0.9827 70%: 0.7892. 80%: 0.3511
  // see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/BtagAnalysis172#LC_Jets
  ,jet_central_jvfeta_max (2.4    )
  ,jet_central_jvfpt_max  (60.    )
  ,jet_central_eta_max    (2.8    )
  ,jet_central_ptmin      (20.    )
  ,jet_central_mv1        (0.3511 )
  ,do_overlapremove       (true)
  ,m_skipsf               (false)
{
  MSG_DEBUG("Constructor");
}

//-----------------------------------------------------------------------------
bool PSL::ObjectSelectionBase::init(void){

  // isolation stuff
  if (!CheckIsolationConfigurationValidity()) return false;
  
  if (ele_isowp != PSL::AnaIso::None) {
    m_EDM->m_el_defaulttrkiso = ele_isowp;
    m_EDM->m_el_defaultcaloiso = ele_isowp;
  } else {
    m_EDM->m_el_defaulttrkiso = ele_trkisotype;
    m_EDM->m_el_defaultcaloiso = ele_caloisotype;
  }

  if (mu_isowp != PSL::AnaIso::None) {
    m_EDM->m_mu_defaulttrkiso = mu_isowp;
    m_EDM->m_mu_defaultcaloiso = mu_isowp;
  } else {
    m_EDM->m_mu_defaulttrkiso = mu_trkisotype;
    m_EDM->m_mu_defaultcaloiso = mu_caloisotype;
  }

  if (ele_eta_max             <= 0) ele_eta_max             = FLT_MAX;
  if (mu_eta_max              <= 0) mu_eta_max              = FLT_MAX;
  if (tau_eta_max             <= 0) tau_eta_max             = FLT_MAX;
  if (jet_eta_max             <= 0) jet_eta_max             = FLT_MAX;
  if (ele_d0_max              <= 0) ele_d0_max              = FLT_MAX;
  if (ele_d0_max_antiid       <= 0) ele_d0_max_antiid       = FLT_MAX;
  if (ele_z0_max              <= 0) ele_z0_max              = FLT_MAX;
  if (ele_z0_max_antiid       <= 0) ele_z0_max_antiid       = FLT_MAX;
  if (ele_trkiso_max          <= 0) ele_trkiso_max          = FLT_MAX;
  if (ele_trkiso_max_antiid   <= 0) ele_trkiso_max_antiid   = FLT_MAX;
  if (ele_caloiso_max         <= 0) ele_caloiso_max         = FLT_MAX;
  if (ele_caloiso_max_antiid  <= 0) ele_caloiso_max_antiid  = FLT_MAX;
  if (zele_d0_max             <= 0) zele_d0_max             = FLT_MAX;
  if (zele_z0_max             <= 0) zele_z0_max             = FLT_MAX;
  if (zele_trkiso_max         <= 0) zele_trkiso_max         = FLT_MAX;
  if (zele_caloiso_max        <= 0) zele_caloiso_max        = FLT_MAX;
  if (mu_d0_max               <= 0) mu_d0_max               = FLT_MAX;
  if (mu_d0_max_antiid        <= 0) mu_d0_max_antiid        = FLT_MAX;
  if (mu_z0_max               <= 0) mu_z0_max               = FLT_MAX;
  if (mu_z0_max_antiid        <= 0) mu_z0_max_antiid        = FLT_MAX;
  if (mu_trkiso_max           <= 0) mu_trkiso_max           = FLT_MAX;
  if (mu_trkiso_max_antiid    <= 0) mu_trkiso_max_antiid    = FLT_MAX;
  if (mu_caloiso_max          <= 0) mu_caloiso_max          = FLT_MAX;
  if (mu_caloiso_max_antiid   <= 0) mu_caloiso_max_antiid   = FLT_MAX;
  if (zmu_d0_max              <= 0) zmu_d0_max              = FLT_MAX;
  if (zmu_z0_max              <= 0) zmu_z0_max              = FLT_MAX;
  if (zmu_trkiso_max          <= 0) zmu_trkiso_max          = FLT_MAX;
  if (zmu_caloiso_max         <= 0) zmu_caloiso_max         = FLT_MAX;
  if (jet_forward_eta_max     <= 0) jet_forward_eta_max     = FLT_MAX;
  if (jet_central_jvfeta_max  <= 0) jet_central_jvfeta_max  = FLT_MAX;
  if (jet_central_eta_max     <= 0) jet_central_eta_max     = FLT_MAX;

  return true;
}

//-----------------------------------------------------------------------------
// Should only be used for additional selection on signal objects
//
bool PSL::ObjectSelectionBase::SignalLeptonPassesAdditionalSignalCuts(int iptordered){
  if (m_EDM->GetSignalLeptonVariable(lepFlavor,iptordered) == (int)ObjType::Electron)
    return IsSignalElectronLocal(m_EDM->GetSignalLeptonContainerIndex(iptordered));
  return IsSignalMuonLocal(m_EDM->GetSignalLeptonContainerIndex(iptordered));
}

bool PSL::ObjectSelectionBase::SignalLeptonPassesAdditionalZBosonCuts(int iptordered){
  if (m_EDM->GetSignalLeptonVariable(lepFlavor,iptordered) == (int)ObjType::Electron)
    return IsZBosonElectronLocal(m_EDM->GetSignalLeptonContainerIndex(iptordered));
  return IsZBosonMuonLocal(m_EDM->GetSignalLeptonContainerIndex(iptordered));
}

bool PSL::ObjectSelectionBase::SignalLeptonPassesAntiIDCuts(int iptordered){
  if (m_EDM->GetSignalLeptonVariable(lepFlavor,iptordered) == (int)ObjType::Electron)
    return IsAntiIDElectron(m_EDM->GetSignalLeptonContainerIndex(iptordered));
  return IsAntiIDMuon(m_EDM->GetSignalLeptonContainerIndex(iptordered));
}

//-----------------------------------------------------------------------------
bool PSL::ObjectSelectionBase::IsAntiIDElectron(int icontainer){
  if (m_EDM->ElePt(icontainer)<(ele_ptmin_antiid*1000.)) return false;
  // Hard-coded: electron crack
  double ElectronEta = m_EDM->EleEtaBE(icontainer);
  if( fabs(ElectronEta) >= 1.37 && fabs(ElectronEta) <= 1.52 ) // crack region is hard-coded
    return false;

  //return !IsSignalElectronLocal(icontainer);
  bool pass_or = false;
  if ((fabs(m_EDM->EleD0Significance(icontainer)) > ele_d0_max_antiid) || (fabs(m_EDM->EleD0Significance(icontainer)) < ele_d0_min_antiid)) {
    if (!ele_antiid_or_d0) return false; // if it fails the cut and the cut is required, then return false
  } else if (ele_antiid_or_d0) pass_or = true; // if it passes the cut and the cut is part of the or, then pass_or is true

  if ((ele_z0_max_antiid < fabs(m_EDM->EleZ0(icontainer)*sin(m_EDM->GetContainerEleTLV(icontainer).Theta()))) ||
      (ele_z0_min_antiid > fabs(m_EDM->EleZ0(icontainer)*sin(m_EDM->GetContainerEleTLV(icontainer).Theta())))) {
    if (!ele_antiid_or_z0) return false;
  } else if (ele_antiid_or_z0) pass_or = true;

  if (ele_isowp > PSL::AnaIso::N_NONWP){ // make sure you are not mixing genres of the iso wp
    if ((ele_isowp_fail_antiid != AnaIso::None &&  (bool)m_EDM->EleIsolation(icontainer,ele_isowp_fail_antiid)) ||
        (ele_isowp_pass_antiid != AnaIso::None && !(bool)m_EDM->EleIsolation(icontainer,ele_isowp_pass_antiid))) {
      if (!ele_antiid_or_iso) return false;
    }
    else if (ele_antiid_or_iso) pass_or = true;
  }
  else {
    if (ele_caloisotype != PSL::AnaIso::None){
      if ((m_EDM->EleIsolation(icontainer,ele_caloisotype) > ele_caloiso_max_antiid) ||
          (m_EDM->EleIsolation(icontainer,ele_caloisotype) < ele_caloiso_min_antiid)) {
        if(!ele_antiid_or_iso) return false;
      } else if (ele_antiid_or_iso) pass_or = true;
    }
    if (ele_trkisotype != PSL::AnaIso::None){
      if ((m_EDM->EleIsolation(icontainer,ele_trkisotype) > ele_trkiso_max_antiid) ||
          (m_EDM->EleIsolation(icontainer,ele_trkisotype) < ele_trkiso_min_antiid)) {
        if (!ele_antiid_or_iso) return false;
      } else if (ele_antiid_or_iso) pass_or = true;
    }
  }

  if ((ele_id_fail_antiid != ElectronID::None && m_EDM->passLikelihood(icontainer,ele_id_fail_antiid)) || 
      (ele_id_pass_antiid != ElectronID::None && !m_EDM->passLikelihood(icontainer,ele_id_pass_antiid))) {
    if (!ele_antiid_or_id) return false;
  } else if (ele_antiid_or_id) pass_or = true;

  if (ele_antiid_or_id || ele_antiid_or_iso || ele_antiid_or_d0 || ele_antiid_or_z0) return pass_or;
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::ObjectSelectionBase::IsAntiIDMuon(int icontainer){
  if (m_EDM->MuonPt(icontainer) < (mu_ptmin_antiid*1000.)) return false;
  // A bit hacky - make sure these are Medium muons!
  if (m_EDM->LepVariableExists(lepMuonID) && !m_EDM->passMuonQuality(icontainer,mu_id)) return false;
  // HACK
  //return !IsSignalMuonLocal(icontainer);
  // HACK
  bool pass_or = false;
  if ((fabs(m_EDM->MuonD0Significance(icontainer)) > mu_d0_max_antiid ) || (fabs(m_EDM->MuonD0Significance(icontainer)) < mu_d0_min_antiid )) {
    if (!mu_antiid_or_d0) return false;
  } else if (mu_antiid_or_d0) pass_or = true;

  if ((mu_z0_max_antiid < fabs(m_EDM->MuonZ0(icontainer)*sin(m_EDM->GetContainerMuonTLV(icontainer).Theta()))) ||
      (mu_z0_min_antiid > fabs(m_EDM->MuonZ0(icontainer)*sin(m_EDM->GetContainerMuonTLV(icontainer).Theta())))) {
    if (!mu_antiid_or_z0) return false;
  } else if (mu_antiid_or_z0) pass_or = true;

  if (mu_isowp > PSL::AnaIso::N_NONWP){
    if ((mu_isowp_fail_antiid != AnaIso::None &&  (bool)m_EDM->MuonIsolation(icontainer,mu_isowp_fail_antiid)) ||
        (mu_isowp_pass_antiid != AnaIso::None && !(bool)m_EDM->MuonIsolation(icontainer,mu_isowp_pass_antiid))) {
      if (!mu_antiid_or_iso) return false;
    }
    else if (mu_antiid_or_iso) pass_or = true;
  }  
  else {
    if (mu_caloisotype != PSL::AnaIso::None){
      if ((m_EDM->MuonIsolation(icontainer,mu_caloisotype) > mu_caloiso_max_antiid ) ||
          (m_EDM->MuonIsolation(icontainer,mu_caloisotype) < mu_caloiso_min_antiid )) {
        if (!mu_antiid_or_iso) return false;
      } else if (mu_antiid_or_iso) pass_or = true;
    }
    if (mu_trkisotype != PSL::AnaIso::None){
      if ((m_EDM->MuonIsolation(icontainer,mu_trkisotype ) > mu_trkiso_max_antiid  ) ||
          (m_EDM->MuonIsolation(icontainer,mu_trkisotype ) < mu_trkiso_min_antiid  )) {
        if (!mu_antiid_or_iso) return false;
      } else if (mu_antiid_or_iso) pass_or = true;
    }
  }

  if (mu_antiid_or_iso || mu_antiid_or_d0 || mu_antiid_or_z0) return pass_or;
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::ObjectSelectionBase::IsSignalElectronLocal(int icontainer){
  if (m_EDM->ElePt(icontainer)<(ele_ptmin*1000.)) return false;
  // Hard-coded: electron crack
  double ElectronEta = m_EDM->EleEtaBE(icontainer);
  if( fabs(ElectronEta) >= 1.37 && fabs(ElectronEta) <= 1.52 ) // crack region is hard-coded
    return false;

  if (ele_d0_max < fabs(m_EDM->EleD0Significance(icontainer))) return false; // Exp
  if (ele_z0_max < fabs(m_EDM->EleZ0(icontainer)*sin(m_EDM->GetContainerEleTLV(icontainer).Theta()))) return false; // Exp
  // Below: If the WP is an IsolationTool WP, then evaluate it as a boolean.
  if (ele_isowp > PSL::AnaIso::N_NONWP){
    if (!(bool)m_EDM->EleIsolation(icontainer,ele_isowp)) return false;
  }
  else {
    if (ele_caloisotype != PSL::AnaIso::None && 
        ele_caloiso_max < m_EDM->EleIsolation(icontainer,ele_caloisotype)) return false;
    if (ele_trkisotype != PSL::AnaIso::None &&
        ele_trkiso_max  < m_EDM->EleIsolation(icontainer,ele_trkisotype )) return false;    
  }
  if ((ele_id != ElectronID::None) && !m_EDM->passLikelihood(icontainer,ele_id)) return false;
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::ObjectSelectionBase::IsZBosonElectronLocal(int icontainer){
  if (m_EDM->ElePt(icontainer)<(zele_ptmin*1000.)) return false;
  if (zele_d0_max < fabs(m_EDM->EleD0Significance(icontainer))) return false; // Exp
  if (zele_z0_max < fabs(m_EDM->EleZ0(icontainer)*sin(m_EDM->GetContainerEleTLV(icontainer).Theta()))) return false; // Exp
  // Below: If the WP is an IsolationTool WP, then evaluate it as a boolean.
  if (zele_isowp > PSL::AnaIso::N_NONWP){
    if (!(bool)m_EDM->EleIsolation(icontainer,zele_isowp)) return false;
  }
  else {
    if (zele_caloisotype != PSL::AnaIso::None && 
        zele_caloiso_max < m_EDM->EleIsolation(icontainer,zele_caloisotype)) return false;
    if (zele_trkisotype != PSL::AnaIso::None &&
        zele_trkiso_max  < m_EDM->EleIsolation(icontainer,zele_trkisotype )) return false;    
  }
  if ((zele_id != ElectronID::None) && !m_EDM->passLikelihood(icontainer,zele_id)) return false;
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::ObjectSelectionBase::IsSignalMuonLocal(int icontainer){
  if (m_EDM->MuonPt(icontainer) < (mu_ptmin*1000.)) return false;
  if (mu_d0_max < fabs(m_EDM->MuonD0Significance(icontainer))) return false; // Exp
  if (mu_z0_max < fabs(m_EDM->MuonZ0(icontainer)*sin(m_EDM->GetContainerMuonTLV(icontainer).Theta()))) return false; // Exp
  // Below: If the WP is an IsolationTool WP, then evaluate it as a boolean.
  if (mu_isowp > PSL::AnaIso::N_NONWP){
    if (!(bool)m_EDM->MuonIsolation(icontainer,mu_isowp)) return false;
  }
  else {
    if (mu_caloisotype != PSL::AnaIso::None && 
        mu_caloiso_max < m_EDM->MuonIsolation(icontainer,mu_caloisotype)) return false;
    if (mu_trkisotype != PSL::AnaIso::None &&
        mu_trkiso_max  < m_EDM->MuonIsolation(icontainer,mu_trkisotype )) return false;    
  }
  if (m_EDM->LepVariableExists(lepMuonID) && !m_EDM->passMuonQuality(icontainer,mu_id)) return false;
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::ObjectSelectionBase::IsZBosonMuonLocal(int icontainer){
  if (m_EDM->MuonPt(icontainer) < (zmu_ptmin*1000.)) return false;
  if (zmu_d0_max < fabs(m_EDM->MuonD0Significance(icontainer))) return false; // Exp
  if (zmu_z0_max < fabs(m_EDM->MuonZ0(icontainer)*sin(m_EDM->GetContainerMuonTLV(icontainer).Theta()))) return false; // Exp
  // Below: If the WP is an IsolationTool WP, then evaluate it as a boolean.
  if (zmu_isowp > PSL::AnaIso::N_NONWP){
    if (!(bool)m_EDM->MuonIsolation(icontainer,zmu_isowp)) return false;
  }
  else {
    if (zmu_caloisotype != PSL::AnaIso::None && 
        zmu_caloiso_max < m_EDM->MuonIsolation(icontainer,zmu_caloisotype)) return false;
    if (zmu_trkisotype != PSL::AnaIso::None &&
        zmu_trkiso_max  < m_EDM->MuonIsolation(icontainer,zmu_trkisotype )) return false;    
  }
  if (m_EDM->LepVariableExists(lepMuonID) && !m_EDM->passMuonQuality(icontainer,zmu_id)) return false;
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::ObjectSelectionBase::CheckIsolationConfigurationValidity(void)
{
  if (ele_isowp != PSL::AnaIso::None && ele_isowp < PSL::AnaIso::N_NONWP) {
    MSG_ERROR("Error! Cannot set ele WP to " << ele_isowp);
    return false;
  }
  if (ele_trkisotype != PSL::AnaIso::None && ele_trkisotype > PSL::AnaIso::N_NONWP) {
    MSG_ERROR("Error! Cannot set ele trkiso to " << ele_trkisotype);
    return false;
  }
  if (ele_caloisotype != PSL::AnaIso::None && ele_caloisotype > PSL::AnaIso::N_NONWP) {
    MSG_ERROR("Error! Cannot set ele caloiso to " << ele_caloisotype);
    return false;
  }
  if (ele_isowp != PSL::AnaIso::None && (ele_trkisotype != PSL::AnaIso::None || ele_caloisotype != PSL::AnaIso::None)){
    MSG_ERROR("Error! Ele WP is set but so is trk or calo iso flavor. Undefined behavior!");
    return false;
  }
  
  if (zele_isowp != PSL::AnaIso::None && zele_isowp < PSL::AnaIso::N_NONWP) {
    MSG_ERROR("Error! Cannot set ele WP to " << zele_isowp);
    return false;
  }
  if (zele_trkisotype != PSL::AnaIso::None && zele_trkisotype > PSL::AnaIso::N_NONWP) {
    MSG_ERROR("Error! Cannot set ele trkiso to " << zele_trkisotype);
    return false;
  }
  if (zele_caloisotype != PSL::AnaIso::None && zele_caloisotype > PSL::AnaIso::N_NONWP) {
    MSG_ERROR("Error! Cannot set ele caloiso to " << zele_caloisotype);
    return false;
  }
  if (zele_isowp != PSL::AnaIso::None && (zele_trkisotype != PSL::AnaIso::None || zele_caloisotype != PSL::AnaIso::None)){
    MSG_ERROR("Error! Ele WP is set but so is trk or calo iso flavor. Undefined behavior!");
    return false;
  }
  
  if (mu_isowp != PSL::AnaIso::None && mu_isowp < PSL::AnaIso::N_NONWP) {
    MSG_ERROR("Error! Cannot set muon WP to " << mu_isowp);
    return false;
  }
  if (mu_trkisotype != PSL::AnaIso::None && mu_trkisotype > PSL::AnaIso::N_NONWP) {
    MSG_ERROR("Error! Cannot set muon trkiso to " << mu_trkisotype);
    return false;
  }
  if (mu_caloisotype != PSL::AnaIso::None && mu_caloisotype > PSL::AnaIso::N_NONWP) {
    MSG_ERROR("Error! Cannot set muon caloiso to " << mu_caloisotype);
    return false;
  }
  if (mu_isowp != PSL::AnaIso::None && (mu_trkisotype != PSL::AnaIso::None || mu_caloisotype != PSL::AnaIso::None)){
    MSG_ERROR("Error! Muon WP is set but so is trk or calo iso flavor. Undefined behavior!");
    return false;
  }

  if (zmu_isowp != PSL::AnaIso::None && zmu_isowp < PSL::AnaIso::N_NONWP) {
    MSG_ERROR("Error! Cannot set muon WP to " << zmu_isowp);
    return false;
  }
  if (zmu_trkisotype != PSL::AnaIso::None && zmu_trkisotype > PSL::AnaIso::N_NONWP) {
    MSG_ERROR("Error! Cannot set muon trkiso to " << zmu_trkisotype);
    return false;
  }
  if (zmu_caloisotype != PSL::AnaIso::None && zmu_caloisotype > PSL::AnaIso::N_NONWP) {
    MSG_ERROR("Error! Cannot set muon caloiso to " << zmu_caloisotype);
    return false;
  }
  if (zmu_isowp != PSL::AnaIso::None && (zmu_trkisotype != PSL::AnaIso::None || zmu_caloisotype != PSL::AnaIso::None)){
    MSG_ERROR("Error! Muon WP is set but so is trk or calo iso flavor. Undefined behavior!");
    return false;
  }
  return true;
}

