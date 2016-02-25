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
        if GetFFHist('all.root','',r,c,p='tw',num_or_den='Num') :
            hists_ttbar[r][c].Add(GetFFHist('all.root','',r,c,p='tw',num_or_den='Num'))
        nevt_ttbar[r][c] = hists_ttbar[r][c].Integral(0,hists_ttbar[r][c].GetNbinsX()+1)
        err_ttbar[r][c] = math.sqrt(sum(list(hists_ttbar[r][c].GetSumw2())))

        print 'U'
        if c != 'eee' :
            hists_ttbar_u[r][c] = GetFFHist('../%sTtbarSR_Ufake/all.root'%(wpwm),'',r,c,p='ttbar',num_or_den='Num')
            if GetFFHist('../%sTtbarSR_Ufake/all.root'%(wpwm),'',r,c,p='tw',num_or_den='Num') :
                hists_ttbar_u[r][c].Add(GetFFHist('../%sTtbarSR_Ufake/all.root'%(wpwm),'',r,c,p='tw',num_or_den='Num'))
            nevt_ttbar_u[r][c] = hists_ttbar_u[r][c].Integral(0,hists_ttbar_u[r][c].GetNbinsX()+1)
            err_ttbar_u[r][c] = math.sqrt(sum(list(hists_ttbar_u[r][c].GetSumw2())))
        else :
            nevt_ttbar_u[r][c] = 0
            err_ttbar_u[r][c] = 0

        print 'E'
        if c != 'uuu' :
            hists_ttbar_e[r][c] = GetFFHist('../%sTtbarSR_Efake/all.root'%(wpwm),'',r,c,p='ttbar',num_or_den='Num')
            if GetFFHist('../%sTtbarSR_Efake/all.root'%(wpwm),'',r,c,p='tw',num_or_den='Num') :
                hists_ttbar_e[r][c].Add(GetFFHist('../%sTtbarSR_Efake/all.root'%(wpwm),'',r,c,p='tw',num_or_den='Num'))
            nevt_ttbar_e[r][c] = hists_ttbar_e[r][c].Integral(0,hists_ttbar_e[r][c].GetNbinsX()+1)
            err_ttbar_e[r][c] = math.sqrt(sum(list(hists_ttbar_e[r][c].GetSumw2())))
        else :
            nevt_ttbar_e[r][c] = 0
            err_ttbar_e[r][c] = 0
        

print '\\ttbar MC, $\mu$-fake   &',
for c in channels :
    r = 'ttt'
    print '%2.2f%s%2.2f &'%(nevt_ttbar_u[r][c],pm,err_ttbar_u[r][c]),
print
print '\\ttbar MC, $e$-fake     &',
for c in channels :
    r = 'ttt'
    print '%2.2f%s%2.2f &'%(nevt_ttbar_e[r][c],pm,err_ttbar_e[r][c]),
print
print '\\ttbar MC, total        &',
for c in channels :
    r = 'ttt'
    print '%2.2f%s%2.2f &'%(nevt_ttbar[r][c],pm,err_ttbar[r][c]),
    #print '%2.2f &'%(nevt_ttbar[r][c]),
    #print '%2.2f &'%(err_ttbar[r][c]),
print

print 'statistical errors on SF, propagated to the SR:'
print '$\mu$ SF Uncertainty           &',
for c in channels : # 1.44$\pm$0.48$\pm$0.17
    r = 'ttt'
    print '%2.2f &'%(math.sqrt(0.48**2+0.17**2)*nevt_ttbar_u[r][c]/1.44),
print
print 'ele SF Uncertainty             &',
for c in channels : # 0.72$\pm$0.35$\pm$0.07
    r = 'ttt'
    print '%2.2f &'%(math.sqrt(0.35**2+0.07**2)*nevt_ttbar_e[r][c]/0.72),
print
