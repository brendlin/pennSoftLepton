#!/usr/bin/env python

import ROOT
import pennSoftLepton.PyAnalysisPlotting as anaplot
import math
from array import array

anaplot.LoadRootCore()

version_emmanuel = True

variables = []
if version_emmanuel :
    variables = ['PtW','MTW','M3l','Mll','met_Et','NJets','PtllZ','YZminusYlW','MtWZ',]
#     variables = ['Mll']
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
DeltaPhiZWSyst Pt3lPlusMet RunNumber                                   \
Mll_ss EvtTrigSF \
AntiIDSF_lep3 AntiIDSF_lep3_pt AntiIDSF_lep3_eta AntiIDSF_lep3_phi AntiIDSF_lep3_flavor \
'
#PtW MtWZ YZminusYlW MtZ PtWZ EvtTrigSF \
#
# expand "region" to include ltt, tlt, ttl, all
#
def GetFFHistIfExists(f,k,r,c,p='ttbar',v='MTW',num_or_den='Den') : # file key region channel process variable
    other = '%s_%s'%(p,v)
    rtmp = r
    if r == 'all' :
        rtmp = 'ltt'
    keyregionchannel = 'PassEvent_FFTool_%s_%s_%s%s'%(k,rtmp,c,num_or_den)
    #print 'Adding','%s/%s_%s'%(keyregionchannel,keyregionchannel,other)
    print 'Adding',('FFTool_%s_%s_%s%s'%(k,rtmp,c,num_or_den)).replace('__','_')
    hists = anaplot.GetPassEventBkgHistos(v,('FFTool_%s_%s_%s%s'%(k,rtmp,c,num_or_den)).replace('__','_'),p,f)
    if not hists :
        return 0
    hist = hists[0]
    if r == 'all' and c not in ['euu','uee'] :
        hist.Add(GetFFHist(f,k,'tlt',c,p,v,num_or_den))
        hist.Add(GetFFHist(f,k,'ttl',c,p,v,num_or_den))
    return hist
    #return pyhelpers.GetRootObj(f,'%s/%s_%s'%(keyregionchannel,keyregionchannel,other))

def GetFFHist(f,k,r,c,p='ttbar',v='MTW',num_or_den='Den') : # file key region channel process variable
    other = '%s_%s'%(p,v)
    rtmp = r
    if r == 'all' :
        rtmp = 'ltt'
    keyregionchannel = 'PassEvent_FFTool_%s_%s_%s%s'%(k,rtmp,c,num_or_den)
    #print 'Adding','%s/%s_%s'%(keyregionchannel,keyregionchannel,other)
    print 'Adding',('FFTool_%s_%s_%s%s'%(k,rtmp,c,num_or_den)).replace('__','_')
    hist = anaplot.GetPassEventBkgHistos(v,('FFTool_%s_%s_%s%s'%(k,rtmp,c,num_or_den)).replace('__','_'),p,f)[0]
    if r == 'all' and c not in ['euu','uee'] :
        hist.Add(GetFFHist(f,k,'tlt',c,p,v,num_or_den))
        hist.Add(GetFFHist(f,k,'ttl',c,p,v,num_or_den))
    
    return hist
    #return pyhelpers.GetRootObj(f,'%s/%s_%s'%(keyregionchannel,keyregionchannel,other))

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
        if (GetFFHist('all.root','z_data','all',c,p='tw',v=v,num_or_den='Num')) :
            hists[-1].Add(GetFFHist('all.root','z_data','all',c,p='tw',v=v,num_or_den='Num'),-1)

        #
        # subtract wz
        #
        hists[-1].Add(GetFFHist('all.root','z_data','all',c,p='wz',v=v,num_or_den='Num'),-1)
        hists[-1].Add(GetFFHist('all.root','z_data','all',c,p='zz',v=v,num_or_den='Num'),-1)

        #
        # Add signal ttbar
        #
        if version_emmanuel :
            hists[-1].Add(GetFFHist('all.root','','ttt',c,p='ttbar',v=v,num_or_den='Num'))
            if GetFFHistIfExists('all.root','','ttt',c,p='tw'   ,v=v,num_or_den='Num') :
                hists[-1].Add(GetFFHist('all.root','','ttt',c,p='tw'   ,v=v,num_or_den='Num'))
            
            hists[-1].SetName('datadriven_ff_plus_ttbar_%s_%s_tmp'%(v,c))
            hists[-1].SetTitle('datadriven_ff_plus_ttbar_%s_%s_tmp'%(v,c))

        else :
            hists[-1].SetName('datadriven_ff_zjetonly_%s_%s'%(v,c))
            hists[-1].SetTitle('datadriven_ff_zjetonly_%s_%s'%(v,c))

        if not version_emmanuel :
            hists.append(GetFFHist('all.root','','ttt',c,p='ttbar',v=v,num_or_den='Num'))
            if GetFFHistIfExists('all.root','','ttt',c,p='tw',v=v,num_or_den='Num') :
                hists[-1].Add(GetFFHist('all.root','','ttt',c,p='tw',v=v,num_or_den='Num'))
            hists[-1].SetName('ttbar_estimate_%s_%s'%(v,c))
            hists[-1].SetTitle('ttbar_estimate_%s_%s'%(v,c))
            
            

hists1 = []

somedict = {
    'PtW':[40,0,400],
    'MTW':[20,0,200],
    'M3l':[32,0.,800.],
    'Mll':[30,75,105],
    'met_Et':[18,0,180],
    'NJets':[11,-0.5,10.5],
    'PtllZ':[40,0.,400.],
    'MtWZ':[28,0,700.],
#     'YZminusYlW':[10,-5,5],
    }

for h in hists :
    copied = False
    print h.GetName()
    for tmpvar in somedict.keys() :
        if tmpvar not in h.GetName() :
            continue
        if not version_emmanuel :
            continue
        nlowhigh = somedict[tmpvar]
        Rebin(h,*nlowhigh)
        hists1.append(ROOT.TH1F(h.GetName().replace('_tmp',''),h.GetTitle().replace('_tmp',''),*nlowhigh))
        Copy(h,hists1[-1],*nlowhigh)
        copied = True

    if 'YZminusYlW' in h.GetName() and version_emmanuel and not copied :
        arrr = [0.,0.4,0.8,1.4,2.2,5.]
        #arrr = [0.,1.,3.,5.]
        #arrr = list(a for a in range(0,6))
        print arrr
        htmp = ROOT.TH1F(h.GetName().replace('_tmp',''),h.GetTitle().replace('_tmp',''),len(arrr)-1,array('d',arrr))
        for i in range(len(arrr[:-1])) :
            content = 0
            err2 = 0
            for j in range(h.GetNbinsX()) :
                bcent = math.fabs(h.GetBinCenter(j+1))
                if arrr[i] < bcent and bcent < arrr[i+1] :
                    content += h.GetBinContent(j+1)
                    err2 += h.GetBinError(j+1)**2
            htmp.SetBinContent(i+1,content)
            htmp.SetBinError(i+1,math.sqrt(err2))
        hists1.append(htmp)
        copied = True

    if not copied :
        hists1.append(h)

for h in hists1 :
    print h.GetName(),h.Integral(0,h.GetNbinsX()+1)
    g.cd()
    h.Write()

g.Close()
# f.Close()
