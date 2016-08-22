#!/usr/bin/env python

##
## This macro takes a file produced using pennSoftLepton using PassEvent functions.
##

import ROOT,sys,os
from pennSoftLepton.PyHelpers import PrintCutflow,GetMassesMap,GetHWWColors,GetRootObj
from pennSoftLepton.PyHelpers import GetListOfKeysFromFile,GetListOfProcessesFromFile
import pennSoftLepton.PlotFunctions as plotfunc
import pennSoftLepton.TAxisFunctions as taxisfunc
import pennSoftLepton.PyAnalysisPlotting as anaplot
import importlib

signal_colors = [ROOT.kRed,ROOT.kBlue,ROOT.kGreen,ROOT.kOrange,ROOT.kCyan]*5

#-------------------------------------------------------------------------
def main(options,args) :
    
    print 'Loading c++...'
    anaplot.LoadRootCore()
    ROOT.gROOT.SetBatch(False)

    plotfunc.SetupStyle()
    ROOT.gStyle.SetPaintTextFormat("4.3f ")
    
    #
    # Draw plots
    #
    print 'Drawing the plots'
    cans = []
    for v in options.variables.split(',') :
        if not v : continue
        xlabel = options.histformat[v][3]
        n,low,high = options.histformat[v][:3]

        bkg_hists = []
        sig_hists = []
        data_hist = None

        rebin = []
        if hasattr(options.usermodule,'rebin') and v in options.usermodule.rebin.keys() :
            rebin = options.usermodule.rebin[v]

        if options.file :
            bkg_processes = options.processes.replace('data','').replace(',,',',')
            bkg_hists = anaplot.GetPassEventBkgHistos(v,options.key,bkg_processes,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high)
            anaplot.PrepareBkgHistosForStack(bkg_hists)
            if 'data' in options.processes :
                data_hist = anaplot.GetPassEventBkgHistos(v,options.key,'data',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high)[0]
                #data_hist = anaplot.GetPassEventBkgHistos(v,'FFTool_z_mccl_ltt_allNum','zjetdd',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high)[0]

                
        if options.susy :
            sig_hists = anaplot.GetPassEventSigHistos(v,options.key,options.susy,normalize=options.normalize,rebin=rebin)


        cans.append(anaplot.DrawHistos('%s_%s'%(options.key,v),v,xlabel,bkg_hists,sig_hists,data_hist=data_hist,dostack=options.stack,log=options.log,ratio=options.ratio,fb=options.fb))
        taxisfunc.SetXaxisRanges(cans[-1],low,high)

    #
    # Cutflow
    #
    f = ROOT.TFile(options.file)
    if f.IsZombie() :
        print 'exiting'
        return
    PrintCutflow(options.key
                 ,f.Get('PassEvent_%s/PassEvent_%s_PassW'%(options.key,options.key))
                 ,samp_list=options.processes.split(',')+['all']
                 ,scientific=True)
    f.Close()

    raw_input('Pausing. Press enter to exit.')
            
    if options.save :
        for can in cans :
            can.Print(can.GetName()+'.pdf')
            can.Print(can.GetName()+'.eps')

    print 'done.'
    return

if __name__ == '__main__':
    from optparse import OptionParser
    p = anaplot.PassEventPlottingOptParser()
    options,args = p.parse_args()
    main(options,args)

