#ifndef pennSoftLepton_ObjectSelectionBase_H
#define pennSoftLepton_ObjectSelectionBase_H

#include <pennSoftLepton/AlgBase.h>

namespace PSL
{
  
  class ObjectSelectionBase : public AlgBase
  {

  public:

    ObjectSelectionBase();

    //* \brief Min electron pt for preselecting objects */
    double ele_ptmin_base;
    //* \brief Min electron pt for preselecting objects */
    double ele_ptmin;
    //* \brief Min electron pt for preselecting objects */
    double ele_ptmin_antiid;
    //* \brief Min muon pt for preselecting objects */
    double mu_ptmin_base;
    //* \brief Min muon pt for preselecting objects */
    double mu_ptmin;
    //* \brief Min muon pt for preselecting objects */
    double mu_ptmin_antiid;
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
    //* \brief electron d0 significance cut */
    double ele_d0_max_antiid;
    //* \brief electron d0 significance cut */
    double ele_d0_min_antiid;
    //* \brief electron z0sintheta cut */
    double ele_z0_max;
    //* \brief electron z0sintheta cut */
    double ele_z0_max_antiid;
    //* \brief electron z0sintheta cut */
    double ele_z0_min_antiid;
    //* \brief electron iso WP (will set for trk and calo. Overrides all other isolation settings.)*/
    AnaIso::AnaIso ele_isowp;
    //
    AnaIso::AnaIso ele_isowp_base;
    //
    AnaIso::AnaIso ele_isowp_fail_antiid;
    //
    AnaIso::AnaIso ele_isowp_pass_antiid;
    //* \brief electron trk isolation flavor. Overridden by isowp. */
    AnaIso::AnaIso ele_trkisotype;
    //* \brief electron calo isolation flavor. Overridden by isowp. */
    AnaIso::AnaIso ele_caloisotype;
    //* \brief electron trk isolation cut */
    double ele_trkiso_max;
    //* \brief electron trk isolation cut */
    double ele_trkiso_max_antiid;
    //* \brief electron trk isolation cut */
    double ele_trkiso_min_antiid;
    //* \brief electron calo isolation cut */
    double ele_caloiso_max;
    //* \brief electron calo isolation cut */
    double ele_caloiso_max_antiid;
    //* \brief electron calo isolation cut */
    double ele_caloiso_min_antiid;
    //* \brief baseline electron identification eg "LooseLLH" */
    ElectronID::ElectronID ele_id_base;
    //* \brief signal electron identification eg "TightLLH" */
    ElectronID::ElectronID ele_id;
    //* \brief signal electron identification eg fail medium */
    ElectronID::ElectronID ele_id_fail_antiid;
    //* \brief signal electron identification eg pass loose */
    ElectronID::ElectronID ele_id_pass_antiid;

    bool ele_antiid_or_d0;
    bool ele_antiid_or_z0;
    bool ele_antiid_or_iso;
    bool ele_antiid_or_id;

    bool mu_antiid_or_d0;
    bool mu_antiid_or_z0;
    bool mu_antiid_or_iso;

    double zele_ptmin;
    double zele_d0_max;
    double zele_z0_max;
    AnaIso::AnaIso zele_isowp;
    AnaIso::AnaIso zele_trkisotype;
    AnaIso::AnaIso zele_caloisotype;
    double zele_trkiso_max;
    double zele_caloiso_max;
    ElectronID::ElectronID zele_id;

    //* \brief muon d0 significance cut */
    double mu_d0_max;
    //* \brief muon d0 significance cut */
    double mu_d0_max_antiid;
    //* \brief muon d0 significance cut */
    double mu_d0_min_antiid;
    //* \brief muon z0sintheta cut */
    double mu_z0_max;
    //* \brief muon z0sintheta cut */
    double mu_z0_max_antiid;
    //* \brief muon z0sintheta cut */
    double mu_z0_min_antiid;
    //* \brief muon iso WP (will set for trk and calo. Overrides all other isolation settings.)*/
    AnaIso::AnaIso mu_isowp;
    //
    AnaIso::AnaIso mu_isowp_base;
    //
    AnaIso::AnaIso mu_isowp_fail_antiid;
    //
    AnaIso::AnaIso mu_isowp_pass_antiid;
    //* \brief muon trk isolation flavor. Overridden by isowp. */
    AnaIso::AnaIso mu_trkisotype;
    //* \brief muon calo isolation flavor. Overridden by isowp. */
    AnaIso::AnaIso mu_caloisotype;
    //* \brief muon trk isolation cut */
    double mu_trkiso_max;
    //* \brief muon trk isolation cut */
    double mu_trkiso_max_antiid;
    //* \brief muon trk isolation cut */
    double mu_trkiso_min_antiid;
    //* \brief muon calo isolation cut */
    double mu_caloiso_max;
    //* \brief muon calo isolation cut */
    double mu_caloiso_max_antiid;
    //* \brief muon calo isolation cut */
    double mu_caloiso_min_antiid;

    double zmu_ptmin;
    double zmu_d0_max;
    double zmu_z0_max;
    AnaIso::AnaIso zmu_isowp;
    AnaIso::AnaIso zmu_trkisotype;
    AnaIso::AnaIso zmu_caloisotype;
    double zmu_trkiso_max;
    double zmu_caloiso_max;

    // Muon ID variables
    xAOD::Muon::Quality mu_id_base;
    xAOD::Muon::Quality mu_id_pass_antiid;
    xAOD::Muon::Quality mu_id;
    xAOD::Muon::Quality zmu_id;

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
    //* \brief jet central jvf ptmax
    double jet_central_jvfpt_max;
    //* \brief Max central (non-bjet) mv1 */
    double jet_central_mv1;

    //* \brief turn on or off overlap removal */
    bool do_overlapremove;

    bool m_skipsf;
    void set_skipsf(bool b){m_skipsf = b;};

    bool init();
    bool CheckIsolationConfigurationValidity();

    bool SignalLeptonPassesAdditionalSignalCuts(int iptordered);
    bool SignalLeptonPassesAdditionalZBosonCuts(int iptordered);
    bool SignalLeptonPassesAntiIDCuts(int iptordered);
    
    bool IsSignalElectronLocal(int icontainer);
    bool IsZBosonElectronLocal(int icontainer);
    bool IsSignalMuonLocal(int icontainer);
    bool IsZBosonMuonLocal(int icontainer);

    bool IsAntiIDElectron(int icontainer);
    bool IsAntiIDMuon(int icontainer);

    void set_ele_ptmin_base        (double d){ele_ptmin_base         = d;};
    void set_ele_ptmin             (double d){ele_ptmin              = d;};
    void set_ele_ptmin_antiid      (double d){ele_ptmin_antiid       = d;};
    void set_mu_ptmin_base         (double d){mu_ptmin_base          = d;};
    void set_mu_ptmin              (double d){mu_ptmin               = d;};
    void set_mu_ptmin_antiid       (double d){mu_ptmin_antiid        = d;};
    void set_tau_ptmin             (double d){tau_ptmin              = d;};
    void set_jet_ptmin             (double d){jet_ptmin              = d;};
    void set_ele_eta_max           (double d){ele_eta_max            = d;};
    void set_mu_eta_max            (double d){mu_eta_max             = d;};
    void set_tau_eta_max           (double d){tau_eta_max            = d;};
    void set_jet_eta_max           (double d){jet_eta_max            = d;};
    void set_ele_d0_max            (double d){ele_d0_max             = d;};
    void set_ele_d0_max_antiid     (double d){ele_d0_max_antiid      = d;};
    void set_ele_d0_min_antiid     (double d){ele_d0_min_antiid      = d;};
    void set_ele_z0_max            (double d){ele_z0_max             = d;};
    void set_ele_z0_max_antiid     (double d){ele_z0_max_antiid      = d;};
    void set_ele_z0_min_antiid     (double d){ele_z0_min_antiid      = d;};
    void set_ele_isowp        (std::string s){ele_isowp = ConvertToAnaIso(s);};
    void set_ele_isowp_base   (std::string s){ele_isowp_base = ConvertToAnaIso(s);};
    void set_ele_isowp_fail_antiid (std::string s){ele_isowp_fail_antiid = ConvertToAnaIso(s);};
    void set_ele_isowp_pass_antiid (std::string s){ele_isowp_pass_antiid = ConvertToAnaIso(s);};
    void set_ele_trkisotype   (std::string s){ele_trkisotype = ConvertToAnaIso(s);};
    void set_ele_caloisotype  (std::string s){ele_caloisotype = ConvertToAnaIso(s);};
    void set_ele_trkiso_max        (double d){ele_trkiso_max         = d;};
    void set_ele_trkiso_max_antiid (double d){ele_trkiso_max_antiid  = d;};
    void set_ele_trkiso_min_antiid (double d){ele_trkiso_min_antiid  = d;};
    void set_ele_caloiso_max       (double d){ele_caloiso_max        = d;};
    void set_ele_caloiso_max_antiid(double d){ele_caloiso_max_antiid = d;};
    void set_ele_caloiso_min_antiid(double d){ele_caloiso_min_antiid = d;};
    void set_ele_id_base      (std::string s){ele_id_base       = ConvertToElectronID(s);};
    void set_ele_id           (std::string s){ele_id            = ConvertToElectronID(s);};
    void set_ele_id_fail_antiid(std::string s){ele_id_fail_antiid = ConvertToElectronID(s);};
    void set_ele_id_pass_antiid(std::string s){ele_id_pass_antiid = ConvertToElectronID(s);};

    void set_ele_antiid_or_d0      (double d){ele_antiid_or_d0  = (bool)d;};
    void set_ele_antiid_or_z0      (double d){ele_antiid_or_z0  = (bool)d;};
    void set_ele_antiid_or_iso     (double d){ele_antiid_or_iso = (bool)d;};
    void set_ele_antiid_or_id      (double d){ele_antiid_or_id  = (bool)d;};

    void set_mu_antiid_or_d0       (double d){mu_antiid_or_d0   = (bool)d;};
    void set_mu_antiid_or_z0       (double d){mu_antiid_or_z0   = (bool)d;};
    void set_mu_antiid_or_iso      (double d){mu_antiid_or_iso  = (bool)d;};

    void set_zele_ptmin            (double d){zele_ptmin       = d;};
    void set_zele_d0_max           (double d){zele_d0_max      = d;};
    void set_zele_z0_max           (double d){zele_z0_max      = d;};
    void set_zele_isowp       (std::string s){zele_isowp       = ConvertToAnaIso(s);};
    void set_zele_trkisotype  (std::string s){zele_trkisotype  = ConvertToAnaIso(s);};
    void set_zele_caloisotype (std::string s){zele_caloisotype = ConvertToAnaIso(s);};
    void set_zele_trkiso_max       (double d){zele_trkiso_max  = d;};
    void set_zele_caloiso_max      (double d){zele_caloiso_max = d;};
    void set_zele_id          (std::string s){zele_id          = ConvertToElectronID(s);};

    void set_mu_d0_max             (double d){mu_d0_max              = d;};
    void set_mu_d0_max_antiid      (double d){mu_d0_max_antiid       = d;};
    void set_mu_d0_min_antiid      (double d){mu_d0_min_antiid       = d;};
    void set_mu_z0_max             (double d){mu_z0_max              = d;};
    void set_mu_z0_max_antiid      (double d){mu_z0_max_antiid       = d;};
    void set_mu_z0_min_antiid      (double d){mu_z0_min_antiid       = d;};
    void set_mu_isowp         (std::string s){mu_isowp = ConvertToAnaIso(s);};
    void set_mu_isowp_base    (std::string s){mu_isowp_base = ConvertToAnaIso(s);};
    void set_mu_isowp_fail_antiid(std::string s){mu_isowp_fail_antiid = ConvertToAnaIso(s);};
    void set_mu_isowp_pass_antiid(std::string s){mu_isowp_pass_antiid = ConvertToAnaIso(s);};
    void set_mu_trkisotype    (std::string s){mu_trkisotype = ConvertToAnaIso(s);};
    void set_mu_caloisotype   (std::string s){mu_caloisotype = ConvertToAnaIso(s);};
    void set_mu_trkiso_max         (double d){mu_trkiso_max          = d;};
    void set_mu_trkiso_max_antiid  (double d){mu_trkiso_max_antiid   = d;};
    void set_mu_trkiso_min_antiid  (double d){mu_trkiso_min_antiid   = d;};
    void set_mu_caloiso_max        (double d){mu_caloiso_max         = d;};
    void set_mu_caloiso_max_antiid (double d){mu_caloiso_max_antiid  = d;};
    void set_mu_caloiso_min_antiid (double d){mu_caloiso_min_antiid  = d;};
 
    void set_zmu_ptmin             (double d){zmu_ptmin       = d;};
    void set_zmu_d0_max            (double d){zmu_d0_max      = d;};
    void set_zmu_z0_max            (double d){zmu_z0_max      = d;};
    void set_zmu_isowp        (std::string s){zmu_isowp       = ConvertToAnaIso(s);};
    void set_zmu_trkisotype   (std::string s){zmu_trkisotype  = ConvertToAnaIso(s);};
    void set_zmu_caloisotype  (std::string s){zmu_caloisotype = ConvertToAnaIso(s);};
    void set_zmu_trkiso_max        (double d){zmu_trkiso_max  = d;};
    void set_zmu_caloiso_max       (double d){zmu_caloiso_max = d;};

    // Muon ID variables
    void set_mu_id_base       (std::string s){mu_id_base    = ConvertToMuonID(s);};
    void set_mu_id            (std::string s){mu_id         = ConvertToMuonID(s);};
    void set_zmu_id           (std::string s){zmu_id        = ConvertToMuonID(s);};
    void set_mu_id_pass_antiid(std::string s){mu_id_pass_antiid = ConvertToMuonID(s);};
                                  
    void set_jet_forward_eta_min   (double d){jet_forward_eta_min    = d;};
    void set_jet_forward_eta_max   (double d){jet_forward_eta_max    = d;};
    void set_jet_forward_ptmin     (double d){jet_forward_ptmin      = d;};
    void set_jet_bjet_ptmin        (double d){jet_bjet_ptmin         = d;};
    void set_jet_bjet_mv1          (double d){jet_bjet_mv1           = d;};
    void set_jet_central_jvfcut    (double d){jet_central_jvfcut     = d;};
    void set_jet_central_jvfeta_max(double d){jet_central_jvfeta_max = d;};
    void set_jet_central_eta_max   (double d){jet_central_eta_max    = d;};
    void set_jet_central_ptmin     (double d){jet_central_ptmin      = d;};
    void set_jet_central_jvfpt_max (double d){jet_central_jvfpt_max  = d;};
    void set_jet_central_mv1       (double d){jet_central_mv1        = d;};
    void set_do_overlapremove      (bool b  ){do_overlapremove       = b;};

    ClassDef(ObjectSelectionBase, 1);
  };
}
#endif
