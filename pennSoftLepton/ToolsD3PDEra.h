#include "pennSoftLepton/switch_d3pd_xaod.h"
#ifndef SKIP_D3PD_COMPILATION

#ifndef pennSoftLepton_ToolsD3PDEra_H
#define pennSoftLepton_ToolsD3PDEra_H

#include <EventLoop/StatusCode.h>
#include "pennSoftLepton/AlgBase.h"
#include "pennSoftLepton/D3PDVariables.h"

#include "PileupReweighting/TPileupReweighting.h"
#include "JVFUncertaintyTool/JVFUncertaintyTool.h"

#include "TrigMuonEfficiency/MuonTriggerMatching.h"
#include "TrigMuonEfficiency/ElectronTriggerMatching.h"
#include "TrigMuonEfficiency/TriggerNavigationVariables.h"

namespace PSL
{

  namespace Trigger2012{
    enum Trigger2012 {
      EF_xe80_tclcw
      ,EF_e24vhi_medium1
      ,EF_e60_medium1
      ,EF_2e12Tvh_loose1
      ,EF_e24vh_medium1_e7_medium1
      ,EF_e18vh_medium1_2e7T_medium1
      ,EF_mu24i_tight
      ,EF_2mu13
      ,EF_mu18_tight_mu8_EFFS
      ,EF_3mu6 // 1
      ,EF_mu18_tight_2mu4_EFFS // 1
      ,EF_mu18_tight_e7_medium1
      ,EF_e12Tvh_medium1_mu8
      ,EF_2e7T_medium1_mu6 // 1
      ,EF_e7T_medium1_2mu6 // 1
      ,n_Trig
    };
  }
  
  class ToolsD3PDEra : public AlgBase
  {
    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
  public:
    
  public:

    Root::TPileupReweighting*   m_pileup; //!
    JVFUncertaintyTool* m_jvfTool; //!

    TriggerNavigationVariables* m_triggerNavigationVariables; //!
    MuonTriggerMatching *m_muonTriggerMatchTool; //!
    ElectronTriggerMatching *m_electronTriggerMatchTool; //!


    // this is a standard constructor
    ToolsD3PDEra();
    //
    virtual EL::StatusCode changeInput (bool firstFile); //used

    bool init(void);
    void loop(void){return;};
    void finish(void);

    //bool matchDimuon(TLorentzVector v1,TLorentzVector v2,D3PD::Var_Bool_t var);
    
    void TriggerMatch(EventDefinition evtdef);
    bool matchesToEFTriggerEle(TLorentzVector& ele
                               ,std::vector<int>* EF
                               ,double EF_thresh=-1.
                               ,double deltar=0.15
                               );
    bool matchMuonTrigger(const TLorentzVector& lv, std::vector<int>* passTrig);

    // this is needed to distribute the algorithm to the workers
    ClassDef(ToolsD3PDEra, 1);
  };
  
}
#endif

#endif // SKIP_D3PD_COMPILATION
