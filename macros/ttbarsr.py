#!/usr/bin/env python

import ROOT
import pennSoftLepton.PyAnalysisPlotting as anaplot
import math,os

anaplot.LoadRootCore()

wpwm = ''
if 'wp_' in os.getcwd() : wpwm = 'wp_'
if 'wm_' in os.getcwd() : wpwm = 'wm_'

#
# expand "region" to include ltt, tlt, ttl, all
#
def GetFFHist(f,k,r,c,p='ttbar',v='MTW',num_or_den='Den') : # file key region channel process variable
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

def AddIf(hist,f,k,r,c,p='ttbar',v='MTW',num_or_den='Den') :
    h = GetFFHist(f,k,r,c,p,v,num_or_den)
    if h :
        hist.Add(h)
    return

pm = "$\pm$"
channels = ['eee','euu','uee','uuu','all']
regions = ['ttt']

hists_ttbar = dict()
nevt_ttbar = dict()
err_ttbar = dict()

hists_ttbar_u = dict()
nevt_ttbar_u = dict()
err_ttbar_u = dict()

hists_ttbar_e = dict()
nevt_ttbar_e = dict()
err_ttbar_e = dict()


for r in regions :
    hists_ttbar[r] = dict()
    nevt_ttbar[r] = dict()
    err_ttbar[r] = dict()

    hists_ttbar_u[r] = dict()
    nevt_ttbar_u[r] = dict()
    err_ttbar_u[r] = dict()

    hists_ttbar_e[r] = dict()
    nevt_ttbar_e[r] = dict()
    err_ttbar_e[r] = dict()

    for c in channels :
        print r,c
        num_or_den = 'Num'

        print 'nominal'
        hists_ttbar[r][c] = GetFFHist('all.root','',r,c,p='ttbar',num_or_den='Num')
        AddIf(hists_ttbar[r][c],'all.root','',r,c,p='tw',num_or_den='Num')
        AddIf(hists_ttbar[r][c],'all.root','',r,c,p='qqww',num_or_den='Num')
        nevt_ttbar[r][c] = hists_ttbar[r][c].Integral(0,hists_ttbar[r][c].GetNbinsX()+1)
        err_ttbar[r][c] = math.sqrt(sum(list(hists_ttbar[r][c].GetSumw2())))

        print 'U'
        if c != 'eee' :
            hists_ttbar_u[r][c] = GetFFHist('../%sTtbarSR_Ufake/all.root'%(wpwm),'',r,c,p='ttbar',num_or_den='Num')
            AddIf(hists_ttbar_u[r][c],'../%sTtbarSR_Ufake/all.root'%(wpwm),'',r,c,p='tw',num_or_den='Num')
            AddIf(hists_ttbar_u[r][c],'../%sTtbarSR_Ufake/all.root'%(wpwm),'',r,c,p='qqww',num_or_den='Num')
            nevt_ttbar_u[r][c] = hists_ttbar_u[r][c].Integral(0,hists_ttbar_u[r][c].GetNbinsX()+1)
            err_ttbar_u[r][c] = math.sqrt(sum(list(hists_ttbar_u[r][c].GetSumw2())))
        else :
            nevt_ttbar_u[r][c] = 0
            err_ttbar_u[r][c] = 0

        print 'E'
        if c != 'uuu' :
            hists_ttbar_e[r][c] = GetFFHist('../%sTtbarSR_Efake/all.root'%(wpwm),'',r,c,p='ttbar',num_or_den='Num')
            AddIf(hists_ttbar_e[r][c],'../%sTtbarSR_Efake/all.root'%(wpwm),'',r,c,p='tw',num_or_den='Num')
            AddIf(hists_ttbar_e[r][c],'../%sTtbarSR_Efake/all.root'%(wpwm),'',r,c,p='qqww',num_or_den='Num')
            nevt_ttbar_e[r][c] = hists_ttbar_e[r][c].Integral(0,hists_ttbar_e[r][c].GetNbinsX()+1)
            err_ttbar_e[r][c] = math.sqrt(sum(list(hists_ttbar_e[r][c].GetSumw2())))
        else :
            nevt_ttbar_e[r][c] = 0
            err_ttbar_e[r][c] = 0
        

print '\\ttbar MC$\\times$SF, $\mu$-fake   &',
for c in channels :
    r = 'ttt'
    print '%2.2f%s%2.2f &'%(nevt_ttbar_u[r][c],pm,err_ttbar_u[r][c]),
print
print '\\ttbar MC$\\times$SF, $e$-fake     &',
for c in channels :
    r = 'ttt'
    print '%2.2f%s%2.2f &'%(nevt_ttbar_e[r][c],pm,err_ttbar_e[r][c]),
print
print '\\ttbar MC$\\times$SF, total        &',
for c in channels :
    r = 'ttt'
    print '%2.2f%s%2.2f &'%(nevt_ttbar[r][c],pm,err_ttbar[r][c]),
    #print ' %2.2f  &'%(nevt_ttbar[r][c]),
    #print ' %2.2f  &'%(err_ttbar[r][c]),
print

# This should be good for W+Z, W-Z, All
print 'statistical errors on SF, propagated to the SR:'
print '$\mu$-fake Uncertainty           &',
for c in channels : # 1.44$\pm$0.48$\pm$0.17 ---> 1.41$\pm$0.49$\pm$0.17
    r = 'ttt'
    print ' %2.2f  &'%(math.sqrt(0.49**2+0.17**2)*nevt_ttbar_u[r][c]/1.41),
print
print '$e$-fake Uncertainty             &',
for c in channels : # 0.72$\pm$0.35$\pm$0.07 ---> 0.54$\pm$0.32$\pm$0.05
    r = 'ttt'
    print ' %2.2f  &'%(math.sqrt(0.32**2+0.05**2)*nevt_ttbar_e[r][c]/0.54),
print

# For the Wp and Wm inputs, there was a small typo. The results are fairly
# similar, but the correct results should be (v5 of the support note)
# wp
# $\mu$-fake Uncertainty           &  0.00  &  0.40  &  0.10  &  0.83  &  1.33  & %%% MISTAKE IT SHOULD BE THIS!!!
# wm
# $\mu$-fake Uncertainty           &  0.00  &  0.27  &  0.20  &  0.49  &  0.96  & %%% MISTAKE IT SHOULD BE THIS!!!

