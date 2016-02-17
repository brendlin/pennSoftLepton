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
    
    options.keys = []
    if not options.e :
        options.keys += [options.key+'_ttt_MuNum',
                         options.key+'_ltt_MuDen']
    if not options.mu :
        options.keys += [options.key+'_ttt_EleNum',
                         options.key+'_ltt_EleDen']

    print options.keys

    #
    # Draw plots
    #
    print 'Drawing the plots'
    cans = []
    for v in options.variables.split(',') :
        if not v : continue
        n,low,high = options.histformat[v][:3]

        lep_pt_dijet_num = None
        lep_pt_dijet_den = None

        for k in options.keys :

            bkg_hists = []
            sig_hists = []
            data_hist = None

            rebin = []
            if hasattr(options.usermodule,'rebin') and v in options.usermodule.rebin.keys() :
                rebin = options.usermodule.rebin[v]
                if ('lepPt' in v) and ('Mu' in k) : rebin = [0,9,15,20,30,100]

            if options.file :
                bkg_processes = options.processes.replace('data','').replace(',,',',')
                bkg_hists = anaplot.GetPassEventBkgHistos(v,k,bkg_processes,options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high)
                anaplot.PrepareBkgHistosForStack(bkg_hists)
                if 'data' in options.processes :
                    data_hist = anaplot.GetPassEventBkgHistos(v,k,'data',options.file,normalize=options.normalize,rebin=rebin,n=n,low=low,high=high)[0]

            xlabel = ROOT.PSL.GetXaxisLabel(v.split('[')[0])
            cans.append(anaplot.DrawHistos('%s_%s'%(k,v),v,xlabel,bkg_hists,sig_hists,data_hist=data_hist,dostack=options.stack,log=options.log,ratio=options.ratio,fb=options.fb))

    #
    # Specifically get the fake factor in pt:
    #
    cans_divide = []

    v = 'lepPt[lepW_index]'
    rebin_mu = [0,9,15,20,30,100]
    rebin_e  = [0,9,15,20,30,50,100]

    hists_MuNum  = dict()
    hists_MuDen  = dict()
    hists_EleNum = dict()
    hists_EleDen = dict()

    mu_options = options.file,options.normalize,rebin_mu

    for p in options.processes.split(',') :
        hists_MuNum [p] = anaplot.GetPassEventBkgHistos(v,options.key+'_ttt_MuNum' ,p,*mu_options)[0]
        hists_MuDen [p] = anaplot.GetPassEventBkgHistos(v,options.key+'_ltt_MuDen' ,p,options.file,normalize=options.normalize,rebin=rebin_mu)[0]
        hists_EleNum[p] = anaplot.GetPassEventBkgHistos(v,options.key+'_ttt_EleNum',p,options.file,normalize=options.normalize,rebin=rebin_e)[0]
        hists_EleDen[p] = anaplot.GetPassEventBkgHistos(v,options.key+'_ltt_EleDen',p,options.file,normalize=options.normalize,rebin=rebin_e)[0]

    z_w = ''
    tmp = ROOT.TFile(options.file)
    syst_check = 'PassEvent_FFTool_forsyst_%s_ttt_MuNum/PassEvent_FFTool_forsyst_%s_ttt_MuNum_zjet_lepPt_antilep0_index'%('wid','wid')
    print syst_check
    if tmp.Get(syst_check) :
        z_w = 'wid'
    elif tmp.Get(syst_check.replace('wid','zid')) :
        z_w = 'zid'
    
    if z_w and options.syst :
        do_z = (z_w == 'zid')
        # try to get SR FF
        hists_MuNum ['zjet_sr']   = anaplot.GetPassEventBkgHistos('lepPt_antilep0_index'   ,'FFTool_forsyst_%s_ttt_MuNum'%(z_w) ,'zjet',options.file,normalize=options.normalize,rebin=rebin_mu)[0]
        if True: hists_MuDen ['zjet_sr']   = anaplot.GetPassEventBkgHistos('lepPt_lepW_index'       ,'FFTool_forsyst_ltt_MuDen','zjet',options.file,normalize=options.normalize,rebin=rebin_mu)[0]
        if do_z: hists_MuDen ['zjet_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepleadZ_index'   ,'FFTool_forsyst_tlt_MuDen','zjet',options.file,normalize=options.normalize,rebin=rebin_mu)[0])
        if do_z: hists_MuDen ['zjet_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepsubleadZ_index','FFTool_forsyst_ttl_MuDen','zjet',options.file,normalize=options.normalize,rebin=rebin_mu)[0])

        hists_MuNum ['zjet_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_antilep0_index'   ,'FFTool_forsyst_%s_ttt_MuNum'%(z_w) ,'zgam',options.file,normalize=options.normalize,rebin=rebin_mu)[0])
        if True: hists_MuDen ['zjet_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepW_index'       ,'FFTool_forsyst_ltt_MuDen','zgam',options.file,normalize=options.normalize,rebin=rebin_mu)[0])
        if do_z: hists_MuDen ['zjet_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepleadZ_index'   ,'FFTool_forsyst_tlt_MuDen','zgam',options.file,normalize=options.normalize,rebin=rebin_mu)[0])
        if do_z: hists_MuDen ['zjet_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepsubleadZ_index','FFTool_forsyst_ttl_MuDen','zgam',options.file,normalize=options.normalize,rebin=rebin_mu)[0])

        hists_EleNum['zjet_sr']   = anaplot.GetPassEventBkgHistos('lepPt_antilep0_index'   ,'FFTool_forsyst_%s_ttt_EleNum'%(z_w),'zjet',options.file,normalize=options.normalize,rebin=rebin_e)[0]
        if True: hists_EleDen['zjet_sr']   = anaplot.GetPassEventBkgHistos('lepPt_lepW_index'       ,'FFTool_forsyst_ltt_EleDen','zjet',options.file,normalize=options.normalize,rebin=rebin_e)[0]
        if do_z: hists_EleDen['zjet_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepleadZ_index'   ,'FFTool_forsyst_tlt_EleDen','zjet',options.file,normalize=options.normalize,rebin=rebin_e)[0])
        if do_z: hists_EleDen['zjet_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepsubleadZ_index','FFTool_forsyst_ttl_EleDen','zjet',options.file,normalize=options.normalize,rebin=rebin_e)[0])

        hists_EleNum['zjet_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_antilep0_index'   ,'FFTool_forsyst_%s_ttt_EleNum'%(z_w),'zgam',options.file,normalize=options.normalize,rebin=rebin_e)[0])
        if True: hists_EleDen['zjet_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepW_index'       ,'FFTool_forsyst_ltt_EleDen','zgam',options.file,normalize=options.normalize,rebin=rebin_e)[0])
        if do_z: hists_EleDen['zjet_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepleadZ_index'   ,'FFTool_forsyst_tlt_EleDen','zgam',options.file,normalize=options.normalize,rebin=rebin_e)[0])
        if do_z: hists_EleDen['zjet_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepsubleadZ_index','FFTool_forsyst_ttl_EleDen','zgam',options.file,normalize=options.normalize,rebin=rebin_e)[0])

        # now for ttbar
        hists_MuNum ['ttbar_sr']   = anaplot.GetPassEventBkgHistos('lepPt_antilep0_index'   ,'FFTool_forsyst_%s_ttt_MuNum'%(z_w) ,'ttbar',options.file,normalize=options.normalize,rebin=rebin_mu)[0]
        if True: hists_MuDen ['ttbar_sr']   = anaplot.GetPassEventBkgHistos('lepPt_lepW_index'       ,'FFTool_forsyst_ltt_MuDen','ttbar',options.file,normalize=options.normalize,rebin=rebin_mu)[0]
        if do_z: hists_MuDen ['ttbar_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepleadZ_index'   ,'FFTool_forsyst_tlt_MuDen','ttbar',options.file,normalize=options.normalize,rebin=rebin_mu)[0])
        if do_z: hists_MuDen ['ttbar_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepsubleadZ_index','FFTool_forsyst_ttl_MuDen','ttbar',options.file,normalize=options.normalize,rebin=rebin_mu)[0])

        hists_EleNum['ttbar_sr']   = anaplot.GetPassEventBkgHistos('lepPt_antilep0_index'   ,'FFTool_forsyst_%s_ttt_EleNum'%(z_w),'ttbar',options.file,normalize=options.normalize,rebin=rebin_e)[0]
        if True: hists_EleDen['ttbar_sr']   = anaplot.GetPassEventBkgHistos('lepPt_lepW_index'       ,'FFTool_forsyst_ltt_EleDen','ttbar',options.file,normalize=options.normalize,rebin=rebin_e)[0]
        if do_z: hists_EleDen['ttbar_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepleadZ_index'   ,'FFTool_forsyst_tlt_EleDen','ttbar',options.file,normalize=options.normalize,rebin=rebin_e)[0])
        if do_z: hists_EleDen['ttbar_sr'].Add(anaplot.GetPassEventBkgHistos('lepPt_lepsubleadZ_index','FFTool_forsyst_ttl_EleDen','ttbar',options.file,normalize=options.normalize,rebin=rebin_e)[0])

        hists_MuNum ['zjet_sr'].SetName('zjet_sr');hists_MuNum ['zjet_sr'].SetTitle('zjet+z#gamma SR FF');
        hists_EleNum['zjet_sr'].SetName('zjet_sr');hists_EleNum['zjet_sr'].SetTitle('zjet+z#gamma SR FF');

        if False :
            hists_MuDen ['zjet_sr'].Add(hists_MuDen ['ttbar_sr'])
            hists_EleDen['zjet_sr'].Add(hists_EleDen['ttbar_sr'])
            hists_MuNum ['zjet_sr'].Add(hists_MuNum ['ttbar_sr'])
            hists_EleNum['zjet_sr'].Add(hists_EleNum['ttbar_sr'])
            hists_MuNum ['zjet_sr'].SetTitle('zjet+z#gamma+ttbar SR FF');
            hists_EleNum['zjet_sr'].SetTitle('zjet+z#gamma+ttbar SR FF');

        hists_MuNum ['zjet_sr'].Divide(hists_MuDen ['zjet_sr'])
        hists_EleNum['zjet_sr'].Divide(hists_EleDen['zjet_sr'])

        hists_MuNum ['ttbar_sr'].SetName('ttbar_sr');hists_MuNum ['ttbar_sr'].SetTitle('ttbar SR FF');
        hists_EleNum['ttbar_sr'].SetName('ttbar_sr');hists_EleNum['ttbar_sr'].SetTitle('ttbar SR FF');
        hists_MuNum ['ttbar_sr'].Divide(hists_MuDen ['ttbar_sr'])
        hists_EleNum['ttbar_sr'].Divide(hists_EleDen['ttbar_sr'])

    print hists_MuNum.keys()

    if (options.region == 'dijet') and 'dijet' in hists_EleNum.keys() :
        hists_MuNum ['dijet'].SetLineColor(ROOT.kBlue+1); hists_MuNum ['dijet'].SetMarkerColor(ROOT.kBlue+1);
        hists_EleNum['dijet'].SetLineColor(ROOT.kBlue+1); hists_EleNum['dijet'].SetMarkerColor(ROOT.kBlue+1);

        hists_MuNum ['dijet'].Divide(hists_MuDen ['dijet'])
        hists_EleNum['dijet'].Divide(hists_EleDen['dijet'])

    if (options.region == 'zjet') and 'zjet' in hists_EleNum.keys() :

        hists_MuNum ['zjet'].SetTitle('zjet (no z#gamma) MC')
        hists_EleNum['zjet'].SetTitle('zjet (no z#gamma) MC')

        if 'zgam' in hists_EleNum.keys() :
            print '##############################################################################'
            print '## Adding zgam to your estimate'
            print '##############################################################################'
            hists_MuNum['zjet'].Add(hists_MuNum['zgam'])
            hists_EleNum['zjet'].Add(hists_EleNum['zgam'])

            hists_MuNum ['zjet'].SetTitle('zjet+z#gamma MC')
            hists_EleNum['zjet'].SetTitle('zjet+z#gamma MC')

        hists_MuNum ['zjet'].SetLineColor(ROOT.kBlue+1); hists_MuNum ['zjet'].SetMarkerColor(ROOT.kBlue+1);
        hists_EleNum['zjet'].SetLineColor(ROOT.kBlue+1); hists_EleNum['zjet'].SetMarkerColor(ROOT.kBlue+1);

        hists_MuNum ['zjet'].Divide(hists_MuDen ['zjet'])
        hists_EleNum['zjet'].Divide(hists_EleDen['zjet'])


#     cans.append(ROOT.TCanvas('%s_Mu_ff_dijet'%(v),'%s_Mu_ff'%(v),500,500))
#     plotfunc.AddHistogram(cans[-1],dijet_hist_MuNum)
#     plotfunc.FormatCanvasAxes(cans[-1])
#     taxisfunc.SetYaxisRanges(cans[-1],0,0.5)

#     cans.append(ROOT.TCanvas('%s_Ele_ff_dijet'%(v),'%s_Ele_ff'%(v),500,500))
#     plotfunc.AddHistogram(cans[-1],dijet_hist_EleNum)
#     plotfunc.FormatCanvasAxes(cans[-1])
#     taxisfunc.SetYaxisRanges(cans[-1],0,0.5)

    if 'data' in hists_EleNum.keys() :
        hists_EleNum['data'].SetTitle('With VV subtraction')
        hists_EleDen['data'].SetTitle('With VV subtraction')
        hists_MuNum ['data'].SetTitle('With VV subtraction')
        hists_MuDen ['data'].SetTitle('With VV subtraction')

        if (options.region == 'dijet') :
            hists_EleNum['data_bws'] = hists_EleNum['data'].Clone(); hists_EleNum['data_bws'].SetName(hists_EleNum['data_bws'].GetName()+'_bws')
            hists_EleDen['data_bws'] = hists_EleDen['data'].Clone(); hists_EleDen['data_bws'].SetName(hists_EleDen['data_bws'].GetName()+'_bws')
            hists_MuNum ['data_bws'] = hists_MuNum ['data'].Clone(); hists_MuNum ['data_bws'].SetName(hists_MuNum ['data_bws'].GetName()+'_bws')
            hists_MuDen ['data_bws'] = hists_MuDen ['data'].Clone(); hists_MuDen ['data_bws'].SetName(hists_MuDen ['data_bws'].GetName()+'_bws')
        
            hists_EleNum['data_bws'].SetTitle('No EW subtraction')
            hists_EleDen['data_bws'].SetTitle('No EW subtraction')
            hists_MuNum ['data_bws'].SetTitle('No EW subtraction')
            hists_MuDen ['data_bws'].SetTitle('No EW subtraction')

            hists_EleNum['data_bzs'] = hists_EleNum['data_bws'].Clone(); hists_EleNum['data_bzs'].SetName(hists_EleNum['data_bzs'].GetName()+'_bzs')
            hists_EleDen['data_bzs'] = hists_EleDen['data_bws'].Clone(); hists_EleDen['data_bzs'].SetName(hists_EleDen['data_bzs'].GetName()+'_bzs')
            hists_MuNum ['data_bzs'] = hists_MuNum ['data_bws'].Clone(); hists_MuNum ['data_bzs'].SetName(hists_MuNum ['data_bzs'].GetName()+'_bzs')
            hists_MuDen ['data_bzs'] = hists_MuDen ['data_bws'].Clone(); hists_MuDen ['data_bzs'].SetName(hists_MuDen ['data_bzs'].GetName()+'_bzs')
            
            hists_EleNum['data_bzs'].SetTitle('With W subtraction')
            hists_EleDen['data_bzs'].SetTitle('With W subtraction')
            hists_MuNum ['data_bzs'].SetTitle('With W subtraction')
            hists_MuDen ['data_bzs'].SetTitle('With W subtraction')

            for i in ['wje','wjm','wjt','zjee','zjmm','zjtt'] :
                if i in hists_EleNum.keys() :
                    print 'Subtracting',i
                    hists_EleNum['data'].Add(hists_EleNum[i],-1)
                    hists_EleDen['data'].Add(hists_EleDen[i],-1)
                    hists_MuNum ['data'].Add(hists_MuNum [i],-1)
                    hists_MuDen ['data'].Add(hists_MuDen [i],-1)
                    if i not in ['zjee','zjmm','zjtt'] :
                        hists_EleNum['data_bzs'].Add(hists_EleNum[i],-1)
                        hists_EleDen['data_bzs'].Add(hists_EleDen[i],-1)
                        hists_MuNum ['data_bzs'].Add(hists_MuNum [i],-1)
                        hists_MuDen ['data_bzs'].Add(hists_MuDen [i],-1)

            hists_MuNum ['data_bws'].Divide(hists_MuDen ['data_bws'])
            hists_EleNum['data_bws'].Divide(hists_EleDen['data_bws'])
                        
            hists_MuNum ['data_bzs'].Divide(hists_MuDen ['data_bzs'])
            hists_EleNum['data_bzs'].Divide(hists_EleDen['data_bzs'])

        if (options.region == 'zjet') :
            hists_EleNum['data_bvvs'] = hists_EleNum['data'].Clone(); hists_EleNum['data_bvvs'].SetName(hists_EleNum['data_bvvs'].GetName()+'_bvvs')
            hists_EleDen['data_bvvs'] = hists_EleDen['data'].Clone(); hists_EleDen['data_bvvs'].SetName(hists_EleDen['data_bvvs'].GetName()+'_bvvs')
            hists_MuNum ['data_bvvs'] = hists_MuNum ['data'].Clone(); hists_MuNum ['data_bvvs'].SetName(hists_MuNum ['data_bvvs'].GetName()+'_bvvs')
            hists_MuDen ['data_bvvs'] = hists_MuDen ['data'].Clone(); hists_MuDen ['data_bvvs'].SetName(hists_MuDen ['data_bvvs'].GetName()+'_bvvs')
        
            hists_EleNum['data_bvvs'].SetTitle('No VV subtraction')
            hists_EleDen['data_bvvs'].SetTitle('No VV subtraction')
            hists_MuNum ['data_bvvs'].SetTitle('No VV subtraction')
            hists_MuDen ['data_bvvs'].SetTitle('No VV subtraction')

            # 15% systematic on vv subtraction
            hists_EleNum['data_vv15sys'] = hists_EleNum['data'].Clone(); hists_EleNum['data_vv15sys'].SetName(hists_EleNum['data_vv15sys'].GetName()+'_vv15sys')
            hists_EleDen['data_vv15sys'] = hists_EleDen['data'].Clone(); hists_EleDen['data_vv15sys'].SetName(hists_EleDen['data_vv15sys'].GetName()+'_vv15sys')
            hists_MuNum ['data_vv15sys'] = hists_MuNum ['data'].Clone(); hists_MuNum ['data_vv15sys'].SetName(hists_MuNum ['data_vv15sys'].GetName()+'_vv15sys')
            hists_MuDen ['data_vv15sys'] = hists_MuDen ['data'].Clone(); hists_MuDen ['data_vv15sys'].SetName(hists_MuDen ['data_vv15sys'].GetName()+'_vv15sys')
        
            hists_EleNum['data_vv15sys'].SetTitle('15% systematic on subtraction')
            hists_EleDen['data_vv15sys'].SetTitle('15% systematic on subtraction')
            hists_MuNum ['data_vv15sys'].SetTitle('15% systematic on subtraction')
            hists_MuDen ['data_vv15sys'].SetTitle('15% systematic on subtraction')

            for i in ['wz','zz'] :
                if i in hists_EleNum.keys() :
                    print 'Subtracting',i
                    hists_EleNum['data'].Add(hists_EleNum[i],-1)
                    hists_EleDen['data'].Add(hists_EleDen[i],-1)
                    hists_MuNum ['data'].Add(hists_MuNum [i],-1)
                    hists_MuDen ['data'].Add(hists_MuDen [i],-1)

                    hists_EleNum['data_vv15sys'].Add(hists_EleNum[i],-1+0.15)
                    hists_EleDen['data_vv15sys'].Add(hists_EleDen[i],-1+0.15)
                    hists_MuNum ['data_vv15sys'].Add(hists_MuNum [i],-1+0.15)
                    hists_MuDen ['data_vv15sys'].Add(hists_MuDen [i],-1+0.15)

            hists_MuNum ['data_bvvs'].Divide(hists_MuDen ['data_bvvs'])
            hists_EleNum['data_bvvs'].Divide(hists_EleDen['data_bvvs'])

            hists_MuNum ['data_vv15sys'].Divide(hists_MuDen ['data_vv15sys'])
            hists_EleNum['data_vv15sys'].Divide(hists_EleDen['data_vv15sys'])

        hists_MuNum ['data'].Divide(hists_MuDen ['data'])
        hists_EleNum['data'].Divide(hists_EleDen['data'])

    f = ROOT.TFile('FakeFactors.root','recreate')
    f.cd()
    if 'data'in hists_EleNum.keys() :
        if not options.e  : hists_MuNum ['data'].Write("Muons")
        if not options.mu : hists_EleNum['data'].Write("Electrons")
        if not options.e  : hists_MuNum ['data_vv15sys'].Write("Muons_wzsyst")
        if not options.mu : hists_EleNum['data_vv15sys'].Write("Electrons_wzsyst")
    f.Close()

    g = ROOT.TFile('FakeFactors_mcClosure.root','recreate')
    g.cd()
    if (options.region == 'zjet') :
        if not options.e  : hists_MuNum['zjet'].Write('Muons')
        if not options.mu : hists_EleNum['zjet'].Write('Electrons')
    if (options.region == 'dijet') :
        if not options.e  : hists_MuNum['dijet'].Write('Muons')
        if not options.mu : hists_EleNum['dijet'].Write('Electrons')
    g.Close()
    
    if 'zjet_sr' in hists_MuNum.keys() : 
        cans.append(plotfunc.RatioCanvas('lepPt_Mu_ff_systematic','%s_Mu_ff_systematic'%(v),500,500))
    else :
        cans.append(ROOT.TCanvas('lepPt_Mu_ff','%s_Mu_ff'%(v),500,500))    

    if 'data'in hists_EleNum.keys() : 

        if (options.region == 'dijet') :
            plotfunc.AddHistogram(cans[-1],hists_MuNum['data_bws'])
            plotfunc.AddHistogram(cans[-1],hists_MuNum['data_bzs'])
            if 'dijet' in hists_EleNum.keys() :
                plotfunc.AddHistogram(cans[-1],hists_MuNum['dijet'])

        if (options.region == 'zjet') :
            if not options.syst : plotfunc.AddHistogram(cans[-1],hists_MuNum['data_bvvs'])
            #plotfunc.AddHistogram(cans[-1],hists_MuNum['data_vv15sys'])
            pass
        if not options.syst : plotfunc.AddHistogram(cans[-1],hists_MuNum['data'])

    from array import array
    band = ROOT.TH1F('band','band',5,array('d',[0,15,20,30,50,100]))
    def SetSysts(h_band,systs) :
        for i,sys in enumerate(systs) :
            h_band.SetBinContent(i+1,1)
            h_band.SetBinError(i+1,sys)
    band.SetFillColor(ROOT.kOrange-0)
    band.SetMarkerSize(0)
    band.SetMarkerColor(ROOT.kOrange-0)

    if (options.region == 'zjet') :
        if 'zjet' in hists_MuNum.keys() :
            plotfunc.AddHistogram(cans[-1],hists_MuNum['zjet'])
            if 'zjet_sr' in hists_MuNum.keys() : 
                if z_w == 'zid' :
                    SetSysts(band,[0.0,0.3,0.2,0.2,0.2])
                else :
                    SetSysts(band,[0.0,0.0,0.2,0.25,0.25])
                plotfunc.AddHistogram(cans[-1].GetPrimitive('pad_bot'),band,'E2')
                plotfunc.AddRatio(cans[-1],hists_MuNum['zjet_sr'],hists_MuNum['zjet'])
                #plotfunc.AddRatio(cans[-1],hists_MuNum['ttbar_sr'],hists_MuNum['zjet'])

    if not plotfunc.CanvasEmpty(cans[-1]) :
        plotfunc.FormatCanvasAxes(cans[-1])
        #taxisfunc.SetYaxisRanges(cans[-1],0,0.5)
        plotfunc.MakeLegend(cans[-1])
        additionaltext = ('supporting triggers' if options.region == 'dijet' else None)
        plotfunc.FullFormatCanvasDefault(cans[-1],sqrts=13,lumi=options.fb,additionaltext=additionaltext)
        if 'zjet_sr' in hists_MuNum.keys() : 
           taxisfunc.SetYaxisRanges(cans[-1].GetPrimitive('pad_bot'),-1,3)
    plotfunc.SetAxisLabels(cans[-1],'p_{T} [GeV]','fake factor')

    if 'zjet_sr' in hists_EleNum.keys() : 
        cans.append(plotfunc.RatioCanvas('lepPt_Ele_ff_systematic','%s_Ele_ff_systematic'%(v),500,500))
    else :
        cans.append(ROOT.TCanvas('lepPt_Ele_ff','%s_Ele_ff'%(v),500,500))

    if 'data'in hists_EleNum.keys() : 

        if (options.region == 'dijet') :
            plotfunc.AddHistogram(cans[-1],hists_EleNum['data_bws'])
            plotfunc.AddHistogram(cans[-1],hists_EleNum['data_bzs'])
            if 'dijet' in hists_EleNum.keys() :
                plotfunc.AddHistogram(cans[-1],hists_EleNum['dijet'])

        if (options.region == 'zjet') :
            if not options.syst : plotfunc.AddHistogram(cans[-1],hists_EleNum['data_bvvs'])
            #plotfunc.AddHistogram(cans[-1],hists_EleNum['data_vv15sys'])

        if not options.syst : plotfunc.AddHistogram(cans[-1],hists_EleNum['data'])

    if (options.region == 'zjet') :
        if 'zjet' in hists_EleNum.keys() :
            plotfunc.AddHistogram(cans[-1],hists_EleNum['zjet'])
            if 'zjet_sr' in hists_EleNum.keys() : 
                if z_w == 'zid' :
                    SetSysts(band,[0.0,0.6,0.35,0.35,0.50])
                else :
                    SetSysts(band,[0.0,0.0,0.3,0.3,0.8])
                plotfunc.AddHistogram(cans[-1].GetPrimitive('pad_bot'),band,'E2')
                plotfunc.AddRatio(cans[-1],hists_EleNum['zjet_sr'],hists_EleNum['zjet'])
                #plotfunc.AddRatio(cans[-1],hists_EleNum['ttbar_sr'],hists_EleNum['zjet'])

    if not plotfunc.CanvasEmpty(cans[-1]) :
        plotfunc.FormatCanvasAxes(cans[-1])
        #taxisfunc.SetYaxisRanges(cans[-1],0,0.5)
        plotfunc.MakeLegend(cans[-1])
        additionaltext = ('supporting triggers' if options.region == 'dijet' else None)
        plotfunc.FullFormatCanvasDefault(cans[-1],sqrts=13,lumi=options.fb,additionaltext=additionaltext)
        if 'zjet_sr' in hists_EleNum.keys() : 
           taxisfunc.SetYaxisRanges(cans[-1].GetPrimitive('pad_bot'),-1,3)
    plotfunc.SetAxisLabels(cans[-1],'p_{T} [GeV]','fake factor')

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
    p.p.add_option('--region','-r',type='string',default='zjet',dest='region',help='Region (dijet,zjet)')
    p.p.add_option('--systematics'  ,action='store_true',default=False  ,dest='syst',help='Run all systematic variations' )
    p.p.add_option('--e'  ,action='store_true',default=False  ,dest='e',help='Plot electrons only' )
    p.p.add_option('--mu'  ,action='store_true',default=False  ,dest='mu',help='Plot muons only' )
    options,args = p.parse_args()
    options.key = 'FFTool'
    main(options,args)

