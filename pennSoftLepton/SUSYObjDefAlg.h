#include "pennSoftLepton/switch_d3pd_xaod.h"
#ifndef SKIP_D3PD_COMPILATION

#ifndef pennSoftLepton_SUSYObjDefAlg_H
#define pennSoftLepton_SUSYObjDefAlg_H

#include <pennSoftLepton/AlgBase.h>
#include <SUSYTools/SUSYObjDef.h>
#include "pennSoftLepton/ToolsD3PDEra.h"
#include "pennSoftLepton/UtilCore.h"

namespace PSL
{
  class SUSYObjDefAlg : public AlgBase
  {
    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
  public:
    //* \brief Min electron pt for preselecting objects */
    double ele_ptmin;
    //* \brief Min muon pt for preselecting objects */
    double mu_ptmin;
    //* \brief Min tau pt for preselecting objects */
    double tau_ptmin;
    //* \brief Min jet pt for preselecting objects */
    double jet_ptmin;

    //* \brief Max electron eta for preselecting objects */
    double ele_eta_max;
    //* \brief Max muon eta for preselecting objects */
    double mu_eta_max;
    //* \brief Max tau eta for preselecting objects */
    double tau_eta_max;
    //* \brief Max jet eta for preselecting objects */
    double jet_eta_max;

    //* \brief electron d0 significance cut */
    double ele_d0_max;
    //* \brief electron z0sintheta cut */
    double ele_z0_max;
    //* \brief electron trk isolation cut */
    double ele_trkiso_max;
    //* \brief electron calo isolation cut */
    double ele_caloiso_max;
    //* \brief baseline electron identification (0=loose, 1=med, 2=tight) */
    int ele_id_base;
    //* \brief electron identification (0=loose, 1=med, 2=tight) */
    int ele_id;

    //* \brief muon d0 significance cut */
    double mu_d0_max;
    //* \brief muon z0sintheta cut */
    double mu_z0_max;
    //* \brief muon trk isolation cut */
    double mu_trkiso_max;
    //* \brief muon calo isolation cut */
    double mu_caloiso_max;

    //* \brief Min forward jet eta */
    double jet_forward_eta_min;
    //* \brief Max forward jet eta */
    double jet_forward_eta_max;
    //* \brief Min forward jet pt */
    double jet_forward_ptmin;

    //* \brief Min Bjet pt */
    double jet_bjet_ptmin;
    //* \brief Min Bjet mv1 */
    double jet_bjet_mv1;
    
    //* \brief Min central jvf cut */
    double jet_central_jvfcut;
    //* \brief Max jvf eta (according to jvf twiki) */
    double jet_central_jvfeta_max;
    //* \brief Max eta */
    double jet_central_eta_max;
    //* \brief Min central pt */
    double jet_central_ptmin;
    //* \brief Max central (non-bjet) mv1 */
    double jet_central_mv1;

    //* \brief turn on or off overlap removal */
    bool do_overlapremove;

    void set_ele_ptmin             (double d){ele_ptmin              = d;};
    void set_mu_ptmin              (double d){mu_ptmin               = d;};
    void set_tau_ptmin             (double d){tau_ptmin              = d;};
    void set_jet_ptmin             (double d){jet_ptmin              = d;};
    void set_ele_eta_max           (double d){ele_eta_max            = d;};
    void set_mu_eta_max            (double d){mu_eta_max             = d;};
    void set_tau_eta_max           (double d){tau_eta_max            = d;};
    void set_jet_eta_max           (double d){jet_eta_max            = d;};
    void set_ele_d0_max            (double d){ele_d0_max             = d;};
    void set_ele_z0_max            (double d){ele_z0_max             = d;};
    void set_ele_trkiso_max        (double d){ele_trkiso_max         = d;};
    void set_ele_caloiso_max       (double d){ele_caloiso_max        = d;};
    void set_ele_id_base      (std::string s){ele_id_base            = 0;}; // not actually used
    void set_ele_id           (std::string s){
      if (s == "Loose") ele_id = 0;
      if (s == "Medium") ele_id = 1;
      if (s == "Tight") ele_id = 2;
    };
    void set_mu_d0_max             (double d){mu_d0_max              = d;};
    void set_mu_z0_max             (double d){mu_z0_max              = d;};
    void set_mu_trkiso_max         (double d){mu_trkiso_max          = d;};
    void set_mu_caloiso_max        (double d){mu_caloiso_max         = d;};
    void set_jet_forward_eta_min   (double d){jet_forward_eta_min    = d;};
    void set_jet_forward_eta_max   (double d){jet_forward_eta_max    = d;};
    void set_jet_forward_ptmin     (double d){jet_forward_ptmin      = d;};
    void set_jet_bjet_ptmin        (double d){jet_bjet_ptmin         = d;};
    void set_jet_bjet_mv1          (double d){jet_bjet_mv1           = d;};
    void set_jet_central_jvfcut    (double d){jet_central_jvfcut     = d;};
    void set_jet_central_jvfeta_max(double d){jet_central_jvfeta_max = d;};
    void set_jet_central_eta_max   (double d){jet_central_eta_max    = d;};
    void set_jet_central_ptmin     (double d){jet_central_ptmin      = d;};
    void set_jet_central_mv1       (double d){jet_central_mv1        = d;};
    void set_do_overlapremove      (bool b  ){do_overlapremove       = b;};


    // variables that don't get filled at submission time should be
    // protected from being send from the submission node to the worker
    // node (done by the //!)
  public:

    SUSYObjDef* m_SUSYObjDef; //!
    ToolsD3PDEra* m_toolsd3pd; //!

    // All of the below have to be initialized at the start of an event
    EventDefinition m_evtdef; //!
    // All of the above have to be initialized at the start of an event

    // this is a standard constructor
    SUSYObjDefAlg();
    //
    bool init(void);
    void loop(void);
    void finish(void);

    void PrintConfiguration(void);

    /** \brief super dumb overlap removal */
    void OverlapRemoveAll(void);
    void OverlapRemove(int type_removal // 0 == electron, 1 == muon, 2 == tau, 3 == jet
		       ,int type_static
		       ,std::vector<int>& indices_removal
		       ,std::vector<int>& indices_dumpto
		       ,std::vector<int> indices_static
		       ,double dr
		       ,bool remove_softer=true // only relevant in the case where types are the same
		       );

    bool isSignalJet(int i);
    bool isCentralLightJet(int i);
    bool isForwardJet(int i);
    bool isCentralBJet(int i);
    bool JetPassesJvfRequirement(int i);

    void HandleBadJet(void);
    void HandleBCH(void);
    void FindSignalJets(void);
    void FindSignalElectrons(void);
    void FindSignalMuons(void);
    void FindSignalTaus(void);
    
    // this is needed to distribute the algorithm to the workers
    ClassDef(SUSYObjDefAlg, 1);
  };
  
}
#endif

#endif // ifndef SKIP_D3PD_COMPILATION
