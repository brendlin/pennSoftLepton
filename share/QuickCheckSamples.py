#!/bin/usr/env python
import os,sys
import pennSoftLepton.PyHelpers as pyhelpers
import ROOT
#
# This file is used for printing out sample information.
# --samples ttbar,zjet,wjet
# --config GridSamples_mc15.conf (where the samples are defined)
# --dsfile /home/mjoana/PennSRM_ForReference_mc15_25ns_ew03.txt
# What it does:
# - You give it a sample name (--sample)
# - You give it a conf file (--config) where the samples are defined
# - You give it a dsfile (--dsfile) where the dataset names are
# - It spits out information.

def main (options,args) :
    #
    # Load c++
    #
    print 'Loading c++...'
    ROOT.gROOT.ProcessLine (".x $ROOTCOREDIR/scripts/load_packages.C")
    ROOT.PSL.EDM # this needs to be here for some reason. do not remove.

    #
    # The following looks in samples.h and expands any compound samples
    #
    sampleslist = []
    for i in options.samples.split(',') :
        tmp = list(ROOT.PSL.ConvertSampleToSampleVec(i))
        sampleslist += tmp

    print sampleslist

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
    fdsets_name = testArea+'/pennSoftLepton/Grid/%s'%(options.dsfile)
    if options.dsfile[0] == '/' :
        fdsets_name = options.dsfile
    if not os.path.exists(fdsets_name) :
        print 'Error! %s does not exist. Exiting.'%(fdsets_name)
        return
    fdsets = open(fdsets_name).readlines()
    print 'Datasets are picked up from %s'%(fdsets_name)

    grid_out = ''

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

            if len(parts) < 6 :
                print 'Skipping run',parts[0],'since it is too small.'
                continue

            if parts[5] in Sample_list_used[i] :
                partstext = parts[0].ljust(10)+parts[3].ljust(18)
                partstext += parts[4].ljust(28)+parts[5].ljust(20)
                if ''.join(fdsets).count(parts[0]) > 1 :
                    print 'duplicates for',parts[0]
                printed = False
                for l2 in fdsets :
                    if parts[0] in l2 :
                        print partstext,
                        print l2.rstrip('\n')
                        grid_out += l2.replace(' ','').replace('#','')
                        printed = True
                        fdsets.pop(fdsets.index(l2))
                        break
                if not printed : print partstext

    if options.p :
        print grid_out

    if options.unused :
        print
        print 'UNUSED SAMPLES:'
        print
        fdsets = ''.join(fdsets)
        fdsets = fdsets.replace(' ','')
        fdsets = fdsets.replace('\n\n','\n')
        print fdsets


if __name__ == "__main__":
    from optparse import OptionParser
    p = OptionParser()
    p.add_option('--samples',type  ='string',default='',dest='samples',help='input sample names (csv)')
    p.add_option('--config',type  ='string',default='DefaultConfiguration.conf',dest='config',help='config file for where samples are defined' )
    p.add_option('--dsfile',type  ='string',default='orig_GridSamples.txt',dest='dsfile',help='File containing the dataset names' )
    p.add_option('-p','--p',action='store_true',default=False  ,dest='p',help='Print samples (for making a list to send to the grid)' )
    p.add_option('--unused',action='store_true',default=False  ,dest='unused',help='Print the unused samples in the ds file (because maybe they are obsolete and you want to remove them)' )
    options,args = p.parse_args()
    main(options,args)
