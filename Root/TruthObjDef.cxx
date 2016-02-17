#include <pennSoftLepton/TruthObjDef.h>
#ifndef SKIP_XAOD_COMPILATION

#include <EventLoop/Worker.h>

#include "xAODMissingET/MissingET.h"

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::TruthObjDef)
  
//-----------------------------------------------------------------------------
PSL::TruthObjDef::TruthObjDef() : AlgBase()
  ,ele_truth_ptmin         (7.    )
  ,ele_truth_etamax        (2.5   )
  ,mu_truth_ptmin          (6.    )
  ,mu_truth_etamax         (2.5   )
  ,jet_truth_ptmin         (20.   )
  ,jet_truth_etamax        (4.5   )
  ,m_acc_particleType      ("classifierParticleType")
  ,m_acc_particleOrigin    ("classifierParticleOrigin")
  ,m_acc_particleOutCome   ("classifierParticleOutCome")
  ,m_acc_partonID          ("PartonTruthLabelID")
  ,m_acc_motherID          ("motherID")
  ,m_acc_e_dressed         ("e_dressed")
  ,m_acc_pt_dressed        ("pt_dressed")
  ,m_acc_eta_dressed       ("eta_dressed")
  ,m_acc_phi_dressed       ("phi_dressed")
{
  MSG_DEBUG("Constructor");
}

//-----------------------------------------------------------------------------

bool PSL::TruthObjDef::init(void)
{

  testHistogram  = new TH1F ("testHistogram" , "Title;x-axis;y-axis", 50, 0., 50.);

  if (ele_truth_etamax    <= 0) ele_truth_etamax    = FLT_MAX;
  if (mu_truth_etamax     <= 0) mu_truth_etamax     = FLT_MAX;
  if (jet_truth_etamax    <= 0) jet_truth_etamax    = FLT_MAX;

  MSG_INFO("TruthObjDef initialized... " );
  PrintConfiguration();
  return true;
}

//-----------------------------------------------------------------------------
void PSL::TruthObjDef::loop(void){
  MSG_DEBUG("loop.");

  if (!m_EDM->m_event->retrieve(m_EDM->truthparticles,"TruthParticles").isSuccess()) {
    m_EDM->xaodWarnOnce("tp_cont_retrieve","Warning! TruthParticles not found!");
    return;
  }

  if (!m_EDM->m_event->retrieve(m_EDM->truthmet,"MET_Truth").isSuccess()) {
    MSG_INFO("Error! MET_Truth not found!");
    return;
  }

  if (!m_EDM->m_event->retrieve(m_EDM->truthelectrons,"TruthElectrons").isSuccess()) {
    MSG_INFO("Error! TruthElectrons not found!");
    return;
  }
  
  if (!m_EDM->m_event->retrieve(m_EDM->truthmuons,"TruthMuons").isSuccess()) {
    MSG_INFO("Error! TruthMuons not found!");
    return;
  }
  
  if (!m_EDM->m_event->retrieve(m_EDM->truthtaus,"TruthTaus").isSuccess()) {
    MSG_INFO("Error! TruthTaus not found!");
    return;
  }

  if (!m_EDM->m_event->retrieve(m_EDM->truthneutrinos,"TruthNeutrinos").isSuccess()) {
    MSG_INFO("Error! TruthNeutrinos not found!");
    return;
  }

  if (!m_EDM->m_event->retrieve(m_EDM->truthjets,"AntiKt4TruthJets").isSuccess()) {
    MSG_INFO("Error! AntiKt4TruthJets not found!");
    return;
  }

  if (!m_EDM->m_event->retrieve(m_EDM->truthphotons,"TruthPhotons").isSuccess()) {
    MSG_INFO("Error! TruthPhotons not found!");
    return;
  }

  m_evtdef.Reset();
  m_EDM->Reset();

  truthTauStatus = new std::vector<int>();
  truthTauBarcode = new std::vector<int>();
  truthTauOrigin = new std::vector<unsigned int>();
  truthTauType = new std::vector<unsigned int>();
  truthTauOutCome = new std::vector<unsigned int>();     
  truthTauMotherID = new std::vector<int>();

  truthLepStatus = new std::vector<int>();
  truthLepBarcode = new std::vector<int>();
  truthLepOrigin = new std::vector<unsigned int>(); 
  truthLepType = new std::vector<unsigned int>();
  truthLepOutCome = new std::vector<unsigned int>(); 
  truthLepMotherID = new std::vector<int>();

  truthJetPartonID = new std::vector<int>();

  truthNeutrinoPt = new std::vector<float>();
  truthNeutrinoEta = new std::vector<float>();
  truthNeutrinoPhi = new std::vector<float>();
  truthNeutrinoE = new std::vector<float>();

  truthPhoStatus = new std::vector<int>();
  truthPhoBarcode = new std::vector<int>();
  truthPhoOrigin = new std::vector<unsigned int>(); 
  truthPhoType = new std::vector<unsigned int>();
  truthPhoOutCome = new std::vector<unsigned int>(); 
  truthPhoMotherID = new std::vector<int>();

  xAOD::MissingETContainer::const_iterator metterm = m_EDM->truthmet->find("NonInt");
  if (metterm==m_EDM->truthmet->end()) { MSG_INFO("Error! NonInt missing!"); exit(1); }
  m_evtdef.met_tv2 = TVector2((*metterm)->mpx(),(*metterm)->mpy());
  m_evtdef.met_sumet = (*metterm)->sumet();

  //TLorentzVector truthMET_tlv; truthMET_tlv.SetPxPyPzE(m_evtdef.met_tv2.X(),m_evtdef.met_tv2.Y(),0,m_evtdef.met_tv2.Mod());

  for(unsigned int i=0;i<m_EDM->truthelectrons->size();++i){
    m_evtdef.n_baseline_ele++;
    const xAOD::TruthParticle* tele = m_EDM->getTruthElectron(i);
    TLorentzVector dressed_ele;
    dressed_ele.SetPtEtaPhiE(m_acc_pt_dressed(*tele),m_acc_eta_dressed(*tele),m_acc_phi_dressed(*tele),m_acc_e_dressed(*tele));
    m_evtdef.AddSignalLepton(Particle(dressed_ele,ObjType::Electron,i,tele->charge()));

  }

  for(unsigned int i=0;i<m_EDM->truthmuons->size();++i){
    m_evtdef.n_baseline_mu++;
    const xAOD::TruthParticle* tmuo = m_EDM->getTruthMuon(i);
    TLorentzVector dressed_muo;
    dressed_muo.SetPtEtaPhiE(m_acc_pt_dressed(*tmuo),m_acc_eta_dressed(*tmuo),m_acc_phi_dressed(*tmuo),m_acc_e_dressed(*tmuo));
    m_evtdef.AddSignalLepton(Particle(dressed_muo,ObjType::Muon,i,tmuo->charge()));

  }

  for(unsigned int i=0;i<m_EDM->truthtaus->size();++i){
    m_evtdef.n_baseline_tau++;
    const xAOD::TruthParticle* ttau = m_EDM->getTruthTau(i);
    m_evtdef.AddSignalTau(Particle(ttau->p4(),ObjType::Tau,i,ttau->charge()));
  }

  for(unsigned int i=0;i<m_EDM->truthjets->size();++i){
    m_evtdef.n_baseline_jet++;
    const xAOD::Jet* tjet = m_EDM->getTruthJet(i);
    m_evtdef.AddSignalJet(Particle(tjet->p4(),ObjType::Jet,i,0));
  }

  m_evtdef.BuildEvent();

  //
  // To keep the indices of local branches in sync with those in the EventDefinition, you must
  // push_back to the local branches _after_ you call BuildEvent. BuildEvent orders the leptons/jets/
  // taus by descending pt!

  for(unsigned int i=0;i<m_evtdef.leps.size();++i){
    PSL::Particle lepton = m_evtdef.leps[i];
    
    int pstatus;
    int pbarcode;
    unsigned int porigin;
    unsigned int ptype;
    unsigned int poutcome;
    int pmother;

    if (lepton.obj_type == ObjType::Electron) {
      pstatus  = m_EDM->getTruthElectron(lepton.i_cont)->status();
      pbarcode = m_EDM->getTruthElectron(lepton.i_cont)->barcode();
      porigin  = ParticleOrigin(*m_EDM->getTruthElectron(lepton.i_cont));
      ptype    = ParticleType(*m_EDM->getTruthElectron(lepton.i_cont));
      poutcome = ParticleOutCome(*m_EDM->getTruthElectron(lepton.i_cont));
      pmother  = ParticleMother(*m_EDM->getTruthElectron(lepton.i_cont));
    }
    else {
      pstatus  = m_EDM->getTruthMuon(lepton.i_cont)->status();
      pbarcode = m_EDM->getTruthMuon(lepton.i_cont)->barcode();
      porigin  = ParticleOrigin(*m_EDM->getTruthMuon(lepton.i_cont));
      ptype    = ParticleType(*m_EDM->getTruthMuon(lepton.i_cont));
      poutcome = ParticleOutCome(*m_EDM->getTruthMuon(lepton.i_cont));
      pmother  = ParticleMother(*m_EDM->getTruthMuon(lepton.i_cont));
    }

    truthLepStatus->push_back(pstatus);
    truthLepBarcode->push_back(pbarcode);
    truthLepOrigin->push_back(porigin);
    truthLepType->push_back(ptype);
    truthLepOutCome->push_back(poutcome);
    truthLepMotherID->push_back(pmother);
  }

  for(unsigned int i=0;i<m_evtdef.NSignalTaus();++i){
    const xAOD::TruthParticle* ttau = m_EDM->getTruthTau(m_evtdef.taus[i].i_cont);
    truthTauStatus->push_back(ttau->status());
    truthTauBarcode->push_back(ttau->barcode());
    truthTauOrigin->push_back(ParticleOrigin(*ttau));
    truthTauType->push_back(ParticleType(*ttau));
    truthTauOutCome->push_back(ParticleOutCome(*ttau));
    truthTauMotherID->push_back(ParticleMother(*ttau));
  }

  for(unsigned int i=0;i<m_evtdef.jets.size();++i){
    const xAOD::Jet* tjet = m_EDM->getTruthJet(m_evtdef.jets[i].i_cont);
    truthJetPartonID->push_back(PartonID(*tjet));
  }

  for(unsigned int i=0;i<m_EDM->truthneutrinos->size();++i){
    const xAOD::TruthParticle* tnu = m_EDM->getTruthNeutrino(i);
    truthNeutrinoPt->push_back(tnu->p4().Pt());
    truthNeutrinoE->push_back(tnu->p4().E());
    truthNeutrinoEta->push_back(tnu->p4().Eta());
    truthNeutrinoPhi->push_back(tnu->p4().Phi());
  }

  for(unsigned int i=0;i<m_EDM->truthphotons->size();++i){
    const xAOD::TruthParticle* tpho = m_EDM->getTruthPhoton(i);
    truthPhoStatus->push_back(tpho->status());
    truthPhoBarcode->push_back(tpho->barcode());
    truthPhoOrigin->push_back(ParticleOrigin(*tpho));
    truthPhoType->push_back(ParticleType(*tpho));
    truthPhoOutCome->push_back(ParticleOutCome(*tpho));
    truthPhoMotherID->push_back(ParticleMother(*tpho));
  }
  
  m_EDM->SetEventDefinitionPointer(&m_evtdef);

  MSG_VERBOSE("loop end");
}

//-----------------------------------------------------------------------------
void PSL::TruthObjDef::finish(void){

  wk()->addOutput(testHistogram);

  MSG_DEBUG("finish.");
}

void PSL::TruthObjDef::SetOutgoingNtupleBranches(TTree* tree){
  
  tree->Branch("truthLepStatus",   "vector<int>",&truthLepStatus);
  tree->Branch("truthLepBarcode",  "vector<int>",&truthLepBarcode); 
  tree->Branch("truthLepOrigin",   "vector<unsigned int>",&truthLepOrigin);  
  tree->Branch("truthLepType",     "vector<unsigned int>",&truthLepType);
  tree->Branch("truthLepOutCome",  "vector<unsigned int>",&truthLepOutCome);
  tree->Branch("truthLepMotherID", "vector<int>",&truthLepMotherID);

  tree->Branch("truthJetPartonID", "vector<int>",&truthJetPartonID);

  tree->Branch("truthNeutrinoPt",  "vector<float>",&truthNeutrinoPt);
  tree->Branch("truthNeutrinoE",   "vector<float>",&truthNeutrinoE);
  tree->Branch("truthNeutrinoEta", "vector<float>",&truthNeutrinoEta);
  tree->Branch("truthNeutrinoPhi", "vector<float>",&truthNeutrinoPhi);

  tree->Branch("truthPhoStatus",   "vector<int>",&truthPhoStatus);
  tree->Branch("truthPhoBarcode",  "vector<int>",&truthPhoBarcode); 
  tree->Branch("truthPhoOrigin",   "vector<unsigned int>",&truthPhoOrigin);  
  tree->Branch("truthPhoType",     "vector<unsigned int>",&truthPhoType);
  tree->Branch("truthPhoOutCome",  "vector<unsigned int>",&truthPhoOutCome);
  tree->Branch("truthPhoMotherID", "vector<int>",&truthPhoMotherID);

  return;
}

//-----------------------------------------------------------------------------
void PSL::TruthObjDef::PrintConfiguration(void){
  MSG_INFO("-------------------------------TruthObjDef Configuration:-------------------------------");
  MSG_INFO("ele_truth_ptmin        : " << ele_truth_ptmin        );
  MSG_INFO("ele_truth_etamax       : " << ele_truth_etamax       );
  MSG_INFO("mu_truth_ptmin         : " << mu_truth_ptmin         );
  MSG_INFO("mu_truth_etamax        : " << mu_truth_etamax        );
  MSG_INFO("jet_truth_ptmin        : " << jet_truth_ptmin        );
  MSG_INFO("jet_truth_etamax       : " << jet_truth_etamax       );
  MSG_INFO("---------------------------------------------------------------------------------------");
}

#endif // SKIP_XAOD_COMPILATION
