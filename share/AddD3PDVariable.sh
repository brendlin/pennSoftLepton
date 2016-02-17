#!/bin/bash

echo ~~~
echo Please call this from the pennSoftLepton directory, e.g. source share/AddD3PDVariable.sh
echo First argument must be the type. Second argument is the variable name.
echo "options are uint, float, v_float, v_int"
echo $1 $2
echo ~~~

if grep -q $2 pennSoftLepton/D3PDVariables.h; then
    echo "Variable already exists. Doing nothing."
    return
fi

bot1="WILLNOTDOANYTHING"
bot1_slimmer="WILLNOTDOANYTHING"
the_type="NONETYPE"
the_branchtype="NONETYPE"

if [ $1 == "uint" ]; then
    bot1="N_UInt_t"
    the_type="UInt_t"
    the_branchtype='i'
fi

if [ $1 == "bool" ]; then
    bot1="N_Bool_t"
    the_type="Bool_t"
    the_branchtype="O"
fi

if [ $1 == "int" ]; then
    bot1="N_int" 
    the_type="int"
fi

if [ $1 == "float" ]; then
    bot1="N_float" 
    the_type="float"
    the_branchtype="F"
fi

if [ $1 == "v_float" ]; then
    bot1="N_vector_float"
fi

if [ $1 == "v_int" ]; then
    bot1="N_vector_int"
fi

if [ $1 == "v_v_uint" ]; then
    bot1="N_vector_vector_UInt_t"
fi

if [ $1 == "v_v_int" ]; then
    bot1="N_vector_vector_int"
fi

if [ $1 == "v_uint" ]; then
    bot1="N_vector_UInt_t"
fi

if [ $1 == "v_v_float" ]; then
    bot1="N_vector_vector_float"
fi

theline1="      ,$bot1"
sed -i "s/$theline1/      ,$2\\
$theline1/g" pennSoftLepton/D3PDVariables.h

theline2="  default: break; \/\/ $bot1"
sed -i "s/$theline2/  case D3PD::$2  : return \"$2\" ;\\
$theline2/g" Root/D3PDVariables.cxx

theline3="    \/\/ Add $bot1"
sed -i "s/$theline3/    $the_type $2; \/\/!\\
$theline3/g" pennSoftLepton/D3PDWrapper.h

theline4="  \/\/ Add $bot1"
sed -i "s/$theline4/  SetBranchStatusAddress(tree,\"$2\" ,\&$2 );\\
$theline4/g" Root/D3PDWrapper.cxx

theline5="  \/\/ Add $bot1"
sed -i "s/$theline5/  tree->Branch(\"$2\",\&(m_EDM->$2  ),\"$2\/$the_branchtype\"  );\\
$theline5/g" Root/SlimmerD3PD.cxx

# f = TFile('/Users/kurtbrendlinger/Temporary/d3pd//mc12_8TeV.126931.PowhegPythia8_AU2CT10_WpWm_em.merge.NTUP_COMMON.e1280_s1469_s1470_r3542_r3549_p1575_tid01344124_00/NTUP_COMMON.01344124._000001.root.1')
# e = f.Get('physics')
# file = open('tmp.txt').readlines()
# for i in file :
#   i = i.replace('\n','')
#   if hasattr(e.GetBranch(i),'GetTypeName') : print e.GetBranch(i).GetTypeName().replace('vector<vector<float> >','v_v_float').replace('vector<float>','v_float').replace('vector<int>','v_int'),i
#   else : print i
