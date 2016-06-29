#!/bin/usr/env python
import os,sys,time
import pennSoftLepton.PyHelpers as pyhelpers
import ROOT
import re

#
# This file finds datasets on the SRM and makes a file list (including sumw)
# - Finds datasets that are on the SRM
# - If the dataset is incomplete, the item is preceded with a #
# - If the dataset is missing, a \n is put in its place
# - Saves a file including: RunNumber Dataset sumw nevts
#

def GetInHMS(seconds):
    seconds = int(seconds)
    hours = seconds / 3600
    seconds -= 3600*hours
    minutes = seconds / 60
    seconds -= 60*minutes
    return "%02d:%02d:%02d" % (hours, minutes, seconds)

def AddToMap(srm_files,runNumber,sim_tag,derivation) :

    if 'a' in sim_tag :
        if 'mc15c' in sim_tag :
            sim_tag = 'amc15c' # atlfast
        else :
            sim_tag = 'a' # atlfast
    elif 's' in sim_tag :
        if 'mc15c' in sim_tag :
            sim_tag = 'smc15c' # fullsim
        else :
            sim_tag = 's' # fullsim
    else :
        sim_tag = 'd' # data

    print runNumber,sim_tag
    text = ''
    for fname in srm_files :
        sumw = ''
        nevts = ''
        fname = fname.replace('srm://srm.hep.upenn.edu/','root://hn.at3f/')
        f = pyhelpers.GetFile(fname,fatal=False)
        if f.IsZombie() :
            print '## Error: failed to open %s'%(fname)
            continue
        if f.Get(runNumber) : # my d3pd skimming scheme
            sumw  = '%2.6f'%f.Get(runNumber).GetBinContent(2)
            nevts = '%d'%f.Get(runNumber).GetBinContent(3)
        elif f.Get("MetaData") and sim_tag!='d': # DxAOD metadata
            #tmp = ROOT.PSL.GetDerivationSkimOriginalSumw(f) # this was used for Rel19 derivations
            tmp = ROOT.PSL.GetDerivationWeightInfo(f,derivation+'Kernel') # effective starting with Rel20 derivations
            sumw  = '%2.6f'%tmp[0] # the original sumw
            nevts = '%d'%tmp[1] # nevents in this skim
            #
            # For dijet samples override sumw and use nevents instead.
            # See twiki: JetStudies2012#Weighting_scheme_and_MC_quality
            #
            if runNumber in list(str(a) for a in range(147910,147918)+range(361020,361033)) :
                sumw = '%2.6f'%tmp[2] # original nevents
        elif f.Get("CollectionTree") and sim_tag=='d':
            t = f.Get("CollectionTree")
            sumw  = str(0)
            nevts = str(t.GetEntries())
        thetext = '%s  %s  %s    %s %s'%(runNumber.ljust(6),sim_tag,fname.ljust(30),sumw.ljust(20),nevts.ljust(10))
        if sumw == '' and nevts == '':
            thetext = ''

        f.Close()
        if thetext != '': text += thetext+'\n'

    return text

def main(options,args) :

    # Load c++
    pyhelpers.LoadRootCore()

    start_time = time.time()
    myhandler = ROOT.SH.SampleHandler()
    reference_file_name = 'PennSRM_ForReference_%s.txt'%(options.tag)
    reference_file = open(reference_file_name,'w')
    print 'Making Penn SRM map of datasets and sumw with name: %s'%(reference_file_name)

    srm_file_map_name = 'MapSRM_%s.txt'%(options.tag)
    srm_file_map = open(srm_file_map_name,'w')

    testArea = os.environ.get('ROOTCOREBIN').rstrip('/')+'/..'
    fgridfile_name = testArea+'/pennSoftLepton/Grid/GridSamples_%s.txt'%(options.tag)
    #fgridfile_name = testArea+'/pennSoftLepton/Grid/GridSamples_af2.txt'
    fgridfile = open(fgridfile_name).readlines()
    print 'Searching for datasets defined in: %s'%(fgridfile_name)
    
    for f in fgridfile :
        f = f.replace('\n','')
        if not f : continue
        if '#' in f : continue
        if len(f.split('.')) < 2 : continue
        runNumber = f.split('.')[1]
        atlfast = re.findall('_a[0-9][0-9][0-9]',f)
        fullsim = re.findall('_s[0-9][0-9][0-9][0-9]',f)
        isdata = ('data' in f)
        sim_tag = ''
        if len(atlfast) : sim_tag = atlfast[0].replace('_','')+'*'
        if len(fullsim) : sim_tag = fullsim[0].replace('_','')+'*'
        cmd = 'dq2-ls -fpL UPENN_LOCALGROUPDISK user.INSERTUSER.*.%s.*%s%s%s/'%(runNumber,sim_tag,options.tag,options.tail)
        ls_output = ''
        if not options.dxaod :
            ls_output = os.popen(cmd.replace('INSERTUSER','brendlin')).readlines()
            if not ls_output :
                ls_output = os.popen(cmd.replace('INSERTUSER','jmiguens')).readlines()
            if not ls_output :
                ls_output = os.popen(cmd.replace('INSERTUSER','ykeisuke')).readlines()
                
        sim_tag = f.split('DAOD_'+options.derivation)[-1].replace('/','')
        cmd = 'dq2-ls -fpL UPENN_LOCALGROUPDISK mc15_13TeV.%s.*%s*%s/'%(runNumber,options.derivation,sim_tag)
        ls_output = os.popen(cmd).readlines()

        # print '~~~~~~~~~~~~~'
        # print cmd
        # print ls_output
        # print '~~~~~~~~~~~~~'

        #check if there are more than 2 occurrences of the dq2-ls command
        #if so use only the first one: we might want to decide to do something else here
        myctr = 0
        mypos = 0
        for myid,mypt in enumerate(ls_output):
            if ('user.jmiguens:' in mypt) or ('user.ykeisuke:' in mypt) or ('user.brendlin:' in mypt) :
                myctr+=1
            if myctr == 2 :
                mypos = myid
                break
        if myctr > 1 :
            ls_output = ls_output[:mypos-1]
        # print '~~~~~~~~~~~~~'
        # print ls_output
        # print '~~~~~~~~~~~~~'
        #end of check for 2 samples with same dsid

        n_total = -1
        n_local = -2
        ds_name = ''
        srm_files = []
        for i in ls_output :
            i = i.replace('\n','')
            if 'Missing voms proxy' in i :
                print 'Missing voms proxy. voms-proxy-init -voms atlas'
                sys.exit()
            if (runNumber in i) and ('srm' not in i) and ('PFN' not in i) : 
                ds_name = i+'/'
            if 'total files:' in i : 
                n_total = int(i.split()[-1])
            if 'local files:' in i : 
                n_local = int(i.split()[-1])
            if ('srm' in i) and ('PFN' not in i) :
                srm_files += [i]
        if ds_name and (int(bool(len(atlfast))) + int(bool(len(fullsim))) + int(isdata) > 1) :
            print 'Error! dataset %s assigned more than one of (fullsim,atlfast,data)! Exiting.'
            sys.exit()
        if ds_name and (not len(atlfast)) and (not len(fullsim)) and (not isdata) :
            print 'Error! dataset %s was not assigned to fullsim or atlfast or data! Exiting.'
            sys.exit()            
        if n_total == n_local and n_total > 0 and ds_name :
            print ds_name
            reference_file.write(ds_name+'\n')
            srm_file_map.write(AddToMap(srm_files,runNumber,sim_tag,options.derivation))
        elif ds_name and n_local > 0 :
            print '# '+ds_name
            reference_file.write('# %s\n'%(ds_name))
            srm_file_map.write(AddToMap(srm_files,runNumber,sim_tag,options.derivation))
        else :
            print
            reference_file.write('\n')

    reference_file.close()
    srm_file_map.close()

    elapsed_time = GetInHMS(time.time() - start_time)
    print 'elapsed time:',elapsed_time

if __name__ == "__main__":
    from optparse import OptionParser
    p = OptionParser()
    p.add_option('--tag',type='string',default='t002',dest='tag',help='dataset tag')
    # Eventloop appends the name of the output file to its dataset name (in this case "_SKIM.root")
    p.add_option('--tail',type='string',default='_EXT0',dest='tail',help='Thing that goes after tag')
    # dc14 tail: _EXT0. d3pd tail: _SKIM.root
    p.add_option('--derivation',type  ='string',default='SUSY2',dest='derivation',help='name of the derivation' )
    options,args = p.parse_args()

    options.dxaod = False
    if options.derivation:
        options.dxaod = True

    if not os.getenv('ATLAS_LOCAL_DQ2CLIENT_PATH') :
        print 'Error! localSetupDQ2Client and voms-proxy-init!'
        sys.exit()

    main(options,args)

