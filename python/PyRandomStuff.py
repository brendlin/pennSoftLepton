
def ReadTable(txtfile) :
    a = open(txtfile).readlines()
    for i in range(len(a)) :
        a[i] = a[i].replace('\n','').split('\\\\')[0]
    thedict = []
    for i in range(len(a)) :
        if '#' in a[i] : continue
        print a[i]
        thedict.append(dict())
        thedict[-1]['rowname'] = a[i].split('&')[0].rstrip().lstrip()
        for j in range(1,len(a[i].split('&'))) :
            tmp = a[i].split('&')[j]
            thedict[-1]['col%d'%(j)] = dict()
            thedict[-1]['col%d'%(j)]['val'] = tmp.split('$\pm$')[0].rstrip().lstrip()
            if len(tmp.split('$\pm$')) > 1 :
                thedict[-1]['col%d'%(j)]['err1'] = tmp.split('$\pm$')[1].rstrip().lstrip()
            if len(tmp.split('$\pm$')) > 2 :
                thedict[-1]['col%d'%(j)]['err2'] = tmp.split('$\pm$')[2].rstrip().lstrip()
    return thedict

def QuadratureColumn(thedict,colnumber,divrownum=-1) :
    import math
    sum2 = 0
    for i in range(len(thedict)) :
        if i == divrownum : continue
        tmpval = thedict[i]['col%d'%(colnumber)]['val']
        #print '~~~%s~~~'%(tmpval)
        if IsFloat(tmpval) :
            sum2 += float(tmpval)**2
            #sum2 += (float(tmpval)-1.00)**2
            #print 'adding',tmpval
    if divrownum > -1 :
        print '%2.2f divided by %2.2f is %2.1f'%(math.sqrt(sum2),
                                                 float(thedict[divrownum]['col%d'%(colnumber)]['val']),
                                                 100.*math.sqrt(sum2)/float(thedict[divrownum]['col%d'%(colnumber)]['val'])) 
        return '%2.2f'%(100.*math.sqrt(sum2)/float(thedict[divrownum]['col%d'%(colnumber)]['val']))
    return '%2.2f'%(math.sqrt(sum2))

def AddColumn(thedict,colnumber) :
    sum = 0
    for i in range(len(thedict)) :
        tmpval = thedict[i]['col%d'%(colnumber)]['val']
        #print '~~~%s~~~'%(tmpval)
        if IsFloat(tmpval) :
            sum += float(tmpval)
            print 'adding',tmpval
    return sum

def WriteTable(thedict) :
    txt = ''
    for i in range(len(thedict)) :
        txt += (thedict[i]['rowname']).ljust(30)
        txt += '\n'
    print txt

def Invert(thedict) :
    txt = ''
    for i in range(len(thedict)) :
        txt += (thedict[i]['rowname']).ljust(30)
        txt += ' '
    txt += '\n'
    for j in range(1,len(thedict[0])) :
        for i in range(len(thedict)) :
            tmp = thedict[i]['col%d'%(j)]['val']
            if tmp == '-' :
                tmp = '0.0'
            elif tmp == '?' :
                tmp = '0.0'
            txt += tmp.rjust(8)
            txt += ' '
        txt += '\n'
    print txt

def GetFFHist(f,k,r,c,p='ttbar',v='GRL',num_or_den='Den',syst='',add_desert=False) : # file key region channel process variable
    import pennSoftLepton.PyAnalysisPlotting as anaplot
    other = '%s_%s'%(p,v)
    rtmp = r
    if r == 'all' :
        rtmp = 'ltt'
    fullkey = ('FFTool_%s_%s_%s_%s%s'%(k,rtmp,syst,c,num_or_den)).replace('__','_')
    hists = anaplot.GetPassEventBkgHistos(v,fullkey,p,f)
    if r == 'all' and ( (c not in ['euu','uee']) or add_desert) :
        hists += anaplot.GetPassEventBkgHistos(v,fullkey.replace('ltt','tlt'),p,f)
        hists += anaplot.GetPassEventBkgHistos(v,fullkey.replace('ltt','ttl'),p,f)
    if not hists :
        print 'Found no histograms for',fullkey
        return 0
    hist = hists[0]
    if len(hists) > 1 : hist.Add(hists[1])
    if len(hists) > 2 : hist.Add(hists[2])
    return hist

def AddFFHistIf(hist,f,k,r,c,p='ttbar',v='GRL',num_or_den='Den',syst='',add_desert=False,scale=1) :
    h = GetFFHist(f,k,r,c,p,v,num_or_den,syst,add_desert)
    if h :
        hist.Add(h,scale)
    return
