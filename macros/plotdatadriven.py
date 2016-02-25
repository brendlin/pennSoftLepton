#!/usr/bin/env python

##
## This macro takes a file produced using pennSoftLepton using PassEvent functions.
##

import ROOT,sys,os,math
from pennSoftLepton.PyHelpers import PrintCutflow,GetMassesMap,GetHWWColors,GetRootObj
from pennSoftLepton.PyHelpers import GetListOfKeysFromFile,GetListOfProcessesFromFile
from pennSoftLepton.PyRandomStuff import GetFFHist,AddFFHistIf
import pennSoftLepton.PlotFunctions as plotfunc
import pennSoftLepton.TAxisFunctions as taxisfunc
import pennSoftLepton.PyAnalysisPlotting as anaplot
import importlib

signal_colors = [ROOT.kRed,ROOT.kBlue,ROOT.kGreen,ROOT.kOrange,ROOT.kCyan]*5
nominally_remove_mispairregions = True

#-------------------------------------------------------------------------
def MakeDetailedTable(options) :
    #pm = u"\u00B1"
    pm = "$\pm$"
    hists = dict()
    syst = dict()
    nevt = dict()
    err = dict()
    if options.region == 'all' :
        regions = ['ltt','tlt','ttl','all']
    else :
        regions = [options.region]
    if options.channel == 'all' :
        channels = ['eee','euu','uee','uuu','all']
    else : 
        channels = [options.channel]
    for r in regions :
        hists[r] = dict()
        nevt[r] = dict()
        err[r] = dict()
        syst[r] = dict()
        for c in channels :
            print r,c,options.key
            if r in ['tlt','ttl'] and c in ['euu','uee'] and options.key in ['z_mccl','z_data'] and nominally_remove_mispairregions :
                nevt[r][c] = 0
                err[r][c] = 0
                continue
            if c in ['uuu','eee'] and options.key in ['z_ttmccl','z_ttvalid'] :
                nevt[r][c] = 0
                err[r][c] = 0
                continue
            num_or_den = 'Num'
            if options.key in ['z_ttvalid','z_valid'] :
                num_or_den = 'Den'
            # get hists in nominal case
            add_desert = (options.key == 'z_est_tt')
            hists[r][c] = GetFFHist(options.file,options.key,r,c,options.bkgs,num_or_den=num_or_den,add_desert=add_desert)
            if options.key == 'z_data' and (options.bkgs == 'zjetdd') and True :
                hists[r][c].Add(GetFFHist(options.file,options.key,r,c,'ttbar',num_or_den=num_or_den),-1)
                hists[r][c].Add(GetFFHist(options.file,options.key,r,c,'tw'   ,num_or_den=num_or_den),-1)
                AddFFHistIf(hists[r][c],options.file,options.key,r,c,'qqww' ,num_or_den=num_or_den,scale=-1)
                hists[r][c].Add(GetFFHist(options.file,options.key,r,c,'wz'   ,num_or_den=num_or_den),-1)
                hists[r][c].Add(GetFFHist(options.file,options.key,r,c,'zz'   ,num_or_den=num_or_den),-1)
            if options.key == 'z_data' and (options.bkgs == 'wz') :
                hists[r][c].Add(GetFFHist(options.file,options.key,r,c,'zz'   ,num_or_den=num_or_den),1)
            if options.key in ['z_data','z_est_tt'] and (options.bkgs == 'ttbar') :
                hists[r][c].Add(GetFFHist(options.file,options.key,r,c,'tw'   ,num_or_den=num_or_den,add_desert=add_desert),1)
                AddFFHistIf(hists[r][c],options.file,options.key,r,c,'qqww' ,num_or_den=num_or_den,scale=1)

            nevt[r][c] = hists[r][c].Integral(0,hists[r][c].GetNbinsX()+1)
            err[r][c] = math.sqrt(sum(list(hists[r][c].GetSumw2())))
            #
            # systematics
            #
            syst[r][c] = 0
            syst_names = [
                'ebin1',
                'ebin2',
                'ebin3',
                'ebin4',
                'mubin1',
                'mubin2',
                'mubin3',
#                 'wzsyst',
#                 'compe',
#                 'compmu',
                ]
            if not options.syst :
                continue
            tmp_systs = []
            for i in syst_names :
                tmp_systs.append(GetFFHist(options.file,options.key,r,c,options.bkgs,num_or_den=num_or_den,syst=i,add_desert=add_desert))
                if options.key == 'z_data' and (options.bkgs == 'zjetdd') and True :
                    tmp_systs[-1].Add(GetFFHist(options.file,options.key,r,c,'ttbar',num_or_den=num_or_den,syst=i),-1)
                    tmp_systs[-1].Add(GetFFHist(options.file,options.key,r,c,'tw'   ,num_or_den=num_or_den,syst=i),-1)
                    AddFFHistIf(tmp_systs[-1],options.file,options.key,r,c,'qqww' ,num_or_den=num_or_den,syst=i,scale=-1)
                    tmp_systs[-1].Add(GetFFHist(options.file,options.key,r,c,'wz'   ,num_or_den=num_or_den,syst=i),-1)
                    tmp_systs[-1].Add(GetFFHist(options.file,options.key,r,c,'zz'   ,num_or_den=num_or_den,syst=i),-1)
                if options.key == 'z_data' and (options.bkgs == 'wz') :
                    tmp_systs[-1].Add(GetFFHist(options.file,options.key,r,c,'zz'   ,num_or_den=num_or_den,syst=i),1)
                if options.key in ['z_data','z_est_tt'] and (options.bkgs == 'ttbar') :
                    tmp_systs[-1].Add(GetFFHist(options.file,options.key,r,c,'tw'   ,num_or_den=num_or_den,syst=i,add_desert=add_desert),1)
                    AddFFHistIf(tmp_systs[-1],options.file,options.key,r,c,'qqww' ,num_or_den=num_or_den,syst=i,scale=1)
                    
            nominal = hists[r][c].Integral(0,hists[r][c].GetNbinsX()+1)
            for i in tmp_systs :
                difference = i.Integral(0,i.GetNbinsX()+1)-nominal
                syst[r][c] += difference**2
            syst[r][c] = math.sqrt(syst[r][c])

    total = 0
    err2 = 0
    print 'Channels       & ',
    for c in channels :
        if c != 'all' :
            print ('\\'+c).ljust(11),
            print ' & ',
        else :
            print (c).ljust(11),
            print '\\\\ \hline',
    print
    for r in regions :
        if r != 'all' :
            print '$\\n%s\cdot F$ & '%(r),
        else :
            print 'Total          & ',
        for c in channels :
            if c in ['eee','uuu'] and options.key in ['z_ttvalid'] :
                print '   -                   ',
                print ' & ',
                continue                
            if r in ['tlt','ttl'] and c in ['euu','uee'] and options.key in ['z_mccl','z_data'] and nominally_remove_mispairregions :
                print '   -                   ',
                print ' & ',
                continue
            if c != 'all' and r != 'all' : total += nevt[r][c]
            if c != 'all' and r != 'all' : err2  += err[r][c]**2
            print '%2.2f%s%2.2f%s%2.2f'%(nevt[r][c],pm,err[r][c],pm,syst[r][c]),
#             print '%2.2f'%(nevt[r][c]),
#             print '%2.2f'%(nevt[r][c]*0.15),
#             print '%2.2f'%(err[r][c]),
#             print '%2.2f'%(syst[r][c]),
            
            # ttbar SF stat systs for ZCR - muons
            # muons. The error should be added linearly. I think you have to literally add.
#             if c not in ['eee','euu'] :
#                 print '%2.2f'%(0.08*nevt[r][c]/1.03),
#             else :
#                 print '   - ',
            # electrons. The error should be added linearly. I think you have to literally add.
#             if c not in ['uee','uuu'] :
#                 print '%2.2f'%(0.14*nevt[r][c]/1.05),
#             else :
#                 print '   - ',

            if c != 'all' :
                print ' & ',
            #if c != 'uuu' : print ' & ',
#         print '%2.2f%s%2.2f%s%s'%(sum(nevt[r][c] for c in ['eee','euu','uee','uuu']),pm,math.sqrt(sum(err[r][c]**2 for c in ['eee','euu','uee','uuu'])),pm,'X.XX'),
        if r not in ['all','ttl'] :
            print ' \\\\'
        else :
            print ' \\\\ \hline'
#     print 'Total          & ',
#     for c in channels :
#         print '%2.2f%s%2.2f%s%s'%(sum(nevt[r][c] for r in ['ltt','tlt','ttl']),pm,math.sqrt(sum(err[r][c]**2 for r in ['ltt','tlt','ttl'])),pm,'X.XX'),
#         print ' & ',
#         #if c != 'uuu' : print ' & ',
#     print '%2.2f%s%2.2f%s%s'%(total,pm,math.sqrt(err2),pm,'X.XX'),
#     print ' \\\\ \hline'

#-------------------------------------------------------------------------
def main(options,args) :

    pm = u"\u00B1"
    
    print 'Loading c++...'
    anaplot.LoadRootCore()
    ROOT.gROOT.SetBatch(False)

    #
    # Tables. E.g. MakeDetailedTable('')
    #
    MakeDetailedTable(options)
            
    print 'done.'
    return

if __name__ == '__main__':
    p = anaplot.PassEventPlottingOptParser()
    p.p.remove_option('--susy')
    p.p.remove_option('-p')
    p.p.add_option('--region' ,'-r',type='string',default='all',dest='region',help='Region (ttt,ltt,tlt,ttl,all (all means ltt+tlt+ttl)')
    p.p.add_option('--channel','-c',type='string',default='all',dest='channel',help='Channel (eee,euu,uee,uuu,all)')
    p.p.add_option('--bkgs'        ,type='string',default='zjetdd',dest='bkgs',help='Backgrounds (not sure yet how this works')
    p.p.remove_option('--key')
    p.p.add_option('-k','--key' ,type='string',default='z_data',dest='key'  ,help='name (key) - no PassEvent, no FakeFactor')
    p.p.add_option('--systematics'  ,action='store_true',default=False  ,dest='syst',help='Run all systematic variations' )
    options,args = p.parse_args()
    # use key for SRmu, SRe, CRZ_ltt_e
    main(options,args)

