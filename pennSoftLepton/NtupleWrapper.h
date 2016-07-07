#ifndef pennSoftLepton_NtupleWrapper_H
#define pennSoftLepton_NtupleWrapper_H

#include <EventLoop/Algorithm.h>
#include <map>
#include <algorithm>
#include "pennSoftLepton/Messaging.h"
#include "pennSoftLepton/Variables.h"
#include "pennSoftLepton/Sample.h"
#include "pennSoftLepton/EventDefinition.h"

#include "TLorentzVector.h"

class TTree;

namespace PSL
{
  class EventDefinition;

  class NtupleWrapper : public EL::Algorithm
  {
  public:

    EventDefinition* m_evtdef; //!
    EventDefinition tree_evtdef; //!
    void SetEventDefinitionPointer(EventDefinition* evt){
      //m_EventVariables.clear(); // want to refresh all of these quantities // no.
      m_evtdef = evt;
    };
    void CopyEventDefinitionFromEDM(EventDefinition& evtdef);

    Sample m_CurrentSample; //!
    int n_processed; //!
    std::map<unsigned int,TH1F*> m_sample_stats; //! // uhh this will not really be used here.

    NtupleWrapper();
    void PrintAllVariablesForDebugging();
    std::string CompiledEDMType(void){return "FlatNtuple";};

    bool IsMC(){return m_CurrentSample != kdata;};
    bool IsData(){return m_CurrentSample == kdata;};
    void ResetSample(void);
    bool ptorderedLikelihood(unsigned int i,ElectronID::ElectronID id);
    bool ptorderedPassMuonQuality(unsigned int i,xAOD::Muon::Quality id);
    double GetEventWeight(void){return GetEventVariable(vTotalWeightNoSF);};
    double GetEventWeightNoPupNoSF(void){return GetEventVariable(vTotalWeightNoPupNoSF);};
    double GetSignalIsolation(int iptordered,AnaIso::AnaIso isotype_ele,AnaIso::AnaIso isotype_mu,bool isAntiId=false);
    double GetContainerIsolation(int icontainer,AnaIso::AnaIso isotype_ele,AnaIso::AnaIso isotype_mu);
    double GetContainerLeptonVariable(LepVariable v,int icontainer);
    int GetSignalLeptonContainerIndex(int iptordered);
    double GetSignalLeptonVariable(LepVariable v,Variable lepindex);
    double GetSignalLeptonVariable(LepVariable v,int iptordered,bool isAntiId=false);
    double GetJetVariable(JetVariable v,int iptordered);
    TLorentzVector GetContainerLeptonTLV(int icontainer);
    TLorentzVector GetContainerEleTLV(int icontainer);
    TLorentzVector GetContainerMuonTLV(int icontainer);
    TLorentzVector GetSignalLeptonTLV(int iptordered);
    TLorentzVector GetContainerJetTLV(int icontainer);
    double GetEventVariable(PSL::Variable v);
    unsigned long long int get_EventNumber(void){return m_eventnumber;};
    bool PassTrigger(Trigger2015 t);
    bool PassTrigger(void){return true;}; // yeah I wouldn't use this.
    int get_el_n(void){return 1;};
    std::vector<PSL::Trigger2015> GetTriggerBitmask(){return std::vector<PSL::Trigger2015>();}

    bool EventKilled(void){return false;};

    bool AssertContainerLeptonIsElectron(int icontainer,bool fatal=true);
    bool AssertContainerLeptonIsMuon(int icontainer,bool fatal=true);
    double EleIsolation(int icontainer,AnaIso::AnaIso isoenum);
    double EleD0Significance(int icontainer);
    double EleEtaBE(int icontainer);
    double EleD0(int icontainer);
    double ElePt(int icontainer);
    double EleZ0(int icontainer);
    double MuonIsolation(int icontainer,AnaIso::AnaIso isoenum);
    double MuonD0Significance(int icontainer);
    double MuonD0(int icontainer);
    double MuonPt(int icontainer);
    double MuonZ0(int icontainer);

    double getLikelihoodResult(int i,ElectronID::ElectronID id){
      (void)i;
      (void)id;
      return 1;
    }
    bool passLikelihood(int icontainer,ElectronID::ElectronID id);
    bool passMuonQuality(int icontainer,xAOD::Muon::Quality id);

    AnaIso::AnaIso m_el_defaultcaloiso; /* configurable */
    AnaIso::AnaIso m_el_defaulttrkiso; /* configurable */
    AnaIso::AnaIso m_mu_defaultcaloiso; /* configurable */
    AnaIso::AnaIso m_mu_defaulttrkiso; /* configurable */

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

    bool VariableExists(Variable v){return !branchMissing(v,m_missing_variables);};
    bool LepVariableExists(LepVariable v){ return !branchMissing(v,m_missing_lepvariables);};
    bool JetVariableExists(JetVariable v){return !branchMissing(v,m_missing_jetvariables);};
    bool TriggerVariableExists(Trigger2015 v){return !branchMissing(v,m_missing_trig);};
    bool MuonSFExists(MuonSF v){return !branchMissing(v,m_missing_muonsf);};
    bool EleSFExists(ElectronSF v){return !branchMissing(v,m_missing_elesf);};

  public:
    Float_t m_EventVariableHolder[PSL::N_VARIABLES]; //!
    Int_t m_EventVariableHolder_int[PSL::N_VARIABLES]; //!
    Int_t m_TrigHolder[PSL::N_TRIGGERS]; //!
    std::vector< std::vector<float>* > m_LepVariableHolder; //!
    std::vector< std::vector<float>* > m_JetVariableHolder; //!
    std::vector< std::vector<int>* > m_ElectronIDHolder; //!
    std::vector< std::vector<int>* > m_MuonIDHolder; //!
    std::vector< std::vector<float>* > m_LepIsoHolder; //!
    std::vector< std::vector<float>* > m_MuonSFHolder; //!
    std::vector< std::vector<float>* > m_EleSFHolder; //!
    unsigned long long int m_eventnumber; //!

    bool hasBranch(TTree* tree,std::string br);
    
    template <class T>
      bool branchMissing(T var, std::vector<T>& missing_variables){
      if (std::find(missing_variables.begin(),missing_variables.end(),var) != missing_variables.end()){
        return true;
      }
      return false;
    }

    template <class T> bool SetBranchStatusAddress(TTree* tree,const char* s,T addr,bool printWarning=true);

    std::vector<PSL::Variable> m_missing_variables; //!
    std::vector<PSL::LepVariable> m_missing_lepvariables; //!
    std::vector<PSL::JetVariable> m_missing_jetvariables; //!
    std::vector<PSL::ElectronID::ElectronID> m_missing_eleid; //!
    std::vector<xAOD::Muon::Quality> m_missing_muonid; //!
    std::vector<PSL::AnaIso::AnaIso> m_missing_iso; //!
    std::vector<PSL::Trigger2015> m_missing_trig; //!
    std::vector<PSL::ElectronSF> m_missing_elesf; //!
    std::vector<PSL::MuonSF> m_missing_muonsf; //!

    double GetContainerMuonSF_IDIsoTTVA(int icont,xAOD::Muon::Quality q,AnaIso::AnaIso iso_wp);
    double GetContainerEleSF_RecoIDIso (int icont,ElectronID::ElectronID e,bool is_d0z0,AnaIso::AnaIso iso_wp);
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

    double GetSignalEleSF_RecoIDIso(int iptordered,ElectronSF sf);
    double GetSignalMuonSF_IDIsoTTVA(int iptordered,MuonSF sf);
    double GetSignalEleSF_RecoIDIso(int iptordered,ElectronID::ElectronID e,bool is_d0z0,AnaIso::AnaIso iso_wp);
    double GetSignalMuonSF_IDIsoTTVA(int iptordered,xAOD::Muon::Quality q,AnaIso::AnaIso iso_wp);

  public:    
    ClassDef(NtupleWrapper,1);
  };
}

#endif // pennSoftLepton_NtupleWrapper_H
