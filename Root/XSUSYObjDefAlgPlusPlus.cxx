//////////////////////////////////////////////////////////
//
// Alternate version of XSUSYObjDefAlg for implimenting
//   SMWZ selection criteria
//
//////////////////////////////////////////////////////////

#include <pennSoftLepton/XSUSYObjDefAlgPlusPlus.h>
#ifndef SKIP_XAOD_COMPILATION
//#ifdef BEFORE_SUSYTOOLS_000709

#include <EventLoop/Worker.h>

#include "SUSYTools/SUSYObjDef_xAOD.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODMissingET/MissingET.h"
#include "JetMomentTools/JetVertexTaggerTool.h"
#include "JetSelectorTools/JetCleaningTool.h"
#include "TrigEgammaMatchingTool/TrigEgammaMatchingTool.h"
#include "TrigMuonMatching/TrigMuonMatching.h"

#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
#include "IsolationCorrections/IsolationCorrectionTool.h"

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::XSUSYObjDefAlgPlusPlus)
  
  
//-----------------------------------------------------------------------------
PSL::XSUSYObjDefAlgPlusPlus::XSUSYObjDefAlgPlusPlus() : ObjectSelectionBase()
  ,m_makefakentuples      (false)
  ,m_SUSYObjDef           (0)
  ,dec_baseline           ("baseline")
  ,dec_signal             ("signal")
  ,dec_passOR             ("passOR")
  ,dec_badjet             ("bad")
  ,dec_bjet               ("bjet")
  ,dec_bad                ("bad")
  ,dec_cosmic             ("cosmic")
  ,dec_zlep               ("zlep")
  ,dec_wlep               ("wlep")
  ,dec_passOR_ll          ("passOR_ll")
  ,dec_passOR_MuDecision  ("passOR_MuORdec")
  ,dec_passOR_JetClean    ("passOR_JetClean")
  ,dec_baselineJetPt20    ("baselineJetPt20")
{
  MSG_DEBUG("Constructor");
}

//-----------------------------------------------------------------------------
bool PSL::XSUSYObjDefAlgPlusPlus::init(void)
{
  if (!ObjectSelectionBase::init()) return false;

  m_SUSYObjDef = new ST::SUSYObjDef_xAOD("SUSYObjDef_xAOD");
  int isData = !((wk()->metaData()->castString("IsData")).empty());
  bool isAtlFast = (wk()->metaData()->castString("Simulation") == "atlfast");
  bool isFullSim = (wk()->metaData()->castString("Simulation") == "fullsim");
  if (isData    ) m_SUSYObjDef->setProperty("DataSource",ST::Data).isSuccess();
  if (isAtlFast ) m_SUSYObjDef->setProperty("DataSource",ST::AtlfastII).isSuccess();
  if (isFullSim ) m_SUSYObjDef->setProperty("DataSource",ST::FullSim).isSuccess();
  MSG_INFO("isAtlFast flag is set to: " << isAtlFast);
  // m_SUSYObjDef->setProperty("EleIsoWP","GradientLoose").isSuccess();
  // m_SUSYObjDef->setProperty("MuIsoWP","GradientLoose").isSuccess();
  //PathResolverSetOutputLevel(MSG::ERROR);
  
#ifdef BEFORE_ANALYSISBASE_2p3p41
  m_SUSYObjDef->setProperty("PhotonIsoWP","FixedCutLoose").isSuccess();

  //m_SUSYObjDef->setProperty("EleID"        ,ConvertElectronIDToStr(ele_id)     ).isSuccess();
  m_SUSYObjDef->setProperty("Ele.Id"        ,"TightLLH"               ).isSuccess();
  //m_SUSYObjDef->setProperty("EleIdBaseline",ConvertElectronIDToStr(ele_id_base)).isSuccess();
  m_SUSYObjDef->setProperty("EleIdBaseline","LooseAndBLayerLLH"      ).isSuccess();
  //m_SUSYObjDef->setProperty("EleIdFallback","MediumLH"               ).isSuccess();
  m_SUSYObjDef->setProperty("EleIdFallback","MediumLLH"              ).isSuccess();
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
#endif // BEFORE_ANALYSISBASE_2p3p41

  // The following lines are intended to override tools that SUSYTools tries
  // to create, resulting in duplicate or buggy tools that we do not use.
  std::string str_idsf = "efficiencySF.offline.TightLLH_d0z0";
  ToolHandle<IAsgElectronEfficiencyCorrectionTool> sftool_id = asg::ToolStore::get<AsgElectronEfficiencyCorrectionTool>(str_idsf);
  if (!m_SUSYObjDef->setProperty("ElectronEfficiencyCorrectionTool_reco",sftool_id).isSuccess()) {
    MSG_INFO("Could not overwrite ElectronEfficiencyCorrectionTool_reco.");
    return false;
  }
  if (!m_SUSYObjDef->setProperty("ElectronEfficiencyCorrectionTool_trig_singleLep",sftool_id).isSuccess()) {
    MSG_INFO("Could not overwrite ElectronEfficiencyCorrectionTool_trig_singleLep.");
    return false;
  }
  if (!m_SUSYObjDef->setProperty("ElectronEfficiencyCorrectionTool_trig_diLep",sftool_id).isSuccess()) {
    MSG_INFO("Could not overwrite ElectronEfficiencyCorrectionTool_trig_diLep.");
    return false;
  }
  if (!m_SUSYObjDef->setProperty("ElectronEfficiencyCorrectionTool_trig_mixLep",sftool_id).isSuccess()) {
     MSG_INFO("Could not overwrite ElectronEfficiencyCorrectionTool_trig_mixLep.");
    return false;
  }
  if (!m_SUSYObjDef->setProperty("ElectronEfficiencyCorrectionTool_id",sftool_id).isSuccess()) {
    MSG_INFO("Could not overwrite ElectronEfficiencyCorrectionTool_id.");
    return false;
  }
  if (!m_SUSYObjDef->setProperty("ElectronEfficiencyCorrectionTool_iso",sftool_id).isSuccess()) {
    MSG_INFO("Could not overwrite ElectronEfficiencyCorrectionTool_iso.");
    return false;
  }
  // End of SUSYTools Tool Override code.
    
  if (!m_SUSYObjDef->setProperty("ConfigFile", "$ROOTCOREBIN/../pennSoftLepton/config/"+m_ConfigFile).isSuccess()){
    MSG_INFO("Could not set SUSYTools ConfigFile.");
    return false;
  }

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
      MSG_ERROR("This is an allowed systematic variation " << sys.name().c_str());
    } //looping over allowed systematics

    if (systematicVariation.empty()) {
      MSG_INFO( "I couldn't find " << m_sysVariation << " in the list of allowed systematic variations, so I'll just run a nominal job");
    }

    if ((m_SUSYObjDef->applySystematicVariation(systematicVariation) != CP::SystematicCode::Ok) ){
      MSG_WARNING( "Cannot configure SUSYTools for systematic variation " << systematicVariation.name() );
      return false;
    } 
    if (!m_EDM->applySystematicVariation(systematicVariation)) return false;
    MSG_INFO("Variation configured... "<< systematicVariation.name() );
  } // asking for systematic

  // Local Trigger Matching tools since the ones in SUSYTools always return false?
  m_EgammaMatchTool = new Trig::TrigEgammaMatchingTool("EgammaTrigMatchTool");
  ToolHandle<Trig::TrigDecisionTool> dec_tool = asg::ToolStore::get<Trig::TrigDecisionTool>("TrigDecisionTool");
  m_EgammaMatchTool->setProperty("TriggerTool",dec_tool).ignore();
  m_EgammaMatchTool->initialize().ignore();
  m_MuonMatchTool = new Trig::TrigMuonMatching("MuonTrigMatchTool");
  m_MuonMatchTool->setProperty("TriggerTool",dec_tool).ignore();
  m_MuonMatchTool->initialize().ignore();

  // set up JVT tool
  m_jetjvf_cut_and_sf = new CP::JetJvtEfficiency("jet_jvt");
  //m_jetjvf_cut_and_sf->setProperty("WorkingPoint","Default");
  if (m_jetjvf_cut_and_sf->initialize().isFailure()) {
    MSG_INFO("jvf tool initialization failed. Exiting.");
    return false;
  }

  // Tool handle for Jet Cleaning Tool
  if ( asg::ToolStore::contains<JetCleaningTool>("JetCleaningTool") ) {
    m_jetCleaningTool = asg::ToolStore::get<JetCleaningTool>("JetCleaningTool");
  } 
  else{
    MSG_ERROR("Failed to get ToolHandle for JetCleaningTool!! Check your SUSYTools setup!!");
    return false;
  }

  // Tool handle for Egamma Trigger Matching Tool
  /*
  if ( asg::ToolStore::contains<Trig::TrigEgammaMatchingTool>("EgammaTrigMatchTool") ) {
    m_EgammaMatchTool = asg::ToolStore::get<Trig::TrigEgammaMatchingTool>("EgammaTrigMatchTool");
  } 
  else{
    MSG_ERROR("Failed to get ToolHandle for TrigEgammaMatchingTool!! Check your SUSYTools setup!!");
    return false;
  }
  // Tool handle for Muon Trigger Matching Tool
  if ( asg::ToolStore::contains<Trig::TrigMuonMatching>("MuonTrigMatchTool") ) {
    m_MuonMatchTool = asg::ToolStore::get<Trig::TrigMuonMatching>("MuonTrigMatchTool");
  }
  else{
    MSG_ERROR("Failed to get ToolHandle for TrigMuonMatchingTool!! Check your SUSYTools setup!!");
    return false;
    }*/

  // Tool handle for MuonCalibrationAndSmearingTool
  if ( asg::ToolStore::contains<CP::MuonCalibrationAndSmearingTool>("MuonCalibrationAndSmearingTool") ) {
    m_muonCalibrationAndSmearingTool = asg::ToolStore::get<CP::MuonCalibrationAndSmearingTool>("MuonCalibrationAndSmearingTool");
    } 
  else{
    MSG_ERROR("Failed to get ToolHandle for MuonCalibrationAndSmearingTool!! Check your SUSYTools setup!!");
    return false;
  }

  // Tool handle for EgammaCalibrationAndSmearingTool
  if ( asg::ToolStore::contains<CP::EgammaCalibrationAndSmearingTool>("EgammaCalibrationAndSmearingTool") ) {
    m_egammaCalibTool = asg::ToolStore::get<CP::EgammaCalibrationAndSmearingTool>("EgammaCalibrationAndSmearingTool");
  } 
  else{
    MSG_ERROR("Failed to get ToolHandle for EgammaCalibrationAndSmearingTool!! Check your SUSYTools setup!!");
    return false;
  }
  
  // Tool handle for IsolationCorrectionTool
  if ( asg::ToolStore::contains<CP::IsolationCorrectionTool>( "IsoCorrTool" ) ) {
    m_isoCorrTool = asg::ToolStore::get<CP::IsolationCorrectionTool>( "IsoCorrTool" );
  } 
  else{
    MSG_ERROR("Failed to get ToolHandle for IsolationCorrectionTool!! Check your SUSYTools setup!!");
    return false;
  }
    
  // Initialize OverlapRemovalTool
#ifndef BEFORE_SUSYTOOLS_000709
  if( ORUtils::harmonizedTools(m_orToolbox, "OverlapRemovalTool", "baseline", "passOR", true, m_doTauOR, m_doPhotonOR).isFailure() ){
    MSG_INFO("Failed to harmonizedTools setup overlap removal toolbox!");
    return false;
  }
  if( ORUtils::harmonizedTools(m_orToolbox_jet,"OverlapRemovalToolJet","baselineJetPt20", "passOR_JetClean", true, m_doTauOR, m_doPhotonOR).isFailure() ){
    MSG_INFO("Failed to harmonizedTools setup overlap removal toolbox for Jet Cleaning!");
  }
#else
  if( ORUtils::harmonizedTools(m_orToolbox,"baseline", "passOR", true, m_doTauOR, m_doPhotonOR).isFailure() ){
    MSG_INFO("Failed to harmonizedTools setup overlap removal toolbox!");
    return false;
  }
  if( ORUtils::harmonizedTools(m_orToolbox_jet,"baselineJetPt20", "passOR_JetClean", true, m_doTauOR, m_doPhotonOR).isFailure() ){
    MSG_INFO("Failed to harmonizedTools setup overlap removal toolbox for Jet Cleaning!");
  }
#endif // BEFORE_SUSYTOOLS_000709
  m_orTool = static_cast<ORUtils::OverlapRemovalTool*>(m_orToolbox.getMasterTool());
  m_orTool->setName("OverlapRemoval");
  if (m_makefakentuples) {
    // We force it so that jet is always removed in the denominator case, by requiring jets
    // to have an absurd number of tracks before the muon is removed.
    static_cast<ORUtils::MuJetOverlapTool*>(m_orToolbox.getTool("MuJetORT"))->setProperty("NumJetTrk",9999999);
  }
  if( m_orToolbox.initialize().isFailure() ){
    MSG_INFO("Failed to initialize overlap removal toolbox");
    return false;
  }
  MSG_INFO("Initialized overlap removal toolbox");
  m_orToolJet = static_cast<ORUtils::OverlapRemovalTool*>(m_orToolbox_jet.getMasterTool());
  m_orTool->setName("OverlapRemovalJet");
  if( m_orToolbox_jet.initialize().isFailure() ){
    MSG_INFO("Failed to initialize jet overlap removal toolbox");
    return false;
  }
  MSG_INFO("Initialized jet overlap removal toolbox");

  // get electron-muon overlap tool
  m_orTool_ll = new ORUtils::EleMuSharedTrkOverlapTool("EleMuORTool");
  m_orTool_ll->setProperty("InputLabel","baseline");
  m_orTool_ll->setProperty("OutputLabel","passOR_ll");
  m_orTool_ll->setProperty("OutputPassValue",true);
  if( m_orTool_ll->initialize().isFailure() ){
    MSG_INFO("Failed to initialize ll-overlap removal tool");
    return false;
  }
  MSG_INFO("Initialized ll-overlap removal tool");

  // Initialize OverlapRemovalTool for Mu decision only
  if (m_makefakentuples) {
#ifndef BEFORE_SUSYTOOLS_000709
    if( ORUtils::harmonizedTools(m_orToolboxPopulateMuOrDecision, "ORTool_MuOROnly", "baseline", "passOR_MuORdec", true, m_doTauOR, m_doPhotonOR).isFailure() ){
      MSG_INFO("Failed to harmonizedTools setup overlap removal toolbox!"); return false;
    }
#else
    if( ORUtils::harmonizedTools(m_orToolboxPopulateMuOrDecision,"baseline", "passOR_MuORdec", true, m_doTauOR, m_doPhotonOR).isFailure() ){
      MSG_INFO("Failed to harmonizedTools setup overlap removal toolbox!"); return false;
    }
#endif // BEFORE_SUSYTOOLS_000709
    m_orToolPopulateMuOrDecision = static_cast<ORUtils::OverlapRemovalTool*>(m_orToolboxPopulateMuOrDecision.getMasterTool());
    m_orToolPopulateMuOrDecision->setName("ORTool_MuOROnly");
    if( m_orToolboxPopulateMuOrDecision.initialize().isFailure() ){
      MSG_INFO("Failed to initialize overlap removal toolbox"); return false;
    }
    MSG_INFO("Initialized overlap removal toolbox for final Mu OR decision");
  }

  // initialize object cut flow histos
  setupObjHistos();

  PrintConfiguration();
  return true;
}

//-----------------------------------------------------------------------------
void PSL::XSUSYObjDefAlgPlusPlus::loop(void){

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

  //m_SUSYObjDef->GetElectrons(m_EDM->electrons,m_EDM->electrons_aux,/*recordSG*/false).isSuccess();
  GetElectrons(m_EDM->electrons,m_EDM->electrons_aux).isSuccess();
  MSG_DEBUG("Number of elecs: " << m_EDM->electrons->size() );

  //m_SUSYObjDef->GetMuons(m_EDM->muons,m_EDM->muons_aux,/*recordSG*/false).isSuccess();
  GetMuons(m_EDM->muons,m_EDM->muons_aux).isSuccess();
  MSG_DEBUG("Number of muons: " << m_EDM->muons->size() );

#ifndef ISREL20 // SUSY2 (which we are switching to starting in Rel20) does not save taus
  m_SUSYObjDef->GetTaus(m_EDM->taus,m_EDM->taus_aux).isSuccess();
  MSG_DEBUG("Number of taus: " << m_EDM->taus->size() );
#endif

  m_evtdef.Reset();
  m_EDM->Reset();

  ////////////////////////////////////////////////////////////////////////
  // BASELINE OBJECTS
  ////////////////////////////////////////////////////////////////////////

  // for harmonization, signal lepton determination has to happen before overlap removal
  // jets: need to find b-jets, for lepton veto purposes
  for(unsigned int i=0;i<m_EDM->jets->size();++i){
    const xAOD::Jet* jet = m_EDM->getJet(i);
    dec_baseline(*jet) = false;
    dec_badjet(*jet) = false;
    dec_baselineJetPt20(*jet) = false;
    // fill special decorator for jet-jetOR (baseline w/ pT > 20. GeV)
    if(isBaselineJet(i,true)){
      dec_baselineJetPt20(*jet) = true;
      //std::cout << "Pass baselineJetPt20 with pT " << jet->pt() << std::endl;
    }
    // check baseline jets (with normal 25 GeV cut)
    if(!isBaselineJet(i,false)) continue;
    dec_baseline(*jet) = true;

    // hopefully this is redundant, but I needed to put this in for SUSYTools-00-05-00-31
    //if (fabs(jet->eta()) > 2.8) dec_baseline(*jet) = false;
    //dec_passOR(*jet) = dec_baseline(*jet);
  }
  
  // --- select baseline electrons & muons. We will do OR next
  for(unsigned int i=0;i<m_EDM->electrons->size();++i){
    const xAOD::Electron* ele = m_EDM->getElectron(i);
    // initialize decorators to false
    dec_baseline(*ele) = false;
    dec_baselineJetPt20(*ele) = false;
    dec_zlep(*ele) = false;
    dec_wlep(*ele) = false;
    // baseline
    if(!isBaselineElectron(i)) continue;
    dec_baseline(*ele) = true;
    dec_baselineJetPt20(*ele) = true;
  }

  for(unsigned int i=0;i<m_EDM->muons->size();++i){
    const xAOD::Muon* muon = m_EDM->getMuon(i);
    // initialize decorators to false
    dec_baseline(*muon) = false;
    dec_baselineJetPt20(*muon) = false;
    dec_zlep(*muon) = false;
    dec_wlep(*muon) = false;
    // baseline
    if(!isBaselineMuon(i)) continue;
    dec_baseline(*muon) = true;
    dec_baselineJetPt20(*muon) = true;
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
  
  // So overlap removal is super fun in this version of the cut flow.  Basically we need to do OR between
  // only the leptons for the first step (don't do OR between jets & leptons).  So we're gonna do that 
  // first, then update cut flow numbers and baseline stuff.
  // Then we do the full OR, including the lepton-jet OR, so that will be done later on, but before we
  // do the Z-lepton selection.  Hopefully this works.........

  // --- Start lepton-lepton overlap removal ---
  if(do_overlapremove){
    MSG_DEBUG("Overlap removal start (lepton-lepton)");
    if( m_orTool_ll->findOverlaps(*m_EDM->electrons,*m_EDM->muons).isFailure() )
      MSG_WARNING("Overlap removal (lepton-lepton) returned StatusCode::FAILURE");
    MSG_DEBUG("Overlap removal end (lepton-lepton)");
  }
  else{
    MSG_DEBUG("Not doing overlap removal because do_overlapremove = false");
  }

  // Now we need to adjust the dec_baseline(*lepton) decorators with the results from the minimal overlap
  // removal, since this is a part of the baseline definition
  if(do_overlapremove){
    for(unsigned int i=0;i<m_EDM->electrons->size();++i){
      const xAOD::Electron* ele = m_EDM->getElectron(i);
      if(!dec_baseline(*ele)) // if it fails baseline game over
	continue;
      if(!dec_passOR_ll(*ele)){ // if it fails ll OR then set baseline = false
	dec_baseline(*ele) = false;
	continue;
      }
      passBaselineElectron->Fill(8.); // update baseline cut flow histogram
    }
    for(unsigned int i=0;i<m_EDM->muons->size();++i){
      const xAOD::Muon* muon = m_EDM->getMuon(i);
      if(!dec_baseline(*muon)) // if it fails baseline game over
	continue;
      if(!dec_passOR_ll(*muon)){ // if it fails ll OR then set baseline = false
	dec_baseline(*muon) = false;
	continue;
      }
      passBaselineMuon->Fill(8.); // update baseline cut flow histogram
    }
  }

  // --- End lepton-lepton overlap removal ---

  // --- Start MET dectorating, etc. ---
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

  // for(unsigned int i=0;i<m_EDM->met->size();++i){
  //   xAOD::MissingET* metterm = m_EDM->met->at(i);
  //   MSG_INFO(Form("MET term %s px = %f py = %f",
  //                 metterm->name().c_str(), metterm->mpx(), metterm->mpy()));
  //   MSG_INFO("Source: " << metterm->source());
  //   //MSG_INFO("refEle: " << xAOD::MissingETBase::Source::Electron);
  // }
  // --- End MET dectorating, etc ---

  ////////////////////////////////////////////////////////////////////////////////
  // START REAL (FULL) OVERLAP REMOVAL
  ////////////////////////////////////////////////////////////////////////////////

  // This should overwrite the dec_passOR(*lepton) set earlier
  /*
    A comment on the additional jet-lepton OR functions: they are not performed simultaneously due to 
    the fact that the recommended OR prescription is done in steps.  
    - Removing jets due to electrons is the second step (after electron-muon OR that is performed 
      manually earlier)
    - Removing jets due to muons is the last step in the OR
    Therefore, in order to respect this hierarchy as closely as possible, the jet-electron OR is done
    first, followed by the full OR (which should NOT overwrite the previous step -- I think), and lastly
    the full OR will be overwritten by the jet-muon OR
   */
  if(do_overlapremove){
    MSG_DEBUG("Overlap removal start (full)");

    // separate jet-electron OR with pt > 20GeV BL jets
    //for(unsigned int i=0;i<m_EDM->jets->size();++i){
    //  const xAOD::Jet *jet = m_EDM->getJet(i);
    //  if(!dec_baselineJetPt20(*jet)) continue;
    //  if(passJetElectronOR(i)) dec_passOR(*jet) = true;
    //}

    // jet OR tool
        if( m_orToolJet->removeOverlaps(m_EDM->electrons,m_EDM->muons,m_EDM->jets).isFailure() )
      MSG_WARNING("Overlap removal (jet) returned StatusCode::FAILURE");

    // full OR tool
    if( m_orTool->removeOverlaps(m_EDM->electrons,m_EDM->muons,m_EDM->jets).isFailure() )
      MSG_WARNING("Overlap removal (full) returned StatusCode::FAILURE");

    // separate jet-muon OR with pt > 20GeV BL jets
    //for(unsigned int i=0;i<m_EDM->jets->size();++i){
    //  const xAOD::Jet *jet = m_EDM->getJet(i);
    //  if(!dec_baselineJetPt20(*jet)) continue;
    //  if(dec_passOR(*jet) && passJetMuonOR(i)) dec_passOR(*jet) = true;
    //}

    MSG_DEBUG("Overlap removal end (full)");
    if (m_makefakentuples) {
      if( m_orToolPopulateMuOrDecision->removeOverlaps(m_EDM->electrons,m_EDM->muons,m_EDM->jets).isFailure() )
        MSG_WARNING("Overlap removal (for Mu decision) returned StatusCode::FAILURE");
    }
    // count OR jets and fill jet cut flow histogram
    for(unsigned int i=0;i<m_EDM->jets->size();++i){
      const xAOD::Jet* jet = m_EDM->getJet(i);
      if(dec_baseline(*jet) && dec_passOR(*jet))
	passBaselineJet->Fill(4.);
    }
  }
  else{
    MSG_DEBUG("Not doing overlap removal because do_overlapremove = false");
  }
  // --- End real (full) overlap removal---

  // Nominally ApplyLeptonVeto does nothing, but the option is used in XSUSYObjDefAlgPlusPlusExpert.
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
  
  ////////////////////////////////////////////////////////////////////////
  // Z & W SIGNAL OBJECTS
  ////////////////////////////////////////////////////////////////////////

  for(unsigned int i=0;i<m_EDM->electrons->size();++i){
    const xAOD::Electron* ele = m_EDM->getElectron(i);
    if(!dec_baseline(*ele)) // fails baseline
      continue;
    passZElectron->Fill(0.);
    if(do_overlapremove && !dec_passOR(*ele)) // if we're checking OR and it fails
      continue;
    passZElectron->Fill(1.);
    // z lepton
    if(!isZElectron(i)) continue;
    dec_zlep(*ele) = true;
    // w lepton
    if(isWElectron(i))
      dec_wlep(*ele) = true;
  }
  for(unsigned int i=0;i<m_EDM->muons->size();++i){
    const xAOD::Muon* muon = m_EDM->getMuon(i);
    if(!dec_baseline(*muon)) // fails either baseline or overlap removal
      continue;
    passZMuon->Fill(0.);
    if(do_overlapremove && !dec_passOR(*muon)) // if we're checking OR and it fails
      continue;
    passZMuon->Fill(1.);
    // z lepton
    if(!isZMuon(i)) continue;
    dec_zlep(*muon) = true;
    // w lepton
    if(isWMuon(i))
      dec_wlep(*muon) = true;
  }

  ////////////////////////////////////////////////////////////////////////
  // SOME JET STUFF
  ////////////////////////////////////////////////////////////////////////

  // dec_passOR is set by OverlapRemoval. Only if dec_baseline is true.

  for(unsigned int i=0;i<m_EDM->jets->size();++i){
    const xAOD::Jet* jet = m_EDM->getJet(i);
    // fill bad jet flag -- we check all jets that pass OR
    if(dec_passOR_JetClean(*jet) || !do_overlapremove){
      if(isBadJet(i)){
	dec_badjet(*jet) = true;
	m_evtdef.m_passBadJet = false;
      }
    }
    // fill baseline/signal jets
    if (dec_baseline(*jet) && (dec_passOR(*jet) || !do_overlapremove) ) { // 
      m_evtdef.n_baseline_jet++;
      Particle p(jet->p4(),ObjType::Jet,i,0);
      
      // Bjet
#ifndef BEFORE_SUSYTOOLS_000611
      // m_SUSYObjDef->IsSignalJet(*jet); // now called in GetJets
      p.is_Bjet = dec_bjet(*jet);
#elif defined(ISREL20)
      p.is_Bjet = m_SUSYObjDef->IsBJet(*jet,jet_bjet_mv1);
#else
      p.is_Bjet = m_SUSYObjDef->IsBJet(*jet,/*is13TeV*/true,jet_bjet_mv1);
#endif
      // end bjet
      
      m_evtdef.AddSignalJet(p); // our "signal jets" are the same as the baseline jets
    }

  }
  
  ////////////////////////////////////////////////////////////////////////
  // ADD SIGNAL OBJECTS TO EVTDEF
  ////////////////////////////////////////////////////////////////////////

  // so here's the plan:
  // - we don't care about baseline leptons anymore, so now the Z-leptons are the "signal leptons"
  // - pass Z-leptons to m_evtdef as AddSignalLepton()
  // - build event does the matching and stuff
  // - check if w lepton from build event passed W selection 

  for(unsigned int i=0;i<m_EDM->electrons->size();++i){
    const xAOD::Electron* ele = m_EDM->getElectron(i);
    if (dec_baseline(*ele)) 
      m_evtdef.n_baseline_ele++;

    // use corrected pt -- i'll have to make sure this doesn't get applied 2x since susy tools does it or something
    if (dec_zlep(*ele)){ // only accepting things that pass z-lepton selection
      float trigsf = 1;
      float sf = 1;
      if (!m_skipsf) sf = m_EDM->GetContainerEleSF_RecoIDIso(i,zele_id,1/*is_d0z0*/,zele_isowp);
      Particle p(ele->p4(),ObjType::Electron,i,ele->charge(),sf,trigsf);
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
    if (dec_baseline(*muon))
      m_evtdef.n_baseline_mu++;
    
    if (dec_zlep(*muon)){ // only accepting things that pass z-lepton selection
      float sf = 1;
      if (muon->pt() > 15000){
        if (!m_skipsf) sf = m_EDM->GetContainerMuonSF_IDIsoTTVA(i,zmu_id,zmu_isowp);
      }
      bool passOR = true;
      if (m_makefakentuples) passOR = dec_passOR_MuDecision(*muon);
      Particle tmp(muon->p4(),ObjType::Muon,i,muon->charge(),sf,1);
      tmp.passOR = passOR;
      m_evtdef.AddSignalLepton(tmp);
    }
  }

  // build the event and check if the w-lepton passes the w-lepton selection
  // if the W-lepton FAILS the selection, the Wlep_index will be set to -1.  This can be cut on
  //   in PassEvent by PassWLepton: Wlep_index >= 0
  m_evtdef.BuildEvent();
  
  int evtdef_wlep_index = m_evtdef.Wlep_index;
  if(evtdef_wlep_index < 0){
    MSG_DEBUG("from EventDefinition: W lepton index = -1, setting m_evtdef.wlep_pass_sel to false");
    m_evtdef.m_wlep_pass_sel = false;
  }
  else{
    int wlep_index_cont = m_evtdef.leps.at(evtdef_wlep_index).i_cont;
    //std::cout << "Evtdef index: " << evtdef_wlep_index << "  lepton index: " << wlep_index << std::endl;
    if(IsElectron(m_evtdef.leps.at(evtdef_wlep_index))){
      const xAOD::Electron* ele = m_EDM->getElectron(wlep_index_cont);
      if(!dec_wlep(*ele)) m_evtdef.m_wlep_pass_sel = false;
      else {
        float sf = 1;
        if (!m_skipsf) sf = m_EDM->GetContainerEleSF_RecoIDIso(wlep_index_cont,ele_id,1/*is_d0z0*/,ele_isowp);
        m_evtdef.leps.at(evtdef_wlep_index).sf_recoIdIso = sf;
      }
    }
    else if(IsMuon(m_evtdef.leps.at(evtdef_wlep_index))){
      const xAOD::Muon* muon = m_EDM->getMuon(wlep_index_cont);
      if(!dec_wlep(*muon)) m_evtdef.m_wlep_pass_sel = false;
      else {
        float sf = 1;
        if (!m_skipsf) sf = m_EDM->GetContainerMuonSF_IDIsoTTVA(wlep_index_cont,mu_id,mu_isowp);
        m_evtdef.leps.at(evtdef_wlep_index).sf_recoIdIso = sf;
      }
    }
    //std::cout << "pass w selection?" << m_evtdef.m_wlep_pass_sel << std::endl;
  }
  m_EDM->SetEventDefinitionPointer(&m_evtdef);

  // Fill trigger stuff
  m_EDM->FillTriggerBits();
  if( (m_EDM->PassTrigger(PSL::tHLT_e24_lhmedium_L1EM20VH) &&  m_SUSYObjDef->isData() ) ||
      (m_EDM->PassTrigger(PSL::tHLT_e24_lhmedium_L1EM18VH) && !m_SUSYObjDef->isData() ) ||
      (m_EDM->PassTrigger(PSL::tHLT_e60_lhmedium)) ||
      (m_EDM->PassTrigger(PSL::tHLT_e120_lhloose)) ){
    m_evtdef.m_pass_trig_ele = true;
  }
  else{ m_evtdef.m_pass_trig_ele = false; }
  //std::cout << "Ele Trigger: " << m_evtdef.m_pass_trig_ele << std::endl;
  if( (m_EDM->PassTrigger(PSL::tHLT_mu20_iloose_L1MU15)) || 
      (m_EDM->PassTrigger(PSL::tHLT_mu50)) ){
    m_evtdef.m_pass_trig_mu = true;
  }
  else{ m_evtdef.m_pass_trig_mu = false; }
  //std::cout << "Mu Trigger: " << m_evtdef.m_pass_trig_mu << std::endl;

  // Trigger matching
  m_evtdef.m_pass_trig_match = false;
  for(unsigned int i = 0; i < m_evtdef.leps.size(); i++){
    bool isMuon = IsMuon(m_evtdef.leps.at(i));
    bool isElectron = IsElectron(m_evtdef.leps.at(i));
    MSG_DEBUG("Checking lepton " << i << " which is a " << (isMuon ? "muon" : "electron"));
    if(isMuon && m_evtdef.m_pass_trig_mu){
      const xAOD::Muon* muon = m_EDM->getMuon(m_evtdef.leps.at(i).i_cont); 
      if(isTriggerMatchedMuon(muon)){
	m_evtdef.m_pass_trig_match = true;
        m_evtdef.leps.at(i).is_trigMatched = true;
      }
    }
    else if(isElectron && m_evtdef.m_pass_trig_ele){
      const xAOD::Electron* electron = m_EDM->getElectron(m_evtdef.leps.at(i).i_cont);
      if(isTriggerMatchedElectron(electron)){
	m_evtdef.m_pass_trig_match = true;
        m_evtdef.leps.at(i).is_trigMatched = true;
      }
    }
  }
  // Trigger scale factors are done in xAODWrapper now!
  // m_evtdef.m_evt_trigger_sf = m_EDM->getTriggerSF_Winter2016WZAnalysis();
  MSG_VERBOSE("loop end");
}

//-----------------------------------------------------------------------------
void PSL::XSUSYObjDefAlgPlusPlus::finish(void){
  if (m_SUSYObjDef) { delete m_SUSYObjDef; m_SUSYObjDef = 0;}
  // maybe write a function to print out the object level cut flows and call it here?
  MSG_DEBUG("finish.");
}

//-----------------------------------------------------------------------------
void PSL::XSUSYObjDefAlgPlusPlus::PrintConfiguration(void){
  MSG_INFO("-------------------------------SUSYObjDef Configuration:-------------------------------");
  MSG_INFO("ConfigFile (your conf) : " << m_ConfigFile           );
  MSG_INFO("sysVariation           : " << m_sysVariation         );
  MSG_INFO("ele_ptmin_base         : " << ele_ptmin_base         );
  MSG_INFO("zele_ptmin             : " << zele_ptmin             );
  MSG_INFO("ele_ptmin              : " << ele_ptmin              );
  MSG_INFO("mu_ptmin_base          : " << mu_ptmin_base          );
  MSG_INFO("zmu_ptmin              : " << zmu_ptmin              );
  MSG_INFO("mu_ptmin               : " << mu_ptmin               );
  MSG_INFO("tau_ptmin              : " << tau_ptmin              );
  MSG_INFO("jet_ptmin              : " << jet_ptmin              );
  MSG_INFO("ele_eta_max            : " << ele_eta_max            );
  MSG_INFO("mu_eta_max             : " << mu_eta_max             );
  MSG_INFO("tau_eta_max            : " << tau_eta_max            );
  MSG_INFO("jet_eta_max            : " << jet_eta_max            );
  MSG_INFO("ele_d0_max             : " << ele_d0_max             );
  MSG_INFO("zele_d0_max             : " << zele_d0_max             );
  MSG_INFO("ele_d0_max_antiid      : " << ele_d0_max_antiid      );
  MSG_INFO("ele_d0_min_antiid      : " << ele_d0_min_antiid      );
  MSG_INFO("ele_z0_max             : " << ele_z0_max             );
  MSG_INFO("ele_z0_max_antiid      : " << ele_z0_max_antiid      );
  MSG_INFO("ele_z0_min_antiid      : " << ele_z0_min_antiid      );
  MSG_INFO("ele_isowp              : " << ConvertAnaIsoToStr(ele_isowp));
  MSG_INFO("zele_isowp             : " << ConvertAnaIsoToStr(zele_isowp));
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
  MSG_INFO("zele_id                 : " << ConvertElectronIDToStr(zele_id));
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
  MSG_INFO("zmu_ptmin              : " << zmu_ptmin              );
  MSG_INFO("zmu_d0_max             : " << zmu_d0_max             );
  MSG_INFO("zmu_z0_max             : " << zmu_z0_max             );
  MSG_INFO("zmu_isowp              : " << zmu_isowp              );
  MSG_INFO("zmu_trkisotype         : " << zmu_trkisotype         );
  MSG_INFO("zmu_caloisotype        : " << zmu_caloisotype        );
  MSG_INFO("zmu_trkiso_max         : " << zmu_trkiso_max         );
  MSG_INFO("zmu_caloiso_max        : " << zmu_caloiso_max        );
  MSG_INFO("mu_id_base             : " << ConvertMuonIDToStr(mu_id_base));
  MSG_INFO("mu_id                  : " << ConvertMuonIDToStr(mu_id));
  MSG_INFO("mu_id_pass_antiid      : " << ConvertMuonIDToStr(mu_id_pass_antiid));
  MSG_INFO("zmu_id                 : " << ConvertMuonIDToStr(zmu_id));
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
  MSG_INFO("makefakentuples        : " << (m_makefakentuples ? "yes" : "no"));
  MSG_INFO("skipsf                 : " << (m_skipsf ? "yes" : "no"));
  if (m_SUSYObjDef) {
    MSG_INFO("SUSYTools::isData()    : " << m_SUSYObjDef->isData());
    MSG_INFO("SUSYTools::isAtlfast() : " << m_SUSYObjDef->isAtlfast());
  }
  MSG_INFO("---------------------------------------------------------------------------------------");
}

/*
void PSL::XSUSYObjDefAlgPlusPlus::OverlapRemoval(const xAOD::ElectronContainer *electrons, const xAOD::MuonContainer *muons, const xAOD::JetContainer *jets,const bool doHarmonization,const double dRejet, const double dRjetmu, const double dRjete, const double dRemu, const double dRee){
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
*/

//-----------------------------------------------------------------------------
void PSL::XSUSYObjDefAlgPlusPlus::setupObjHistos(){
  MSG_INFO("Setting up Object-level cut flow histograms");
  // initialize
  passBaselineMuon     = new TH1F("passBaselineMuon",     "Baseline Muon Cut Flow",     10, 0, 10);
  setHistogramOutput(passBaselineMuon);
  passBaselineElectron = new TH1F("passBaselineElectron", "Baseline Electron Cut Flow", 10, 0, 10); 
  setHistogramOutput(passBaselineElectron);
  passZMuon     = new TH1F("passZMuon",     "Z Muon Cut Flow",     10, 0, 10);
  setHistogramOutput(passZMuon);
  passZElectron = new TH1F("passZElectron", "Z Electron Cut Flow", 10, 0, 10); 
  setHistogramOutput(passZElectron);
  passWMuon     = new TH1F("passWMuon",     "W Muon Cut Flow",     10, 0, 10);
  setHistogramOutput(passWMuon);
  passWElectron = new TH1F("passWElectron", "W Electron Cut Flow", 10, 0, 10); 
  setHistogramOutput(passWElectron);
  passBaselineJet = new TH1F("passBaselineJet", "Baseline Jet Cut Flow", 10, 0, 10); 
  setHistogramOutput(passBaselineJet);
  //  name bins
  const char* BaselineLabels[10] = {"Total","OQ (el only)","pT","eta","ID","d0","z0","Iso","Overlap (nojet)"," "};
  const char* ZLabels[10]        = {"Total","Full Overlap","pT","eta (crack,el only)","ID","Iso"," "," "," "," "};
  const char* WLabels[10]        = {"Total","pT","ID","Iso"," "," "," "," "," "," "};
  const char* JLabels[10]        = {"Total","pT","eta","JVF","Overlap"," "," "," "," "," "};
  for(unsigned int i=0; i<10; i++){
    passBaselineMuon->GetXaxis()->SetBinLabel(i+1,BaselineLabels[i]);
    passBaselineElectron->GetXaxis()->SetBinLabel(i+1,BaselineLabels[i]);
    passZMuon->GetXaxis()->SetBinLabel(i+1,ZLabels[i]);
    passZElectron->GetXaxis()->SetBinLabel(i+1,ZLabels[i]);
    passWMuon->GetXaxis()->SetBinLabel(i+1,WLabels[i]);
    passWElectron->GetXaxis()->SetBinLabel(i+1,WLabels[i]);
    passBaselineJet->GetXaxis()->SetBinLabel(i+1,JLabels[i]);
  }
}

// --- Object selection cuts.  
//   cut value naming convention as follows:
//    (1) Baseline: ele_cut_base/mu_cut_base
//    (2) Z-lepton: zele_cut/zmu_cut
//    (3) W-lepton: ele_cut/mu_cut
//-----------------------------------------------------------------------------
bool PSL::XSUSYObjDefAlgPlusPlus::isBaselineMuon(int icontainer){
  // Total muons
  passBaselineMuon->Fill(0.); 
  // pT cut
  double MuonPT = m_EDM->MuonPt(icontainer);
  // NaN check. because apparently that can happen...
  if(MuonPT != MuonPT){
    MSG_INFO("Muon with pT = NaN.  That's cool");
    return false;
  }
  if(m_EDM->MuonPt(icontainer) < mu_ptmin_base*1000.)
    return false;
  passBaselineMuon->Fill(2.);
  // eta cut
  double MuonEta = (m_EDM->GetContainerMuonTLV(icontainer)).Eta();
  if ( fabs(MuonEta) > mu_eta_max ) 
    return false;
  passBaselineMuon->Fill(3.);
  // ID Cut
  xAOD::Muon::Quality mu_qual;
  mu_qual = m_EDM->m_muonSelection->getQuality( *(m_EDM->getMuon(icontainer)) );
  bool passID = m_EDM->m_muonSelection->passedIDCuts( *(m_EDM->getMuon(icontainer)) );
  if( (mu_qual > mu_id_base) || !passID )
    return false;
  passBaselineMuon->Fill(4.);
  // d0 cut
  if( mu_d0_max < fabs(m_EDM->MuonD0Significance(icontainer)) )
    return false;
  passBaselineMuon->Fill(5.);
  // z0 cut
  float z0sintheta = m_EDM->MuonZ0(icontainer)*( sin(m_EDM->GetContainerMuonTLV(icontainer).Theta()) );
  if ( mu_z0_max < fabs(z0sintheta) ) 
    return false; 
  passBaselineMuon->Fill(6.);
  // iso cut
  if( !(bool)m_EDM->MuonIsolation(icontainer,mu_isowp_base) )
    return false;
  passBaselineMuon->Fill(7.);
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::XSUSYObjDefAlgPlusPlus::isBaselineElectron(int icontainer){
  // Total electrons
  passBaselineElectron->Fill(0.); 
  // electron OQ cut
  if( !m_EDM->getElectron(icontainer)->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON) ) 
    return false;
  passBaselineElectron->Fill(1.);
  // pT cut
  double ElectronPT = m_EDM->ElePt(icontainer);
  // NaN check.
  if(ElectronPT != ElectronPT){
    MSG_INFO("Electron with pT = NaN.  That's cool");
    return false;
  }
  if(ElectronPT < ele_ptmin_base*1000.)
    return false;
  passBaselineElectron->Fill(2.);
  // eta cut
  double ElectronClusterEta = m_EDM->getElectron(icontainer)->caloCluster()->etaBE(2);
  double ElectronTrackEta = m_EDM->getElectron(icontainer)->eta();
  if( fabs(ElectronClusterEta) > ele_eta_max || fabs(ElectronTrackEta) > 2.5 )
    return false;
  passBaselineElectron->Fill(3.);
  // ID Cut
  if( !m_EDM->passLikelihood(icontainer,ele_id_base) )
    return false;
  passBaselineElectron->Fill(4.);
  // d0 cut
  if( ele_d0_max < fabs(m_EDM->EleD0Significance(icontainer)) )
    return false;
  passBaselineElectron->Fill(5.);
  // z0 cut
  float z0sintheta = m_EDM->EleZ0(icontainer)*( sin(m_EDM->GetContainerEleTLV(icontainer).Theta()) );
  if ( ele_z0_max < fabs(z0sintheta) ) 
    return false; 
  passBaselineElectron->Fill(6.);
  // iso cut
  if( !(bool)m_EDM->EleIsolation(icontainer,ele_isowp_base) )
    return false;
  passBaselineElectron->Fill(7.);
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::XSUSYObjDefAlgPlusPlus::isZMuon(int icontainer){
  // Total muons
  // passZMuon->Fill(0.);  // already filled earlier before full OR
  // pT cut
  if(m_EDM->MuonPt(icontainer) < zmu_ptmin*1000.)
    return false;
  passZMuon->Fill(2.);
  // ID cut
  xAOD::Muon::Quality mu_qual;
  mu_qual = m_EDM->m_muonSelection->getQuality( *(m_EDM->getMuon(icontainer)) );
  if(mu_qual > zmu_id)
    return false;
  passZMuon->Fill(4.);
  // iso cut
  if( !(bool)m_EDM->MuonIsolation(icontainer,zmu_isowp) )
    return false;
  passZMuon->Fill(5.);
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::XSUSYObjDefAlgPlusPlus::isZElectron(int icontainer){
  // Total electrons
  // passZElectron->Fill(0.);  // already filled earlier before full OR
  // pT cut
  if(m_EDM->ElePt(icontainer) < zele_ptmin*1000.)
    return false;
  passZElectron->Fill(2.);
  // eta cut (crack region)
  double ElectronEta = m_EDM->getElectron(icontainer)->caloCluster()->etaBE(2);
  if( fabs(ElectronEta) >= 1.37 && fabs(ElectronEta) <= 1.52 ) // crack region is hard-coded
    return false;
  passZElectron->Fill(3.);
  // ID cut
  if( !m_EDM->passLikelihood(icontainer,zele_id) )
    return false;
  passZElectron->Fill(4.);
  // iso cut
  if( !(bool)m_EDM->EleIsolation(icontainer,zele_isowp) )
    return false;
  passZElectron->Fill(5.);
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::XSUSYObjDefAlgPlusPlus::isWMuon(int icontainer){
  // I assume this function is only called on things that pass isZMuon
  //   So redundant cuts are NOT checked!!

  // Total muons
  passWMuon->Fill(0.);
  // pT cut
  if(m_EDM->MuonPt(icontainer) < mu_ptmin*1000.)
    return false;
  passWMuon->Fill(1.);
  // ID cut
  xAOD::Muon::Quality mu_qual;
  mu_qual = m_EDM->m_muonSelection->getQuality( *(m_EDM->getMuon(icontainer)) );
  if(mu_qual > mu_id)
    return false;
  passWMuon->Fill(2.);
  // iso cut
  if( !(bool)m_EDM->MuonIsolation(icontainer,mu_isowp) )
    return false;
  passWMuon->Fill(3.);
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::XSUSYObjDefAlgPlusPlus::isWElectron(int icontainer){
  // I assume this function is only called on things that pass isZElectron
  //   So redundant cuts are NOT checked!!

  // Total electrons
  passWElectron->Fill(0.);
  // pT cut
  if(m_EDM->ElePt(icontainer) < ele_ptmin*1000.)
    return false;
  passWElectron->Fill(1.);
  // ID cut
  if( !m_EDM->passLikelihood(icontainer,ele_id) )
    return false;
  passWElectron->Fill(2.);
  // iso cut
  if( !(bool)m_EDM->EleIsolation(icontainer,ele_isowp) )
    return false;
  passWElectron->Fill(3.);
  return true;
}

bool PSL::XSUSYObjDefAlgPlusPlus::isBaselineJet(int icontainer, bool forOR /*true for 20GeV pT cut*/){
  // Total jets
  passBaselineJet->Fill(0.);
  // pT cut
  const xAOD::Jet *jet = m_EDM->getJet(icontainer);

  if(!forOR){ // our regular baseline pT cut
    if(jet->pt() < jet_ptmin*1000.) return false;
    passBaselineJet->Fill(1.);
  }
  else{
    if(jet->pt() < 20000.) return false;
  }
  // eta cut
  if(fabs(jet->eta()) > jet_eta_max) return false;
  if(!forOR)
    passBaselineJet->Fill(2.);

  // jvt cut
  bool passJvt = (jet->pt() > jet_central_jvfpt_max*1000. || 
                  fabs(jet->eta()) > jet_central_jvfeta_max || 
                  m_jetjvf_cut_and_sf->passesJvtCut(*jet));
  if (!passJvt) return false;

  if(!forOR)
    passBaselineJet->Fill(3.);
  return true;
}

bool PSL::XSUSYObjDefAlgPlusPlus::isBadJet(int icontainer){
  // returns true if the jet is bad!
  /*
  1. For all jets within |eta|<=2.4 and with (20 GeV < pT < 50 GeV)
  - If JVT > 0.64 && jetIsBad: reject the event

  2. For all jets within |eta|>2.4 and with (20 GeV < pT < 50 GeV)
  - If jetIsBad: reject the event
  
  3. For all jets with pT >= 50 GeV
  - If jetIsBad: reject the event
  */
  const xAOD::Jet *jet = m_EDM->getJet(icontainer);
  bool isBad = !m_jetCleaningTool->keep(*jet);

  bool passJvt = (jet->pt() > jet_central_jvfpt_max*1000. || 
                  fabs(jet->eta()) > jet_central_jvfeta_max || 
                  m_jetjvf_cut_and_sf->passesJvtCut(*jet));

  // These are hardcoded, instead of using the configurable cuts.
  // see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/HowToCleanJets2015#Event_Cleaning
  // Mimicking the logic on the twiki:
  if (20000. < jet->pt() && jet->pt() < jet_central_jvfpt_max*1000. && fabs(jet->eta()) < jet_central_jvfeta_max) {
    if (passJvt && isBad) return true;
  }
  else if (20000. < jet->pt() && jet->pt() < jet_central_jvfpt_max*1000. && fabs(jet->eta()) >= jet_central_jvfeta_max) {
    if (isBad) return true;
  }
  else if (jet->pt() > jet_central_jvfpt_max*1000.) {
    if (isBad) return true;
  }
  return false;
}

// Custom jet-lepton OR function (since we need jet pt > 20)
/* 
   Basically if jets overlap with leptons, we want to use 20GeV jets, but if leptons overlap with jets we want
   25GeV jets.  The OR tools don't let us do this, so I've coded the jet-lepton overlaps manually.

   There are 2 functions to do this (one for electrons and one for muons) to respect the order in which OR
   is supposed to be performed 

   The jet passed to these functions should already have been checked to see if it passes the required 
   decorators -- there is no check for this within the function itself!!
*/
bool PSL::XSUSYObjDefAlgPlusPlus::passJetElectronOR(int icontainer){
  // get the jet
  const xAOD::Jet *jet = m_EDM->getJet(icontainer);
  // loop over electrons
  for(unsigned int i=0;i<m_EDM->electrons->size();++i){
    const xAOD::Electron *ele = m_EDM->getElectron(i);
    if(!dec_baseline(*ele)) continue; // if it fails baseline game over
    float dR = jet->p4().DeltaR(ele->p4());
    if(dR < 0.2) return false;
  } 
  return true;
}
bool PSL::XSUSYObjDefAlgPlusPlus::passJetMuonOR(int icontainer){
  // get the jet
  const xAOD::Jet *jet = m_EDM->getJet(icontainer);
  // get the number of tracks in the jet
  const xAOD::Vertex *pv = m_EDM->GetPrimaryVertex();
  if(!pv){
    MSG_ERROR("-- passJetMuonOR -- Failed to get primary vertex !!");
    return true;
  }
  size_t vtxIdx = pv->index();
  int nTrk = jet->getAttribute< std::vector<int> >(xAOD::JetAttribute::NumTrkPt500)[vtxIdx];
  if(nTrk >= 3) return true; // only check jets with < 3 tracks
  // loop over muons
  for(unsigned int i=0;i<m_EDM->muons->size();i++){
    const xAOD::Muon *mu = m_EDM->getMuon(i);
    if(!dec_baseline(*mu)) continue; // if it fails baseline game over
    float dR = jet->p4().DeltaR(mu->p4());
    if(dR < 0.4) return false;
  }
  return true;
}

// Trigger matching functions.  I'll probably move these to xAODWrapper at some point...
bool PSL::XSUSYObjDefAlgPlusPlus::isTriggerMatchedElectron(const xAOD::Electron *ele){
  std::string elTriggers[3] = {"HLT_e24_lhmedium_L1EM20VH","HLT_e60_lhmedium","HLT_e120_lhloose"}; // hard coded for now
  if(!m_SUSYObjDef->isData()){
    elTriggers[0] = "HLT_e24_lhmedium_L1EM18VH";
  }
  bool matched = false;
  for(int i=0;i<3;i++){ // loop over triggers
    matched = m_EgammaMatchTool->matchHLT(ele,elTriggers[i]);
    //std::cout << elTriggers[i] << " " << matched << std::endl;
    if(matched) break;
  }
  return matched;
}

bool PSL::XSUSYObjDefAlgPlusPlus::isTriggerMatchedMuon(const xAOD::Muon *muon){
  std::string muTriggers[2] = {"HLT_mu20_iloose_L1MU15","HLT_mu50"}; // hard coded for now
  bool matched = false;
  for(int i=0;i<2;i++){ // loop over triggers
    matched = m_MuonMatchTool->match(muon,muTriggers[i].c_str());
    //std::cout << muTriggers[i] << " " << matched << std::endl;
    if(matched) break;
  }
  return matched;
}

// Do this ourselves instead of in SUSYTools.  Fill containers and apply calibrations
StatusCode PSL::XSUSYObjDefAlgPlusPlus::GetElectrons(xAOD::ElectronContainer*& ele, xAOD::ShallowAuxContainer*& eleAux, const std::string& elekey){
  // retrieve the container
  const xAOD::ElectronContainer* electrons(0);
  if( m_EDM->m_event->retrieve(electrons,elekey).isFailure() ){
    MSG_ERROR("Could not retrieve Electron container!! That's bad!!");
    return StatusCode::FAILURE;
  }

  // fill aux container
  std::pair<xAOD::ElectronContainer*, xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*electrons);
  ele = shallowcopy.first;
  eleAux = shallowcopy.second;
  bool setLinks = xAOD::setOriginalObjectLink(*electrons, *ele);
  if (!setLinks) {
    MSG_ERROR("Failed to set original object links on " << elekey);
    return StatusCode::FAILURE;
  }

  // do calibrations
  for(const auto& electron : *ele){
    MSG_VERBOSE("ELECTRON pre-calibration  pt=" << electron->pt() );
    if ( m_egammaCalibTool->applyCorrection(*electron) != CP::CorrectionCode::Ok)
      MSG_WARNING("GetElectrons: EgammaCalibTool applyCorrection failed ");
    if (m_isoCorrTool->applyCorrection(*electron)  != CP::CorrectionCode::Ok)
      MSG_WARNING("GetElectrons: IsolationCorrectionTool applyCorrection failed");
    MSG_VERBOSE("ELECTRON post-calibration pt=" << electron->pt() );
  }
  return StatusCode::SUCCESS;
}

StatusCode PSL::XSUSYObjDefAlgPlusPlus::GetMuons(xAOD::MuonContainer*& mu, xAOD::ShallowAuxContainer*& muAux, const std::string& mukey){
  // retrieve the container
  const xAOD::MuonContainer* muons(0);
  if( m_EDM->m_event->retrieve(muons,mukey).isFailure() ){
    MSG_ERROR("Could not retrieve Muon container!! That's bad!!");
    return StatusCode::FAILURE;
  }

  // fill aux container
  std::pair<xAOD::MuonContainer*, xAOD::ShallowAuxContainer*> shallowcopy = xAOD::shallowCopyContainer(*muons);
  mu = shallowcopy.first;
  muAux = shallowcopy.second;
  bool setLinks = xAOD::setOriginalObjectLink(*muons, *mu);
  if (!setLinks) {
    MSG_ERROR("Failed to set original object links on " << mukey);
    return StatusCode::FAILURE;
  }
  // do calibrations
  for(const auto& muon : *mu){
    MSG_VERBOSE("MUON pre-calibration  pt=" << muon->pt() );
    if (fabs(muon->eta()) < 2.7) {
      if(m_muonCalibrationAndSmearingTool->applyCorrection(*muon) == CP::CorrectionCode::OutOfValidityRange)
        MSG_WARNING("GetMuons: applyCorrection out of validity range");
    }
    MSG_VERBOSE("MUON post-calibration pt=" << muon->pt() );
  }
  return StatusCode::SUCCESS;
}

//#endif // BEFORE_SUSYTOOLS_000709
#endif // SKIP_XAOD_COMPILATION
