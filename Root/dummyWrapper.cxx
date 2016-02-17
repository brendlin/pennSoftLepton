#include <pennSoftLepton/dummyWrapper.h>

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::dummyWrapper)

double PSL::dummyWrapper::GetEventVariable(PSL::Variable v){
  if (m_EventVariables.count(v)) return m_EventVariables[v];
  double d = -999;
  // begin all of the variable definitions
  //else if (v == ) d = ;
  MSG_INFO("Error! Variable not implemented in GetEventVariable: " << ConvertVarToStr(v));
  // end all of the variable definitions
  m_EventVariables[v] = d;
  return d;
}

// void PSL::dummyWrapper::ConfigureName(std::string nm){
//   configured_name = nm; SetName(nm.c_str());
// }

void PSL::dummyWrapper::ConfigureRun(int runNumber,std::string samp){
  if (m_run_sample_map.count(runNumber)){
    MSG_INFO("Ignoring duplicate run: " << runNumber);
    return;
  }
  Sample s = ConvertToSample(samp);
  m_run_sample_map[runNumber] = s;
  return;
}

void PSL::dummyWrapper::PrintRunConfiguration(void){
  for (std::map<int,PSL::Sample>::const_iterator it=m_run_sample_map.begin();
       it!=m_run_sample_map.end();++it){
    MSG_INFO("Run " << it->first << ": " << ConvertSampleToStr(it->second));
  }
}

PSL::dummyWrapper::dummyWrapper() : m_CurrentSample(knone)
// 				,m_event(0)				
// 				,eventInfo(0)
// 				,electrons(0)
// 				,electrons_aux(0)
// 				,m_lhtool(0)
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
  MSG_DEBUG("Constructor");
  SetName("PSL_EDM_CLASS");
}

EL::StatusCode PSL::dummyWrapper::setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.
  MSG_DEBUG("setupJob");
  (void)job;
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::dummyWrapper::histInitialize()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.
  MSG_DEBUG("histInitialize");
  MSG_DEBUG("my name is " << GetName());

  m_EventVariables.clear();

  n_processed = 0;

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::dummyWrapper::fileExecute()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  MSG_DEBUG("fileExecute");
  m_CurrentSample = knone;
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::dummyWrapper::changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  MSG_DEBUG("changeInput");
  (void)firstFile;
  m_CurrentSample = knone;
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::dummyWrapper::initialize()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.
  MSG_DEBUG("initialize");
  //m_event = wk()->xaodEvent();
  //PrintRunConfiguration();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::dummyWrapper::execute()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.
  MSG_DEBUG("execute");

  if (m_CurrentSample == knone) {
    int run = runNumber();
    //if(!eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)){ // not simulation == data
    if (false){
      MSG_INFO("Setting sample to data.");
      m_CurrentSample = kdata;
    }
    else if (!m_run_sample_map.count(run)) {
      MSG_INFO("Error! Run " << run << " cannot be converted to sample! Exiting.");
      exit(1);
    }
    else {
      m_CurrentSample = m_run_sample_map[run];
      MSG_INFO("Setting sample (Run " << run << ") to " << ConvertSampleToStr(m_CurrentSample));
    }
  }
  // if (!m_event->retrieve(electrons_aux,"ElectronCollectionAux").isSuccess())
  //   MSG_INFO("Error! ElectronCollectionAux not found!");
  n_processed++;
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::dummyWrapper::postExecute()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  MSG_DEBUG("postExecute");
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::dummyWrapper::finalize()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.
  MSG_INFO("finalize. Nevents processed: " << n_processed);
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::dummyWrapper::histFinalize()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.
  MSG_DEBUG("histFinalize");
 return EL::StatusCode::SUCCESS;
}
