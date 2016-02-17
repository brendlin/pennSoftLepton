#ifndef pennSoftLepton_CUTITEM_H
#define pennSoftLepton_CUTITEM_H

#include <pennSoftLepton/AlgBase.h>
#include <pennSoftLepton/Sample.h>
#include <pennSoftLepton/UtilCore.h>
#include <pennSoftLepton/Variables.h>

// C/C++
#include <vector>
#include <string>
#include <iomanip>
#include <iostream>

namespace PSL
{
  //* \brief CutTemplate holds predefined operators comparing up to four elements. */
  enum CutTemplate {
    a_EQ_b   // =
    ,a_LT_b  // <
    ,a_GT_b  // >
    ,a_LEQ_b // <=
    ,a_GEQ_b // >=
    ,a_AND_b // &&
    ,a_OR_b  // ||
    ,a_GT_b__AND__c_EQ_d  // for pt cuts and flavor
    ,a_LT_b__AND__c_EQ_d  // for pt cuts and flavor
    ,a_LT_b__AND__c_LT_d 
    ,a_GT_b__AND__c_GT_d 
    ,a_GT_b__OR__c_LT_d 
    ,a_NEQ_b // != $
    ,a_TRUE // empty
    ,a_EQ_b__AND__c_EQ_d
    ,a_OR_b_OR_c
    ,a_OR_b_OR_c_OR_d
    ,a_AND_b_AND_c
  };

  std::string ConvertCutTemplateToStr(CutTemplate c);
  CutTemplate ConvertToCutTemplate(const std::string& c);

  //class CutItem;

  class CutItem : public AlgBase
  {
  public:
    CutItem();
    virtual ~CutItem(){};
    
    bool alwaysPass;
    bool doTemplatedComparison;
    CutTemplate template_ComparisonType;
    bool doCompareSubCutItems;
    CutTemplate subCut_ComparisonType;

    bool a_b_intComparison;
    bool c_d_intComparison;

    std::vector<std::string> SubCutStrings;
    
    Variable m_var1;
    Variable m_var2;
    Variable m_var3;
    Variable m_var4;
    Variable weight;

    LepVariable m_lepvar1;
    LepVariable m_lepvar2;
    LepVariable m_lepvar3;
    LepVariable m_lepvar4;
    LepVariable lepweight;

    double m_number1;
    double m_number2;
    double m_number3;
    double m_number4;

    std::vector<PSL::CutItem*> SubCutItems; //!

    void PrintConfig(void);
    bool init(void);

    bool pass(void);
    bool passTemplatedComparison(void);
    bool passCompareSubItems(void);
    void set_doAlwaysPass(bool bo){alwaysPass = bo;};
    void set_doTemplatedComparison(){
      doCompareSubCutItems = false;
      doTemplatedComparison = true;
    };
    void set_doCompareSubCutItems(){
      doCompareSubCutItems = true;
      doTemplatedComparison = false;
    };
    void set_template_ComparisonType(std::string s){
      template_ComparisonType = ConvertToCutTemplate(s);
    };
    void set_subCut_ComparisonType(std::string s){
      bool valid = true;
      if (s.find("=") != std::string::npos) valid = false;
      if (s.find("<") != std::string::npos) valid = false;
      if (s.find(">") != std::string::npos) valid = false;
      if (s.find("@") != std::string::npos) valid = false;
      if (s.find("#") != std::string::npos) valid = false;
      if (s.find("$") != std::string::npos) valid = false;
      if (s.find("&") != std::string::npos && 
          s.find("|") != std::string::npos) valid = false;
      if (s.find("&") != std::string::npos) subCut_ComparisonType = a_AND_b;
      if (s.find("|") != std::string::npos) subCut_ComparisonType = a_OR_b;
      if (!valid) {
        MSG_INFO("Error! Cannot compare subcuts using " << s);
        exit(1);
      }
    };
    void set_SubCutStrings(std::string s){StringTok(SubCutStrings,s," ");};
    void set_number1(double doub){m_number1 = doub;};
    void set_number2(double doub){m_number2 = doub;};
    void set_number3(double doub){m_number3 = doub;};
    void set_number4(double doub){m_number4 = doub;};
    void set_var1(std::string s){
      m_var1 = ConvertToVariable(s); 
      if (VariableIsInt(m_var1) && m_lepvar1 == lepNone) a_b_intComparison = true;
    };
    void set_var2(std::string s){
      m_var2 = ConvertToVariable(s);
      if (VariableIsInt(m_var2) && m_lepvar2 == lepNone) a_b_intComparison = true;
    };
    void set_var3(std::string s){
      m_var3 = ConvertToVariable(s);
      if (VariableIsInt(m_var3) && m_lepvar3 == lepNone) c_d_intComparison = true;
    };
    void set_var4(std::string s){
      m_var4 = ConvertToVariable(s);
      if (VariableIsInt(m_var4) && m_lepvar4 == lepNone) c_d_intComparison = true;
    };
    
    void set_lepvar1(std::string s){
      m_lepvar1 = ConvertToLepVariable(s); 
      if (VariableIsInt(m_lepvar1)) a_b_intComparison = true;
      else a_b_intComparison = false;
    };
    void set_lepvar2(std::string s){
      m_lepvar2 = ConvertToLepVariable(s);
      if (VariableIsInt(m_lepvar2)) a_b_intComparison = true;
      else a_b_intComparison = false;
    };
    void set_lepvar3(std::string s){
      m_lepvar3 = ConvertToLepVariable(s);
      if (VariableIsInt(m_lepvar3)) c_d_intComparison = true;
      else c_d_intComparison = false;
    };
    void set_lepvar4(std::string s){
      m_lepvar4 = ConvertToLepVariable(s);
      if (VariableIsInt(m_lepvar4)) c_d_intComparison = true;
      else c_d_intComparison = false;
    };
    
  public:
    ClassDef(CutItem, 1);
  };
  
} // PSL

#endif // pennSoftLepton_CUTITEM_H
