#!/usr/bin/env python

import ROOT
import os,sys
import pennSoftLepton.PyHelpers as pyhelpers
import pennSoftLepton.PyAnalysisPlotting as anaplot
import time

#-------------------------------------------------------------------------
def main (options,args) :
    #
    # output
    #
    outputname = pyhelpers.getOutputName(options.out,put_in_run_directory=False,preserve_dir=options.finish)
    #
    # Load c++
    #
    pyhelpers.LoadRootCore()

    #
    # TEnv Configuration File
    #
    confname    = '%s/../pennSoftLepton/config/%s'%(os.getenv('ROOTCOREBIN'),options.config)
    defaultconf = '%s/../pennSoftLepton/config/%s'%(os.getenv('ROOTCOREBIN'),options.dconfig)
    env = pyhelpers.GetTEnvFromConfs(confname,defaultconf)

    myjob = ROOT.EL.Job()
    edm_alg = ROOT.PSL.EDM()
    myjob.algsAdd(edm_alg)

    if options.n > 0 :
        myjob.options().setDouble(ROOT.EL.Job.optMaxEvents,options.n)

    #
    # Deal with samples
    #
    myhandler = ROOT.SH.SampleHandler()

    samples = []
    if options.bkgs :
        print options.bkgs
        for b in options.bkgs.split(',') :
            samples.append(b.replace('.root',''))
            tmp = ROOT.SH.SampleLocal(b.replace('.root',''))
            tmp.add(b)
            # print '%%%%%%%%%%%%%%%%%%%%%%%%%'
            # print tmp.meta().dumpToString()
            # print '%%%%%%%%%%%%%%%%%%%%%%%%%'
            myhandler.add(tmp)

    if options.data :
        tmp = ROOT.SH.SampleLocal(options.data.replace('.root',''))
        samples.append('data')
        tmp.add(options.data)
        myhandler.add(tmp)

    

    #
    # Other algorithms
    #
    all_cuts = pyhelpers.MakeCutsDictionaryFromTEnv(env)
    start_cut = ROOT.PSL.CutItem()
    start_cut.SetName('Start') # for the starting point of PassEvent algorithms
    start_cut.set_doAlwaysPass(True)
    myjob.algsAdd(start_cut)

    PassEventClasses = []
    for inst in env.GetValue('PassEvent.Instances','').split() :
        # inst is the instance "sr" or "top" or whatever
        PassEventClasses.append(ROOT.PSL.PassEvent())
        PassEventClasses[-1].SetName(inst)
        pyhelpers.ConfigureCxxAlg(PassEventClasses[-1],env)
        #PassEventClasses += pyhelpers.ConfigurePassEvent(inst,env) 
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

    #
    # Fake factor tool (if any) should be run after event selector but before PassEvent classes
    #
    FakeFactorToolClasses = []
    for inst in env.GetValue('FakeFactorTool.Instances','').split() :
        if options.systematics : 
            if not list(a in inst for a in ['ltt','tlt','ttl']).count(True) :
                continue
            if 'valid' in inst :
                continue
            systematics = ['nominal','ebin1','ebin2','ebin3','ebin4'
                           ,'mubin1','mubin2','mubin3','wzsyst','compe','compmu']
            tmp_systs = [ # probably bad to run all at once. comment a few out at a time...
                'nominal',
                'ebin1',
                'ebin2',
                'ebin3',
                'ebin4',
                'mubin1',
                'mubin2',
                'mubin3',
                'wzsyst',
                'compe',
                'compmu'
                ]
            for syst in tmp_systs :
                FakeFactorToolClasses.append(ROOT.PSL.FakeFactorTool())
                FakeFactorToolClasses[-1].SetName(inst)
                pyhelpers.ConfigureCxxAlg(FakeFactorToolClasses[-1],env)
                if (syst != 'nominal') :
                    FakeFactorToolClasses[-1].SetName('%s_%s'%(inst,syst))
                    FakeFactorToolClasses[-1].set_nvariation(systematics.index(syst))
                myjob.algsAdd(FakeFactorToolClasses[-1])
        else :
            FakeFactorToolClasses.append(ROOT.PSL.FakeFactorTool())
            FakeFactorToolClasses[-1].SetName(inst)
            pyhelpers.ConfigureCxxAlg(FakeFactorToolClasses[-1],env)
            myjob.algsAdd(FakeFactorToolClasses[-1])

    myhandler.setMetaString("nc_tree","physics") # must be done AFTER scanDir
    myjob.sampleHandler(myhandler)
    myhandler.printContent()
    ROOT.PSL.PrintJobAlgorithms(myjob)

    #
    # Ntupler
    #
    if options.ntuple :
        options.out = 'NTUPLE_%s'%(options.out)
        ntuple_alg = ROOT.PSL.Ntupler()
        myjob.algsAdd(ntuple_alg)
        pyhelpers.ConfigureCxxAlg(ntuple_alg,env)
        ntuple_alg.GetAlgList(myjob)
    
    #
    # Run
    #
    driver = ROOT.EL.DirectDriver()
    if not options.finish :
        driver.submit(myjob,outputname)

    # save a copy of config in output directory
    while True :
        if not os.path.exists(outputname) :
            time.sleep(2)
            continue
        os.system('cp %s %s/.'%(confname,outputname))
        os.system('cp %s %s/.'%(defaultconf,outputname))
        break

    #
    # Print a summary
    #
    print 'Output files:'
    for s in samples:
        fname = '%s/hist-%s.root'%(outputname,s)
        print fname

    return

if __name__ == "__main__":
    from optparse import OptionParser

    #p = anaplot.TreePlottingOptParser()
    p = OptionParser()

    p.add_option('--bkgs',type='string',default='',dest='bkgs',help='input files for bkg (csv)')
    p.add_option('--susy',type='string',default='',dest='susy',help='input files for susy (csv)')
    p.add_option('--data',type='string',default='',dest='data',help='input file for data (csv)')

    p.add_option('--config' ,type  ='string',default='sr3l_soft.conf'           ,dest='config' ,help='config file' )
    p.add_option('--dconfig',type  ='string',default='DefaultConfiguration_mc15.conf',dest='dconfig',help='default config file' )
    p.add_option('--out'   ,type  ='string'    ,default='out'   ,dest='out',help='output name' )
    p.add_option('-n','--n',type  ='int'        ,default=-1   ,dest='n'     ,help='max nevents to run over'  )
    p.add_option('--ntuple',action='store_true',default=False  ,dest='ntuple',help='write an ntuple' )

    p.add_option('--finish',action='store_true',default=False,dest='finish',help='Wrap up algos')
    p.add_option('--systematics'  ,action='store_true',default=False  ,dest='systematics',help='Run all systematic variations' )
    p.add_option('--instances'   ,type  ='string'    ,default=''   ,dest='instances',help='Instances of PassEvent classes that you want to run' )

    options,args = p.parse_args()

    if options.susy and not '.root' in options.susy :
        dir = options.susy
        options.susy = ','.join('%s/%s'%(dir,a) for a in os.listdir(options.susy))
    print options.susy

    options.bkgs = options.bkgs.split(',')
    for b in range(len(options.bkgs)) :
        if not options.bkgs[b] :
            continue
        if '.root' not in options.bkgs[b] :
            options.bkgs[b] = options.bkgs[b]+'.root'
    options.bkgs = ','.join(options.bkgs)
    print options.bkgs

    main(options,args)
    
