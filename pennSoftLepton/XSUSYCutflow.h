//////////////////////////////////////////////////////////
//
// Alternate version of XSUSYObjDefAlg for implimenting
//   SMWZ selection criteria
//
//////////////////////////////////////////////////////////

#include "pennSoftLepton/switch_d3pd_xaod.h"
#ifndef SKIP_XAOD_COMPILATION
//#ifdef BEFORE_SUSYTOOLS_000709

#ifndef pennSoftLepton_XSUSYCutflow_H
#define pennSoftLepton_XSUSYCutflow_H

#include <pennSoftLepton/ObjectSelectionBase.h>
#include "pennSoftLepton/UtilCore.h"

// overlap removal 
//   (locally here for testing purposes, will move to xAODWrapper later)
#include "AssociationUtils/OverlapRemovalInit.h"
#include "AssociationUtils/DeltaROverlapTool.h"
#include "AssociationUtils/EleMuSharedTrkOverlapTool.h"
#include "AssociationUtils/EleJetOverlapTool.h"
#include "AssociationUtils/MuJetOverlapTool.h"
#include "AssociationUtils/TauLooseEleOverlapTool.h"
#include "AssociationUtils/TauLooseMuOverlapTool.h"
#include "AssociationUtils/OverlapRemovalTool.h"

// trigger matching
//   (local for now)
//#include "TrigEgammaMatchingTool/TrigEgammaMatchingTool.h"
//#include "TrigMuonMatching/TrigMuonMatching.h"

// jet jvt
#include "JetJvtEfficiency/JetJvtEfficiency.h"

namespace ST{
  class SUSYObjDef_xAOD;
}

// Tool interfaces
namespace CP{
  class IMuonCalibrationAndSmearingTool;
  class IEgammaCalibrationAndSmearingTool;
  class IIsolationCorrectionTool;
  class IMuonEfficiencyScaleFactors;
}
class IJetUpdateJvt;
class IJetSelector;
//namespace Trig {
// class ITrigEgammaMatchingTool;
//  class ITrigMuonMatching;
//}

//#include "CPAnalysisExamples/errorcheck.h"
//#include <TError.h>

namespace PSL
{
  class XSUSYCutflow : public ObjectSelectionBase
  {
    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
  public:

    bool m_makefakentuples;
    void set_makefakentuples(bool b){m_makefakentuples = b;};

    // variables that don't get filled at submission time should be
    // protected from being send from the submission node to the worker
    // node (done by the //!)
  public:

    /* // All of the below have to be initialized at the start of an event */
    EventDefinition m_evtdef; //!
    /* // All of the above have to be initialized at the start of an event */

    // this is a standard constructor
    XSUSYCutflow();
    //
    bool init(void);
    void loop(void);
    void finish(void);

    virtual void PrintConfiguration(void);

#ifndef __MAKECINT__
    ST::SUSYObjDef_xAOD* m_SUSYObjDef; //!

    std::string m_ConfigFile;
    void set_ConfigFile(std::string s){m_ConfigFile = s;};
    void set_sysVariation(std::string s){m_sysVariation = s;};
    std::string m_sysVariation; /* configurable */

    CP::SystematicSet systematicVariation; //!

    void set_hackedCode(bool b){m_hackedCode = b;};
    bool m_hackedCode; /* configurable */

    // could convince SUSYTools people to put these in their .h file....
    SG::AuxElement::Decorator<char> dec_baseline; //!
    SG::AuxElement::Decorator<char> dec_signal; //!
    SG::AuxElement::Decorator<char> dec_passOR; //!
    SG::AuxElement::Decorator<char> dec_badjet; //!
    SG::AuxElement::Decorator<char> dec_bjet; //!
    SG::AuxElement::Decorator<char> dec_bad; //!
    SG::AuxElement::Decorator<char> dec_cosmic; //!
    // for leptons that pass the Z and W selections
    SG::AuxElement::Decorator<char> dec_zlep; //!
    SG::AuxElement::Decorator<char> dec_wlep; //!
    // lepton-lepton OR
    SG::AuxElement::Decorator<char> dec_passOR_ll; //!
    SG::AuxElement::Decorator<char> dec_passOR_MuDecision; //!
    SG::AuxElement::Decorator<char> dec_passOR_JetClean; //!
    // special baseline for jet-jet OR
    SG::AuxElement::Decorator<char> dec_baselineForJetCleaning; //!

    virtual void ApplyLeptonVeto(void){};

    //virtual void OverlapRemoval(const xAOD::ElectronContainer *electrons, const xAOD::MuonContainer *muons, const xAOD::JetContainer *jets,const bool doHarmonization = true,const double dRejet = 0.2, const double dRjetmu = 0.4, const double dRjete = 0.4, const double dRemu = 0.01, const double dRee = 0.05);

#endif // not cint

    // Define object-level selection functions
    bool isBaselineMuon(int icontainer);
    bool isBaselineElectron(int icontainer);
    bool isZMuon(int icontainer);
    bool isZElectron(int icontainer);
    bool isWMuon(int icontainer);
    bool isWElectron(int icontainer);
    bool isBaselineJet(int icontainer,float ptcut_gev,float etacut,bool forAnalysis);
    bool isBadJet(int icontainer);
    // Trigger matching function
    bool isTriggerMatchedElectron(const xAOD::Electron *ele);
    bool isTriggerMatchedMuon(const xAOD::Muon *mu);
    // Define object-level cut flows
    TH1F *passBaselineMuon;     //!
    TH1F *passBaselineElectron; //!
    TH1F *passZMuon;     //!
    TH1F *passZElectron; //!
    TH1F *passWMuon;     //!
    TH1F *passWElectron; //!
    TH1F *passBaselineJet; //!
    void setupObjHistos();

    // define overlap removal tool
    ORUtils::ORToolBox m_orToolbox; //!
    ORUtils::ORToolBox m_orToolbox_jetCleaning; //!
    ORUtils::OverlapRemovalTool *m_orTool; //!
    ORUtils::OverlapRemovalTool *m_orToolJetCleaning; //!
    ORUtils::EleMuSharedTrkOverlapTool *m_orTool_ll; //!
    bool m_doTauOR    = false;
    bool m_doPhotonOR = false;
    // custom jet-lepton overlap functions
    bool passJetElectronOR(int icontainer);
    bool passJetMuonOR(int icontainer);
    // trigger matching tools
    //Trig::TrigEgammaMatchingTool* m_EgammaMatchTool; //!
    //Trig::TrigMuonMatching* m_MuonMatchTool; //!

    // define overlap removal tool
    ORUtils::ORToolBox m_orToolboxPopulateMuOrDecision; //!
    ORUtils::OverlapRemovalTool *m_orToolPopulateMuOrDecision; //!

    // Tool Handles
    CP::JetJvtEfficiency* m_jetjvf_cut_and_sf; //!
    ToolHandle<IJetSelector> m_jetCleaningTool; //!
    //ToolHandle<Trig::ITrigEgammaMatchingTool> m_EgammaMatchTool; //!
    //ToolHandle<Trig::ITrigMuonMatching> m_MuonMatchTool; //!

    ToolHandle<CP::IIsolationCorrectionTool> m_isoCorrTool; //!
    ToolHandle<CP::IEgammaCalibrationAndSmearingTool> m_egammaCalibTool; //!
    ToolHandle<CP::IMuonCalibrationAndSmearingTool> m_muonCalibrationAndSmearingTool; //!
    ToolHandle<CP::IMuonEfficiencyScaleFactors> m_muonSFTool; //!

    // Get lepton funcitons to avoid SUSYTools
    StatusCode GetElectrons(xAOD::ElectronContainer*& ele, xAOD::ShallowAuxContainer*& eleAux, const std::string& elekey = "Electrons");
    StatusCode GetMuons(xAOD::MuonContainer*& mu, xAOD::ShallowAuxContainer*& muAux, const std::string& mukey = "Muons");

    // this is needed to distribute the algorithm to the workers
    ClassDef(XSUSYCutflow, 1);
  };
  
}
#endif // pennSoftLepton_XSUSYCutflow_H

//#endif // BEFORE_SUSYTOOLS_000709
#endif // SKIP_XAOD_COMPILATION
