#include <vector>

// Order matters???
#include <pennSoftLepton/switch_d3pd_xaod.h>
#include <pennSoftLepton/UtilCore.h>
#include <pennSoftLepton/Sample.h>
#include <pennSoftLepton/Variables.h>
#include "pennSoftLepton/EventDefinition.h"
// xaod
#include "pennSoftLepton/HelpersXaod.h"
// Wrappers
#include <pennSoftLepton/xAODWrapper.h>
#include <pennSoftLepton/D3PDWrapper.h>
#include <pennSoftLepton/dummyWrapper.h>
#include <pennSoftLepton/NtupleWrapper.h>
#include <pennSoftLepton/EDM.h>
// Others
#include <pennSoftLepton/AlgBase.h>
#include <pennSoftLepton/ObjectSelectionBase.h>
#include <pennSoftLepton/CutItem.h>
#include <pennSoftLepton/PassEvent.h>
#include <pennSoftLepton/WeightSystematics.h>
#include <pennSoftLepton/Ntupler.h>
#include <pennSoftLepton/FakeFactorTool.h>

#ifndef BEFORE_SUSYTOOLS_000709
#include <pennSoftLepton/XSUSYObjDefAlgV7.h>
#else
#include <pennSoftLepton/XSUSYObjDefAlg.h>
#endif // BEFORE_SUSYTOOLS_000709

#include <pennSoftLepton/XSUSYObjDefAlgExpert.h>
#include <pennSoftLepton/XSUSYObjDefAlgPlusPlus.h>
#include <pennSoftLepton/XSUSYCutflow.h>
#include <pennSoftLepton/TruthObjDef.h>

// d3pd
#include <pennSoftLepton/SUSYObjDefAlg.h>
#include <pennSoftLepton/ToolsD3PDEra.h>
#include <pennSoftLepton/SlimmerD3PD.h>

// Soon to be deprecated
#include <pennSoftLepton/LikelihoodAlg.h>

// SUSY cross section
#ifndef SKIP_XAOD_COMPILATION
#include <SUSYTools/SUSYCrossSection.h>
#endif // SKIP_XAOD_COMPILATION

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#pragma link C++ namespace PSL;

// UtilCore
#pragma link C++ function PSL::SetSHSampleEntries;
#pragma link C++ function PSL::PrintJobAlgorithms;
#pragma link C++ function PSL::GetDerivationSkimOriginalSumw;

// Sample
#pragma link C++ enum PSL::Sample;
#pragma link C++ function PSL::ConvertSampleToSampleVec;
#pragma link C++ function PSL::ConvertToSample;
#pragma link C++ function PSL::GetAllSamples;
#pragma link C++ function PSL::GetAllSamplesDetailed;
#pragma link C++ function PSL::GetAllSampleStrings;
#pragma link C++ function PSL::GetAllSampleStringsDetailed;
#pragma link C++ function PSL::ConvertSampleToStr;
/* #pragma link C++ class std::map<unsigned int,PSL::Sample>; // causes crash! */

// Variable
#pragma link C++ enum PSL::Variable+;
#pragma link C++ function PSL::GetAllVariables;
#pragma link C++ function PSL::GetAllVariableStrings;
#pragma link C++ function PSL::VariableIsInt;
#pragma link C++ function PSL::GetXaxisLabel;
#pragma link C++ function PSL::GetVariableHistArgs;
#pragma link C++ function PSL::SetBinLabels;
#pragma link C++ function PSL::GetAllLeptonTruthLabelStrings;
#pragma link C++ function PSL::GetAllElectronIDStrings;
#pragma link C++ function PSL::ConvertToInt;

// EventDefinition
#pragma link C++ enum PSL::ObjType;
#pragma link C++ enum PSL::LeptonChannel;
#pragma link C++ class PSL::Particle; // no +
#pragma link C++ class PSL::EventDefinition+;

// xAOD
#ifndef SKIP_XAOD_COMPILATION
#pragma link C++ function PSL::GetDerivationWeightInfo+;
#endif // SKIP_XAOD_COMPILATION

// Wrappers
#ifndef SKIP_XAOD_COMPILATION
#pragma link C++ class PSL::xAODWrapper+;
#endif // SKIP_XAOD_COMPILATION
#ifndef SKIP_D3PD_COMPILATION
#pragma link C++ class PSL::D3PDWrapper+;
#endif // SKIP_D3PD_COMPILATION
#pragma link C++ class PSL::dummyWrapper+;
#pragma link C++ class PSL::NtupleWrapper+;
#pragma link C++ class PSL::EDM+;

// Additional classes
#pragma link C++ class PSL::AlgBase+;
#pragma link C++ class PSL::ObjectSelectionBase+;
#pragma link C++ class PSL::CutItem+;
#pragma link C++ class PSL::PassEvent+;
#pragma link C++ class PSL::Ntupler+;
#pragma link C++ class PSL::FakeFactorTool+;

#ifndef SKIP_XAOD_COMPILATION
#ifndef BEFORE_SUSYTOOLS_000709
#pragma link C++ class PSL::XSUSYObjDefAlgV7+;
#else
#pragma link C++ class PSL::XSUSYObjDefAlg+;
#pragma link C++ class PSL::XSUSYObjDefAlgExpert+;
#endif // BEFORE_SUSYTOOLS_000709
#pragma link C++ class PSL::XSUSYObjDefAlgPlusPlus+;
#pragma link C++ class PSL::XSUSYCutflow+;
#pragma link C++ class PSL::TruthObjDef+;
#pragma link C++ class PSL::WeightSystematics+;
#endif // SKIP_XAOD_COMPILATION

#ifndef SKIP_D3PD_COMPILATION
#pragma link C++ class PSL::SUSYObjDefAlg+;
#pragma link C++ class PSL::ToolsD3PDEra+;
#pragma link C++ class PSL::SlimmerD3PD+;
#endif // SKIP_D3PD_COMPILATION

// Soon to be deprecated
#pragma link C++ class PSL::LikelihoodAlg+;

#ifndef SKIP_XAOD_COMPILATION
#pragma link C++ namespace SUSY;
#pragma link C++ class CrossSectionDB;
#endif // SKIP_XAOD_COMPILATION

#endif // __CINT__
