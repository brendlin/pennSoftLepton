#!/bin/usr/env python
import os,sys,commands
import pennSoftLepton.PyHelpers as pyhelpers
import ROOT
from GetSamplesAndSumwSRM import AddToMap
from SampleNavigator import buildSRM
from SampleNavigator import checkSampleAtPenn
import re
#
# This file is used for making lists of datasets for:
# - running over the grid using all files from given samples
# - Making a list of SRM files for given samples
# What it does:
# - You give it a stream name (e.g. --dataStream Main   or   --samples Main,Muons)
# - You give it a AOD reco rtag (e.g. --rtag r6630_r6264) so it can look for and find the right dataset (it will work even it you don't provide an rtag, but it will find too many datasets)
# - You give it a xAOD ptag (e.g. --ptag p2361) so it can look for and find the right dataset (it will work even it you don't provide a ptag, but it will find too many datasets)
# - You give it the derivation you are interested in (e.g. --derivation SUSY2)
# - You give it a private derivation production tag  (e.g. --pvttag mydata) so it can look for and find the right dataset (it will work even it you don't provide an private tag, but it will find too many datasets)
# - It spits out information and writes a few lists that you can use for other things
#TODO: if period contained get produced like in Run1 this script will need to be updated

def main (options,args) :
    #
    # Load c++
    #
    pyhelpers.LoadRootCore()

    fulltag = ''
    #if options.rtag :
    #    fulltag += '_'+options.rtag
    #if options.ptag :
    #    fulltag += '_'+options.ptag
    if options.pvttag :
        fulltag += '_'+options.pvttag

    #some sample lists
    samplesPenn_name    = 'PennSRM_DataForReference%s.txt'%(fulltag)
    gridSubmission_name = 'GridData%s.txt'%(fulltag)
    duplicates_name     = 'DuplicatedData%s.txt'%(fulltag)
    missing_name        = 'MissingData%s.txt'%(fulltag)
    forDeletion_name    = 'PennSRM_DataForDeletion%s.txt'%(fulltag)

    samplesPenn    = open(samplesPenn_name,"w")
    gridSubmission = open(gridSubmission_name,"w")
    duplicates     = open(duplicates_name,"w")
    missing        = open(missing_name,"w")
    forDeletion    = open(forDeletion_name,"w")
    #replicatePenn  = open("ReplicationSamples%s.txt"%(fulltag),"w")

    coolPennPeople = ['jmiguens','brendlin','ykeisuke']

    dataStreams = []
    for st in options.dataStream.split(',') :
        dataStreams.append(st)
    
    grlFiles = []
    ls_out = commands.getoutput('ls $ROOTCOREBIN/../pennSoftLepton/data/data15_13TeV*.xml')
    if not options.grl:
        for grl in ls_out.split('\n') :
            grlFiles.append(grl)
    #grlFiles.append('../data/data15_13TeV.periodA1_DetStatus-v62-pro17_DQDefects-00-01-02_PHYS_CombinedPerf_Tracking_Tracking.xml')
    #grlFiles.append('../data/data15_13TeV.periodA_DetStatus-v62-pro18_DQDefects-00-01-02_PHYS_StandardGRL_All_Good.xml')
    #grlFiles.append('../data/data15_13TeV.periodB1_DetStatus-v62-pro18_DQDefects-00-01-02_PHYS_StandardGRL_All_Good.xml')
    #grlFiles.append('%s/../pennSoftLepton/data/data15_13TeV.periodAllYear_DetStatus-v63-pro18-01_DQDefects-00-01-02_PHYS_StandardGRL_All_Good.xml'%(os.getenv('ROOTCOREBIN')))
    else:
        grlFiles = options.grl.split(',')


    runs = []

    for grlFile_name in grlFiles:
        print 'Getting info from grl',grlFile_name
        grlFile = open(grlFile_name).readlines()
        for line in grlFile:
            if '"RunList"' in line:
                runlist = line.split('>')[1].split('<')[0].split(',')

        runs+= list(int(a) for a in runlist)

    runs = list(set(runs))
    print runs

    ptags = options.ptag.split(',')
    rtags = options.rtag.split(',')

    for run in sorted(runs):
        for stream in dataStreams:
            run = str(run)
            print '\n'+run+' '+stream+':\n'

            #look for an official derivation first
            offderivs = ''
            for rtag in rtags:
                for ptag in ptags:
                    cmd = 'dq2-ls data15_13TeV.00'+run+'.physics_'+stream+'.merge.DAOD_'+options.derivation+'.*'+rtag+'*'+ptag+'*/ | sort -r'
                    print cmd
                    offderivs = commands.getoutput(cmd)
                    if offderivs != "" : break
                if offderivs != "" : break

            #found official derivation
            if offderivs != "":

                #found more than 1 matching datasets
                offderiv = offderivs.split('\n')
                if len(offderiv)>1:
                    print '\tFound more than one derivation for run '+run+' and stream '+stream+'. Will continue with the first derivation found. You can see the full list of derivations under '+duplicates.name+'.'
                    for dupoffderiv in offderiv:
                        duplicates.write(dupoffderiv+'\n')

                offderivPenn = checkSampleAtPenn(offderiv[0])

                #derivation is complete at Penn
                if offderivPenn is True:
                    print '\tFound official derivation complete at Penn \n \t'+offderiv[0]
                    samplesPenn.write(offderiv[0].split(":")[1]+'\n')

                    #TODO add part where if there is a matching private production we can delete it

                #derivation is incomplete or not at Penn
                else:
                    print '\tWill request replication of official derivation to Penn \n \t'+offderiv[0]
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
                        pvtderivs = commands.getoutput('dq2-ls user.'+username+'.data15_13TeV.00'+run+'.physics_'+stream+'.*DxAOD.'+options.derivation+'.*'+rtag+'*'+options.pvttag+'_EXT0/ | sort -r')
                        if pvtderivs != "" : break
                    if pvtderivs != "" : break

                #found private derivation    
                if pvtderivs != "":

                    #found more than 1 private derivation
                    pvtderiv = pvtderivs.split('\n')
                    if len(pvtderiv)>1:
                        print '\tFound more than one private derivation for run '+run+' and stream '+stream+'. Will continue with the first derivation found. You can see the full list of derivations under '+duplicates.name+'.'
                        for duppvtderiv in pvtderiv:
                            duplicates.write(duppvtderiv)

                    #TODO: maybe good idea if the private derivation is complete? if not resubmit the job?

                    pvtderivPenn = checkSampleAtPenn(pvtderiv[0])

                    #derivation is complete at Penn
                    if pvtderivPenn is True:
                        print '\tFound private derivation complete at Penn \n \t'+pvtderiv[0]
                        samplesPenn.write(pvtderiv[0].split(":")[1]+'\n')
                    
                    #derivation is incomplete or not at Penn
                    else:
                        print run+'\tWill request replication of private derivation to Penn\n \t'+pvtderiv[0]
                        #replicatePenn.write(pvtderiv[0].split(":")[1]+'\n')
                        #os.system('dq2-register-subscription '+pvtderiv[0].split(":")[1].replace("/","")+' UPENN_LOCALGROUPDISK')
                        cmd = 'rucio add-rule '+pvtderiv[0].replace("/","")+' --grouping DATASET 1 "UPENN_LOCALGROUPDISK"'
                        if options.dryrun :
                            print cmd
                        else :
                            os.system(cmd)

                #couldn't find private derivation
                else:
                    # check for MERGE first:
                    offaods = ''
                    for rtag in rtags:
                        cmd = 'dq2-ls data15_13TeV.00'+run+'.physics_'+stream+'*merge.AOD*'+rtag+'* | sort -r'
                        print cmd
                        offaods = commands.getoutput(cmd)
                        if offaods != "" : break
                    
                    #if not offaods :
                        # MERGE does not exist, so check for any AOD
                        #cmd = 'dq2-ls data15_13TeV.00'+run+'.physics_'+stream+'*merge.AOD*'+options.rtag+'* | grep -v _tid | sort -r'
                        #print cmd
                        #offaods = commands.getoutput(cmd)

                    #found AOD
                    if offaods != "":
                        offaod = offaods.split('\n')

                        #found more than 1 AOD
                        if len(offaod)>1:
                            print '\tFound more than one AOD for run '+run+' and stream '+stream+'. Will continue with the first AOD found. You can see the full list of AOD under '+duplicates.name+'.'
                            for aoddup in offaod:
                                duplicates.write(aoddup+'\n')
                        
                        #write AOD to grid submission file
                        print '\tFound AOD \n \t'+offaod[0]
                        gridSubmission.write(offaod[0].split(":")[1]+'\n')

                    #couldn't find anything for this ID, just write it in a file for completeness
                    else:
                        print '\tCould not find any datasets'
                        missing.write(run+'\t'+stream+'\n')

    #replicatePenn.close()
    samplesPenn.close()   
    gridSubmission.close()
    duplicates.close()    
    missing.close()
    forDeletion.close()

    if options.buildSRMmap :
        srm_file_map_name = 'MapSRM_data%s.txt'%(fulltag)
        srm_file_map = open(srm_file_map_name,'w')
        print '\nMaking Penn SRM map of datasets and sumw with name: %s\n'%(srm_file_map_name)
        buildSRM(samplesPenn,srm_file_map)
        srm_file_map.close()
        samplesPenn.close()

if __name__ == "__main__":
    from optparse import OptionParser
    p = OptionParser()
    p.add_option('--dataStream',type  ='string',default='Main',dest='dataStream',help='name of the data stream' )
    p.add_option('--derivation',type  ='string',default='SUSY2',dest='derivation',help='name of the derivation' )
    p.add_option('--ptag',type  ='string',default='',dest='ptag',help='derivation p-tag')
    p.add_option('--rtag',type  ='string',default='',dest='rtag',help='reco r-tag')
    p.add_option('--pvttag',type  ='string',default='',dest='pvttag',help='private production tag')
    p.add_option('--grl',type  ='string',default='',dest='grl',help='GRL xml file')
    p.add_option('--buildSRMmap',action='store_true',default=False  ,dest='buildSRMmap',help='build SRM map from samples at Penn (takes a loooooong time)' )
    p.add_option('--dryrun'     ,action='store_true',default=False  ,dest='dryrun',help='do a dry run (do not submit rules)' )
    options,args = p.parse_args()
    main(options,args)
