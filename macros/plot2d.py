#!/bin/usr/env python

##
## This macro takes a file produced using the pennSoftLepton Ntupler.
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
    weight = 'TotalWeightNoSF*lepSFRecoIdIso[lep0_index]*lepSFRecoIdIso[lep1_index]*lepSFRecoIdIso[lep2_index]*EvtTrigSF*AntiIDSF_lep3*PassEvent_SMWZsig'
    #weight = 'TotalWeightNoPupNoSF'
    if ''.join(options.cuts) :
        weight = weight+'*(%s)'%(' && '.join(options.cuts))
        dweight = '('+' && '.join(options.cuts)+')'
    if hasattr(options.usermodule,'weight') :
        weight += '*(%s)'%(options.usermodule.weight)
    if hasattr(options.usermodule,'dataweight') :
        dweight += '*(%s)'%(options.usermodule.dataweight)        

    cans = []

    # get the histograms from the files
    for vi,v1 in enumerate(options.variables.split(',')) :
        n1,low1,high1 = options.histformat[v1][:3]
        xlabel1 = options.histformat[v1][3]

        rebin1 = []
        if hasattr(options.usermodule,'rebin') and v1 in options.usermodule.rebin.keys() :
            rebin1 = options.usermodule.rebin[v1]

        for vj,v2 in enumerate(options.variables.split(',')) :
            if v2 == v1 : continue
            if vj > vi : continue

            n2,low2,high2 = options.histformat[v2][:3]
            xlabel2 = options.histformat[v2][3]

            rebin2 = []
            if hasattr(options.usermodule,'rebin') and v2 in options.usermodule.rebin.keys() :
                rebin2 = options.usermodule.rebin[v2]
            
            bkg_hists = []
            sig_hists = []
            data_hist = None

            if options.bkgs :
                bkg_hists = anaplot.Get2dVariableHistsFromTrees(trees_b,keys_b,v1,v2,weight ,n1,low1,high1,n2,low2,high2,normalize=options.normalize,rebin1=rebin1,rebin2=rebin2,scale=lumi_scale_factor)
                anaplot.PrepareBkgHistosForStack(bkg_hists)
                for i,h in enumerate(bkg_hists) :
                    canname = '%s_%s_%s'%(v1,v2,keys_b[i])
                    cans.append(ROOT.TCanvas(canname,canname,500,500))
                    h.SetMinimum(-0.00001)
                    plotfunc.AddHistogram(cans[-1],h,drawopt='colz')
                    plotfunc.FormatCanvasAxes(cans[-1])
                    plotfunc.SetAxisLabels(cans[-1],xlabel1,xlabel2)
            if options.susy :
                sig_hists = anaplot.Get2dVariableHistsFromTrees(trees_s,keys_s,v1,v2,weight ,n1,low1,high1,n2,low2,high2,normalize=options.normalize,rebin1=rebin1,rebin2=rebin2,scale=lumi_scale_factor)
                for i,h in enumerate(sig_hists) :
                    canname = '%s_%s_%s'%(v1,v2,keys_s[i])
                    cans.append(ROOT.TCanvas(canname,canname,500,500))
                    h.SetMinimum(-0.00001)
                    plotfunc.AddHistogram(cans[-1],h,drawopt='colz')
                    plotfunc.FormatCanvasAxes(cans[-1])
                    plotfunc.SetAxisLabels(cans[-1],xlabel1,xlabel2)
                
            if options.data :
                data_hist = anaplot.Get2dVariableHistsFromTrees(tree_d ,key_d ,v1,v2,dweight,n1,low1,high1,n2,low2,high2,normalize=options.normalize,rebin1=rebin1,rebin2=rebin2,scale=lumi_scale_factor)[0]
                data_hist.SetLineWidth(2)
                data_hist.SetLineColor(1)
                data_hist.SetMarkerColor(1)
                canname = '%s_%s_%s'%(v1,v2,'data')
                cans.append(ROOT.TCanvas(canname,canname,500,500))
                h.SetMinimum(-0.00001)
                plotfunc.AddHistogram(cans[-1],h,drawopt='colz')
                plotfunc.FormatCanvasAxes(cans[-1])
                plotfunc.SetAxisLabels(cans[-1],xlabel1,xlabel2)

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

