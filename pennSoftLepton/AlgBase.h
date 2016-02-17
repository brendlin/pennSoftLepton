#ifndef pennSoftLepton_AlgBase_H
#define pennSoftLepton_AlgBase_H

#include <EventLoop/Algorithm.h>
#include "pennSoftLepton/Messaging.h"
#include "pennSoftLepton/EDM.h"
#include "pennSoftLepton/Sample.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

namespace PSL
{
  
  class AlgBase : public EL::Algorithm
  {
    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
  public:
    // float cutValue;
    std::string configured_name;
    // variables that don't get filled at submission time should be
    // protected from being send from the submission node to the worker
    // node (done by the //!)
  public:
    // Tree *myTree; //!
    // TH1 *myHist; //!
    
    EDM* m_EDM; //!
    
    // this is a standard constructor
    AlgBase();
    ~AlgBase(){};
    
    // configuration
    virtual void SetName(const char * nm){
      configured_name = nm; 
      TNamed::SetName(nm);
      //MSG_INFO("Setting name to " << configured_name << " (TObject: " << GetName() << ").");
    };

    //* Unfortunate way of holding onto histograms if needed (necessitated by EventLoop) */
    std::vector<TH1F*> tmp_hists; //!
    
    // these are the functions inherited from Algorithm
    virtual EL::StatusCode setupJob (EL::Job& job);
    virtual EL::StatusCode fileExecute ();
    virtual EL::StatusCode histInitialize (); // calls init()
    virtual EL::StatusCode changeInput (bool firstFile);
    virtual EL::StatusCode initialize ();
    virtual EL::StatusCode execute (); // calls loop()
    virtual EL::StatusCode postExecute ();
    virtual EL::StatusCode finalize ();
    virtual EL::StatusCode histFinalize (); // calls finish()
    
    void setHistogramOutput(TH1* hist);
    void SaveHistogram(TH1* hist,TFile* file);
    void SaveIfNotEmpty(TH1* hist,bool saveanyway=false);
    EL::Algorithm* connectToAnotherAlgorithm(std::string name);

    void MakeVariableTH1FVector(std::vector<std::vector<TH1*> >& obj,std::string classname,std::string instname,std::vector<PSL::Sample> samples,std::vector<PSL::Variable> variables);
    void MakeLepVariableTH1FVector(std::vector<std::vector<TH1*> >& obj,std::string classname,std::string instname,std::vector<PSL::Sample> samples);
    void MakeVectorOfLepVariableTH1FVectors(std::vector<std::vector<std::vector<TH1*> > >& obj,std::string classname,std::string instname,std::vector<PSL::Sample> samples,std::vector<PSL::LepVariable> lepvariables,std::vector<PSL::Variable> lepindices);

    // these are methods meant to put distance between the analysis code and EventLoop
    // They are virtual because they need to be specified in the inheriting alg
    virtual bool init(void){return true;}; // called from histInitialize. Do setHistogramOutput here.
    virtual void loop(void){return;}; // called from Execute
    virtual void finish(void){return;}; // called from finalize. 
    virtual void ReconnectHistograms(TFile* file){MSG_VERBOSE("Verbosity for " << file->GetName()); return;};

    // Lots of algorithms use vectors of variables to indicate what to save. (Ntupler, PassEvent, etc)
    // variables to save
    std::vector<Variable> m_variables; // configurable
    std::vector<LepVariable> m_lepvariables; // configurable
    std::vector<JetVariable> m_jetvariables; // configurable
    std::vector<PSL::Variable> m_lepindices; // These are the lepton indices you want to save as hists
    std::vector<PSL::Sample> Samples; // configurable

    void set_Variables(std::string v); 
    void set_SkipVariables(std::string v);
    void set_LepVariables(std::string v);
    void set_Samples(std::string s); // converts to sample and puts in a vector

    // This was moved from PassEvent so that other types of
    // Algs could aid in event selection
    AlgBase* ParentEventSelector; //!
    TH2F* hist_PassN; //!
    TH2F* hist_PassW; //!
    unsigned long long int m_currenteventnumber; //! (this is going to be the event number. Hacky thing....)
    int m_eventpassed; //! This will be connected to a tree!
    virtual bool passed(unsigned long long int i){return (i == m_currenteventnumber && m_eventpassed);};
    virtual void SetOutgoingNtupleBranches(TTree* tree){(void)tree; return;};
    
    std::string GetHistogramLocation(std::string classname,std::string instname,std::string name);
    std::string GetHistogramLocation(std::string classname,std::string instname,Sample s,Variable v);
    std::string GetHistogramLocation(std::string classname,std::string instname,Sample s,std::string name);
    std::string GetHistogramLocation(std::string classname,std::string instname,Sample s,LepVariable v,Variable lep_index);
    std::string GetHistogramLocation(std::string classname,std::string instname,Sample s,LepVariable v);
    
    // this is needed to distribute the algorithm to the workers
    ClassDef(AlgBase, 1);
  };
}
#endif
