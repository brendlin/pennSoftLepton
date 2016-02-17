#ifndef pennSoftLepton_EDM_H
#define pennSoftLepton_EDM_H

#include "pennSoftLepton/switch_d3pd_xaod.h"

#include "pennSoftLepton/dummyWrapper.h"
#include "pennSoftLepton/D3PDWrapper.h"
#include "pennSoftLepton/xAODWrapper.h"
#include "pennSoftLepton/NtupleWrapper.h"

namespace PSL
{

  //
  // Automatic choice of Wrapper based on switch_d3pd_xaod.h setting
  // 
#ifndef SKIP_D3PD_COMPILATION
  typedef D3PDWrapper WrapperChoice;
#else 
#ifndef SKIP_XAOD_COMPILATION
  typedef xAODWrapper WrapperChoice;
#else
  typedef NtupleWrapper WrapperChoice;
#endif // SKIP_XAOD_COMPILATION
#endif // SKIP_D3PD_COMPILATION

  class EDM : public WrapperChoice
  {
  public:
    EDM();
    ~EDM(){};
    
    ClassDef(EDM,1);
  };
}

#endif // pennSoftLepton_EDM_H
