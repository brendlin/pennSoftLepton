#include "pennSoftLepton/switch_d3pd_xaod.h"
#ifndef SKIP_D3PD_COMPILATION

#ifndef pennSoftLepton_D3PDWrapper_H
#define pennSoftLepton_D3PDWrapper_H

#include <map>
#include <cmath>

#include <EventLoop/Algorithm.h>
//#include <xAODRootAccess/TEvent.h>

#include "pennSoftLepton/D3PDVariables.h"
#include "pennSoftLepton/UtilCore.h"
#include "pennSoftLepton/EventDefinition.h"
#include "pennSoftLepton/Messaging.h"
#include "pennSoftLepton/Variables.h"
#include "pennSoftLepton/Sample.h"

// For cross sections
#include "SUSYTools/SUSYCrossSection.h"
#include "egammaAnalysisUtils/CaloIsoCorrection.h"

#include "TTree.h"
#include "TFile.h"
#include "TLorentzVector.h"

namespace PSL
{
  
  class D3PDWrapper : public EL::Algorithm
  {
    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
  public:
    // float cutValue;
    //std::string configured_name;
    std::map<unsigned int,PSL::Sample> m_run_sample_map;
    std::map<unsigned int,float> m_run_xsec_map;
    std::map<unsigned int,float> m_run_sumw_map;
    //* \brief We want to force a crash if a sample label dne. Not true for skimming though. */
    bool m_allowUnlabeledSamples;
    void set_allowUnlabeledSamples(bool b){m_allowUnlabeledSamples = b;};
    
    // variables that don't get filled at submission time should be
    // protected from being send from the submission node to the worker
    // node (done by the //!)
    
  private:
    
  public:
    std::map<PSL::Variable,double> m_EventVariables; //!
    Sample m_CurrentSample; //!
    int n_processed; //!
    SUSY::CrossSectionDB m_susy_xsecDB; //!
    std::map<unsigned int,TH1F*> m_sample_stats; //!

    float m_weight; //! This is the overall weight!
    float m_xsec; //!
    float m_kfac; //!
    float m_feff; //!
    float m_lumi; //!
    float m_sumw; //!
    double m_sumw_thisjob; //!

    // d3pd vector pointer variable treatment
    std::vector< std::vector<float>* > m_v_float; //!
    std::vector< std::vector<int>* > m_v_int; //!
    std::vector< std::vector<short>* > m_v_short; //!
    std::vector< std::vector<std::string>* > m_v_string; //!
    std::vector< std::vector< std::vector<unsigned int> >* > m_v_v_uint; //!
    std::vector< std::vector< std::vector<int> >* > m_v_v_int; //!
    std::vector< std::vector< std::vector<float> >* >  m_v_v_float; //!
    std::vector< std::vector<unsigned int>* > m_v_uint; //!

    // Methods to get a pointer
    unsigned int Get(D3PD::Var_UInt_t v){
      if (v == D3PD::EventNumber      ) return EventNumber      ;
      if (v == D3PD::RunNumber        ) return RunNumber        ;
      if (v == D3PD::mc_channel_number) return mc_channel_number;
      if (v == D3PD::larError         ) return larError         ;
      if (v == D3PD::tileError        ) return tileError        ;
      if (v == D3PD::lbn              ) return lbn              ;
      if (v == D3PD::trig_DB_SMK      ) return trig_DB_SMK      ;
      return 0;
    }

    const unsigned int* GetPointer(D3PD::Var_UInt_t v){
      if (v == D3PD::EventNumber      ) return &EventNumber      ;
      if (v == D3PD::RunNumber        ) return &RunNumber        ;
      if (v == D3PD::mc_channel_number) return &mc_channel_number;
      if (v == D3PD::larError         ) return &larError         ;
      if (v == D3PD::tileError        ) return &tileError        ;
      if (v == D3PD::lbn              ) return &lbn              ;
      if (v == D3PD::trig_DB_SMK      ) return &trig_DB_SMK      ;
      return 0;
    }

    int Get(D3PD::Var_Int v){
      if (v == D3PD::el_MET_Egamma10NoTau_n) return el_MET_Egamma10NoTau_n;
      if (v == D3PD::el_n) return el_n;
      if (v == D3PD::mu_staco_n) return mu_staco_n;
      if (v == D3PD::tau_n) return tau_n;
      if (v == D3PD::jet_AntiKt4LCTopo_n) return jet_AntiKt4LCTopo_n;
      if (v == D3PD::trig_EF_el_n) return trig_EF_el_n;
      if (v == D3PD::trig_Nav_n) return trig_Nav_n;
      return 0;
    }

    const int* GetPointer(D3PD::Var_Int v){
      if (v == D3PD::el_MET_Egamma10NoTau_n) return &el_MET_Egamma10NoTau_n;
      if (v == D3PD::el_n                  ) return &el_n                  ;
      if (v == D3PD::mu_staco_n            ) return &mu_staco_n            ;
      if (v == D3PD::tau_n                 ) return &tau_n                 ;
      if (v == D3PD::jet_AntiKt4LCTopo_n   ) return &jet_AntiKt4LCTopo_n   ;
      if (v == D3PD::trig_EF_el_n          ) return &trig_EF_el_n          ;
      if (v == D3PD::trig_Nav_n            ) return &trig_Nav_n            ;
      return 0;
    }

    std::vector<float>* Get(D3PD::Var_vector_float v){return m_v_float[v];};
    std::vector<int>* Get(D3PD::Var_vector_int v){return m_v_int[v];};
    std::vector<short>* Get(D3PD::Var_vector_short v){return m_v_short[v];};
    std::vector<std::string>* Get(D3PD::Var_vector_string v){return m_v_string[v];};
    std::vector<std::vector<unsigned int> >* Get(D3PD::Var_vector_vector_UInt_t v){return m_v_v_uint[v];};
    std::vector<std::vector<int> >* Get(D3PD::Var_vector_vector_int v){return m_v_v_int[v];};
    std::vector<std::vector<float> >* Get(D3PD::Var_vector_vector_float v){return m_v_v_float[v];};
    std::vector<unsigned int>* Get(D3PD::Var_vector_UInt_t v){return m_v_uint[v];};

    // begin d3pd variables - integers and float (not vector pointers)
    UInt_t EventNumber; //!
    UInt_t RunNumber; //!
    UInt_t mc_channel_number; //!
    UInt_t larError; //!
    UInt_t tileError; //!
    UInt_t lbn; //!
    UInt_t trig_DB_SMK; //!
    // Add N_UInt_t

    int el_n; //!
    int mu_staco_n; //!    
    int jet_AntiKt4LCTopo_n; //!
    int tau_n; //!
    int el_MET_Egamma10NoTau_n; //!
    int trig_EF_el_n; //!
    int trig_Nav_n; //!
    int trig_EF_trigmuonef_n; //!
    // Add N_int

    Bool_t EF_xe80_tclcw; //!
    Bool_t EF_e24vhi_medium1; //!
    Bool_t EF_e60_medium1; //!
    Bool_t EF_2e12Tvh_loose1; //!
    Bool_t EF_e24vh_medium1_e7_medium1; //!
    Bool_t EF_e18vh_medium1_2e7T_medium1; //!
    Bool_t EF_mu24i_tight; //!
    Bool_t EF_2mu13; //!
    Bool_t EF_mu18_tight_mu8_EFFS; //!
    Bool_t EF_3mu6; //!
    Bool_t EF_mu18_tight_2mu4_EFFS; //!
    Bool_t EF_mu18_tight_e7_medium1; //!
    Bool_t EF_e12Tvh_medium1_mu8; //!
    Bool_t EF_2e7T_medium1_mu6; //!
    Bool_t EF_e7T_medium1_2mu6; //!
    // Add N_Bool_t

    float MET_RefFinal_et           ; //!
    float MET_RefFinal_etx          ; //!
    float MET_RefFinal_ety          ; //!
    float MET_RefFinal_phi          ; //!
    float MET_RefFinal_sumet        ; //!
    float actualIntPerXing          ; //!
    float averageIntPerXing         ; //!
    float Eventshape_rhoKt4LC       ; //!
    float MET_CellOut_Eflow_STVF_etx; //!
    float MET_CellOut_Eflow_STVF_ety; //!
    float MET_CellOut_Eflow_STVF_sumet; //!
    float MET_Egamma10NoTau_RefGamma_etx; //!
    float MET_Egamma10NoTau_RefGamma_ety; //!
    float MET_Egamma10NoTau_RefGamma_sumet; //!
    float MET_Egamma10NoTau_CellOut_etx; //!
    float MET_Egamma10NoTau_CellOut_ety; //!
    float MET_Egamma10NoTau_CellOut_sumet; //!
    float mc_event_weight; //!
    // Add N_float

    // this is a standard constructor
    D3PDWrapper ();
    std::string CompiledEDMType(void){return "D3PD";};

    //
    // These are variables that are provided that can be set by other algorithms
    // 
    EventDefinition* m_evtdef; //!
    void SetEventDefinitionPointer(EventDefinition* evt){
      m_EventVariables.clear(); // want to refresh all of these quantities
      m_evtdef = evt;
    };
    // skim indices should be distinct!
    std::vector<int> el_index_save; //!
    std::vector<int> mu_index_save; //!
    std::vector<int> tau_index_save; //!
    std::vector<int> jet_index_save; //!
    std::vector<int> GetThinIndices(D3PD::Var_Int v){
      if (v == D3PD::el_MET_Egamma10NoTau_n ) { sort(el_index_save .begin(),el_index_save .end()); return el_index_save ;}
      if (v == D3PD::el_n                   ) { sort(el_index_save .begin(),el_index_save .end()); return el_index_save ;}
      if (v == D3PD::mu_staco_n             ) { sort(mu_index_save .begin(),mu_index_save .end()); return mu_index_save ;}
      if (v == D3PD::tau_n                  ) { sort(tau_index_save.begin(),tau_index_save.end()); return tau_index_save;}
      if (v == D3PD::jet_AntiKt4LCTopo_n    ) { sort(jet_index_save.begin(),jet_index_save.end()); return jet_index_save;}
      return std::vector<int>();
    }
    void AddThinIndices(D3PD::Var_Int v,std::vector<int> vec){
      if (v == D3PD::el_MET_Egamma10NoTau_n) return AddThinIndices(el_index_save,vec);
      if (v == D3PD::el_n)                   return AddThinIndices(el_index_save,vec);
      if (v == D3PD::mu_staco_n)             return AddThinIndices(mu_index_save,vec);
      if (v == D3PD::tau_n)                  return AddThinIndices(tau_index_save,vec);
      if (v == D3PD::jet_AntiKt4LCTopo_n)    return AddThinIndices(jet_index_save,vec);
    }
    void AddThinIndices(std::vector<int>& save,std::vector<int> vec){
      for (unsigned int i=0;i<vec.size();i++){
        if (std::count(save.begin(),save.end(),vec[i]) > 0) {
          continue;
        }
        else {
          save.push_back(vec[i]);
        }
      }
      // Need to sort later!
    }

    // electron related
    std::vector<PSL::Trigger2015> m_trigger_bitmask; /* configurable */
    bool isMC(void){return m_CurrentSample != kdata;};
    bool isData(void){return m_CurrentSample == kdata;}; // may have to be changed for DD stuff
    int get_EventNumber(){return EventNumber;};
    int get_el_n(void){return el_n;};
    double GetEventWeight(void){ if (isMC()) return m_weight; else return 1.;};
    bool ptorderedLikelihood(unsigned int i,ElectronID::ElectronID id){
      return false;
    };
    bool passLikelihood(int i,ElectronID::ElectronID id){
      MSG_VERBOSE("passLikelihood not implemented. " << i << " " << ConvertElectronIDToStr(id));
      return false;
    };
    double getLikelihoodResult(int i,ElectronID::ElectronID id){
      MSG_VERBOSE("getLikelihoodResult not implemented. " << i << " " << ConvertElectronIDToStr(id));
      return -1;
    };
    bool PassGRL(void){return true;}
    bool PassPrimaryVtx(void){return m_v_int[D3PD::vxp_nTracks]->size() && m_v_int[D3PD::vxp_nTracks]->at(0) > 4;};
    bool PassCosmicMuon(void);
    int NBaselineLeptons(void);
    int NSignalLeptons(void);
    bool PassCaloJetCut(void);
    bool PassBCH(void);
    double EleTrkIsolation(int i);
    double EleCaloIsolation(int i);
    double EleD0Significance(int i);
    double EleZ0(int i);
    double MuonTrkIsolation(int i);
    double MuonCaloIsolation(int i);
    double MuonD0Significance(int i);
    double MuonZ0(int i);
    double GetIsolation(int iptordered,AnaIso::AnaIso isotype_ele,AnaIso::AnaIso isotype_mu,bool isAntiId=false){
      (void)(isotype_ele+isotype_mu+isAntiId+iptordered);
      return 0;
    }

    /* bool matchesToElectronTriggerEF(int i,std::vector<int>& trig_branch,double deltar=0.15); */
    /* bool matchMuonTrigger(const TLorentzVector* lv, std::vector<int>* passTrig); */

    void Reset(void){m_EventVariables.clear();}
    void SetEventVariable(PSL::Variable v,double d);
    double GetEventVariable(PSL::Variable v);
    double GetLeptonVariable(Variable v,int iptordered);
    //void UpdateEventVariable(PSL::Variable v,double d);
    bool PassTrigger(unsigned int index){return false;};
    bool PassTrigger(void){return false;};

    // configuration
    void ConfigureRun(unsigned int runNumber, std::string samp,float xsec_k_f,float sumw);
    void PrintRunConfiguration(void);
    bool hasBranch(TTree* tree,std::string br);
    template <class T> void SetBranchStatusAddress(TTree* tree,const char* s,T addr,bool printWarning=true);
    
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
    
    // this is needed to distribute the algorithm to the workers
    ClassDef(D3PDWrapper, 1);
  };
}

#endif // pennSoftLepton_D3PDWrapper_H

#endif // ifndef SKIP_D3PD_COMPILATION
