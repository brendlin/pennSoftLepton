#include "pennSoftLepton/switch_d3pd_xaod.h"
#ifndef SKIP_XAOD_COMPILATION

#ifndef pennSoftLepton_TruthObjDef_H
#define pennSoftLepton_TruthObjDef_H

#include <pennSoftLepton/AlgBase.h>
#include "pennSoftLepton/UtilCore.h"

#include <sstream>

//#include "CPAnalysisExamples/errorcheck.h"
//#include <TError.h>

namespace PSL
{
  class TruthObjDef : public AlgBase
  {
    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
  public:
    //* \ptmin */
    double ele_truth_ptmin;
    double ele_truth_etamax;
    double mu_truth_ptmin; 
    double mu_truth_etamax;  
    double jet_truth_ptmin;  
    double jet_truth_etamax; 

    void set_ele_truth_ptmin   (double d){ele_truth_ptmin      = d;};
    void set_ele_truth_etamax  (double d){ele_truth_etamax     = d;};
    void set_mu_truth_ptmin    (double d){mu_truth_ptmin       = d;};
    void set_mu_truth_etamax   (double d){mu_truth_etamax      = d;};
    void set_jet_truth_ptmin   (double d){jet_truth_ptmin      = d;};
    void set_jet_truth_etamax  (double d){jet_truth_etamax     = d;};

  public:

#ifndef __MAKECINT__
    SG::AuxElement::Accessor<unsigned int> m_acc_particleType;
    SG::AuxElement::Accessor<unsigned int> m_acc_particleOrigin;
    SG::AuxElement::Accessor<unsigned int> m_acc_particleOutCome;
    SG::AuxElement::Accessor<int> m_acc_partonID;
    SG::AuxElement::Accessor<int> m_acc_motherID;
    SG::AuxElement::Accessor<float> m_acc_e_dressed;
    SG::AuxElement::Accessor<float> m_acc_pt_dressed;
    SG::AuxElement::Accessor<float> m_acc_eta_dressed;
    SG::AuxElement::Accessor<float> m_acc_phi_dressed;
    unsigned int ParticleType(const xAOD::IParticle& p){return m_acc_particleType(p);}
    unsigned int ParticleOrigin(const xAOD::IParticle& p){return m_acc_particleOrigin(p);}
    unsigned int ParticleOutCome(const xAOD::IParticle& p){return m_acc_particleOutCome(p);}
    int PartonID(const xAOD::IParticle& p){return m_acc_partonID(p);}
    int ParticleMother(const xAOD::IParticle& p){return m_acc_motherID(p);}
#endif // not cint

    TH1F *testHistogram; //!

    std::vector<int> *truthLepStatus; //!   
    std::vector<int> *truthLepBarcode; //!  
    std::vector<unsigned int> *truthLepOrigin; //!   
    std::vector<unsigned int> *truthLepType; //!
    std::vector<unsigned int> *truthLepOutCome; //!     
    std::vector<int> *truthLepMotherID; //!

    std::vector<int> *truthTauStatus; //!    
    std::vector<int> *truthTauBarcode; //!   
    std::vector<unsigned int> *truthTauOrigin; //!    
    std::vector<unsigned int> *truthTauType; //!
    std::vector<unsigned int> *truthTauOutCome; //!      
    std::vector<int> *truthTauMotherID; //!

    std::vector<int> *truthJetPartonID; //!

    std::vector<float> *truthNeutrinoPt; //!
    std::vector<float> *truthNeutrinoEta; //!
    std::vector<float> *truthNeutrinoPhi; //!
    std::vector<float> *truthNeutrinoE; //!

    std::vector<int> *truthPhoStatus; //!    
    std::vector<int> *truthPhoBarcode; //!   
    std::vector<unsigned int> *truthPhoOrigin; //!    
    std::vector<unsigned int> *truthPhoType; //!
    std::vector<unsigned int> *truthPhoOutCome; //!      
    std::vector<int> *truthPhoMotherID; //!

    /* // All of the below have to be initialized at the start of an event */
    EventDefinition m_evtdef; //!
    //xAOD::TEvent *m_event; //!
    /* // All of the above have to be initialized at the start of an event */

    // this is a standard constructor
    TruthObjDef();
    //
    bool init(void);
    void loop(void);
    void finish(void);

    void SetOutgoingNtupleBranches(TTree* tree);
    void PrintConfiguration(void);

    // this is needed to distribute the algorithm to the workers
    ClassDef(TruthObjDef, 1);
  };
  
}
#endif // pennSoftLepton_XSUSYObjDefAlg_H

#endif // SKIP_XAOD_COMPILATION
