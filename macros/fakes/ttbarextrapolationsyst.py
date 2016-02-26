#!/usr/bin/env python

import ROOT
import math
import sys
import pennSoftLepton.PyHelpers as pyhelpers
import pennSoftLepton.PyAnalysisPlotting as anaplot
import os

anaplot.LoadRootCore()

def GetFFHistNew(f,k,r,c,p='ttbar',v='MTW',num_or_den='Den') : # file key region channel process variable
    other = '%s_%s'%(p,v)
    rtmp = r
    if r == 'all' :
        rtmp = 'ltt'
    keyregionchannel = ('FFTool_%s_%s_%s%s'%(k,rtmp,c,num_or_den)).replace('__','_')
    print 'Adding',keyregionchannel
    hists = anaplot.GetPassEventBkgHistos(v,keyregionchannel,p,f)
    if not hists :
        print 'NOT Adding (was missing): ',keyregionchannel
        return 0
    hist = hists[0]
    if r == 'all' :
        hist.Add(GetFFHistNew(f,k,'tlt',c,p,v,num_or_den))
        hist.Add(GetFFHistNew(f,k,'ttl',c,p,v,num_or_den))
    return hist
    #return pyhelpers.GetRootObj(f,'%s/%s_%s'%(keyregionchannel,keyregionchannel,other))

def GetListOfSystematics(channel,region) :

    hists = []
    histd = []
    thelist = []
    nominal = ''
    for i in os.listdir('.') :
        if 'DS_Store' in i : continue
        if '.root' in i : continue
        if '.py' in i : continue
        if '.sh' in i : continue
        if 'unused' in i : continue
        if 'NOMINAL' not in i : continue
        thelist.append(i)

    for i in os.listdir('.') :
        if 'DS_Store' in i : continue
        if '.txt' in i : continue
        if '.root' in i : continue
        if '.py' in i : continue
        if '.sh' in i : continue
        if 'unused' in i : continue
        if 'NOMINAL' in i : continue
        thelist.append(i)

    print thelist

    for i in thelist :
        fname = '%s/hist-ttbar.root'%(i)
        f = ROOT.TFile('%s/hist-ttbar.root'%(i))
        print f.GetName()

        # The numerator

        key = 'z_data'
        #key = 'z_est_tt'

        #print key,channel
        hists.append(GetFFHistNew(fname,key,region,channel))
        hists[-1].SetDirectory(0)

        # the denominator of the thing
        ked = 'z_ttvalid'
        #print ked
        channed = {'eee':'uee','uee':'euu','euu':'uee','uuu':'euu'}.get(channel)
        #print ked,channed
        histd.append(GetFFHistNew(fname,ked,region,channed))
        histd[-1].SetDirectory(0)

        f.Close()

    pm = u"\u00B1"
    error2 = 0
    n = 0
    nominal = hists[0].Integral(0,hists[0].GetNbinsX()+1)
    nominad = histd[0].Integral(0,hists[0].GetNbinsX()+1)

    up_var_2 = 0
    dn_var_2 = 0

    column_1 = []
    column_2 = []
    
    for i in range(len(thelist)) :
        thelist[i] = thelist[i].replace('ttbar_FakesSelection_mc15b_ttsys_','')
    for i in range(len(hists)) :
        #print i.GetName()
        #print hists[i].GetName()
        #print '%2.2f%s%2.2f'%(i.Integral(0,i.GetNbinsX()+1),pm,math.sqrt(sum(list(i.GetSumw2()))))
        if 'NOMINAL' in thelist[i] : continue
        result = hists[i].Integral(0,hists[i].GetNbinsX()+1)
        difference = result - nominal
        resuld = histd[i].Integral(0,hists[i].GetNbinsX()+1)
    #     print '%s Result: %2.5f, difference from nominal: %2.5f'%(thelist[i].ljust(65),result,difference)
    #     print '%s Resuld: %2.5f, difference from nominal: %2.5f'%(thelist[i].ljust(65),result,difference)
        the_result = (result*nominad / float(resuld*nominal))
        #print '%s &  %2.4f'%(thelist[i].ljust(65),the_result)
        the_result_percent = (1-the_result)*100.
        #print '%s &  %2.2f'%(thelist[i].ljust(65),the_result_percent)
    #     error2 += difference**2
        if '__1down' in thelist[i] :
            dn_var_2 += (1-the_result)**2
        if '__1up' in thelist[i] :
            up_var_2 += (1-the_result)**2

        column_1.append(thelist[i])
        column_2.append(the_result_percent)

    #print '%s &  %2.2f'%('Total down (\%)'.ljust(65),100.*math.sqrt(dn_var_2))
    #print '%s &  %2.2f'%('Total up (\%)'.ljust(65)  ,100.*math.sqrt(up_var_2))

    column_1.append('Total down')
    column_1.append('Total up')
    
    column_2.append(100.*math.sqrt(dn_var_2))
    column_2.append(100.*math.sqrt(up_var_2))

    # print 'Result: %2.2f %s %2.2f'%(nominal,pm,math.sqrt(error2))
    
    return column_1,column_2

names,eee = GetListOfSystematics('eee','all')
names,euu = GetListOfSystematics('euu','all')
names,uee = GetListOfSystematics('uee','all')
names,uuu = GetListOfSystematics('uuu','all')

for i in range(len(names)) :
    tmp = names[i].replace('_','\_')
    hline = ''
    if (tmp == 'MUONS\_SCALE\_\_1up') :
        hline = '\\hline'
    if (tmp == 'Total up') :
        hline = '\\hline'
    print '%s &  %s  &  %s  &  %s  &  %s  \\\\%s'%(tmp.ljust(45),
                                                   ('%2.2f'%eee[i]).replace('-','$-$').rjust(8),
                                                   ('%2.2f'%euu[i]).replace('-','$-$').rjust(8),
                                                   ('%2.2f'%uee[i]).replace('-','$-$').rjust(8),
                                                   ('%2.2f'%uuu[i]).replace('-','$-$').rjust(8),
                                                   hline)
    

