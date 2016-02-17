#include "pennSoftLepton/switch_d3pd_xaod.h"
#ifndef SKIP_XAOD_COMPILATION

#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"
#include "pennSoftLepton/EDM.h"
#ifdef BEFORE_SUSYTOOLS_000709
#include "pennSoftLepton/XSUSYObjDefAlg.h"
#else
#include "pennSoftLepton/XSUSYObjDefAlgV7.h"
#endif // BEFORE_SUSYTOOLS_000709
#include "pennSoftLepton/PassEvent.h"
#include "pennSoftLepton/CutItem.h"

int main( int argc, char* argv[] ) {

  (void)argc;
  (void)argv;

  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  SH::SampleHandler sh;
  SH::scanDir(sh, "/disk/userdata00/atlas_data2/kurb/SampleDataSets/dxaod_c1n2_Mar19/");

  // Set the name of the input TTree. It's always "CollectionTree"
  // for xAOD files.
  sh.setMetaString( "nc_tree", "CollectionTree" );

  EL::Job job;
  job.sampleHandler( sh );
  job.useXAOD();
  job.options()->setString(EL::Job::optXaodAccessMode,EL::Job::optXaodAccessMode_branch);
  //job.options()->setString(EL::Job::optXaodAccessMode,EL::Job::optXaodAccessMode_class);
  //job.options()->setDouble(EL::Job::optMaxEvents,10);

  PSL::EDM* edm = new PSL::EDM();
  job.algsAdd( edm );
#ifdef BEFORE_SUSYTOOLS_000709  
  PSL::XSUSYObjDefAlg* susy_obj = new PSL::XSUSYObjDefAlg();
#else
  PSL::XSUSYObjDefAlgV7* susy_obj = new PSL::XSUSYObjDefAlgV7();
#endif // BEFORE_SUSYTOOLS_000709
  job.algsAdd( susy_obj );
  PSL::PassEvent* pass_alg = new PSL::PassEvent();
  job.algsAdd( pass_alg );
  PSL::CutItem* cut_item = new PSL::CutItem();
  cut_item->SetName("Start");
  cut_item->set_doAlwaysPass(true);
  job.algsAdd( cut_item );

  // Run the job using the local/direct driver:
  std::string submitDir = "unitTestOutput";
  EL::DirectDriver driver;
  driver.submit(job, submitDir);

  return 0;

}

// SKIP_XAOD_COMPILATION
#elif !defined SKIP_D3PD_COMPILATION

int main( int argc, char* argv[] ) {
  return 0;
}

#else // SKIP_D3PD_COMPILATION

int main( int argc, char* argv[] ) {
  return 0;
}

#endif // ifndef
