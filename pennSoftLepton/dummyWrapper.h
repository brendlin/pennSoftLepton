#ifndef pennSoftLepton_dummyWrapper_H
#define pennSoftLepton_dummyWrapper_H

#include <map>

#include <EventLoop/Algorithm.h>
//#include <xAODRootAccess/TEvent.h>

#include "pennSoftLepton/Messaging.h"
#include "pennSoftLepton/Variables.h"
#include "pennSoftLepton/Sample.h"

namespace PSL
{
  class dummyWrapper : public EL::Algorithm
  {
    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
  public:
    // float cutValue;
    //std::string configured_name;
    std::map<int,PSL::Sample> m_run_sample_map;
    
    // variables that don't get filled at submission time should be
    // protected from being send from the submission node to the worker
    // node (done by the //!)
    
  private:
    
  public:
    //xAOD::TEvent *m_event; //!
    std::map<PSL::Variable,double> m_EventVariables; //!
    Sample m_CurrentSample; //!
    int n_processed; //!
    std::map<unsigned int,TH1F*> m_sample_stats; //!
    std::vector<PSL::Trigger2015> m_trigger_bitmask; /* configurable */
    
/* #ifndef __MAKECINT__ */
/*       const xAOD::EventInfo* eventInfo; //! */
/*       const xAOD::ElectronContainer* electrons; //! */
/*       const xAOD::ElectronAuxContainer* electrons_aux; //! */
/*       AsgElectronLikelihoodTool* m_lhtool; //! */
/*       const xAOD::Electron* getElectron(int i){return electrons->at(i);}; */
/* #endif // not cint */

    // 
    
    // this is a standard constructor
    dummyWrapper ();
    
    // event related
    int runNumber(void){return 999999;};
    int get_EventNumber(void){return 1;};
    int GetEventWeight(void){return 1;};
    
    // electron related
    int get_el_n(void){return 1;};
    bool passLikelihood(int i,ElectronID::ElectronID id){
      (void)i;
      (void)id;
      return true;};
    double getLikelihoodResult(int i,ElectronID::ElectronID id){
      (void)i;
      (void)id;
      return 1;
    };
    bool ptorderedLikelihood(unsigned int i,ElectronID::ElectronID id){
      (void)i;
      (void)id;
      return true;
    };
    double GetIsolation(int iptordered,AnaIso::AnaIso isotype_ele,AnaIso::AnaIso isotype_mu,bool isAntiId=false){
      (void)iptordered; 
      (void)isotype_ele;
      (void)isotype_mu;
      (void)isAntiId;
      return 1;
    };
    
    double GetEventVariable(PSL::Variable v);
    double GetLeptonVariable(LepVariable v,Variable lepindex){
      (void)v;
      (void)lepindex;
      return 1;
    };
    double GetLeptonVariable(LepVariable v,int iptordered,bool isAntiId=false){
      (void)v;
      (void)iptordered;
      (void)isAntiId;
      return 1;
    };
    //void UpdateEventVariable(PSL::Variable v,double d);
    
    // configuration
    void ConfigureRun(int runNumber, std::string samp);
    void PrintRunConfiguration(void);

    bool PassTrigger(int i){(void)i; return true;};
    
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
    ClassDef(dummyWrapper, 1);
  };
}

#endif // pennSoftLepton_dummyWrapper_H
