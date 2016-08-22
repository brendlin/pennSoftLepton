import ROOT
from itertools import product
from array import array

#-------------------------------------------------------------------------
def LoadRootCore() :
    import ROOT
    if 'RootCore' not in ROOT.gSystem.GetLibraries() :
        print 'Loading c++...'
        ROOT.gROOT.ProcessLine (".x $ROOTCOREDIR/scripts/load_packages.C")
        # Some weird bug where functions are not accessible until a class is called
        ROOT.PSL.EDM
    return

#-----------------------------------------------------------------------------------------
def IsFloat(thing) :
    try :
        out = float(thing)
    except ValueError :
        return False
    return True

#-----------------------------------------------------------------------------------------
def IsInt(thing) :
    if not IsFloat(thing) : return False
    return int(float(thing)) == float(thing)

#-----------------------------------------------------------------------------------------
def ConvertToFloat(thing) :
    try :
        out = float(thing)
    except ValueError :
        out = 0
    return out

#-----------------------------------------------------------------------------------------
def ConvertToInt(thing) :
    try :
        out = int(thing)
    except ValueError :
        out = 0
    return out

#-----------------------------------------------------------------------------------------
def MakeCutsDictionaryFromTEnv(env) :
    #
    # Make registry for each cut (starting with Cut.*) that exists
    # All cuts will be put into all_cuts dict?
    #
    CutItems = dict()
    for i in env.GetTable() :
        name = i.GetName()
        if name[:3] == 'Cut' :
            value = env.GetValue(name,'')
            value = value.replace(' ','')
            weight = None
            #name = name[4:] # Cut.* - don't use this anymore.
            #
            # Weights are denoted as such: Cut.CutBVeto: NBtagJet == 0; weight=BtagEventWeight
            #
            if ';weight=' in value :
                weight = value.split(';weight=')[1]
                value  = value.split(';weight=')[0]

            #print weight
            #print value

            value = value.replace('!=','$')
            value = value.replace('==','=')
            value = value.replace('<=','@')
            value = value.replace('>=','#')
            value = value.replace('&&','&')
            value = value.replace('||','|')

            if '|' in value and '&' in value :
                print 'Error! And AND Or together is not currently supported.'
                import sys
                sys.exit()

            #
            # Separators are: >= <= == && || < >
            #
            variables = []
            logic = []
            last = 0
            for i,s in enumerate(value) :
                if s in ['$','@','#','&','|','=','>','<'] :
                    logic.append(s)
                    variables.append(value[last:i])
                    last = i+1
                if i == len(value)-1 :
                    variables.append(value[last:])
                    
            # print variables
            # print logic

            #
            # Try to make a composite first, out of existing composites
            #
            ThisCutItem = ROOT.PSL.CutItem()
            ThisCutItem.SetName(name)
            if weight :
                ThisCutItem.weight = ROOT.PSL.ConvertToVariable(weight)

            if variables[0] in (CutItems[k].GetName() for k in CutItems.keys()) :
                if '|' in logic and '&' in logic :
                    print 'Error! Cannot do and and or of subcuts!'
                    continue
                comptype = ''
                if   '|' in logic : comptype = '|'
                elif '&' in logic : comptype = '&'
                ThisCutItem.set_subCut_ComparisonType(comptype)
                ThisCutItem.set_doCompareSubCutItems()
                ThisCutItem.set_SubCutStrings(' '.join(variables))
            
            else :
                ThisCutItem.set_doTemplatedComparison()
                ThisCutItem.set_template_ComparisonType(''.join(logic))

                #print dir(ThisCutItem)
                
                #
                # New treatment for lepton quantities with indices
                #
                lep_variables = []
                for i,v in enumerate(variables) :
                    if '[' in v :
                        lep_variables.append(v.split('[')[0])
                        variables[i] = v.split('[')[1].rstrip(']')
                    else :
                        lep_variables.append('')

                if len(variables) > 0 :
                    if IsFloat(variables[0]) : ThisCutItem.set_number1(ConvertToFloat(variables[0]));
                    else :                     ThisCutItem.set_var1(variables[0])
                    if lep_variables[0] : ThisCutItem.set_lepvar1(lep_variables[0])
                if len(variables) > 1 :
                    if IsFloat(variables[1]) : ThisCutItem.set_number2(ConvertToFloat(variables[1]));
                    else :                     ThisCutItem.set_var2(variables[1])
                    if lep_variables[1] : ThisCutItem.set_lepvar2(lep_variables[1])
                if len(variables) > 2 :
                    if IsFloat(variables[2]) : ThisCutItem.set_number3(ConvertToFloat(variables[2]));
                    else :                     ThisCutItem.set_var3(variables[2])
                    if lep_variables[2] : ThisCutItem.set_lepvar3(lep_variables[2])
                if len(variables) > 3 :
                    if IsFloat(variables[3]) : ThisCutItem.set_number4(ConvertToFloat(variables[3]));
                    else :                     ThisCutItem.set_var4(variables[3])
                    if lep_variables[3] : ThisCutItem.set_lepvar4(lep_variables[3])

            CutItems[ThisCutItem.GetName()] = ThisCutItem
            #ThisCutItem.PrintConfig()
                    
    #print CutItems
    return CutItems

#-------------------------------------------------------------------------
def ReadDatasetInfo(path, env):
    '''
       Read cross-section file and fill XSecData object(s)
       Return a list of XSecData objects
    '''

    import os

    if not os.path.isfile(path):
        print 'ReadDatasetInfo - invalid path to xsec file: %s' %path
        import sys
        sys.exit()

    #
    # Get sample definitions from TEnv. Store by e.g. Sample_list['Sample_z_sherpa_ee'] = 'zjee'
    #
    Sample_list = dict()
    for i in env.GetTable() :
        name = i.GetName()
        if name[:7] != 'Sample.' :
            continue
        Sample = name[7:]
        DSGroups = env.GetValue(name,'').split()
        for DSGroup in DSGroups :
            #
            # Here there is an option to specify fullsim or atlfast by appending
            # "-s" or "-a" to the name of the DS Group
            #
            DSGroupName = DSGroup.split('-')[0]
            DSGroupSiml = 'unspecified'
            UniqueMarkerAtlfastFullsimMC15aMC15b = 'unspecified'
            if len(DSGroup.split('-')) > 1 :
                DSGroupSiml = {'a':'atlfast','s':'fullsim','d':'data','amc15c':'atlfast','smc15c':'fullsim'}.get(DSGroup.split('-')[1])
                UniqueMarkerAtlfastFullsimMC15aMC15b = DSGroup.split('-')[1]
            Sample_list[DSGroupName] = dict()
            Sample_list[DSGroupName]['Sample'] = Sample
            Sample_list[DSGroupName]['Simulation'] = DSGroupSiml
            Sample_list[DSGroupName]['UniqueMarker(atlfast/fullsim,mc15a/mc15b)'] = UniqueMarkerAtlfastFullsimMC15aMC15b

    #
    # Read Sample.cxx file and parse lines for run, xsec, eff, etc
    #
    f = open(path, 'r')
    x = []

    for line in f.readlines():

        parts = line.rstrip('\n')
        if not parts : continue
        if '#' in parts : continue
        parts = parts.split()
        if not parts : continue
        if len(parts) < 6 :
            print 'Skipping run',parts[0],'since it is too small.'
            continue
        if parts[5] not in Sample_list.keys() : # 5 = Sample
            #print 'Skipping run',parts[0],'key',parts[5],'since it is not in the conf list.'
            continue

        x.append(dict())
        x[-1]['RunNumber'] = ConvertToInt(parts[0])
        #x[-1]['m_c1'     ] = ConvertToFloat(parts[1])
        #x[-1]['m_n1'     ] = ConvertToFloat(parts[2])
        x[-1]['generator'] = ConvertToFloat(parts[3])
        x[-1]['sample'   ] = Sample_list[parts[5]]['Sample'] # ggww, qqww, susy, etc.
        x[-1]['xsec*kfactor*feff'] = 1. # set elsewhere
        x[-1]['Simulation'] = Sample_list[parts[5]]['Simulation']
        x[-1]['UniqueMarker(atlfast/fullsim,mc15a/mc15b)'] = Sample_list[parts[5]]['UniqueMarker(atlfast/fullsim,mc15a/mc15b)']
        x[-1]['DatasetGroupName'] = parts[5]

    return x

#-------------------------------------------------------------------------
def GetMassesMap() :
    import os
    testArea = os.environ.get('ROOTCOREBIN').rstrip('/')+'/..'
    DsInfoPath = testArea+'/pennSoftLepton/config/Samples.txt'
    f = open(DsInfoPath, 'r')
    x = dict()

    for line in f.readlines() :
        parts = line.rstrip('\n')
        if not parts : continue
        if '#' in parts : continue
        parts = parts.split()
        if not parts : continue
        if len(parts) < 3 :
            print 'skipping',parts[0]
            continue
        if not IsFloat(parts[1]) :
            continue
        run = ConvertToInt(parts[0])
        x[run] = dict()
        x[run]['RunNumber']    = ConvertToInt(parts[0])
        x[run]['mc1']          = ConvertToFloat(parts[1])
        x[run]['mn1']          = ConvertToFloat(parts[2])
    return x

#-------------------------------------------------------------------------
def GetFromEnv(env,ClassName,ClassInstanceName,ItemName,Default) :
    """
    Defines my override structure of TEnv
    ClassName: PassEvent
    ClassInstanceName: sr
    ItemName: Samples
    PassEvent.sr.Samples overrides PassEvent.Samples
    You MUST specify a default because Env needs to know what kind of item you're looking for
    """
    output = Default
    #print '%s.%s'%(ClassName,ItemName)
    #print '%s.%s.%s'%(ClassName,ClassInstanceName,ItemName)
    if env.Defined(           '%s.%s'%(ClassName,ItemName)) :
        output = env.GetValue('%s.%s'%(ClassName,ItemName),Default)
    if env.Defined(           '%s.%s.%s'%(ClassName,ClassInstanceName,ItemName)) :
        output = env.GetValue('%s.%s.%s'%(ClassName,ClassInstanceName,ItemName),Default)
    return output

#-------------------------------------------------------------------------
def ConfigureCxxAlg(theinstance,env,class_name='') :
    instance_name = theinstance.GetName()
    if not class_name : class_name = theinstance.ClassName().split('::')[-1]
    for i in env.GetTable() :
        name = i.GetName()
        if name.split('.')[0] != class_name :
            continue
        if 'Instance' in name :
            continue
        # prevent cross-talk between two different class instances:
        if len(name.split('.')) == 3 and name.split('.')[1] != instance_name :
            continue
        member = name.split('.')[-1]
        value = GetFromEnv(env,class_name,instance_name,member,'')
        value = value.rstrip(' ').lstrip(' ') # it is ridiculous that we need this.
        # print member,value
        if 'set_%s'%(member) in dir(theinstance) :
            #print 'setting %s'%member
            if IsInt(value) :
                #print 'Setting %s to %d'%(member,int(float(value)))
                exec('theinstance.set_%s(%s)'%(member,int(float(value))))
            elif IsFloat(value) :
                #print 'Setting %s to %d'%(member,value)
                exec('theinstance.set_%s(%s)'%(member,value))
            else :
                #print 'Setting %s to %s'%(member,value)
                exec('theinstance.set_%s(\"%s\")'%(member,value))                
        else :
            print 'Error! You meant to set property set_%s in class %s'%(member,class_name),
            print 'but this property does not exist! Exiting.'
            import sys
            sys.exit()
    return

#-------------------------------------------------------------------------
def ConfigurePassEvent(instance_name,env) :
    PassEventClasses = []
    #
    # If you want a matrix based on a bunch of cuts, then
    # write them into a PassEvent.topcr.MakeMatrixFromCuts value, as if it were
    # a python dictionary. The code will be exec'ed. As in:
    # [['CutChannel','CutChannelaa','CutChannelee','CutChanneleu','CutChanneluu','CutChannelll','CutChannelORDEReu','CutChannelORDERue'],
    # ['CutSign','CutSignOS','CutSignSS']]
    #
    # You need to specify the following in the configuration file:
    #
    # PassEvent.topcr.MakeMatrixFromCuts (i.e. PassEvent.instance_name.Cuts)
    # PassEvent.topcr.Cuts
    #
    stop_before_list = GetFromEnv(env,'PassEvent',instance_name,'StopBefore','ALL')
    stop_before_list = stop_before_list.split()
    samples = GetFromEnv(env,'PassEvent',instance_name,'Samples','all')
    original_cuts = GetFromEnv(env,'PassEvent',instance_name,'Cuts','')
    variables = GetFromEnv(env,'PassEvent',instance_name,'Variables','')
    lepvariables = GetFromEnv(env,'PassEvent',instance_name,'LepVariables','')
    parent_selector = GetFromEnv(env,'PassEvent',instance_name,'ParentEventSelectorName','')

    matrix_string = GetFromEnv(env,'PassEvent',instance_name,'MakeMatrixFromCuts','')
    placeholder_cuts = ['DUMMY']
    list_of_inserts = [['DUMMY']]
    if matrix_string :
        exec_string = 'the_dict = %s'%(matrix_string)
        exec exec_string
        placeholder_cuts = []
        list_of_inserts = []
        for k in the_dict :
            placeholder_cuts.append(k[0]) # the placeholder name
            list_of_inserts.append(k[1:]) # the list of replacements

    for x in product(*list_of_inserts) :
        new_cuts = original_cuts
        alg_name = '%s'%(instance_name) # used to be pass_%s
        #
        # Go over the list of cuts you want to expand in. DUMMY is put there
        # so you make 1 instance.
        #
        for i in range(len(x)) :
            new_cuts = new_cuts.replace(placeholder_cuts[i],x[i])
            #
            # If you want to bin in some different cut, you need to add the postfix name here.
            #
            alg_name += {'CutSignOS':''
                         ,'CutSignSS':'_ss'
                         ,'CutChannelaa':'_aa'
                         ,'CutChannelee':'_ee'
                         ,'CutChanneleu':'_eu'
                         ,'CutChanneluu':'_uu'
                         ,'CutChannelll':'_ll'
                         ,'CutChannelORDEReu':'_ORDEReu'
                         ,'CutChannelORDERue':'_ORDERue'
                         ,'CutNElecGe3':'_elnge3'
                         ,'CutNElecLt3':'_elnlt3'
                         ,'DUMMY':''
                         }.get(x[i],x[i])
        #
        # Also bin in StopBefore cut
        #
        for stop_before in stop_before_list :
            PassEventClasses.append(ROOT.PSL.PassEvent())
            #
            # Set name (postfixes according to cut matrix, and stop_before_list)
            #
            stop_before_postfix = ''
            if stop_before != 'ALL' :
                stop_before_postfix = '_%s'%(stop_before.replace('Cut','Before'))
            alg_name += stop_before_postfix
            #print 'Alg name:',alg_name
            PassEventClasses[-1].SetName(alg_name)
            #
            # Configure
            #
            if stop_before in new_cuts :
                stop_before_cutoff = new_cuts.index(stop_before)
                new_cuts = new_cuts[:stop_before_cutoff]
            PassEventClasses[-1].set_Cuts(new_cuts);
            PassEventClasses[-1].set_Samples(samples)
            if variables : PassEventClasses[-1].set_Variables(variables)
            if lepvariables : PassEventClasses[-1].set_LepVariables(lepvariables)
            PassEventClasses[-1].set_ParentEventSelectorName(parent_selector)
        # stop_before list
    # list of cut inserts
    return PassEventClasses

#-------------------------------------------------------------------------
def AddCutsToJob(myjob,cutname,cut_dict) :
    if myjob.algsHas(cutname) : return
    # print 'Adding cut %s'%(cutname)
    subcuts = list(cut_dict[cutname].SubCutStrings)
    for subcut in subcuts :
        # print 'Requesting cut %s'%(subcut)
        if subcut not in cut_dict.keys() :
            print 'Error! Could not find alg %s, requested by %s. Exiting.'%(subcut,cutname)
            import sys
            sys.exit()
        AddCutsToJob(myjob,subcut,cut_dict)
    myjob.algsAdd(cut_dict[cutname])
    return

#-------------------------------------------------------------------------
def GetTTreeLumiScaleFactor(list_of_files,fb_requested) :
    lumi_from_file = 0
    #
    # In case list of files isn't flattened yet... flatten it.
    #
    files_flattened = []
    for f in list_of_files :
        if issubclass(type(f),ROOT.TFile) : files_flattened.append(f)
        elif type(f) == type([]) : files_flattened += f
    
    for f in files_flattened :
        for k in f.GetListOfKeys() :
            ko = k.ReadObj()
            if not issubclass(type(ko),ROOT.TH1) : continue
            if not (ko.GetNbinsX() >= 6) : continue
            if not ('lumi' in ko.GetXaxis().GetBinLabel(6)) : continue
            if not lumi_from_file :
                lumi_from_file = float(ko.GetXaxis().GetBinLabel(6).split(':')[1])
            elif lumi_from_file != float(ko.GetXaxis().GetBinLabel(6).split(':')[1]) :
                print 'Error! Lumi does not agree between files!'

    lumi_scale_factor = 1
    if (fb_requested > 0) and (lumi_from_file != fb_requested) :
        lumi_scale_factor = fb_requested*1000./float(lumi_from_file)

    print 'Old lumi (pb):',lumi_from_file,'New lumi (pb):',fb_requested*1000,'Scale factor:',lumi_scale_factor
    the_lumi_fb = lumi_from_file/1000.
    if fb_requested > 0 :
        the_lumi_fb = fb_requested
    return the_lumi_fb,lumi_scale_factor

#-------------------------------------------------------------------------
def PrintCutflow(label,hist,samp_list=[],scientific=False,spreadsheet=False,latex=False) :
    pm = '$\pm$' if latex else u"\u00B1" # unicode +/-
    import ROOT
    if not issubclass(type(hist),ROOT.TH1) :
        print 'problem'
        return
    import math
    colwidth_nevts = 4 # 0.00
    colwidth_err   = 4 # 0.00
    for j in range(hist.GetNbinsY()) : # cut
        for i in range(hist.GetNbinsX()) : # sample
            if samp_list and hist.GetXaxis().GetBinLabel(i+1) not in samp_list : continue
            cont = max(1,hist.GetBinContent(i+1,j+1))
            colwidth_nevts = int(max(colwidth_nevts,3+1+math.floor(math.log(cont)/math.log(10))))
            if scientific :
                colwidth_nevts = 8
            err = max(1,hist.GetBinError(i+1,j+1))
            colwidth_err = int(max(colwidth_err,3+1+math.floor(math.log(err)/math.log(10))))
    
    firstcolwidth = 4
    for i in range(hist.GetNbinsY()) :
        firstcolwidth = int(max(firstcolwidth,len(hist.GetYaxis().GetBinLabel(i+1))))
    firstcolwidth = int(max(firstcolwidth,len(label)))
    firstcolwidth = firstcolwidth + 2

    if spreadsheet :
        firstcolwidth = 25

    colwidth = colwidth_nevts+colwidth_err+6 # 5 for \pm, 1 extra for buffer
    if latex : colwidth += 2
    text = ''
    if not spreadsheet :
        text += label.ljust(firstcolwidth)
        if latex : text += '& '
        for i in range(hist.GetNbinsX()) :
            if samp_list and hist.GetXaxis().GetBinLabel(i+1) not in samp_list : continue
            text += hist.GetXaxis().GetBinLabel(i+1).ljust(colwidth)
            if latex and (i != hist.GetNbinsX()-1) : text += '& '
        if latex : text += '  \\\\'
        text += '\n'
    for j in range(hist.GetNbinsY()) : # cut
        if spreadsheet and hist.GetYaxis().GetBinLabel(j+1) == 'Start' : continue
        text += hist.GetYaxis().GetBinLabel(j+1).ljust(firstcolwidth)
        if latex : text += '& '
        for i in range(hist.GetNbinsX()) : # sample
            if spreadsheet and hist.GetXaxis().GetBinLabel(i+1) == 'data' : continue
            if samp_list and hist.GetXaxis().GetBinLabel(i+1) not in samp_list : continue
            cont = hist.GetBinContent(i+1,j+1)
            err = hist.GetBinError(i+1,j+1)
            if scientific :
                text += ('%.2e'%cont).rjust(colwidth_nevts)
            else :
                text += ('%2.2f'%cont).rjust(colwidth_nevts)
            text += ' %s '%pm
            if scientific :
                text += ('%.2e'%(err)).rjust(colwidth_err)
            else :
                text += ('%2.2f'%(err)).rjust(colwidth_err)
            if latex and (i != hist.GetNbinsX()-1) : text += ' & '
            else : text += '   '
        if latex : text += '\\\\'
        text += '\n'
    if spreadsheet :
        text = text.rstrip('\n')
    print text
    return

#-------------------------------------------------------------------------
def getFile(filename,fatal=True) :
    from ROOT import TFile
    filename = filename.replace('root://hn.at3f/disk/space00/srm/','/xrootd/srm/')
    filename = filename.replace('root://hn.at3f//srm/','/xrootd/srm/')

    if ('eosatlas' in filename) or ('castoratlas' in filename) :
        tfile = TXNetFile(filename,'READ')
    else :
        #print 'not an eos file'
        tfile = TFile(filename,'READ')
    if tfile.IsZombie() and fatal :
        print 'Fatal. Exiting.'
        import sys
        sys.exit()
    return tfile

def GetFile(filename,fatal=True) :
    return getFile(filename,fatal)

def getTree(file,tree='') :
    from ROOT import TTree
    nEvents = -1
    returntree = 0
    for item in file.GetListOfKeys() :
        itree = item.ReadObj()
        if tree and itree.GetName() != tree : continue
        if itree.GetName() not in ['photon','egamma','physics','trigger','electron_ID','susy'] :
            continue
        if type(itree) == type(TTree()) :
            ievents = int(itree.GetEntries())
            if ievents > nEvents :
                nEvents = ievents
                returntree = itree
    if tree and (not returntree) :
        print 'Error! Tree %s not found! Trying to find any tree.'%tree
        return getTree(file)
    if not returntree : print 'Error! Tree not found!',file
    return returntree

def GetRootObj(file,name,printWarning=True,fatal=False):
    obj = file.Get(name)
    if not obj :
        if printWarning : print 'Warning in %s: %s does not exist. %s'%(file.GetName(),name,'Fatal; exiting.' if fatal else '')
        if fatal :
            import sys
            sys.exit()
        return 0
    else : return obj

#-------------------------------------------------------------------------
def getOutputName(outname,put_in_run_directory=True,preserve_dir=False) :
    import datetime,os
    #ctime = datetime.datetime.now()
    #thetime = ctime.strftime('%Y_%m_%d_%H_%M_%S')
    rootcorebindir = os.getenv('ROOTCOREBIN')
    if put_in_run_directory :
        os.system('mkdir -p %s/../run'%(rootcorebindir))
    #dirName = "%s/../run/%s_%s"%(rootcorebindir,outname,thetime)
    if put_in_run_directory and outname[0] != '/' :
        dirName = "%s/../run/%s"%(rootcorebindir,outname)
    else :
        dirName = "%s"%(outname)
    if os.path.exists(dirName) and not preserve_dir :
        print 'Warning: Output directory exists.'
        tmp = raw_input('Please enter a new output directory (or press enter to overwrite old one): ')
        if not tmp : 
            #
            # Remove very safely.
            #
            def ClearDir(_dir) :
                if os.path.exists(_dir) : 
                    cmd = 'rm %s/*'%_dir ; print cmd; 
                    if os.listdir(_dir) :
                        os.system(cmd)
                    cmd = 'rmdir %s'%_dir; print cmd;
                    os.system(cmd)
                return
            
            ClearDir('%s/%s'%(dirName,'output-SKIM'))
            ClearDir('%s/%s'%(dirName,'data-SKIM'  ))
            ClearDir('%s/%s'%(dirName,'hist'       ))
            ClearDir('%s/%s'%(dirName,'input'      ))
            ClearDir('%s/%s/%s'%(dirName,'fetch','data-SKIM'))
            ClearDir('%s/%s'%(dirName,'fetch'      ))
            ClearDir('%s/%s'%(dirName,'run'        ))
            ClearDir('%s/%s'%(dirName,'submit'     ))
            ClearDir('%s/%s'%(dirName,'veto'       ))
            ClearDir('%s'   %(dirName))

        else :
            dirName = "%s/../run/%s"%(rootcorebindir,tmp)

    return dirName

#-------------------------------------------------------------------------
def MergeSamplesV2(sh,samplehandler) :
    done = False
    while not done :
        if not samplehandler.size() :
            break
        for sample in samplehandler :
            samplename = sample.name()
            if 'mc12_8TeV' in samplename :
                newname = samplename.split('mc12_8TeV.')[1].split('.')[0]
                sh.mergeSamples(samplehandler,newname,samplename)
                break
            if 'mc14_13TeV' in samplename :
                newname = samplename.split('mc14_13TeV.')[1].split('.')[0]
                sh.mergeSamples(samplehandler,newname,samplename)
                break
            done = True
    return

#-------------------------------------------------------------------------
def GetDataFiles(sh,myhandler,pennsrm,edm_alg) :
    fpennsrm = open(pennsrm).readlines()

    sample_byrun = dict()
    entries_byrun = dict()

    for f in fpennsrm :
        #run = f.split()[0]
        run = 'data'
        samplename = 'data_%s'%(run)
        filename = f.split()[2]
        filename = filename.replace('root://hn.at3f/disk/space00/srm/'
                                    ,'root://hn.at3f//srm/')
        #
        # Group data files by their filename
        #
        if run not in sample_byrun.keys() :
            sample_byrun[run] = ROOT.SH.SampleLocal(samplename)
            entries_byrun[run] = []

        sample_byrun[run].add(filename)
        entries_byrun[run].append(float(f.split()[4]))

    for s in sample_byrun.keys() :
        sample_byrun[s].setMetaString("IsData","True")
        sample_byrun[s].setMetaString("PileupProfile","data")
        myhandler.add(sample_byrun[s])
        ROOT.PSL.SetSHSampleEntries(sample_byrun[s],len(entries_byrun[s]),array('d',entries_byrun[s]))

    return

#-------------------------------------------------------------------------
def GetFilesFromSamples(sh,myhandler,xsec_list_from_txtfile,pennsrm,samples,edm_alg) :
    import os
    #
    # Get xsec_list_from_txtfile by running PyHelpers.readXSecFile
    #
    print 'Running via sample input. Taking from %s'%(pennsrm)
    if not os.path.isfile(pennsrm) :
        print 'Error! Could not find SRM file %s. Exiting.'%(pennsrm)
        import sys
        sys.exit()
    fpennsrm = open(pennsrm).readlines()
    sampleslist = []
    #
    # The following looks in samples.h and expands any compound samples
    #
    for i in samples.split(',') :
        if i == 'data' :
            continue
        sampleslist += list(ROOT.PSL.ConvertSampleToSampleVec(i))

    #
    # Load susy xs database
    #
    dbpath = '%s/../pennSoftLepton/config/AdditionalCrossSections.txt'%(os.getenv("ROOTCOREBIN"))
    print 'Adding cross sections from %s (first, so official results can supercede the ones in this file)'%(dbpath)
    susy_xsecDB = ROOT.SUSY.CrossSectionDB(dbpath)

    dbpath = '%s/../SUSYTools/data/mc15_13TeV/'%(os.getenv("ROOTCOREBIN"))
    if not os.path.exists(dbpath) :
        dbpath = '%s/../SUSYTools/data/mc15_13TeV/'%(os.getenv("ROOTCOREDIR"))
    print 'Getting cross sections using SUSY.CrossSectionDB from %s'%(dbpath)
    for dbfile in os.listdir(dbpath) :
        print dbpath+dbfile
        susy_xsecDB.loadFile(dbpath+dbfile)

    #let's overwrite some of these files
    dbpath = '%s/../pennSoftLepton/config/AdditionalCrossSections_overwriteSUSY.txt'%(os.getenv("ROOTCOREBIN"))
    print 'Adding cross sections from %s. These contain fixes to the SUSYTools cross-sections'%(dbpath)
    susy_xsecDB.loadFile(dbpath)

    #for i in susy_xsecDB :
    #    print i.second.ID(),i.second.name()

    #
    # Non-SUSY samples will be merged by sample. SUSY samples will be kept as individual DS files.
    #
    for sample in list(set(sampleslist)) :
        print sample
        #
        # loop over run dicts
        #
        for run_info_dict in xsec_list_from_txtfile :
            # Find a match to the sample
            if not (run_info_dict['sample'] == sample) : # if qqww == qqww
                continue
            run = run_info_dict['RunNumber'] # 999999
            chosen_sim_tag = run_info_dict['Simulation']
            chosen_unique_marker = run_info_dict['UniqueMarker(atlfast/fullsim,mc15a/mc15b)']
            is_user_specified_sim_tag = (chosen_unique_marker != 'unspecified')
            samplename = '%s_%d'%(sample,run)
            sample_byrun = ROOT.SH.SampleLocal(samplename)
            sample_exists = False
            sumw = 0
            nfiles = 0
            entries_byrun = []
            #
            # loop over SRM map
            #
            for f in fpennsrm :
                if '#' in f :
                    continue
                if not (str(run) == f[:6]) :
                    continue
                #current_file_sim_tag = {'a':'atlfast','s':'fullsim','d':'data','amc15b':'atlfast','smc15b':'fullsim'}.get(f.split()[1])
                current_file_sim_tag = {'a':'atlfast','s':'fullsim','d':'data','amc15c':'atlfast','smc15c':'fullsim'}.get(f.split()[1])
                current_file_unique_marker = f.split()[1]
                #
                # If the user specify the simulation type, then honor that
                #
                if (is_user_specified_sim_tag) and (current_file_unique_marker != chosen_unique_marker) :
                    #print 'rejecting the file.'
                    continue
                #
                # Protection against unknowingly having the choice between simulation type
                #
                if (not is_user_specified_sim_tag) :
                    if (chosen_unique_marker != 'unspecified') and (current_file_unique_marker != chosen_unique_marker) :
                        print 'Error! User did not specify simulation (atlfast/fullsim) and/or mc15a/b, and there is >1 option!'
                        print 'Please specify one of the following in your conf file:'
                        print 'Sample.%s: %s-s (fullsim, mc15a)'%(sample,run_info_dict['DatasetGroupName'])
                        print 'Sample.%s: %s-a (atlfast, mc15a)'%(sample,run_info_dict['DatasetGroupName'])
                        print 'Sample.%s: %s-smc15c (fullsim, mc15c)'%(sample,run_info_dict['DatasetGroupName'])
                        print 'Sample.%s: %s-amc15c (atlfast, mc15c)'%(sample,run_info_dict['DatasetGroupName'])
                        import sys
                        sys.exit()

                #print 'accepting the file.'
                nfiles += 1
                chosen_unique_marker = current_file_unique_marker
                chosen_sim_tag = current_file_sim_tag
                #DefaultChannel = {'a':0,'s':0,'d':0,'amc15b':410000,'smc15b':410000}.get(f.split()[1])
                PileupProfile = {'a':'mc15a','s':'mc15a','d':'data','amc15c':'mc15c','smc15c':'mc15c'}.get(f.split()[1])

                sample_exists = True
                sumw += float(f.split()[3]) # sumw
                filename = f.split()[2]
                filename = filename.replace('root://hn.at3f/disk/space00/srm/'
                                            ,'root://hn.at3f//srm/')
                sample_byrun.add(filename)
                entries_byrun.append(float(f.split()[4]))
            # end loop over SRM map

            if not sample_exists :
                print 'WARNING Skipping run %d (sample %s) since it is not in the Map_SRM'%(run,sample)
                if ('n' == raw_input('Do you still want to continue? (y/n)')) :
                    import sys
                    sys.exit()
                continue # If there are no files then do not add to run list
            elif sumw == 0 :
                print 'WARNING Skipping run %d (sample %s) due to sumw=0'%(run,sample)
                if ('n' == raw_input('Do you still want to continue? (y/n)')) :
                    import sys
                    sys.exit()
                continue # If there were no events then do not add to run list

            sample_byrun.setMetaString("Simulation",chosen_sim_tag)
            #sample_byrun.setMetaDouble("DefaultChannel",DefaultChannel)
            sample_byrun.setMetaString("PileupProfile",PileupProfile)
            myhandler.add(sample_byrun)
            ROOT.PSL.SetSHSampleEntries(sample_byrun,len(entries_byrun),array('d',entries_byrun))

            print 'Adding run to sample list:',
            print 'sample: %s'%(sample),
            print 'run: %d'%(run),
            print 'identifier: %s'%(chosen_unique_marker),
            print 'simulation: %s'%(chosen_sim_tag),
            print 'sumw: %2.5f'%(sumw),
            print 'nentries: %d'%(sum(entries_byrun)),
            print 'nfiles: %d'%(nfiles)

            # do this for both SUSY and other samples
            db_p = susy_xsecDB.process(run)
            if db_p.ID() < 0 :
                db_p = susy_xsecDB.process(run,125) # stupid "process" thing
            if db_p.ID() > 0 :
                run_info_dict['xsec*kfactor*feff'] = db_p.xsect()*db_p.kfactor()*db_p.efficiency()
                print 'with cross section',run_info_dict['xsec*kfactor*feff']
            else :
                print 'Error! No cross section found for %s using SUSY DB!'%(run),
                import sys
                sys.exit()
                print 'Default to value stored in Samples.txt '
            edm_alg.ConfigureRun(run,run_info_dict['sample'],run_info_dict['xsec*kfactor*feff'],sumw)
        # end loop over run dicts
    # end sample enums
    #myhandler.printContent()
    return

#-------------------------------------------------------------------------
def GetTEnvFromConfs(confname,defaultconf) :
    import ROOT,os

    if not os.path.exists(defaultconf) :
        print 'Cannot find default configuration file, %s. Exiting.'%(defaultconf)
        import sys; sys.exit();
        return
    if not os.path.exists(confname) :
        print 'Cannot find configuration file, %s. Exiting.'%(confname)
        import sys; sys.exit();
        return
    env = ROOT.TEnv(confname)        
    print 'Using conf name %s'%(confname)
    defaults = ROOT.TEnv(defaultconf)
    print 'Using default conf %s'%(defaultconf)
    for i in defaults.GetTable() :
        if not env.Defined(i.GetName()) :
            env.SetValue(i.GetName(),i.GetValue())
        #env.PrintEnv()
    return env

#-------------------------------------------------------------------------
def SetTreeName(samplehandler,treename='susy') :
    #
    # Optional to switch d3pd name on the fly
    #
    for sample in samplehandler :
        # print sample
        # for i in range(sample.numFiles()) :
        for i in range(1) :
            print 'Accessing %s to get tree name.'%(sample.fileName(i))
            f = GetFile(sample.fileName(i))
            e = getTree(f,treename)
            print 'Found tree %s with %d entries'%(e.GetName(),e.GetEntries())
            treename = e.GetName()
        f.Close()
        break # just need one sample
    samplehandler.setMetaString("nc_tree",treename) # must be done AFTER scanDir

#-------------------------------------------------------------------------
def SetOutputDatasetNames(samplehandler,tag) :
    import re
    for sample in samplehandler :
        name_new = 'user.brendlin.%s.%s'%(sample.name(),tag)
        name_new = name_new.replace('.merge.','.')
        name_new = name_new.replace('_AUET2B_CTEQ6L1_','_')
        name_new = name_new.replace('_AUET2BCTEQ6L1_','_')
        name_new = name_new.replace('_AU2MSTW2008LO_','_')
        name_new = name_new.replace('_UEEE3_CTEQ6L1_','_')
        name_new = name_new.replace('_AUET2CTEQ6L1_','_')
        name_new = name_new.replace('_AU2_CTEQ6L1_','_')
        name_new = name_new.replace('_AU2CTEQ6L1_','_')
        name_new = name_new.replace('_AUET2CT10_','_')
        name_new = name_new.replace('_AU2CT10_','_')
        name_new = name_new.replace('_CT10_','_')

        name_new = name_new.replace('MadGraphPythia','MadGraph')
        name_new = name_new.replace('PowHegPythia8','PowP8')
        name_new = name_new.replace('PowhegPythia8','PowP8')
        name_new = name_new.replace('McAtNloJimmy','McAtNlo')
        name_new = name_new.replace('AlpgenPythia_P2011C' ,'AlpPy')
        name_new = name_new.replace('AlpgenPythia_Auto_P2011C' ,'AlpPyAuto')
        name_new = name_new.replace('AlpgenJimmy' ,'AlpJim')
        name_new = name_new.replace('Pythia8_AU2CTEQ6L1' ,'Pythia8')

        name_new = re.sub(r'\.e[0-9][0-9][0-9][0-9]_','.',name_new)

        name_new = name_new.replace('NTUP_SUSY','SUSY')
        name_new = name_new.replace('LeptonFilter5','LFil')
        name_new = name_new.replace('LeptonFilter','LFil')
        name_new = name_new.replace('LeptonPhotonFilter' ,'LPFil')
        name_new = name_new.replace('_fullme','')
        name_new = name_new.replace('mll0p250d0' ,'mll0p250')
        name_new = name_new.replace('mll0p4614d0','mll0p4614')
        name_new = name_new.replace('mll3p804d0' ,'mll3p804')
        name_new = name_new.replace('Z11_mll' ,'Z11mll')
        name_new = name_new.replace('Z13_mll' ,'Z13mll')
        name_new = name_new.replace('Z15_mll' ,'Z15mll')
        name_new = name_new.replace('Ztautau' ,'Ztt')
        name_new = name_new.replace('Zmumu' ,'Zmm')
        name_new = name_new.replace('Mll10to40' ,'M10t40')
        name_new = name_new.replace('Mll40to60' ,'M40t60')
        name_new = name_new.replace('Mll10to60' ,'M10t60')
        name_new = name_new.replace('Excl' ,'Ex')
        name_new = name_new.replace('Incl' ,'In')
        name_new = name_new.replace('_simplifiedModel_' ,'_')

        sample.setMetaString("nc_outputSampleName",name_new)

        force_exit = False
        for sample in samplehandler :
            nm = sample.getMetaString("nc_outputSampleName")
            len1 = len('.user.brendlin._hist-output.root/') + len(nm) - len('merge.')
            len2 = len('user.brendlin./') + len(nm) - len('merge.')
            if len1 >= 132 :
                print 'Too large:',len1,nm
                force_exit = True
            elif len2 >= 120 :
                print 'Too large:',len2,nm
                force_exit = True
        
    if force_exit :
        return False
    return True

#-------------------------------------------------------------------------
def sendToGrid(job,samplehandler,options) :
    #
    # Looks for pennSoftLepton/config/GridSamples_t000.txt
    #
    samplelist = '%s/../pennSoftLepton/config/GridSamples_%s.txt'%(os.getenv('ROOTCOREBIN'),options.tag)
    print 'Reading datasets from %s'%(samplelist)
    files = open(samplelist).readlines()
    for f in files :
        f = f.replace('\n','')
        if not f : continue
        if '#' in f : continue
        ROOT.SH.addGrid (samplehandler,f);
    samplehandler.setMetaString("nc_tree",options.treename) # most be done after reading files?

    if not SetOutputDatasetNames(samplehandler,options.tag) :
        print 'Error! Sample names are too large.'
        return

    for sample in samplehandler :
        print ' '+sample.getMetaString("nc_outputSampleName")
    print 'Submitting %d samples total.'%(len(samplehandler))

    job.sampleHandler(samplehandler)

    driver = ROOT.EL.PrunDriver()
    #job.options().setString(ROOT.EL.Job.optGridNFilesPerJob, "MAX");
    job.options().setDouble(ROOT.EL.Job.optGridMaxNFilesPerJob, 1);
    job.options().setDouble(ROOT.EL.Job.optGridMergeOutput, 1); #run merging jobs for all 
    job.options().setString("nc_destSE","UPENN_LOCALGROUPDISK")
    job.options().setString("nc_transferredDS","*SKIM.root*")
    job.options().setDouble("nc_individualOutDS",1)
    #job.options().setString("nc_outputs","SKIM.root") # did not work

    print '##'
    print '## Submitting jobs with tag %s.'%(options.tag)
    print '## You have 10 seconds to kill this process.'
    print '##'

    if not options.submit :
        print 'Not submitted. Specify --submit.'
        sys.exit()

    time.sleep(10) # seconds

    #samplehandler.printContent() # same as print, but python won't allow print
    options.out = 'GRID_%s'%(options.out)
    driver.submitOnly(job,getOutputName(options));
    
    print 'Grid submission done.'

#-------------------------------------------------------------------------
def GetListOfKeysFromFile(f) :
    keys = []
    for i in f.GetListOfKeys() :
        #print i.GetName()
        if 'PassEvent_' in i.GetName() :
            keys.append(i.GetName().replace('PassEvent_',''))
    return keys

def GetListOfProcessesFromFile(f,key,detailed=False) :
    procs = []
    import ROOT
    if not f.Get('PassEvent_%s'%(key)) :
        print 'Error: File %s does not have PassEvent_%s'%(f.GetName(),key)
        import sys
        sys.exit()
    for i in f.Get('PassEvent_%s'%(key)).GetListOfKeys() :
        if not 'PassEvent_' in i.GetName() :
            continue
        #print i.GetName()
        proc = i.GetName().replace('PassEvent_','').replace('%s_'%key,'').split('_')
        if not len(proc) : 
            continue 
        if proc[0] in ['PassN','PassW','all'] :
            continue
        if proc[0] not in procs :
            procs.append(proc[0])

    if detailed :
        for i in procs :
            subs = list(ROOT.PSL.ConvertSampleToSampleVec(i))
            if len(subs) <= 1 :
                continue
            for sub in subs :
                if sub in procs :
                    procs.pop(procs.index(i))
                    break

    if not detailed :
        for i in procs :
            subs = list(ROOT.PSL.ConvertSampleToSampleVec(i))
            if len(subs) <= 1 :
                continue
            for sub in subs :
                if sub in procs :
                    procs.pop(procs.index(sub))
                    
    return procs

def GetListOfVariablesFromFile(f,key) :
    vars = []
    varlist = list(ROOT.PSL.ConvertVarToStr(x) for x in ROOT.PSL.GetAllVariables())
    for i in f.Get('PassEvent_%s'%(key)).GetListOfKeys() :
        for j in varlist :
            if j in i.GetName() and j not in vars :
                vars.append(j)
    return vars

#-------------------------------------------------------------------------
def SaveConfigInOutputDirectory(fileloc,outputdir) :
    import time
    import os
    while True :
        if not os.path.exists(outputdir) :
            time.sleep(2)
            continue
        os.system('cp %s %s/.'%(fileloc,outputdir))
        break
    return

#-------------------------------------------------------------------------
def GetHWWColors() :
    import ROOT
    colors_dict = {'ggww' :ROOT.kBlue-8
                   ,'qqww':ROOT.kBlue-9
                   ,'smww':ROOT.kBlue-9
                   ,'wz'  :ROOT.kMagenta-3
                   ,'zz'  :ROOT.kMagenta-2
                   ,'wzzz':ROOT.kMagenta-3
                   ,'singletop':ROOT.kYellow+2
                   ,'ttbar':ROOT.kYellow+1
                   ,'vvv' :220
                   ,'upsl' :ROOT.kGreen-1
                   ,'jpsi' :ROOT.kGreen-2
                   ,'zjet' :ROOT.kGreen-3
                   ,'zjee' :ROOT.kGreen-4
                   ,'zjmm' :ROOT.kGreen-5
                   ,'zjtt' :ROOT.kGreen-6
                   ,'zgam' :ROOT.kOrange-3
                   ,'wgam' :ROOT.kOrange
                   ,'wjet' :ROOT.kCyan-9
                   ,'wje'  :ROOT.kCyan-8
                   ,'wjm'  :ROOT.kCyan-7
                   ,'wjt'  :ROOT.kCyan-5
                   ,'wgvbs':ROOT.kCyan-6
                   ,'dijet':ROOT.kWhite
                   ,'dijetdd':ROOT.kGray
                   ,'higgs':2
                   }
    return colors_dict

#-------------------------------------------------------------------------
def GetSystematicsList() :
    full_list_systs = [
        'EG_RESOLUTION_ALL__1down',
        'EG_RESOLUTION_ALL__1up',
        'EG_SCALE_ALL__1down',
        'EG_SCALE_ALL__1up',
        # WeightSystematics.cxx # 'EL_EFF_ID_TotalCorrUncertainty__1down',
        # WeightSystematics.cxx # 'EL_EFF_ID_TotalCorrUncertainty__1up',
        # WeightSystematics.cxx # 'EL_EFF_Iso_TotalCorrUncertainty__1down',
        # WeightSystematics.cxx # 'EL_EFF_Iso_TotalCorrUncertainty__1up',
        # WeightSystematics.cxx # 'EL_EFF_Reco_TotalCorrUncertainty__1down',
        # WeightSystematics.cxx # 'EL_EFF_Reco_TotalCorrUncertainty__1up',
        # WeightSystematics.cxx # 'EL_EFF_Trigger_TotalCorrUncertainty__1down',
        # WeightSystematics.cxx # 'EL_EFF_Trigger_TotalCorrUncertainty__1up',
        # WeightSystematics.cxx # 'EL_EFF_TriggerEff_TotalCorrUncertainty__1down',
        # WeightSystematics.cxx # 'EL_EFF_TriggerEff_TotalCorrUncertainty__1up',
        'JET_BJES_Response__1up',
        'JET_BJES_Response__1down',
        'JET_EffectiveNP_1__1up',
        'JET_EffectiveNP_1__1down',
        'JET_EffectiveNP_2__1up',
        'JET_EffectiveNP_2__1down',
        'JET_EffectiveNP_3__1up',
        'JET_EffectiveNP_3__1down',
        'JET_EffectiveNP_4__1up',
        'JET_EffectiveNP_4__1down',
        'JET_EffectiveNP_5__1up',
        'JET_EffectiveNP_5__1down',
        'JET_EffectiveNP_6restTerm__1up',
        'JET_EffectiveNP_6restTerm__1down',
        'JET_EtaIntercalibration_Modelling__1up',
        'JET_EtaIntercalibration_Modelling__1down',
        'JET_EtaIntercalibration_TotalStat__1up',
        'JET_EtaIntercalibration_TotalStat__1down',
        'JET_Flavor_Composition__1up',
        'JET_Flavor_Composition__1down',
        'JET_Flavor_Response__1up',
        'JET_Flavor_Response__1down',
        'JET_GroupedNP_1__1up',
        'JET_GroupedNP_1__1down',
        'JET_JER_SINGLE_NP__1up',
        'JET_Pileup_OffsetMu__1up',
        'JET_Pileup_OffsetMu__1down',
        'JET_Pileup_OffsetNPV__1up',
        'JET_Pileup_OffsetNPV__1down',
        'JET_Pileup_PtTerm__1up',
        'JET_Pileup_PtTerm__1down',
        'JET_Pileup_RhoTopology__1up',
        'JET_Pileup_RhoTopology__1down',
        'JET_PunchThrough_MC15__1up',
        'JET_PunchThrough_MC15__1down',
        'JET_SingleParticle_HighPt__1up',
        'JET_SingleParticle_HighPt__1down',
        'MET_SoftTrk_ResoPara',
        'MET_SoftTrk_ResoPerp',
        'MET_SoftTrk_ScaleDown',
        'MET_SoftTrk_ScaleUp',
        'MUONS_ID__1down',
        'MUONS_ID__1up',
        'MUONS_MS__1down',
        'MUONS_MS__1up',
        'MUONS_SCALE__1down',
        'MUONS_SCALE__1up',
        # WeightSystematics.cxx # 'MUON_EFF_STAT__1down',
        # WeightSystematics.cxx # 'MUON_EFF_STAT__1up',
        # WeightSystematics.cxx # 'MUON_EFF_STAT_LOWPT__1down',
        # WeightSystematics.cxx # 'MUON_EFF_STAT_LOWPT__1up',
        # WeightSystematics.cxx # 'MUON_EFF_SYS__1down',
        # WeightSystematics.cxx # 'MUON_EFF_SYS__1up',
        # WeightSystematics.cxx # 'MUON_EFF_SYS_LOWPT__1down',
        # WeightSystematics.cxx # 'MUON_EFF_SYS_LOWPT__1up',
        # WeightSystematics.cxx # 'MUON_EFF_TrigStatUncertainty__1down',
        # WeightSystematics.cxx # 'MUON_EFF_TrigStatUncertainty__1up',
        # WeightSystematics.cxx # 'MUON_EFF_TrigSystUncertainty__1down',
        # WeightSystematics.cxx # 'MUON_EFF_TrigSystUncertainty__1up',
        # WeightSystematics.cxx # 'MUON_ISO_STAT__1down',
        # WeightSystematics.cxx # 'MUON_ISO_STAT__1up',
        # WeightSystematics.cxx # 'MUON_ISO_SYS__1down',
        # WeightSystematics.cxx # 'MUON_ISO_SYS__1up',
        # WeightSystematics.cxx # 'PRW_DATASF__1down',
        # WeightSystematics.cxx # 'PRW_DATASF__1up',
        'NOMINAL', # PLEASE KEEP NOMINAL LAST!!!
        ]
    return full_list_systs

#-------------------------------------------------------------------------
