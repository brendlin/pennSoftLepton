#include "pennSoftLepton/CutItem.h"

ClassImp(PSL::CutItem)

//-----------------------------------------------------------------------------
PSL::CutItem::CutItem() : AlgBase()
  ,alwaysPass(false)
  ,doTemplatedComparison(false)
  ,template_ComparisonType(a_EQ_b)
  ,doCompareSubCutItems(false)
  ,subCut_ComparisonType(a_AND_b)
  ,a_b_intComparison(false)
  ,c_d_intComparison(false)
  ,m_var1(vNone)
  ,m_var2(vNone)
  ,m_var3(vNone)
  ,m_var4(vNone)
  ,weight(vNone)
  ,m_lepvar1(lepNone)
  ,m_lepvar2(lepNone)
  ,m_lepvar3(lepNone)
  ,m_lepvar4(lepNone)
  ,lepweight(lepNone)
  ,m_number1(-999.)
  ,m_number2(-999.)
  ,m_number3(-999.)
  ,m_number4(-999.)
{
  MSG_DEBUG("asdf");
}

//-----------------------------------------------------------------------------
bool PSL::CutItem::init(void){
  for (unsigned int i=0;i<SubCutStrings.size();++i){
    CutItem* tmp = (CutItem*)connectToAnotherAlgorithm(SubCutStrings[i]);
    if (!tmp){
      MSG_INFO("Error! Could not find CutItem " << SubCutStrings[i] << ". Exiting.");
      return false;
    }
    SubCutItems.push_back(tmp);
    MSG_DEBUG("Successfully connected " << SubCutStrings[i] << " to " << GetName() << ".");
  }
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::CutItem::pass(void){
  if (alwaysPass) return true;
  if (doTemplatedComparison) return passTemplatedComparison();
  if (doCompareSubCutItems) return passCompareSubItems();
  return true;
}

//-----------------------------------------------------------------------------
bool PSL::CutItem::passCompareSubItems(void){
  MSG_DEBUG("Comparing SubItems");
  bool any = false;
  for (unsigned int i=0;i<SubCutItems.size();++i){
    if (subCut_ComparisonType == a_AND_b){
      if (!SubCutItems[i]->pass()) return false;
    }
    else {
      any = any | SubCutItems[i]->pass();
    }
  }
  if (subCut_ComparisonType == a_AND_b) return true;
  return any;
}

//-----------------------------------------------------------------------------
bool PSL::CutItem::passTemplatedComparison(void){
  MSG_DEBUG("Comparing via Template. Template is " << ConvertCutTemplateToStr(template_ComparisonType));
  double _a = m_number1;
  if (m_lepvar1 != lepNone) _a = m_EDM->GetSignalLeptonVariable(m_lepvar1,m_var1);
  else if (m_var1 != vNone) _a = m_EDM->GetEventVariable(m_var1);
  double _b = m_number2;
  if (m_lepvar2 != lepNone) _b = m_EDM->GetSignalLeptonVariable(m_lepvar2,m_var2);
  else if (m_var2 != vNone) _b = m_EDM->GetEventVariable(m_var2);
  double _c = m_number3;
  if (m_lepvar3 != lepNone) _c = m_EDM->GetSignalLeptonVariable(m_lepvar3,m_var3);
  else if (m_var3 != vNone) _c = m_EDM->GetEventVariable(m_var3);
  double _d = m_number4;
  if (m_lepvar4 != lepNone) _d = m_EDM->GetSignalLeptonVariable(m_lepvar4,m_var4);
  else if (m_var4 != vNone) _d = m_EDM->GetEventVariable(m_var4);
  // convert to ints now
  if (a_b_intComparison) {_a = (int)_a; _b = (int)_b;}
  if (c_d_intComparison) {_c = (int)_c; _d = (int)_d;}
  if (a_b_intComparison) {
    if ((_a != _b) && ((int)_a == (int)_b)) MSG_INFO("Int as Double Error!");
  }
  if (c_d_intComparison) {
    if ((_c != _d) && ((int)_c == (int)_d)) MSG_INFO("Int as Double Error!");
  }
  // comparisons
  if (template_ComparisonType == a_EQ_b             ) return _a == _b;
  if (template_ComparisonType == a_LT_b             ) return _a < _b;
  if (template_ComparisonType == a_GT_b             ) return _a > _b;
  if (template_ComparisonType == a_LEQ_b            ) return _a <= _b;
  if (template_ComparisonType == a_GEQ_b            ) return _a >= _b;
  if (template_ComparisonType == a_AND_b            ) return bool(_a) && bool(_b);
  if (template_ComparisonType == a_OR_b             ) return bool(_a) || bool(_b);
  if (template_ComparisonType == a_GT_b__AND__c_EQ_d) return (_a > _b) && (_c == _d);
  if (template_ComparisonType == a_LT_b__AND__c_EQ_d) return (_a < _b) && (_c == _d);
  if (template_ComparisonType == a_LT_b__AND__c_LT_d) return (_a < _b) && (_c < _d);
  if (template_ComparisonType == a_GT_b__AND__c_GT_d) return (_a > _b) && (_c > _d);
  if (template_ComparisonType == a_GT_b__OR__c_LT_d ) return (_a > _b) || (_c < _d);
  if (template_ComparisonType == a_NEQ_b            ) return _a != _b;
  if (template_ComparisonType == a_TRUE             ) return bool(_a);
  if (template_ComparisonType == a_EQ_b__AND__c_EQ_d) return (_a == _b) && (_c == _d);
  if (template_ComparisonType == a_OR_b_OR_c        ) return bool(_a) || bool(_b) || bool(_c);
  if (template_ComparisonType == a_OR_b_OR_c_OR_d   ) return bool(_a) || bool(_b) || bool(_c) || bool(_d);
  if (template_ComparisonType == a_AND_b_AND_c      ) return bool(_a) && bool(_b) && bool(_c);
  MSG_INFO("Error - got to end of cut item without comparison!");
  return false;
}

//-----------------------------------------------------------------------------
std::string PSL::ConvertCutTemplateToStr(CutTemplate c){
  switch(c)
    {
    case a_EQ_b               : return "a_EQ_b"             ;
    case a_LT_b               : return "a_LT_b"             ;
    case a_GT_b               : return "a_GT_b"             ;
    case a_LEQ_b              : return "a_LEQ_b"            ;
    case a_GEQ_b              : return "a_GEQ_b"            ;
    case a_AND_b              : return "a_AND_b"            ;
    case a_OR_b               : return "a_OR_b"             ;
    case a_GT_b__AND__c_EQ_d  : return "a_GT_b__AND__c_EQ_d";
    case a_LT_b__AND__c_EQ_d  : return "a_LT_b__AND__c_EQ_d";
    case a_LT_b__AND__c_LT_d  : return "a_LT_b__AND__c_LT_d";
    case a_GT_b__AND__c_GT_d  : return "a_GT_b__AND__c_GT_d";
    case a_GT_b__OR__c_LT_d   : return "a_GT_b__OR__c_LT_d" ;
    case a_NEQ_b              : return "a_NEQ_b"            ;
    case a_TRUE               : return "a_TRUE"             ;
    case a_EQ_b__AND__c_EQ_d  : return "a_EQ_b__AND__c_EQ_d";
    case a_OR_b_OR_c          : return "a_OR_b_OR_c"        ;
    case a_OR_b_OR_c_OR_d     : return "a_OR_b_OR_c_OR_d"   ;
    case a_AND_b_AND_c        : return "a_AND_b_AND_c"      ;
    default : break;
    }
  return "";
}

//-----------------------------------------------------------------------------
PSL::CutTemplate PSL::ConvertToCutTemplate(const std::string& cut){
  if (cut == "a_EQ_b"             ) return a_EQ_b             ;
  if (cut == "a_LT_b"             ) return a_LT_b             ;
  if (cut == "a_GT_b"             ) return a_GT_b             ;
  if (cut == "a_LEQ_b"            ) return a_LEQ_b            ;
  if (cut == "a_GEQ_b"            ) return a_GEQ_b            ;
  if (cut == "a_AND_b"            ) return a_AND_b            ;
  if (cut == "a_OR_b"             ) return a_OR_b             ;
  if (cut == "a_GT_b__AND__c_EQ_d") return a_GT_b__AND__c_EQ_d;
  if (cut == "a_LT_b__AND__c_EQ_d") return a_LT_b__AND__c_EQ_d;
  if (cut == "a_LT_b__AND__c_LT_d") return a_LT_b__AND__c_LT_d;
  if (cut == "a_GT_b__AND__c_GT_d") return a_GT_b__AND__c_GT_d;
  if (cut == "a_GT_b__OR__c_LT_d" ) return a_GT_b__OR__c_LT_d ;
  if (cut == "a_NEQ_b"            ) return a_NEQ_b            ;
  if (cut == "a_TRUE"             ) return a_TRUE             ;
  if (cut == "a_EQ_b__AND__c_EQ_d") return a_EQ_b__AND__c_EQ_d;
  if (cut == "="  ) return a_EQ_b             ;
  if (cut == "<"  ) return a_LT_b             ;
  if (cut == ">"  ) return a_GT_b             ;
  if (cut == "@"  ) return a_LEQ_b            ;
  if (cut == "#"  ) return a_GEQ_b            ;
  if (cut == "&"  ) return a_AND_b            ;
  if (cut == "|"  ) return a_OR_b             ;
  if (cut == ">&=") return a_GT_b__AND__c_EQ_d;
  if (cut == "<&=") return a_LT_b__AND__c_EQ_d;
  if (cut == "<&<") return a_LT_b__AND__c_LT_d;
  if (cut == ">&>") return a_GT_b__AND__c_GT_d;
  if (cut == ">|<") return a_GT_b__OR__c_LT_d ;
  if (cut == "$"  ) return a_NEQ_b            ;
  if (cut == ""   ) return a_TRUE             ;
  if (cut == "=&=") return a_EQ_b__AND__c_EQ_d;
  if (cut == "||" ) return a_OR_b_OR_c        ;
  if (cut == "|||") return a_OR_b_OR_c_OR_d   ;
  if (cut == "&&") return a_AND_b_AND_c       ;
  MSG_INFO("Error! No cut template known for " << cut);
  exit(1);
  return a_EQ_b;
}

//-----------------------------------------------------------------------------
void PSL::CutItem::PrintConfig(void) {
  MSG_INFO(  "Name                  : " << GetName());
  MSG_INFO(  "doTemplatedComparison : " << (doTemplatedComparison ? "yes" : "no"));
  if (doTemplatedComparison) {
    MSG_INFO("Comparison Type       : " << ConvertCutTemplateToStr(template_ComparisonType));
    MSG_INFO("m_var1                : " << (m_var1 == vNone ? "None" : ConvertVarToStr(m_var1)) << " " << m_number1);
    MSG_INFO("m_var2                : " << (m_var2 == vNone ? "None" : ConvertVarToStr(m_var2)) << " " << m_number2);
    MSG_INFO("m_var3                : " << (m_var3 == vNone ? "None" : ConvertVarToStr(m_var3)) << " " << m_number3);
    MSG_INFO("m_var4                : " << (m_var4 == vNone ? "None" : ConvertVarToStr(m_var4)) << " " << m_number4);
  }
  MSG_INFO(  "doCompareSubCutItems  : " << (doCompareSubCutItems ? "yes" : "no"));
  if (doCompareSubCutItems){
    MSG_INFO("ComparisonType        : " << ConvertCutTemplateToStr(subCut_ComparisonType));
  }
  MSG_INFO(  "weight                : " << (weight == vNone ? "None" : ConvertVarToStr(weight)));
}

