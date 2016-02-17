#include "pennSoftLepton/HelpersXaod.h"
#ifndef SKIP_XAOD_COMPILATION

#include "pennSoftLepton/Messaging.h"
#include "xAODRootAccess/TEvent.h"

#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"

//-----------------------------------------------------------------------------
std::vector<double> PSL::GetDerivationWeightInfo(TFile* file,std::string derivationName){

  std::vector<double> ret;
  ret.push_back(-1.);
  ret.push_back(-1.);
  ret.push_back(-1.);
  ret.push_back(-1.);
  
  // TEventSvc *const svc = dynamic_cast<TEventSvc*>(getAlg (TEventSvc::name));
  // if (svc == 0) MSG_INFO("Job not configured for xAOD support");
  // svc->event();
  //TEvent* m_event = svc->event();
  xAOD::TEvent* m_event = new xAOD::TEvent(xAOD::TEvent::kBranchAccess);
  if (!m_event->readFrom(file).isSuccess()) {
    MSG_INFO("failed to read from xAOD");
    return ret;
  }

  // get the MetaData tree once a new file is opened, with
  TTree *MetaData = (TTree*)file->Get("MetaData");
  if (!MetaData) {
    MSG_INFO("Error! File does not have Derivation Framework skim metadata!");
    return ret;
  }
  MetaData->LoadTree(0);
  
  //check if file is from a DxAOD
  bool m_isDerivation = !MetaData->GetBranch("StreamAOD");
  
  if(!m_isDerivation) {
    MSG_INFO("Error! File metadata does not have StreamAOD branch!");
    return ret;
  }

  const xAOD::CutBookkeeperContainer* incompleteCBC = nullptr;
  if(!m_event->retrieveMetaInput(incompleteCBC, "IncompleteCutBookkeepers").isSuccess()){
    Error("initializeEvent()","Failed to retrieve IncompleteCutBookkeepers from MetaData! Exiting.");
    return ret;
  }
  if ( incompleteCBC->size() != 0 ) {
    Error("initializeEvent()","Found incomplete Bookkeepers! Check file for corruption.");
    return ret;
  }
  // Now, let's find the actual information
  const xAOD::CutBookkeeperContainer* completeCBC = 0;
  if(!m_event->retrieveMetaInput(completeCBC, "CutBookkeepers").isSuccess()){
    Error("initializeEvent()","Failed to retrieve CutBookkeepers from MetaData! Exiting.");
    return ret;
  }
  
  // First, let's find the smallest cycle number,
  // i.e., the original first processing step/cycle
  int minCycle = 10000;
  for ( auto cbk : *completeCBC ) {
    if ( ! cbk->name().empty()  && minCycle > cbk->cycle() ){ minCycle = cbk->cycle(); }
    //if ( ! cbk->name().empty()) MSG_INFO(cbk->name());
  }
  // Now, let's actually find the right one that contains all the needed info...
  const xAOD::CutBookkeeper* allEventsCBK=0;
  const xAOD::CutBookkeeper* DxAODEventsCBK=0;
  for ( auto cbk :  *completeCBC ) {
    if ( minCycle == cbk->cycle() && cbk->name() == "AllExecutedEvents" ){
      allEventsCBK = cbk;
    }
    if ( cbk->name() == derivationName){
      DxAODEventsCBK = cbk;
    }
    if ( cbk->name() == derivationName+"Skim"){
      DxAODEventsCBK = cbk;
    }
  }

  if (!allEventsCBK) {
    MSG_INFO("Error! Failed to find AllExecutedEvents CutBookkeeper.");
    return ret;
  }

  if (!DxAODEventsCBK) {
    MSG_INFO("Error! Failed to find " << derivationName << " CutBookkeeper.");
    return ret;
  }

  uint64_t nEventsProcessed  = allEventsCBK->nAcceptedEvents();
  double sumOfWeights        = allEventsCBK->sumOfEventWeights();
  //double sumOfWeightsSquared = allEventsCBK->sumOfEventWeightsSquared();
  
  uint64_t nEventsDxAOD           = DxAODEventsCBK->nAcceptedEvents();
  double sumOfWeightsDxAOD        = DxAODEventsCBK->sumOfEventWeights();
  //double sumOfWeightsSquaredDxAOD = DxAODEventsCBK->sumOfEventWeightsSquared();

  ret.clear();
  ret.push_back(sumOfWeights);
  ret.push_back(nEventsDxAOD);
  ret.push_back(nEventsProcessed);
  ret.push_back(sumOfWeightsDxAOD);
  return ret;
}

#ifndef ISREL20
namespace xAOD {
  
  namespace TruthHelpers {
    
    /// @param p The particle that we find the associated truth particle for                          
    /// @returns A pointer to the associated truth particle if available,                             
    ///          or a null pointer if not                                                             
    ///                                                                                               
    const xAOD::TruthParticle* getTruthParticle( const xAOD::IParticle& p ) {
      
      /// A convenience type declaration                                                             
      typedef ElementLink< xAOD::TruthParticleContainer > Link_t;
      
      /// A static accessor for the information                                                      
      static SG::AuxElement::ConstAccessor< Link_t > acc( "truthParticleLink" );
      
      // Check if such a link exists on the object:                                                  
      if( ! acc.isAvailable( p ) ) {
        return 0;
      }

      // Get the link:                                                                               
      const Link_t& link = acc( p );

      // Check if the link is valid:                                                                 
      if( ! link.isValid() ) {
        return 0;
      }

      // Everything has passed, let's return the pointer:                                            
      return *link;
    }
  }

  namespace EgammaHelpers {
    const xAOD::TruthParticle* getBkgElectronMother(const xAOD::Electron* el){
      const xAOD::TruthParticle *truthel = xAOD::TruthHelpers::getTruthParticle(*el);
      return getBkgElectronMother(truthel);
    }

    const xAOD::TruthParticle* getBkgElectronMother(const xAOD::TruthParticle* truthel){
      
      //Truth must exist and be an electron
      if (!truthel || truthel->absPdgId()!=11){
        return 0;
      }
      ///
      // The first parent has to exist
      if (!truthel->nParents()){
        return 0;
      }
      //And has to be a photon or electron
      const xAOD::TruthParticle* parent = truthel->parent();
      if(parent->absPdgId() !=22 && parent->absPdgId() !=11){
        return 0;
      }
      while (parent->nParents()){ //Loop over the generations
        const xAOD::TruthParticle* tmp = parent->parent();
        //You want to see an electron or a photon
        if(tmp->absPdgId() ==22 || tmp->absPdgId() ==11){
          parent=tmp;
        }
        else{ // if we do not see any more electron and photons we stop
          break ;
        }
      }
      return parent;
    }
  }
}
#endif // not ISREL20

#endif // SKIP_XAOD_COMPILATION
