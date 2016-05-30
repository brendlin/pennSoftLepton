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

def Q(f1,f2,f3=0) :
    import math
    return math.sqrt(f1**2 + f2**2 + f3**2)

#-------------------------------------------------------------------------
def DrawHistos(name,variable,xlabel,bkg_hists=[],sig_hists=[],data_hist=None,dostack=True,log=False,ratio=False,fb=10) :
    #
    # bkg_hists is a list of background histograms (TH1)
    # sig_hists is a list of signal histograms (TH1)
    # variable is a variable name available in pennSoftLepton/Variables.cxx
    #
    import ROOT
    import pennSoftLepton.PlotFunctions as plotfunc
    import pennSoftLepton.TAxisFunctions as taxisfunc
    import math
    #
    # stack, before adding SUSY histograms
    #
    canname = name.replace('[','_').replace(']','').replace('_index','')
    canname = canname.replace('[','_').replace(']','_').replace('(','_').replace(')','_').replace('/','_over_')
    canname = canname.replace('>','gt').replace('<','lt').replace('-','minus').replace(' ','_').replace('&&','and')
    canname = canname.lstrip('_').rstrip('_')

    if not ratio :
        can = ROOT.TCanvas(canname,canname,500,500)
        if log : can.SetLogy()
    else :
        can = plotfunc.RatioCanvas(canname,canname,500,500)
        if log : can.GetPrimitive('pad_top').SetLogy()

    if bkg_hists :
        totb = bkg_hists[0].Clone()
        totb.SetNameTitle('SM_%s'%(variable),'remove me')
        totb.SetLineColor(ROOT.kRed)
        totb.SetLineWidth(1)
        totb.SetMarkerSize(0)
        totb.SetFillColor(0)
        for i in bkg_hists[1:] :
            totb.Add(i)
        totberror = totb.Clone()
        totberror.SetName(totb.GetName()+'_error')
        totberror.SetTitle('Tot. unc.')
        #totberror.SetFillColor(ROOT.kViolet-9)
        totberror.SetFillColor(1)
        totberror.SetFillStyle(3254)
        #totberror.SetFillStyle(3344)
        #totberror.SetFillStyle(3001)

    for index,i in enumerate(bkg_hists) :
        # if no data, but you specified you wanted a ratio, then do ratio of MC
        if (not dostack) :
            i.SetLineWidth(2)
            i.SetLineColor(i.GetMarkerColor())
        if (index > 0) and (not data_hist) and (not dostack) and (ratio) :
            plotfunc.AddRatio(can,i,bkg_hists[0])
        else :
            plotfunc.AddHistogram(can,i)

    if bkg_hists and dostack :
        plotfunc.Stack(can)
        plotfunc.AddHistogram(can,totberror,drawopt='E2')
        pm = u"\u00B1"
        print 'Total background: %2.2f%s%2.2f'%(totb.Integral(0,totb.GetNbinsX()+1),pm,math.sqrt(sum(list(totb.GetSumw2()))))
#         for i in range(totb.GetNbinsX()) :
#            print ' --  %2.2f%s%2.2f'%(totb.GetBinContent(i),pm,totb.GetBinError(i))
        plotfunc.AddHistogram(can,totb,drawopt='hist')

    for h in sig_hists :
        plotfunc.AddHistogram(can,h)

    if data_hist :
        if ratio :
            plotfunc.AddRatio(can,data_hist,totb)
        else :
            plotfunc.AddHistogram(can,data_hist)
        #p0 = data_hist.Chi2Test(totb,"UW")
        p0 = data_hist.KolmogorovTest(totb,"UO")
        print 'p0 is',p0

    plotfunc.FormatCanvasAxes(can)
    text_lines = [plotfunc.GetAtlasInternalText()]
    text_lines += [plotfunc.GetSqrtsText(13)+', %1.1f %s^{-1}'%(3.2,'fb')]
    if fb > 0 :
        text_lines += []
    text_lines += ['\ell^{\pm}\ell\prime^{\mp}\ell^{\pm}']
    #text_lines += ['{(\ell\prime, \ell } #font[72]{= e  or }']
    text_lines += ['(\ell\prime,\ell}=']
    a = ROOT.TLatex()
    a.SetTextSize(18)
    a.SetTextFont(43)
    a.DrawLatexNDC(0.325,0.74,'e,#mu or #mu,e)')
    option = ['f','f','f','f','fl','f','f','f','f','f','f','f']
    if ratio :
        plotfunc.DrawText(can,text_lines,0.15,0.65,0.5,0.90,totalentries=4)
        plotfunc.MakeLegend(can,0.53,0.65,0.92,0.90,totalentries=5,ncolumns=1,skip=['remove me'],option=option)
        taxisfunc.SetYaxisRanges(plotfunc.GetBotPad(can),0,2)
    else :
        plotfunc.DrawText(can,text_lines,0.1,0.73,0.5,0.92,totalentries=4)
        plotfunc.MakeLegend(can,0.60,0.63,0.94,0.92,totalentries=5,ncolumns=1,skip=['remove me'],option=option)
    plotfunc.SetAxisLabels(can,xlabel,'Events')
    plotfunc.AutoFixAxes(can)
    return can


colors_dict = {
    'wz':ROOT.kWhite,
    'W^{#pm}Z #times 1.17':ROOT.kViolet-9,
    #'W^{#pm}Z #times 1.17':ROOT.kRed-8,
    'ZZ':ROOT.kMagenta+3,
    #'tt+V':ROOT.kSpring+6,
    #'tt+V':ROOT.kSpring-4,
    #'tt+V':ROOT.kSpring-9,
    'tt+V':ROOT.kOrange,
    #'VVV' :ROOT.kOrange,
    #'VVV' :ROOT.,
    'ttbar':ROOT.kGreen-3,
    't#bar{t} / Wt / WW':ROOT.kGreen-3,
    'Others':ROOT.kPink-8,
    'Z+j/Z#gamma':ROOT.kAzure+6,
    }

#-------------------------------------------------------------------------
def main(options,args) :
    
    print 'Loading c++...'
    anaplot.LoadRootCore()
    ROOT.gROOT.SetBatch(False)

    plotfunc.SetupStyle()
    style = ROOT.gROOT.GetStyle('mystyle')
    style.SetEndErrorSize(0)
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
            bkg_hists = []
            bkg_hists.append(anaplot.GetPassEventBkgHistos(v,options.key,'zz'   ,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=Q(0.05,0.08,0.04))[0])
            bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,options.key,'vvv'  ,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=Q(0.05,0.20))[0])
            bkg_hists[-1].SetTitle('Others')
            bkg_hists.append(anaplot.GetPassEventBkgHistos(v,options.key,'ttv'  ,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=Q(0.05,0.13))[0])
            bkg_hists[-1].SetTitle('tt+V')
            bkg_hists.append(anaplot.GetPassEventBkgHistos(v,options.key,'wz'   ,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=Q(0.05,0.07))[0])
            bkg_hists[-1].Scale(1.17/1.15)
            bkg_hists[-1].SetTitle('W^{#pm}Z #times 1.17')
            bkg_hists.append(anaplot.GetPassEventBkgHistos(v,options.key,'ttbar' ,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.)[0])
            bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,options.key,'tw'   ,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.)[0])
            bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,options.key,'qqww',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.)[0])
            bkg_hists[-1].SetTitle('t#bar{t} / Wt / WW')
            
            #bkg_hists.append(anaplot.GetPassEventBkgHistos(v,options.key,'zjet' ,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.)[0])
            #bkg_hists.append(anaplot.GetPassEventBkgHistos(v,options.key,'zgam' ,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.)[0])

            anaplot.PrepareBkgHistosForStack(bkg_hists,colors_dict=colors_dict)

            for i in bkg_hists :
                i.SetLineColor(i.GetFillColor())

            data_hist = anaplot.GetPassEventBkgHistos(v,options.key,'data',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high)[0]
            #data_hist = anaplot.GetPassEventBkgHistos(v,'FFTool_z_mccl_ltt_allNum','zjetdd',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high)[0]

                
        if options.susy :
            sig_hists = anaplot.GetPassEventSigHistos(v,options.key,options.susy,normalize=options.normalize,rebin=rebin)


        #bkg_hists[0].GetXaxis().SetNdivisions(5,0,0)
        cans.append(DrawHistos('%s_%s'%(options.key,v),v,xlabel,bkg_hists,sig_hists,data_hist=data_hist,dostack=options.stack,log=options.log,ratio=options.ratio,fb=options.fb))
        taxisfunc.SetXaxisRanges(cans[-1],low,high)
        taxisfunc.SetYaxisRanges(cans[-1],0,12*0.9525)
        taxisfunc.SetNdivisions(cans[-1],9,0,0)

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
    p.p.remove_option('--key')    
    options,args = p.parse_args()
    options.key = 'FFTool_ttcr_ttt_allNum'
    main(options,args)

