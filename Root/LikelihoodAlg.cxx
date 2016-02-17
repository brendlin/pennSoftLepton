#include <pennSoftLepton/LikelihoodAlg.h>

// this is needed to distribute the algorithm to the workers
ClassImp(PSL::LikelihoodAlg)

PSL::LikelihoodAlg::LikelihoodAlg() : AlgBase()
				    ,lh_name("")
				    ,myHist(0)
{
  MSG_DEBUG("Constructor");
}

bool PSL::LikelihoodAlg::init(void)
{
  MSG_DEBUG("Likelihood algbase name is " << configured_name);
  MSG_DEBUG("Likelihood name is " << lh_name);

  // Make sure (in EventLoop) that the new call and the setHistogramOutput bit are in the same step
  myHist = new TH1F("myHist","myHist",200,-2,2);
  setHistogramOutput(myHist);
  return true;
}

void PSL::LikelihoodAlg::loop(void){
  MSG_INFO("Number of electrons is " << m_EDM->get_el_n());
  for(int i=0;i<m_EDM->get_el_n();i++){
    myHist->Fill(m_EDM->getLikelihoodResult(i,ElectronID::TightLLH));
    MSG_INFO("Electron " << i << " Result: " << (m_EDM->getLikelihoodResult(i,ElectronID::TightLLH)));
    MSG_INFO("Electron " << i << " passed? " << (m_EDM->passLikelihood(i,ElectronID::TightLLH) ? "yes":"no"));
//     MSG_INFO("Electron " << i << " Bits:   " 
// 	     << " d0   " << m_EDM->m_lhtool->getTAccept().getCutResult("TrackA0") 
// 	     << " dphi " << m_EDM->m_lhtool->getTAccept().getCutResult("TrackMatchPhiRes")
// 	     << " deta " << m_EDM->m_lhtool->getTAccept().getCutResult("TrackMatchEta")
// 	     );
    MSG_INFO("Electron " << i << " LHTight " << (m_EDM->getLikelihoodResult(i,ElectronID::TightLLH) ? "yes":"no"));
  }
}

void PSL::LikelihoodAlg::finish(void){
  MSG_DEBUG("finish.");
}
