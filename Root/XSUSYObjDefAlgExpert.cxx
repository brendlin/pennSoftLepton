#include <pennSoftLepton/XSUSYObjDefAlgExpert.h>
#ifndef SKIP_XAOD_COMPILATION
#ifdef BEFORE_SUSYTOOLS_000709

#include <EventLoop/Worker.h>

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::XSUSYObjDefAlgExpert)
  
//-----------------------------------------------------------------------------
PSL::XSUSYObjDefAlgExpert::XSUSYObjDefAlgExpert() : XSUSYObjDefAlg()
  ,do_harmonization       (false)
  ,do_or_after_baselinesel(false)
  ,do_susytools_signallep (false)
  ,do_bjet_overlap        (false)
  ,m_leptonMultiplicityOR (0)
  ,m_jetMultiplicityOR    (0)
  ,m_leptonMultiplicityOR_count (0)
  ,m_jetMultiplicityOR_count    (0)
  //,do_custom_overlapremove(false)
{
  MSG_DEBUG("Constructor");
}

//-----------------------------------------------------------------------------
bool PSL::XSUSYObjDefAlgExpert::init(void)
{
  XSUSYObjDefAlg::init();

  m_leptonMultiplicityOR = new TH2F("lepMultiplicity","lepMultiplicity",10,0,10,7,0,7);
  m_leptonMultiplicityOR->GetYaxis()->SetBinLabel(1,"baseline");
  m_leptonMultiplicityOR->GetYaxis()->SetBinLabel(2,"e-jet OR");
  m_leptonMultiplicityOR->GetYaxis()->SetBinLabel(3,"mu-jet OR");
  m_leptonMultiplicityOR->GetYaxis()->SetBinLabel(4,"e-mu OR");
  m_leptonMultiplicityOR->GetYaxis()->SetBinLabel(5,"e-e OR");
  m_leptonMultiplicityOR->GetYaxis()->SetBinLabel(6,"signal mu");
  m_leptonMultiplicityOR->GetYaxis()->SetBinLabel(7,"signal e");
  setHistogramOutput(m_leptonMultiplicityOR);
  
  m_jetMultiplicityOR = new TH2F("jetMultiplicityOR","jetMultiplicityOR",10,0,10,4,0,4);
  m_jetMultiplicityOR->GetYaxis()->SetBinLabel(1,"baseline");
  m_jetMultiplicityOR->GetYaxis()->SetBinLabel(2,"jet-e");
  m_jetMultiplicityOR->GetYaxis()->SetBinLabel(3,"jet-mu");
  m_jetMultiplicityOR->GetYaxis()->SetBinLabel(4,"signal");
  setHistogramOutput(m_jetMultiplicityOR);

  m_leptonMultiplicityOR_count = new TH2F("lepMultiplicityOR_ct","lepMultiplicity_ct",10,0,10,7,0,7);
  m_leptonMultiplicityOR_count->GetYaxis()->SetBinLabel(1,"baseline");
  m_leptonMultiplicityOR_count->GetYaxis()->SetBinLabel(2,"e-jet OR");
  m_leptonMultiplicityOR_count->GetYaxis()->SetBinLabel(3,"mu-jet OR");
  m_leptonMultiplicityOR_count->GetYaxis()->SetBinLabel(4,"e-mu OR");
  m_leptonMultiplicityOR_count->GetYaxis()->SetBinLabel(5,"e-e OR");
  m_leptonMultiplicityOR_count->GetYaxis()->SetBinLabel(6,"signal mu");
  m_leptonMultiplicityOR_count->GetYaxis()->SetBinLabel(7,"signal e");
  setHistogramOutput(m_leptonMultiplicityOR_count);
  
  m_jetMultiplicityOR_count = new TH2F("jetMultiplicityOR_ct","jetMultiplicityOR_ct",10,0,10,4,0,4);
  m_jetMultiplicityOR_count->GetYaxis()->SetBinLabel(1,"baseline");
  m_jetMultiplicityOR_count->GetYaxis()->SetBinLabel(2,"jet-e");
  m_jetMultiplicityOR_count->GetYaxis()->SetBinLabel(3,"jet-mu");
  m_jetMultiplicityOR_count->GetYaxis()->SetBinLabel(4,"signal");
  setHistogramOutput(m_jetMultiplicityOR_count);

  return true;
}

//-----------------------------------------------------------------------------
void PSL::XSUSYObjDefAlgExpert::loop(void){
  MSG_DEBUG("loop.");
  XSUSYObjDefAlg::loop();
  m_jetMultiplicityOR->Fill(3,m_evtdef.jets.size()); // signal
  m_leptonMultiplicityOR->Fill(m_evtdef.NSignalLeptons(),5); // signal e/mu
}

//-----------------------------------------------------------------------------
void PSL::XSUSYObjDefAlgExpert::ApplyLeptonVeto(void){
  MSG_DEBUG("ApplyLeptonVeto begin");
  for(unsigned int i=0;i<m_EDM->electrons->size();++i){
    const xAOD::Electron* ele = m_EDM->getElectron(i);
    // For the veto, consider only baseline leptons (may change)
    if (!dec_baseline(*ele)) continue;
    if (dec_signal(*ele)) continue;
    // if it overlaps with a b-jet, then do not consider it as a veto lepton
    bool overlaps_with_bjet = false;
    TLorentzVector lep4vec = ele->p4();
    for(unsigned int j=0;j<m_EDM->jets->size();++j){
      const xAOD::Jet* jet = m_EDM->getJet(j);
      if (!dec_bjet(*jet)) continue;
      // jets before overlap removal? After?
      TLorentzVector jet4vec = jet->p4();
      if (lep4vec.DeltaR(jet4vec)<0.2) {
        overlaps_with_bjet = true;
        break;
      }
    }
    if (overlaps_with_bjet) continue;
    m_evtdef.AddVetoLepton(Particle(ele->p4(),ObjType::Electron,i,ele->charge()));
  }
  for(unsigned int i=0;i<m_EDM->muons->size();++i){
    const xAOD::Muon* muon = m_EDM->getMuon(i);
    // For the veto, consider only baseline leptons (may change)
    if (!dec_baseline(*muon)) continue;
    if (dec_signal(*muon)) continue;
    // if it overlaps with a b-jet, then do not consider it as a veto lepton
    bool overlaps_with_bjet = false;
    TLorentzVector lep4vec = muon->p4();
    for(unsigned int j=0;j<m_EDM->jets->size();++j){
      const xAOD::Jet* jet = m_EDM->getJet(j);
      if (!dec_bjet(*jet)) continue;
      // jets before overlap removal? After?
      TLorentzVector jet4vec = jet->p4();
      if (lep4vec.DeltaR(jet4vec)<0.2) {
        overlaps_with_bjet = true;
        break;
      }
    }
    if (overlaps_with_bjet) continue;
    m_evtdef.AddVetoLepton(Particle(muon->p4(),ObjType::Muon,i,muon->charge()));
  }
  MSG_DEBUG("ApplyLeptonVeto end");
}

//-----------------------------------------------------------------------------
void PSL::XSUSYObjDefAlgExpert::PrintConfiguration(void){
  XSUSYObjDefAlg::PrintConfiguration();
  MSG_INFO("do_harmonization       : " << (do_harmonization ? "yes" : "no"));
  MSG_INFO("do_or_after_baselinesel: " << (do_or_after_baselinesel ? "yes" : "no"));
  MSG_INFO("do_susytools_signallep : " << (do_susytools_signallep ? "yes" : "no"));
  MSG_INFO("do_bjet_overlap        : " << (do_bjet_overlap ? "yes" : "no"));
  MSG_INFO("---------------------------------------------------------------------------------------");
}

//-----------------------------------------------------------------------------
void PSL::XSUSYObjDefAlgExpert::LocalOverlapRemoval(const xAOD::ElectronContainer *electrons, const xAOD::MuonContainer *muons, const xAOD::JetContainer *jets, const bool doHarmonization, double dRejet, double dRjetmu, double dRjete, double dRemu, double dRee){
  int njets = 0;
  int nleps = 0;

  xAOD::JetContainer::const_iterator jet_itr = jets->begin();
  xAOD::JetContainer::const_iterator jet_end = jets->end();
  for( ; jet_itr != jet_end; ++jet_itr ) {
    bool jet_sel = dec_baseline(**jet_itr);
    if (dec_baseline(**jet_itr)) njets += 1;
    if(jet_sel)
      dec_passOR( **jet_itr ) = 1;
    else
      dec_passOR( **jet_itr ) = 0;
  }
  xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
  xAOD::MuonContainer::const_iterator mu_end = muons->end();
  for( ; mu_itr != mu_end; ++mu_itr ) {
    bool mu_sel;
    if(doHarmonization) mu_sel = dec_signal(**mu_itr);
    else mu_sel = dec_baseline(**mu_itr);
    if (dec_baseline(**mu_itr)) nleps += 1;
    if(mu_sel)
      dec_passOR( **mu_itr ) = 1;
    else
      dec_passOR( **mu_itr ) = 0;
  }

  m_jetMultiplicityOR->Fill(njets,0);

  // remove jets overlapping with (baseline/signal) electrons
  xAOD::ElectronContainer::const_iterator el_itr = electrons->begin();
  xAOD::ElectronContainer::const_iterator el_end = electrons->end();
  for( ; el_itr != el_end; ++el_itr ) {
    bool el_sel;
    if(doHarmonization) el_sel = dec_signal(**el_itr);
    else el_sel = dec_baseline(**el_itr);
    if (dec_baseline(**el_itr)) nleps += 1;
    if( !el_sel ){
      dec_passOR( **el_itr ) = 0;
      continue;
    }else{
      dec_passOR( **el_itr ) = 1;
    }
    
    xAOD::JetContainer::const_iterator jet_itr = jets->begin();
    xAOD::JetContainer::const_iterator jet_end = jets->end();
    
    for( ; jet_itr != jet_end; ++jet_itr ) {
      if( !dec_passOR(**jet_itr) ) continue;
      //
      // this is the line that is added
      //
      if(do_bjet_overlap && dec_bjet(**jet_itr)) continue;
      
      TLorentzVector el4vec = (*el_itr)->p4();
      TLorentzVector jet4vec = (*jet_itr)->p4();

      if (el4vec.DeltaR(jet4vec)<dRejet) {
        MSG_DEBUG( " Rejecting jet at (eta,phi)=(" << (*jet_itr)->eta() <<"," << (*jet_itr)->phi() <<") "
                      << " due to electron at (eta,phi)=(" << (*el_itr)->eta() <<"," << (*el_itr)->phi() <<")" );
        dec_passOR( **jet_itr ) = 0;
        njets -= 1;
      }
    }
  } // END loop over electrons

  m_leptonMultiplicityOR->Fill(nleps,0); // baseline leptons
  m_jetMultiplicityOR->Fill(njets,1); // jet-e OR
  
    // Remove electrons and muons overlapping with jets
  el_itr = electrons->begin();
  el_end = electrons->end();
  for( ; el_itr != el_end; ++el_itr ) {
    
    if( !dec_passOR(**el_itr) ) continue;
    
    xAOD::JetContainer::const_iterator jet_itr = jets->begin();
    xAOD::JetContainer::const_iterator jet_end = jets->end();
    
    for( ; jet_itr != jet_end; ++jet_itr ) {
      
      if ( !dec_passOR( **jet_itr ) ) continue;
      
      TLorentzVector el4vec = (*el_itr)->p4();
      TLorentzVector jet4vec = (*jet_itr)->p4();
      
      if (el4vec.DeltaR(jet4vec)<dRjete) {
        MSG_DEBUG( " Rejecting electron at (eta,phi)=(" << (*el_itr)->eta() <<"," << (*el_itr)->phi() <<") "
                      << " due to jet at (eta,phi)=(" << (*jet_itr)->eta() <<"," << (*jet_itr)->phi() <<")" );
        dec_passOR( **el_itr ) = 0;
        nleps -= 1;
      }
    }
  }
  
  m_leptonMultiplicityOR->Fill(nleps,1); // e-jet OR
  
  mu_itr = muons->begin();
  mu_end = muons->end();
  
  for( ; mu_itr != mu_end; ++mu_itr ) {
    
    if( !dec_passOR(**mu_itr) ) continue;
    
    xAOD::JetContainer::const_iterator jet_itr = jets->begin();
    xAOD::JetContainer::const_iterator jet_end = jets->end();
    
    for( ; jet_itr != jet_end; ++jet_itr ) {
      
      if ( !dec_passOR( **jet_itr ) ) continue;
      
      TLorentzVector mu4vec = (*mu_itr)->p4();
      TLorentzVector jet4vec = (*jet_itr)->p4();
      
      std::vector<int> nTrkVec;
      (*jet_itr)->getAttribute(xAOD::JetAttribute::NumTrkPt500, nTrkVec);
      int jet_nTrk = nTrkVec[0];
      
      if (mu4vec.DeltaR(jet4vec)<dRjetmu) {
        if(doHarmonization && jet_nTrk<3){ // was doHarmonization
          MSG_DEBUG( " Rejecting jet at (pT,eta,phi)=(" << (*jet_itr)->pt() <<","<< (*jet_itr)->eta() <<"," << (*jet_itr)->phi() <<") with only nTrk=" << jet_nTrk
                        << " due to muon at (pT,eta,phi)=(" << (*mu_itr)->pt() <<","<< (*mu_itr)->eta() <<"," << (*mu_itr)->phi() <<")" );
          dec_passOR( **jet_itr ) = 0;
          njets -= 1;
        }else{
          MSG_DEBUG( " Rejecting muon at (eta,phi)=(" << (*mu_itr)->eta() <<"," << (*mu_itr)->phi() <<") "
                        << " due to jet at (eta,phi)=(" << (*jet_itr)->eta() <<"," << (*jet_itr)->phi() <<")" );
          dec_passOR( **mu_itr ) = 0;
          nleps -= 1;
        }
      }
    }
  }
  
  m_leptonMultiplicityOR->Fill(nleps,2); // mu-jet OR
  m_jetMultiplicityOR->Fill(njets,2); // jet-mu OR
  
  // Remove electrons and muons overlapping with each other
  el_itr = electrons->begin();
  el_end = electrons->end();
  for( ; el_itr != el_end; ++el_itr ) {
    
    if( !dec_passOR(**el_itr) ) continue;
    
    mu_itr = muons->begin();
    mu_end = muons->end();
    
    for( ; mu_itr != mu_end; ++mu_itr ) {
      
      if ( !dec_passOR( **mu_itr ) ) continue;
      
      TLorentzVector el4vec = (*el_itr)->p4();
      TLorentzVector mu4vec = (*mu_itr)->p4();
      
      if (el4vec.DeltaR(mu4vec)<dRemu) {
        MSG_DEBUG( " Rejecting both electron at (eta,phi)=(" << (*el_itr)->eta() <<"," << (*el_itr)->phi() <<") "
                      << " and muon at (eta,phi)=(" << (*mu_itr)->eta() <<"," << (*mu_itr)->phi() <<")" );
        dec_passOR( **el_itr ) = 0;
        dec_passOR( **mu_itr ) = 0;
        nleps -= 2;
      }
    }
  }
  m_leptonMultiplicityOR->Fill(nleps,3); // mu-e OR
  
  // Remove electrons overlapping with each other
  el_itr = electrons->begin();
  el_end = electrons->end();
  for( ; el_itr != el_end; ++el_itr ) {
    
    if( !dec_passOR(**el_itr) ) continue;
    
    xAOD::ElectronContainer::const_iterator el2_itr = electrons->begin();
    xAOD::ElectronContainer::const_iterator el2_end = electrons->end();
    
    for( ; el2_itr != el2_end; ++el2_itr ) {
      
      if(el_itr == el2_itr) continue;
      if ( !dec_passOR( **el_itr ) ) continue;
      
      TLorentzVector el4vec = (*el_itr)->p4();
      TLorentzVector el24vec = (*el2_itr)->p4();
      
      if (el4vec.DeltaR(el24vec)<dRee) {
        if((*el_itr)->pt() < (*el2_itr)->pt()){
          MSG_DEBUG( " Rejecting electron at (eta,phi)=(" << (*el_itr)->eta() <<"," << (*el_itr)->phi() <<") "
                        << " and muon at (eta,phi)=(" << (*el2_itr)->eta() <<"," << (*el2_itr)->phi() <<")" );
          if (dec_passOR(**el_itr)) nleps -= 1;
          dec_passOR( **el_itr ) = 0;
          nleps -= 1;
        }else{
          MSG_DEBUG( " Rejecting electron at (eta,phi)=(" << (*el2_itr)->eta() <<"," << (*el2_itr)->phi() <<") "
                        << " and muon at (eta,phi)=(" << (*el_itr)->eta() <<"," << (*el_itr)->phi() <<")" );
          if (dec_passOR(**el2_itr)) nleps -= 1;
          dec_passOR( **el2_itr ) = 0;
        }
      }
    }
  }
  
  m_leptonMultiplicityOR->Fill(nleps,4); // e-e OR
  
  // Count number of objects after overlap removal
  int Nel=0;
  el_itr = electrons->begin();
  el_end = electrons->end();
  for( ; el_itr != el_end; ++el_itr ) {
    if(dec_passOR( **el_itr )) Nel++;
  }
  
  int Nmu=0;
  mu_itr = muons->begin();
  mu_end = muons->end();
  for( ; mu_itr != mu_end; ++mu_itr ) {
    if(dec_passOR( **mu_itr )) Nmu++;
  }
  
  int Njet=0;
  jet_itr = jets->begin();
  jet_end = jets->end();
  for( ; jet_itr != jet_end; ++jet_itr ) {
    if(dec_passOR( **jet_itr )) Njet++;
  }
  
  MSG_DEBUG( " After overlap removal: Nel=" << Nel <<", Nmu="<< Nmu <<", Njet=" << Njet );
  return;
}

void PSL::XSUSYObjDefAlgExpert::CountOR(void){
  for (int i=0;i<m_leptonMultiplicityOR->GetNbinsX();++i){
    for (int j=0;j<m_leptonMultiplicityOR->GetNbinsY();++j){
      if (m_leptonMultiplicityOR->GetBinContent(i+1,j+1)) m_leptonMultiplicityOR_count->Fill(i,j);
    }
  }
  for (int i=0;i<m_jetMultiplicityOR->GetNbinsX();++i){
    for (int j=0;j<m_jetMultiplicityOR->GetNbinsY();++j){
      if (m_jetMultiplicityOR->GetBinContent(i+1,j+1)) m_jetMultiplicityOR_count->Fill(i,j);
    }
  }
}

#endif // BEFORE_SUSYTOOLS_000709
#endif // SKIP_XAOD_COMPILATION
