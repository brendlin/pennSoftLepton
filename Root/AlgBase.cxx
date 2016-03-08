#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <pennSoftLepton/AlgBase.h>

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::AlgBase)

PSL::AlgBase::AlgBase() : ParentEventSelector(0)
  ,hist_PassN(0)
  ,hist_PassW(0)
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
  MSG_DEBUG("The algbase constructor is called");
}

EL::StatusCode PSL::AlgBase::setupJob(EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.
  MSG_VERBOSE("Verb "  << job.algsHas(""));
  return EL::StatusCode::SUCCESS;
}

EL::Algorithm* PSL::AlgBase::connectToAnotherAlgorithm(std::string name){
  EL::Algorithm* al = (EL::Algorithm*)wk()->getAlg(name);
  if (!al){
    MSG_INFO("Error! Could not find alg " << name << ", requested by " << GetName() << ". Exiting.");
    exit(1);
  }
  return al;
}

EL::StatusCode PSL::AlgBase::histInitialize()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.
  SetName(configured_name.c_str());
  MSG_DEBUG("my name is " << GetName());
  m_EDM = (EDM*)wk()->getAlg("PSL_EDM_CLASS");
  if (!m_EDM) {
    MSG_INFO("Error! Could not connect to EDM class! Exiting.");
    return EL::StatusCode::FAILURE;
  }
  tmp_hists.clear();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::AlgBase::fileExecute()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::AlgBase::changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  MSG_VERBOSE("Verb "  << firstFile);
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::AlgBase::initialize()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.
  if (!init()) return EL::StatusCode::FAILURE;
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::AlgBase::execute()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.
  
  // If xAODWrapper determines that the event should be skipped (e.g. z+jet/zgamma overlap)
  // then skip the event altogether
  if (m_EDM->EventKilled()) return EL::StatusCode::SUCCESS;
  loop();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::AlgBase::postExecute()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::AlgBase::finalize()
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
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PSL::AlgBase::histFinalize()
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
  finish();
  return EL::StatusCode::SUCCESS;
}

void PSL::AlgBase::setHistogramOutput(TH1* hist){
  wk()->addOutput(hist);
}

//-----------------------------------------------------------------------------
void PSL::AlgBase::SaveHistogram(TH1* hist,TFile* file){
  MSG_INFO("SaveHistogram");
  std::string name;
  std::string path = hist->GetName();
  std::string::size_type split = path.rfind ("/");
  if (split == std::string::npos) name = path;
  else {
    std::string dirname = path.substr (0, split);
    name = path.substr (split + 1);
    TNamed *named = dynamic_cast<TNamed*>(hist);
    if (named)
      named->SetName (name.c_str());
    TDirectory* dir = dynamic_cast<TDirectory*>(file->Get (dirname.c_str()));
    if (!dir)
    {
      file->mkdir (dirname.c_str());
      dir = dynamic_cast<TDirectory*>(file->Get (dirname.c_str()));
    }
    dir->cd();
    hist->Write(hist->GetName(),TObject::kOverwrite);
  }
  MSG_INFO("SaveHistogram end");
  return;
}

//-----------------------------------------------------------------------------
void PSL::AlgBase::SaveIfNotEmpty(TH1* hist,bool saveanyway){
  int nbins = hist->GetNbinsX();
  if (hist->Integral() ||
      hist->GetBinContent(0) ||
      hist->GetBinContent(nbins+1) || 
      saveanyway) {
    TH1F* tmp = (TH1F*)hist->Clone();
    setHistogramOutput(tmp);
    tmp_hists.push_back(tmp);
  }
  return;
}

//-----------------------------------------------------------------------------
void PSL::AlgBase::MakeVariableTH1FVector(std::vector<std::vector<TH1*> >& obj,std::string classname,std::string instname,std::vector<PSL::Sample> samples,std::vector<PSL::Variable> variables){
  for (unsigned int i=0;i<samples.size();++i){
    std::vector<TH1*> varvec;
    for (unsigned int j=0;j<variables.size();++j){
      std::string name = GetHistogramLocation(classname,instname,samples[i],variables[j]);
      TH1F* tmphist = MakeVariableTH1F(name,variables[j]);
      varvec.push_back(tmphist);
    }
    obj.push_back(varvec);
  }
  return;
}

//-----------------------------------------------------------------------------
void PSL::AlgBase::MakeLepVariableTH1FVector(std::vector<std::vector<TH1*> >& obj,std::string classname,std::string instname,std::vector<PSL::Sample> samples){
  for (unsigned int i=0;i<samples.size();++i){
    std::vector<TH1*> varvec;
    for (unsigned int j=0;j<GetAllLepVariables().size();++j){
      std::string name = GetHistogramLocation(classname,instname,samples[i],GetAllLepVariables()[j]);
      TH1F* tmphist = MakeVariableTH1F(name,GetAllLepVariables()[j]);
      varvec.push_back(tmphist);
    }
    obj.push_back(varvec);
  }
  return;
}

//-----------------------------------------------------------------------------
void PSL::AlgBase::MakeVectorOfLepVariableTH1FVectors(std::vector<std::vector<std::vector<TH1*> > >& obj,std::string classname,std::string instname,std::vector<PSL::Sample> samples,std::vector<PSL::LepVariable> lepvariables,std::vector<PSL::Variable> lepindices){
  for (unsigned int i=0;i<samples.size();++i){
    std::vector<std::vector<TH1*> > lepvarvec_1deep;
    for (unsigned int j=0;j<lepvariables.size();++j){
      std::vector<TH1*> lepvarvec_2deep;
      for (unsigned int k=0;k<lepindices.size();++k){
        std::string name = GetHistogramLocation(classname,instname,samples[i],lepvariables[j],lepindices[k]);
        TH1F* tmphist = MakeVariableTH1F(name,lepvariables[j]);
        tmphist->SetDirectory(0);
        lepvarvec_2deep.push_back(tmphist);
      }
      lepvarvec_1deep.push_back(lepvarvec_2deep);
    }
    obj.push_back(lepvarvec_1deep);
  }
  return;
}

//-----------------------------------------------------------------------------
std::string PSL::AlgBase::GetHistogramLocation(std::string classname,std::string instname,Sample s,Variable v){
  std::string samplename = ConvertSampleToStr(s);
  std::string varname = ConvertVarToStr(v);
  std::string location = classname+"_"+instname;
  location += "/";
  location += classname+"_"+instname;
  location += "_"+samplename+"_"+varname;
  return location;
}
//-----------------------------------------------------------------------------
std::string PSL::AlgBase::GetHistogramLocation(std::string classname,std::string instname,std::string name){
  std::string location = classname+"_"+instname;
  location += "/";
  location += classname+"_"+instname;
  location += "_"+name;
  return location;
}
//-----------------------------------------------------------------------------
std::string PSL::AlgBase::GetHistogramLocation(std::string classname,std::string instname,Sample s,std::string name){
  std::string samplename = ConvertSampleToStr(s);
  std::string location = classname+"_"+instname;
  location += "/";
  location += classname+"_"+instname;
  location += "_"+samplename+"_"+name;
  return location;
}
//-----------------------------------------------------------------------------
std::string PSL::AlgBase::GetHistogramLocation(std::string classname,std::string instname,Sample s,LepVariable v,Variable lep_index){
  std::string samplename = ConvertSampleToStr(s);
  std::string varname = ConvertLepVarToStr(v);
  std::string indexname = ConvertVarToStr(lep_index);
  std::string location = classname+"_"+instname;
  location += "/";
  location += classname+"_"+instname;
  location += "_"+samplename+"_"+varname+"_"+indexname;
  return location;
}
//-----------------------------------------------------------------------------
std::string PSL::AlgBase::GetHistogramLocation(std::string classname,std::string instname,Sample s,LepVariable v){
  std::string samplename = ConvertSampleToStr(s);
  std::string varname = ConvertLepVarToStr(v);
  std::string location = classname+"_"+instname;
  location += "/";
  location += classname+"_"+instname;
  location += "_"+samplename+"_"+varname;
  return location;
}

//-----------------------------------------------------------------------------
std::string PSL::AlgBase::GetHistogramLocation(std::string classname,std::string instname,Sample s,JetVariable v){
  std::string samplename = ConvertSampleToStr(s);
  std::string varname = ConvertJetVarToStr(v);
  std::string location = classname+"_"+instname;
  location += "/";
  location += classname+"_"+instname;
  location += "_"+samplename+"_"+varname;
  return location;
}

//-----------------------------------------------------------------------------
void PSL::AlgBase::set_Variables(std::string v){
  MSG_DEBUG("setting algorithm " << GetName() << " variables to " << v);
  std::vector<std::string> tmp;
  PSL::StringTok(tmp,v," ");
  m_variables.clear();
  for(unsigned int i=0;i<tmp.size();i++){
    m_variables.push_back(ConvertToVariable(tmp[i]));
  }
}

//-----------------------------------------------------------------------------
void PSL::AlgBase::set_SkipVariables(std::string v){
  MSG_DEBUG("skipping ntupler variables: " << v);
  std::vector<std::string> tmp;
  PSL::StringTok(tmp,v," "); 
  std::vector<Variable> variables = GetAllVariables();
  m_variables.clear();
  for(unsigned int i=0;i<variables.size();i++){
    if (std::find(tmp.begin(),tmp.end(),ConvertVarToStr(variables[i])) != tmp.end()) continue;
    m_variables.push_back(variables[i]);
  }
}

//-----------------------------------------------------------------------------
void PSL::AlgBase::set_LepVariables(std::string v){
  MSG_DEBUG("setting AlgBase lep variables to " << v);
  std::vector<std::string> tmp;
  PSL::StringTok(tmp,v," ");
  m_lepvariables.clear();
  for(unsigned int i=0;i<tmp.size();i++){
    m_lepvariables.push_back(ConvertToLepVariable(tmp[i]));
  }
}

//-----------------------------------------------------------------------------
void PSL::AlgBase::set_Samples(std::string s){
  MSG_DEBUG("setting samples to " << s);
  std::vector<std::string> samples_str_tmp;
  Samples.clear();
  PSL::StringTok(samples_str_tmp,s," ");
  for(unsigned int i=0;i<samples_str_tmp.size();i++){
    //MSG_INFO(" " << samples_str_tmp.at(i));
    PSL::Sample samp = PSL::ConvertToSample(samples_str_tmp.at(i));
    if (!std::count(Samples.begin(),Samples.end(),samp)) Samples.push_back(samp);
  }
}
