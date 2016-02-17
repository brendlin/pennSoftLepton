#!/bin/usr/env python

##
## This macro takes a file produced using pennSoftLepton using ntuples from Ntupler.cxx.
##

import ROOT,sys,os,math
from pennSoftLepton.PyHelpers import PrintCutflow,GetMassesMap,GetHWWColors
from pennSoftLepton.PyHelpers import GetListOfKeysFromFile,GetListOfProcessesFromFile,GetListOfVariablesFromFile
import pennSoftLepton.TAxisFunctions as taxisfunc
import pennSoftLepton.PyAnalysisPlotting as anaplot
import pennSoftLepton.PlotFunctions as plotfunc
import pennSoftLepton.PyHelpers as helpers

signal_colors = [ROOT.kRed,ROOT.kBlue,ROOT.kGreen,ROOT.kOrange,ROOT.kCyan]*5

## kurt.brendlinger@cern.ch
##
## Arguments:
## --bkgs BACKGROUNDS (will be tallied in "Total Bkg."
## --susy SIGNAL.root (wz if you are doing the wz analysis)
## --data data.root
## --fb 10.0 (if your ntuples were produced using a different luminosity then this will scale the
##            numbers. HOWEVER it will not update the pileup reweighting!)
## --latex (for outputting rsults in latex)
## Be careful not to unblind yourself!
##

#-------------------------------------------------------------------------
def main(options,args) :

    anaplot.LoadRootCore()
    
    files_b,trees_b,keys_b,cf_b = anaplot.GetTreesFromFiles(options.bkgs)
    files_s,trees_s,keys_s,cf_s = anaplot.GetTreesFromFiles(options.susy)
    file_d ,tree_d ,key_d ,cf_d = anaplot.GetTreesFromFiles(options.data)
    all_cutflows = cf_b+cf_s+cf_d

    options.fb,lumi_scale_factor = helpers.GetTTreeLumiScaleFactor(files_b+files_s,options.fb)

    for i in all_cutflows :
        for j in i :
            j.Scale(lumi_scale_factor)

    dweight = ''
    weight = 'TotalWeightNoSF'
#     if ''.join(options.cuts) :
#         weight = weight+'*(%s)'%(' && '.join(options.cuts))
#         dweight = '('+' && '.join(options.cuts)+')'
    if hasattr(options.usermodule,'weight') :
        weight += '*(%s)'%(options.usermodule.weight)
    if hasattr(options.usermodule,'dataweight') :
        dweight += '*(%s)'%(options.usermodule.dataweight)        

    cuts = []
    samples = []
    data_index = -1
    if options.bkgs : samples += list(a.replace('.root','') for a in options.bkgs.split(','))
    samples += ['Total Bkg.']
    totbkg_index = len(samples)-1
    if options.susy : samples += list(a.replace('.root','') for a in options.susy.split(','))
    if options.data :
        samples += ['data']
        data_index = len(samples)-1
    samples += ['s/b']
    s_over_b_index = len(samples)-1
    samples += ['purity']
    purity_index = len(samples)-1
    print samples

    #
    # Figure out what all the cuts were
    #
    firsthistlist = all_cutflows[0]
    for i in firsthistlist :
        for y in range(i.GetNbinsY(),0,-1) :
            cuts.append(i.GetYaxis().GetBinLabel(y))
    
    if hasattr(options.usermodule,'cutflow') :
        for i in options.usermodule.cutflow :
            cuts.insert(0,i[0])

    cuts.reverse()
    print cuts

    #
    # Make a histogram to fill
    #
    cutflow_hist = ROOT.TH2F('Cutflow','Cutflow',len(samples),0,len(samples),len(cuts),0,len(cuts))
    for i in range(len(samples)) :
        cutflow_hist.GetXaxis().SetBinLabel(i+1,samples[i])
    for i in range(len(cuts)) :
        cutflow_hist.GetYaxis().SetBinLabel(i+1,cuts[i])
    
    #
    # Grab the bin content from the "all" histograms that were saved in each file. (bkgs)
    #
    current_sample = 1
    for i,histlist in enumerate(cf_b) :
        current_cut = len(cuts)
        if hasattr(options.usermodule,'cutflow') :
            tree_cuts_list = list('('+a[1]+')' for a in options.usermodule.cutflow)
            for j in options.usermodule.cutflow :
                tmp_weight = weight+'*(%s)'%(' && '.join(tree_cuts_list))
                trees_b[keys_b[i]].Draw('GRL>>h1(2,0,2)',tmp_weight,'egoff')
                tree_cuts_list.pop()
                hist = ROOT.gDirectory.Get('h1')
                hist.Scale(lumi_scale_factor)
                cutflow_hist.SetBinContent(current_sample,current_cut,hist.GetBinContent(2))
                cutflow_hist.SetBinError(current_sample,current_cut,hist.GetBinError(2))
                totbkg_content = cutflow_hist.GetBinContent(totbkg_index+1,current_cut) + hist.GetBinContent(2)
                totbkg_error   = math.sqrt(cutflow_hist.GetBinError(totbkg_index+1,current_cut)**2 + hist.GetBinError(2)**2)
                cutflow_hist.SetBinContent(totbkg_index+1,current_cut,totbkg_content)
                cutflow_hist.SetBinError(totbkg_index+1,current_cut,totbkg_error)
                ROOT.gROOT.ProcessLine('delete h1')
                current_cut -= 1
        for hist in histlist :
            for x in range(hist.GetNbinsX()) :
                if hist.GetXaxis().GetBinLabel(x+1) == 'all' :
                    allbin = x+1
                    break
            for y in range(hist.GetNbinsY(),0,-1) :
                cutflow_hist.SetBinContent(current_sample,current_cut,hist.GetBinContent(allbin,y))
                cutflow_hist.SetBinError(current_sample,current_cut,hist.GetBinError(allbin,y))
                totbkg_content = cutflow_hist.GetBinContent(totbkg_index+1,current_cut) + hist.GetBinContent(allbin,y)
                totbkg_error   = math.sqrt(cutflow_hist.GetBinError(totbkg_index+1,current_cut)**2 + hist.GetBinError(allbin,y)**2)
                cutflow_hist.SetBinContent(totbkg_index+1,current_cut,totbkg_content)
                cutflow_hist.SetBinError(totbkg_index+1,current_cut,totbkg_error)
                current_cut -= 1
        current_sample += 1

    current_sample += 1

    #
    # susy (or wz!)
    #
    for i,histlist in enumerate(cf_s) :
        current_cut = len(cuts)
        if hasattr(options.usermodule,'cutflow') :
            tree_cuts_list = list('('+a[1]+')' for a in options.usermodule.cutflow)
            for j in options.usermodule.cutflow :
                #print tree_cuts_list
                tmp_weight = weight+'*(%s)'%(' && '.join(tree_cuts_list))
                trees_s[keys_s[i]].Draw('GRL>>h1(2,0,2)',tmp_weight,'egoff')
                tree_cuts_list.pop()
                hist = ROOT.gDirectory.Get('h1')
                hist.Scale(lumi_scale_factor)
                signal_n = hist.GetBinContent(2)
                cutflow_hist.SetBinContent(current_sample,current_cut,signal_n)
                if signal_n>0 : cutflow_hist.SetBinError(current_sample,current_cut,100./float(math.sqrt(signal_n)))
                tot_bkg = cutflow_hist.GetBinContent(totbkg_index+1,current_cut)
                if tot_bkg : cutflow_hist.SetBinContent(s_over_b_index+1,current_cut,signal_n/float(tot_bkg))
                if tot_bkg+signal_n : cutflow_hist.SetBinContent(purity_index+1,current_cut,100.*signal_n/float(tot_bkg+signal_n))
                ROOT.gROOT.ProcessLine('delete h1')
                current_cut -= 1
        for hist in histlist :
            for x in range(hist.GetNbinsX()) :
                if hist.GetXaxis().GetBinLabel(x+1) == 'all' :
                    allbin = x+1
                    break
            for y in range(hist.GetNbinsY(),0,-1) :
                signal_n = hist.GetBinContent(allbin,y)
                cutflow_hist.SetBinContent(current_sample,current_cut,signal_n)
                cutflow_hist.SetBinError(current_sample,current_cut,100./float(math.sqrt(signal_n)))
                tot_bkg = cutflow_hist.GetBinContent(totbkg_index+1,current_cut)
                if tot_bkg : cutflow_hist.SetBinContent(s_over_b_index+1,current_cut,signal_n/float(tot_bkg))
                if tot_bkg+signal_n : cutflow_hist.SetBinContent(purity_index+1,current_cut,100.*signal_n/float(tot_bkg+signal_n))
                current_cut -= 1
        current_sample += 1

    #
    # data
    #
    for i,histlist in enumerate(cf_d) :
        current_cut = len(cuts)
        if hasattr(options.usermodule,'cutflow') :
            tree_cuts_list = list('('+a[1]+')' for a in options.usermodule.cutflow)
            for j in options.usermodule.cutflow :
                tmp_weight = weight+'*(%s)'%(' && '.join(tree_cuts_list))
                tree_d[key_d[i]].Draw('GRL>>h1(2,0,2)',tmp_weight,'egoff')
                tree_cuts_list.pop()
                hist = ROOT.gDirectory.Get('h1')
                hist.Scale(lumi_scale_factor)
                cutflow_hist.SetBinContent(current_sample,current_cut,hist.GetBinContent(2))
                cutflow_hist.SetBinError(current_sample,current_cut,hist.GetBinError(2))
                ROOT.gROOT.ProcessLine('delete h1')
                current_cut -= 1
        for hist in histlist :
            for x in range(hist.GetNbinsX()) :
                if (hist.GetXaxis().GetBinLabel(x+1) == 'data') :
                    allbin = x+1
                    break
            for y in range(hist.GetNbinsY(),0,-1) :
                cutflow_hist.SetBinContent(current_sample,current_cut,hist.GetBinContent(allbin,y))
                cutflow_hist.SetBinError(current_sample,current_cut,hist.GetBinError(allbin,y))
                current_cut -= 1
        current_sample += 1

    PrintCutflow('Cutflow',cutflow_hist,samp_list=','.join(samples),latex=options.latex)

    return

if __name__ == '__main__':

    p = anaplot.TreePlottingOptParser()
    p.p.remove_option('--variables')
    p.p.remove_option('--cuts')
    p.p.remove_option('--ratio')
    p.p.remove_option('--save')
    p.p.remove_option('--normalize')
    p.p.remove_option('--nostack')
    p.p.remove_option('--log')
    p.p.remove_option('--batch')
    p.p.add_option('--latex',action='store_true',default=False,dest='latex',help='Output is latex')
    options,args = p.parse_args()

    main(options,args)

