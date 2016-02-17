#********************************************************************
# SUSY5.py test derivation targeting stop1L
# reductionConf flag SUSY5 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *

from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkJetEtMiss.SUSYCustomMET import *

#====================================================================                                              
# Special jets
#====================================================================                                                                                                                                       
from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm
from JetRec.JetRecFlags import jetFlags

#DerivationFrameworkJob += addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.3, ptfrac=0.05, includePreTools=True)
#if jetFlags.useTruth: 
#	DerivationFrameworkJob += addTrimmedJets("AntiKt", 1.0, "Truth", rclus=0.3, ptfrac=0.05)

# includePreTools=False
# if jetFlags.useTruth:
# 	includePreTools=True
# 	DerivationFrameworkJob += addTrimmedJets("AntiKt", 1.0, "Truth", rclus=0.3, ptfrac=0.05, includePreTools=True)
# if includePreTools==False:
# 	DerivationFrameworkJob += addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.3, ptfrac=0.05, includePreTools=True)
# else:
# 	DerivationFrameworkJob += addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.3, ptfrac=0.05)
    
#====================================================================
# THINNING TOOLS 
#====================================================================
thinningTools=[]
# MET/Jet tracks
thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
SUSY5MetTPThinningTool = DerivationFramework__TrackParticleThinning( name               = "SUSY5MetTPThinningTool",
                                                                ThinningService         = "SUSY5ThinningSvc",
                                                                SelectionString         = thinning_expression,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += SUSY5MetTPThinningTool
thinningTools.append(SUSY5MetTPThinningTool)

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
SUSY5JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "SUSY5JetTPThinningTool",
                                                                ThinningService         = "SUSY5ThinningSvc",
                                                                JetKey                  = "AntiKt4LCTopoJets",
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += SUSY5JetTPThinningTool
thinningTools.append(SUSY5JetTPThinningTool)

# TrackParticles directly
SUSY5TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY5TPThinningTool",
                                                                 ThinningService         = "SUSY5ThinningSvc",
                                                                 SelectionString         = "InDetTrackParticles.pt > 10*GeV",
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY5TPThinningTool
thinningTools.append(SUSY5TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY5MuonTPThinningTool",
                                                                         ThinningService         = "SUSY5ThinningSvc",
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY5MuonTPThinningTool
thinningTools.append(SUSY5MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY5ElectronTPThinningTool",
                                                                                 ThinningService        = "SUSY5ThinningSvc",
                                                                                 SGKey             	= "ElectronCollection",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY5ElectronTPThinningTool
thinningTools.append(SUSY5ElectronTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY5TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY5TauTPThinningTool",
                                                                        ThinningService = "SUSY5ThinningSvc",
                                                                        TauKey          = "TauRecContainer",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY5TauTPThinningTool
thinningTools.append(SUSY5TauTPThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
truth_cond_WZH    = "((abs(TruthParticle.pdgId) >= 23) && (abs(TruthParticle.pdgId) <= 25))"		# W, Z and Higgs
truth_cond_Lepton = "((abs(TruthParticle.pdgId) >= 11) && (abs(TruthParticle.pdgId) <= 16))"		# Leptons
truth_cond_Quark  = "((abs(TruthParticle.pdgId) ==  6))"				     		# Top quark
truth_cond_Photon = "((abs(TruthParticle.pdgId) == 22) && (TruthParticle.pt > 10000.))"       		# Photon
truth_cond_SUSY = "((abs(TruthParticle.pdgId) >= 1000001) && (abs(TruthParticle.pdgId) <= 1000039))"	# SUSY particles

truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark +' || '+truth_cond_Photon+' || '+truth_cond_SUSY+')'

#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
#SUSY5TruthThinningTool = DerivationFramework__GenericTruthThinning( name		     = "SUSY5TruthThinningTool",
#								    ThinningService	   = "SUSY5ThinningSvc",
#								    ParticleSelectionString = truth_expression,
#								    PreserveDescendants     = True,
#								    PreserveAncestors      = False)

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
SUSY5TruthThinningTool = DerivationFramework__MenuTruthThinning(name                       = "SUSY5TruthThinningTool",
                                                     ThinningService            = "SUSY5ThinningSvc",
                                                     WritePartons               = False,
                                                     WriteHadrons               = False,
                                                     WriteBHadrons              = True,
                                                     WriteGeant                 = False,
                                                     GeantPhotonPtThresh        = 20000,
                                                     WriteTauHad                = True,
                                                     PartonPtThresh             = -1.0,
                                                     WriteBSM                   = True,
                                                     WriteBosons                = True,
						     WriteBosonProducts = True,
                                                     WriteBSMProducts           = True,
                                                     WriteTopAndDecays          = True,
                                                     WriteEverything            = False,
                                                     WriteAllLeptons            = False,
                                                     WriteLeptonsNotFromHadrons = False,
                                                     WriteStatus3               = False,
                                                     WriteFirstN                = -1,
								PreserveAncestors = True,
								PreserveGeneratorDescendants = False)

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='geant4':
	ToolSvc += SUSY5TruthThinningTool
	thinningTools.append(SUSY5TruthThinningTool)
	
#====================================================================
# SKIMMING TOOL 
#====================================================================
muonsRequirements = '(Muons.pt >= 5.*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonGoodMuon)'
electronsRequirements = '(DFCommonElectrons_pt > 5.*GeV) && (abs(DFCommonElectrons_eta) < 2.47)'
#jetsRequirements = '(abs(AntiKt4LCTopoJets.pt) >= 10.*GeV)'
objectSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+') >= 2)'
#objectSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+') >= 1) && (count('+jetsRequirements+') >=2 )'

beamEnergy = jobproperties.Beam.energy()
expression = ''
if(beamEnergy < 4.1e+06):
	mcSelection = '(EventInfo.eventTypeBitmask==1)'
	muonTrig = '(EF_mu24i_tight || EF_mu36_tight || EF_2mu13 || EF_mu18_tight_e7_medium1 || EF_mu18_tight_mu8_EFFS )'
	egammaTrig = '(EF_e24vhi_medium1 || EF_e60_medium1 || EF_2e12Tvh_loose1 || EF_e12Tvh_medium1_mu8 || EF_e24vh_medium1_e7_medium1 )'
	xeTrig = '(EF_xe80_tclcw_loose || EF_xe80T_tclcw_loose)'
	expression = '('+mcSelection+' || '+muonTrig+' || '+egammaTrig+' || '+xeTrig+') && '+objectSelection

if(beamEnergy > 6.0e+06):
# 13 TeV MC
	muonTrig = '(HLT_mu24 || HLT_mu26 || HLT_mu24_imedium || HLT_mu26_imedium || HLT_mu50 || HLT_mu60_0eta105_msonly)'
	dimuonTrig = '(HLT_2mu10 || HLT_2mu14 || HLT_mu24_mu8noL1 || HLT_mu14_iloose_mu14 || HLT_mu20_imedium_mu8noL1 || HLT_mu20_iloose_mu8noL1)'
	electronTrig = '(HLT_e24_medium_iloose || HLT_e24_tight_L1EM20V || HLT_e28_tight_iloose || HLT_e60_medium || HLT_e55_loose1 || HLT_e120_loose1 || HLT_e28_lhtight_iloose || HLT_e45_loose1 || HLT_e7_medium1 || HLT_e12_medium1 || HLT_e17_medium1 )'
	dielectronTrig = '(HLT_2e12_loose1 || HLT_2e17_loose1)'    
	elemuonTrig = '(HLT_e17_medium1_mu12 || HLT_e7_medium1_mu24)'
	xeTrig = '(HLT_xe50_cell || HLT_xe50_pueta || HLT_xe100|| HLT_xe70_pueta || HLT_xe100_cell || HLT_xe100_pueta || HLT_xe70 || HLT_xe50)'
	#expression = '('+muonTrig+' || '+dimuonTrig+' || '+electronTrig+' || '+dielectronTrig+' || '+elemuonTrig+' || '+ xeTrig +') && '+objectSelection 
	expression = objectSelection

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY5SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY5SkimmingTool",
                                                                expression = expression)

ToolSvc += SUSY5SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("SUSY5Kernel",
									SkimmingTools = [SUSY5SkimmingTool],
									ThinningTools = thinningTools
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_SUSY5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY5Stream )
SUSY5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY5Stream.AcceptAlgs(["SUSY5Kernel"])
# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="SUSY5ThinningSvc", outStreams=[evtStream] )

#====================================================================
# CONTENT LIST  
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY5SlimmingHelper = SlimmingHelper("SUSY5SlimmingHelper")
SUSY5SlimmingHelper.SmartCollections = ["PhotonCollection","ElectronCollection","Muons","TauRecContainer","AntiKt4LCTopoJets","MET_RefFinal","BTagging_AntiKt4LCTopo", "InDetTrackParticles", "PrimaryVertices"]
SUSY5SlimmingHelper.AllVariables = ["AntiKt4TruthJets", "BTagging_AntiKt4Truth", "MuonTruthParticle", "TruthParticle", "TruthEvent", "TruthVertex", "MET_Truth","BTagging_AntiKt4LCTopo"]					
SUSY5SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4LCTopo.MV1_discriminant","PhotonCollection.author.Loose.Tight","ElectronCollection.author.Loose.Medium.Tight.truthOrigin.truthType.charge.ptcone20.etcone30","Muons.ptcone30.ptcone20.etcone30.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag","AntiKt4LCTopoJets.ActiveArea4vec_pt.ActiveArea4vec_eta.ActiveArea4vec_phi.ActiveArea4vec_m.EnergyPerSampling.NumTrkPt1000.TrackWidthPt1000.GhostMuonSegment.NumTrkPt500","GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix","CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType","ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType","TauRecContainer.TruthCharge.TruthProng.IsTruthMatched.TruthPtVis.truthOrigin.truthType"]
SUSY5SlimmingHelper.AppendContentToStream(SUSY5Stream)

# Add the MET containers to the stream
addMETOutputs(SUSY5Stream,["SUSY"])

for item in OutputJets["LargeR"]:
	if "Trim" not in item:
		continue
	SUSY5Stream.AddItem("xAOD::JetContainer_v1#"+item)
	SUSY5Stream.AddItem("xAOD::JetAuxContainer_v1#"+item+"Aux.")
	pass

MSMgr.RemoveItemFromAllStreams("PileUpEventInfo#McEventInfo")
MSMgr.RemoveItemFromAllStreams("EventInfo#*")
