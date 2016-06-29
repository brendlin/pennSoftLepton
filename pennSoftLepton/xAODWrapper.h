#include "pennSoftLepton/switch_d3pd_xaod.h"
#ifndef SKIP_XAOD_COMPILATION

#ifndef pennSoftLepton_xAODWrapper_H
#define pennSoftLepton_xAODWrapper_H

#include <sstream>

#include <EventLoop/Algorithm.h>
#include "pennSoftLepton/Messaging.h"
#include "pennSoftLepton/Variables.h"
#include "pennSoftLepton/Sample.h"
#include "pennSoftLepton/EventDefinition.h"

namespace xAOD{
  class TEvent;
}

namespace Trig{
  class TrigDecisionTool;
}

namespace TrigConf{
  class xAODConfigTool;
}

#include "xAODEventInfo/EventInfoContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"
#include "MuonSelectorTools/MuonSelectionTool.h"
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"
#include "xAODBase/IParticleContainer.h"
#include "EventPrimitives/EventPrimitivesHelpers.h" // track error accessor
#include "xAODPrimitives/IsolationType.h"
#include "xAODPrimitives/tools/getIsolationAccessor.h"
#include "PileupReweighting/PileupReweightingTool.h"
// triggers
#include "xAODTrigEgamma/TrigElectronContainer.h"
// GRL
#include "GoodRunsLists/GoodRunsListSelectionTool.h"
#ifdef BEFORE_SUSYTOOLS_000611
#include "ElectronIsolationSelection/IsolationSelectionTool.h"
#else
#include "IsolationSelection/IsolationSelectionTool.h"
#endif // BEFORE_SUSYTOOLS_000611
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "TruthUtils/PIDHelpers.h"
#ifdef ISREL20
#include "xAODTruth/xAODTruthHelpers.h"
#endif // ISREL20

namespace PSL
{
  class EventDefinition;

  class xAODWrapper : public EL::Algorithm
  {
    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
  public:
    std::map<unsigned int,unsigned int> m_run_sample_map;
    std::map<unsigned int,float> m_run_xsec_map;
    std::map<unsigned int,float> m_run_sumw_map;
    std::vector<std::string> m_xaod_warnings;
    void xaodWarnOnce(std::string wname,std::string message);
    bool m_allowUnlabeledSamples;

    void set_allowUnlabeledSamples(bool b){m_allowUnlabeledSamples = b;};
    void set_luminosity_pb(double d){m_lumi_pb = d;};
    void set_pu_rescaling(double d){m_pu_rescaling = d;};
    void set_grlfile(std::string s){m_grlfile = s;};
    //void set_prwConfFiles(std::string s){m_prwConfFiles = s;};
    void set_prwLcalcFiles(std::string s){m_prwLcalcFiles = s;};

    void set_RemoveZgammaFromZjet(bool b){m_RemoveZgammaFromZjet = b;}
    void set_CalculateZZAntiid(bool b){m_CalculateZZAntiid = b;}
    void set_doRecoAntiID(bool b){m_doRecoAntiID = b;}
    void set_doXCheckAntiID(bool b){m_doXCheckAntiID = b;};
    Particle GetZZAntiIDParticleFromReco(void);
    double CalculateZZAntiIDScaleFactor(Particle fourthLepton);
    int GetNLepsfromZ(Particle truthLepton);
    Particle GetMatchingRecoLepton(Particle truthLepton);
    Particle GetZZAntiIDParticle(void);
    void set_EgammaToys(bool b){m_EgammaToys = b;}
    bool IsEgammaToys(void){return m_EgammaToys;}
    
    void set_trigger_bitmask(std::string bits);
    void set_el_defaultcaloiso(std::string i){
      (void)i;
      MSG_INFO("set_el_defaultcaloiso is Deprecated! Exit.");
      exit(1);
    };
    void set_el_defaulttrkiso (std::string i){
      (void)i;
      MSG_INFO("set_el_defaulttrkiso is Deprecated! Exit.");
      exit(1);
    };
    void set_mu_defaultcaloiso(std::string i){
      (void)i;
      MSG_INFO("set_mu_defaultcaloiso is Deprecated! Exit.");
      exit(1);
    };
    void set_mu_defaulttrkiso (std::string i){
      (void)i;
      MSG_INFO("set_mu_defaulttrkiso is Deprecated! Exit.");
      exit(1);
    };
    
    // Configurable items
    float m_lumi_pb; /* configurable now */
    std::string m_grlfile; /* configurable */
    //int m_prwDefaultChannel;
    //std::string m_prwConfFiles;/* configurable */
    std::string m_prwLcalcFiles;/* configurable */
    float m_muUncert;
    float m_pu_rescaling;/* configurable */
    AnaIso::AnaIso m_el_defaultcaloiso; /* configurable */
    AnaIso::AnaIso m_el_defaulttrkiso; /* configurable */
    AnaIso::AnaIso m_mu_defaultcaloiso; /* configurable */
    AnaIso::AnaIso m_mu_defaulttrkiso; /* configurable */
    bool m_RemoveZgammaFromZjet; /* configurable */
    bool m_CalculateZZAntiid; /* configurable */
    bool m_doRecoAntiID; /* configurable */
    bool m_doXCheckAntiID; /*configurable */
    bool m_EgammaToys; /* configurable */
    std::vector<PSL::Trigger2015> GetTriggerBitmask(){return m_trigger_bitmask;}
    
    // variables that don't get filled at submission time should be
    // protected from being send from the submission node to the worker
    // node (done by the //!)
    
  private:
    std::vector<PSL::Trigger2015> m_trigger_bitmask; /* configurable */
    
  public:
    std::map<PSL::Variable,double> m_EventVariables; //!
    std::map<PSL::LepVariable,std::map<int,double> > m_LepVariables; //!
    Sample m_CurrentSample; //!
    void ResetSample(void){
      m_CurrentSample = kunlabeled;
      if(!eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) m_CurrentSample = kdata;
      else if (!m_run_sample_map.count(mc_channel_number)) m_CurrentSample = kunlabeled;
      else m_CurrentSample = static_cast<PSL::Sample>(m_run_sample_map[mc_channel_number]);
      return;
    };
    bool isZgammaEvent(void);
    bool m_event_killed; //!
    bool EventKilled(void){return m_event_killed;};
    int n_processed; //!
    std::map<unsigned int,TH1F*> m_sample_stats; //!

    float m_LumiBlockMu; //!

    int mc_channel_number; //!
    float m_weight; //! This is the overall weight!
    float m_weight_pu; //! This is the overall weight with PU
    float m_xsec; //!
    float m_kfac; //!
    float m_feff; //!
    float m_sumw; //!
    float m_purw; //!
    double m_sumw_thisjob; //!
    
    
    // 
    
    // this is a standard constructor
    xAODWrapper ();
    std::string CompiledEDMType(void){return "xAOD";};

    bool IsMC(){return m_CurrentSample != kdata;};
    bool IsData(){return m_CurrentSample == kdata;};
    
    //
    // These are variables that are provided that can be set by other algorithms
    // 
    EventDefinition* m_evtdef; //!
    
    std::vector<PSL::Particle> truth_jets; //!
    void SetEventDefinitionPointer(EventDefinition* evt){
      m_EventVariables.clear(); // want to refresh all of these quantities // yes!
      m_LepVariables.clear();
      m_evtdef = evt;
    };
    void CopyEventDefinitionFromEDM(EventDefinition& evtdef){
      if (!m_evtdef) {
        MSG_INFO("Error! Requested pointer is missing! Exiting.");
        exit(1);
      }
      return m_evtdef->MakeCopy(evtdef);
    }

    // electron related
    bool passLikelihood(int icontainer,ElectronID::ElectronID id);
    bool ptorderedLikelihood(unsigned int i,ElectronID::ElectronID id);
    double getLikelihoodResult(int i,ElectronID::ElectronID id);
    int ContinuousElectronID(int iptordered);
    int ContinuousMuonID(int iptordered);

    bool passMuonQuality(int icontainer,xAOD::Muon::Quality q);
    bool ptorderedPassMuonQuality(int iptordered,xAOD::Muon::Quality q);

    //double GetEventWeight(void){return m_weight_pu;};

    double GetEventWeight(void){
        double weight = m_weight_pu;
        if (GetEventVariable(vnSignalLeptons) < 3) return weight;
        if (!m_evtdef) return weight;
        weight *= GetSignalLeptonVariable(lepSFRecoIdIso,0);
        weight *= GetSignalLeptonVariable(lepSFRecoIdIso,1);
        weight *= GetSignalLeptonVariable(lepSFRecoIdIso,2);
        weight *= GetEventVariable(vEvtTrigSF);
        weight *= GetEventVariable(vAntiIDSF_lep3);
        
        return weight;
    };

    double GetEventWeightSpecial(void){
      //MSG_INFO("NsignalLeptons: " << GetEventVariable(vnSignalLeptons));
      double weight = m_weight; // including xsec, kfac, etc.
      weight *= m_prwTool->getCombinedWeight( *eventInfo );
      if (GetEventVariable(vnSignalLeptons) < 3) return weight;
      if (!m_evtdef) return weight;
      if (m_evtdef->Wlep_index        < 0) return weight;
      if (m_evtdef->leadZlep_index    < 0) return weight;
      if (m_evtdef->subleadZlep_index < 0) return weight;
      Particle w  = m_evtdef->WLepton();
      Particle z1 = m_evtdef->LeadZLepton();
      Particle z2 = m_evtdef->SubleadZLepton();
      //MSG_INFO((w.obj_type  == ObjType::Electron) << (z1.obj_type == ObjType::Electron) << (z2.obj_type == ObjType::Electron));
      if (w.obj_type  == ObjType::Electron) weight *= GetContainerEleSF_RecoIDIso(w.i_cont ,ElectronID::TightLLH ,1/*is_d0z0*/,AnaIso::Gradient);
      if (z1.obj_type == ObjType::Electron) weight *= GetContainerEleSF_RecoIDIso(z1.i_cont,ElectronID::MediumLLH,1/*is_d0z0*/,AnaIso::GradientLoose);
      if (z2.obj_type == ObjType::Electron) weight *= GetContainerEleSF_RecoIDIso(z2.i_cont,ElectronID::MediumLLH,1/*is_d0z0*/,AnaIso::GradientLoose);
      if (w.obj_type  == ObjType::Muon) weight *= GetContainerMuonSF_IDIsoTTVA(w.i_cont ,xAOD::Muon::Quality::Medium,AnaIso::GradientLoose);
      if (z1.obj_type == ObjType::Muon) weight *= GetContainerMuonSF_IDIsoTTVA(z1.i_cont,xAOD::Muon::Quality::Medium,AnaIso::GradientLoose);
      if (z2.obj_type == ObjType::Muon) weight *= GetContainerMuonSF_IDIsoTTVA(z2.i_cont,xAOD::Muon::Quality::Medium,AnaIso::GradientLoose);
      weight *= getTriggerSF_Winter2016WZAnalysis();
      return weight;
    }

    int GetNPV();
    double GetSignalIsolation(int iptordered,AnaIso::AnaIso isotype_ele,AnaIso::AnaIso isotype_mu,bool isAntiId=false);
    double GetSignalLeptonVariable(LepVariable v,Variable lepindex);
    double GetSignalLeptonVariable(LepVariable v,int iptordered,bool isAntiId=false);
    double GetContainerLeptonVariable(LepVariable v,int icontainer);
    int GetSignalLeptonContainerIndex(int iptordered);
    double GetJetVariable(JetVariable v,int iptordered);
    
    TLorentzVector GetSignalLeptonTLV(int iptordered);
    TLorentzVector CorrectedElectronTLV(int icontainer);
    TLorentzVector GetContainerEleTLV(int icontainer);
    TLorentzVector GetContainerMuonTLV(int icontainer);
    double EleIsolation(int icontainer,AnaIso::AnaIso isoenum);
    double EleD0Significance(int icontainer);
    double EleEtaBE(int icontainer);
    double EleD0(int icontainer);
    double ElePt(int icontainer);
    double EleZ0(int icontainer);
    int EleBlayer(int icontainer);
    double MuonIsolation(int icontainer,AnaIso::AnaIso isoenum);
    double MuonD0Significance(int icontainer);
    double MuonD0(int icontainer);
    double MuonPt(int icontainer);
    double MuonZ0(int icontainer);

    double MllForEwkRejection(int iptordered);

    bool VariableExists(Variable v){(void)v; return true;};
    bool LepVariableExists(LepVariable v){(void)v; return true;};
    bool JetVariableExists(JetVariable v){(void)v; return true;};

    LeptonTruthLabel getMuonTruthLabel(int i);
    LeptonTruthLabel getElectronTruthLabel(int i);

    void Reset(void){m_EventVariables.clear();m_LepVariables.clear();}
    double GetEventVariable(PSL::Variable v);
    //void UpdateEventVariable(PSL::Variable v,double d);
    bool PassTrigger(void);
    bool PassTrigger(Trigger2015 t);
    void FillTriggerBits(void);
    bool TriggerMatch(const std::string& trigname);
    // Trigger scale factor functions
    double getTriggerSF_Winter2016WZAnalysis(void);
    std::pair<double,double> getMuonTriggerWeight(int icontainer);
    std::pair<double,double> getElectronTriggerWeight(int icontainer);

    Particle LeadingTruthJet(void)   {
      if (truth_jets.size() < 1) return Particle(); return truth_jets.at(0);
    };
    Particle SubleadingTruthJet(void){
      if (truth_jets.size() < 2) return Particle(); return truth_jets.at(1);
    };

    // configuration
    void ConfigureRun(unsigned int runNumber, std::string samp,float xsec_k_f,float sumw);
    void PrintRunConfiguration(void);
    void PrintConfiguration(void);
    
    // these are the functions inherited from Algorithm
    virtual EL::StatusCode setupJob (EL::Job& job);
    virtual EL::StatusCode fileExecute ();
    virtual EL::StatusCode histInitialize ();
    virtual EL::StatusCode changeInput (bool firstFile);
    virtual EL::StatusCode initialize ();
    virtual EL::StatusCode execute ();
    virtual EL::StatusCode postExecute ();
    virtual EL::StatusCode finalize ();
    virtual EL::StatusCode histFinalize ();

#ifndef __MAKECINT__
    xAOD::TEvent *m_event; //!
    const xAOD::EventInfo* eventInfo; //!
    xAOD::JetContainer* jets; //!
    xAOD::ShallowAuxContainer* jets_aux; //! // for XSUSYObjDef
    xAOD::ElectronContainer* electrons; //!
    xAOD::ShallowAuxContainer* electrons_aux; //!
    xAOD::MuonContainer* muons; //!
    xAOD::ShallowAuxContainer* muons_aux; //!
    xAOD::TauJetContainer* taus; //!
    xAOD::ShallowAuxContainer* taus_aux; //!
    xAOD::PhotonContainer* photons; //!
    xAOD::ShallowAuxContainer* photons_aux; //!
    xAOD::MissingETContainer* met; //!
    xAOD::MissingETAuxContainer* met_aux; //!
    //xAOD::IParticleContainer* leps; //!
    const xAOD::JetContainer* truthjets; //!
    const xAOD::MissingETContainer* truthmet; //!
    const xAOD::TruthParticleContainer* truthparticles; //!
    const xAOD::TruthParticleContainer* truthelectrons; //!
    const xAOD::TruthParticleContainer* truthmuons; //!
    const xAOD::TruthParticleContainer* truthtaus; //!
    const xAOD::TruthParticleContainer* truthneutrinos; //!
    const xAOD::TruthParticleContainer* truthphotons; //!
    //
    TrigConf::xAODConfigTool* m_trigConfTool; //!
    Trig::TrigDecisionTool* m_trigDecTool; //!
    GoodRunsListSelectionTool* m_grl; //!
    //CP::PileupReweightingTool* m_prwTool; //!
    ToolHandle<CP::IPileupReweightingTool> m_prwTool;//!
    //
    std::map<PSL::ElectronID::ElectronID,AsgElectronLikelihoodTool*> m_lhtools; //!
    std::map<PSL::ElectronSF,AsgElectronEfficiencyCorrectionTool*> m_elesfs; //!
    std::map<PSL::MuonSF,CP::MuonEfficiencyScaleFactors*> m_muonsfs; //!
    CP::MuonTriggerScaleFactors *m_muonTriggerSFTool; //!
    CP::MuonTriggerScaleFactors *m_muonTriggerSFToolNOSYST; //!
    std::map<PSL::AnaIso::AnaIso,CP::IsolationSelectionTool*> m_isotools; //!
    //
    CP::MuonSelectionTool *m_muonSelection; //!

    bool applySystematicVariation(const CP::SystematicSet& systConfig);

    double GetContainerMuonSF(int icontainer,xAOD::Muon::Quality q,AnaIso::AnaIso iso_wp=AnaIso::None,bool skip_id=false,bool skip_iso=false, bool skip_ttva=false);
    double GetContainerEleSF (int icontainer,ElectronID::ElectronID e,bool is_d0z0,AnaIso::AnaIso iso_wp=AnaIso::None,bool skip_reco=false,bool skip_id=false,bool skip_iso=false);

    double GetContainerMuonSF_IDOnly   (int icont,xAOD::Muon::Quality q                      ){return GetContainerMuonSF(icont,q,AnaIso::None,false,true ,true );}
    double GetContainerMuonSF_IsoOnly  (int icont,xAOD::Muon::Quality q,AnaIso::AnaIso iso_wp){return GetContainerMuonSF(icont,q,iso_wp      ,true ,false,true );}
    double GetContainerMuonSF_IDIso    (int icont,xAOD::Muon::Quality q,AnaIso::AnaIso iso_wp){return GetContainerMuonSF(icont,q,iso_wp      ,false,false,true );}
    double GetContainerMuonSF_IDIsoTTVA(int icont,xAOD::Muon::Quality q,AnaIso::AnaIso iso_wp){return GetContainerMuonSF(icont,q,iso_wp      ,false,false,false);}

    double GetContainerEleSF_RecoOnly (int icont                                                            ){return GetContainerEleSF(icont,ElectronID::None,false  ,AnaIso::None,false,true ,true );};
    double GetContainerEleSF_IDOnly   (int icont,ElectronID::ElectronID e,bool is_d0z0                      ){return GetContainerEleSF(icont,e               ,is_d0z0,AnaIso::None,true ,false,true );};
    double GetContainerEleSF_IsoOnly  (int icont,ElectronID::ElectronID e,bool is_d0z0,AnaIso::AnaIso iso_wp){return GetContainerEleSF(icont,e               ,is_d0z0,iso_wp      ,true ,true ,false);};
    double GetContainerEleSF_RecoIDIso(int icont,ElectronID::ElectronID e,bool is_d0z0,AnaIso::AnaIso iso_wp){return GetContainerEleSF(icont,e               ,is_d0z0,iso_wp      ,false,false,false);};
    //double GetEleSF_IDIsoTrigger();

    double GetContainerEleSF_RecoIDIso(int icont,ElectronSF sf){
      if (sf == EleFullSF_Reco_LooseAndBLayerLLH_d0z0_isolLooseTrackOnly) return GetContainerEleSF_RecoIDIso(icont,ElectronID::LooseAndBLayerLLH,true,AnaIso::LooseTrackOnly);
      if (sf == EleFullSF_Reco_MediumLLH_d0z0_v8_isolGradientLoose      ) return GetContainerEleSF_RecoIDIso(icont,ElectronID::MediumLLH        ,true,AnaIso::GradientLoose );
      if (sf == EleFullSF_Reco_TightLLH_d0z0_v8_isolGradient            ) return GetContainerEleSF_RecoIDIso(icont,ElectronID::TightLLH         ,true,AnaIso::Gradient      );
      MSG_ERROR("No support for operating point.");
      exit(1);
      return 1;
    }

    double GetContainerMuonSF_IDIsoTTVA(int icont,MuonSF sf){
      if (sf == MuFullSF_Medium_d0z0_isolGradientLoose) return GetContainerMuonSF_IDIsoTTVA(icont,xAOD::Muon::Medium,AnaIso::GradientLoose );
      if (sf == MuFullSF_Loose_d0z0_isolLooseTrackOnly) return GetContainerMuonSF_IDIsoTTVA(icont,xAOD::Muon::Loose ,AnaIso::LooseTrackOnly);
      MSG_ERROR("No support for operating point.");
      exit(1);
      return 1;
    }

    double GetSignalEleSF_RecoIDIso(int iptordered,ElectronSF sf){
      if (!m_evtdef->SignalLeptonIsElectron(iptordered)) return 1;
      return GetContainerEleSF_RecoIDIso(m_evtdef->GetContainerIndex(iptordered),sf);
    }

    double GetSignalMuonSF_IDIsoTTVA(int iptordered,MuonSF sf){
      if (!m_evtdef->SignalLeptonIsMuon(iptordered)) return 1;
      return GetContainerMuonSF_IDIsoTTVA(m_evtdef->GetContainerIndex(iptordered),sf);
    }

    double GetSignalEleSF_RecoIDIso(int iptordered,ElectronID::ElectronID e,bool is_d0z0,AnaIso::AnaIso iso_wp){
      if (!m_evtdef->SignalLeptonIsElectron(iptordered)) return 1;
      return GetContainerEleSF_RecoIDIso(m_evtdef->GetContainerIndex(iptordered),e,is_d0z0,iso_wp);
    }
    
    double GetSignalMuonSF_IDIsoTTVA(int iptordered,xAOD::Muon::Quality q,AnaIso::AnaIso iso_wp){
      if (!m_evtdef->SignalLeptonIsMuon(iptordered)) return 1;
      return GetContainerMuonSF_IDIsoTTVA(m_evtdef->GetContainerIndex(iptordered),q,iso_wp);
    }

    const xAOD::Electron* getElectron(int i){return electrons->at(i);};
    const xAOD::Muon*     getMuon(int i){return muons->at(i);};
    const xAOD::TauJet*   getTau(int i){return taus->at(i);};
    const xAOD::Jet*      getJet(int i){return jets->at(i);};
    const xAOD::Photon*   getPhoton(int i){return photons->at(i);};
    const xAOD::Vertex*   GetPrimaryVertex();

    const xAOD::Jet*                getTruthJet(int i){return truthjets->at(i);};
    const xAOD::TruthParticle*      getTruthParticle(int i){return truthparticles->at(i);};
    const xAOD::TruthParticle*      getTruthElectron(int i){return truthelectrons->at(i);};
    const xAOD::TruthParticle*      getTruthMuon(int i){return truthmuons->at(i);};
    const xAOD::TruthParticle*      getTruthNeutrino(int i){return truthneutrinos->at(i);};
    const xAOD::TruthParticle*      getTruthTau(int i){return truthtaus->at(i);};
    const xAOD::TruthParticle*      getTruthPhoton(int i){return truthphotons->at(i);};
    const xAOD::MissingET*          getTruthMET(){return (*truthmet)["NonInt"];};

    SG::AuxElement::Accessor<unsigned int> m_acc_particleType;
    SG::AuxElement::Accessor<unsigned int> m_acc_particleOrigin;
    SG::AuxElement::Accessor<unsigned int> m_acc_particleOutCome;
    SG::AuxElement::Accessor<int> m_acc_motherID;
    unsigned int ParticleType(const xAOD::IParticle& p){return m_acc_particleType(p);}
    unsigned int ParticleOrigin(const xAOD::IParticle& p){return m_acc_particleOrigin(p);}
    unsigned int ParticleOutCome(const xAOD::IParticle& p){return m_acc_particleOutCome(p);}
    int ParticleMother(const xAOD::IParticle& p){return m_acc_motherID(p);}

    SG::AuxElement::Accessor<int> m_acc_truthType;
    SG::AuxElement::Accessor<int> m_acc_truthOrigin;
    SG::AuxElement::Accessor<float> m_acc_e_dressed;
    SG::AuxElement::Accessor<float> m_acc_pt_dressed;
    SG::AuxElement::Accessor<float> m_acc_eta_dressed;
    SG::AuxElement::Accessor<float> m_acc_phi_dressed;

    int TruthType(const xAOD::IParticle& p){
      // If you are trying to access muon information from STDM derivations you need to do so through
      // the linked truth particle.
      if (IsData()) return -1;
      if (!m_acc_truthType.isAvailable(p)) {
        const xAOD::TruthParticle *tp = xAOD::TruthHelpers::getTruthParticle(p);
        if (tp && m_acc_particleType.isAvailable(*tp)) return m_acc_particleType(*tp);
        else return 0;
      }
      return m_acc_truthType(p);
    }
    int TruthOrigin(const xAOD::IParticle& p){
      // If you are trying to access muon information from STDM derivations you need to do so through
      // the linked truth particle.
      if (IsData()) return -1;
      if (!m_acc_truthOrigin.isAvailable(p)) {
        const xAOD::TruthParticle *tp = xAOD::TruthHelpers::getTruthParticle(p);
        if (tp && m_acc_particleOrigin.isAvailable(*tp)) return m_acc_particleOrigin(*tp);
        else return 0;
      }
      return m_acc_truthOrigin(p);
    }

    int get_el_n(void){return electrons->size();};
    unsigned long long int get_EventNumber(void){return eventInfo->eventNumber();};

#endif // not cint
    
    // this is needed to distribute the algorithm to the workers
    ClassDef(xAODWrapper, 1);
  };
}

#endif // pennSoftLepton_xAODWrapper_H

#endif // SKIP_XAOD_COMPILATION
