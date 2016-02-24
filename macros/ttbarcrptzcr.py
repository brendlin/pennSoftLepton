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
    hists = anaplot.GetPassEventBkgHistos(v,('FFTool_%s_%s_%s%s'%(k,rtmp,c,num_or_den)).replace('__','_'),p,f)
    if not hists :
        print 'NOT Adding (was missing): ',('FFTool_%s_%s_%s%s'%(k,rtmp,c,num_or_den)).replace('__','_')
        return 0
    hist = hists[0]
    if r == 'all' :
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
#channels = ['euu','uee','all']
channels = ['euu','uee','all']
regions = ['ltt','tlt','ttl','all']
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
        #
        # ttcr_ttt (data times FF
        #
        hists_ttbar[r][c] = GetFFHist('all.root','z_ttvalid',r,c,p='ttbar',num_or_den='Den')
        AddIf(hists_ttbar[r][c],'all.root','z_ttvalid',r,c,p='tw',num_or_den='Den')
        AddIf(hists_ttbar[r][c],'all.root','z_ttvalid',r,c,p='qqww',num_or_den='Den')
        nevt_ttbar[r][c] = hists_ttbar[r][c].Integral(0,hists_ttbar[r][c].GetNbinsX()+1)
        err_ttbar[r][c] = math.sqrt(sum(list(hists_ttbar[r][c].GetSumw2())))

        hists_other[r][c] = GetFFHist('all.root','z_ttvalid',r,c,p='wz',num_or_den='Den')
        AddIf(hists_other[r][c],'all.root','z_ttvalid',r,c,p='zz',num_or_den='Den')
        AddIf(hists_other[r][c],'all.root','z_ttvalid',r,c,p='vvv',num_or_den='Den')
        AddIf(hists_other[r][c],'all.root','z_ttvalid',r,c,p='tother',num_or_den='Den')
        AddIf(hists_other[r][c],'all.root','z_ttvalid',r,c,p='ttv',num_or_den='Den')
        AddIf(hists_other[r][c],'all.root','z_ttvalid',r,c,p='zjet',num_or_den='Den')
        AddIf(hists_other[r][c],'all.root','z_ttvalid',r,c,p='zgam',num_or_den='Den')
        AddIf(hists_other[r][c],'all.root','z_ttvalid',r,c,p='singletop',num_or_den='Den')
        AddIf(hists_other[r][c],'all.root','z_ttvalid',r,c,p='tz',num_or_den='Den')

        nevt_other[r][c] = hists_other[r][c].Integral(0,hists_other[r][c].GetNbinsX()+1)
        err_other[r][c] = math.sqrt(sum(list(hists_other[r][c].GetSumw2())))

        hists_total[r][c] = hists_other[r][c].Clone() # all the others
        hists_total[r][c].SetNameTitle('total_%s'%(c),'total_%s'%(c))
        hists_total[r][c].Add(hists_ttbar[r][c]) # add ttbar
        nevt_total[r][c] = hists_total[r][c].Integral(0,hists_total[r][c].GetNbinsX()+1)
        err_total[r][c] = math.sqrt(sum(list(hists_total[r][c].GetSumw2())))

        hists_data[r][c] = GetFFHist('all.root','z_ttvalid',r,c,p='data',num_or_den='Den')
        nevt_data[r][c] = hists_data[r][c].Integral(0,hists_data[r][c].GetNbinsX()+1)
        err_data[r][c] = math.sqrt(sum(list(hists_data[r][c].GetSumw2())))

print 'Channels      & ',
for c in channels :
    if c == 'euu' :
        print '\\ttma+\\ttmb     & ',
        #print '$\mu^\pm e^\mp \mu^\pm$+$\mu^\pm \mu^\pm e^\mp$ & ',
    elif c == 'uee' :
        print '\\ttea+\\tteb     & ',
        #print '$e^\pm e^\pm \mu^\mp$+$e^\pm \mu^\mp e^\pm$     & ',
    else :
        print 'All \\\\ \\hline',
print

for r in regions :
    if r == 'ltt' : print '$\\nltt$, data & ',
    if r == 'tlt' : print '$\\ntlt$, data & ',
    if r == 'ttl' : print '$\\nttl$, data & ',
    if r == 'all' : print 'Total, data   & ',
    for c in channels :
        end = '\\\\' if c == 'all' else '&'
        print ('%2.2f%s%2.2f %s'%(nevt_data[r][c],pm,err_data[r][c],end)).rjust(19),
    if r == 'ltt' : print
    if r == 'tlt' : print
    if r == 'ttl' : print '\\hline'
    if r == 'all' : print '\\hline\\hline'

for r in regions :
    if r == 'ltt' : print '$\\nltt$, \\ttbar MC & ',
    if r == 'tlt' : print '$\\ntlt$, \\ttbar MC & ',
    if r == 'ttl' : print '$\\nttl$, \\ttbar MC & ',
    if r == 'all' : print 'Total, \\ttbar MC   & ',
    for c in channels :
        end = '\\\\' if c == 'all' else '&'
        print ('%2.2f%s%2.2f %s'%(nevt_ttbar[r][c],pm,err_ttbar[r][c],end)).rjust(19),
    if r == 'ltt' : print
    if r == 'tlt' : print
    if r == 'ttl' : print '\\hline'
    if r == 'all' : print '\\hline'

for r in regions :
    if r == 'ltt' : print '$\\nltt$, other MC & ',
    if r == 'tlt' : print '$\\ntlt$, other MC & ',
    if r == 'ttl' : print '$\\nttl$, other MC & ',
    if r == 'all' : print 'Total, other MC   & ',
    for c in channels :
        end = '\\\\' if c == 'all' else '&'
        print ('%2.2f%s%2.2f %s'%(nevt_other[r][c],pm,err_other[r][c],end)).rjust(19),
    print
print 'Data/MC scale factor & ',

cf_euu = (nevt_data['all']['euu'] - nevt_other['all']['euu'])/float(nevt_ttbar['all']['euu'])
cf_euu_err_datastat = err_data['all']['euu']/nevt_ttbar['all']['euu']
cf_euu_err_ttbarstat = cf_euu * err_ttbar['all']['euu']/nevt_ttbar['all']['euu']
cf_euu_toterr = math.sqrt(cf_euu_err_datastat**2 + cf_euu_err_ttbarstat**2)

cf_uee = (nevt_data['all']['uee'] - nevt_other['all']['uee'])/float(nevt_ttbar['all']['uee'])
cf_uee_err_datastat = err_data['all']['uee']/nevt_ttbar['all']['uee']
cf_uee_err_ttbarstat = cf_uee * err_ttbar['all']['uee']/nevt_ttbar['all']['uee']
cf_uee_toterr = math.sqrt(cf_uee_err_datastat**2 + cf_uee_err_ttbarstat**2)

print ('%2.2f%s%2.2f &'%(cf_euu,pm,cf_euu_toterr)).rjust(18),
print ('%2.2f%s%2.2f &'%(cf_uee,pm,cf_uee_toterr)).rjust(18),
print ' - \\\\ \hline'

print 'done'
