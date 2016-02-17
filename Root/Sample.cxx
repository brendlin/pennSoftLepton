#include "pennSoftLepton/Sample.h"

#include <iomanip>
#include <iostream>

#include <pennSoftLepton/Messaging.h>
#include <pennSoftLepton/UtilCore.h>

//-----------------------------------------------------------------------------
const std::vector<PSL::sample_props> PSL::GetSampleProperties(){
  static std::vector<sample_props> s_props;
  if (s_props.empty()){
    //                             ENUM       code-readable human-readable
    s_props.push_back(sample_props(knone      ,"none"      ,"None"       )); // no!
    s_props.push_back(sample_props(kdata      ,"data"      ,"Data"       ));
    s_props.push_back(sample_props(kunlabeled ,"unlabeled" ,"Unlabeled"  ));
    s_props.push_back(sample_props(khiggs     ,"higgs"     ,"Higgs"      ));
    s_props.push_back(sample_props(kggww      ,"ggww"      ,"ggWW"       ));
    s_props.push_back(sample_props(kqqww      ,"qqww"      ,"qqWW"       ));
    s_props.push_back(sample_props(kttbar     ,"ttbar"     ,"ttbar"      ));
    s_props.push_back(sample_props(ksingletop ,"singletop" ,"singletop"  ));
    s_props.push_back(sample_props(kttv       ,"ttv"       ,"ttV"        ));
    s_props.push_back(sample_props(ktz        ,"tz"        ,"tZ"         ));
    s_props.push_back(sample_props(ktw        ,"tw"        ,"Wt"         ));
    s_props.push_back(sample_props(ktother    ,"tother"    ,"tOther"     ));
    s_props.push_back(sample_props(kwz        ,"wz"        ,"WZ"         ));
    s_props.push_back(sample_props(kzz        ,"zz"        ,"ZZ"         ));
    s_props.push_back(sample_props(kvvv       ,"vvv"       ,"VVV"        ));
    s_props.push_back(sample_props(kzjee      ,"zjee"      ,"Zj_ee"      ));
    s_props.push_back(sample_props(kzjmm      ,"zjmm"      ,"Zj_mm"      ));
    s_props.push_back(sample_props(kzjtt      ,"zjtt"      ,"Zj_tt"      ));
    s_props.push_back(sample_props(kzjhf      ,"zjhf"      ,"Zj_HF"      ));
    s_props.push_back(sample_props(kzjnn      ,"zjnn"      ,"Zj_nunu"    ));
    s_props.push_back(sample_props(kzgam      ,"zgam"      ,"Zgamma"     ));
    s_props.push_back(sample_props(kwje       ,"wje"       ,"Wj_e"       ));
    s_props.push_back(sample_props(kwjm       ,"wjm"       ,"Wj_mu"      ));
    s_props.push_back(sample_props(kwjt       ,"wjt"       ,"Wj_tau"     ));
    s_props.push_back(sample_props(kwjhf      ,"wjhf"      ,"Wj_HF"      ));
    s_props.push_back(sample_props(kwgam      ,"wgam"      ,"Wgamma"     ));
    s_props.push_back(sample_props(kwgvbs     ,"wgvbs"     ,"Wgamma_VBS" ));
    s_props.push_back(sample_props(kjpsi      ,"jpsi"      ,"JPsi"       ));
    s_props.push_back(sample_props(kupsl      ,"upsl"      ,"Upsilon"    ));
    s_props.push_back(sample_props(kqflip     ,"qflip"     ,"ChargeFlip" ));
    s_props.push_back(sample_props(kwjdd      ,"wjdd"      ,"Wjet_DD"    ));
    s_props.push_back(sample_props(ksusy      ,"susy"      ,"Susy"       ));
    s_props.push_back(sample_props(kdijet     ,"dijet"     ,"Dijet"      ));
    s_props.push_back(sample_props(kdijetdd   ,"dijetdd"   ,"Dijet_DD"   ));
    s_props.push_back(sample_props(kzjetdd    ,"zjetdd"    ,"Zjet_DD"    ));
    s_props.push_back(sample_props(kttdblct   ,"ttdblct"   ,"ttbar_DblCt"));
    s_props.push_back(sample_props(kttfix     ,"ttfix"     ,"ttbar_Fix"  ));
    s_props.push_back(sample_props(kEndOfNonOverlapping,"error","Error"  ));
    s_props.push_back(sample_props(kall       ,"all"       ,"All"        ));
    s_props.push_back(sample_props(ksmww      ,"smww"      ,"SMWW"       ));
    s_props.push_back(sample_props(ktall      ,"tall"      ,"Top"        ));
    s_props.push_back(sample_props(kzjet      ,"zjet"      ,"Z+jets"     ));
    s_props.push_back(sample_props(kzall      ,"zall"      ,"Z+jets/Zgamma"));
    s_props.push_back(sample_props(kwjet      ,"wjet"      ,"W+jets"     ));
    s_props.push_back(sample_props(kwzzz      ,"wzzz"      ,"WZ,ZZ"      ));
  }
  return s_props;
}

//-----------------------------------------------------------------------------
PSL::Sample PSL::ConvertToSample(const std::string &s){
  // do not do this during a loop
  static std::vector<sample_props> props = GetSampleProperties();
  for (unsigned int i=0;i<props.size();++i){
    if (s == props[i].second) return props[i].first;
  }
  MSG_INFO("Error! Sample.cxx Cannot find sample enum for " << s);
  exit(1);
  return knone;
}

//-----------------------------------------------------------------------------
std::string PSL::ConvertSampleToStr(Sample s) {
  return ConvertToStr(s,GetSampleProperties(),"Sample");
}

//-----------------------------------------------------------------------------
std::vector<PSL::Sample> PSL::GetAllSamples(bool do_overlapping_samples){
  std::vector<Sample> samples;
  std::vector<Sample> samples_covered;
  static std::vector<sample_props> props = GetSampleProperties();
  unsigned int start = kEndOfNonOverlapping;
  if (do_overlapping_samples) start = props.size();
  for (unsigned int i=start; i>0; --i) {
    Sample s = static_cast<PSL::Sample>(i-1);
    if (s == knone) continue;
    if (s == kdata) continue;
    if (s == kunlabeled) continue;
    if (s == kEndOfNonOverlapping) continue;
    if (s == kall) continue;
    if (std::find(samples_covered.begin(),samples_covered.end(),s) != samples_covered.end())
      continue;
    samples.push_back(s);
    std::vector<Sample> tmp = ConvertSampleToSampleVec(s);
    for (unsigned int j=0;j<tmp.size();++j){
      samples_covered.push_back(tmp[j]);
    }
  }
  std::reverse(samples.begin(),samples.end());
  return samples;
}
//-----------------------------------------------------------------------------
std::vector<PSL::Sample> PSL::GetAllSamplesDetailed(){
  return GetAllSamples(false);
}

//-----------------------------------------------------------------------------
std::vector<std::string> PSL::GetAllSampleStrings(bool do_overlapping_samples){
  std::vector<Sample> samples = GetAllSamples(do_overlapping_samples);
  std::vector<std::string> tmp;
  for (unsigned int i=0;i<samples.size();++i){
    tmp.push_back(ConvertSampleToStr(samples[i]));
  }
  return tmp;
}

//-----------------------------------------------------------------------------
std::vector<std::string> PSL::GetAllSampleStringsDetailed(){
  return GetAllSampleStrings(false);
}

//-----------------------------------------------------------------------------
std::vector<PSL::Sample> PSL::ConvertSampleToSampleVec(Sample s){
  // If non-overlapping / unique, return itself
  if (s < kEndOfNonOverlapping) return std::vector<PSL::Sample>(1,s);
  // All is all
  if (s == kall) {
    std::vector<PSL::Sample> vec;
    for (unsigned int i=0;i<kEndOfNonOverlapping;i++){
      if (i == knone) continue;
      if (i == kdata) continue;
      vec.push_back(static_cast<PSL::Sample>(i));
    }
    return vec;
  }
  // Composites - SMWW, top, Zjet
  if (s == ksmww) return PrettyConvert(kggww,kqqww);
  if (s == ktall) return PrettyConvert(kttbar,ksingletop,kttv,ktz,ktw,ktother);
  if (s == kzjet) return PrettyConvert(kzjee,kzjmm,kzjtt,kzjhf,kzjnn);
  if (s == kzall) return PrettyConvert(kzjee,kzjmm,kzjtt,kzjhf,kzjnn,kzgam);
  if (s == kwjet) return PrettyConvert(kwje,kwjm,kwjt,kwjhf);
  if (s == kwzzz) return PrettyConvert(kwz,kzz);
  // Return empty vector.
  MSG_INFO("Error! Could not convert sample to sample vector!");
  exit(1);
  return std::vector<PSL::Sample>();
}

//-----------------------------------------------------------------------------
std::vector<std::string> PSL::ConvertSampleToSampleVec(std::string s){
  std::vector<PSL::Sample> vec = ConvertSampleToSampleVec(ConvertToSample(s));
  std::vector<std::string> tmp;
  for (unsigned int i=0;i<vec.size();++i){
    tmp.push_back(ConvertSampleToStr(vec[i]));
  }
  return tmp;
}

//-----------------------------------------------------------------------------
std::vector<PSL::Sample> PSL::PrettyConvert(Sample s1,Sample s2,
                                            Sample s3,Sample s4,Sample s5,Sample s6){
  std::vector<PSL::Sample> tmp;
  if(s1 != knone) tmp.push_back(s1);
  if(s2 != knone) tmp.push_back(s2);
  if(s3 != knone) tmp.push_back(s3);
  if(s4 != knone) tmp.push_back(s4);
  if(s5 != knone) tmp.push_back(s5);
  if(s6 != knone) tmp.push_back(s6);
  return tmp;
}

//-----------------------------------------------------------------------------
bool PSL::MatchSample(Sample s1, Sample s2){
  if (s1 == s2) return true;
  std::vector<PSL::Sample> svec1 = ConvertSampleToSampleVec(s1);
  std::vector<PSL::Sample> svec2 = ConvertSampleToSampleVec(s2);
  for (unsigned int i=0;i<svec1.size();++i){
    for (unsigned int j=0;j<svec2.size();++j){
      //
      // in all logical cases one of these samples will be a unique sample, and have a length of 1.
      // So this checks if any of the unique samples overlap with each other.
      if (svec1.at(i) == svec2.at(j)) return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
void PSL::PrintSample(Sample s){
  std::cout << "   " << ConvertSampleToStr(s) << ":" << std::endl;
  std::vector<PSL::Sample> subsamples = ConvertSampleToSampleVec(s);
  for (unsigned int j=0;j<subsamples.size();++j){
    std::cout << "      " << ConvertSampleToStr(subsamples.at(j)) << std::endl;
  }
  return;
}

//-----------------------------------------------------------------------------
void PSL::PrintSamples(std::vector<PSL::Sample> s){
  for(unsigned i = 0; i < s.size(); ++i) { 
    PrintSample(s.at(i));
  }
  return;
}

//-----------------------------------------------------------------------------
// std::string PSL::ConvertToTex(const SamplexxSet &s)
// {
//   if(s.GetName() == "data")  return "Data";
//   if(s.GetName() == "hgww")  return "HWW";
//   if(s.GetName() == "higg")  return "Higgs";
//   if(s.GetName() == "smww")  return "WW";
//   if(s.GetName() == "hggf")  return "HWW ggf";
//   if(s.GetName() == "hvbf")  return "HWW vbf";
//   if(s.GetName() == "whww")  return "HWW wh";
//   if(s.GetName() == "tthw")  return "HWW tth";
//   if(s.GetName() == "zjet")  return "Z$+$jets";
//   if(s.GetName() == "zjee")  return "Z$+$jets ee";
//   if(s.GetName() == "zjuu")  return "Z$+$jets $\\mu\\mu$";
//   if(s.GetName() == "zjtt")  return "Z$+$jets $\\tau\\tau$";
//   if(s.GetName() == "zjhf")  return "Z$+$jets HF";
//   if(s.GetName() == "zvbf")  return "Z$+$jets VBF";
//   if(s.GetName() == "zall")  return "Z$+$jets";
//   if(s.GetName() == "zldy")  return "Z$+$jets";
//   if(s.GetName() == "wjet")  return "W$+$jets";
//   if(s.GetName() == "wjdt")  return "W$+$jets data";
//   if(s.GetName() == "wjdtm")  return "W$+$jets data (muon Fake)";
//   if(s.GetName() == "wjdte")  return "W$+$jets data (e Fake)";
//   if(s.GetName() == "qflip")  return "Charge Flip";
//   if(s.GetName() == "wgam")  return "W$\\gamma$";
//   if(s.GetName() == "wgas")  return "W$\\gamma$*";
//   if(s.GetName() == "wzzz")  return "WZ/ZZ/W$\\gamma$";
//   if(s.GetName() == "singletop")  return "SingleTop";
//   if(s.GetName() == "ttbar")  return "$t\\bar{t}$";
//   if(s.GetName() == "wdpi")  return "$WW$ DPI";
//   if(s.GetName() == "ttv")   return "$t\\bar{t}+V$";
//   if(s.GetName() == "vvv")   return "VVV";
//   if(s.GetName() == "jpsi")  return "$J/\\psi$";
//   if(s.GetName() == "upsl")  return "$\\upsilon$";

//   return s.GetName();
// }

