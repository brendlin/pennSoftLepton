#include "pennSoftLepton/switch_d3pd_xaod.h"
#ifndef SKIP_D3PD_COMPILATION

#ifndef pennSoftLepton_SlimmerD3PD_H
#define pennSoftLepton_SlimmerD3PD_H

#include "pennSoftLepton/Messaging.h"
#include <pennSoftLepton/D3PDVariables.h>
#include "pennSoftLepton/EDM.h"

#include <EventLoop/Algorithm.h>
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <EventLoop/OutputStream.h>

namespace PSL
{

  using namespace D3PD;

  struct ThinBlock{

  ThinBlock() : original_indices_tosave(0)
    {
      
    }

    Var_Int block_n_name;
    int block_n_address;
    std::vector<int>* original_indices_tosave;

    std::vector<Var_vector_float        > vector_floats       ;
    std::vector<Var_vector_int          > vector_ints         ;
    std::vector<Var_vector_vector_UInt_t> vector_vector_UInts ;
    std::vector<Var_vector_vector_float > vector_vector_floats;
    std::vector<Var_vector_UInt_t       > vector_UInts        ;

    // same as in D3PDWrapper
    std::vector< std::vector<float> >                       m_v_float   ; //!
    std::vector< std::vector<int> >                         m_v_int     ; //!
    std::vector< std::vector< std::vector<unsigned int> > > m_v_v_uint  ; //!
    std::vector< std::vector< std::vector<float> > >        m_v_v_float ; //!
    std::vector< std::vector<unsigned int> >                m_v_uint    ; //!

    void SetBranches(TTree* tree){
      std::string nm = ConvertD3PDVarToStr(block_n_name);
      std::string nm_i = nm+"/I";
      std::string nm_indices = nm+"_d3pd_index";
      tree->Branch(nm.c_str(),&block_n_address,nm_i.c_str()); // el_n
      tree->Branch(nm_indices.c_str(),"vector<int>",&original_indices_tosave);
      
      for (unsigned int i=0;i<vector_floats.size();++i){
        m_v_float.push_back(std::vector<float>());
      }
      for (unsigned int i=0;i<vector_floats.size();++i){
        tree->Branch(ConvertD3PDVarToStr(vector_floats[i]).c_str(),"vector<float>",&(m_v_float[i]));
      }
      for (unsigned int i=0;i<vector_ints.size();++i){
        m_v_int.push_back(std::vector<int>());
      }
      for (unsigned int i=0;i<vector_ints.size();++i){
        tree->Branch(ConvertD3PDVarToStr(vector_ints[i]).c_str(),"vector<int>",&(m_v_int[i]));
      }
      for (unsigned int i=0;i<vector_vector_UInts.size();++i){
        m_v_v_uint.push_back(std::vector<std::vector<unsigned int> >());
      }
      for (unsigned int i=0;i<vector_vector_UInts.size();++i){
        tree->Branch(ConvertD3PDVarToStr(vector_vector_UInts[i]).c_str(),"vector<vector<unsigned int> >",&(m_v_v_uint[i]));
      }
      for (unsigned int i=0;i<vector_vector_floats.size();++i){
        m_v_v_float.push_back(std::vector<std::vector<float> >());
      }
      for (unsigned int i=0;i<vector_vector_floats.size();++i){
        tree->Branch(ConvertD3PDVarToStr(vector_vector_floats[i]).c_str(),"vector<vector<float> >",&(m_v_v_float[i]));
      }
      for (unsigned int i=0;i<vector_UInts.size();++i){
        m_v_uint.push_back(std::vector<unsigned int>());
      }
      for (unsigned int i=0;i<vector_UInts.size();++i){
        tree->Branch(ConvertD3PDVarToStr(vector_UInts[i]).c_str(),"vector<unsigned int>",&(m_v_uint[i]));
      }
    }

    void Clear(void){
      original_indices_tosave->clear();
      for (unsigned int i=0;i<vector_floats       .size();++i){ m_v_float  [i].clear(); }
      for (unsigned int i=0;i<vector_ints         .size();++i){ m_v_int    [i].clear(); }
      for (unsigned int i=0;i<vector_vector_UInts .size();++i){ m_v_v_uint [i].clear(); }
      for (unsigned int i=0;i<vector_vector_floats.size();++i){ m_v_v_float[i].clear(); }
      for (unsigned int i=0;i<vector_UInts        .size();++i){ m_v_uint   [i].clear(); }
    }

    // Make a template?
    void PrepareFill(D3PDWrapper* edm){
      //for (int i=0;i<edm->Get(block_n_name);++i){
      //block_n_address = edm->Get(block_n_name);
      std::vector<int> tmp = edm->GetThinIndices(block_n_name);
      block_n_address = tmp.size();
      for (unsigned int ii=0;ii<tmp.size();++ii){
        int i = tmp[ii];
        original_indices_tosave->push_back(i);
        for (unsigned int j=0;j<vector_floats.size();++j){
          m_v_float[j].push_back(edm->Get(vector_floats[j])->at(i));
        }
        for (unsigned int j=0;j<vector_ints.size();++j){
          m_v_int[j].push_back(edm->Get(vector_ints[j])->at(i));
        }
        for (unsigned int j=0;j<vector_vector_UInts.size();++j){
          m_v_v_uint[j].push_back(edm->Get(vector_vector_UInts[j])->at(i));
        }
        for (unsigned int j=0;j<vector_vector_floats.size();++j){
          m_v_v_float[j].push_back(edm->Get(vector_vector_floats[j])->at(i));
        }
        for (unsigned int j=0;j<vector_UInts.size();++j){
          m_v_uint[j].push_back(edm->Get(vector_UInts[j])->at(i));
        }
      }
    }

    void AddVariable(Var_vector_float         v){vector_floats       .push_back(v);};
    void AddVariable(Var_vector_int           v){vector_ints         .push_back(v);};
    void AddVariable(Var_vector_vector_UInt_t v){vector_vector_UInts .push_back(v);};
    void AddVariable(Var_vector_vector_float  v){vector_vector_floats.push_back(v);};
    void AddVariable(Var_vector_UInt_t        v){vector_UInts        .push_back(v);};
  };
  /////////

  class SlimmerD3PD : public EL::Algorithm
  {
  public:
    // this is a standard constructor
    SlimmerD3PD();
    ~SlimmerD3PD(){};
    //

    EDM* m_EDM; //!
    TTree* tree; //!
    std::string treename;

    ThinBlock el_block; //!
    ThinBlock el_met_block; //!
    ThinBlock mu_block; //!
    ThinBlock tau_block; //!
    ThinBlock jet_block; //!

    void Set_treename(std::string s){treename=s;};

    template <typename T> void HandleVariable(T variable);

    virtual EL::StatusCode setupJob (EL::Job& job); // used
    virtual EL::StatusCode fileExecute (){return EL::StatusCode::SUCCESS;};
    virtual EL::StatusCode histInitialize (){return EL::StatusCode::SUCCESS;};
    virtual EL::StatusCode changeInput (bool firstFile){return EL::StatusCode::SUCCESS;};
    virtual EL::StatusCode initialize (); // used
    virtual EL::StatusCode execute (); // used
    virtual EL::StatusCode postExecute (){return EL::StatusCode::SUCCESS;};
    virtual EL::StatusCode finalize (){return EL::StatusCode::SUCCESS;}; // calls finish()
    virtual EL::StatusCode histFinalize ();//{return EL::StatusCode::SUCCESS;};

    // this is needed to distribute the algorithm to the workers
    ClassDef(SlimmerD3PD, 1);
  }; // class SlimmerD3PD
} // namespace PSL

#endif // pennSoftLepton_SlimmerD3PD_H

#endif // SKIP_D3PD_COMPILATION
