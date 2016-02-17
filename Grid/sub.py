#!/usr/bin/env python
from SampleNaming import ShortenDatasetName
import sys
import os

def main(options) :

    f = open(options.file)
    for line in f.readlines():
        input_ds = line.strip()
        if len(input_ds)<5: continue
        if input_ds.find('#')==0: continue
        tmp = input_ds.replace('.merge.AOD','.DxAOD.%s'%(options.reductionConf))
        tmp = tmp.replace('/','')
        output_ds = 'user.%s.%s_%s'%(options.user,tmp,options.tag)
        output_ds = output_ds.replace('/','')
        output_ds = ShortenDatasetName(output_ds)
        #rootoutfile = tmp.replace('.DxAOD.SUSY5','')+skimversion+'.root'
        #rootoutfile = (user+input_ds.split('.')[1]+'.DxAOD.'+skimversion+'.root').replace('..','.')
        rootoutfile = (input_ds.split('.')[1]+'_'+options.tag+'.root').replace('..','.')
        print " * input_ds:", input_ds
        print " * output_ds:",output_ds
        print " * root file:",rootoutfile
        if len(rootoutfile)>150:
            rootoutfile = tmp+'_'+options.tag+'.root'
            if len(rootoutfile)>150:
                rootoutfile = tmp+'.root'
            print 'output root file too long (150 characters maximum). will reduce it to:',
            pass

        # put together skimming command
        cmd = 'pathena --useNewTRF --trf "Reco_tf.py '
        cmd+= ' --inputAODFile %IN'
        cmd+= ' --outputDAODFile "'+rootoutfile+'"'
        cmd+= ' --reductionConf %s'%(options.reductionConf)
        cmd+= ' --ignoreErrors True'
        cmd+= '"' # the " ends the --trf block
        cmd+= ' --noBuild'
        cmd+= ' --extOutFile DAOD_%s.%s' % (options.reductionConf,rootoutfile)
        if options.excludedSite :
            cmd+= ' --excludedSite=%s' % (options.excludedSite)
        cmd+= ' --outDS='+output_ds
        cmd+= ' --inDS='+input_ds
        cmd+= ' --nGBPerJob=10'
        #cmd+= ' --mergeOutput'
        if options.destSE :
            cmd+= ' --destSE='+options.destSE
        if options.verbose :
            print cmd
        if options.submit : 
            os.system( cmd )
            
    return

if __name__ == "__main__":
    from optparse import OptionParser
    p = OptionParser()
    
    p.add_option('-f','--file',type='string',default='',dest='file',help='tree name' )
    p.add_option('--tag'      ,type='string',default='',dest='tag',help='dataset tag' )
    p.add_option('--user'     ,type='string',default='',dest='user',help='dataset tag' )
    p.add_option('--susy2',action='store_true',default=False,dest='susy2',help='For official SUSY2')
    p.add_option('--susy5',action='store_true',default=False,dest='susy5',help='For official SUSY5')
    p.add_option('--destSE',type='string',default='UPENN_LOCALGROUPDISK',dest='destSE',help='destSE (double-quotes for none)')
    p.add_option('--excludedSite',type='string',default='',dest='excludedSite',help='excludedSite (csv)')
    p.add_option('-v','--verbose',action='store_true',default=False,dest='verbose',help='print commands')
    p.add_option('--submit',action='store_true',default=False,dest='submit',help='submit')

    options,args = p.parse_args()

    if not options.file :
        print 'Error! Please specify in input file! (like --file GridSamples_mc15_50ns_ew01.txt)'
        sys.exit()

    if not options.tag :
        print 'Error! Please specify a tag! (like --tag mc15_50ns_ew01)'
        sys.exit()

    if (not options.susy2) and (not options.susy5) :
        print 'Error! Please specify a reductionConf! (like --susy2 or --susy5)'
        sys.exit()

    if int(options.susy2)+int(options.susy5) > 1 :
        print 'Error! Please specify a singlereductionConf! (like --susy2 or --susy5)'
        sys.exit()

    if options.susy2 :
        options.reductionConf = 'SUSY2'
    if options.susy5 :
        options.reductionConf = 'SUSY5'

    if not options.user :
        options.user = {'ykeisuke':'ykeisuke'
                        ,'mjoana':'jmiguens'
                        ,'kurb':'brendlin'}.get(os.getenv('USER'),'')

    if not options.user :
        print 'Error! Please specify a user! (your grid username)'
        sys.exit()

    main(options)
