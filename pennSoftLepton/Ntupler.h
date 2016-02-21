#ifndef pennSoftLepton_Ntupler_H
#define pennSoftLepton_Ntupler_H

#include <pennSoftLepton/AlgBase.h>
#include <pennSoftLepton/PassEvent.h>

#include <EventLoop/Algorithm.h>
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <EventLoop/OutputStream.h>

namespace PSL
{

  class Ntupler : public AlgBase
  {
  public:
    // this is a standard constructor
    Ntupler();
    ~Ntupler(){};
    //

    TTree* tree; //!
    std::string treename;
    AlgBase* ParentEventSelector; //!
    std::string ParentEventSelectorName;    
    /* Ntupler has the privilege of knowing the names of all the other algorithms*/
    /* So it can ask those algorithms whether they want to save a branch of their choosing. */
    std::vector<std::string> all_algs;

    void set_ParentEventSelectorName(std::string s){ParentEventSelectorName = s;};
    void set_SkipIsoVariables(std::string v);
    void set_DoTriggers(bool b){m_DoTriggers = b;};
    void set_DoLeptonID(bool b){m_DoLeptonID = b;};

    // additional variables to save
    std::vector<Trigger2015> m_triggers; //!
    bool m_DoTriggers;
    std::vector<ElectronID::ElectronID> m_lep_id; //!
    std::vector<xAOD::Muon::Quality> m_lep_muon_id; //!
    std::vector<AnaIso::AnaIso> m_lep_iso;
    bool m_DoLeptonID;

    unsigned long long int m_eventnumber; //!
    Float_t* fATreeEvent; //! event variables (all floats now)
    Int_t* fATreeEvent_int; //! event variables (some need to be ints)
    std::vector< std::vector<float> > m_lep_vector_floats; //!
    std::vector< std::vector<int> > m_lep_vector_ints; //!

    std::vector< std::vector<float> > m_jet_vector_floats; //!

    void Set_treename(std::string s){treename=s;};

    virtual EL::StatusCode setupJob (EL::Job& job);
    bool init(void);
    void loop(void);
    void finish(void);

    void PrintConfiguration(void);

    void GetAlgList(EL::Job job);

    // this is needed to distribute the algorithm to the workers
    ClassDef(Ntupler, 1);
  }; // class Ntupler
} // namespace PSL

#endif // pennSoftLepton_Ntupler_H
