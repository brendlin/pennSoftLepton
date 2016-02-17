#include <pennSoftLepton/WeightSystematics.h>
#ifndef SKIP_XAOD_COMPILATION

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::WeightSystematics)

//-----------------------------------------------------------------------------
PSL::WeightSystematics::WeightSystematics() : AlgBase()
{
  MSG_DEBUG("Constructor");
}

//-----------------------------------------------------------------------------
bool PSL::WeightSystematics::init(void)
{

  if (!ParentEventSelectorName.empty()){
    ParentEventSelector = (PassEvent*)connectToAnotherAlgorithm(ParentEventSelectorName);
    if (!ParentEventSelector) {
      MSG_INFO("Error! Could not find parent PassEvent " << ParentEventSelectorName << ". Exiting");
      return false;
    }
    MSG_DEBUG("Successfully connected " << ParentEventSelectorName << " to " << GetName() << ".");
  }
  
  // Now we can look at systematics:
  const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();

  bool toys = m_EDM->IsEgammaToys();

  std::vector<std::string> systs;
  if (!toys) {
    systs.push_back("EL_EFF_ID_TotalCorrUncertainty__1down");
    systs.push_back("EL_EFF_ID_TotalCorrUncertainty__1up");
    systs.push_back("EL_EFF_Iso_TotalCorrUncertainty__1down");
    systs.push_back("EL_EFF_Iso_TotalCorrUncertainty__1up");
    systs.push_back("EL_EFF_Reco_TotalCorrUncertainty__1down");
    systs.push_back("EL_EFF_Reco_TotalCorrUncertainty__1up");
    systs.push_back("EL_EFF_TriggerEff_TotalCorrUncertainty__1down");
    systs.push_back("EL_EFF_TriggerEff_TotalCorrUncertainty__1up");
    systs.push_back("EL_EFF_Trigger_TotalCorrUncertainty__1down");
    systs.push_back("EL_EFF_Trigger_TotalCorrUncertainty__1up");
  }
  systs.push_back("MUON_EFF_STAT__1down");
  systs.push_back("MUON_EFF_STAT__1up");
  systs.push_back("MUON_EFF_STAT_LOWPT__1down");
  systs.push_back("MUON_EFF_STAT_LOWPT__1up");
  systs.push_back("MUON_EFF_SYS__1down");
  systs.push_back("MUON_EFF_SYS__1up");
  systs.push_back("MUON_EFF_SYS_LOWPT__1down");
  systs.push_back("MUON_EFF_SYS_LOWPT__1up");
  systs.push_back("MUON_EFF_TrigStatUncertainty__1down");
  systs.push_back("MUON_EFF_TrigStatUncertainty__1up");
  // systs.push_back("MUON_EFF_TrigSystUncertainty__1down");
  // systs.push_back("MUON_EFF_TrigSystUncertainty__1up");
  systs.push_back("MUON_ISO_STAT__1down");
  systs.push_back("MUON_ISO_STAT__1up");
  systs.push_back("MUON_ISO_SYS__1down");
  systs.push_back("MUON_ISO_SYS__1up");
  systs.push_back("PRW_DATASF__1down");
  systs.push_back("PRW_DATASF__1up");

  // this is the nominal set
  m_systInfoList.clear();
  m_systInfoList.reserve(recommendedSystematics.size() * 2); // allow for continuous systematics
  m_systInfoList.push_back(CP::SystematicSet());
  for (const auto& systSet : CP::make_systematics_vector(recommendedSystematics)) {
    for (const auto& sys : systSet) {
      CP::SystematicSet sysInfo;
      bool book = false;
      if (toys && sys.basename().compare(0, 19, "EL_EFF_ID_COMBMCTOY") == 0) { 
        MSG_DEBUG("It is an EFF ID COMBMCTOY!");
        book = true;
      }
      else if (toys && sys.basename().compare(0, 20, "EL_EFF_Iso_COMBMCTOY") == 0) { 
        MSG_DEBUG("It is an EFF Iso COMBMCTOY!");
        book = true;
      }
      else if (toys && sys.basename().compare(0, 21, "EL_EFF_Reco_COMBMCTOY") == 0) { 
        MSG_DEBUG("It is an EFF Reco COMBMCTOY!");
        book = true;
      }
      else if (toys && sys.basename().compare(0, 27, "EL_EFF_TriggerEff_COMBMCTOY") == 0) { 
        MSG_DEBUG("It is an EFF TriggerEff COMBMCTOY!");
        book = true;
      }
      else if (toys && sys.basename().compare(0, 24, "EL_EFF_Trigger_COMBMCTOY") == 0) { 
        MSG_DEBUG("It is an EFF TriggerEff COMBMCTOY!");
        book = true;
      }
      else {
        for (auto syststring : systs) {
          if (std::string(sys.name()).find(syststring) != std::string::npos) {
            MSG_DEBUG("It is in our list!");
            book = true;
            break;
          }
        }
      }

      if (book) {
        sysInfo.insert(sys);
        MSG_DEBUG("Booking Variation " << sys.name());
        m_systInfoList.push_back(sysInfo);
      } else{
        MSG_DEBUG("Skipping " << sys.name());
      }

    }
  }
  
  m_systInfoList.push_back(CP::SystematicSet());
  MSG_INFO("Returning list of " << m_systInfoList.size() << " systematic variations");

  syst_hist = new TH1F((std::string(GetName())+"_syst_hist").c_str(),(std::string(GetName())+"_syst_hist").c_str(),m_systInfoList.size(),0,m_systInfoList.size());
  setHistogramOutput(syst_hist);

  for (unsigned int i=0;i<m_systInfoList.size();++i){
    if (m_systInfoList[i].empty()) {
      syst_hist->GetXaxis()->SetBinLabel(i+1,"NOMINAL");
    } else{
      for (auto syst : m_systInfoList.at(i)) {
        syst_hist->GetXaxis()->SetBinLabel(i+1,syst.name().c_str());
      }
    }
  }

  PrintConfiguration();
  return true;
}

//-----------------------------------------------------------------------------
void PSL::WeightSystematics::loop(void){
  MSG_DEBUG("loop");
  m_currenteventnumber = m_EDM->get_EventNumber();
  m_eventpassed = 0;
  if (ParentEventSelector){
    // Hacky thing to make sure the algorithms are talking about the same event.
    MSG_DEBUG("Checking parent event selector");
    if (!ParentEventSelector->passed(m_EDM->get_EventNumber())) return;
  }
  m_eventpassed = 1;
  double weight = m_EDM->GetEventWeight();
  MSG_DEBUG("Starting weight: " << weight);
  // std::string tmp;
  
  MSG_DEBUG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  for (unsigned int i=0;i<m_systInfoList.size();++i){
    // tmp = "";
    // if (m_systInfoList[i].empty()) {
    //   tmp = "EMPTY Variation";
    // } else{
    //   for (auto syst : m_systInfoList.at(i)) {
    //     tmp += syst.name();
    //   }
    // }
    // Reset
    m_EDM->applySystematicVariation(CP::SystematicSet());
    // Apply new systematic
    m_EDM->applySystematicVariation(m_systInfoList[i]);
    weight = m_EDM->GetEventWeightSpecial();
    // MSG_DEBUG(tmp << ": " << weight);
    syst_hist->Fill(double(i),weight);
  }
  MSG_DEBUG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  return;
}

//-----------------------------------------------------------------------------
void PSL::WeightSystematics::finish(void){
  MSG_DEBUG("finish");
  return;
}

//-----------------------------------------------------------------------------
void PSL::WeightSystematics::PrintConfiguration(void){
  MSG_INFO("-------------------------------PassEvent Configuration:--------------------------------");
  MSG_INFO("This Instance Name      : " << GetName()              );
  MSG_INFO("ParentEventSelectorName : " << ParentEventSelectorName);
  MSG_INFO("EgammaToys (set in xAODWrapper) : " << m_EDM->IsEgammaToys());
  std::string tmp;
  for (unsigned int i=0;i<m_systInfoList.size();++i){
    tmp = "";
    if (m_systInfoList.at(i).empty()) tmp += "EMPTY";
    else {
      for (auto syst : m_systInfoList.at(i)) {
        tmp += syst.name() + " ";
      }
    }
    if (i == 0) {
      MSG_INFO("Systematics             : " << tmp);
    } else {
      MSG_INFO("                          " << tmp);
    }
  }
  MSG_INFO("---------------------------------------------------------------------------------------");
}

#endif // SKIP_XAOD_COMPILATION
