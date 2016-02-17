#ifndef pennSoftLepton_SAMPLE_H
#define pennSoftLepton_SAMPLE_H

/**********************************************************************************
 * @Package: MonojetSoftLepton
 * @Class  : Sample
 * @Author : Rustem Ospanov, Doug Schaefer
 *
 * @Brief  :
 * 
 *  Enum for dataset keys (samples) and helper class for a set of samples
 * 
 **********************************************************************************/

// C/C++
#include <vector>
#include <string>

/** \brief The PSL namespace */
namespace PSL
{
  //* \brief Enums for unique dataset samples. Used for naming and stuff. After a point they are overlapping. */
  enum Sample {
    knone,
    kdata,
    kunlabeled,
    khiggs,
    kggww, // ksmww
    kqqww, // ksmww
    kttbar,     // ktall
    ksingletop, // ktall
    kttv,       // ktall
    ktz,       // ktall
    ktw,   // ktall
    ktother,   // ktall
    kwz, // kwzzz
    kzz, // kwzzz
    kvvv,
    kzjee, // kzjet
    kzjmm, // kzjet
    kzjtt, // kzjet
    kzjhf, // kzjet
    kzjnn, // kzjet
    kzgam,
    kwje, // kwjet
    kwjm, // kwjet
    kwjt, // kwjet
    kwjhf, // kwjet
    kwgam,
    kwgvbs,
    kjpsi,
    kupsl,
    kqflip, // charge flip dd
    kwjdd, // wjet dd
    ksusy,
    kdijet,
    kdijetdd, // dijet dd
    kzjetdd,
    kttdblct, // double-counted ttbar (in Z+jet region)
    kttfix, // fix for double-counted ttbar
    kEndOfNonOverlapping, // end of non-overlapping! All samples after this have a special treatment
    kall,
    ksmww,
    ktall,
    kzjet,
    kzall,
    kwjet,
    kwzzz
  };
  
  class sample_props{
  public:
    Sample first; // sample
    std::string second; // name
    std::string human_readable; // human-readable name
    std::string tex_label;

    sample_props(){};
    sample_props(Sample s,std::string nm,std::string hum_read){
      first = s;
      second = nm;
      human_readable = hum_read;
      //tex_label = lab;
    };
  };

  const std::vector<PSL::sample_props> GetSampleProperties();
  //const std::vector<std::pair<PSL::Sample,std::string> > GetSampleStringPairs(void);
  
  std::string ConvertSampleToStr(Sample s);
  //std::string ConvertSampleToTex(Sample s);
  Sample ConvertToSample(const std::string &s);
  
  std::vector<PSL::Sample> GetAllSamples(bool do_overlapping_samples=true);
  std::vector<PSL::Sample> GetAllSamplesDetailed();
  std::vector<std::string> GetAllSampleStrings(bool do_overlapping_samples=true);
  std::vector<std::string> GetAllSampleStringsDetailed();
  
  std::vector<PSL::Sample> PrettyConvert(Sample s1, Sample s2=PSL::knone,
                                         Sample s3=PSL::knone, Sample s4=PSL::knone
                                         ,Sample s5=PSL::knone,Sample s6=PSL::knone);
  
  //* \brief To be able to have composite sample sets */
  std::vector<PSL::Sample> ConvertSampleToSampleVec(Sample s);
  std::vector<std::string> ConvertSampleToSampleVec(std::string s);
  
  /** \brief Match samples to each other */
  bool MatchSample(Sample s1, Sample s2);
  void PrintSample(Sample s);
  void PrintSamples(std::vector<Sample> s);
  
}

#endif
