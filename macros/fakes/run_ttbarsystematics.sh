#!/usr/bin/env sh

# 1. link all the ttbar systematics root files to the current directory.
# 2. run this script
# 3. profit!

for i in $(ls); do
    if [[ $i != *".root"* ]]
    then
        # echo $i is not a root file.;
        continue;
    fi

    if [ -d ${i/.root/} ]; then
        echo ${i/.root/} has already been run.;
        continue;
    fi
  echo '##########################################################################################################################################################################'
  echo '##########################################################################################################################################################################'
  echo '##########################################################################################################################################################################'
  echo '##########################################################################################################################################################################'
  echo '##########################################################################################################################################################################'
  echo $i;
  ln -s $i ttbar.root
  echo ntupleJob.py --bkgs ttbar --config FakeFactorZjet.conf --out ${i/.root/} --instances FFTool_z_data_ltt,FFTool_z_data_tlt,FFTool_z_data_ttl,FFTool_z_ttvalid_ltt,FFTool_z_ttvalid_tlt,FFTool_z_ttvalid_ttl,FFTool_z_est_tt_ltt,FFTool_z_est_tt_tlt,FFTool_z_est_tt_ttl
  ntupleJob.py --bkgs ttbar --config FakeFactorZjet.conf --out ${i/.root/} --instances FFTool_z_data_ltt,FFTool_z_data_tlt,FFTool_z_data_ttl,FFTool_z_ttvalid_ltt,FFTool_z_ttvalid_tlt,FFTool_z_ttvalid_ttl,FFTool_z_est_tt_ltt,FFTool_z_est_tt_tlt,FFTool_z_est_tt_ttl
  rm ttbar.root
done;

# ntupleJob.py --bkgs zjee,zjmm,zjtt,zgam,ttbar,wz,zz,ttv,tz,data --config FakeFactorZjet.conf --out 

