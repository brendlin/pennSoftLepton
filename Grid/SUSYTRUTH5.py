#==============================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# ATTACH THE RECONSTRUCTION TO THE SEQUENCER  
#====================================================================

#from DerivationFrameworkJetEtMiss.JetEtMissCommon import *
# Add translator from EVGEN input to xAOD-like truth here
from RecExConfig.ObjKeyStore import objKeyStore
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
    DerivationFrameworkJob += xAODMaker__xAODTruthCnvAlg("GEN_EVNT2xAOD",AODContainerName="GEN_EVENT")

from JetRec.JetFlavorAlgs import scheduleCopyTruthParticles
scheduleCopyTruthParticles(DerivationFrameworkJob)
# Set jet flags
from JetRec.JetRecFlags import jetFlags
jetFlags.useTruth = True
jetFlags.useTracks = False
# Add jet algorithms
from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm
# Standard truth jets
akt4 = jtm.addJetFinder("AntiKt4TruthJets", "AntiKt", 0.4, "truth", ptmin= 25000)
akt4alg = JetAlgorithm("jetalgAntiKt4TruthJets", Tools = [akt4] )
DerivationFrameworkJob += akt4alg

# WZ Truth Jets
#jtm.addJetFinder("AntiKt4TruthWZJets",  "AntiKt", 0.4,  "truthwz", ptmin= 5000)
#jtm.addJetFinder("AntiKt6TruthWZJets",  "AntiKt", 0.6,  "truthwz", ptmin= 5000)
# Other jets
#akt6  = jtm.addJetFinder("AntiKt6TruthJets", "AntiKt", 0.6, "truth", ptmin= 5000)
akt10 = jtm.addJetFinder("AntiKt10TruthJets", "AntiKt", 1.0, "truth", ptmin= 100000)
akt10alg = JetAlgorithm("jetalgAntiKt10TruthJets", Tools = [akt10] )
DerivationFrameworkJob += akt10alg
akt10trim = jtm.addJetTrimmer("TrimmedAntiKt10TruthJets", rclus=0.3, ptfrac=0.05, input='AntiKt10TruthJets')
akt10trimalg = JetAlgorithm("jetalgTrimmedAntiKt10TruthJets", Tools = [akt10trim] )
DerivationFrameworkJob += akt10trimalg

# Add truth-based MET algorithm here
import METReconstruction.METConfig_Truth
from METReconstruction.METRecoFlags import metFlags # not sure if you even need this line
from METReconstruction.METRecoConfig import getMETRecoAlg
metAlg = getMETRecoAlg('METReconstruction')
DerivationFrameworkJob += metAlg

# Add skimming configuration for truth!

#==============================================================================
# Schedule the tool for adding new truth collection
#==============================================================================

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker

SUSYTRUTH5MuonTool = DerivationFramework__TruthCollectionMaker(name                   = "SUSYTRUTH5MuonTool",
                                                         NewCollectionName       = "TruthMuons",
                                                         ParticleSelectionString = "abs(TruthParticle.pdgId) == 13 && TruthParticle.status==1")
ToolSvc += SUSYTRUTH5MuonTool
SUSYTRUTH5ElectronTool = DerivationFramework__TruthCollectionMaker(name               = "SUSYTRUTH5ElectronTool",
                                                         NewCollectionName       = "TruthElectrons",
                                                         ParticleSelectionString = "abs(TruthParticle.pdgId) == 11 && TruthParticle.status==1")
ToolSvc += SUSYTRUTH5ElectronTool
SUSYTRUTH5PhotonTool = DerivationFramework__TruthCollectionMaker(name                 = "SUSYTRUTH5PhotonTool",
                                                         NewCollectionName       = "TruthPhotons",
                                                         ParticleSelectionString = "abs(TruthParticle.pdgId) == 22 && TruthParticle.pt > 50.*GeV")
ToolSvc += SUSYTRUTH5PhotonTool
SUSYTRUTH5TauTool = DerivationFramework__TruthCollectionMaker(name                    = "SUSYTRUTH5TauTool",
                                                         NewCollectionName       = "TruthTaus",
                                                         ParticleSelectionString = "abs(TruthParticle.pdgId) == 15")
ToolSvc += SUSYTRUTH5TauTool

SUSYTRUTH5BosonTool = DerivationFramework__TruthCollectionMaker(name                    = "SUSYTRUTH5BosonTool",
                                                         NewCollectionName       = "TruthBoson",
                                                         ParticleSelectionString = "abs(TruthParticle.pdgId) == 24 || abs(TruthParticle.pdgId) == 25 || abs(TruthParticle.pdgId) == 23")
ToolSvc += SUSYTRUTH5BosonTool

SUSYTRUTH5TopTool = DerivationFramework__TruthCollectionMaker(name                    = "SUSYTRUTH5TopTool",
                                                         NewCollectionName       = "TruthTop",
                                                         ParticleSelectionString = "abs(TruthParticle.pdgId) == 6");
ToolSvc += SUSYTRUTH5TopTool

SUSYTRUTH5SUSYTool = DerivationFramework__TruthCollectionMaker(name                    = "SUSYTRUTH5SUSYTool",
                                                         NewCollectionName       = "TruthSUSY",
                                                         ParticleSelectionString = "((abs(TruthParticle.pdgId) >= 1000001) && (abs(TruthParticle.pdgId) <= 1000039))");
ToolSvc += SUSYTRUTH5SUSYTool

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthIsolationTool
SUSYTRUTH5ElectronIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "SUSYTRUTH5ElectronIsolationTool1",
                                                                  particlesKey = "TruthElectrons",
                                                                  particleIDsToCalculate = [11],
                                                                  IsolationConeSize = 0.2,
                                                                  IsolationVarName = 'EtCone20',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += SUSYTRUTH5ElectronIsolationTool1
SUSYTRUTH5ElectronIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "SUSYTRUTH5ElectronIsolationTool2",
                                                                  particlesKey = "TruthElectrons",
                                                                  particleIDsToCalculate = [11],
                                                                  IsolationConeSize = 0.3,
                                                                  IsolationVarName = 'PtCone30',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += SUSYTRUTH5ElectronIsolationTool2
SUSYTRUTH5MuonIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "SUSYTRUTH5MuonIsolationTool1",
                                                                  particlesKey = "TruthMuons",
                                                                  particleIDsToCalculate = [13],
                                                                  IsolationConeSize = 0.2,
                                                                  IsolationVarName = 'EtCone20',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += SUSYTRUTH5MuonIsolationTool1
SUSYTRUTH5MuonIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "SUSYTRUTH5MuonIsolationTool2",
                                                                  particlesKey = "TruthMuons",
                                                                  particleIDsToCalculate = [13],
                                                                  IsolationConeSize = 0.3,
                                                                  IsolationVarName = 'PtCone30',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += SUSYTRUTH5MuonIsolationTool2
SUSYTRUTH5PhotonIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "SUSYTRUTH5PhotonIsolationTool1",
                                                                  particlesKey = "TruthPhotons",
                                                                  particleIDsToCalculate = [22],
                                                                  IsolationConeSize = 0.2,
                                                                  IsolationVarName = 'EtCone20',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += SUSYTRUTH5PhotonIsolationTool1
SUSYTRUTH5PhotonIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "SUSYTRUTH5PhotonIsolationTool2",
                                                                  particlesKey = "TruthPhotons",
                                                                  particleIDsToCalculate = [22],
                                                                  IsolationConeSize = 0.3,
                                                                  IsolationVarName = 'PtCone30',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += SUSYTRUTH5PhotonIsolationTool2

#==============================================================================
# Create the derivation kernel algorithm
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("SUSYTRUTH5Kernel",
                                                                        AugmentationTools = [SUSYTRUTH5MuonTool,SUSYTRUTH5ElectronTool,SUSYTRUTH5PhotonTool,SUSYTRUTH5TauTool,
                                                                                             SUSYTRUTH5TopTool,SUSYTRUTH5BosonTool,SUSYTRUTH5SUSYTool,
                                                                                             SUSYTRUTH5ElectronIsolationTool1, SUSYTRUTH5ElectronIsolationTool2,
                                                                                             SUSYTRUTH5MuonIsolationTool1, SUSYTRUTH5MuonIsolationTool2,
                                                                                             SUSYTRUTH5PhotonIsolationTool1, SUSYTRUTH5PhotonIsolationTool2,
                                                                                             ])
# Now we add the sequencer to the job
#DerivationFrameworkJob += EvgenAODSequence

#==============================================================================
# Set up stream
#==============================================================================
streamName = derivationFlags.WriteDAOD_SUSYTRUTH5Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_SUSYTRUTH5Stream )
SUSYTRUTH5Stream = MSMgr.NewPoolRootStream( streamName, fileName )

# Only events that pass the filters listed are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
SUSYTRUTH5Stream.AcceptAlgs(['SUSYTRUTH5Kernel'])

#==============================================================================
# Set up slimming content list here
#==============================================================================
SUSYTRUTH5Stream.AddItem("xAOD::EventInfo_v1#*")
SUSYTRUTH5Stream.AddItem("xAOD::EventAuxInfo_v1#*")
SUSYTRUTH5Stream.AddItem("xAOD::JetContainer_v1#TrimmedAntiKt10TruthJets*")
SUSYTRUTH5Stream.AddItem("xAOD::JetAuxContainer_v1#TrimmedAntiKt10TruthJets*")
SUSYTRUTH5Stream.AddItem("xAOD::JetContainer_v1#AntiKt4TruthJets*")
SUSYTRUTH5Stream.AddItem("xAOD::JetAuxContainer_v1#AntiKt4TruthJets*")
SUSYTRUTH5Stream.AddItem("xAOD::MissingETContainer_v1#*")
SUSYTRUTH5Stream.AddItem("xAOD::MissingETAuxContainer_v1#*")

#SUSYTRUTH5Stream.AddItem( "xAOD::TruthEventContainer_v1#*" )
#SUSYTRUTH5Stream.AddItem( "xAOD::TruthEventAuxContainer_v1#*" )

#SUSYTRUTH5Stream.AddItem( "xAOD::TruthVertexContainer_v1#*" )
#SUSYTRUTH5Stream.AddItem( "xAOD::TruthVertexAuxContainer_v1#*" )
#SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleContainer_v1#*" )
#SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer_v1#*" )
SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleContainer_v1#TruthPhotons*")
SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleContainer_v1#TruthElectrons*")
SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleContainer_v1#TruthMuons*")
SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleContainer_v1#TruthTaus*")
SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleContainer_v1#TruthTop*")
SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleContainer_v1#TruthBoson*")
SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleContainer_v1#TruthSUSY*")

SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer_v1#TruthPhotons*")
SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer_v1#TruthElectrons*")
SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer_v1#TruthMuons*")
SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer_v1#TruthTaus*")
SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer_v1#TruthTop*")
SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer_v1#TruthBoson*")
SUSYTRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer_v1#TruthSUSY*")
