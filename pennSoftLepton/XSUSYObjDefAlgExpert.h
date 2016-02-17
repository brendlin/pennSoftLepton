#include "pennSoftLepton/switch_d3pd_xaod.h"
#ifndef SKIP_XAOD_COMPILATION
#ifdef BEFORE_SUSYTOOLS_000709

#ifndef pennSoftLepton_XSUSYObjDefAlgExpert_H
#define pennSoftLepton_XSUSYObjDefAlgExpert_H

#include "pennSoftLepton/XSUSYObjDefAlg.h"

//#include "CPAnalysisExamples/errorcheck.h"
//#include <TError.h>

namespace PSL
{
  class XSUSYObjDefAlgExpert : public XSUSYObjDefAlg
  {
    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
  public:

    //* \brief turn on or off harmonization */
    bool do_harmonization;
    //* \brief object passes baseline even if it fails OR */
    bool do_or_after_baselinesel;
    //* \brief use SUSYTools signal muon/electron functions */
    bool do_susytools_signallep;
    //* \brief overlap remove with bjets */
    bool do_bjet_overlap;

    TH2F* m_leptonMultiplicityOR; //!
    TH2F* m_jetMultiplicityOR; //!

    TH2F* m_leptonMultiplicityOR_count; //!
    TH2F* m_jetMultiplicityOR_count; //!

  public:

    // this is a standard constructor
    XSUSYObjDefAlgExpert();
    //
    bool init(void);
    void loop(void);

    void ApplyLeptonVeto(void);

    void PrintConfiguration(void);
    void CountOR(void);


#ifndef __MAKECINT__
    void OverlapRemoval(const xAOD::ElectronContainer *electrons, const xAOD::MuonContainer *muons, const xAOD::JetContainer *jets,const bool doHarmonization = true,const double dRejet = 0.2, const double dRjetmu = 0.4, const double dRjete = 0.4, const double dRemu = 0.01, const double dRee = 0.05){
      LocalOverlapRemoval(electrons,muons,jets,doHarmonization,dRejet,dRjetmu,dRjete,dRemu,dRee);
    };
    
    void LocalOverlapRemoval(const xAOD::ElectronContainer *electrons, const xAOD::MuonContainer *muons, const xAOD::JetContainer *jets,const bool doHarmonization = true,const double dRejet = 0.2, const double dRjetmu = 0.4, const double dRjete = 0.4, const double dRemu = 0.01, const double dRee = 0.05);

#endif // not cint

    // this is needed to distribute the algorithm to the workers
    ClassDef(XSUSYObjDefAlgExpert, 1);
  };
  
}
#endif // pennSoftLepton_XSUSYObjDefAlgExpert_H

#endif // BEFORE_SUSYTOOLS_000709
#endif // SKIP_XAOD_COMPILATION
