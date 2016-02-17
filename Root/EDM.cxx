#include <pennSoftLepton/EDM.h>

#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::EDM)
  
PSL::EDM::EDM() : WrapperChoice()
{    
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
  MSG_DEBUG("Constructor");
}
