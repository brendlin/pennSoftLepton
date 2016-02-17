#!/usr/bin/env python

##
## This macro takes a file produced using pennSoftLepton using PassEvent functions.
##

import ROOT,sys,os,math
from pennSoftLepton.PyHelpers import PrintCutflow,GetMassesMap,GetHWWColors,GetRootObj
from pennSoftLepton.PyHelpers import GetListOfKeysFromFile,GetListOfProcessesFromFile
import pennSoftLepton.PlotFunctions as plotfunc
import pennSoftLepton.TAxisFunctions as taxisfunc
import pennSoftLepton.PyAnalysisPlotting as anaplot
import importlib

signal_colors = [ROOT.kRed,ROOT.kBlue,ROOT.kGreen,ROOT.kOrange,ROOT.kCyan]*5

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
            if r in ['tlt','ttl'] and c in ['euu','uee'] and options.key in ['z_mccl','z_data'] :
                nevt[r][c] = 0
                err[r][c] = 0
                continue
            if c in ['uuu','eee'] and options.key in ['z_ttmccl','z_ttvalid'] :
                nevt[r][c] = 0
                err[r][c] = 0
                continue
            num_or_den = 'Num'
            if options.key in ['z_ttvalid'] :
                num_or_den = 'Den'
            # get hists in nominal case
            if r != 'all' :
                hists[r][c] = anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s%s'%(options.key,r,c,num_or_den)).replace('__','_'),options.bkgs,options.file)[0]
                if options.key == 'z_data' and (options.bkgs == 'zjetdd') :
                    hists[r][c].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s%s'%(options.key,r,c,num_or_den)).replace('__','_'),'ttbar',options.file)[0],-1)
                    hists[r][c].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s%s'%(options.key,r,c,num_or_den)).replace('__','_'),'wz',options.file)[0],-1)
            else :
                # get hists for "all" case
                hists[r][c] = anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s%s'%(options.key,'ltt',c,num_or_den)).replace('__','_'),options.bkgs,options.file)[0]
                if options.key == 'z_data' and (options.bkgs == 'zjetdd') :
                    hists[r][c].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s%s'%(options.key,'ltt',c,num_or_den)).replace('__','_'),'ttbar',options.file)[0],-1)
                    hists[r][c].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s%s'%(options.key,'ltt',c,num_or_den)).replace('__','_'),'wz',options.file)[0],-1)
                for r2 in ['tlt','ttl'] :
                    if r2 in ['tlt','ttl'] and c in ['euu','uee'] and options.key in ['z_mccl','z_data'] :
                        continue
                    hists[r][c].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s%s'%(options.key,r2,c,num_or_den)).replace('__','_'),options.bkgs,options.file)[0])
                    if options.key == 'z_data' and (options.bkgs == 'zjetdd') :
                        hists[r][c].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s%s'%(options.key,r2,c,num_or_den)).replace('__','_'),'ttbar',options.file)[0],-1)
                        hists[r][c].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s%s'%(options.key,r2,c,num_or_den)).replace('__','_'),'wz',options.file)[0],-1)

            nevt[r][c] = hists[r][c].Integral(0,hists[r][c].GetNbinsX()+1)
            err[r][c] = math.sqrt(sum(list(hists[r][c].GetSumw2())))
            #
            # systematics
            #
            syst[r][c] = 0
            syst_names = ['ebin1','ebin2','ebin3','ebin4','mubin1','mubin2','mubin3']
            tmp_systs = []
            for i in syst_names :
                if r != 'all' :
                    tmp_systs.append(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s_%s%s'%(options.key,r,i,c,num_or_den)).replace('__','_'),options.bkgs,options.file)[0])
                    if options.key == 'z_data' and (options.bkgs == 'zjetdd') :
                        tmp_systs[-1].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s_%s%s'%(options.key,r,i,c,num_or_den)).replace('__','_'),'ttbar',options.file)[0],-1)
                        tmp_systs[-1].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s_%s%s'%(options.key,r,i,c,num_or_den)).replace('__','_'),'wz',options.file)[0],-1)
                else :
                    tmp_systs.append(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s_%s%s'%(options.key,'ltt',i,c,num_or_den)).replace('__','_'),options.bkgs,options.file)[0])
                    if options.key == 'z_data' and (options.bkgs == 'zjetdd') :
                        tmp_systs[-1].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s_%s%s'%(options.key,'ltt',i,c,num_or_den)).replace('__','_'),'ttbar',options.file)[0],-1)
                        tmp_systs[-1].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s_%s%s'%(options.key,'ltt',i,c,num_or_den)).replace('__','_'),'wz',options.file)[0],-1)
                    for r2 in ['tlt','ttl'] :
                        if r2 in ['tlt','ttl'] and c in ['euu','uee'] and options.key in ['z_mccl','z_data'] :
                            continue
                        tmp_systs[-1].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s_%s%s'%(options.key,r2,i,c,num_or_den)).replace('__','_'),options.bkgs,options.file)[0])
                        if options.key == 'z_data' and (options.bkgs == 'zjetdd') :
                            tmp_systs[-1].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s_%s%s'%(options.key,r2,i,c,num_or_den)).replace('__','_'),'ttbar',options.file)[0],-1)
                            tmp_systs[-1].Add(anaplot.GetPassEventBkgHistos('Mll',('FFTool_%s_%s_%s_%s%s'%(options.key,r2,i,c,num_or_den)).replace('__','_'),'wz',options.file)[0],-1)
                    
            nominal = hists[r][c].Integral(0,hists[r][c].GetNbinsX()+1)
            for i in tmp_systs :
                difference = i.Integral(0,i.GetNbinsX()+1)-nominal
                syst[r][c] += difference**2
            syst[r][c] = math.sqrt(syst[r][c])

    total = 0
    err2 = 0
    print 'Channels       & ',
    for c in channels :
        print ('\\'+c).ljust(11),
        print ' & ',
    print ('All').ljust(13)+'\\\\ \hline',
    print
    for r in regions :
        print '$\\n%s\cdot F$ & '%(r),
        for c in channels :
            if r in ['tlt','ttl'] and c in ['euu','uee'] and options.key in ['z_mccl','z_data'] :
                print '   -                   ',
                print ' & ',
                continue
            if c != 'all' and r != 'all' : total += nevt[r][c]
            if c != 'all' and r != 'all' : err2  += err[r][c]**2
            print '%2.2f%s%2.2f%s%2.2f'%(nevt[r][c],pm,err[r][c],pm,syst[r][c]),
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

    plotfunc.SetupStyle()
    
#     options.keys = ['SRmu',
#                     'SRe',
#                     'CRZ_ltt_e',
#                     ]
    the_options = ['SR',
                   'SRMu',
                   'SREle',
                   'SReee',
                   'SReuu',
                   'SRuee',
                   'SRuuu',
                   'tablez',
                   'tablett',
                   'CRZ_e',
                   'CRZ_u',
                   'CRZ_ltt_e',
                   'CRZ_ltt_u',
                   'CRZ_tlt_e',
                   'CRZ_tlt_u',
                   'CRZ_ttl_e',
                   'CRZ_ttl_u',
                   'CRTT_all_all'
                   'CRTT_ttt_Ele',
                   'CRTT_ttt_Mu',
                   'CRTT_ltt_eee',
                   'CRTT_ltt_euu',
                   'CRTT_tlt_uee',
                   'CRTT_tlt_uuu',
#                    'CRTT_ttl_e',
#                    'CRTT_ttl_u',
                   'CRTT_u',
                   'CRTT_e',
                   'TTclose_e',
                   'TTclose_u',
                   'Zclose_e',
                   ]

#     if options.key not in the_options : 
#         print 'Try one of these:'
#         print the_options
#         return
    options.keys = [options.key]

    def GetAllCRs(v,FFTool_all_XxDen,proc) :
        hist   = anaplot.GetPassEventBkgHistos(v,FFTool_all_XxDen.replace('all','ltt'),proc,options.file,normalize=options.normalize,rebin=rebin)[0]
        hist.Add(anaplot.GetPassEventBkgHistos(v,FFTool_all_XxDen.replace('all','tlt'),proc,options.file,normalize=options.normalize,rebin=rebin)[0])
        hist.Add(anaplot.GetPassEventBkgHistos(v,FFTool_all_XxDen.replace('all','ttl'),proc,options.file,normalize=options.normalize,rebin=rebin)[0])
        print '%s full: %2.2f%s%2.2f'%(proc,hist.Integral(0,hist.GetNbinsX()+1),pm,math.sqrt(sum(list(hist.GetSumw2()))))
        return hist

    print options.keys

    #
    # Tables. E.g. MakeDetailedTable('')
    #
    MakeDetailedTable(options)

    #
    # Draw plots
    #
    print 'Drawing the plots'
    cans = []
    for v in options.variables.split(',') :
        if not v : continue

        lep_pt_dijet_num = None
        lep_pt_dijet_den = None

        bkg_hists = []
        sig_hists = []
        data_hist = None

        rebin = []
        if hasattr(options.usermodule,'rebin') and v in options.usermodule.rebin.keys() :
            rebin = options.usermodule.rebin[v]

        bkg_processes = options.processes.replace('data','').replace(',,',',')
        bkg_hists = []


#         # SR
#         elif 'SR' in options.key :
        if 'ttt' in options.key :
            channel = k.replace('SR','')
#                 bkg_hists += [GetAllCRs(v,'FFTool_ttmccl_all_%sNum'%(channel),'ttfix')]
#                 #bkg_hists[-1].SetTitle('tt_ZCR')
#                 bkg_hists[-1].SetTitle('skipme')
#                 bkg_hists[-1].Scale(-1)
#                 bkg_hists += [GetAllCRs(v,'FFTool_zmccl_all_%sNum'%(channel),'wz')]
#                 #bkg_hists[-1].SetTitle('wz_ZCR')
#                 bkg_hists[-1].SetTitle('skipme')
#                 bkg_hists[-1].Scale(-1)
#                 bkg_hists += [GetAllCRs(v,'FFTool_zmccl_all_%sNum'%(channel),'zjetdd')]
            #bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttt_%sNum'%(channel),'zjet',options.file,normalize=options.normalize,rebin=rebin)
            bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttt_%sNum'%(channel),'zz',options.file,normalize=options.normalize,rebin=rebin)
            bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttt_%sNum'%(channel),'ttbar',options.file,normalize=options.normalize,rebin=rebin)
            bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttt_%sNum'%(channel),'wz',options.file,normalize=options.normalize,rebin=rebin)
            #data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_ttt_%sNum'%(channel),'data',options.file,normalize=options.normalize,rebin=rebin)[0]

#         #
#         # Ttbar closure plots
#         #

#         elif k == 'Zclose_e' :
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_zmccl_ltt_EleNum','zjetdd',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_zmccl_ttl_EleNum','zjetdd',options.file,normalize=options.normalize,rebin=rebin)[0])
#             bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_zmccl_tlt_EleNum','zjetdd',options.file,normalize=options.normalize,rebin=rebin)[0])

#         elif k == 'TTclose_e' :
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_zmccl_ltt_EleNum','ttdblct',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_zmccl_ttl_EleNum','ttdblct',options.file,normalize=options.normalize,rebin=rebin)[0])
#             bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_zmccl_tlt_EleNum','ttdblct',options.file,normalize=options.normalize,rebin=rebin)[0])
#             print 'ttdblct full: %2.2f%s%2.2f'%(bkg_hists[-1].Integral(0,bkg_hists[-1].GetNbinsX()+1),pm,math.sqrt(sum(list(bkg_hists[-1].GetSumw2()))))
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttmccl_ltt_EleNum','ttfix',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_ttmccl_ttl_EleNum','ttfix',options.file,normalize=options.normalize,rebin=rebin)[0])
#             bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_ttmccl_tlt_EleNum','ttfix',options.file,normalize=options.normalize,rebin=rebin)[0])
#             print 'ttfix full: %2.2f%s%2.2f'%(bkg_hists[-1].Integral(0,bkg_hists[-1].GetNbinsX()+1),pm,math.sqrt(sum(list(bkg_hists[-1].GetSumw2()))))

#         elif k == 'TTclose_u' :
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_zmccl_ltt_MuNum','ttdblct',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_zmccl_ttl_MuNum','ttdblct',options.file,normalize=options.normalize,rebin=rebin)[0])
#             bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_zmccl_tlt_MuNum','ttdblct',options.file,normalize=options.normalize,rebin=rebin)[0])
#             print 'ttdblct full: %2.2f%s%2.2f'%(bkg_hists[-1].Integral(0,bkg_hists[-1].GetNbinsX()+1),pm,math.sqrt(sum(list(bkg_hists[-1].GetSumw2()))))
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttmccl_ltt_MuNum','ttfix',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_ttmccl_ttl_MuNum','ttfix',options.file,normalize=options.normalize,rebin=rebin)[0])
#             bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_ttmccl_tlt_MuNum','ttfix',options.file,normalize=options.normalize,rebin=rebin)[0])
#             print 'ttfix full: %2.2f%s%2.2f'%(bkg_hists[-1].Integral(0,bkg_hists[-1].GetNbinsX()+1),pm,math.sqrt(sum(list(bkg_hists[-1].GetSumw2()))))

#         #
#         # zjet CR
#         #
#         elif k == 'CRZ_e' :
#             bkg_hists += [GetAllCRs(v,'FFTool_all_EleDen','wz')]
#             bkg_hists += [GetAllCRs(v,'FFTool_all_EleDen','zz')]
#             bkg_hists += [GetAllCRs(v,'FFTool_all_EleDen','ttbar')]
#             bkg_hists += [GetAllCRs(v,'FFTool_all_EleDen','zjet')]
#             bkg_hists += [GetAllCRs(v,'FFTool_all_EleDen','zgam')]
#             data_hist  =  GetAllCRs(v,'FFTool_all_EleDen','data')

#         elif k == 'CRZ_u' :
#             bkg_hists += [GetAllCRs(v,'FFTool_all_MuDen','wz')]
#             bkg_hists += [GetAllCRs(v,'FFTool_all_MuDen','zz')]
#             bkg_hists += [GetAllCRs(v,'FFTool_all_MuDen','ttbar')]
#             bkg_hists += [GetAllCRs(v,'FFTool_all_MuDen','zjet')]
#             bkg_hists += [GetAllCRs(v,'FFTool_all_MuDen','zgam')]
#             data_hist  =  GetAllCRs(v,'FFTool_all_MuDen','data')

#         elif k == 'CRZ_ltt_e' :
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ltt_EleDen','wz',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ltt_EleDen','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ltt_EleDen','zjet',options.file,normalize=options.normalize,rebin=rebin)
#             data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_ltt_EleDen','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#         elif k == 'CRZ_ltt_u' :
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ltt_MuDen','wz',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ltt_MuDen','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ltt_MuDen','zjet',options.file,normalize=options.normalize,rebin=rebin)
#             data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_ltt_MuDen','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#         elif k == 'CRZ_tlt_e' :
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_tlt_EleDen','wz',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_tlt_EleDen','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_tlt_EleDen','zjet',options.file,normalize=options.normalize,rebin=rebin)
#             data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_tlt_EleDen','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#         elif k == 'CRZ_tlt_u' :
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_tlt_MuDen','wz',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_tlt_MuDen','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_tlt_MuDen','zjet',options.file,normalize=options.normalize,rebin=rebin)
#             data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_tlt_MuDen','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#         elif k == 'CRZ_ttl_e' :
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttl_EleDen','wz',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttl_EleDen','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttl_EleDen','zjet',options.file,normalize=options.normalize,rebin=rebin)
#             data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_ttl_EleDen','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#         elif k == 'CRZ_ttl_u' :
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttl_MuDen','wz',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttl_MuDen','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#             bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttl_MuDen','zjet',options.file,normalize=options.normalize,rebin=rebin)
#             data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_ttl_MuDen','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#         #
#         # ttbar CR
#         #
#         elif 'CRTT' in k :
#             channel = k.split('_')[2]
#             region = k.split('_')[1]
#             if region == 'all' :
#                 bkg_hists += [GetAllCRs(v,'FFTool_ttcr_all_%sNum'%(channel),'wz')]
#                 bkg_hists += [GetAllCRs(v,'FFTool_ttcr_all_%sNum'%(channel),'zz')]
#                 bkg_hists += [GetAllCRs(v,'FFTool_ttcr_all_%sNum'%(channel),'zjet')]
#                 bkg_hists += [GetAllCRs(v,'FFTool_ttcr_all_%sNum'%(channel),'zgam')]
#                 bkg_hists += [GetAllCRs(v,'FFTool_ttcr_all_%sNum'%(channel),'ttbar')]
#                 data_hist   = GetAllCRs(v,'FFTool_ttcr_all_%sNum'%(channel),'data')
#             else :
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_%s_%sNum'%(region,channel),'wz'   ,options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_%s_%sNum'%(region,channel),'zz'   ,options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_%s_%sNum'%(region,channel),'zjet' ,options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_%s_%sNum'%(region,channel),'zgam' ,options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_%s_%sNum'%(region,channel),'ttbar',options.file,normalize=options.normalize,rebin=rebin)
#                 data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_%s_%sNum'%(region,channel),'data',options.file,normalize=options.normalize,rebin=rebin)[0]

#             if k == 'CRTT_ttt_e' :
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttt_EleNum','wz',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttt_EleNum','zjet',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttt_EleNum','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#                 data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttt_EleNum','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#             if k == 'CRTT_ltt_e' :
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ltt_EleDen','wz',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ltt_EleDen','zjet',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ltt_EleDen','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#                 data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ltt_EleDen','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#             if k == 'CRTT_ltt_u' :
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ltt_MuDen','wz',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ltt_MuDen','zjet',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ltt_MuDen','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#                 data_hist = anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ltt_MuDen','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#             if k == 'CRTT_tlt_e' :
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_tlt_EleDen','wz',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_tlt_EleDen','zjet',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_tlt_EleDen','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#                 data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_tlt_EleDen','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#             if k == 'CRTT_u' :
#                 bkg_hists += [GetAllCRs(v,'FFTool_ttcr_all_MuDen','wz')]
#                 bkg_hists += [GetAllCRs(v,'FFTool_ttcr_all_MuDen','zz')]
#                 bkg_hists += [GetAllCRs(v,'FFTool_ttcr_all_MuDen','ttbar')]
#                 bkg_hists += [GetAllCRs(v,'FFTool_ttcr_all_MuDen','zjet')]
#                 bkg_hists += [GetAllCRs(v,'FFTool_ttcr_all_MuDen','zgam')]
#                 data_hist   = GetAllCRs(v,'FFTool_ttcr_all_MuDen','data')

#             if k == 'CRTT_ttt_e' :
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttt_MuNum','wz',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttt_MuNum','zjet',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttt_MuNum','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#                 data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttt_MuNum','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#             if k == 'CRTT_tlt_u' :
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_tlt_MuDen','wz',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_tlt_MuDen','zjet',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_tlt_MuDen','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#                 data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_tlt_MuDen','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#             if k == 'CRTT_ttl_e' :
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttl_EleDen','wz',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttl_EleDen','zjet',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttl_EleDen','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#                 data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttl_EleDen','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#             if k == 'CRTT_ttl_u' :
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttl_MuDen','wz',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttl_MuDen','zjet',options.file,normalize=options.normalize,rebin=rebin)
#                 bkg_hists += anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttl_MuDen','ttbar',options.file,normalize=options.normalize,rebin=rebin)
#                 data_hist  = anaplot.GetPassEventBkgHistos(v,'FFTool_ttcr_ttl_MuDen','data',options.file,normalize=options.normalize,rebin=rebin)[0]

#             else :
#                 print 'error: bad key.'
#                 import sys
#                 sys.exit()

#             anaplot.PrepareBkgHistosForStack(bkg_hists)
#             xlabel = ROOT.PSL.GetXaxisLabel(v)
#             cans.append(anaplot.DrawHistos(k+v,v,xlabel,bkg_hists,sig_hists,data_hist=data_hist,dostack=options.stack,log=options.log,ratio=options.ratio,fb=options.fb))
#             cans[-1].SetName('%s_%s'%(cans[-1].GetName(),k))

    raw_input('Pausing. Press enter to exit.')

    if options.save :
        for can in cans :
            can.Print(can.GetName()+'.pdf')
            can.Print(can.GetName()+'.eps')
            
    print 'done.'
    return

if __name__ == '__main__':
    p = anaplot.PassEventPlottingOptParser()
    p.p.remove_option('--susy')
    p.p.add_option('--region' ,'-r',type='string',default='all',dest='region',help='Region (ttt,ltt,tlt,ttl,all (all means ltt+tlt+ttl)')
    p.p.add_option('--channel','-c',type='string',default='all',dest='channel',help='Channel (eee,euu,uee,uuu,all)')
    p.p.add_option('--bkgs'        ,type='string',default='zjet',dest='bkgs',help='Backgrounds (not sure yet how this works')
    options,args = p.parse_args()
    # use key for SRmu, SRe, CRZ_ltt_e
    main(options,args)

