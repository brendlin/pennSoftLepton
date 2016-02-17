#ifndef pennSoftLepton_PassEvent_H
#define pennSoftLepton_PassEvent_H

#include <pennSoftLepton/AlgBase.h>
#include <pennSoftLepton/Sample.h>
#include <pennSoftLepton/UtilCore.h>
#include <pennSoftLepton/Variables.h>
#include <pennSoftLepton/CutItem.h>
//#include <pennSoftLepton/XSUSYObjDefAlgExpert.h>

#include <vector>

namespace PSL
{

  struct EventCounter{
    double PassN;
    double PassW;
    double Passw2;
  };

  class PassEvent : public AlgBase
  {
    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
  public:
    std::vector<std::string> Cuts;
    std::vector<PSL::Sample> Samples;
    std::vector<PSL::Variable> m_lepindices;
    std::string ParentEventSelectorName;

    void set_Cuts(std::string c); // puts them in a vector
    void set_ParentEventSelectorName(std::string s){ParentEventSelectorName = s;};

    std::vector<std::string> get_Cuts(void){return Cuts;};
    std::vector<std::string> get_Samples(void);
    std::vector<std::vector<double> > get_CountsByCutAndSample(std::string type); // N, err, W, Werr
    void SaveCountsByCutAndSampleToHist(void);
    
    // variables that don't get filled at submission time should be
    // protected from being send from the submission node to the worker
    // node (done by the //!)
  public:
    std::vector<PSL::CutItem*> CutItemPointers; //!

    // this is a standard constructor
    PassEvent();
    //
    virtual bool init(void);
    virtual void loop(void);
    virtual void finish(void);
    void ReconnectHistograms(TFile* file);
    void FillVariableHistograms(double weight);
    void FillLepVariableHistograms(double weight);

    void SetOutgoingNtupleBranches(TTree* tree);
    void PrintConfiguration(void);
    
  private:
    std::vector<std::vector<EventCounter> > m_CountsByCutAndSample; //!
    //std::vector<std::vector<std::vector<TH1*> > > m_VariablesByCutAndSample; //! // cut sample var
    std::vector<std::vector<TH1*> > m_VariablesBySample; //! // sample var
    std::vector<std::vector<std::vector<TH1*> > > m_LepVariablesBySample; //!

  public:
    // this is needed to distribute the algorithm to the workers
    ClassDef(PassEvent, 1);
  };
  
}
#endif
