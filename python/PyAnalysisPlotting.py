#-------------------------------------------------------------------------
def LoadRootCore() :
    import ROOT
    if 'RootCore' not in ROOT.gSystem.GetLibraries() :
        print 'Loading c++...'
        ROOT.gROOT.ProcessLine (".x $ROOTCOREDIR/scripts/load_packages.C")
        # Some weird bug where functions are not accessible until a class is called
        ROOT.PSL.EDM
    return

#-------------------------------------------------------------------------
def PrepareBkgHistosForStack(bkg_hists) :
    from pennSoftLepton.PyHelpers import GetHWWColors
    colors_dict = GetHWWColors()
    for i in bkg_hists :
        i.SetLineColor(1)
        i.SetMarkerColor(colors_dict.get(i.GetTitle(),1))
        if i.GetMarkerColor() == 0 :
            i.SetMarkerColor(1)
        i.SetFillColor(colors_dict.get(i.GetTitle(),1))
        i.SetLineWidth(1)
    return

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
    
    #
    # Clean up name
    #
    canname = name.replace('[','_').replace(']','').replace('_index','')
    canname = canname.replace('[','_').replace(']','_').replace('(','_').replace(')','_').replace('/','_over_')
    canname = canname.replace('>','gt').replace('<','lt').replace('-','minus').replace(' ','_').replace('&&','and')
    canname = canname.lstrip('_').rstrip('_')

    #
    # stack, before adding SUSY histograms
    #
    if not ratio :
        can = ROOT.TCanvas(canname,canname,500,500)
        if log : can.SetLogy()
    else :
        can = plotfunc.RatioCanvas(canname,canname,500,500)
        if log : can.GetPrimitive('pad_top').SetLogy()

    if bkg_hists :
        totb = bkg_hists[0].Clone()
        totb.SetNameTitle('SM_%s'%(variable),'remove me')
        totb.SetLineColor(1)
        totb.SetLineWidth(1)
        totb.SetMarkerSize(0)
        totb.SetFillColor(0)
        for i in bkg_hists[1:] :
            totb.Add(i)
        totberror = totb.Clone()
        totberror.SetName(totb.GetName()+'_error')
        totberror.SetTitle('SM (stat)')
        totberror.SetFillColor(12)
        totberror.SetFillStyle(3254)

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
        plotfunc.AddHistogram(can,totb,drawopt='hist')

    for h in sig_hists :
        plotfunc.AddHistogram(can,h)

    if data_hist :
        if ratio :
            plotfunc.AddRatio(can,data_hist,totb)
        else :
            plotfunc.AddHistogram(can,data_hist)

    plotfunc.FormatCanvasAxes(can)
    text_lines = [plotfunc.GetSqrtsText(13)]
    if fb > 0 :
        text_lines += [plotfunc.GetLuminosityText(fb)]
    text_lines += [plotfunc.GetAtlasInternalText()]
    if ratio :
        plotfunc.DrawText(can,text_lines,0.2,0.65,0.5,0.90,totalentries=4)
        plotfunc.MakeLegend(can,0.53,0.65,0.92,0.90,totalentries=5,ncolumns=2,skip=['remove me'])
        taxisfunc.SetYaxisRanges(plotfunc.GetBotPad(can),0,2)
    else :
        plotfunc.DrawText(can,text_lines,0.2,0.75,0.5,0.94,totalentries=4)
        plotfunc.MakeLegend(can,0.53,0.75,0.94,0.94,totalentries=5,ncolumns=2,skip=['remove me'])
    plotfunc.SetAxisLabels(can,xlabel,'entries')
    plotfunc.AutoFixAxes(can)
    return can

#-------------------------------------------------------------------------
def GetNtupleCutflowHistograms(file) :
    hists = []
    for k in file.GetListOfKeys() :
        name = k.GetName()
        if 'PassEvent' in name and 'PassW' in name :
            hists.append(k.ReadObj())
    return hists

#-------------------------------------------------------------------------
def GetTreesFromFiles(filelist_csv) :
    import ROOT,os
    LoadRootCore()

    cutflow_hists = []
    files = []
    trees = dict()
    keys = []
    for f in filelist_csv.split(',') :
        if not f : continue
        cutflow_hists.append([])
        name = f.replace('.root','').replace('/','_').replace('-','_').replace('.','_')
        is_samplename = (f.replace('.root','') in list(ROOT.PSL.GetAllSampleStrings()))
        #
        # For backgrounds, try to chain together sub-files (if the filename does not exist)
        #
        if is_samplename and (not os.path.isfile(f)) :
            files.append([])
            f_expanded = ROOT.PSL.ConvertSampleToSampleVec(f.replace('.root',''))
            trees[name] = ROOT.TChain('physics')
            subsamples = []
            for ff in f_expanded :
                if os.path.isfile(ff+'.root') :
                    files[-1].append(ROOT.TFile(ff+'.root'))
                    if files[-1][-1].IsZombie() :
                        print 'exiting'
                        import sys
                        sys.exit()
                    trees[name].Add(ff+'.root')
                    subsamples.append(ff)
                    #
                    # get cutflow hists
                    #
                    hists = GetNtupleCutflowHistograms(files[-1][-1])
                    if not cutflow_hists[-1] :
                        for h in hists :
                            cutflow_hists[-1].append(h.Clone())
                    else :
                        for i in range(len(hists)) :
                            cutflow_hists[-1][i].Add(hists[i])
                else :
                    print 'Warning: sub-sample %s of %s is missing.'%(ff,f.replace('.root',''))
            if trees[name].GetEntries() :
                print 'The sample %s will be composed of %s.'%(name,','.join(subsamples))
                keys.append(name)
            continue

        #
        # regular files
        # 
        files.append(ROOT.TFile(f))
        if files[-1].IsZombie() :
            print 'exiting'
            import sys
            sys.exit()
        for h in GetNtupleCutflowHistograms(files[-1]) :
            cutflow_hists[-1].append(h)
        keys.append(name)
        trees[name] = files[-1].Get('physics')
    
    return files,trees,keys,cutflow_hists

#-------------------------------------------------------------------------
def GetVariableHistsFromTrees(trees,keys,variable,weight,n,low,high,normalize=False,rebin=[],scale=0) :
    import ROOT
    from array import array
    import PlotFunctions as plotfunc
    import math

    def formatfloat(a) :
        a = str(a).rstrip('0') if '.' in str(a) else a
        return a

    n,low,high = formatfloat(n),formatfloat(low),formatfloat(high)
        
    hists = []
    for k in keys :
        name = '%s_%s'%(variable,k)
        name = name.replace('[','_').replace(']','_').replace('(','_').replace(')','_').replace('/','_over_')
        name = name.replace('>','gt').replace('<','lt').replace('-','minus').replace(' ','_').replace('&&','and')
        name = name.lstrip('_')
        print name

        while ROOT.gDirectory.Get(name) :
            #print 'changing name'
            name = name+'x'
        if rebin and type(rebin) == type([]) :
            name = name+'_unrebinned'
        arg1,arg2,arg3 = '%s>>%s(%s,%s,%s)'%(variable,name,n,low,high),weight,'egoff'
        #arg1,arg2,arg3 = '%s>>%s'%(variable,name),weight,'egoff'
        print 'tree.Draw(\'%s\',\'%s\',\'%s\')'%(arg1,arg2,arg3)
        trees[k].Draw(arg1,arg2,arg3)

        if rebin and type(rebin) == type([]) :
            tmp = ROOT.gDirectory.Get(name)
            name = name.replace('_unrebinned','')
            tmp.Rebin(len(rebin)-1,name,array('d',rebin))
                
        hists.append(ROOT.gDirectory.Get(name))
        if rebin and type(rebin) == type(1) :
            hists[-1].Rebin(rebin)

        hists[-1].SetTitle(k)
        ROOT.PSL.SetBinLabels(variable.split('[')[0],hists[-1])
        pm = u"\u00B1"
        # print the yield and error after cuts (includes overflow)
        print '%2.2f %s %2.2f'%(hists[-1].Integral(0,hists[-1].GetNbinsX()+1),pm,math.sqrt(sum(list(hists[-1].GetSumw2()))))

        if scale and (scale != 1) :
            hists[-1].Scale(scale)
        if normalize :
            hists[-1].Scale(1/float(hists[-1].Integral()))
        if rebin :
            plotfunc.ConvertToDifferential(hists[-1])

    return hists

#-------------------------------------------------------------------------
def Get2dVariableHistsFromTrees(trees,keys,variable1,variable2,weight,n1,low1,high1,n2,low2,high2
                                ,normalize=False,rebin1=[],rebin2=[],scale=0) :
    import ROOT
    from array import array
    import PlotFunctions as plotfunc
    import math

    def formatfloat(a) :
        a = str(a).rstrip('0') if '.' in str(a) else a
        return a

    n1,low1,high1 = formatfloat(n1),formatfloat(low1),formatfloat(high1)
    n2,low2,high2 = formatfloat(n2),formatfloat(low2),formatfloat(high2)
        
    hists = []
    for k in keys :
        name = '%s_%s_%s'%(variable1,variable2,k)
        name = name.replace('[','_').replace(']','_').replace('(','_').replace(')','_').replace('/','_over_')
        while ROOT.gDirectory.Get(name) :
            #print 'changing name'
            name = name+'x'
#         if rebin1 and type(rebin) == type([]) :
#             name = name+'_unrebinned'
        arg1,arg2,arg3 = '%s:%s>>%s(%s,%s,%s,%s,%s,%s)'%(variable2,variable1,name,n1,low1,high1,n2,low2,high2),weight,'egoff'
        print 'tree.Draw(\'%s\',\'%s\',\'%s\')'%(arg1,arg2,arg3)
        trees[k].Draw(arg1,arg2,arg3)

#         if rebin and type(rebin) == type([]) :
#             tmp = ROOT.gDirectory.Get(name)
#             name = name.replace('_unrebinned','')
#             tmp.Rebin(len(rebin)-1,name,array('d',rebin))
                
        hists.append(ROOT.gDirectory.Get(name))
#         if rebin and type(rebin) == type(1) :
#             hists[-1].Rebin(rebin)

        hists[-1].SetTitle(k)
        ROOT.PSL.SetBinLabels(variable1.split('[')[0],variable2.split('[')[0],hists[-1])
        pm = u"\u00B1"
        # print the yield and error after cuts (includes overflow)
        print '%2.2f %s %2.2f'%(hists[-1].Integral(0,hists[-1].GetNbinsX()+1,0,hists[-1].GetNbinsY()+1),pm,math.sqrt(sum(list(hists[-1].GetSumw2()))))

        hists[-1].SetMinimum(-0.00001)

        if scale and (scale != 1) :
            hists[-1].Scale(scale)
        if normalize :
            hists[-1].Scale(1/float(hists[-1].Integral()))
#         if rebin :
#             plotfunc.ConvertToDifferential(hists[-1])

    return hists

#-------------------------------------------------------------------------
class TreePlottingOptParser :
    def __init__(self) :
        from optparse import OptionParser
        self.p = OptionParser()
        self.p.add_option('--batch',action='store_true',default=False,dest='batch',help='run in batch mode')
        self.p.add_option('--bkgs',type='string',default='',dest='bkgs',help='input files for bkg (csv)')
        self.p.add_option('--susy',type='string',default='',dest='susy',help='input files for susy (csv)')
        self.p.add_option('--data',type='string',default='',dest='data',help='input file for data (csv)')
        self.p.add_option('-v','--variables',type='string',default='',dest='variables',help='Variables (see Variables.cxx for names)')
        self.p.add_option('-l','--log',action='store_true',default=False,dest='log',help='log')
        self.p.add_option('--nostack',action='store_true',default=False,dest='nostack',help='do not stack')
        self.p.add_option('--normalize',action='store_true',default=False,dest='normalize',help='normalize')
        self.p.add_option('--save',action='store_true',default=False,dest='save',help='save cans to pdf')
        self.p.add_option('--ratio',action='store_true',default=False,dest='ratio',help='Plot as a ratio')
        self.p.add_option('-c','--cuts',type='string',default='',dest='cuts',help='cut string')
        self.p.add_option('--fb',type='float',default=-1,dest='fb',help='int luminosity (fb)')
        self.p.add_option('--config',type='string',default='',dest='config',help='Input configuration file (python module)')

    def parse_args(self) :
        import sys,os
        import ROOT
        import importlib
        LoadRootCore()

        self.options,self.args = self.p.parse_args()

        if self.options.batch :
            ROOT.gROOT.SetBatch(True)
        else :
            ROOT.gROOT.SetBatch(False)

        self.options.stack = not self.options.nostack
        
        if self.options.susy and not '.root' in self.options.susy :
            dir = self.options.susy
            self.options.susy = ','.join('%s/%s'%(dir,a) for a in os.listdir(self.options.susy))
        print self.options.susy

        self.options.bkgs = self.options.bkgs.split(',')
        for b in range(len(self.options.bkgs)) :
            if not self.options.bkgs[b] :
                continue
            if '.root' not in self.options.bkgs[b] :
                self.options.bkgs[b] = self.options.bkgs[b]+'.root'
        self.options.bkgs = ','.join(self.options.bkgs)
        print self.options.bkgs

        # to get your current directory viewable by the code:
        sys.path.append(os.getcwd())
        self.options.histformat = dict()
        self.options.usermodule = None
        if self.options.config :
            usermodule = importlib.import_module(self.options.config.replace('.py',''))
            self.options.usermodule = usermodule
            if hasattr(usermodule,'cuts') :
                self.options.cuts = usermodule.cuts
                for i,c in enumerate(self.options.cuts) :
                    self.options.cuts[i] = '('+c+')'
            if hasattr(usermodule,'variables') :
                self.options.variables = ','.join(usermodule.variables)
            if hasattr(usermodule,'histformat') :
                self.options.histformat = usermodule.histformat

        if self.options.variables.lower() == 'all' :
            self.options.variables = ','.join(ROOT.PSL.GetAllVariableStrings())

        for v in self.options.variables.split(',') :
            if v == '' : continue
            vtmp = v
            if '[' in vtmp :
                vtmp = vtmp.split('[')[0]
            if v in self.options.histformat.keys() :
                if len(self.options.histformat[v]) < 4 :
                    self.options.histformat[v].append(ROOT.PSL.GetXaxisLabel(vtmp))
                continue
            label = ROOT.PSL.GetXaxisLabel(vtmp)
            n,xdn,xup = ROOT.PSL.GetVariableHistArgs(vtmp)
            self.options.histformat[v] = [n,xdn,xup,label]

        # scripts will be looking for a python list of cuts
        if type(self.options.cuts) == type('') :
            self.options.cuts = [self.options.cuts]

        return self.options,self.args

#-------------------------------------------------------------------------
class PassEventPlottingOptParser :
    def __init__(self) :
        from optparse import OptionParser
        self.p = OptionParser()
        self.p.add_option('-f','--file',type='string',default='',dest='file',help='input file')
        self.p.add_option('-s','--susy',type='string',default='',dest='susy',help='input files for susy (csv)')
        self.p.add_option('-k','--key' ,type='string',default='',dest='key'  ,help='PassEvent name (key)')
        self.p.add_option('-v','--variables',type='string',default='',dest='variables',help='Variables (see Variables.cxx for names)')
        self.p.add_option('-p','--processes',type='string',default='',dest='processes',help='Processes (see Sample.cxx for names)')
        self.p.add_option('-l','--log',action='store_true',default=False,dest='log',help='log')
        self.p.add_option('--nostack',action='store_true',default=False,dest='nostack',help='do not stack')
        self.p.add_option('--normalize',action='store_true',default=False,dest='normalize',help='normalize')
        self.p.add_option('--ratio',action='store_true',default=False,dest='ratio',help='Plot as a ratio')
        # config understands histformat, variables
        self.p.add_option('--config',type='string',default='',dest='config',help='Input configuration file (python module)')
        self.p.add_option('--fb',type='float',default=10,dest='fb',help='int luminosity (fb)')
        self.p.add_option('--save',action='store_true',default=False,dest='save',help='save cans to pdf')

    def parse_args(self) :
        import sys,os
        import importlib
        import ROOT
        LoadRootCore()
        
        self.options,self.args = self.p.parse_args()
        print self.options.susy
        if self.options.susy and not '.root' in self.options.susy :
            dir = self.options.susy
            self.options.susy = ','.join('%s/%s'%(dir,a) for a in os.listdir(self.options.susy))
        print self.options.susy

        self.options.stack = not self.options.nostack

        sys.path.append(os.getcwd())
        self.options.histformat = dict()
        self.options.usermodule = None
        if self.options.config :
            usermodule = importlib.import_module(self.options.config.replace('.py',''))
            self.options.usermodule = usermodule
            if hasattr(usermodule,'variables') :
                self.options.variables = ','.join(usermodule.variables)
            if hasattr(usermodule,'histformat') :
                self.options.histformat = usermodule.histformat

        for v in self.options.variables.split(',') :
            if v == '' : continue
            vtmp = v
            if '[' in vtmp :
                vtmp = vtmp.split('[')[0]
            if v in self.options.histformat.keys() :
                if len(self.options.histformat[v]) < 4 :
                    self.options.histformat[v].append(ROOT.PSL.GetXaxisLabel(vtmp))
                continue
            label = ROOT.PSL.GetXaxisLabel(vtmp)
            n,xdn,xup = ROOT.PSL.GetVariableHistArgs(vtmp)
            self.options.histformat[v] = [n,xdn,xup,label]

        return self.options,self.args
    
#-------------------------------------------------------------------------
#
# This takes a single file, which is the hadded result of all PassEvent sub-jobs.
#
def GetPassEventBkgHistos(variable,key,processes,filename,normalize=False,rebin=[]) :
    import ROOT
    import pennSoftLepton.PyHelpers as pyhelpers
    import PlotFunctions as plotfunc
    from array import array
    import math

    hists = []
    
    the_file = ROOT.TFile(filename)
    if the_file.IsZombie() :
        print 'exiting'
        import sys
        sys.exit()

    #
    # Loop over processes
    #
    print '## %s, %s'%(key,variable)
    for s in processes.split(',') :
        if not s : continue
        variable_label = variable
        if '[' in variable :
            variable_label = variable.split('[')[0]
            variable = variable.replace('[','_').rstrip(']')
        name = 'PassEvent_%s/PassEvent_%s_%s_%s'%(key,key,s,variable)
        h = pyhelpers.GetRootObj(the_file,name,fatal=True).Clone()
        if not h :
            print '%s does not exist'%(name)
        else :
            ROOT.PSL.SetBinLabels(variable,h)
            h.GetXaxis().SetRange(0,h.GetNbinsX())
            if rebin :
                if type(rebin) == type([]) :
                    name = h.GetName()+'_rebinned'
                    h.Rebin(len(rebin)-1,name,array('d',rebin))
                    h = ROOT.gDirectory.Get(name)
                else :
                    h.Rebin(rebin)

            # pm = u"\u00B1"
            # print '%s: %2.2f %s %2.2f'%(s,h.Integral(0,h.GetNbinsX()+1),pm,math.sqrt(sum(list(h.GetSumw2()))))

            if normalize :
                h.Scale(1/float(h.Integral()))
            if rebin and type(rebin) == type([]) :
                plotfunc.ConvertToDifferential(h)
                
            h.SetTitle(s)
            h.SetLineWidth(2)
            h.SetLineColor(1)
            h.SetDirectory(0)
            hists.append(h)

    the_file.Close()
    return hists


#-------------------------------------------------------------------------
#
# This takes a list of susy files
#
def GetPassEventSigHistos(variable,key,filenames,normalize=False,rebin=[]) :
    import ROOT
    import pennSoftLepton.PyHelpers as pyhelpers    
    from array import array

    #
    # Loop over SUSY files
    #
    hists = []
    nsignal = 0
    files = []
    for f in filenames.split(',') :
        file = ROOT.TFile(f)
        if file.IsZombie() :
            print 'exiting'
            import sys
            sys.exit()

        #
        # get SUSY title
        #
        mass_map = GetMassesMap()
        #print mass_map
        title = 'susy'
        for i in file.GetListOfKeys() :
            try :
                ii = int(i.GetName())
            except ValueError :
                continue
            if ii in mass_map.keys() :
                title = '%s,%s'%(mass_map[ii]['mc1'],mass_map[ii]['mn1'])

        #
        # Get SUSY histogram
        #
        name = 'PassEvent_%s/PassEvent_%s_%s_%s'%(options.key,options.key,'susy',variable)
        h = GetRootObj(file,name,fatal=True).Clone()
        if not h :
            print '%s does not exist'%(name)
        else :
            ROOT.PSL.SetBinLabels(variable,h)
            if rebin :
                if type(rebin) == type([]) :
                    name = h.GetName()+'_rebinned'
                    h.Rebin(len(rebin)-1,name,array('d',rebin))
                    h = ROOT.gDirectory.Get(name)
                else :
                    h.Rebin(rebin)
            h.SetTitle(title)
            h.SetLineWidth(2)
            h.SetMarkerColor(signal_colors[nsignal])
            h.SetLineColor(signal_colors[nsignal])
            nsignal += 1
            h.SetDirectory(0)
            hists.append(h)

        file.Close()

    return hists

