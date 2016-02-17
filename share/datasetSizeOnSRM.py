#!/bin/usr/env python
import os

##
## Usage: 
## - To make a file to delete datasets, for a private tag (is fast) :
## python datasetSizeOnSRM.py --user brendlin --pvttag mc15_50ns_ew02 --rtag r6630 --delete
##
## - To make a file to delete official dxaod productions :
## python datasetSizeOnSRM.py --rtag r6630 --ptag p2361 --delete
##
## - To check the size of files on the SRM for a particular tag (official and private) :
## python datasetSizeOnSRM.py --pvttag mc15_50ns_ew02 --rtag r6630 --ptag p2361 --official SUSY2 --size
##

def main(options,args) :
    
    if options.delete :
        delete_filename = 'delete_files'

        print 'Making deletion scripts (this should not take long)'
        cmd = 'rucio list-datasets-rse UPENN_LOCALGROUPDISK'
        if options.user :
            cmd += ' | grep %s'%(options.user)
            delete_filename += '_%s'%(options.user)
        if options.rtag :
            cmd += ' | grep %s'%(options.rtag)
            delete_filename += '_%s'%(options.rtag)
        if options.ptag :
            cmd += ' | grep %s'%(options.ptag)
            delete_filename += '_%s'%(options.ptag)
        if options.pvttag :
            cmd += ' | grep %s'%(options.pvttag)
            delete_filename += '_%s'%(options.pvttag)
        delete_filename += '.sh'

        output = os.popen(cmd).readlines()
        fileout = open(delete_filename,'w')
        for i in output :
            ii = i.replace('\n','')
            fileout.write('dq2-delete-replicas %s UPENN_LOCALGROUPDISK\n'%(ii))
            
        fileout.close()
        print 'Deletion script saved as %s'%(delete_filename)

    if options.size :
        
        print 'Calculating size on SRM - this may take a while!'
        output = []

        tags = []
        tags_private = []
        if options.rtag : 
            tags.append(options.rtag)
            tags_private.append(options.rtag)
        if options.ptag : 
            tags.append(options.ptag)
        if options.pvttag : 
            tags_private.append(options.pvttag)

        fulltagdxaod = '*'.join(tags)
        fulltagprivate = '*'.join(tags_private)

        if options.pvttag :
            user = 'brendlin'
            print 'polling %s'%user
            cmd = 'dq2-ls -fpHL UPENN_LOCALGROUPDISK user.%s.*%s*/ | grep total | grep size'%(user,fulltagprivate)
            print cmd
            output += os.popen(cmd).readlines()

            user = 'jmiguens'
            print 'polling %s'%user
            cmd = 'dq2-ls -fpHL UPENN_LOCALGROUPDISK user.%s.*%s*/ | grep total | grep size'%(user,fulltagprivate)
            print cmd
            output += os.popen(cmd).readlines()

            user = 'ykeisuke'
            print 'polling %s'%user
            cmd = 'dq2-ls -fpHL UPENN_LOCALGROUPDISK user.%s.*%s*/ | grep total | grep size'%(user,fulltagprivate)
            print cmd
            output += os.popen(cmd).readlines()

        if options.official :
            print 'polling%s'%options.official
            cmd = 'dq2-ls -fpHL UPENN_LOCALGROUPDISK mc15_13TeV.*merge.DAOD_%s*%s*/ | grep total | grep size'%(options.official,fulltagdxaod)
            print cmd
            output += os.popen(cmd).readlines()

        size = 0
        for i in output :
            i = i.replace('\n','')
            if not len(i.split()) : continue
            if not 'total size' in i : continue
            factor = 1
            if   'GB'    in i : factor = 1.
            elif 'MB'    in i : factor = 0.001
            elif 'KB'    in i : factor = 0.000001
            elif 'bytes' in i : factor = 0.000000001
            else : 
                print 'ERROR do not understand',i
            size += float(i.split()[-2])*factor

        print 'Total size of tags \'%s\' \'%s\' \'%s\' dxaod \'%s\' is %2.2f GB'%(options.ptag,options.rtag,options.pvttag,options.official,size)

    return

if __name__ == "__main__":
    from optparse import OptionParser
    p = OptionParser()
    p.add_option('--official',type  ='string',default='',dest='official',help='name of the official derivation - SUSY2,SUSY5' )
    p.add_option('--ptag',type  ='string',default='',dest='ptag',help='derivation p-tag')
    p.add_option('--rtag',type  ='string',default='',dest='rtag',help='reco r-tag')
    p.add_option('--pvttag',type  ='string',default='',dest='pvttag',help='private production tag')
    p.add_option('--user',type  ='string',default='',dest='user',help='the user')
    p.add_option('--size',action='store_true',default=False,dest='size',help='calculate size')
    p.add_option('--delete',action='store_true',default=False,dest='delete',help='make deletion files')
    options,args = p.parse_args()

    if not os.getenv('ATLAS_LOCAL_DQ2CLIENT_PATH') :
        print 'Error! localSetupDQ2Client and voms-proxy-init!'
        sys.exit()

    main(options,args)
