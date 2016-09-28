#ifndef pennSoftLepton_EventDefinition_H
#define pennSoftLepton_EventDefinition_H

#include <bitset>
#include <algorithm>

#include "TLorentzVector.h"

#include <pennSoftLepton/Messaging.h>
#include "pennSoftLepton/Variables.h"
#include "pennSoftLepton/UtilCore.h"

#include "TObject.h"

namespace PSL
{

  class Particle {
  public:
    //* Index in the container */
    int i_cont;
    bool passOR;
    ObjType::ObjType obj_type;
    TLorentzVector tlv;
    int charge;
    float sf_recoIdIso;
    float sf_trig;
    bool is_Bjet;
    bool is_CentralLightJet;
    bool is_ForwardJet;
    bool is_trigMatched;

    Particle();
    Particle(TLorentzVector t,ObjType::ObjType type,int index,int ch,float sfall = 1,float sftrig = 1){
      i_cont = index;
      obj_type = type;
      tlv = t;
      charge = ch;
      sf_recoIdIso = sfall;
      sf_trig = sftrig;
      is_Bjet = false;
      is_CentralLightJet = false;
      is_ForwardJet = false;
      passOR = true;
      is_trigMatched = false;
    }
    ~Particle(){};
    double M()   const { if (tlv.Pt()) return tlv.M()  ; return -999;}
    double Pt()  const { return tlv.Pt();}
    double Phi() const { if (tlv.Pt()) return tlv.Phi(); return -999;}
    double Eta() const { if (tlv.Pt()) return tlv.Eta(); return -999;}
    double E()   const { if (tlv.Pt()) return tlv.E()  ; return -999;}
  };
  bool DescendingPt(const Particle& l1,const Particle& l2);
  bool IsElectron(const Particle& l);
  bool IsMuon(const Particle& l);
  bool IsBjet(const Particle& l);
  bool IsCentralLightJet(const Particle& l);
  bool IsForwardJet(const Particle& l);
  
  class EventDefinition : public TObject {  
  public:
    EventDefinition();

    bool valid; //!
    std::string name; //!

    int n_baseline_ele; //!
    int n_baseline_mu; //!
    int n_baseline_tau; //!
    int n_baseline_jet; //!
    int n_signal_jet; //!

    bool m_passBadJet; //!
    bool m_passBadMuon; //!
    bool m_passCosmicMuon; //!
    bool m_wlep_pass_sel; //!

    bool m_pass_trig_ele; //!
    bool m_pass_trig_mu; //!
    bool m_pass_trig_match; //!

    bool m_HLT_xe110_mht_xe70_L1XE50; //!
    bool m_HLT_xe110_mht_xe75_L1XE50; //!
    bool m_HLT_xe110_mht_L1XE50; //!
    bool m_HLT_xe130_mht_L1XE50; //! 
    bool m_cell_xe70; //!
    bool m_cell_xe75; //!

    LeptonChannel::LeptonChannel chan; //!
    LeptonChannel::LeptonChannel chan_antiid; //!
    std::vector<PSL::Particle> leps; //!
    std::vector<PSL::Particle> antileps; //!
    std::vector<PSL::Particle> vetoleps; //!
    std::vector<PSL::Particle> jets; //!
    std::vector<PSL::Particle> taus; //!

    int leadZlep_index;
    int subleadZlep_index;
    int Wlep_index;

    int leadlep_Zalt_index;
    int subleadlep_Zalt_index;

    int leadMinSFOS_index;
    int subleadMinSFOS_index;
    int WlepMinSFOS_index;

    int leadYlep_index;
    int subleadYlep_index;
    int WlepYpsilon_index;

    int antilep0_index;

    bool trigger_bits_set;
    std::bitset<PSL::N_TRIGGERS> trigger_bits; //! 0 for failed, 1 for passed

    // The indices for evaluating BCH (not exactly signal jets)
    std::vector<int> i_bch_jet; //!

    TVector2 met_tv2        ; //!
    double   met_sumet      ; //!
    TVector2 refEle_tv2     ; //!
    double   refEle_sumet   ; //!
    TVector2 refMuo_tv2     ; //!
    double   refMuo_sumet   ; //!
    TVector2 refJet_tv2     ; //!
    double   refJet_sumet   ; //!
    TVector2 refGamma_tv2   ; //!
    double   refGamma_sumet ; //!
    TVector2 softTerm_tv2   ; //!
    double   softTerm_sumet ; //!
    TVector2 truthmet_tv2   ; //!
    double   truthmet_sumet ; //!

    double truthmet_Et(void){return truthmet_tlv().Et();};
    double truthmet_phi(void){return truthmet_tlv().Phi();};

    double met_Et (void){return met_tlv().Et() ;};
    double met_phi(void){return met_tlv().Phi();};

    double refEle    (void){return refEle_tv2.Mod();};
    double refEle_etx(void){return refEle_tv2.Px() ;};
    double refEle_ety(void){return refEle_tv2.Py() ;};

    double refMuo    (void){return refMuo_tv2.Mod();};
    double refMuo_etx(void){return refMuo_tv2.Px() ;};
    double refMuo_ety(void){return refMuo_tv2.Py() ;};

    double refJet    (void){return refJet_tv2.Mod();};
    double refJet_etx(void){return refJet_tv2.Px() ;};
    double refJet_ety(void){return refJet_tv2.Py() ;};

    double refGamma    (void){return refGamma_tv2.Mod();};
    double refGamma_etx(void){return refGamma_tv2.Px() ;};
    double refGamma_ety(void){return refGamma_tv2.Py() ;};

    double softTerm    (void){return softTerm_tv2.Mod();};
    double softTerm_etx(void){return softTerm_tv2.Px() ;};
    double softTerm_ety(void){return softTerm_tv2.Py() ;};

    TLorentzVector met_tlv(void){
      TLorentzVector mtlv;
      mtlv.SetPxPyPzE(met_tv2.X(),met_tv2.Y(),0,met_tv2.Mod());      
      return mtlv;
    }

    TLorentzVector truthmet_tlv(void){
      TLorentzVector mtlv;
      mtlv.SetPxPyPzE(truthmet_tv2.X(), truthmet_tv2.Y(),0, truthmet_tv2.Mod());
      return mtlv;
    }
    //TLorentzVector met_tlv; //!

    void Reset(void){
      valid = false;
      m_passBadJet = true;
      m_passBadMuon = true;
      m_passCosmicMuon = true;
      m_wlep_pass_sel = true;

      m_pass_trig_ele = true;
      m_pass_trig_mu = true;

      m_HLT_xe110_mht_xe70_L1XE50 = false;
      m_HLT_xe110_mht_xe75_L1XE50 = false;
      m_HLT_xe110_mht_L1XE50 = false;
      m_HLT_xe130_mht_L1XE50 = false;
      m_cell_xe70 = false;
      m_cell_xe75 = false;
      
      n_baseline_ele = 0;
      n_baseline_mu = 0;
      n_baseline_tau = 0;
      n_baseline_jet = 0;
      n_signal_jet = 0;

      chan = LeptonChannel::none;
      chan_antiid = LeptonChannel::none;
      leps.clear();
      antileps.clear();
      vetoleps.clear();
      jets.clear();
      taus.clear();

      leadZlep_index = -1;
      subleadZlep_index = -1;
      Wlep_index = -1;

      leadlep_Zalt_index = -1;
      subleadlep_Zalt_index = -1;

      leadMinSFOS_index = -1;
      subleadMinSFOS_index = -1;
      WlepMinSFOS_index = -1;

      leadYlep_index = -1;
      subleadYlep_index = -1;
      WlepYpsilon_index = -1;

      antilep0_index = -1;

      trigger_bits_set = false;
      trigger_bits.reset();
    }

    LeptonChannel::LeptonChannel GetChannel(int n_ele,int n_mu){
      if (n_ele == 0 && n_mu == 0) return LeptonChannel::lep0; // 0 leptons
      if (n_ele == 1 && n_mu == 0) return LeptonChannel::e   ; // e
      if (n_ele == 0 && n_mu == 1) return LeptonChannel::mu  ; // mu
      if (n_ele == 2 && n_mu == 0) return LeptonChannel::ee  ; // ee
      if (n_ele == 1 && n_mu == 1) return LeptonChannel::emu ; // emu
      if (n_ele == 0 && n_mu == 2) return LeptonChannel::mumu; // mumu
      if (n_ele == 3 && n_mu == 0) return LeptonChannel::eee ; // eee
      if (n_ele == 2 && n_mu == 1) return LeptonChannel::eem ; // eem
      if (n_ele == 1 && n_mu == 2) return LeptonChannel::emm ; // emm
      if (n_ele == 0 && n_mu == 3) return LeptonChannel::mmm ; // mmm
      if (n_ele == 4 && n_mu == 0) return LeptonChannel::eeee ; // 
      if (n_ele == 3 && n_mu == 1) return LeptonChannel::eeem ; // 
      if (n_ele == 2 && n_mu == 2) return LeptonChannel::eemm ; // 
      if (n_ele == 1 && n_mu == 3) return LeptonChannel::emmm ; // 
      if (n_ele == 0 && n_mu == 4) return LeptonChannel::mmmm ; // 
      if (n_ele + n_mu > 4) return LeptonChannel::gt4leps ; // >4 leps
      return LeptonChannel::none;
    }

    void AddSignalLepton(Particle p){leps.push_back(p);}
    void RemoveSignalLepton(int index){
      // Use this carefully!
      // Note that you will have to recalculate the indices of the event
      if (index >= (int)leps.size() || index < 0) {
        MSG_ERROR("Error! Tried to remove a lepton that doesn't exist!");
      }
      leps.erase(leps.begin()+index);
      return;
    }
    void AddSignalJet(Particle p){jets.push_back(p);}
    void AddSignalTau(Particle p){taus.push_back(p);}
    void AddVetoLepton(Particle p){vetoleps.push_back(p);}
    void AddAntiIDLepton(Particle p){antileps.push_back(p);}

    void BuildEvent(void){
      std::sort(leps.begin(), leps.end(), DescendingPt);
      std::sort(antileps.begin(), antileps.end(), DescendingPt);
      std::sort(vetoleps.begin(), vetoleps.end(),DescendingPt);
      std::sort(jets.begin(), jets.end(), DescendingPt);
      std::sort(taus.begin(), taus.end(), DescendingPt);

      // anti-id leptons will be counted in the number of leptons. So if you don't want that,
      // then don't fill any antiid indices.
      int n_ele = count_if(leps.begin(),leps.end(),IsElectron);
      int n_antiele = count_if(antileps.begin(),antileps.end(),IsElectron);
      int n_mu  = count_if(leps.begin(),leps.end(),IsMuon);
      int n_antimu = count_if(antileps.begin(),antileps.end(),IsMuon);
      chan = GetChannel(n_ele+n_antiele,n_mu+n_antimu);
      chan_antiid = GetChannel(n_antiele,n_antimu);

      int dummy1; int dummy2;
      AssignSFOSIndices(leadZlep_index,subleadZlep_index,Wlep_index,leadlep_Zalt_index,subleadlep_Zalt_index);
      AssignSFOSIndices(leadMinSFOS_index,subleadMinSFOS_index,WlepMinSFOS_index,dummy1,dummy2,0.);
      AssignSFOSIndices(leadYlep_index,subleadYlep_index,WlepYpsilon_index,dummy1,dummy2,9400.);
      return;
    }

    void MakeCopy(EventDefinition& evtdef) const;

    int NBaselineLeptons(){ return n_baseline_ele+n_baseline_mu;}
    int NBaselineMuons(){ return n_baseline_mu;}
    int NBaselineElectrons(){ return n_baseline_ele;}
    int NBaselineJet(){ return n_baseline_jet;}

    int NSignalJet(){ return n_signal_jet;}
    int NSignalLeptons(){ return leps.size();}
    int NSignalMuons(){ return count_if(leps.begin(),leps.end(),IsMuon);}
    int NSignalElectrons(){ return count_if(leps.begin(),leps.end(),IsElectron);}
    int NSignalTaus(){ return taus.size();}

    int NAntiIDLeptons() { return antileps.size();}
    int NVetoLeptons(){return 0;};
    int WlepPassWSelection(){return m_wlep_pass_sel ? 1 : 0;}

    int PassTriggerEle(){return m_pass_trig_ele ? 1 : 0;}
    int PassTriggerMu(){return m_pass_trig_mu ? 1 : 0;}
    int PassTriggerMatch(){return m_pass_trig_match ? 1 : 0;}
    
    int HLT_xe110_mht_xe70_L1XE50() {return m_HLT_xe110_mht_xe70_L1XE50 ? 1:0;}
    int HLT_xe110_mht_xe75_L1XE50() {return m_HLT_xe110_mht_xe75_L1XE50? 1:0;}
    int HLT_xe110_mht_L1XE50() {return m_HLT_xe110_mht_L1XE50? 1:0;}
    int HLT_xe130_mht_L1XE50() {return m_HLT_xe130_mht_L1XE50? 1:0;}
    int cell_xe70() {return m_cell_xe70? 1:0;}
    int cell_xe75() {return m_cell_xe75? 1:0;}
    
    // transverse masses

    double Mt(void){
      TLorentzVector ll = Pair_LeadingLeptons();
      double mll2 = ll.M()*ll.M();
      double ptll2 = ll.Pt()*ll.Pt();
      TLorentzVector MET = met_tlv();
      return sqrt( mll2+2*(sqrt(ptll2 + mll2)*MET.Et()-MET.Px()*ll.Px()-MET.Py()*ll.Py())  );
    }

    double Mtll(void){
      TLorentzVector ll = Z_tlv();
      double mll2 = ll.M()*ll.M();
      double ptll2 = ll.Pt()*ll.Pt();
      return sqrt( mll2+2*(sqrt(ptll2+mll2)-ll.Px()-ll.Py()) );
    }
    double Mtll_gev(void){return Mtll()/1000.;};

    double MtWZ(void){
      TLorentzVector lll = Z_tlv() + W_tlv();
      double mlll2 = lll.M()*lll.M();
      double ptlll2 = lll.Pt()*lll.Pt();
      TLorentzVector MET = met_tlv();
      return sqrt( mlll2+2*(sqrt(ptlll2+mlll2)*MET.Et()-MET.Px()*lll.Px()-MET.Py()*lll.Py()));
    }
    double MtWZ_gev(void){return MtWZ()/1000.;};

    double Mt_leptonPair(TLorentzVector ll){
      double mll2 = ll.M()*ll.M();
      double ptll2 = ll.Pt()*ll.Pt();
      TLorentzVector MET = met_tlv();
      return sqrt(mll2+2*(sqrt(ptll2 + mll2)*MET.Et()-MET.Px()*ll.Px()-MET.Py()*ll.Py()));
    }

    double Mt_singleLepton(TLorentzVector lep){//equals CMS definition if used with dilep pair
      TLorentzVector MET = met_tlv();
      return sqrt(2*lep.Pt()*MET.Pt()*(1-cos(MET.DeltaPhi(lep))));
    }

    double Mt_WLepton_cand(){
      int i = Wlep_index;
      Particle p;
      if (i >= 0) p = leps[i];
      return Mt_singleLepton(p.tlv);
    }

    double Mt_WLepton_MinSFOS(){
      int i = WlepMinSFOS_index;
      Particle p;
      if (i >= 0) p = leps[i];
      return Mt_singleLepton(p.tlv);
    }

    //SUSY variables
    double Lt(void){
      if (leps.size() < 3) return -999;
      return ((leps[0].tlv).Pt()+(leps[1].tlv).Pt()+(leps[2].tlv).Pt());
    }
    double Lt_gev(void){return Lt()/1000.;};

    double Meff(){
      if (leps.size() < 3) return -999;
      return ((leps[0].tlv).Pt()+(leps[1].tlv).Pt()+(leps[2].tlv).Pt()+(met_tlv()).Pt());
    }
    double Meff_gev(void){return Meff()/1000.;};

    double MetOverMeff(){
      return met_Et()/Meff();
    }

    //delta R

    double DeltaRll(void){
      if (leps.size() < 2) return -999;
      return leps[0].tlv.DeltaR(leps[1].tlv);
    }

    double DeltaR_Zleptons(double closest_to=91188.){
      (void)closest_to;
      TLorentzVector t1;
      int i0 = leadZlep_index;
      int i1 = subleadZlep_index;
      if (i0 < 0 || i1 < 0) return 0;
      return leps[i0].tlv.DeltaR(leps[i1].tlv);
    }

    double DeltaR_ZleptonsZ(){return DeltaR_Zleptons();};
    double DeltaR_ZleptonsZMin(){return DeltaR_Zleptons(0.);};
    double PtlloverMET(){return Pair_LeadingLeptonsPt_gev()/met_Et_gev();};
    double PtllZoverMET(){return Pair_SFOSPt_gev()/met_Et_gev();};
    double PtllZminoverMET(){return Pair_minSFOSPt_gev()/met_Et_gev();};
    
    //delta phi

    double DeltaPhill(void){
      if (leps.size() < 2) return -999;
      return std::abs(leps[0].tlv.DeltaPhi(leps[1].tlv));
    }
    
    double DeltaPhiLeadZWLep(void){
      if ((leps.size() < 2)||(leadZlep_index < 0) || (Wlep_index < 0)) return -999;
      return std::abs(leps[leadZlep_index].tlv.DeltaPhi(leps[Wlep_index].tlv));
    }

    double DeltaPhiZLeps(void){
      if ((leps.size() < 2)||(leadZlep_index < 0) || (subleadZlep_index < 0)) return -999;
      return std::abs(leps[leadZlep_index].tlv.DeltaPhi(leps[subleadZlep_index].tlv));
    }

    double DeltaPhiZWLep(void){
      if ((leps.size() < 3)||(leadZlep_index < 0) || (subleadZlep_index < 0)||(Wlep_index < 0)) return -999;
      return std::abs(Z_tlv().DeltaPhi(leps[Wlep_index].tlv));
    }

    double DeltaPhiZMet(void){
      if ((leps.size() < 2)||(leadZlep_index < 0) || (subleadZlep_index < 0)) return -999;
      return std::abs(met_tlv().DeltaPhi(Z_tlv()));
    }
    
    double DeltaPhiZWSyst(void){
      if ((leps.size() < 3)||(leadZlep_index < 0) || (subleadZlep_index < 0) || (Wlep_index < 0)) return -999;
      return std::abs((met_tlv()+leps[Wlep_index].tlv).DeltaPhi(Z_tlv()));
    }
    
    double DeltaPhiLep0MET(void){
      if (leps.size() < 1) return 999.;
      return std::abs( met_tlv().DeltaPhi(leps[0].tlv) );
    }

    double DeltaPhiLep1MET(void){
      if (leps.size() < 2) return 999.;
      return std::abs( met_tlv().DeltaPhi(leps[1].tlv) );
    }

    double DeltaPhiLep2MET(void){
      if (leps.size() < 3) return 999.;
      return std::abs( met_tlv().DeltaPhi(leps[2].tlv) );
    }

    double DeltaPhiLepMET(TLorentzVector lep){
      return std::abs( met_tlv().DeltaPhi(lep) );
    }

    double MinDeltaPhiLep0Lep1_MET(void){return std::min(DeltaPhiLep0MET(),DeltaPhiLep1MET());}

    double MinDeltaPhi3Leps_MET(void){return std::min(MinDeltaPhiLep0Lep1_MET(),DeltaPhiLep2MET());}

    double MaxDeltaPhi3Leps_MET(void){
      if (leps.size() < 1) return -999.;
      if (leps.size() < 2) return std::abs( met_tlv().DeltaPhi(leps[0].tlv)) ;
      if (leps.size() < 3) return std::max( std::abs( met_tlv().DeltaPhi(leps[0].tlv) ) , std::abs( met_tlv().DeltaPhi(leps[1].tlv) ) );
      return std::max( std::abs( met_tlv().DeltaPhi(leps[3].tlv) ) , std::max( std::abs(met_tlv().DeltaPhi(leps[0].tlv) ) , std::abs( met_tlv().DeltaPhi(leps[1].tlv) ) ) );
    }

    double DeltaPhi3Leps_MET(void){
      return std::abs(met_tlv().DeltaPhi(Three_LeadingLeptons()));
    }

    double DeltaPhi_MET_Jet1(void){
      if (jets.size() < 1) return -999.;
      return std::abs(jets[0].tlv.DeltaPhi(met_tlv()));
    }    

    double METoverPtj0(void){
      if (jets.size() < 1) return -999.;
      return met_Et()/jets[0].Pt();
    }
    
    double MaxDeltaPhiLep0Lep1_Jet1(void){
      if (leps.size() < 1 || jets.size() < 1) return -999.;
      if (leps.size() < 2) return std::abs(jets[0].tlv.DeltaPhi(leps[0].tlv));
      return std::max(std::abs(jets[0].tlv.DeltaPhi(leps[0].tlv)),std::abs(jets[0].tlv.DeltaPhi(leps[1].tlv)));
    }

    double MaxDeltaPhi3Leps_Jet1(void){
      if (leps.size() < 1 || jets.size() < 1) return 999.;
      if (leps.size() < 2) return std::abs(jets[0].tlv.DeltaPhi(leps[0].tlv));
      if (leps.size() < 3) return std::max(std::abs(jets[0].tlv.DeltaPhi(leps[0].tlv)),std::abs(jets[0].tlv.DeltaPhi(leps[1].tlv)));
      return std::max(std::abs(jets[0].tlv.DeltaPhi(leps[3].tlv)),std::max(std::abs(jets[0].tlv.DeltaPhi(leps[0].tlv)),std::abs(jets[0].tlv.DeltaPhi(leps[1].tlv))));
    }

    double MinDeltaPhi3Leps_Jet1(void){
      if (leps.size() < 1 || jets.size() < 1) return 999.;
      if (leps.size() < 2) return std::abs(jets[0].tlv.DeltaPhi(leps[0].tlv));
      if (leps.size() < 3) return std::min(std::abs(jets[0].tlv.DeltaPhi(leps[0].tlv)),std::abs(jets[0].tlv.DeltaPhi(leps[1].tlv)));
      return std::min( std::abs( jets[0].tlv.DeltaPhi(leps[3].tlv) ),std::min(std::abs(jets[0].tlv.DeltaPhi(leps[0].tlv)),std::abs(jets[0].tlv.DeltaPhi(leps[1].tlv))));
    }

    double DeltaPhi3Leps_Jet1(void){
      if (jets.size() < 1) return -999.;
      return std::abs(jets[0].tlv.DeltaPhi(Three_LeadingLeptons()));
    }

    double DeltaPhiLepJet1(TLorentzVector lep){
      if (jets.size() < 1) return -999.;
      return std::abs(jets[0].tlv.DeltaPhi(lep));
    }

    double YZminusYlW(void){
      return (Z_tlv().Rapidity()- (W_tlv()).Rapidity());
    }

    int ChanFlavor(void){return (int)chan;}
    int ChanFlavorAntiID(void){return (int)chan_antiid;}

    int IsSameSign(void){  // 2L events
      if (leps.size() != 2) return -999; 
      return (leps[0].charge == leps[1].charge ? 1 : 0);
    }

    double Pair_SFSS_M_gev(void){
      if (leps.size() != 2) return -999; 
      if (leps[0].charge != leps[1].charge) return -999.;
      return (leps[0].tlv+leps[1].tlv).M()/1000.;
    }

    int IsSFOS(void){
      for (unsigned int i=0;i<leps.size();++i){
        for (unsigned int j=0;j<leps.size();++j){
          if (i == j) continue;
          if (leps[i].obj_type != leps[j].obj_type) continue;
          if (leps[i].charge == leps[j].charge) continue;
          return 1;
        }
      }
      return 0;
    }

    Particle Pair_SFOS(void){
      int i0 = leadZlep_index;
      int i1 = subleadZlep_index;
      if (i0 < 0 || i1 < 0) return Particle();
      return Particle(leps[i0].tlv+leps[i1].tlv,leps[i0].obj_type,0,0);
    }
    Particle Pair_minSFOS(void){
      int i0 = leadMinSFOS_index;
      int i1 = subleadMinSFOS_index;
      if (i0 < 0 || i1 < 0) return Particle();
      return Particle(leps[i0].tlv+leps[i1].tlv,leps[i0].obj_type,0,0);
    }
    Particle Pair_SFOS_Upsilon(void){
      int i0 = leadYlep_index;
      int i1 = subleadYlep_index;
      if (i0 < 0 || i1 < 0) return Particle();
      return Particle(leps[i0].tlv+leps[i1].tlv,leps[i0].obj_type,0,0);      
    }

    void AssignSFOSIndices(int& SFOS_lep1index,int& SFOS_lep2index,int& wlep_i
                           ,int& SFOS_alt1index,int& SFOS_alt2index,double closest_to=91188.){
      SFOS_lep1index = -1;
      SFOS_lep2index = -1;
      // the "alt" indices are for the second-closest SFOS lepton pair to the zmass
      // (or other specified mass).
      SFOS_alt1index = -1;
      SFOS_alt2index = -1;
      double min_delta = 999999999;
      double min_delta2 = 999999999;
      for (unsigned int i=0;i<leps.size();++i){
        for (unsigned int j=0;j<leps.size();++j){
          if (i == j) continue;
          if (leps[i].obj_type != leps[j].obj_type) continue;
          if (leps[i].charge == leps[j].charge) continue;
          if (leps[i].Pt() < leps[j].Pt()) continue;
          Particle tmp(leps[i].tlv+leps[j].tlv,leps[i].obj_type,0,0);
          if (fabs(tmp.M()-closest_to) < min_delta) {
            min_delta2 = min_delta;
            min_delta = fabs(tmp.M()-closest_to);
            SFOS_alt1index = SFOS_lep1index;
            SFOS_alt2index = SFOS_lep2index;
            SFOS_lep1index = i;
            SFOS_lep2index = j;
          } else if (fabs(tmp.M()-closest_to) < min_delta2) {
            min_delta2 = fabs(tmp.M()-closest_to);
            SFOS_alt1index = i;
            SFOS_alt2index = j;
          }
        }
      }
      //MSG_INFO("min_delta: " << min_delta << " min_delta2: " << min_delta2);

      //
      // if SFOS above failed, then there is no W candidate that makes sense:
      if (SFOS_lep1index < 0 || SFOS_lep2index < 0) return;
      //MSG_INFO("Assigned z lep indices to " << SFOS_lep1index << ", " << SFOS_lep2index);

      // get lepton that is not SFOS
      // if there are more than one then take the first (pt-ordered) one
      for (unsigned int i=0;i<leps.size();++i){
        if ( ((int)i != SFOS_lep1index) && ((int)i != SFOS_lep2index) ){
          wlep_i = i;
          break;
        }
      }
      return;
    }
    
    TLorentzVector Z_tlv(void){
      if ((leps.size() < 2)||(leadZlep_index < 0) || (subleadZlep_index < 0)) return TLorentzVector();
      return (leps[leadZlep_index].tlv+leps[subleadZlep_index].tlv);
    }

    TLorentzVector W_tlv(void){
      if ((leps.size() < 1)||(Wlep_index < 0)) return TLorentzVector();
      return (leps[Wlep_index].tlv);
    }

    TLorentzVector Three_LeadingLeptons(void){
      if (leps.size() < 3) return TLorentzVector();
      return (leps[0].tlv+leps[1].tlv+leps[2].tlv);
    }

    TLorentzVector Pair_LeadingLeptons(void){
      if (leps.size() < 2) return TLorentzVector();
      return (leps[0].tlv+leps[1].tlv);
    }
    double Pair_LeadingLeptonsPt_gev(void){return Pair_LeadingLeptons().Pt()/1000.;};
    double Three_LeadingLeptons_gev(void){return Three_LeadingLeptons().Pt()/1000.;};
    double Pt3lPlusMet(void){return (Three_LeadingLeptons()+met_tlv()).Pt()/1000.;};
    double PtWZ(void){return (Z_tlv()+W_tlv()+met_tlv()).Pt()/1000.;};
    double PtW(void){return (W_tlv()+met_tlv()).Pt()/1000.;};
    double Pair_SFOSPt_gev(void){return Pair_SFOS().Pt()/1000.;};
    double Pair_minSFOSPt_gev(void){return Pair_minSFOS().Pt()/1000.;};
    double Pair_SFOS_M_gev(void){return Pair_SFOS().M()/1000.;};
    double Pair_minSFOS_M_gev(void){return Pair_minSFOS().M()/1000.;};
    double Mt_WLepton_cand_gev(void){return Mt_WLepton_cand()/1000.;};
    double Mt_WLepton_MinSFOS_gev(void){return Mt_WLepton_MinSFOS()/1000.;};
    double Mt_gev(void){return Mt()/1000.;};
    double M3l_gev(void){return Three_LeadingLeptons().M()/1000.;};
    double Mll_Upsilon_gev(void){return Pair_SFOS_Upsilon().M()/1000.;}
    double AltPair_SFOS_M_gev(void){
      if (leadlep_Zalt_index < 0 || subleadlep_Zalt_index < 0) return 0;
      return (leps[leadlep_Zalt_index].tlv+leps[subleadlep_Zalt_index].tlv).M()/1000.;
    }

    Particle PtOrderedLepton(int i){ 
      if (leps.size() < (unsigned int)i+1) return Particle();
      return leps.at(i);
    };
    Particle PtOrderedAntiLepton(int i){ 
      if (antileps.size() < (unsigned int)i+1) return Particle();
      return antileps.at(i);
    };
    Particle PtOrderedJet(int i){
      if (jets.size() < (unsigned int)i+1) return Particle();
      return jets.at(i);
    }

    bool SignalLeptonIsMuon(int i){
      if (leps.size() < (unsigned int)i+1) return false;
      return IsMuon(leps.at(i));
    }

    bool SignalLeptonIsElectron(int i){
      if (leps.size() < (unsigned int)i+1) return false;
      return IsElectron(leps.at(i));
    }

    int GetContainerIndex(int iptordered){
      if (leps.size() < (unsigned int)iptordered+1) return -1;
      return leps.at(iptordered).i_cont;
    }

    Particle LeadingLepton(void)   { if (leps.size() < 1)   return Particle(); return leps.at(0);};
    Particle SubleadingLepton(void){ if (leps.size() < 2)   return Particle(); return leps.at(1);};
    Particle ThirdLepton(void)     { if (leps.size() < 3)   return Particle(); return leps.at(2);};
    Particle TrailingLepton(void)  { if (leps.size() == 0)  return Particle(); return leps.back();};
    Particle LeadingJet(void)      { if (jets.size() < 1)   return Particle(); return jets.at(0);};
    Particle SubleadingJet(void)   { if (jets.size() < 2)   return Particle(); return jets.at(1);};
    Particle TrailingJet(void)     { if (jets.size() == 0)  return Particle(); return jets.back();};
    Particle LeadZLepton(void)     { 
      if (leadZlep_index < 0) return Particle(); return leps.at(leadZlep_index);
    };
    Particle SubleadZLepton(void)  { 
      if (subleadZlep_index < 0) return Particle(); return leps.at(subleadZlep_index);
    };
    Particle WLepton(void) {
      if (Wlep_index < 0) return Particle(); return leps.at(Wlep_index);
    };

    Particle LeadingAntiLepton(void){ if (antileps.size() < 1) return Particle(); return antileps.at(0);};

    int LeadingLeptonIndex(void){return (NSignalLeptons() > 0) ? 0 : -1;};
    int SubleadingLeptonIndex(void){return (NSignalLeptons() > 1) ? 1 : -1;};
    int ThirdLeptonIndex(void){return (NSignalLeptons() > 2) ? 2 : -1;};
    int LeadZLepIndex(void){return leadZlep_index;};
    int SubleadZLepIndex(void){return subleadZlep_index;};
    int WlepIndex(void){return Wlep_index;};
    int LeadMinSFOSIndex(void){return leadMinSFOS_index;};
    int SubleadMinSFOSIndex(void){return subleadMinSFOS_index;};
    int WMinSFOSIndex(void){return WlepMinSFOS_index;};
    int Antilep0Index(void){return antilep0_index;};
    int Jet0Index(void){return (nJets() > 0) ? 0 : -1;};
    int Jet1Index(void){return (nJets() > 1) ? 1 : -1;};

    Particle TrailingMuon(void) { 
      for(std::vector<PSL::Particle>::reverse_iterator rit = leps.rbegin(); rit != leps.rend(); ++rit){
        if ((*rit).obj_type == ObjType::Muon) return *rit;
      }
      return Particle();
    }
    Particle TrailingElectron(void) {
      for(std::vector<PSL::Particle>::reverse_iterator rit = leps.rbegin(); rit != leps.rend(); ++rit){
        if ((*rit).obj_type == ObjType::Electron) return *rit;
      }
      return Particle();
    }

    int nJets(void){ 
      MSG_DEBUG("Returning njets of " << jets.size());
      return jets.size();
    }

    int nBjets      (void){ return count_if(jets.begin(),jets.end(),IsBjet           );}
    int nCentralJets(void){ return count_if(jets.begin(),jets.end(),IsCentralLightJet);}
    int nForwardJets(void){ return count_if(jets.begin(),jets.end(),IsForwardJet     );}
    
    Particle Pair_Wjj(){
      double closest_to=80385.;
      double min_delta = 999999999;
      Particle return_part;
      for (unsigned int i=0;i<jets.size();++i){
        for (unsigned int j=0;j<jets.size();++j){
          if (i == j) continue;
          Particle tmp(jets[i].tlv+jets[j].tlv,jets[i].obj_type,0,0);
          if (fabs(tmp.M()-closest_to) < min_delta) {
            min_delta = fabs(tmp.M()-closest_to);
            return_part = tmp;
          }
        }
      }
      return return_part;
    }
    double Pair_Wjj_M_gev(void){ return Pair_Wjj().M()/1000.;}

    Particle Pair_jjMin(){
      double closest_to=0.;
      double min_delta = 999999999;
      Particle return_part;
      for (unsigned int i=0;i<jets.size();++i){
        for (unsigned int j=0;j<jets.size();++j){
          if (i == j) continue;
          Particle tmp(jets[i].tlv+jets[j].tlv,jets[i].obj_type,0,0);
          if (fabs(tmp.M()-closest_to) < min_delta) {
            min_delta = fabs(tmp.M()-closest_to);
            return_part = tmp;
          }
        }
      }
      return return_part;
    }
    double Pair_jjMin_M_gev(){ return Pair_jjMin().M()/1000.; }

    int PassBadJet(void){ return m_passBadJet ? 1 : 0; }
    int PassBadMuon(void){ return m_passBadMuon ? 1 : 0; }
    int PassCosmicMuon(void){ return m_passCosmicMuon ? 1 : 0; }

    void FillVectorOfFunctions(void);
    double GetEventVariable(Variable v);

    double met_Et_gev        (void){return met_Et        ()/1000.;};
    double truthmet_Et_gev   (void){return truthmet_Et   ()/1000.;};
    double met_sumet_gev     (void){return met_sumet       /1000.;};
    double refEle_gev        (void){return refEle        ()/1000.;};
    double refEle_etx_gev    (void){return refEle_etx    ()/1000.;};
    double refEle_ety_gev    (void){return refEle_ety    ()/1000.;};
    double refEle_sumet_gev  (void){return refEle_sumet    /1000.;};
    double refMuo_gev        (void){return refMuo        ()/1000.;};
    double refMuo_etx_gev    (void){return refMuo_etx    ()/1000.;};
    double refMuo_ety_gev    (void){return refMuo_ety    ()/1000.;};
    double refMuo_sumet_gev  (void){return refMuo_sumet    /1000.;};
    double refJet_gev        (void){return refJet        ()/1000.;};
    double refJet_etx_gev    (void){return refJet_etx    ()/1000.;};
    double refJet_ety_gev    (void){return refJet_ety    ()/1000.;};
    double refJet_sumet_gev  (void){return refJet_sumet    /1000.;};
    double refGamma_gev      (void){return refGamma      ()/1000.;};
    double refGamma_etx_gev  (void){return refGamma_etx  ()/1000.;};
    double refGamma_ety_gev  (void){return refGamma_ety  ()/1000.;};
    double refGamma_sumet_gev(void){return refGamma_sumet  /1000.;};
    double softTerm_gev      (void){return softTerm      ()/1000.;};
    double softTerm_etx_gev  (void){return softTerm_etx  ()/1000.;};
    double softTerm_ety_gev  (void){return softTerm_ety  ()/1000.;};
    double softTerm_sumet_gev(void){return softTerm_sumet  /1000.;};

  private:

#ifndef __MAKECINT__
    std::vector<double (PSL::EventDefinition::*) ()> m_VariableFunctions_double; //!
    std::vector<int    (PSL::EventDefinition::*) ()> m_VariableFunctions_int; //!
#endif // __MAKECINT__
    
  private:
    // These are here for technical reasons only
    double ERROR_D(){
      MSG_INFO("Error! You made a huge mistake.");
      exit(1);
    }
    int ERROR_I(){
      MSG_INFO("Error! You made a huge mistake.");
      exit(1);
    }

  public:

    ClassDef(EventDefinition, 1);
    
  };

}
#endif // pennSoftLepton_EventDefinition_H
