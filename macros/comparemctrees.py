#!/bin/usr/env python

##
## This macro takes a file produced using pennSoftLepton using trees (from ntupler)
##

import ROOT,sys,os
from pennSoftLepton.PyHelpers import GetMassesMap,GetHWWColors
import pennSoftLepton.TAxisFunctions as taxisfunc
import pennSoftLepton.PyAnalysisPlotting as anaplot
import pennSoftLepton.PlotFunctions as plotfunc

#-------------------------------------------------------------------------
def main(options,args) :

    plotfunc.SetupStyle()

    for i in options.histformat.keys() :
        print '\'%s\':'%(i),options.histformat[i]

    anaplot.LoadRootCore()
    
    files_mc1,trees_mc1,keys_mc1,cf_mc1 = anaplot.GetTreesFromFiles(options.mc1)
    files_mc2,trees_mc2,keys_mc2,cf_mc2 = anaplot.GetTreesFromFiles(options.mc2)

    weight = 'TotalWeightNoSF'
    if ''.join(options.cuts) :
        weight = weight+'*(%s)'%(' && '.join(options.cuts))
    if hasattr(options.usermodule,'weight') :
        weight += '*(%s)'%(options.usermodule.weight)

    cans = []

    # get the histograms from the files
    for v in options.variables.split(',') :
        n,low,high = options.histformat[v][:3]
        xlabel = options.histformat[v][3]

        mc_hists = []

        rebin = []
        if hasattr(options.usermodule,'rebin') and v in options.usermodule.rebin.keys() :
            rebin = options.usermodule.rebin[v]

        if options.mc1 :
            mc_hists.append(anaplot.GetVariableHistsFromTrees(trees_mc1,keys_mc1,v,weight ,n,low,high,normalize=options.normalize,rebin=rebin)[0])
            mc_hists[-1].SetTitle(options.label1)
            mc_hists[-1].SetLineWidth(2)
            mc_hists[-1].SetLineColor(1)
            mc_hists[-1].SetMarkerColor(1)
        if options.mc2 :
            mc_hists.append(anaplot.GetVariableHistsFromTrees(trees_mc2,keys_mc2,v,weight ,n,low,high,normalize=options.normalize,rebin=rebin)[0])
            mc_hists[-1].SetTitle(options.label2)
            mc_hists[-1].SetLineWidth(ROOT.kRed+1)
            mc_hists[-1].SetLineColor(ROOT.kRed+1)
            mc_hists[-1].SetMarkerColor(ROOT.kRed+1)

        cans.append(anaplot.DrawHistos(v,v,xlabel,mc_hists,dostack=False,log=options.log,ratio=options.ratio,fb=options.fb))

    if not options.batch :
        raw_input('Press enter to exit')

    if options.save :
        for can in cans :
            can.Print(can.GetName()+'.pdf')

    print 'done.'
    return

if __name__ == '__main__':

    p = anaplot.TreePlottingOptParser()
    p.p.remove_option('--susy')
    p.p.remove_option('--bkgs')
    p.p.remove_option('--data')
    p.p.remove_option('--nostack')

    p.p.add_option('--mc1',type='string',default='',dest='mc1',help='input files for first mc')
    p.p.add_option('--mc2',type='string',default='',dest='mc2',help='input files for second mc')

    p.p.add_option('--label1',type='string',default='',dest='label1',help='label for first mc')
    p.p.add_option('--label2',type='string',default='',dest='label2',help='label for second mc')

    options,args = p.parse_args()

    if not options.variables :
        print 'Error! Please specify a variable!'
        sys.exit()

    main(options,args)

