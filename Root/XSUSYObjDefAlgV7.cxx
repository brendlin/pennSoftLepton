#include <pennSoftLepton/XSUSYObjDefAlgV7.h>
#ifndef SKIP_XAOD_COMPILATION
#ifndef BEFORE_SUSYTOOLS_000709

#include <EventLoop/Worker.h>

#include "SUSYTools/SUSYObjDef_xAOD.h"
#include "xAODMissingET/MissingET.h"

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::XSUSYObjDefAlgV7)
  
//-----------------------------------------------------------------------------
PSL::XSUSYObjDefAlgV7::XSUSYObjDefAlgV7() : ObjectSelectionBase()
  ,do_antiid              (false)
  ,keep_overlapremoved_muons(false)
  //,do_custom_overlapremove(false)
  ,m_SUSYObjDef           (0)
  ,dec_baseline           ("baseline")
  ,dec_signal             ("signal")
  ,dec_passOR             ("passOR")
  ,dec_badjet             ("bad")
  ,dec_bjet               ("bjet")
  ,dec_bad                ("bad")
  ,dec_cosmic             ("cosmic")
{
  MSG_DEBUG("Constructor");
}

//-----------------------------------------------------------------------------
bool PSL::XSUSYObjDefAlgV7::init(void)
{
  if (!ObjectSelectionBase::init()) return false;

  m_SUSYObjDef = new ST::SUSYObjDef_xAOD("SUSYObjDef_xAOD");
  int isData = !((wk()->metaData()->castString("IsData")).empty());
  bool isAtlFast = (wk()->metaData()->castString("Simulation") == "atlfast");
  bool isFullSim = (wk()->metaData()->castString("Simulation") == "fullsim");
  if (isData    ) m_SUSYObjDef->setProperty("DataSource",ST::ISUSYObjDef_xAODTool::Data).isSuccess();
  if (isAtlFast ) m_SUSYObjDef->setProperty("DataSource",ST::ISUSYObjDef_xAODTool::AtlfastII).isSuccess();
  if (isFullSim ) m_SUSYObjDef->setProperty("DataSource",ST::ISUSYObjDef_xAODTool::FullSim).isSuccess();
  // m_SUSYObjDef->setProperty("EleIsoWP","GradientLoose").isSuccess();
  // m_SUSYObjDef->setProperty("MuIsoWP","GradientLoose").isSuccess();
  //PathResolverSetOutputLevel(MSG::ERROR);
  
  m_SUSYObjDef->setProperty("PhotonIsoWP","FixedCutLoose").isSuccess();

  m_SUSYObjDef->setProperty("EleID"        ,ConvertElectronIDToStr(ele_id)     ).isSuccess();
  m_SUSYObjDef->setProperty("EleIdBaseline",ConvertElectronIDToStr(ele_id_base)).isSuccess();
  m_SUSYObjDef->setProperty("EleIdFallback","MediumLH"               ).isSuccess();
  m_SUSYObjDef->setProperty("PhotonId"     ,"Tight"                  ).isSuccess();
  m_SUSYObjDef->setProperty("MuId"         ,ConvertMuonIDToStr(mu_id)).isSuccess();
  // no baseline muon id?
  m_SUSYObjDef->setProperty("BtagWP"       ,"FixedCutBEff_77"        ).isSuccess();
  // what to do when you do not want to apply id?
  if (ele_isowp == AnaIso::None)
    m_SUSYObjDef->setProperty("EleIsoWP","GradientLoose").isSuccess(); // give SUSYTools a dummy iso
  else m_SUSYObjDef->setProperty("EleIsoWP",ConvertAnaIsoToStr(ele_isowp)).isSuccess();
  if (mu_isowp == AnaIso::None)
    m_SUSYObjDef->setProperty("MuIsoWP","GradientLoose").isSuccess(); // give SUSYTools a dummy iso
  else m_SUSYObjDef->setProperty("MuIsoWP",ConvertAnaIsoToStr(mu_isowp )).isSuccess();

  m_SUSYObjDef->setProperty("ConfigFile", "$ROOTCOREBIN/../pennSoftLepton/config/"+m_ConfigFile).isSuccess();

  m_SUSYObjDef->msg().setLevel( MSG::WARNING );
  
// from SUSYTools-00-05-00-30 SUSYToolsInit is called internally during initialize
#ifdef BEFORE_SUSYTOOLS_050030
  if( m_SUSYObjDef->SUSYToolsInit().isFailure() ) {
    MSG_INFO("Failed to initialise tools in SUSYToolsInit()...");
    MSG_INFO("Exiting..." );
    return false;
  }
#endif // BEFORE_SUSYTOOLS_050030

  if( m_SUSYObjDef->initialize() != StatusCode::SUCCESS){
    PrintConfiguration();
    MSG_INFO("Cannot intialize SUSYObjDef_xAOD..." );
    MSG_INFO("Exiting... " );
    return false;
  }
  MSG_INFO("SUSYObjDef_xAOD initialized... " );

  if ( m_sysVariation.empty() ){ //let's work on systematics

    MSG_INFO("Running nominal job with no systematic variation");
  } else {
    MSG_INFO("Running job with systematic variation " << m_sysVariation);
    CP::SystematicCode::enableFailure(); //let the code crash if something goes wrong with the systematic variation
    std::vector<ST::SystInfo> systInfoList = m_SUSYObjDef->getSystInfoList();
    MSG_INFO("Getting all allowed systematic variations...");
    for (const auto& sysInfo : systInfoList){
      const CP::SystematicSet& sys = sysInfo.systset;
      if (sys.name() == m_sysVariation) {
        systematicVariation = sys;
        MSG_INFO("Found wanted systematic in list of allowed variations " << sys.name().c_str());
      }
      MSG_DEBUG("This is an allowed systematic variation " << sys.name().c_str());
    } //looping over allowed systematics

    if (systematicVariation.empty()) {
      MSG_INFO( "I couldn't find " << m_sysVariation << " in the list of allowed systematic variations, so I'll just run a nominal job");
    }

    if ((m_SUSYObjDef->applySystematicVariation(systematicVariation) != CP::SystematicCode::Ok) ){
      MSG_WARNING( "Cannot configure SUSYTools for systematic variation " << systematicVariation.name() );
    } else {
      MSG_INFO("Variation configured... "<< systematicVariation.name() );
    }
  } // asking for systematic

  PrintConfiguration();
  return true;
}

//-----------------------------------------------------------------------------
void PSL::XSUSYObjDefAlgV7::loop(void){
  MSG_DEBUG("loop.");

  if (m_EDM->jets         ){ delete m_EDM->jets         ; m_EDM->jets          = 0 ;}
  if (m_EDM->jets_aux     ){ delete m_EDM->jets_aux     ; m_EDM->jets_aux      = 0 ;}
  if (m_EDM->electrons    ){ delete m_EDM->electrons    ; m_EDM->electrons     = 0 ;}
  if (m_EDM->electrons_aux){ delete m_EDM->electrons_aux; m_EDM->electrons_aux = 0 ;}
  if (m_EDM->muons        ){ delete m_EDM->muons        ; m_EDM->muons         = 0 ;}
  if (m_EDM->muons_aux    ){ delete m_EDM->muons_aux    ; m_EDM->muons_aux     = 0 ;}
  if (m_EDM->taus         ){ delete m_EDM->taus         ; m_EDM->taus          = 0 ;}
  if (m_EDM->taus_aux     ){ delete m_EDM->taus_aux     ; m_EDM->taus_aux      = 0 ;}
  if (m_EDM->photons      ){ delete m_EDM->photons      ; m_EDM->photons       = 0 ;}
  if (m_EDM->photons_aux  ){ delete m_EDM->photons_aux  ; m_EDM->photons_aux   = 0 ;}
  if (m_EDM->met          ){ delete m_EDM->met          ; m_EDM->met           = 0 ;}
  if (m_EDM->met_aux      ){ delete m_EDM->met_aux      ; m_EDM->met_aux       = 0 ;}

  // baseline jets (before overlap) automatically have a 20 GeV hard-coded cut.
  // ptmin and eta_max are enforced on the bad_jet bit.
  // Not sure how to treat forward jets here.
#ifdef BEFORE_SUSYTOOLS_000611
  m_SUSYObjDef->GetJets(m_EDM->jets,m_EDM->jets_aux
                        ,/*recordSG*/false,jet_ptmin*1000.,jet_central_eta_max).isSuccess();
#else
  m_SUSYObjDef->GetJets(m_EDM->jets,m_EDM->jets_aux,/*recordSG*/false).isSuccess();  
#endif
  MSG_DEBUG("Number of jets: " << (static_cast< int >( m_EDM->jets->size() )));

  m_SUSYObjDef->GetElectrons(m_EDM->electrons,m_EDM->electrons_aux,/*recordSG*/false).isSuccess();
  MSG_DEBUG("Number of elecs: " << m_EDM->electrons->size() );
  ////// HACK TO APPLY Run-1-style Overlap Removal! TREAT WITH CARE. SHOULD BE COMMENTD BY DEFAULT!
  // // // for(unsigned int icontainer=0;icontainer<m_EDM->electrons->size();++icontainer){
  // // //   const xAOD::Electron* ele = m_EDM->getElectron(icontainer);
  // // //   if (fabs(m_EDM->EleD0Significance(icontainer)) > 5.) dec_baseline(*ele) = false;
  // // //   if (fabs(m_EDM->EleZ0(icontainer)*sin(m_EDM->GetContainerEleTLV(icontainer).Theta())) > 0.5)
  // // //     dec_baseline(*ele) = false;
  // // //   if (!m_EDM->EleIsolation(icontainer,AnaIso::GradientLoose)) dec_baseline(*ele) = false;
  // // // }
  ////// end HACK TO APPLY Run-1-style Overlap Removal! TREAT WITH CARE. SHOULD BE COMMENTD BY DEFAULT!

  m_SUSYObjDef->GetMuons(m_EDM->muons,m_EDM->muons_aux,/*recordSG*/false).isSuccess();
  MSG_DEBUG("Number of muons: " << m_EDM->muons->size() );
  ////// HACK TO APPLY Run-1-style Overlap Removal! TREAT WITH CARE. SHOULD BE COMMENTD BY DEFAULT!
  // // // for(unsigned int icontainer=0;icontainer<m_EDM->muons->size();++icontainer){
  // // //   const xAOD::Muon* muon = m_EDM->getMuon(icontainer);
  // // //   if (fabs(m_EDM->MuonD0Significance(icontainer)) > 3.) dec_baseline(*muon) = false;
  // // //   if (fabs(m_EDM->MuonZ0(icontainer)*sin(m_EDM->GetContainerMuonTLV(icontainer).Theta())) > 0.5)
  // // //     dec_baseline(*muon) = false;
  // // //   if (!m_EDM->MuonIsolation(icontainer,AnaIso::GradientLoose)) dec_baseline(*muon) = false;
  // // // }
  ////// end HACK TO APPLY Run-1-style Overlap Removal! TREAT WITH CARE. SHOULD BE COMMENTD BY DEFAULT!

#ifndef ISREL20 // SUSY2 (which we are switching to starting in Rel20) does not save taus
  m_SUSYObjDef->GetTaus(m_EDM->taus,m_EDM->taus_aux).isSuccess();
  MSG_DEBUG("Number of taus: " << m_EDM->taus->size() );
#endif

  // m_SUSYObjDef->GetPhotons(m_EDM->photons,m_EDM->photons_aux).isSuccess();
  // MSG_DEBUG("Number of photons: " << m_EDM->photons->size() );

  m_EDM->met = new xAOD::MissingETContainer;
  m_EDM->met_aux = new xAOD::MissingETAuxContainer;
  m_EDM->met->setStore(m_EDM->met_aux);
  m_SUSYObjDef->GetMET(*m_EDM->met
                       ,m_EDM->jets
                       ,m_EDM->electrons
                       ,m_EDM->muons
                       ,m_EDM->photons
                       ,m_EDM->taus
                       ).isSuccess();

  m_evtdef.Reset();
  m_EDM->Reset();

  // for(unsigned int i=0;i<m_EDM->met->size();++i){
  //   xAOD::MissingET* metterm = m_EDM->met->at(i);
  //   MSG_INFO(Form("MET term %s px = %f py = %f",
  //                 metterm->name().c_str(), metterm->mpx(), metterm->mpy()));
  //   MSG_INFO("Source: " << metterm->source());
  //   //MSG_INFO("refEle: " << xAOD::MissingETBase::Source::Electron);
  // }
  
  xAOD::MissingETContainer::const_iterator metterm = m_EDM->met->find("RefEle");
  if (metterm==m_EDM->met->end()) { MSG_INFO("Error! RefEle missing!"); exit(1); }
  m_evtdef.refEle_tv2 = TVector2((*metterm)->mpx(),(*metterm)->mpy());
  m_evtdef.refEle_sumet = (*metterm)->sumet();

#ifndef ISREL20
  metterm = m_EDM->met->find("RefGamma");
  if (metterm==m_EDM->met->end()) { MSG_INFO("Error! RefGamma missing!"); exit(1); }
  m_evtdef.refGamma_tv2 = TVector2((*metterm)->mpx(),(*metterm)->mpy());
  m_evtdef.refGamma_sumet = (*metterm)->sumet();
#endif
  
  // metterm = m_EDM->met->find("RefTau");
  // if (metterm==m_EDM->met->end()) { MSG_INFO("Error! RefTau missing!"); exit(1); }
  // m_evtdef.refTau_tv2 = TVector2((*metterm)->mpx(),(*metterm)->mpy());
  // m_evtdef.refTau_sumet = (*metterm)->sumet();

  metterm = m_EDM->met->find("RefJet");
  if (metterm==m_EDM->met->end()) { MSG_INFO("Error! RefJet missing!"); exit(1); }
  m_evtdef.refJet_tv2 = TVector2((*metterm)->mpx(),(*metterm)->mpy());
  m_evtdef.refJet_sumet = (*metterm)->sumet();

  metterm = m_EDM->met->find("Muons");
  if (metterm==m_EDM->met->end()) { MSG_INFO("Error! Muons missing!"); exit(1); }
  m_evtdef.refMuo_tv2 = TVector2((*metterm)->mpx(),(*metterm)->mpy());
  m_evtdef.refMuo_sumet = (*metterm)->sumet();

  // metterm = m_EDM->met->find("SoftClus");
  // if (metterm==m_EDM->met->end()) { MSG_INFO("Error! SoftClus missing!"); exit(1); }
  // m_evtdef.softTerm_tv2 = TVector2((*metterm)->mpx(),(*metterm)->mpy());
  // m_evtdef.softTerm_sumet = (*metterm)->sumet();  

  metterm = m_EDM->met->find("Final");
  if (metterm==m_EDM->met->end()) { MSG_INFO("Error! Met term called Final missing!"); exit(1); }
  m_evtdef.met_tv2 = TVector2((*metterm)->mpx(),(*metterm)->mpy());
  m_evtdef.met_sumet = (*metterm)->sumet();

  // before overlap removal, but after m_evtdef has been reset
  // I think this was done to preserve dxaod-xaod compatibility
  //for(unsigned int i=0;i<m_EDM->electrons->size();++i){
  //const xAOD::Electron* ele = m_EDM->getElectron(i);
    //m_evtdef.ele.tlv[i] = ele->p4();
    // if (ele->p4().Pt() != ele->pt()) MSG_INFO("p4 pt != ele pt"); // this pings a lot.
    //m_evtdef.ele.tlv[i] = m_EDM->CorrectedElectronTLV(i); // to match derivation
    //   if (fabs(m_evtdef.ele.tlv[i].Eta())>ele_eta_max) dec_baseline(*ele) = false;
    //if (m_evtdef.ele.tlv[i].Pt()<ele_ptmin_base*1000.) dec_baseline(*ele) = false;
  //}
  
  // Bad muon event veto cut (function IsBadMuon in SUSYTools):
  // veto any event where a baseline muon before overlap removal and satisfying σ(q/p)/abs(q/p) > 0.2 is found.
  for(unsigned int i=0;i<m_EDM->muons->size();++i){
    const xAOD::Muon* muon = m_EDM->getMuon(i);
    
    if (dec_baseline(*muon) && dec_bad(*muon)) {
      m_evtdef.m_passBadMuon = false;
      break;
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // SIGNAL OBJECTS
  ////////////////////////////////////////////////////////////////////////

  // for harmonization, signal lepton determination has to happen before overlap removal
  // jets: need to find b-jets, for lepton veto purposes
  for(unsigned int i=0;i<m_EDM->jets->size();++i){
    const xAOD::Jet* jet = m_EDM->getJet(i);
    // hopefully this is redundant, but I needed to put this in for SUSYTools-00-05-00-31
    if (fabs(jet->eta()) > 2.8) dec_baseline(*jet) = false;
    dec_passOR(*jet) = dec_baseline(*jet);
  }
  
  for(unsigned int i=0;i<m_EDM->electrons->size();++i){
    const xAOD::Electron* ele = m_EDM->getElectron(i);
    dec_passOR(*ele) = dec_baseline(*ele);
    if (false){ // set this to true if you want the SUSYTools output (mostly for debugging)
      // SUSYTools now runs IsSignalElectron in FillElectron
    } else {
      dec_signal(*ele) = dec_baseline(*ele) && IsSignalElectronLocal(i);
    }
  }
  
  for(unsigned int i=0;i<m_EDM->muons->size();++i){
    const xAOD::Muon* muon = m_EDM->getMuon(i);
    dec_passOR(*muon) = dec_baseline(*muon);
    if (false) { // set this to true if you want the SUSYTools output (mostly for debugging)
      // SUSYTools now runs IsSignalMuon in FillMuon
    } else {
      dec_signal(*muon) = dec_baseline(*muon) && IsSignalMuonLocal(i);
    }
  }
  
  if (m_EDM->taus) {
    for(unsigned int i=0;i<m_EDM->taus->size();++i){
      const xAOD::TauJet* tau = m_EDM->getTau(i);
      dec_passOR(*tau) = dec_baseline(*tau);
      // SUSYTools now runs IsSignalTau in FillTau
    }
  }
  
  ////////////////////////////////////////////////////////////////////////
  // OVERLAP REMOVAL
  ////////////////////////////////////////////////////////////////////////
  
  // doHarmonization is first option.
  //m_SUSYObjDef->OverlapRemoval(m_EDM->electrons,m_EDM->muons,m_EDM->jets,false).isSuccess();
  MSG_DEBUG("Overlap removal start");
  OverlapRemoval(m_EDM->electrons,m_EDM->muons,m_EDM->jets,false);
  MSG_DEBUG("Overlap removal end");

  // Nominally ApplyLeptonVeto does nothing, but the option is used in XSUSYObjDefAlgExpert.
  ApplyLeptonVeto();

  // Cosmics rejection cuts (function IsCosmicMuon in SUSYTools 
  // seeting the cosmic decoration to the objects):
  // abs(z0PV) < 1 mm AND abs(d0PV) < 0.2 mm . Be careful: these cuts should be applied after
  // overlap removal to avoid removing muons from heavy flavor decays.
  for(unsigned int i=0;i<m_EDM->muons->size();++i){
    const xAOD::Muon* muon = m_EDM->getMuon(i);
    if (dec_passOR(*muon) && dec_baseline(*muon) && dec_cosmic(*muon)) {
      m_evtdef.m_passCosmicMuon = false;
      break;
    }
  }
  
  // dec_passOR is set by OverlapRemoval. Only if dec_baseline is true.

  for(unsigned int i=0;i<m_EDM->jets->size();++i){
    const xAOD::Jet* jet = m_EDM->getJet(i);
    if (dec_baseline(*jet) && dec_passOR(*jet)) { // 
      m_evtdef.n_baseline_jet++;
    }
    if (dec_baseline(*jet) && dec_passOR(*jet)) { // overlap removal
      Particle p(jet->p4(),ObjType::Jet,i,0);
      
      // Bjet
#ifndef BEFORE_SUSYTOOLS_000611
      // IsSignalJet is called in GetJets
      // IsBjet is called in GetJets
      p.is_Bjet = dec_bjet(*jet);
#elif defined(ISREL20)
      p.is_Bjet = m_SUSYObjDef->IsBJet(*jet,jet_bjet_mv1);
#else
      p.is_Bjet = m_SUSYObjDef->IsBJet(*jet,/*is13TeV*/true,jet_bjet_mv1);
#endif
      // end bjet
      
#ifdef BEFORE_SUSYTOOLS_000611
      m_evtdef.AddSignalJet(p);
      if (!m_SUSYObjDef->IsGoodJet(*jet)) m_evtdef.m_passBadJet = false;
#else
      if (dec_signal(*jet)) {
        m_evtdef.AddSignalJet(p);
      }
      if (dec_bad(*jet)) m_evtdef.m_passBadJet = false;
#endif
      if (jet->pt() < 20000.) MSG_INFO("Error!!!");
      MSG_DEBUG("Signal jet (" << jet->pt() << ",eta: " << jet->eta() << 
               ",phi: " << jet->phi() << ") " );
    }
  }
  
  for(unsigned int i=0;i<m_EDM->electrons->size();++i){
    const xAOD::Electron* ele = m_EDM->getElectron(i);
    if (dec_baseline(*ele) && dec_passOR(*ele)) m_evtdef.n_baseline_ele++;
    if (do_antiid && dec_passOR(*ele) && dec_baseline(*ele) && IsAntiIDElectron(i)) {
      m_evtdef.AddAntiIDLepton(Particle(ele->p4(),ObjType::Electron,i,ele->charge()));
    }
    // use corrected pt!
    if (dec_signal(*ele) && dec_passOR(*ele) && dec_baseline(*ele) && ele->pt() > ele_ptmin*1000.){
      float sf = 1;
      if (!m_skipsf) sf = m_EDM->GetContainerEleSF_RecoIDIso(i,ele_id,1/*is_d0z0*/,ele_isowp);
      Particle p(ele->p4(),ObjType::Electron,i,ele->charge(),sf,/*trigsf*/1.);
      m_evtdef.AddSignalLepton(p);
    }
    int type = m_EDM->TruthType(*ele);
    int origin = m_EDM->TruthOrigin(*ele);
    MSG_DEBUG("Particle type " << type << " origin " << origin);
  }

  if (m_EDM->taus) {
    for(unsigned int i=0;i<m_EDM->taus->size();++i){
      const xAOD::TauJet* tau = m_EDM->getTau(i);
      if (dec_baseline(*tau)) m_evtdef.n_baseline_tau++;
      if (dec_signal(*tau) && dec_passOR(*tau) && dec_baseline(*tau)) 
        m_evtdef.AddSignalTau(Particle(tau->p4(),ObjType::Tau,i,tau->charge()));
    }
  }
  
  for(unsigned int i=0;i<m_EDM->muons->size();++i){
    const xAOD::Muon* muon = m_EDM->getMuon(i);
    if (dec_baseline(*muon) && (dec_passOR(*muon) || keep_overlapremoved_muons)) m_evtdef.n_baseline_mu++;
    
    if (dec_signal(*muon) && (dec_passOR(*muon) || keep_overlapremoved_muons) && dec_baseline(*muon)) {
      float sf = 1.;
      if (!m_skipsf) sf = m_EDM->GetContainerMuonSF_IDIsoTTVA(i,mu_id,mu_isowp);
      Particle p(muon->p4(),ObjType::Muon,i,muon->charge(),sf,/*trigsf*/1.);
      p.passOR = (bool)dec_passOR(*muon);
      m_evtdef.AddSignalLepton(p);
    }
  }
  
  //m_EDM->TriggerMatch("e24_lhmedium_iloose");

  m_evtdef.BuildEvent();
  m_EDM->SetEventDefinitionPointer(&m_evtdef);
  m_EDM->FillTriggerBits();
  
  //xAOD::ElectronContainer::iterator it = m_EDM->electrons->begin();
  //xAOD::ElectronContainer::iterator it_end = m_EDM->electrons->end();
  // for(;it != it_end; ++it){
  //   m_SUSYObjDef->IsSignalElectronExp(**it,ST::SignalIsoExp::TightIso);
  // }
  
  MSG_VERBOSE("loop end");
}

//-----------------------------------------------------------------------------
void PSL::XSUSYObjDefAlgV7::finish(void){
  if (m_SUSYObjDef) { delete m_SUSYObjDef; m_SUSYObjDef = 0;}
  MSG_DEBUG("finish.");
}

//-----------------------------------------------------------------------------
void PSL::XSUSYObjDefAlgV7::PrintConfiguration(void){
  MSG_INFO("-------------------------------SUSYObjDef Configuration:-------------------------------");
  MSG_INFO("ConfigFile (your conf) : " << m_ConfigFile           );
  MSG_INFO("sysVariation           : " << m_sysVariation         );
  MSG_INFO("ele_ptmin_base         : " << ele_ptmin_base         );
  MSG_INFO("ele_ptmin              : " << ele_ptmin              );
  MSG_INFO("mu_ptmin_base          : " << mu_ptmin_base          );
  MSG_INFO("mu_ptmin               : " << mu_ptmin               );
  MSG_INFO("tau_ptmin              : " << tau_ptmin              );
  MSG_INFO("jet_ptmin              : " << jet_ptmin              );
  MSG_INFO("ele_eta_max            : " << ele_eta_max            );
  MSG_INFO("mu_eta_max             : " << mu_eta_max             );
  MSG_INFO("tau_eta_max            : " << tau_eta_max            );
  MSG_INFO("jet_eta_max            : " << jet_eta_max            );
  MSG_INFO("ele_d0_max             : " << ele_d0_max             );
  MSG_INFO("ele_d0_max_antiid      : " << ele_d0_max_antiid      );
  MSG_INFO("ele_d0_min_antiid      : " << ele_d0_min_antiid      );
  MSG_INFO("ele_z0_max             : " << ele_z0_max             );
  MSG_INFO("ele_z0_max_antiid      : " << ele_z0_max_antiid      );
  MSG_INFO("ele_z0_min_antiid      : " << ele_z0_min_antiid      );
  MSG_INFO("ele_isowp              : " << ConvertAnaIsoToStr(ele_isowp));
  MSG_INFO("ele_trkisotype         : " << ConvertAnaIsoToStr(ele_trkisotype));
  MSG_INFO("ele_caloisotype        : " << ConvertAnaIsoToStr(ele_caloisotype));
  MSG_INFO("ele_trkiso_max         : " << ele_trkiso_max         );
  MSG_INFO("ele_trkiso_max_antiid  : " << ele_trkiso_max_antiid  );
  MSG_INFO("ele_trkiso_min_antiid  : " << ele_trkiso_min_antiid  );
  MSG_INFO("ele_caloiso_max        : " << ele_caloiso_max        );
  MSG_INFO("ele_caloiso_max_antiid : " << ele_caloiso_max_antiid );
  MSG_INFO("ele_caloiso_min_antiid : " << ele_caloiso_min_antiid );
  MSG_INFO("ele_id_base            : " << ConvertElectronIDToStr(ele_id_base));
  MSG_INFO("ele_id                 : " << ConvertElectronIDToStr(ele_id));
  MSG_INFO("ele_id_fail_antiid      : " << ConvertElectronIDToStr(ele_id_fail_antiid));
  MSG_INFO("ele_id_pass_antiid      : " << ConvertElectronIDToStr(ele_id_pass_antiid));
  MSG_INFO("mu_d0_max              : " << mu_d0_max              );
  MSG_INFO("mu_d0_max_antiid       : " << mu_d0_max_antiid       );
  MSG_INFO("mu_d0_min_antiid       : " << mu_d0_min_antiid       );
  MSG_INFO("mu_z0_max              : " << mu_z0_max              );
  MSG_INFO("mu_z0_max_antiid       : " << mu_z0_max_antiid       );
  MSG_INFO("mu_z0_min_antiid       : " << mu_z0_min_antiid       );
  MSG_INFO("mu_isowp               : " << ConvertAnaIsoToStr(mu_isowp));
  MSG_INFO("mu_trkisotype          : " << ConvertAnaIsoToStr(mu_trkisotype));
  MSG_INFO("mu_caloisotype         : " << ConvertAnaIsoToStr(mu_caloisotype));
  MSG_INFO("mu_trkiso_max          : " << mu_trkiso_max          );
  MSG_INFO("mu_trkiso_max_antiid   : " << mu_trkiso_max_antiid   );
  MSG_INFO("mu_trkiso_min_antiid   : " << mu_trkiso_min_antiid   );
  MSG_INFO("mu_caloiso_max         : " << mu_caloiso_max         );
  MSG_INFO("mu_caloiso_max_antiid  : " << mu_caloiso_max_antiid  );
  MSG_INFO("mu_caloiso_min_antiid  : " << mu_caloiso_min_antiid  );
  MSG_INFO("jet_forward_eta_min    : " << jet_forward_eta_min    );
  MSG_INFO("jet_forward_eta_max    : " << jet_forward_eta_max    );
  MSG_INFO("jet_forward_ptmin      : " << jet_forward_ptmin      );
  MSG_INFO("jet_bjet_ptmin         : " << jet_bjet_ptmin         );
  MSG_INFO("jet_bjet_mv1           : " << jet_bjet_mv1           );
  MSG_INFO("jet_central_jvfeta_max : " << jet_central_jvfeta_max );
  MSG_INFO("jet_central_eta_max    : " << jet_central_eta_max    );
  MSG_INFO("jet_central_ptmin      : " << jet_central_ptmin      );
  MSG_INFO("jet_central_mv1        : " << jet_central_mv1        );
  MSG_INFO("do_overlapremove       : " << (do_overlapremove ? "yes" : "no"));
  MSG_INFO("do_antiid              : " << (do_antiid ? "yes" : "no"));
  MSG_INFO("keep_overlapremoved_muons: " << (keep_overlapremoved_muons ? "yes" : "no"));
  MSG_INFO("skipsf                 : " << (m_skipsf ? "yes" : "no"));
  if (m_SUSYObjDef) {
    MSG_INFO("SUSYTools::isData()    : " << m_SUSYObjDef->isData());
    MSG_INFO("SUSYTools::isAtlfast() : " << m_SUSYObjDef->isAtlfast());
  }
  MSG_INFO("---------------------------------------------------------------------------------------");
}

void PSL::XSUSYObjDefAlgV7::OverlapRemoval(const xAOD::ElectronContainer *electrons, const xAOD::MuonContainer *muons, const xAOD::JetContainer *jets,const bool doHarmonization,const double dRejet, const double dRjetmu, const double dRjete, const double dRemu, const double dRee){
#ifdef BEFORE_SUSYTOOLS_000624
  m_SUSYObjDef->OverlapRemoval(electrons,muons,jets,doHarmonization).isSuccess();
#else
  m_SUSYObjDef->OverlapRemoval(electrons,muons,jets).isSuccess();
#endif //BEFORE_SUSYTOOLS_000624
  (void)dRejet;
  (void)dRjetmu;
  (void)dRjete;
  (void)dRemu;
  (void)dRee;
}

#endif // BEFORE_SUSYTOOLS_000709
#endif // SKIP_XAOD_COMPILATION
