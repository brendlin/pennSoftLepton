#include "pennSoftLepton/switch_d3pd_xaod.h"
#ifndef SKIP_XAOD_COMPILATION

#ifndef pennSoftLepton_HelpersXaod_H
#define pennSoftLepton_HelpersXaod_H

#include "TFile.h"
#include "TTree.h"

#ifndef ISREL20
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"
#include "xAODBase/IParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEgamma/Electron.h"
#endif // not ISREL20

namespace PSL
{
  std::vector<double> GetDerivationWeightInfo(TFile* file,std::string derivationName="SUSY2Kernel");
}

//
// This is temporary, so we can have truth matching on dc14
//
#ifndef ISREL20
namespace xAOD {

  // Forward declaration(s):                                                                           
  class IParticle;

  /// Dedicated namespace for the helper functions                                                     
  namespace TruthHelpers {

    /// Return the truthParticle associated to the given IParticle (if any)                           
    const TruthParticle* getTruthParticle( const xAOD::IParticle& p );

  } // namespace TruthHelpers                                                                          

  namespace EgammaHelpers {
    const xAOD::TruthParticle* getBkgElectronMother(const xAOD::Electron* el);
    const xAOD::TruthParticle* getBkgElectronMother(const xAOD::TruthParticle* truthel);
  }

} // namespace xAOD
#endif // not ISREL20

#endif // pennSoftLepton_HelpersXaod_H

#endif // SKIP_XAOD_COMPILATION
