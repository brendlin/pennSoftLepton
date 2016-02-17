#!/bin/usr/env python
import os,sys,commands
import pennSoftLepton.PyHelpers as pyhelpers
import ROOT
from GetSamplesAndSumwSRM import AddToMap
import re
#
# This file is used for making lists of datasets for:
# - running over the grid using all files from given samples
# - Making a list of SRM files for given samples
# What it does:
# - You give it a sample name (e.g. --samples dijet   or   --samples dijet,ttbar   or   --samples all)
# - You give it a conf file (e.g. --config DefaultConfiguration_mc15.conf) where the samples are defined (useful to have one with *all* the samples, that you wouldn't be able to use for running the actual code)
# - You give it a AOD reco rtag (e.g. --rtag r6630_r6264) so it can look for and find the right dataset (it will work even it you don't provide an rtag, but it will find too many datasets)
# - You give it a xAOD ptag (e.g. --ptag p2361) so it can look for and find the right dataset (it will work even it you don't provide a ptag, but it will find too many datasets)
# - You give it a private derivation production tag (e.g. --pvttag mc15a_ew02) so it can look for and find the right dataset (it will work even it you don't provide an private tag, but it will find too many datasets)
# - It spits out information and writes a few lists that you can use for other things
#TODO merge with the actual mapping part to reduce number of dq-ls calls

def buildSRM(inputFile,outputFile,derivation):

    samplesAtPenn = open(inputFile.name).readlines()
        
    for f in samplesAtPenn:

        f = f.replace('\n','')
        if not f : continue
        if '#' in f : continue
        if len(f.split('.')) < 2 : continue
        runNumber = ''
        if 'data' in f:
            runNumber = f.split('data15_13TeV.')[1].split('.')[0]
        else:
            runNumber = f.split('mc15_13TeV.')[1].split('.')[0]
        atlfast = re.findall('a[0-9][0-9][0-9]',f)
        fullsim = re.findall('s[0-9][0-9][0-9][0-9]',f)
        isdata = ('data' in f)
        sim_tag = ''
        if len(atlfast) : sim_tag = atlfast[0].replace('_','')+'*'
        if len(fullsim) : sim_tag = fullsim[0].replace('_','')+'*'

        cmd = 'dq2-ls -fpL UPENN_LOCALGROUPDISK '+f
        ls_output = os.popen(cmd).readlines()

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
            outputFile.write(AddToMap(srm_files,runNumber,sim_tag,derivation))
        elif ds_name and n_local > 0 :
            print '# '+ds_name
            outputFile.write(AddToMap(srm_files,runNumber,sim_tag,derivation))
        else :
            print

    return

def checkSampleAtPenn(checksample):

    isatpenn = False

    #look for replicas, dq2 prints a summary table which is nicer to look at than rucio
    #replica = commands.getoutput('rucio list-dataset-replicas '+checksample.replace('/',''))
    replica = commands.getoutput('dq2-list-dataset-replicas-container '+checksample)

    #looking for that nice table
    if 'Summary:' in replica:
        replica = replica.split('Summary:')[1]

        #derivation is at Penn
        if 'UPENN_LOCALGROUPDISK' in replica:

            #splitting the rows of the table and looking for the Penn row
            for pennrep in replica.split('\n'):
                if 'UPENN_LOCALGROUPDISK' in pennrep:

                    #splitting the columns of the table
                    pennrep = pennrep.split()
                
                    #derivation is complete at Penn
                    if pennrep[1]==pennrep[3] : isatpenn = True

                    #derivation incomplete
                    else : isatpenn = False
                        
        #derivation not at penn
        else : isatpenn = False

    #can't understand output of the command
    else : isatpenn = False

    return isatpenn    

def main (options,args) :
    #
    # Load c++
    #
    print 'Loading c++...'
    ROOT.gROOT.ProcessLine (".x $ROOTCOREDIR/scripts/load_packages.C")
    ROOT.PSL.EDM # this needs to be here for some reason. do not remove.

    #some sample lists
    samplesPenn_name    = 'PennSRM_ForReference_%s.txt'%(options.pvttag)
    gridSubmission_name = 'GridSamples_%s.txt'%(options.pvttag)
    duplicates_name     = 'DuplicatedSamples_%s.txt'%(options.pvttag)
    missing_name        = 'MissingSamples_%s.txt'%(options.pvttag)
    forDeletion_name    = 'PennSRM_ForDeletion_%s.txt'%(options.pvttag)

    samplesPenn    = open(samplesPenn_name,"w")
    gridSubmission = open(gridSubmission_name,"w")
    duplicates     = open(duplicates_name,"w")
    missing        = open(missing_name,"w")
    forDeletion    = open(forDeletion_name,"w")
    #replicatePenn     = open("ReplicationSamples.txt","w")

    coolPennPeople = ['jmiguens','brendlin','ykeisuke']

    ptags = options.ptag.split(',')
    rtags = options.rtag.split(',')

    #
    # The following looks in samples.h and expands any compound samples
    #
    sampleslist = []
    for i in options.samples.split(',') :
        tmp = list(ROOT.PSL.ConvertSampleToSampleVec(ROOT.PSL.ConvertToSample(i)))
        for t in tmp : 
            sampleslist.append(ROOT.PSL.ConvertSampleToStr(t))

    confname = '%s/../pennSoftLepton/config/%s'%(os.getenv('ROOTCOREBIN'),options.config)
    if not os.path.exists(confname) :
        print 'Error! %s does not exist. Exiting.'%(confname)
        return
    print 'Picking up sample definitions from conf file %s'%(confname)
    env = ROOT.TEnv(confname)

    testArea = os.environ.get('ROOTCOREBIN').rstrip('/')+'/..'
    xsecPath = testArea+'/pennSoftLepton/config/Samples.txt'
    print 'Samples are defined in file %s'%(xsecPath)

    if not os.path.exists(xsecPath) :
        print 'Error! %s does not exist. Exiting.'%(xsecPath)
        return
    fxsec = open(xsecPath, 'r').readlines()

    #I think we don't want this bit because we want to build our own files
    #fdsets_name = testArea+'/pennSoftLepton/Grid/%s'%(options.dsfile)
    #if not os.path.exists(fdsets_name) :
    #    print 'Error! %s does not exist. Exiting.'%(fdsets_name)
    #    return
    #fdsets = open(fdsets_name).readlines()
    #print 'Datasets are picked up from %s'%(fdsets_name)
    #I think we don't want this bit because we want to build our own files

    #grid_out = ''

    Sample_list_used = dict()
    for i in env.GetTable() :
        name = i.GetName()
        if name[:7] == 'Sample.' :
            values = env.GetValue(name,'').split()
            name = name[7:]
            if sampleslist and name not in sampleslist : continue
            Sample_list_used[name] = []
            for v in values :
                Sample_list_used[name].append(v)

    #loop the samples in the configuration file
    for i in sorted(Sample_list_used.keys()) :
        print
        print i,':',Sample_list_used[i]
        print
        for line in fxsec :
            parts = line.rstrip('\n')
            if not parts : continue
            if '#' in parts : continue
            parts = parts.split()
            if not parts : continue

            #require that the dsid has 6 digits
            if len(parts) < 6 :
                print 'Skipping run',parts[0],'since it is too small.'
                continue
            
            #if a sample in Sample.txt is in the config file look for the dataset
            if parts[5] in Sample_list_used[i] :

                #look for the official derivation first
                offderivs = ''
                found = False
                for rtag in rtags:
                    for ptag in ptags:
                        cmd = 'dq2-ls mc15_13TeV.'+parts[0]+'.*.merge.DAOD_'+options.derivation+'.*'+rtag+'*'+ptag+'*/ | sort -r'
                        offderivs = commands.getoutput(cmd)
                        if offderivs != "" : break
                    if offderivs != "" : break
                
                #found official derivation
                if offderivs != "":

                    #found more than 1 matching datasets
                    offderiv = offderivs.split('\n')
                    if len(offderiv)>1:
                        print 'Found more than one derivation for sample ID '+parts[0]+'. Will continue with the first derivation found. You can see the full list of derivations for this ID under '+duplicates.name+'.'
                        for dupoffderiv in offderiv:
                            duplicates.write(dupoffderiv)

                    offderivPenn = checkSampleAtPenn(offderiv[0])

                    #derivation is complete at Penn
                    if offderivPenn is True:
                        print parts[0]+' ('+parts[5]+')'+' found official derivation complete at Penn \n \t'+offderiv[0]
                        samplesPenn.write(offderiv[0].split(":")[1]+'\n')

                        #let's also delete any matching private derivation
                        #tmprun=offderiv[0].split('mc15_13TeV.')[1].split('.')[0]
                        tmptag=offderiv[0].split('DAOD_'+options.derivation+'.')[1].split('_p')[0][6:]
                        for user in coolPennPeople:
                            delpvt = 'user.'+user+'.mc15_13TeV.'+parts[0]+'.*.DxAOD.'+options.derivation+'.'+tmptag+'_'+options.pvttag+'_EXT0/'
                            #print 'this is what a corresponding private production would look like '+delpvt
                            delpvtls = commands.getoutput('dq2-ls '+delpvt+' | sort -r')
                            delpvtrep = commands.getoutput('dq2-list-dataset-replicas-container '+delpvt+' | sort -r')
                            if delpvtls != "" and 'UPENN_LOCALGROUPDISK' in delpvtrep:
                                forDeletion.write(delpvtls)
                            else : continue

                    #derivation is incomplete or not at Penn
                    else:
                        print parts[0]+' ('+parts[5]+')'+' will request replication to Penn of official derivation \n \t'+offderiv[0]
                        #replicatePenn.write(offderiv[0].split(":")[1]+'\n')                   
                        cmd = 'rucio add-rule '+offderiv[0].replace("/","")+' --grouping DATASET 1 "UPENN_LOCALGROUPDISK"'
                        if options.dryrun :
                            print cmd
                        else :
                            os.system(cmd)
                        #os.system('dq2-register-subscription '+offderiv[0].split(":")[1].replace("/","")+' UPENN_LOCALGROUPDISK')

                #didn't find official derivation, will look for private derivation
                else:
                    pvtderivs = ''
                    for username in coolPennPeople:
                        for rtag in rtags:
                            cmd = 'dq2-ls user.'+username+'.mc15_13TeV.'+parts[0]+'.*DxAOD.'+options.derivation+'.*'+rtag+'*'+options.pvttag+'_EXT0/ | sort -r'
                            pvtderivs = commands.getoutput(cmd)
                            if pvtderivs != "" : break
                        if pvtderivs != "" : break

                    #found private derivation    
                    if pvtderivs != "":

                        #found more than 1 private derivation
                        pvtderiv = pvtderivs.split('\n')
                        if len(pvtderiv)>1:
                            print 'Found more than one private derivation for sample ID '+parts[0]+'. Will continue with the first derivation found. You can see the full list of derivations for this ID under '+duplicates.name+'.'
                            for duppvtderiv in pvtderiv:
                                duplicates.write(duppvtderiv)

                        #TODO: maybe good idea if the private derivation is complete? if not resubmit the job?

                        pvtderivPenn = checkSampleAtPenn(pvtderiv[0])

                        #derivation is complete at Penn
                        if pvtderivPenn is True:
                            print parts[0]+' ('+parts[5]+')'+' found private derivation complete at Penn \n \t'+pvtderiv[0]
                            samplesPenn.write(pvtderiv[0].split(":")[1]+'\n')
                        
                        #derivation is incomplete or not at Penn
                        else:
                            print parts[0]+' ('+parts[5]+')'+' will request replication to Penn of private derivation \n \t'+pvtderiv[0]
                            #replicatePenn.write(pvtderiv[0].split(":")[1]+'\n')
                            #os.system('dq2-register-subscription '+pvtderiv[0].split(":")[1].replace("/","")+' UPENN_LOCALGROUPDISK')
                            cmd = 'rucio add-rule '+pvtderiv[0].replace("/","")+' --grouping DATASET 1 "UPENN_LOCALGROUPDISK"'
                            if options.dryrun :
                                print cmd
                            else :
                                os.system(cmd)

                    #couldn't find private derivation
                    else:
                        offaods = ''
                        for rtag in rtags:
                            cmd = 'dq2-ls mc15_13TeV.'+parts[0]+'.*.merge.AOD*'+rtag+'*/ | sort -r'
                            offaods = commands.getoutput(cmd)
                            if offaods != "" : break

                        #found AOD
                        if offaods != "":
                            offaod = offaods.split('\n')

                            #found more than 1 AOD
                            if len(offaod)>1:
                                print 'Found more than one AOD for sample ID '+parts[0]+'. Will continue with the first AOD found. You can see the full list of AOD for this ID under '+duplicates.name+'.'
                                for aoddup in offaod:
                                    duplicates.write(aoddup)
                            
                            #write AOD to grid submission file
                            print parts[0]+' ('+parts[5]+')'+' found AOD \n \t'+offaod[0]
                            gridSubmission.write(offaod[0].split(":")[1]+'\n')

                        #couldn't find anything for this ID, just write it in a file for completeness
                        else:
                            print parts[0]+' ('+parts[5]+')'+' could not find any datasets'
                            missing.write(parts[0]+'\n')

                #partstext = parts[0].ljust(10)+parts[3].ljust(18)
                #partstext += parts[4].ljust(28)+parts[5].ljust(20)
                #print partstext
                ###### for each line in the grid file which i don't think we want
                #if ''.join(fdsets).count(parts[0]) > 1 : 
                #    print 'duplicates for',parts[0]
                #printed = False
                #for l2 in fdsets :
                #    if parts[0] in l2 :
                #        print partstext,
                #        print l2.rstrip('\n')
                #        grid_out += l2.replace(' ','').replace('#','')
                #        printed = True
                #        fdsets.pop(fdsets.index(l2))
                #        break
                ####### for each line in the grid file which i don't think we want
                #if not printed : print partstext

    #if options.p :
        #print grid_out

    #if options.unused :
        #print
        #print 'UNUSED SAMPLES:'
        #print
        #fdsets = ''.join(fdsets)
        #fdsets = fdsets.replace(' ','')
        #fdsets = fdsets.replace('\n\n','\n')
        #print fdsets

    #replicatePenn.close()
    samplesPenn.close()   
    gridSubmission.close()
    duplicates.close()    
    missing.close()
    forDeletion.close()

    if options.buildSRMmap :
        srm_file_map_name = 'MapSRM_%s.txt'%(options.pvttag)
        srm_file_map = open(srm_file_map_name,'w')
        print '\nMaking Penn SRM map of datasets and sumw with name: %s\n'%(srm_file_map_name)
        buildSRM(samplesPenn,srm_file_map,options.derivation)
        srm_file_map.close()
        samplesPenn.close()

if __name__ == "__main__":
    from optparse import OptionParser
    p = OptionParser()
    p.add_option('--samples',type  ='string',default='',dest='samples',help='input sample names (csv)')
    p.add_option('--config',type  ='string',default='DefaultConfiguration.conf',dest='config',help='config file for where samples are defined' )
    #p.add_option('--dsfile',type  ='string',default='orig_GridSamples.txt',dest='dsfile',help='File containing the dataset names' )
    #p.add_option('-p','--p',action='store_true',default=False  ,dest='p',help='Print samples (for making a list to send to the grid)' )
    #p.add_option('--unused',action='store_true',default=False  ,dest='unused',help='Print the unused samples in the ds file (because maybe they are obsolete and you want to remove them)' )
    p.add_option('--derivation',type  ='string',default='SUSY2',dest='derivation',help='name of the derivation' )
    p.add_option('--ptag',type  ='string',default='',dest='ptag',help='derivation p-tag')
    p.add_option('--rtag',type  ='string',default='',dest='rtag',help='reco r-tag')
    p.add_option('--pvttag',type  ='string',default='',dest='pvttag',help='private production tag')
    p.add_option('--buildSRMmap',action='store_true',default=False  ,dest='buildSRMmap',help='build SRM map from samples at Penn (takes a loooooong time)' )
    p.add_option('--dryrun'     ,action='store_true',default=False  ,dest='dryrun',help='do a dry run (do not submit rules)' )
    options,args = p.parse_args()
    main(options,args)
