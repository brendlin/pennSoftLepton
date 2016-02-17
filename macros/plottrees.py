#!/bin/usr/env python

##
## This macro takes a file produced using pennSoftLepton using PassEvent functions.
##

import ROOT,sys,os
from pennSoftLepton.PyHelpers import PrintCutflow,GetMassesMap,GetHWWColors
from pennSoftLepton.PyHelpers import GetListOfKeysFromFile,GetListOfProcessesFromFile,GetListOfVariablesFromFile
import pennSoftLepton.TAxisFunctions as taxisfunc
import pennSoftLepton.PyAnalysisPlotting as anaplot
import pennSoftLepton.PlotFunctions as plotfunc
import pennSoftLepton.PyHelpers as helpers

signal_colors = [ROOT.kRed,ROOT.kBlue,ROOT.kGreen,ROOT.kOrange,ROOT.kCyan]*5

#-------------------------------------------------------------------------
def main(options,args) :

    plotfunc.SetupStyle()

    for i in options.histformat.keys() :
        print '\'%s\':'%(i),options.histformat[i]

    anaplot.LoadRootCore()
    
    files_b,trees_b,keys_b,cf_b = anaplot.GetTreesFromFiles(options.bkgs)
    files_s,trees_s,keys_s,cf_s = anaplot.GetTreesFromFiles(options.susy)
    file_d ,tree_d ,key_d ,cf_d = anaplot.GetTreesFromFiles(options.data)

    options.fb,lumi_scale_factor = helpers.GetTTreeLumiScaleFactor(files_b+files_s,options.fb)

    dweight = ''
    weight = 'TotalWeightNoSF'
    if ''.join(options.cuts) :
        weight = weight+'*(%s)'%(' && '.join(options.cuts))
        dweight = '('+' && '.join(options.cuts)+')'
    if hasattr(options.usermodule,'weight') :
        weight += '*(%s)'%(options.usermodule.weight)
    if hasattr(options.usermodule,'dataweight') :
        dweight += '*(%s)'%(options.usermodule.dataweight)        

    cans = []

    # get the histograms from the files
    for v in options.variables.split(',') :
        n,low,high = options.histformat[v][:3]
        xlabel = options.histformat[v][3]

        bkg_hists = []
        sig_hists = []
        data_hist = None

        rebin = []
        if hasattr(options.usermodule,'rebin') and v in options.usermodule.rebin.keys() :
            rebin = options.usermodule.rebin[v]

        if options.bkgs :
            bkg_hists = anaplot.GetVariableHistsFromTrees(trees_b,keys_b,v,weight ,n,low,high,normalize=options.normalize,rebin=rebin,scale=lumi_scale_factor)
            anaplot.PrepareBkgHistosForStack(bkg_hists)
        if options.susy :
            sig_hists = anaplot.GetVariableHistsFromTrees(trees_s,keys_s,v,weight ,n,low,high,normalize=options.normalize,rebin=rebin,scale=lumi_scale_factor)
        if options.data :
            data_hist = anaplot.GetVariableHistsFromTrees(tree_d ,key_d ,v,dweight,n,low,high,normalize=options.normalize,rebin=rebin,scale=lumi_scale_factor)[0]
            data_hist.SetLineWidth(2)
            data_hist.SetLineColor(1)
            data_hist.SetMarkerColor(1)

        nsignal = 0
        for si,s in enumerate(keys_s) :
            mass_map = GetMassesMap()
            for i in mass_map.keys() :
                if str(i) in sig_hists[si].GetName() :
                    title = '%s,%s'%(mass_map[i]['mc1'],mass_map[i]['mn1'])
                    sig_hists[si].SetTitle(title)
            sig_hists[si].SetLineWidth(2)
            sig_hists[si].SetLineColor(signal_colors[nsignal])
            sig_hists[si].SetMarkerColor(signal_colors[nsignal])
            nsignal += 1

        cans.append(anaplot.DrawHistos(v,v,xlabel,bkg_hists,sig_hists,data_hist,dostack=options.stack,log=options.log,ratio=options.ratio,fb=options.fb))

    if not options.batch :
        raw_input('Press enter to exit')

    if options.save :
        for can in cans :
            can.Print(can.GetName()+'.pdf')

    print 'done.'
    return

if __name__ == '__main__':

    p = anaplot.TreePlottingOptParser()
    options,args = p.parse_args()

    if not options.variables :
        print 'Error! Please specify a variable!'
        sys.exit()

    main(options,args)

