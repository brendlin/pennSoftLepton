#include <pennSoftLepton/D3PDWrapper.h>
#ifndef SKIP_D3PD_COMPILATION

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::D3PDWrapper)

void PSL::D3PDWrapper::SetEventVariable(PSL::Variable v,double d){
  m_EventVariables[v] = d;
}
double PSL::D3PDWrapper::GetEventVariable(PSL::Variable v){
  if (m_EventVariables.count(v)) return m_EventVariables[v];
  if (!m_evtdef) {MSG_INFO("Error: m_evtdef not set."); return 0;}
  double d = -999;
  // begin all of the variable definitions
  if (v == vGRL       ) d = PassGRL();
  else if (v == vpassLarError  ) d = (int)(larError != 2);
  else if (v == vpassTileError ) d = (int)(tileError != 2);
  else if (v == vpassCoreBit ) d = 1;
  else if (v == vpassPrimaryVtx) d = PassPrimaryVtx();
  else if (v == vpassCosmicMuon) d = PassCosmicMuon();
  else if (v == vpassTileTripReader   ) d = 1;
  else if (v == vpassIncompleteEvents ) d = 1;
  else if (v == vpassTileCalHotSpot   ) d = 1;
  else if (v == vpassBadJet           ) d = (m_evtdef->PassBadJet() ? 1. : 0.);
  else if (v == vpassCaloJet          ) d = (PassCaloJetCut() ? 1. : 0.);
  else if (v == vpassBadMuon          ) d = 1;
  else if (v == vpassHFOR             ) d = 1;
  else if (v == vpassMCOverlap        ) d = 1;
  else if (v == vpassBCH              ) d = (PassBCH() ? 1. : 0.);
  else if (v == vMuIsValid            ) d = 1; // unless it is set upstream...
  else if (v == vnBaselineLeptons     ) d = m_evtdef->NBaselineLeptons();
  else if (v == vnSignalLeptons       ) d = m_evtdef->leps.size();
  else if (v == vpass_xe80_tclcw      ) d = (EF_xe80_tclcw ? 1. : 0.);
  else if (v == vChanFlavor      ) d = m_evtdef->ChanFlavor();
  else if (v == vSameSign         ) d = m_evtdef->IsSameSign();
  else if (v == vNSignalTaus      ) d = m_evtdef->tau.i_sig.size();
  else if (v == vmet_Et         ) d = m_evtdef->met_Et()      /1000.;
  else if (v == vmet_phi        ) d = m_evtdef->met_phi()           ;
  else if (v == vmet_sumet      ) d = m_evtdef->met_sumet     /1000.;
  else if (v == vrefEle         ) d = m_evtdef->refEle()      /1000.;
  else if (v == vrefEle_etx     ) d = m_evtdef->refEle_etx()  /1000.;
  else if (v == vrefEle_ety     ) d = m_evtdef->refEle_ety()  /1000.;
  else if (v == vrefEle_sumet   ) d = m_evtdef->refEle_sumet  /1000.;
  else if (v == vrefMuo         ) d = m_evtdef->refMuo()      /1000.;
  else if (v == vrefMuo_etx     ) d = m_evtdef->refMuo_etx()  /1000.;
  else if (v == vrefMuo_ety     ) d = m_evtdef->refMuo_ety()  /1000.;
  else if (v == vrefMuo_sumet   ) d = m_evtdef->refMuo_sumet  /1000.;
  else if (v == vrefJet         ) d = m_evtdef->refJet()      /1000.;
  else if (v == vrefJet_etx     ) d = m_evtdef->refJet_etx()  /1000.;
  else if (v == vrefJet_ety     ) d = m_evtdef->refJet_ety()  /1000.;
  else if (v == vrefJet_sumet   ) d = m_evtdef->refJet_sumet  /1000.;
  else if (v == vrefGamma       ) d = m_evtdef->refGamma()    /1000.;
  else if (v == vrefGamma_etx   ) d = m_evtdef->refGamma_etx()/1000.;
  else if (v == vrefGamma_ety   ) d = m_evtdef->refGamma_ety()/1000.;
  else if (v == vrefGamma_sumet ) d = m_evtdef->refGamma_sumet/1000.;
  else if (v == vsoftTerm       ) d = m_evtdef->softTerm()    /1000.;
  else if (v == vsoftTerm_etx   ) d = m_evtdef->softTerm_etx()/1000.;
  else if (v == vsoftTerm_ety   ) d = m_evtdef->softTerm_ety()/1000.;
  else if (v == vsoftTerm_sumet ) d = m_evtdef->softTerm_sumet/1000.;
  else if (v == vSFOS           ) d = m_evtdef->IsSFOS();
  else if (v == vnBjets         ) d = m_evtdef->nBjets();
  else if (v == vlep0Pt             ) d = m_evtdef->LeadingLepton().Pt()/1000.;
  else if (v == vlep0Eta            ) d = m_evtdef->LeadingLepton().Eta();
  else if (v == vlep0Phi            ) d = m_evtdef->LeadingLepton().Phi();
  else if (v == vlep0D0Significance ) d = GetLeptonVariable(vlep0D0Significance,0);
  else if (v == vlep0Z0SinTheta     ) d = GetLeptonVariable(vlep0Z0SinTheta,0);
  else if (v == vlep0TrkIso         ) d = GetLeptonVariable(vlep0TrkIso,0);
  else if (v == vlep0CaloIso        ) d = GetLeptonVariable(vlep0CaloIso,0);
  else if (v == vlep1Pt             ) d = m_evtdef->SubleadingLepton().Pt()/1000.;
  else if (v == vlep1Eta            ) d = m_evtdef->SubleadingLepton().Eta();
  else if (v == vlep1Phi            ) d = m_evtdef->SubleadingLepton().Phi();
  else if (v == vlep1D0Significance ) d = GetLeptonVariable(vlep0D0Significance,1); // see syntax!
  else if (v == vlep1Z0SinTheta     ) d = GetLeptonVariable(vlep0Z0SinTheta,1); // see syntax!
  else if (v == vlep1TrkIso         ) d = GetLeptonVariable(vlep0TrkIso,1); // see syntax!
  else if (v == vlep1CaloIso        ) d = GetLeptonVariable(vlep0CaloIso,1); // see syntax!
  else if (v == vlep2Pt             ) d = m_evtdef->ThirdLepton().Pt()/1000.;
  else if (v == vlep2Eta            ) d = m_evtdef->ThirdLepton().Eta();
  else if (v == vlep2Phi            ) d = m_evtdef->ThirdLepton().Phi();
  else if (v == vlep2D0Significance ) d = GetLeptonVariable(vlep0D0Significance,2); // see syntax!
  else if (v == vlep2Z0SinTheta     ) d = GetLeptonVariable(vlep0Z0SinTheta,2); // see syntax!
  else if (v == vlep2TrkIso         ) d = GetLeptonVariable(vlep0TrkIso,2); // see syntax!
  else if (v == vlep2CaloIso        ) d = GetLeptonVariable(vlep0CaloIso,2); // see syntax!
  else if (v == vjet0Pt         ) d = m_evtdef->LeadingJet().Pt()/1000.; 
  else if (v == vjet0Eta        ) d = m_evtdef->LeadingJet().Eta();
  else if (v == vjet0Phi        ) d = m_evtdef->LeadingJet().Phi();
  else if (v == vjet1Pt         ) d = m_evtdef->SubleadingJet().Pt()/1000.;
  else if (v == vjet1Eta        ) d = m_evtdef->SubleadingJet().Eta();
  else if (v == vjet1Phi        ) d = m_evtdef->SubleadingJet().Phi();
  else if (v == vPtll           ) d = m_evtdef->Pair_LeadingLeptons().Pt()/1000.;
  else if (v == vMll            ) d = m_evtdef->Pair_SFOS().M()/1000.;
  else if (v == vMinMll         ) d = m_evtdef->Pair_minSFOS().M()/1000.;
  else if (v == vNJets          ) d = m_evtdef->nJets();
  else if (v == vNCentralJets   ) d = m_evtdef->nCentralJets();
  else if (v == vNForwardJets   ) d = m_evtdef->nForwardJets();
  else if (v == vDeltaRll       ) d = m_evtdef->DeltaRll();
  else if (v == vMEToverPtj0    ) d = GetEventVariable(vmet_Et)/GetEventVariable(vjet0Pt);
  else if (v == vPtlloverMET    ) d = GetEventVariable(vPtll)/GetEventVariable(vmet_Et);
  else if (v == vMinDeltaPhilMET) d = m_evtdef->MinDeltaPhiLep0Lep1_MET();
  else if (v == vMaxDeltaPhilJet) d = m_evtdef->MaxDeltaPhiLep0Lep1_Jet1();
  else if (v == vPassTrigger ) d = 0;
  else if (v == vMt             ) d = m_evtdef->Mt()/1000.;
  else if (v == vM3l            ) d = m_evtdef->Three_LeadingLeptons().M()/1000.;
  else if (v == vMll_Upsilon    ) d = m_evtdef->Pair_SFOS_Upsilon().M()/1000.;
  else if (v == vpassLooseLH      ) d = 0;
  else if (v == vpassMediumLH      ) d = 0;
  else if (v == vpassTightLH      ) d = 0;
  else if (v == vSample      ) d = m_CurrentSample;
  else if (v == vTrailingMuonPt      ) d = 0;
  else if (v == vTrailingElectronPt      ) d = 0;
  else if (v == vantilep0Flavor      ) d = 0;
  else if (v == vantilep0Pt      ) d = 0;
  else if (v == vantilep0Eta      ) d = 0;
  else if (v == vantilep0Phi      ) d = 0;
  else if (v == vantilep0D0Significance      ) d = 0;
  else if (v == vantilep0Z0SinTheta      ) d = 0;
  else if (v == vantilep0TrkIso      ) d = 0;
  else if (v == vantilep0CaloIso      ) d = 0;
  else if (v == vtruthjet0Pt      ) d = 0;
  else if (v == vnVetoLeptons      ) d = 0;
  else if (v == vTotalWeight      ) d = 0;
  else if (v == vTotalWeightNoPupNoSF      ) d = 0;
  else if (v == vmc_channel_number      ) d = 0;
  else if (v == vptAvg      ) d = 0;
  else if (v == vtruthptAvg      ) d = 0;
  else if (v == vptAvgOverTruthjet0Pt      ) d = 0;
  // Add_new_variables_here to GetEventVariable
  //else if (v == ) d = ;
  else MSG_INFO("Error! Variable not implemented in D3PDWrapper::GetEventVariable: " << ConvertVarToStr(v));
  // end all of the variable definitions
  m_EventVariables[v] = d;
  return d;
}

double PSL::D3PDWrapper::GetLeptonVariable(PSL::Variable v,int iptordered){
  if (!m_evtdef) return 999.;
  Particle p = m_evtdef->PtOrderedLepton(iptordered);
  MSG_DEBUG("Particle has momentum " << p.tlv.Pt() << " and index " << p.i_cont);
  if (!p.tlv.Pt()) return 999.;
  if (p.obj_type == ObjType::Muon) {
    if      (v == vlep0D0Significance) return MuonD0Significance(p.i_cont);
    else if (v == vlep0Z0SinTheta    ) return fabs(MuonZ0(p.i_cont)*sin(p.tlv.Theta()));
    else if (v == vlep0TrkIso        ) return MuonTrkIsolation(p.i_cont);
    else if (v == vlep0CaloIso       ) return MuonCaloIsolation(p.i_cont);
  }
  if (p.obj_type == ObjType::Electron) {
    if      (v == vlep0D0Significance) return EleD0Significance(p.i_cont);
    else if (v == vlep0Z0SinTheta    ) return fabs(EleZ0(p.i_cont)*sin(p.tlv.Theta()));
    else if (v == vlep0TrkIso        ) return EleTrkIsolation(p.i_cont);
    else if (v == vlep0CaloIso       ) return EleCaloIsolation(p.i_cont);
  }
  return 999.;
}

void PSL::D3PDWrapper::ConfigureRun(unsigned int runNumber,std::string samp,float xsec_k_f,float sumw){
  Sample s = ConvertToSample(samp);
  m_run_sample_map[runNumber] = s;
  m_run_xsec_map[runNumber] = xsec_k_f;
  m_run_sumw_map[runNumber] = sumw;
  return;
}

void PSL::D3PDWrapper::PrintRunConfiguration(void){
  for (std::map<unsigned int,PSL::Sample>::const_iterator it=m_run_sample_map.begin();
       it!=m_run_sample_map.end();++it){
    MSG_INFO("Run " << it->first << ": " << ConvertSampleToStr(it->second)
             << " xsec : " << m_run_xsec_map[it->first]
             << " sumw : " << m_run_sumw_map[it->first]);
  }
}

bool PSL::D3PDWrapper::hasBranch(TTree* tree,std::string br){
  // if (m_branchesCheckList.count(br)) {
  //   MSG_DEBUG("Branch " << br << " was already checked and is " << m_branchesCheckList[br]);
  //   return m_branchesCheckList[br];      
  // }
  //
  TObjArray* branches = tree->GetListOfBranches();
  MSG_DEBUG("Got object " << br << " " << branches->GetEntries());
  for (int i=0;i<branches->GetEntries();i++) {
    MSG_VERBOSE("---" << branches->At(i)->GetName());
  }
  if (branches->FindObject(br.c_str())) {
    MSG_DEBUG("Tree has the branch " << br);
    // m_branchesCheckList[br] = true;
    return true;
  }
  MSG_DEBUG("Tree does not have the branch " << br);
  // m_branchesCheckList[br] = false;
  return false;
}


template <class T> void PSL::D3PDWrapper::SetBranchStatusAddress(TTree* tree,const char* s,T addr,bool printWarning){
  //std::cout << "Setting branch address AND status!" << std::endl;
  if (!hasBranch(tree,std::string(s))) {
    if (std::string(s) == "vxp_nTracks" && hasBranch(tree,"vx_nTracks")) {
      MSG_INFO("Switching from vxp_nTracks to vx_nTracks");
      s = "vx_nTracks";
    }
    else { 
      if (printWarning) MSG_INFO("Warning: Branch missing: " << s);
      return;
    }
  }
  tree->SetBranchStatus(s,1);
  tree->SetBranchAddress(s,addr);
  MSG_DEBUG("Successfully set branch address AND status of " << s);
  return;
}

PSL::D3PDWrapper::D3PDWrapper() : m_allowUnlabeledSamples(false)
                                ,m_CurrentSample(knone)
                                ,mc_channel_number(0)
                                ,mc_event_weight(1)
                                ,m_evtdef(0)
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
  MSG_DEBUG("Constructor");
  SetName("PSL_EDM_CLASS");
}

EL::StatusCode PSL::D3PDWrapper::setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.
  MSG_DEBUG("setupJob");
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::D3PDWrapper::histInitialize()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.
  MSG_DEBUG("histInitialize");
  MSG_DEBUG("my name is " << GetName());

  m_EventVariables.clear();
  n_processed = 0;
  m_sumw_thisjob = 0;

  std::string dbpath = getenv("ROOTCOREBIN");
  dbpath += "/../SUSYTools/data/susy_crosssections_8TeV.txt";
  MSG_INFO("Loading SUSY XS DB path " << dbpath);
  m_susy_xsecDB = SUSY::CrossSectionDB(dbpath);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::D3PDWrapper::fileExecute()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  MSG_DEBUG("fileExecute");
  m_CurrentSample = knone;
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::D3PDWrapper::changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  MSG_DEBUG("changeInput");
  m_CurrentSample = knone;
  m_xsec = -1;
  m_kfac = -1;
  m_feff = -1;
  m_sumw = -1;

  TTree *tree = wk()->tree();
  tree->SetBranchStatus ("*", 0);

  SetBranchStatusAddress(tree,"EventNumber"      ,&EventNumber      );
  SetBranchStatusAddress(tree,"RunNumber"        ,&RunNumber        );
  mc_channel_number = 0;
  SetBranchStatusAddress(tree,"mc_channel_number",&mc_channel_number);
  SetBranchStatusAddress(tree,"larError"         ,&larError         );
  SetBranchStatusAddress(tree,"tileError"        ,&tileError        );
  SetBranchStatusAddress(tree,"lbn" ,&lbn );
  SetBranchStatusAddress(tree,"trig_DB_SMK" ,&trig_DB_SMK );
  // Add N_UInt_t

  SetBranchStatusAddress(tree,"el_n"               ,&el_n               );
  SetBranchStatusAddress(tree,"mu_staco_n"         ,&mu_staco_n         );
  SetBranchStatusAddress(tree,"jet_AntiKt4LCTopo_n",&jet_AntiKt4LCTopo_n);
  SetBranchStatusAddress(tree,"tau_n" ,&tau_n );
  SetBranchStatusAddress(tree,"el_MET_Egamma10NoTau_n" ,&el_MET_Egamma10NoTau_n );
  SetBranchStatusAddress(tree,"trig_EF_el_n" ,&trig_EF_el_n );
  SetBranchStatusAddress(tree,"trig_Nav_n" ,&trig_Nav_n );
  SetBranchStatusAddress(tree,"trig_EF_trigmuonef_n" ,&trig_EF_trigmuonef_n );
  // Add N_int

  SetBranchStatusAddress(tree,"EF_xe80_tclcw" ,&EF_xe80_tclcw );
  SetBranchStatusAddress(tree,"EF_e24vhi_medium1" ,&EF_e24vhi_medium1 );
  SetBranchStatusAddress(tree,"EF_e60_medium1" ,&EF_e60_medium1 );
  SetBranchStatusAddress(tree,"EF_2e12Tvh_loose1" ,&EF_2e12Tvh_loose1 );
  SetBranchStatusAddress(tree,"EF_e24vh_medium1_e7_medium1" ,&EF_e24vh_medium1_e7_medium1 );
  SetBranchStatusAddress(tree,"EF_e18vh_medium1_2e7T_medium1" ,&EF_e18vh_medium1_2e7T_medium1 );
  SetBranchStatusAddress(tree,"EF_mu24i_tight" ,&EF_mu24i_tight );
  SetBranchStatusAddress(tree,"EF_2mu13" ,&EF_2mu13 );
  SetBranchStatusAddress(tree,"EF_mu18_tight_mu8_EFFS" ,&EF_mu18_tight_mu8_EFFS );
  SetBranchStatusAddress(tree,"EF_3mu6" ,&EF_3mu6 );
  SetBranchStatusAddress(tree,"EF_mu18_tight_2mu4_EFFS" ,&EF_mu18_tight_2mu4_EFFS );
  SetBranchStatusAddress(tree,"EF_mu18_tight_e7_medium1" ,&EF_mu18_tight_e7_medium1 );
  SetBranchStatusAddress(tree,"EF_e12Tvh_medium1_mu8" ,&EF_e12Tvh_medium1_mu8 );
  SetBranchStatusAddress(tree,"EF_2e7T_medium1_mu6" ,&EF_2e7T_medium1_mu6 );
  SetBranchStatusAddress(tree,"EF_e7T_medium1_2mu6" ,&EF_e7T_medium1_2mu6 );
  // Add N_Bool_t

  SetBranchStatusAddress(tree,"MET_RefFinal_et"           ,&MET_RefFinal_et           );
  SetBranchStatusAddress(tree,"MET_RefFinal_etx"          ,&MET_RefFinal_etx          );
  SetBranchStatusAddress(tree,"MET_RefFinal_ety"          ,&MET_RefFinal_ety          );
  SetBranchStatusAddress(tree,"MET_RefFinal_phi"          ,&MET_RefFinal_phi          );
  SetBranchStatusAddress(tree,"MET_RefFinal_sumet"        ,&MET_RefFinal_sumet        );
  SetBranchStatusAddress(tree,"actualIntPerXing"          ,&actualIntPerXing          );
  SetBranchStatusAddress(tree,"averageIntPerXing"         ,&averageIntPerXing         );
  SetBranchStatusAddress(tree,"Eventshape_rhoKt4LC"       ,&Eventshape_rhoKt4LC       );
  SetBranchStatusAddress(tree,"MET_CellOut_Eflow_STVF_etx" ,&MET_CellOut_Eflow_STVF_etx );
  SetBranchStatusAddress(tree,"MET_CellOut_Eflow_STVF_ety" ,&MET_CellOut_Eflow_STVF_ety );
  SetBranchStatusAddress(tree,"MET_CellOut_Eflow_STVF_sumet" ,&MET_CellOut_Eflow_STVF_sumet );
  SetBranchStatusAddress(tree,"MET_Egamma10NoTau_RefGamma_etx" ,&MET_Egamma10NoTau_RefGamma_etx );
  SetBranchStatusAddress(tree,"MET_Egamma10NoTau_RefGamma_ety" ,&MET_Egamma10NoTau_RefGamma_ety );
  SetBranchStatusAddress(tree,"MET_Egamma10NoTau_RefGamma_sumet" ,&MET_Egamma10NoTau_RefGamma_sumet );
  SetBranchStatusAddress(tree,"MET_Egamma10NoTau_CellOut_etx" ,&MET_Egamma10NoTau_CellOut_etx );
  SetBranchStatusAddress(tree,"MET_Egamma10NoTau_CellOut_ety" ,&MET_Egamma10NoTau_CellOut_ety );
  SetBranchStatusAddress(tree,"MET_Egamma10NoTau_CellOut_sumet" ,&MET_Egamma10NoTau_CellOut_sumet );
  mc_event_weight = 1;
  SetBranchStatusAddress(tree,"mc_event_weight" ,&mc_event_weight );
  // Add N_float

  // vector float
  for (unsigned int i=0;i<D3PD::N_vector_float;++i){
    m_v_float.push_back(new std::vector<float>());
  }
  for (unsigned int i=0;i<D3PD::N_vector_float;++i){
    SetBranchStatusAddress(tree
  			   ,ConvertD3PDVarToStr(static_cast<D3PD::Var_vector_float>(i)).c_str()
  			   ,&m_v_float[i]);
  }

  // vector int
  for (unsigned int i=0;i<D3PD::N_vector_int;++i){
    m_v_int.push_back(new std::vector<int>());
  }
  for (unsigned int i=0;i<D3PD::N_vector_int;++i){
    SetBranchStatusAddress(tree
  			   ,ConvertD3PDVarToStr(static_cast<D3PD::Var_vector_int>(i)).c_str()
  			   ,&m_v_int[i]);
  }
  
  // vector short
  for (unsigned int i=0;i<D3PD::N_vector_short;++i){
    m_v_short.push_back(new std::vector<short>());
  }
  for (unsigned int i=0;i<D3PD::N_vector_short;++i){
    SetBranchStatusAddress(tree
  			   ,ConvertD3PDVarToStr(static_cast<D3PD::Var_vector_short>(i)).c_str()
  			   ,&m_v_short[i]);
  }
  
  // vector string
  for (unsigned int i=0;i<D3PD::N_vector_string;++i){
    m_v_string.push_back(new std::vector<std::string>());
  }
  for (unsigned int i=0;i<D3PD::N_vector_string;++i){
    SetBranchStatusAddress(tree
  			   ,ConvertD3PDVarToStr(static_cast<D3PD::Var_vector_string>(i)).c_str()
  			   ,&m_v_string[i]);
  }
  
  // vector vector uint
  for (unsigned int i=0;i<D3PD::N_vector_vector_UInt_t;++i){
    m_v_v_uint.push_back(new std::vector<std::vector<unsigned int> >());
  }
  for (unsigned int i=0;i<D3PD::N_vector_vector_UInt_t;++i){
    SetBranchStatusAddress(tree
  			   ,ConvertD3PDVarToStr(static_cast<D3PD::Var_vector_vector_UInt_t>(i)).c_str()
  			   ,&m_v_v_uint[i]);
  }

  // vector vector int
  for (unsigned int i=0;i<D3PD::N_vector_vector_int;++i){
    m_v_v_int.push_back(new std::vector<std::vector<int> >());
  }
  for (unsigned int i=0;i<D3PD::N_vector_vector_int;++i){
    SetBranchStatusAddress(tree
  			   ,ConvertD3PDVarToStr(static_cast<D3PD::Var_vector_vector_int>(i)).c_str()
  			   ,&m_v_v_int[i]);
  }

  // vector vector float
  for (unsigned int i=0;i<D3PD::N_vector_vector_float;++i){
    m_v_v_float.push_back(new std::vector<std::vector<float> >());
  }
  for (unsigned int i=0;i<D3PD::N_vector_vector_float;++i){
    SetBranchStatusAddress(tree
  			   ,ConvertD3PDVarToStr(static_cast<D3PD::Var_vector_vector_float>(i)).c_str()
  			   ,&m_v_v_float[i]);
  }

  // vector int
  for (unsigned int i=0;i<D3PD::N_vector_UInt_t;++i){
    m_v_uint.push_back(new std::vector<unsigned int>());
  }
  for (unsigned int i=0;i<D3PD::N_vector_UInt_t;++i){
    SetBranchStatusAddress(tree
  			   ,ConvertD3PDVarToStr(static_cast<D3PD::Var_vector_UInt_t>(i)).c_str()
  			   ,&m_v_uint[i]);
  }
  
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::D3PDWrapper::initialize()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.
  MSG_DEBUG("initialize");
  //m_event = wk()->xaodEvent();
  //PrintRunConfiguration();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::D3PDWrapper::execute()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.
  MSG_DEBUG("execute");
  MSG_VERBOSE("Before GetEntry");
  wk()->tree()->GetEntry(wk()->treeEntry());
  MSG_VERBOSE("After GetEntry");
  m_EventVariables.clear();
  el_index_save.clear();
  mu_index_save.clear();
  tau_index_save.clear();
  jet_index_save.clear();

  // for (unsigned int i=0;i<m_v_float.size();++i){
  //   MSG_INFO(ConvertD3PDVarToStr(static_cast<D3PD::Var_vector_float>(i)) << ": ");
  //   for (unsigned int j=0;j<m_v_float[i]->size();++j){
  //     MSG_INFO(" - " << m_v_float[i]->at(j));
  //   }
  // }

  if (m_CurrentSample == knone) {
    m_sumw = 1.;
    m_xsec = 1.;
    m_kfac = 1.;
    m_feff = 1.;
    m_lumi = 20276.9;
  
    //if(!eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)){ // not simulation == data
    m_CurrentSample = kunlabeled;
    if (mc_channel_number == 0){
      MSG_INFO("Setting sample to data.");
      m_CurrentSample = kdata;
    }
    else {
      MSG_DEBUG("Setting sample to mc.");
      if (!m_run_sample_map.count(mc_channel_number)) {
        MSG_INFO("Error! Run " << mc_channel_number << " cannot be converted to sample!");
        if (!m_allowUnlabeledSamples) return EL::StatusCode::FAILURE;
      }
      else {
        m_CurrentSample = m_run_sample_map[mc_channel_number];
        m_sumw = m_run_sumw_map[mc_channel_number];
        m_xsec = m_run_xsec_map[mc_channel_number];
      }
      MSG_INFO("Setting sample (Run " << mc_channel_number 
               << ") to " << ConvertSampleToStr(m_CurrentSample));

      SUSY::CrossSectionDB::Process p = m_susy_xsecDB.process(mc_channel_number);
      if (p.ID() > 0){
        m_xsec = p.xsect();
        m_kfac = p.kfactor();
        m_feff = p.efficiency();
        MSG_INFO("--- mc_channel_number: " << mc_channel_number);
        MSG_INFO("--- SUSY DB xsec     : " << m_xsec);
        MSG_INFO("--- SUSY DB kfactor  : " << m_kfac);
        MSG_INFO("--- SUSY DB filt eff : " << m_feff);
        MSG_INFO("--- Map_SRM sumw     : " << m_sumw);
        MSG_INFO("--- Local lumi       : " << m_lumi);
      }
      else {
        MSG_INFO("Error! No cross section found using SUSY DB! Default to value stored in Samples.txt");
        MSG_INFO("--- mc_channel_number: " << mc_channel_number);
        MSG_INFO("--- Samples.txt xsec*kfactor*feff: " << m_xsec);
        MSG_INFO("--- kfactor = 1      : " << m_kfac);
        MSG_INFO("--- filt eff = 1     : " << m_feff);        
        MSG_INFO("--- Map_SRM sumw     : " << m_sumw);
        MSG_INFO("--- Local lumi       : " << m_lumi);
      }
    }
  }
  
  if (!m_sample_stats.count(mc_channel_number)) {
    TString mcchan;
    mcchan.Form("%d",mc_channel_number);
    TString sumw_total_string;
    sumw_total_string.Form("%5.3f",m_sumw);
    m_sample_stats[mc_channel_number] = new TH1F(mcchan,mcchan,3,0,3);
    //m_sample_stats[mc_channel_number]->GetXaxis()->SetBinLabel(1,"sumw_fullsample");
    m_sample_stats[mc_channel_number]->GetXaxis()->SetBinLabel(1,sumw_total_string);
    m_sample_stats[mc_channel_number]->GetXaxis()->SetBinLabel(2,"sumw_thisjob");
    m_sample_stats[mc_channel_number]->GetXaxis()->SetBinLabel(3,"nevts_thisjob");
    wk()->addOutput(m_sample_stats[mc_channel_number]);
    m_sample_stats[mc_channel_number]->SetBinContent(1,m_sumw);
  }

  if (m_CurrentSample == kdata) m_weight = 1;
  else m_weight = mc_event_weight*m_xsec*m_kfac*m_feff*m_lumi/m_sumw;
  
  n_processed++;
  m_sumw_thisjob += mc_event_weight;
  m_sample_stats[mc_channel_number]->Fill(1.5,mc_event_weight);
  m_sample_stats[mc_channel_number]->Fill(2.5,1);
  MSG_DEBUG("Events processed: " << n_processed);
  if (!(n_processed%1000)) MSG_INFO("Events processed: " << n_processed);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::D3PDWrapper::postExecute()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  MSG_DEBUG("postExecute");
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::D3PDWrapper::finalize()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.
  MSG_INFO("finalize. Nevents processed: " << n_processed);
  MSG_INFO("finalize. sumw for this job: " << m_sumw_thisjob);
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::D3PDWrapper::histFinalize()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.
  MSG_DEBUG("histFinalize");
 return EL::StatusCode::SUCCESS;
}

bool PSL::D3PDWrapper::PassCosmicMuon(void){
  MSG_VERBOSE("passCosmicMuon");
  if (!m_evtdef) return false;
  double z0cut = 1;
  double d0cut = .2;
  for(unsigned int ii=0;ii<m_evtdef->mu.i_pre.size();++ii){
    int i = m_evtdef->mu.i_pre[ii];
    if (fabs(Get(D3PD::mu_staco_z0_exPV)->at(i)) >= z0cut || fabs(Get(D3PD::mu_staco_d0_exPV)->at(i)) >= d0cut){
      MSG_VERBOSE("passCosmicMuon false. z0: " << 
		  fabs(Get(D3PD::mu_staco_z0_exPV)->at(i)) << " d0: "
		  << fabs(Get(D3PD::mu_staco_d0_exPV)->at(i)));
      return false;
    }
  }
  MSG_VERBOSE("passCosmicMuon true");
  return true;
}

int PSL::D3PDWrapper::NBaselineLeptons(void){
  if (!m_evtdef) return false;
  return m_evtdef->mu.i_base.size() + m_evtdef->ele.i_base.size();
}

int PSL::D3PDWrapper::NSignalLeptons(void){
  if (!m_evtdef) return false;
  return m_evtdef->mu.i_sig.size() + m_evtdef->ele.i_sig.size();
}

// This used to be called passDeadRegions
bool PSL::D3PDWrapper::PassCaloJetCut(void){
  // https://indico.cern.ch/getFile.py/access?contribId=9&resId=0&materialId=slides&confId=223778
  // This is only done run number > 213863, when FEB turned off in HEC, and for all MC
  if (isData() && (RunNumber <= 213863)) return true;
  for(unsigned int ii=0;ii<m_evtdef->jet.i_pre.size();++ii){
    int i = m_evtdef->jet.i_pre[ii];
    if (m_evtdef->jet.tlv[i].Pt() <= 40000.) continue;
    if (Get(D3PD::jet_AntiKt4LCTopo_BCH_CORR_JET)->at(i) <= 0.05) continue;
    if (fabs(m_evtdef->jet.tlv[i].DeltaPhi(m_evtdef->met_tlv())) < 0.3) return false;
  }
  return true;
}
// bool SusyNtTools::passDeadRegions(const JetVector& preJets, const Met* met, int RunNumber, bool isMC)
// {
//   // Info taken from here:
//   // 
//   // Loop over all selected jets with Pt > 30 GeV and BCH_Corr_JET > 0.05
//   // If dPhi(met, jet) < 0.3, reject event
//   
//   if( !(RunNumber > 213863 || isMC) ) return true;
//   for(uint ij = 0; ij<preJets.size(); ++ij){
//     const Jet* jet = preJets.at(ij);
//     if( !(jet->Pt() > 40.) )          continue;
//     if( !(jet->bch_corr_jet > 0.05) ) continue;
//     if( fabs(jet->DeltaPhi( met->lv() )) < 0.3 ) return false;
//   }
//   return true;
// }

bool PSL::D3PDWrapper::PassBCH(void){
  if (!m_evtdef) return false;
  // BCH is apparently applied after all overlap removal (to final analysis jets)
  for(unsigned int ii=0;ii<m_evtdef->i_bch_jet.size();++ii){
    int i = m_evtdef->i_bch_jet[ii];
    if (!m_evtdef->m_passBCH[i]) return false;
  }
  return true;
}

double PSL::D3PDWrapper::EleTrkIsolation(int i){
  if (!m_evtdef) return 0;
  return Get(D3PD::el_ptcone30)->at(i)/m_evtdef->ele.tlv[i].Pt();
}
double PSL::D3PDWrapper::EleCaloIsolation(int i){
  return CaloIsoCorrection::GetPtEDCorrectedIsolation(Get(D3PD::el_Etcone40)->at(i)
                                                      ,Get(D3PD::el_Etcone40_ED_corrected)->at(i)
                                                      ,Get(D3PD::el_cl_E)->at(i)
                                                      ,Get(D3PD::el_etas2)->at(i)
                                                      ,Get(D3PD::el_etap)->at(i)
                                                      ,Get(D3PD::el_cl_eta)->at(i)
                                                      ,0.3
                                                      ,isMC()
                                                      ,Get(D3PD::el_Etcone30)->at(i));
}

double PSL::D3PDWrapper::EleD0Significance(int i){
  double d0sign = Get(D3PD::el_trackIPEstimate_sigd0_unbiasedpvunbiased)->at(i);
  if (d0sign) return fabs(Get(D3PD::el_trackIPEstimate_d0_unbiasedpvunbiased)->at(i)/d0sign);
  return 0;
}
double PSL::D3PDWrapper::EleZ0(int i){
  return Get(D3PD::el_trackIPEstimate_z0_unbiasedpvunbiased)->at(i);
}

double PSL::D3PDWrapper::MuonTrkIsolation(int i){
  if (!m_evtdef) return 0;
  return Get(D3PD::mu_staco_ptcone30)->at(i)/m_evtdef->mu.tlv[i].Pt();
}
double PSL::D3PDWrapper::MuonCaloIsolation(int i){
  return Get(D3PD::mu_staco_ptcone30)->at(i);
}
double PSL::D3PDWrapper::MuonD0Significance(int i){
  double d0sign = Get(D3PD::mu_staco_trackIPEstimate_sigd0_unbiasedpvunbiased)->at(i);
  if (d0sign) return fabs(Get(D3PD::mu_staco_trackIPEstimate_d0_unbiasedpvunbiased)->at(i)/d0sign);
  return 0;
}
double PSL::D3PDWrapper::MuonZ0(int i){
  return Get(D3PD::mu_staco_trackIPEstimate_z0_unbiasedpvunbiased)->at(i);
}

// PSL::LeptonChannel::LeptonChannel SignalLeptonsPassTriggers(void){
//   for (unsigned int i=0;i<m_evtdef->ele.i_sig.size();++i){
// }

#endif // ifndef SKIP_D3PD_COMPILATION
