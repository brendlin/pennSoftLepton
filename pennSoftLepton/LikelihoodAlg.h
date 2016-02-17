#ifndef pennSoftLepton_LikelihoodAlg_H
#define pennSoftLepton_LikelihoodAlg_H

#include <pennSoftLepton/AlgBase.h>

namespace PSL
{
  
  class LikelihoodAlg : public AlgBase
  {
    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
  public:
    // float cutValue;
    std::string lh_name;
    
    // variables that don't get filled at submission time should be
    // protected from being send from the submission node to the worker
    // node (done by the //!)
  public:
    // Tree *myTree; //!
    TH1 *myHist; //!
    
    // this is a standard constructor
    LikelihoodAlg();
    //
    bool init(void);
    void loop(void);
    void finish(void);
    
    
    // this is needed to distribute the algorithm to the workers
    ClassDef(LikelihoodAlg, 1);
  };
  
}
#endif
