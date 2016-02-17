#!/bin/bash

echo ~~~
echo "Please call this from the pennSoftLepton directory, e.g. source share/AddAnalysisVariable.sh"
echo "First argument is the variable name. Second is if it is an integer (just say int)"
echo "You must then go into xAODWrapper.cxx or D3PDWrapper.cxx and edit GetEventVariable"
echo $1 $2
echo ~~~

if [ $# -lt 1 ]; then
    echo "Did nothing."
    return
fi

isinteger=""
if [ $# -gt 1 ]; then # if number of arguments >1
    isinteger=",true"
fi

theline1="  }; \/\/ Add_new_variables_here"

sed -i "s/$theline1/    ,v$1\\
$theline1/g" pennSoftLepton/Variables.h

theline2="  } \/\/ Add_new_variables_here"

#sed -i "s/$theline2/    pairs.push_back(std::make_pair(v$1,\"$1\"));\\
sed -i "s/$theline2/    p.push_back(var_props(v$1,\"$1\" ,\"$1\" ,  10,    0,   10$isinteger));\\
$theline2/g" Root/Variables.cxx

#
# add to list of integers
#
# if [ $# -gt 1 ]; then # if number of arguments >1
#     echo "Adding it as an integer."
#     theline3="  \/\/ Add_new_variables_here"
#     sed -i "s/$theline3/  if (v == v$1 ) return true;\\
# $theline3/g" Root/Variables.cxx    
# fi

#
# Add to GetEventVariable (needs to be edited)
#
theline4="  \/\/ Add_new_variables_here to GetEventVariable"

sed -i "s/$theline4/  else if (v == v$1      ) d = 0;\\
$theline4/g" Root/D3PDWrapper.cxx

sed -i "s/$theline4/  else if (v == v$1      ) d = 0;\\
$theline4/g" Root/xAODWrapper.cxx

#
# Add dummy binning (needs to be edited)
#
# theline5="  \/\/ Add_variable_binning_here"

# sed -i "s/$theline5/  else if (v == v$1 ) {out.nbins = 10; out.xlow = 0; out.xup = 10;}\\
# $theline5/g" Root/Variables.cxx
