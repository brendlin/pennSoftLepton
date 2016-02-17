#include "pennSoftLepton/SlimmerD3PD.h"
#ifndef SKIP_D3PD_COMPILATION

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::SlimmerD3PD)
  
PSL::SlimmerD3PD::SlimmerD3PD() : m_EDM(0)
  ,tree(0)
  ,treename("physics")
{
  MSG_DEBUG("Constructor");
  SetName("SLIMMER_D3PD");
}

EL::StatusCode PSL::SlimmerD3PD::initialize(void){
  m_EDM = (EDM*)wk()->getAlg("PSL_EDM_CLASS");
  if (!m_EDM) {
    MSG_INFO("Error! Could not connect to EDM class! Exiting.");
    return EL::StatusCode::FAILURE;
  }

  TFile *file = wk()->getOutputFile("SKIM");
  tree = new TTree(treename.c_str(),treename.c_str());
  tree->SetDirectory(file);

  tree->Branch("EventNumber"      ,&(m_EDM->EventNumber      ),"EventNumber/i"      );
  tree->Branch("RunNumber"        ,&(m_EDM->RunNumber        ),"RunNumber/i"        );
  tree->Branch("mc_channel_number",&(m_EDM->mc_channel_number),"mc_channel_number/i");
  tree->Branch("larError"         ,&(m_EDM->larError         ),"larError/i"         );
  tree->Branch("tileError"        ,&(m_EDM->tileError        ),"tileError/i"        );
  tree->Branch("lbn"              ,&(m_EDM->lbn              ),"lbn/i"              );
  // Add N_UInt_t

  tree->Branch("EF_xe80_tclcw",&(m_EDM->EF_xe80_tclcw),"EF_xe80_tclcw/O");
  // Add N_Bool_t

  tree->Branch("MET_RefFinal_et"                 ,&(m_EDM->MET_RefFinal_et                 ),"MET_RefFinal_et/F"                 );
  tree->Branch("MET_RefFinal_etx"                ,&(m_EDM->MET_RefFinal_etx                ),"MET_RefFinal_etx/F"                );
  tree->Branch("MET_RefFinal_ety"                ,&(m_EDM->MET_RefFinal_ety                ),"MET_RefFinal_ety/F"                );
  tree->Branch("MET_RefFinal_phi"                ,&(m_EDM->MET_RefFinal_phi                ),"MET_RefFinal_phi/F"                );
  tree->Branch("MET_RefFinal_sumet"              ,&(m_EDM->MET_RefFinal_sumet              ),"MET_RefFinal_sumet/F"              );
  tree->Branch("actualIntPerXing"                ,&(m_EDM->actualIntPerXing                ),"actualIntPerXing/F"                );
  tree->Branch("averageIntPerXing"               ,&(m_EDM->averageIntPerXing               ),"averageIntPerXing/F"               );
  tree->Branch("Eventshape_rhoKt4LC"             ,&(m_EDM->Eventshape_rhoKt4LC             ),"Eventshape_rhoKt4LC/F"             );
  tree->Branch("MET_CellOut_Eflow_STVF_etx"      ,&(m_EDM->MET_CellOut_Eflow_STVF_etx      ),"MET_CellOut_Eflow_STVF_etx/F"      );
  tree->Branch("MET_CellOut_Eflow_STVF_ety"      ,&(m_EDM->MET_CellOut_Eflow_STVF_ety      ),"MET_CellOut_Eflow_STVF_ety/F"      );
  tree->Branch("MET_CellOut_Eflow_STVF_sumet"    ,&(m_EDM->MET_CellOut_Eflow_STVF_sumet    ),"MET_CellOut_Eflow_STVF_sumet/F"    );
  tree->Branch("MET_Egamma10NoTau_RefGamma_etx"  ,&(m_EDM->MET_Egamma10NoTau_RefGamma_etx  ),"MET_Egamma10NoTau_RefGamma_etx/F"  );
  tree->Branch("MET_Egamma10NoTau_RefGamma_ety"  ,&(m_EDM->MET_Egamma10NoTau_RefGamma_ety  ),"MET_Egamma10NoTau_RefGamma_ety/F"  );
  tree->Branch("MET_Egamma10NoTau_RefGamma_sumet",&(m_EDM->MET_Egamma10NoTau_RefGamma_sumet),"MET_Egamma10NoTau_RefGamma_sumet/F");
  tree->Branch("MET_Egamma10NoTau_CellOut_etx"   ,&(m_EDM->MET_Egamma10NoTau_CellOut_etx   ),"MET_Egamma10NoTau_CellOut_etx/F"   );
  tree->Branch("MET_Egamma10NoTau_CellOut_ety"   ,&(m_EDM->MET_Egamma10NoTau_CellOut_ety   ),"MET_Egamma10NoTau_CellOut_ety/F"   );
  tree->Branch("MET_Egamma10NoTau_CellOut_sumet" ,&(m_EDM->MET_Egamma10NoTau_CellOut_sumet ),"MET_Egamma10NoTau_CellOut_sumet/F" );
  tree->Branch("mc_event_weight",&(m_EDM->mc_event_weight  ),"mc_event_weight/F"  );
  // Add N_float

  tree->Branch("vxp_nTracks","vector<int>",m_EDM->Get(vxp_nTracks));
  

  el_block.block_n_name = el_n;
  el_met_block.block_n_name = el_MET_Egamma10NoTau_n;
  mu_block.block_n_name = mu_staco_n;
  tau_block.block_n_name = tau_n;
  jet_block.block_n_name = jet_AntiKt4LCTopo_n;
  
  for(unsigned int i=0;i<N_vector_float;++i){
    HandleVariable(static_cast<Var_vector_float>(i));
  }
  for(unsigned int i=0;i<N_vector_int;++i){
    if (static_cast<Var_vector_int>(i) == vxp_nTracks) continue;
    HandleVariable(static_cast<Var_vector_int>(i));
  }
  for(unsigned int i=0;i<N_vector_vector_UInt_t;++i){
    HandleVariable(static_cast<Var_vector_vector_UInt_t>(i));
  }
  for(unsigned int i=0;i<N_vector_vector_float;++i){
    HandleVariable(static_cast<Var_vector_vector_float>(i));
  }
  for(unsigned int i=0;i<N_vector_UInt_t;++i){
    HandleVariable(static_cast<Var_vector_UInt_t>(i));
  }

  el_block.SetBranches(tree);
  el_met_block.SetBranches(tree);
  mu_block.SetBranches(tree);
  tau_block.SetBranches(tree);
  jet_block.SetBranches(tree);

  //tree->Branch(,&(m_EDM->),);

  return EL::StatusCode::SUCCESS;
}

template <typename T> void PSL::SlimmerD3PD::HandleVariable(T variable){
  std::string vstr = ConvertD3PDVarToStr(variable);
  if (vstr.find("el_MET_Egamma10NoTau_") == 0) {
    MSG_VERBOSE("Adding " << vstr << " to el_MET_Egamma10NoTau block");
    el_met_block.AddVariable(variable);
  }else if(vstr.find("el_") == 0) {// means startswith
    MSG_VERBOSE("Adding " << vstr << " to el block");
    el_block.AddVariable(variable);
  } else if(vstr.find("mu_staco_") == 0) {
    MSG_VERBOSE("Adding " << vstr << " to mu block");
    mu_block.AddVariable(variable);
  }else if(vstr.find("jet_AntiKt4LCTopo_") == 0) {
    MSG_VERBOSE("Adding " << vstr << " to jet block");
    jet_block.AddVariable(variable);
  }else if(vstr.find("jet_AntiKt4LCTopo_") == 0) {
    MSG_VERBOSE("Adding " << vstr << " to jet block");
    jet_block.AddVariable(variable);
  }else if(vstr.find("tau_") == 0) {
    MSG_VERBOSE("Adding " << vstr << " to tau block");
    tau_block.AddVariable(variable);
  }else{
    MSG_INFO("Warning: Did not handle " << vstr << ".");
  }
}  

EL::StatusCode PSL::SlimmerD3PD::execute(void){
  MSG_DEBUG("Clearing blocks.");
  el_block.Clear();
  el_met_block.Clear();
  mu_block.Clear();
  tau_block.Clear();
  jet_block.Clear();
  MSG_DEBUG("Blocks cleared. Preparing fill.");
  el_block.PrepareFill(m_EDM);
  el_met_block.PrepareFill(m_EDM);
  mu_block.PrepareFill(m_EDM);
  tau_block.PrepareFill(m_EDM);
  jet_block.PrepareFill(m_EDM);
  MSG_DEBUG("Fill prepared. Filling.");
  tree->Fill();
  MSG_DEBUG("Fill complete.");
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::SlimmerD3PD::setupJob(EL::Job& job){
  EL::OutputStream out("SKIM");
  job.outputAdd(out);
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::SlimmerD3PD::histFinalize(){
  TFile *file = wk()->getOutputFile("SKIM");
  std::map<unsigned int, TH1F*>::iterator it;
  for (it = m_EDM->m_sample_stats.begin(); it != m_EDM->m_sample_stats.end(); it++)
  {
    TString mcchan;
    TH1F* hist = (TH1F*)it->second->Clone();
    hist->SetDirectory(file);
  }
  return EL::StatusCode::SUCCESS;
}

#endif // SKIP_D3PD_COMPILATION

