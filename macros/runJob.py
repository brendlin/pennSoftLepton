#!/bin/usr/env python

import ROOT
import os,sys
import pennSoftLepton.PyHelpers as pyhelpers

#-------------------------------------------------------------------------
def main (options,args) :
    #
    # output
    #
    outputname = pyhelpers.getOutputName(options.out)
    #
    # Load c++
    #
    pyhelpers.LoadRootCore()

    #
    # Configuration
    #
    confname    = '%s/../pennSoftLepton/config/%s'%(os.getenv('ROOTCOREBIN'),options.config)
    if not os.path.exists(confname) :
        confname = options.config
    defaultconf = '%s/../pennSoftLepton/config/%s'%(os.getenv('ROOTCOREBIN'),options.dconfig)
    if not os.path.exists(defaultconf) :
        defaultconf = options.dconfig
    if not os.path.exists(defaultconf) :
        print 'Cannot find default configuration file, %s. Exiting.'%(defaultconf)
        return
    if not os.path.exists(confname) : 
       print 'Cannot find configuration file, %s. Exiting.'%(confname)
       return
    env = ROOT.TEnv(confname)        
    print 'Using conf name %s'%(confname)
    defaults = ROOT.TEnv(defaultconf)
    print 'Using default conf %s'%(defaultconf)
    for i in defaults.GetTable() :
        if not env.Defined(i.GetName()) :
            env.SetValue(i.GetName(),i.GetValue())
        #env.PrintEnv()

    myjob = ROOT.EL.Job()
    edm_alg = ROOT.PSL.EDM()
    pyhelpers.ConfigureCxxAlg(edm_alg,env,class_name='EDM')
    if options.skim :
        edm_alg.set_allowUnlabeledSamples(True);

    options.xaod = (edm_alg.CompiledEDMType() == 'xAOD')
    if options.xaod :
        myjob.useXAOD()

    #
    # Only add algorithms AFTER you call useXAOD. Otherwise you will cause a very hard-to-find bug!
    #
    myjob.algsAdd(edm_alg)

    options.d3pd = not options.xaod

    susytools_alg = 0
    if options.xaod :
        # check out https://svnweb.cern.ch/trac/atlasoff/browser/Control/xAODRootAccess
        ROOT.xAOD.TReturnCode.enableFailure()
        ROOT.xAOD.Init().isSuccess() # other options: isSuccess, isFailure, isRecoverable
        # branch access is a requirement for the trigger tools.
        myjob.options().setString (ROOT.EL.Job.optXaodAccessMode, ROOT.EL.Job.optXaodAccessMode_branch)
        #myjob.options().setString (ROOT.EL.Job.optXaodAccessMode, ROOT.EL.Job.optXaodAccessMode_class)
        if options.truth:
            susytools_alg = ROOT.PSL.TruthObjDef()
            susytools_alg.SetName('truth_alg')
            pyhelpers.ConfigureCxxAlg(susytools_alg,env,class_name='TruthObjDef')
            myjob.algsAdd(susytools_alg)
        else :
            if options.susy2016 :
                print '##########################################'
                print '# Using SUSYTools Object Selection, via PSL::XSUSYCutflow'
                print '##########################################'
                susytools_alg = ROOT.PSL.XSUSYCutflow()
            elif options.susy :
                print '##########################################'
                print '# Using SUSYTools Object Selection, via PSL::XSUSYObjDefAlgV7'
                print '##########################################'
                susytools_alg = ROOT.PSL.XSUSYObjDefAlgV7()
            else :
                print '##########################################'
                print '# Using SMWZ Object Selection, via PSL::XSUSYObjDefAlgPlusPlus'
                print '##########################################'
                susytools_alg = ROOT.PSL.XSUSYObjDefAlgPlusPlus()
            susytools_alg.SetName('susytools_alg')
            susytools_alg.set_ConfigFile(options.config.split('/')[-1])
            pyhelpers.ConfigureCxxAlg(susytools_alg,env,class_name='SUSYObjDefAlg')
            if not susytools_alg.CheckIsolationConfigurationValidity() :
                sys.exit()
            myjob.algsAdd(susytools_alg)

    if options.d3pd :
        toolsd3pd_alg = ROOT.PSL.ToolsD3PDEra()
        myjob.algsAdd(toolsd3pd_alg)
        susytools_alg = ROOT.PSL.SUSYObjDefAlg()
        susytools_alg.SetName('susytools_alg')
        pyhelpers.ConfigureCxxAlg(susytools_alg,env)
        myjob.algsAdd(susytools_alg)

    #
    # Any conceivable algorithm
    #
    passevent_alg = ROOT.PSL.PassEvent()
    passevent_alg.SetName('Top_PassEvent_Alg')
    passevent_alg.set_Variables('GRL')
    passevent_alg.set_LepVariables('lepPt')
    myjob.algsAdd(passevent_alg);

    all_cuts = pyhelpers.MakeCutsDictionaryFromTEnv(env)
    start_cut = ROOT.PSL.CutItem()
    start_cut.SetName('Start') # for the starting point of PassEvent algorithms
    start_cut.set_doAlwaysPass(True)
    all_cuts['Start'] = start_cut

    #
    # Fake factor tool (if any) should be run after event selector but before PassEvent classes
    #
    if env.GetValue('FakeFactorTool.Instance','') :
        FakeFactorTool = ROOT.PSL.FakeFactorTool()
        FakeFactorTool.SetName(env.GetValue('FakeFactorTool.Instance',''))
        pyhelpers.ConfigureCxxAlg(FakeFactorTool,env)
        myjob.algsAdd(FakeFactorTool)

    PassEventClasses = []
    for inst in env.GetValue('PassEvent.Instances','').split() :
        # inst is the instance "sr" or "top" or whatever
        PassEventClasses += pyhelpers.ConfigurePassEvent(inst,env) 
    for i in PassEventClasses :
        #
        # Add the cuts that the PassEvent classes need
        #
        for cut in i.Cuts :
            if myjob.algsHas(cut) : continue
            if cut not in all_cuts.keys() :
                print 'Error! Could not find alg %s, requested by %s. Exiting.'%(cut,i.GetName())
                import sys
                sys.exit()
            # print 'Cut %s requested by %s'%(cut,i.GetName())
            pyhelpers.AddCutsToJob(myjob,cut,all_cuts)
        myjob.algsAdd(i)

    if options.skim and options.d3pd :
        options.out = 'SKIM_%s'%(options.out)
        skim_alg = ROOT.PSL.SlimmerD3PD()
        skim_alg.Set_treename(options.treename)
        myjob.algsAdd(skim_alg)

    if options.ntuple and options.xaod :
        options.out = 'NTUPLE_%s'%(options.out)
        ntuple_alg = ROOT.PSL.Ntupler()
        myjob.algsAdd(ntuple_alg)
        pyhelpers.ConfigureCxxAlg(ntuple_alg,env)
        ntuple_alg.GetAlgList(myjob)

    #
    # End all algorithms setup
    #

    #
    # Read dataset info (setting up EDM)
    #
    testArea = os.environ.get('ROOTCOREBIN').rstrip('/')+'/..'
    DsInfoPath = testArea+'/pennSoftLepton/config/%sSamples.txt'%('' if options.xaod else '')
    DsInfoFromTxtFile = pyhelpers.ReadDatasetInfo(DsInfoPath, env)
    for run_info_dict in DsInfoFromTxtFile :
        #
        # Need this to make sure that running with the -i option works (for debugging and stuff)
        #
        #print run_info_dict['RunNumber'],run_info_dict['sample']
        #print 'Adding run to sample list:',
        #print 'sample %s run %d'%(run_info_dict['sample'],run_info_dict['RunNumber'])
        edm_alg.ConfigureRun(run_info_dict['RunNumber'],run_info_dict['sample'],1,1) # sumw and xsec =1

    myhandler = ROOT.SH.SampleHandler()

    #
    # All configuration should end here. Grid submission splits off here.
    #
    if options.grid :
        pyhelpers.sendToGrid(myjob,myhandler,options)
        return

    #
    # Input by samples
    #
    if options.samples :
        pennsrm = '%s/../pennSoftLepton/config/MapSRM_%s.txt'%(os.getenv('ROOTCOREBIN'),options.tag)
        pyhelpers.GetFilesFromSamples(ROOT.SH,myhandler,DsInfoFromTxtFile
                                      ,pennsrm,options.samples,edm_alg)

        if 'data' in options.samples :
            pennsrm_data = '%s/../pennSoftLepton/config/MapSRM_data_%s.txt'%(os.getenv('ROOTCOREBIN')
                                                                            ,options.tag)
            pyhelpers.GetDataFiles(ROOT.SH,myhandler,pennsrm_data,edm_alg)

        edm_alg.PrintRunConfiguration()

    elif options.input :
        #filepath = '/afs/cern.ch/atlas/project/PAT/tutorial/triumf-sep2014/r5591'
        #filepath = '/afs/cern.ch/work/b/brendlin/xAOD_v3/' # AOD.01572495._005585.pool.root.2
        #filepath = '/afs/cern.ch/work/b/brendlin/xAOD/' # AOD.01572495._005585.pool.root.2
        ROOT.SH.scanDir(myhandler,options.input)
        pyhelpers.MergeSamplesV2(ROOT.SH,myhandler)
        if not myhandler.size() :
            print 'Error! Found no datasets. Exiting.'
            return
        print 'Setting input metadata:',
        print 'isAtlfast: %s'%('yes' if options.isAtlfast else 'no')
        print 'isData: %s'%('yes' if options.isData else 'no')
        for sample in myhandler :
            if options.mc15a :
                sample.setMetaString("PileupProfile","mc15a")
            else :
                sample.setMetaString("PileupProfile","mc15b")
            
            if options.isAtlfast :
                sample.setMetaString("Simulation","atlfast")
            elif options.isData :
                sample.setMetaString("Simulation","data")
                sample.setMetaString("IsData","True")
                sample.setMetaString("PileupProfile","data")
            else :
                sample.setMetaString("Simulation","fullsim") 

    # must be done AFTER scanDir
    if options.xaod :
        myhandler.setMetaString("nc_tree","CollectionTree") # must be done AFTER scanDir
    else :
        pyhelpers.SetTreeName(myhandler,options.treename)
        
    if options.n > 0 :
        myjob.options().setDouble(ROOT.EL.Job.optMaxEvents,options.n)
    #myjob.options().setDouble(ROOT.EL.Job.optSkipEvents,10000) # to start at a certain event

    if not myhandler.size() :
        print 'Error! Found no datasets. Exiting.'
        return

    #myhandler.setMetaString("submitDir","CollectionTree") # need?
    myjob.sampleHandler(myhandler)
    #myhandler.printContent()
    ROOT.PSL.PrintJobAlgorithms(myjob)

    driver = ROOT.EL.CondorDriver() if options.condor else ROOT.EL.DirectDriver()
    if options.condor :
        myjob.options().setString(ROOT.EL.Job.optCondorConf, 'getenv = True')
        #myjob.options().setDouble(ROOT.EL.Job.optFilesPerWorker,7)
        myhandler.setMetaDouble(ROOT.EL.Job.optEventsPerWorker,options.max);
        #myjob.options().setDouble(ROOT.EL.Job.optCacheSize, 500*1024*1024); # in MB (ignore 1024s)
        if options.systematics :
            if not susytools_alg :
                print 'Error! you tried to run systematics but you are not using SUSYObjDefAlg!'
                import sys
                sys.exit()
            os.system('mkdir -p %s'%(outputname))
            myhandler.setMetaString('vetodir','%s/veto'%(outputname))
            print 'veto directory is','%s/veto'%(outputname)
            for syst in pyhelpers.GetSystematicsList() :
                susytools_alg.set_sysVariation(syst)
                print 'Variation:',syst
                outputname_tail = outputname.split('/')[-1]
                outputname_syst = pyhelpers.getOutputName('%s/%s_%s'%(outputname,outputname_tail,syst))
                # Add weight systematics to nominal output
                if syst == 'NOMINAL' :
                    weight_systs = []
                    for inst in env.GetValue('WeightSystematics.Instances','').split() :
                        weight_systs.append(ROOT.PSL.WeightSystematics())
                        weight_systs[-1].SetName(inst)
                        pyhelpers.ConfigureCxxAlg(weight_systs[-1],env)
                        myjob.algsAdd(weight_systs[-1])
                driver.submitOnly(myjob,outputname_syst)
                print 'done',syst
            outputname = '%s_NOMINAL'%(outputname)
        else :
            driver.submitOnly(myjob,outputname)
            print 'Result written to %s'%(outputname) 
    else :
        # for running locally
        driver.submit(myjob,outputname)

    pyhelpers.SaveConfigInOutputDirectory(confname,outputname)
    pyhelpers.SaveConfigInOutputDirectory(defaultconf,outputname)

    if options.condor :
        return

    #
    # Retrieve output
    #
    for i in myhandler :
        print '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
        fname = '%s/hist-%s.root'%(outputname,i.name())
        print fname
        f = ROOT.TFile(fname,'read')
        for j in PassEventClasses :
            j.ReconnectHistograms(f)
            #print j.GetName()
            pyhelpers.PrintCutflow(j.GetName(),j.hist_PassN)
            #pyhelpers.PrintCutflow(j.GetName(),j.hist_PassW)
        f.Close()

    print 'Result written to %s'%(outputname)
    if options.ntuple :
        print 'Ntuple written to %s/data-SKIM'%(outputname)

    return        

if __name__ == "__main__":
    from optparse import OptionParser
    p = OptionParser()
    #
    # Options to input data. isData and isAtlfast are only used when --input is specified.
    #
    p.add_option('-i','--input',type  ='string'     ,default=''   ,dest='input'  ,help='input directory (as ready by SH)')
    p.add_option('--isData'   ,action='store_true',default=False,dest='isData'   ,help='IsData'   )
    p.add_option('--isAtlfast',action='store_true',default=False,dest='isAtlfast',help='isAtlfast')
    p.add_option('--mc15a'    ,action='store_true',default=False,dest='mc15a'    ,help='(for pileup reweighting) is it mc15a? Default is mc15b.')

    p.add_option('--samples'   ,type  ='string'     ,default=''   ,dest='samples',help='input samples (see Sample.h)')
    #
    # Other options
    #
    p.add_option('-n','--n',type  ='int'        ,default=-1   ,dest='n'     ,help='max nevents to run over'  )
    p.add_option('--max'  ,type  ='int'        ,default=500000,dest='max'   ,help='max nevents per condor process'  )
    p.add_option('--dconfig',type  ='string',default='DefaultConfiguration_mc15.conf',dest='dconfig',help='default config file' )
    p.add_option('--config' ,type  ='string',default='sr3l_soft.conf'           ,dest='config' ,help='config file' )
    p.add_option('--out'   ,type  ='string'    ,default='out'   ,dest='out',help='output name' )
    p.add_option('--treename'  ,type  ='string',default='susy'  ,dest='treename',help='tree name' )
    p.add_option('--skim'  ,action='store_true',default=False  ,dest='skim',help='do a skim (d3pd)' )
    p.add_option('--grid'  ,action='store_true',default=False  ,dest='grid',help='send to grid (d3pd)' )
    p.add_option('--submit',action='store_true',default=False  ,dest='submit',help='Submit to grid (d3pd)' )
    p.add_option('--ntuple',action='store_true',default=False  ,dest='ntuple',help='write an ntuple' )
    p.add_option('--condor',action='store_true',default=False  ,dest='condor',help='use condor' )
    p.add_option('--gridsamples',type ='string',default='GridSamples.txt',dest='gridsamples',help='Grid samples text file' )
    p.add_option('--tag',type  ='string'    ,default='dc14_ew01'   ,dest='tag',help='dataset tag' )
    p.add_option('--truth'  ,action='store_true',default=False  ,dest='truth',help='truth level analysis on truth DAODs' )
    p.add_option('--susy'  ,action='store_true',default=False  ,dest='susy',help='Use susy object selection (instead of WZ object selection)' )
    p.add_option('--susy2016'  ,action='store_true',default=False  ,dest='susy2016',help='Use susy2016 object selection (instead of WZ object selection)' )
    p.add_option('--systematics'  ,action='store_true',default=False  ,dest='systematics',help='Run all systematic variations' )

    options,args = p.parse_args()
    if options.grid :
       options.skim = True

    if options.systematics and not options.condor :
        print 'You are trying to run all systematics locally. Please use --condor.'
        import sys
        sys.exit()

    main(options,args)
