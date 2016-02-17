#include "pennSoftLepton/switch_d3pd_xaod.h"
#ifndef SKIP_XAOD_COMPILATION

#ifndef pennSoftLepton_WeightSystematics_H
#define pennSoftLepton_WeightSystematics_H

#include <pennSoftLepton/AlgBase.h>
#include <pennSoftLepton/PassEvent.h>

#include "PATInterfaces/SystematicsUtil.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h"

#include <vector>

namespace PSL
{

  class WeightSystematics : public AlgBase
  {
  public:

    WeightSystematics();

    virtual bool init(void);
    virtual void loop(void);
    virtual void finish(void);
    void PrintConfiguration(void);

    TH1F* syst_hist; //!
    std::vector<std::string> syst_list; //!
    std::vector<CP::SystematicSet> m_systInfoList; //!

    std::string ParentEventSelectorName;
    void set_ParentEventSelectorName(std::string s){ParentEventSelectorName = s;};

  public:
    // this is needed to distribute the algorithm to the workers
    ClassDef(WeightSystematics,1);
  };

}

#endif // pennSoftLepton_WeightSystematics_H
#endif // SKIP_XAOD_COMPILATION
