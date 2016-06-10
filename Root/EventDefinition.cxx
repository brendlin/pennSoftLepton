#include "pennSoftLepton/EventDefinition.h"

ClassImp(PSL::EventDefinition)

PSL::Particle::Particle()
{
  tlv = TLorentzVector();
  charge = 0;
  i_cont = -1; // the d3pd index of this signal particle
  obj_type = ObjType::None;
}

PSL::EventDefinition::EventDefinition() : name("")
{
  return;
}

void PSL::EventDefinition::FillVectorOfFunctions(){
  // Set up vectors of functions with dummy functions first
  for (unsigned int i=0;i<N_VARIABLES;++i){
    m_VariableFunctions_double.push_back(&EventDefinition::ERROR_D);
  }
  for (unsigned int i=0;i<N_VARIABLES;++i){
    m_VariableFunctions_int.push_back(&EventDefinition::ERROR_I);
  }

  // Connect each function to the correct variable element in the vector
  // Separate vectors for integers and doubles.
  m_VariableFunctions_int[vpassCosmicMuon         ] = &EventDefinition::PassCosmicMuon       ;
  m_VariableFunctions_int[vpassBadJet             ] = &EventDefinition::PassBadJet           ;
  m_VariableFunctions_int[vpassBadMuon            ] = &EventDefinition::PassBadMuon          ;
  m_VariableFunctions_int[vnBaselineLeptons       ] = &EventDefinition::NBaselineLeptons     ;
  m_VariableFunctions_int[vnBaselineMuons         ] = &EventDefinition::NBaselineMuons       ;
  m_VariableFunctions_int[vnBaselineElectrons     ] = &EventDefinition::NBaselineElectrons   ;
  m_VariableFunctions_int[vnSignalLeptons         ] = &EventDefinition::NSignalLeptons       ;
  m_VariableFunctions_int[vnSignalMuons           ] = &EventDefinition::NSignalMuons         ;
  m_VariableFunctions_int[vnSignalElectrons       ] = &EventDefinition::NSignalElectrons     ;
  m_VariableFunctions_int[vChanFlavor             ] = &EventDefinition::ChanFlavor           ;
  m_VariableFunctions_int[vSameSign               ] = &EventDefinition::IsSameSign           ;
  m_VariableFunctions_int[vNSignalTaus            ] = &EventDefinition::NSignalTaus          ;
  m_VariableFunctions_int[vSFOS                   ] = &EventDefinition::IsSFOS               ;
  m_VariableFunctions_int[vnBjets                 ] = &EventDefinition::nBjets               ;
  m_VariableFunctions_int[vjet0_index             ] = &EventDefinition::Jet0Index            ;
  m_VariableFunctions_int[vjet1_index             ] = &EventDefinition::Jet1Index            ;
  m_VariableFunctions_int[vlep0_index             ] = &EventDefinition::LeadingLeptonIndex   ;
  m_VariableFunctions_int[vlep1_index             ] = &EventDefinition::SubleadingLeptonIndex;
  m_VariableFunctions_int[vlep2_index             ] = &EventDefinition::ThirdLeptonIndex     ;
  m_VariableFunctions_int[vlepleadZ_index         ] = &EventDefinition::LeadZLepIndex        ;
  m_VariableFunctions_int[vlepsubleadZ_index      ] = &EventDefinition::SubleadZLepIndex     ;
  m_VariableFunctions_int[vlepW_index             ] = &EventDefinition::WlepIndex            ;
  m_VariableFunctions_int[vlepleadMinSFOS_index   ] = &EventDefinition::LeadMinSFOSIndex     ;
  m_VariableFunctions_int[vlepsubleadMinSFOS_index] = &EventDefinition::SubleadMinSFOSIndex  ;
  m_VariableFunctions_int[vlepWMinSFOS_index      ] = &EventDefinition::WMinSFOSIndex        ;
  m_VariableFunctions_int[vantilep0_index         ] = &EventDefinition::Antilep0Index        ;
  m_VariableFunctions_int[vNJets                  ] = &EventDefinition::nJets                ;
  m_VariableFunctions_int[vNCentralJets           ] = &EventDefinition::nCentralJets         ;
  m_VariableFunctions_int[vNForwardJets           ] = &EventDefinition::nForwardJets         ;
  m_VariableFunctions_int[vnVetoLeptons           ] = &EventDefinition::NVetoLeptons         ;
  m_VariableFunctions_int[vWlepPassWSelection     ] = &EventDefinition::WlepPassWSelection   ;
  m_VariableFunctions_int[vPassTriggerEle         ] = &EventDefinition::PassTriggerEle       ;
  m_VariableFunctions_int[vPassTriggerMu          ] = &EventDefinition::PassTriggerMu        ;
  m_VariableFunctions_int[vPassTriggerMatch       ] = &EventDefinition::PassTriggerMatch     ;

  // now the double functions
  m_VariableFunctions_double[vmet_Et          ] = &EventDefinition::met_Et_gev               ;
  m_VariableFunctions_double[vmet_phi         ] = &EventDefinition::met_phi                  ;
  m_VariableFunctions_double[vmet_sumet       ] = &EventDefinition::met_sumet_gev            ;
  m_VariableFunctions_double[vrefEle          ] = &EventDefinition::refEle_gev               ;
  m_VariableFunctions_double[vrefEle_etx      ] = &EventDefinition::refEle_etx_gev           ;
  m_VariableFunctions_double[vrefEle_ety      ] = &EventDefinition::refEle_ety_gev           ;
  m_VariableFunctions_double[vrefEle_sumet    ] = &EventDefinition::refEle_sumet_gev         ;
  m_VariableFunctions_double[vrefMuo          ] = &EventDefinition::refMuo_gev               ;
  m_VariableFunctions_double[vrefMuo_etx      ] = &EventDefinition::refMuo_etx_gev           ;
  m_VariableFunctions_double[vrefMuo_ety      ] = &EventDefinition::refMuo_ety_gev           ;
  m_VariableFunctions_double[vrefMuo_sumet    ] = &EventDefinition::refMuo_sumet_gev         ;
  m_VariableFunctions_double[vrefJet          ] = &EventDefinition::refJet_gev               ;
  m_VariableFunctions_double[vrefJet_etx      ] = &EventDefinition::refJet_etx_gev           ;
  m_VariableFunctions_double[vrefJet_ety      ] = &EventDefinition::refJet_ety_gev           ;
  m_VariableFunctions_double[vrefJet_sumet    ] = &EventDefinition::refJet_sumet_gev         ;
  m_VariableFunctions_double[vrefGamma        ] = &EventDefinition::refGamma_gev             ;
  m_VariableFunctions_double[vrefGamma_etx    ] = &EventDefinition::refGamma_etx_gev         ;
  m_VariableFunctions_double[vrefGamma_ety    ] = &EventDefinition::refGamma_ety_gev         ;
  m_VariableFunctions_double[vrefGamma_sumet  ] = &EventDefinition::refGamma_sumet_gev       ;
  m_VariableFunctions_double[vsoftTerm        ] = &EventDefinition::softTerm_gev             ;
  m_VariableFunctions_double[vsoftTerm_etx    ] = &EventDefinition::softTerm_etx_gev         ;
  m_VariableFunctions_double[vsoftTerm_ety    ] = &EventDefinition::softTerm_ety_gev         ;
  m_VariableFunctions_double[vsoftTerm_sumet  ] = &EventDefinition::softTerm_sumet_gev       ;
  m_VariableFunctions_double[vPtll            ] = &EventDefinition::Pair_LeadingLeptonsPt_gev;
  m_VariableFunctions_double[vPt3l            ] = &EventDefinition::Three_LeadingLeptons_gev ;
  m_VariableFunctions_double[vPtllZ           ] = &EventDefinition::Pair_SFOSPt_gev          ;
  m_VariableFunctions_double[vPtllZmin        ] = &EventDefinition::Pair_minSFOSPt_gev       ;
  m_VariableFunctions_double[vMll             ] = &EventDefinition::Pair_SFOS_M_gev          ;
  m_VariableFunctions_double[vMll_alt         ] = &EventDefinition::AltPair_SFOS_M_gev       ;
  m_VariableFunctions_double[vMll_ss          ] = &EventDefinition::Pair_SFSS_M_gev          ;
  m_VariableFunctions_double[vMWjj            ] = &EventDefinition::Pair_Wjj_M_gev           ;
  m_VariableFunctions_double[vMjjMin          ] = &EventDefinition::Pair_jjMin_M_gev         ;
  m_VariableFunctions_double[vMinMll          ] = &EventDefinition::Pair_minSFOS_M_gev       ;
  m_VariableFunctions_double[vMTW             ] = &EventDefinition::Mt_WLepton_cand_gev      ;
  m_VariableFunctions_double[vMTWmin          ] = &EventDefinition::Mt_WLepton_MinSFOS_gev   ;
  m_VariableFunctions_double[vDeltaRll        ] = &EventDefinition::DeltaRll                 ;
  m_VariableFunctions_double[vDeltaRllZ       ] = &EventDefinition::DeltaR_ZleptonsZ         ;
  m_VariableFunctions_double[vDeltaRllZmin    ] = &EventDefinition::DeltaR_ZleptonsZMin      ;
  m_VariableFunctions_double[vMEToverPtj0     ] = &EventDefinition::METoverPtj0              ;
  m_VariableFunctions_double[vPtlloverMET     ] = &EventDefinition::PtlloverMET              ;
  m_VariableFunctions_double[vPtllZoverMET    ] = &EventDefinition::PtllZoverMET             ;
  m_VariableFunctions_double[vPtllZminoverMET ] = &EventDefinition::PtllZminoverMET          ;
  m_VariableFunctions_double[vMinDeltaPhilMET ] = &EventDefinition::MinDeltaPhiLep0Lep1_MET  ;
  m_VariableFunctions_double[vMinDeltaPhi3LMET] = &EventDefinition::MinDeltaPhi3Leps_MET     ;
  m_VariableFunctions_double[vMaxDeltaPhi3LMET] = &EventDefinition::MaxDeltaPhi3Leps_MET     ;
  m_VariableFunctions_double[vMaxDeltaPhilJet ] = &EventDefinition::MaxDeltaPhiLep0Lep1_Jet1 ;
  m_VariableFunctions_double[vMinDeltaPhi3LJet] = &EventDefinition::MinDeltaPhi3Leps_Jet1    ;
  m_VariableFunctions_double[vMaxDeltaPhi3LJet] = &EventDefinition::MaxDeltaPhi3Leps_Jet1    ;
  m_VariableFunctions_double[vDeltaPhi3LJet   ] = &EventDefinition::DeltaPhi3Leps_Jet1       ;
  m_VariableFunctions_double[vDeltaPhi3LMET   ] = &EventDefinition::DeltaPhi3Leps_MET        ;
  m_VariableFunctions_double[vDeltaPhiMETJet  ] = &EventDefinition::DeltaPhi_MET_Jet1        ;
  m_VariableFunctions_double[vMt              ] = &EventDefinition::Mt_gev                   ;
  m_VariableFunctions_double[vM3l             ] = &EventDefinition::M3l_gev                  ;
  m_VariableFunctions_double[vMll_Upsilon     ] = &EventDefinition::Mll_Upsilon_gev          ;
  m_VariableFunctions_double[vDeltaPhill      ] = &EventDefinition::DeltaPhill               ;
  m_VariableFunctions_double[vDeltaPhiLeadZWLep]= &EventDefinition::DeltaPhiLeadZWLep        ;
  m_VariableFunctions_double[vDeltaPhiZLeps   ] = &EventDefinition::DeltaPhiZLeps            ;
  m_VariableFunctions_double[vDeltaPhiZWLep   ] = &EventDefinition::DeltaPhiZWLep            ;
  m_VariableFunctions_double[vDeltaPhiZMet    ] = &EventDefinition::DeltaPhiZMet             ;
  m_VariableFunctions_double[vDeltaPhiZWSyst  ] = &EventDefinition::DeltaPhiZWSyst           ;
  m_VariableFunctions_double[vPt3lPlusMet     ] = &EventDefinition::Pt3lPlusMet              ;
  m_VariableFunctions_double[vPtW             ] = &EventDefinition::PtW                      ;
  m_VariableFunctions_double[vMtWZ            ] = &EventDefinition::MtWZ_gev                 ;
  m_VariableFunctions_double[vYZminusYlW      ] = &EventDefinition::YZminusYlW               ;
  m_VariableFunctions_double[vMtZ             ] = &EventDefinition::Mtll_gev                 ;
  m_VariableFunctions_double[vPtWZ            ] = &EventDefinition::PtWZ                     ;
  return;
}

//
// NtupleWrapper.cxx needs this function to be able to play around with the event definition
// at the ntuple stage.
//
void PSL::EventDefinition::MakeCopy(EventDefinition& evtdef) const{
  evtdef.name = name;
  evtdef.n_baseline_ele = n_baseline_ele;
  evtdef.n_baseline_mu = n_baseline_mu;
  evtdef.n_baseline_tau = n_baseline_tau;
  evtdef.n_baseline_jet = n_baseline_jet;
  evtdef.m_passBadJet = m_passBadJet;
  evtdef.m_passBadMuon = m_passBadMuon;
  evtdef.m_passCosmicMuon = m_passCosmicMuon;
  evtdef.m_wlep_pass_sel = m_wlep_pass_sel;
  evtdef.chan = chan;
  evtdef.chan_antiid = chan_antiid;
  evtdef.leps = leps;
  evtdef.antileps = antileps;
  evtdef.vetoleps = vetoleps;
  evtdef.jets = jets;
  evtdef.taus = taus;
  evtdef.leadZlep_index = leadZlep_index;
  evtdef.subleadZlep_index = subleadZlep_index;
  evtdef.Wlep_index = Wlep_index;
  evtdef.leadMinSFOS_index = leadMinSFOS_index;
  evtdef.subleadMinSFOS_index = subleadMinSFOS_index;
  evtdef.WlepMinSFOS_index = WlepMinSFOS_index;
  evtdef.leadYlep_index = leadYlep_index;
  evtdef.subleadYlep_index = subleadYlep_index;
  evtdef.WlepYpsilon_index = WlepYpsilon_index;
  evtdef.trigger_bits_set = trigger_bits_set;
  evtdef.trigger_bits = trigger_bits;
  evtdef.i_bch_jet = i_bch_jet;
  evtdef.met_tv2        = met_tv2       ;
  evtdef.met_sumet      = met_sumet     ;
  evtdef.refEle_tv2     = refEle_tv2    ;
  evtdef.refEle_sumet   = refEle_sumet  ;
  evtdef.refMuo_tv2     = refMuo_tv2    ;
  evtdef.refMuo_sumet   = refMuo_sumet  ;
  evtdef.refJet_tv2     = refJet_tv2    ;
  evtdef.refJet_sumet   = refJet_sumet  ;
  evtdef.refGamma_tv2   = refGamma_tv2  ;
  evtdef.refGamma_sumet = refGamma_sumet;
  evtdef.softTerm_tv2   = softTerm_tv2  ;
  evtdef.softTerm_sumet = softTerm_sumet;
}

//
// This is the magic function that takes an enum Variable and returns the 
// result of the function of the same name.
//
double PSL::EventDefinition::GetEventVariable(Variable v){
  //MSG_INFO(ConvertVarToStr(v));
  if (!m_VariableFunctions_double.size()) FillVectorOfFunctions();
  if (VariableIsInt(v)) return (double)(this->*m_VariableFunctions_int[v])();
  return (this->*m_VariableFunctions_double[v])();
}

bool PSL::DescendingPt(const Particle& l1,const Particle& l2) { return l1.tlv.Pt() > l2.tlv.Pt(); }

bool PSL::IsElectron(const Particle& l){return l.obj_type == ObjType::Electron;}
bool PSL::IsMuon(const Particle& l){return l.obj_type == ObjType::Muon;}

bool PSL::IsBjet(const Particle& l){return l.is_Bjet;}
bool PSL::IsCentralLightJet(const Particle& l){return l.is_CentralLightJet;}
bool PSL::IsForwardJet(const Particle& l){return l.is_ForwardJet;}

