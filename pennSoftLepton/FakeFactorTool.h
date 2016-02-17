#ifndef pennSoftLepton_FakeFactorTool_H
#define pennSoftLepton_FakeFactorTool_H

#include <pennSoftLepton/AlgBase.h>
#include <pennSoftLepton/ObjectSelectionBase.h>
#include "TFile.h"

//////////////////
// CALCULATE fake efficiencies
// - Require at least 1 loose lepton (signal selection too)
// - Define disjoint denominator
// - Define disjoint numerator
// - Fill numerator and disjoint denominator
//
//////////////////

namespace PSL
{
  
  class FakeFactorTool : public ObjectSelectionBase
  {
  public:

    std::string ParentEventSelectorName;

    void set_ParentEventSelectorName(std::string s){ParentEventSelectorName = s;};

    std::map<unsigned int,float> m_prescales;

    std::string PrescalesFile;
    std::string SavedFakeFactorFile;
    float MtCut;
    float MzCutLow;
    float MzCutHigh;

    std::string Region; // dijet, zjet, wjet
    bool do_dijet;
    bool do_zjet;

    bool do_ttt;
    bool do_ltt;
    bool do_tlt;
    bool do_ttl;
    bool do_SR;
    bool do_wjet;
    bool do_ttbar;  

    bool do_wplus;
    bool do_wminus;

    double m_ZWindow;
    bool m_mcclosure;

    // the "Z Desert" is the TLT, TTL uee and euu channels
    // We want to include these in validation plots and
    // in certain ttbar cases.
    bool m_includeZdesert;

    int m_nvariation; // variations are numbered.

    TH1F* m_saved_muFakeFactor; //!
    TH1F* m_saved_eleFakeFactor; //!

    AlgBase* ParentEventSelector; //!

    EventDefinition m_evtdef; //!

    std::vector<std::vector<TH1*> > m_VariablesBySample_MuNum; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_MuDen; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_EleNum; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_EleDen; //!

    std::vector<std::vector<TH1*> > m_VariablesBySample_allNum; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_allDen; //!

    std::vector<std::vector<TH1*> > m_VariablesBySample_eeeNum; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_eeeDen; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_euuNum; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_euuDen; //!

    std::vector<std::vector<TH1*> > m_VariablesBySample_ueeNum; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_ueeDen; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_uuuNum; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_uuuDen; //!

    std::vector<std::vector<TH1*> > m_VariablesBySample_uuNum; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_uuDen; //!

    std::vector<std::vector<TH1*> > m_VariablesBySample_ueNum; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_ueDen; //!

    std::vector<std::vector<TH1*> > m_VariablesBySample_euNum; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_euDen; //!

    std::vector<std::vector<TH1*> > m_VariablesBySample_ueuoNum; //!
    std::vector<std::vector<TH1*> > m_VariablesBySample_ueuoDen; //!

    std::vector<PSL::Variable> m_lepindices; // These are the lepton indices you want to save as hists
    void set_LepIndices(std::string v);

    // samples, lepvariables, lepindex
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_MuNum; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_MuDen; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_EleNum; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_EleDen; //!

    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_allNum; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_allDen; //!

    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_eeeNum; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_eeeDen; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_euuNum; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_euuDen; //!

    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_ueeNum; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_ueeDen; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_uuuNum; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_uuuDen; //!

    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_uuNum; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_uuDen; //!

    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_ueNum; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_ueDen; //!

    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_euNum; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_euDen; //!

    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_ueuoNum; //!
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample_ueuoDen; //!

    void FillStuff(double weight,std::vector<std::vector<TH1*> >& h_all_variables
                   ,std::vector<std::vector<std::vector<TH1*> > >& h_all_lepvariables);

    void set_PrescalesFile         (std::string s){PrescalesFile = s;};
    void set_SavedFakeFactorFile   (std::string s){SavedFakeFactorFile = s;};
    // dijet region cuts
    void set_MtCut                 (double d){MtCut     = d;};
    void set_MzCutLow              (double d){MzCutLow  = d;};
    void set_MzCutHigh             (double d){MzCutHigh = d;};
    //
    void set_SubRegion(std::string s){
      if (s == "ttt")     { do_ttt = true ; do_ltt = false; do_tlt = false; do_ttl = false; }
      if (s == "ltt")     { do_ttt = false; do_ltt = true ; do_tlt = false; do_ttl = false; }
      if (s == "tlt")     { do_ttt = false; do_ltt = false; do_tlt = true ; do_ttl = false; }
      if (s == "ttl")     { do_ttt = false; do_ltt = false; do_tlt = false; do_ttl = true ; }
    }
    void set_Region(std::string s){
      Region = s;
      if (Region == "dijet") { do_dijet = true ; do_zjet = false; do_ttbar = false; do_wjet = false;}
      if (Region == "zjet")  { do_dijet = false; do_zjet = true ; do_ttbar = false; do_wjet = false;}
      if (Region == "ttbar") { do_dijet = false; do_zjet = false; do_ttbar = true ; do_wjet = false;}
      if (Region == "wjet")  { do_dijet = false; do_zjet = false; do_ttbar = false; do_wjet = true ;}
    }
    void set_doSR(bool b){ do_SR = b; };
    void set_wplus(bool b){ do_wplus = b; };
    void set_wminus(bool b){ do_wminus = b; };
    void set_ZWindow(double d){m_ZWindow = d;};
    void set_mcclosure(bool b) {m_mcclosure = b;};

    void set_nvariation(int i){m_nvariation = i;};

/*     void set_doZee(bool b){ do_zee = b;}; */
/*     void set_doZmm(bool b){ do_zmm = b;}; */

    void loopDijetRegion(void);
    void loopZjetRegion(void);
    void loopWjetRegion(void);
    void loopTtbarRegion(void);

    FakeFactorTool();
    bool init(void);
    void loop(void);
    void finish(void);
    void PrintConfiguration(void);

    void ReconnectHistograms(TFile* file);
    void SetOutgoingNtupleBranches(TTree* tree);

    double TtbarMultiplier(void);

    int m_eventpassed_EleNum; //! This will be connected to a tree!
    int m_eventpassed_EleDen; //! This will be connected to a tree!
    int m_eventpassed_MuNum; //! This will be connected to a tree!
    int m_eventpassed_MuDen; //! This will be connected to a tree!
    
  public:
    ClassDef(FakeFactorTool,1);
  };

}

#endif // pennSoftLepton_FakeFactorTool_H
