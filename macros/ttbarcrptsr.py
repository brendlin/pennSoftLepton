#!/usr/bin/env python

import ROOT
import pennSoftLepton.PyAnalysisPlotting as anaplot
import math

anaplot.LoadRootCore()
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
    hist = anaplot.GetPassEventBkgHistos(v,('FFTool_%s_%s_%s%s'%(k,rtmp,c,num_or_den)).replace('__','_'),p,f)[0]
    if r == 'all' and c not in ['euu','uee'] :
        hist.Add(GetFFHist(f,k,'tlt',c,p,v,num_or_den))
        hist.Add(GetFFHist(f,k,'ttl',c,p,v,num_or_den))
    return hist
    #return pyhelpers.GetRootObj(f,'%s/%s_%s'%(keyregionchannel,keyregionchannel,other))

pm = "$\pm$"
#channels = ['euu','uee','all']
channels = ['euu','uee']
regions = ['ttt']
#region = 'all'

hists_ttbar = dict()
nevt_ttbar = dict()
err_ttbar = dict()

hists_other = dict()
nevt_other = dict()
err_other = dict()

hists_total = dict()
nevt_total = dict()
err_total = dict()

hists_data = dict()
nevt_data = dict()
err_data = dict()

for r in regions :
    hists_ttbar[r] = dict()
    nevt_ttbar[r] = dict()
    err_ttbar[r] = dict()
    hists_other[r] = dict()
    nevt_other[r] = dict()
    err_other[r] = dict()
    hists_total[r] = dict()
    nevt_total[r] = dict()
    err_total[r] = dict()
    hists_data[r] = dict()
    nevt_data[r] = dict()
    err_data[r] = dict()

    for c in channels :
        print r,c
        num_or_den = 'Num'
        #
        # ttcr_ttt (data times FF
        #
        hists_ttbar[r][c] = GetFFHist('all.root','ttcr',r,c,p='ttbar',num_or_den='Num')
        nevt_ttbar[r][c] = hists_ttbar[r][c].Integral(0,hists_ttbar[r][c].GetNbinsX()+1)
        err_ttbar[r][c] = math.sqrt(sum(list(hists_ttbar[r][c].GetSumw2())))

        hists_other[r][c] = GetFFHist('all.root','ttcr',r,c,p='wz',num_or_den='Num')
        hists_other[r][c].Add(GetFFHist('all.root','ttcr',r,c,p='zz',num_or_den='Num'))
        hists_other[r][c].Add(GetFFHist('all.root','ttcr',r,c,p='ttv',num_or_den='Num'))
        hists_other[r][c].Add(GetFFHist('all.root','ttcr',r,c,p='tz',num_or_den='Num'))
        nevt_other[r][c] = hists_other[r][c].Integral(0,hists_other[r][c].GetNbinsX()+1)
        err_other[r][c] = math.sqrt(sum(list(hists_other[r][c].GetSumw2())))

        hists_total[r][c] = hists_other[r][c].Clone() # all the others
        hists_total[r][c].SetNameTitle('total_%s'%(c),'total_%s'%(c))
        hists_total[r][c].Add(hists_ttbar[r][c]) # add ttbar
        nevt_total[r][c] = hists_total[r][c].Integral(0,hists_total[r][c].GetNbinsX()+1)
        err_total[r][c] = math.sqrt(sum(list(hists_total[r][c].GetSumw2())))

        hists_data[r][c] = GetFFHist('all.root','ttcr',r,c,p='data',num_or_den='Num')
        nevt_data[r][c] = hists_data[r][c].Integral(0,hists_data[r][c].GetNbinsX()+1)
        err_data[r][c] = math.sqrt(sum(list(hists_data[r][c].GetSumw2())))

print 'Channel                                         & ',
print '\\ttbar MC     & ',
print 'Other MC      & ',
print 'Total MC      & ',
print 'Data          & ',
print '\\ttbar scale factor \\\\ \\hline'
for c in channels :
    if c == 'euu' :
        print '$\mu^\pm e^\mp \mu^\pm$+$\mu^\pm \mu^\pm e^\mp$ & ',
    else :
        print '$e^\pm e^\pm \mu^\mp$+$e^\pm \mu^\mp e^\pm$     & ',
    for r in regions :
        # print '%2.2f%s%2.2f%s%2.2f'%(nevt[r][c],pm,err[r][c],pm,syst[r][c]),
        print '%2.2f%s%2.2f &'%(nevt_ttbar[r][c],pm,err_ttbar[r][c]),
        print '%2.2f%s%2.2f &'%(nevt_other[r][c],pm,err_other[r][c]),
        print '%2.2f%s%2.2f &'%(nevt_data[r][c],pm,err_data[r][c]),
        print '%2.2f%s%2.2f &'%(nevt_total[r][c],pm,err_total[r][c]),
        cf = (nevt_data[r][c] - nevt_other[r][c])/float(nevt_ttbar[r][c])
        cf_err_data = err_data[r][c]/float(nevt_ttbar[r][c])
        cf_err_other_stat = err_other[r][c]/float(nevt_ttbar[r][c])
        cf_err_other_syst = 0.15*nevt_other[r][c]/float(nevt_ttbar[r][c])
        cf_err_ttbar_stat = cf*err_ttbar[r][c]/float(nevt_ttbar[r][c])
        cf_err_total = math.sqrt(cf_err_data**2 + cf_err_other_stat**2 + cf_err_ttbar_stat**2)
        #cf_err_total = math.sqrt(cf_err_data**2 + cf_err_other_stat**2 + cf_err_other_syst**2 + cf_err_ttbar_stat**2)
        print '%2.2f%s%2.2f%s%2.2f'%(cf,pm,0,pm,cf_err_total),
        print '%2.2f'%(cf_err_other_syst),
        #print '%2.2f'%(nevt[r][c]),
        #print '%2.2f'%(nevt[r][c]*0.15),
        #print '%2.2f'%(err[r][c]),
        #print '%2.2f'%(syst[r][c]),
        if c != 'all' :
            print ' & ',
    if r not in ['all','ttl'] :
        print ' \\\\'
    else :
        print ' \\\\ \hline'

print 'done'
