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
from pennSoftLepton.PyRandomStuff import GetFFHist,AddFFHistIf
import importlib

signal_colors = [ROOT.kRed,ROOT.kBlue,ROOT.kGreen,ROOT.kOrange,ROOT.kCyan]*5

def AddIf(h1,h2,scale=1) :
    if h2:
        h1.Add(h2[0],scale)
    return

colors_dict = {
    'wz':ROOT.kWhite,
    'W^{#pm}Z #times 1.17':ROOT.kWhite,
    'ZZ':ROOT.kMagenta+3,
    'ttv':ROOT.kOrange-2,
    'vvv' :220,
    'ttbar':ROOT.kYellow+1,
    't#bar{t}/Wt/WW':ROOT.kYellow+1,
    't#bar{t}/Wt/WW/ttv/other':ROOT.kPink-8,
    'Others':ROOT.kPink-8,
    'Z+j / Z#gamma':ROOT.kAzure+6,
    }

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
        #can = ROOT.TCanvas(canname,canname,572,440)
        can = ROOT.TCanvas(canname,canname,500,500)
        if log : can.SetLogy()
    else :
        can = plotfunc.RatioCanvas(canname,canname,572,440)
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
        totberror.SetFillColor(ROOT.kViolet-9)
        #totberror.SetFillStyle(3254)

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
    text_lines += ['\ell\prime\ell\ell']
#     text_lines += ['{(\ell\prime, \ell } #font[72]{= e  or }']
    text_lines += ['(\ell\prime,\ell}=']
    a = ROOT.TLatex()
    a.SetTextSize(18)
    a.SetTextFont(43)
    a.DrawLatexNDC(0.325,0.74,'e or #mu)')
    option = ['f','f','f','f','fl','f','f','f']
    if ratio :
        plotfunc.DrawText(can,text_lines,0.15,0.65,0.5,0.90,totalentries=4)
        plotfunc.MakeLegend(can,0.53,0.65,0.92,0.90,totalentries=5,ncolumns=1,skip=['remove me'],option=option)
        #plotfunc.MakeLegend(can,0.53,0.65,0.92,0.90,totalentries=3,ncolumns=2,skip=['remove me'],option=option)
        taxisfunc.SetYaxisRanges(plotfunc.GetBotPad(can),0,2)
    else :
        plotfunc.DrawText(can,text_lines,0.1,0.73,0.5,0.92,totalentries=4)
        #plotfunc.MakeLegend(can,0.60,0.63,0.94,0.92,totalentries=5,ncolumns=1,skip=['remove me'],option=option)
        plotfunc.MakeLegend(can,0.56,0.73,0.94,0.92,totalentries=7,ncolumns=2,skip=['remove me'],option=option)
    plotfunc.SetAxisLabels(can,xlabel,'Events / 10 GeV')
    plotfunc.AutoFixAxes(can)
    return can

#-------------------------------------------------------------------------
def main(options,args) :
    
    print 'Loading c++...'
    anaplot.LoadRootCore()
    ROOT.gROOT.SetBatch(False)

    plotfunc.SetupStyle()
    style = ROOT.gROOT.GetStyle('mystyle')
    style.SetEndErrorSize(0)

    channel = 'all'
    if options.e :
        channel = 'Ele'
    if options.mu :
        channel = 'Mu'

    options.key = 'FFTool_ttt_%sNum'%(channel)
    
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
            #bkg_hists = anaplot.GetPassEventBkgHistos(v,options.key,bkg_processes,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high)

            bkg_hists.append(anaplot.GetPassEventBkgHistos(v,'FFTool_ttt_%sNum'%(channel),'vvv',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=Q(0.05,0.20))[0])
            bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_ttt_%sNum'%(channel),'tother',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=Q(0.05,0.13))[0])
            bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_ttt_%sNum'%(channel),'tz',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=Q(0.05,0.15))[0])
            bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_ttt_%sNum'%(channel),'ttv',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=Q(0.05,0.13))[0])
#             bkg_hists[-1].Scale(3.15813/3.20905) # all other MC (but not ttbar) scaled down lumi-wise
            
            bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_ttt_%sNum'%(channel),'ttbar',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.32)[0])
            bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_ttt_%sNum'%(channel),'tw',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.32)[0])
            AddIf(bkg_hists[-1],anaplot.GetPassEventBkgHistos(v,'FFTool_ttt_%sNum'%(channel),'qqww',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.32))
            #bkg_hists[-1].SetTitle('t#bar{t}/Wt/WW/ttv/other')
            bkg_hists[-1].SetTitle('Others')

            bkg_hists.append(anaplot.GetPassEventBkgHistos(v,'FFTool_ttt_%sNum'%(channel),'zz',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=Q(0.05,0.08,0.04))[0])
#             bkg_hists[-1].Scale(3.15813/3.20905) # ZZ scaled down lumi-wise
            bkg_hists[-1].SetTitle('ZZ')

            bkg_hists.append(anaplot.GetPassEventBkgHistos(v,'FFTool_z_data_ltt_%sNum'%(channel),'zjetdd' ,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.00,ltt_tlt_ttl=True)[0])
            bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_z_data_ltt_%sNum'%(channel),'ttbar' ,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.00,ltt_tlt_ttl=True)[0],-1)
            bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_z_data_ltt_%sNum'%(channel),'tw'    ,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.00,ltt_tlt_ttl=True)[0],-1)
            AddIf(bkg_hists[-1],anaplot.GetPassEventBkgHistos(v,'FFTool_z_data_ltt_%sNum'%(channel),'qqww',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.00,ltt_tlt_ttl=True),-1)
            bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_z_data_ltt_%sNum'%(channel),'wz'    ,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.00,ltt_tlt_ttl=True)[0],-1)
            bkg_hists[-1].Add(anaplot.GetPassEventBkgHistos(v,'FFTool_z_data_ltt_%sNum'%(channel),'zz'    ,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=0.00,ltt_tlt_ttl=True)[0],-1)
            for i in range(bkg_hists[-1].GetNbinsX()) :
                bkg_hists[-1].SetBinError(i+1,bkg_hists[-1].GetBinContent(i+1)*0.38)
            
            bkg_hists[-1].SetTitle('Z+j / Z#gamma')
            import math
            pm = u"\u00B1"
            print '%s: %2.2f%s%2.2f'%('zjetdd',bkg_hists[-1].Integral(0,bkg_hists[-1].GetNbinsX()+1),pm,math.sqrt(sum(list(bkg_hists[-1].GetSumw2()))))

            bkg_hists += anaplot.GetPassEventBkgHistos(v,options.key,'wz',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high,globalError=Q(0.07,0.05))
            bkg_hists[-1].Scale(1.17/1.15)
            bkg_hists[-1].SetTitle('W^{#pm}Z #times 1.17')

#             bkg_hists = []
#             bkg_hists.append(ROOT.TH1F('hist','hist',500,0,500))
#             for i in range(bkg_hists[-1].GetNbinsX()) :
#                 bkg_hists[-1].Fill(i+0.5)

#             for i in bkg_hists :
#                 taxisfunc.PutOverflowIntoLastBin(i,high)

#             for b in bkg_hists :
#                 if b.GetTitle() == 'zjetdd' :
#                     for i in range(b.GetNbinsX()) :
#                         b.SetBinError(i+1,b.GetBinContent(i+1)*0.38)
#                 if b.GetTitle() == 't#bar{t}/Wt/WW/ttv/other' :
#                     for i in range(b.GetNbinsX()) :
#                         b.SetBinError(i+1,b.GetBinContent(i+1)*0.32)
#                 if b.GetTitle() == '' :
#                     for i in range(b.GetNbinsX()) :
#                         b.SetBinError(i+1,b.GetBinContent(i+1)*0.32)

            anaplot.PrepareBkgHistosForStack(bkg_hists,colors_dict = colors_dict)

            for i in bkg_hists[:-1] :
                i.SetLineColor(i.GetFillColor())
            bkg_hists[-1].SetLineColor(ROOT.kRed)

            if 'data' in options.processes :
                data_hist = anaplot.GetPassEventBkgHistos(v,options.key,'data',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high)[0]
                #data_hist = anaplot.GetPassEventBkgHistos(v,'FFTool_z_mccl_ltt_%sNum'%(channel),'zjetdd',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high)[0]
                taxisfunc.PutOverflowIntoLastBin(data_hist,high)
                
        if options.susy :
            sig_hists = anaplot.GetPassEventSigHistos(v,options.key,options.susy,normalize=options.normalize,rebin=rebin)


        cans.append(DrawHistos('%s_%s'%(options.key,v),v,xlabel,bkg_hists,sig_hists,data_hist=data_hist,dostack=options.stack,log=options.log,ratio=options.ratio,fb=options.fb))
        taxisfunc.SetXaxisRanges(cans[-1],low,high)
        taxisfunc.SetYaxisRanges(cans[-1],0,57.15)

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
    p.p.add_option('--e'  ,action='store_true',default=False  ,dest='e',help='Plot electrons only' )
    p.p.add_option('--mu'  ,action='store_true',default=False  ,dest='mu',help='Plot muons only' )
    options,args = p.parse_args()
    main(options,args)

