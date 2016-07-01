#!/usr/bin/env python

import ROOT
import pennSoftLepton.PyAnalysisPlotting as anaplot
import math
from array import array
from pennSoftLepton.PyRandomStuff import GetFFHist,AddFFHistIf

anaplot.LoadRootCore()

version_emmanuel = True

variables = []
if version_emmanuel :
    variables = ['PtW','MTW','M3l','Mll','met_Et','NJets','NJetsLtEta2p5','PtllZ','PtllZ_Rebinned','YZminusYlW','MtWZ','DeltaPhiZWSyst','MWZ','NPV','jetPt','jetRapidity']
else :
    variables = list(ROOT.PSL.GetAllVariableStrings())
    for i in ['lepPt','lepEta','lepCharge'] :
        for j in ['lep0_index','lep1_index','lep2_index',
                  'lepW_index','lepleadZ_index','lepsubleadZ_index'] :
            variables += ['%s[%s]'%(i,j)]

skip_them = '\
nBaselineMuons nBaselineElectrons met_sumet jet0_index jet1_index \
DeltaRll PassTriggerEle PassTriggerMu PassTriggerMatch \
AntiIDSF_lep3 AntiIDSF_lep3_pt AntiIDSF_lep3_eta AntiIDSF_lep3_phi AntiIDSF_lep3_flavor \
AntiIDSF_lep3_pt_reco AntiIDSF_lep3_eta_reco AntiIDSF_lep3_phi_reco AntiIDSF_lep3_flavor_reco \
AntiIDSF_lep3_match_DR AntiIDSF_lep3_origin AntiIDSF_lep3_type NlepsfromZ AntiIDSF_lep3_truth_origin \
AntiIDSF_lep3_truth_type XCheckAntiIDSF_lep3_pt XCheckAntiIDSF_lep3_eta XCheckAntiIDSF_lep3_phi \
XCheckAntiIDSF_lep3_flavor XCheckAntiIDSF_lep3_origin XCheckAntiIDSF_lep3_type \
passLarError passTileError passSctError passCoreBit passPrimaryVtx     \
passCosmicMuon passTileTripReader passIncompleteEvents                 \
passTileCalHotSpot passBadJet passCaloJet passBadMuon passHFOR         \
passMCOverlap passBCH MuIsValid                                        \
NSignalTaus refEle refEle_etx refEle_ety refEle_sumet refMuo           \
refMuo_etx refMuo_ety refMuo_sumet refJet refJet_etx refJet_ety        \
refJet_sumet refGamma refGamma_etx refGamma_ety refGamma_sumet         \
softTerm softTerm_etx softTerm_ety softTerm_sumet lepleadMinSFOS_index \
lepsubleadMinSFOS_index lepWMinSFOS_index PtllZmin MWjj MjjMin MinMll  \
MTWmin NCentralJets NForwardJets DeltaRllZmin METoverPtj0 PtlloverMET  \
PtllZoverMET PtllZminoverMET MinDeltaPhilMET MinDeltaPhi3LMET          \
MaxDeltaPhi3LMET MaxDeltaPhilJet MaxDeltaPhi3LJet MinDeltaPhi3LJet     \
DeltaPhi3LJet DeltaPhi3LMET DeltaPhiMETJet Mll_Upsilon truthjet0Pt     \
truthjet1Pt nVetoLeptons ptAvg truthptAvg ptAvgOverTruthjet0Pt         \
DeltaPhill DeltaPhiLeadZWLep DeltaPhiZLeps DeltaPhiZWLep DeltaPhiZMet  \
Pt3lPlusMet RunNumber \
Mll_ss EvtTrigSF \
AntiIDSF_lep3 AntiIDSF_lep3_pt AntiIDSF_lep3_eta AntiIDSF_lep3_phi AntiIDSF_lep3_flavor \
'
#PtW MtWZ YZminusYlW MtZ PtWZ EvtTrigSF \

def RebinVariableBinning(h,thelist) :
    return

def Rebin(h,n,low,high) :
    desired_width = (high-low)/float(n)
    axis = h.GetXaxis()
    nbinsx = h.GetNbinsX()
    current_width = (axis.GetBinLowEdge(nbinsx+1)-axis.GetBinLowEdge(1))/float(nbinsx)
    rebin_factor = desired_width/current_width
    print 'desired:',n,low,high,desired_width
    print 'current:',nbinsx,axis.GetBinLowEdge(1),axis.GetBinLowEdge(nbinsx+1),current_width
    if math.fabs(rebin_factor - int(rebin_factor)) > 0.000001 :
        print 'not consistent'
        import sys
        sys.exit()
    h.Rebin(int(rebin_factor))

def Copy(hin,hout,n,low,high) :
    print hin.GetName(),n,low,high
    binbegin = hin.FindBin(low-1)
    binend = hin.FindBin(high+1)
    outi = 0
    print '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~',binbegin,binend
    for i in range(binbegin,binend) :
        hout.SetBinContent(outi,hin.GetBinContent(i))
        hout.SetBinError(outi,hin.GetBinError(i))
        outi += 1

# f = ROOT.TFile('all.root')
g = ROOT.TFile('out.root','recreate')

channels = ['all','eee','euu','uee','uuu']

hists = []
region = 'all'


for v in variables :
    if v in skip_them.split() : continue
    for c in channels :
        #
        # z_data (data times FF
        #
        hists.append(GetFFHist('all.root','z_data','all',c,p='zjetdd',v=v,num_or_den='Num'))
        #
        # subtract ttbar
        #
        hists[-1].Add(GetFFHist('all.root','z_data','all',c,p='ttbar',v=v,num_or_den='Num'),-1)
        AddFFHistIf(hists[-1],'all.root','z_data','all',c,p='tw',v=v,num_or_den='Num',scale=-1)
        AddFFHistIf(hists[-1],'all.root','z_data','all',c,p='qqww',v=v,num_or_den='Num',scale=-1)
        #
        # subtract wz
        #
        hists[-1].Add(GetFFHist('all.root','z_data','all',c,p='wz',v=v,num_or_den='Num'),-1)
        hists[-1].Add(GetFFHist('all.root','z_data','all',c,p='zz',v=v,num_or_den='Num'),-1)

        #
        # Add signal ttbar
        #
        hists[-1].Add(GetFFHist('all.root','','ttt',c,p='ttbar',v=v,num_or_den='Num'))
        AddFFHistIf(hists[-1],'all.root','','ttt',c,p='tw',v=v,num_or_den='Num')
        AddFFHistIf(hists[-1],'all.root','','ttt',c,p='qqww',v=v,num_or_den='Num')
        
        hists[-1].SetName('datadriven_ff_plus_ttbar_%s_%s'%(v,c))
        hists[-1].SetTitle('datadriven_ff_plus_ttbar_%s_%s'%(v,c))

for v in variables :
    if v in skip_them.split() : continue
    for c in channels :
        #
        # Add the ttbar estimate alone
        #
        hists.append(GetFFHist('all.root','','ttt',c,p='ttbar',v=v,num_or_den='Num'))
        AddFFHistIf(hists[-1],'all.root','','ttt',c,p='tw',v=v,num_or_den='Num')
        AddFFHistIf(hists[-1],'all.root','','ttt',c,p='qqww',v=v,num_or_den='Num')
        hists[-1].SetName('ttbar_estimate_%s_%s'%(v,c))
        hists[-1].SetTitle('ttbar_estimate_%s_%s'%(v,c))
            
for v in variables :
    if v in skip_them.split() : continue
    for c in channels :
        #
        # Zjet estimate alone
        #
        hists.append(GetFFHist('all.root','z_data','all',c,p='zjetdd',v=v,num_or_den='Num'))
        hists[-1].Add(GetFFHist('all.root','z_data','all',c,p='ttbar',v=v,num_or_den='Num'),-1)
        AddFFHistIf(hists[-1],'all.root','z_data','all',c,p='tw',v=v,num_or_den='Num',scale=-1)
        AddFFHistIf(hists[-1],'all.root','z_data','all',c,p='qqww',v=v,num_or_den='Num',scale=-1)
        hists[-1].Add(GetFFHist('all.root','z_data','all',c,p='wz',v=v,num_or_den='Num'),-1)
        hists[-1].Add(GetFFHist('all.root','z_data','all',c,p='zz',v=v,num_or_den='Num'),-1)
        hists[-1].SetName('zjet_estimate_%s_%s'%(v,c))
        hists[-1].SetTitle('zjet_estimate_%s_%s'%(v,c))


for h in hists :
    print '%s: %2.3f'%(h.GetName(),h.Integral(0,h.GetNbinsX()+1))
    g.cd()
    h.Write()

g.Close()
# f.Close()
