#include "pennSoftLepton/switch_d3pd_xaod.h"
#ifndef SKIP_XAOD_COMPILATION
#ifndef BEFORE_SUSYTOOLS_000709

#ifndef pennSoftLepton_XSUSYObjDefAlgV7_H
#define pennSoftLepton_XSUSYObjDefAlgV7_H

#include <pennSoftLepton/ObjectSelectionBase.h>
#include "pennSoftLepton/UtilCore.h"

namespace ST{
  class SUSYObjDef_xAOD;
}

//#include "CPAnalysisExamples/errorcheck.h"
//#include <TError.h>

namespace PSL
{
  class XSUSYObjDefAlgV7 : public ObjectSelectionBase
  {
    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
  public:
    //* \brief turn on or off anti-id */
    bool do_antiid;
    bool keep_overlapremoved_muons;
    
    //* \brief Some scale factors do not exist. So we have to fill in dummy info */
    bool m_trust_ele_SF; //!

    // variables that don't get filled at submission time should be
    // protected from being send from the submission node to the worker
    // node (done by the //!)
  public:

    /* // All of the below have to be initialized at the start of an event */
    EventDefinition m_evtdef; //!
    /* // All of the above have to be initialized at the start of an event */

    // this is a standard constructor
    XSUSYObjDefAlgV7();
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

    // could convince SUSYTools people to put these in their .h file....
#ifdef BEFORE_SUSYTOOLS_050022
    SG::AuxElement::Decorator<bool> dec_baseline; //!
    SG::AuxElement::Decorator<bool> dec_signal; //!
    SG::AuxElement::Decorator<bool> dec_passOR; //!
    SG::AuxElement::Decorator<bool> dec_badjet; //!
    SG::AuxElement::Decorator<bool> dec_bjet; //!
    SG::AuxElement::Decorator<bool> dec_cosmic; //!
#else
    SG::AuxElement::Decorator<char> dec_baseline; //!
    SG::AuxElement::Decorator<char> dec_signal; //!
    SG::AuxElement::Decorator<char> dec_passOR; //!
    SG::AuxElement::Decorator<char> dec_badjet; //!
    SG::AuxElement::Decorator<char> dec_bjet; //!
    SG::AuxElement::Decorator<char> dec_bad; //!
    SG::AuxElement::Decorator<char> dec_cosmic; //!
#endif // BEFORE_SUSYTOOLS_050022
    /* void SetXaodStuff(void); */

    void set_do_antiid             (bool b  ){do_antiid              = b;};
    void set_keep_overlapremoved_muons(bool b){keep_overlapremoved_muons = b;};

    virtual void ApplyLeptonVeto(void){};

    virtual void OverlapRemoval(const xAOD::ElectronContainer *electrons, const xAOD::MuonContainer *muons, const xAOD::JetContainer *jets,const bool doHarmonization = true,const double dRejet = 0.2, const double dRjetmu = 0.4, const double dRjete = 0.4, const double dRemu = 0.01, const double dRee = 0.05);

#endif // not cint

    // this is needed to distribute the algorithm to the workers
    ClassDef(XSUSYObjDefAlgV7, 1);
  };
  
}
#endif // pennSoftLepton_XSUSYObjDefAlgV7_H

#endif // BEFORE_SUSYTOOLS_000709
#endif // SKIP_XAOD_COMPILATION
