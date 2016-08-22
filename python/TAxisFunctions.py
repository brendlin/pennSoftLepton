##
## Available functions:
##
## AutoFixAxes(can)
## FixYaxisRanges(can)
## SetYaxisRanges(can,ymin,ymax)
## GetYaxisRanges(can,check_all=False)
## 
## FixXaxisRanges(can)
## SetXaxisRanges(can,xmin,xmax)
## GetXaxisRanges(can,check_all=False)
## 
## Smaller, helpers:
##  NearestNiceNumber(miny,maxy)
##  MinimumForLog(can)
##  

##
## This function will fit all of the histgram bin content, or TGraph points on a plot.
## If a text or legend has been added to the plot it will force the plot content to appear BELOW
## the text.
##
def AutoFixAxes(can,symmetrize=False) :
    if can.GetPrimitive('pad_top') :
        AutoFixAxes(can.GetPrimitive('pad_top'))
        AutoFixAxes(can.GetPrimitive('pad_bot'))
        return
    FixXaxisRanges(can)    
    AutoFixYaxis(can)
    return

def AutoFixYaxis(can) :
    #
    # Makes space for text as well!
    #
    from ROOT import TFrame
    import math
    maxy_frac = 1
    #
    # Now we make space for any text we drew on the canvas, and
    # also the Legend
    #
    plots_exist = False
    tframe_height = 1-can.GetTopMargin()-can.GetBottomMargin()
    for i in can.GetListOfPrimitives() :
        if type(i) == type(TFrame()) :
            continue
        if hasattr(i,'GetY1NDC') :
            maxy_frac = min(maxy_frac,i.GetY1NDC())
            plots_exist = True
        if hasattr(i,'GetY') :
            maxy_frac = min(maxy_frac,i.GetY())
            plots_exist = True
    if not plots_exist :
        print 'Your plot %s has nothing in it. Doing nothing.'%(can.GetName())
        return
    (miny,maxy) = GetYaxisRanges(can,check_all=True)
    # print 'AutoFixAxes0',miny,maxy
    if miny == 0 and maxy == 0 :
        return
    miny = (0.95*miny)*(miny>0)+(1.05*miny)*(miny<0)
    maxy_frac = maxy_frac-can.GetBottomMargin()
    if can.GetLogy() :
        # special treatment for log plots
        miny = 0.85*MinimumForLog(can)
        # some orders of magnitude *above* miny, making room for text
        orderofmagnitude_span = math.log(maxy/miny)/math.log(10)
        orderofmagnitude_span = 1.1*orderofmagnitude_span*tframe_height/maxy_frac
        maxy = miny*math.pow(10,orderofmagnitude_span)
    else :
        # scale to make space for text
        maxy_frac = maxy_frac-.02
        maxy = tframe_height*(maxy-miny)/float(maxy_frac)+miny
        # round y axis to nice round numbers
        (miny,maxy) = NearestNiceNumber(miny,maxy)
    # print 'AutoFixAxes',miny,maxy
#     if symmetrize :
#         (miny,maxy) = -max(math.fabs(miny),math.fabs(maxy)),max(math.fabs(miny),math.fabs(maxy))
    SetYaxisRanges(can,miny,maxy)
    return

##
## Snap to base-ten-centric numbers
##
def NearestNiceNumber(miny,maxy) :
    import math
    round_number = 10 # or 5 perhaps
    smallest_increment = pow(10,math.floor(math.log((maxy-miny))/math.log(10))-2)
    newminy = round_number*smallest_increment*      int(miny/(round_number*smallest_increment))
    newmaxy = round_number*smallest_increment*math.ceil(maxy/(round_number*smallest_increment))
    # print 'NearestNiceNumber',newminy,newmaxy
    return newminy,newmaxy

##
## Find the non-zero y-axis minimum of plot content.
##
def MinimumForLog(can) :
    from ROOT import TGraph,TH1,TMath,THStack
    ymin = 999999999
    for i in can.GetListOfPrimitives() :
        if issubclass(type(i),TGraph) :
            for y in i.GetY() :
                if y <= 0 :
                    y = ymin
                ymin = min(ymin,y)
        if issubclass(type(i),TH1) :
            for bin in range(i.GetNbinsX()) :
                y = i.GetBinContent(bin+1)
                if y <= 0 :
                    y = ymin
                ymin = min(ymin,y)
        if issubclass(type(i),THStack) :
            j = i.GetHists()[0]
            for bin in range(j.GetNbinsX()) :
                y = j.GetBinContent(bin+1)
                if y <= 0 :
                    y = ymin
                ymin = min(ymin,y)
    # print 'MinimumForLog',ymin
    return ymin

##
## Fit all the data into the canvas (for the y-axis)
##
def FixYaxisRanges(can) :
    (ymin,ymax) = GetYaxisRanges(can,check_all=True)
    SetYaxisRanges(can,ymin,ymax)
    return

##
## Set the x-axis ranges of a canvas
##
def SetYaxisRanges(can,ymin,ymax) :
    if can.GetPrimitive('pad_top') :
        SetYaxisRanges(can.GetPrimitive('pad_top'),ymin,ymax)
    from ROOT import TGraph,TH1,THStack
    yaxis = 0
    for i in can.GetListOfPrimitives() :
        if issubclass(type(i),TGraph) :
            # print 'SetYaxisRanges',ymin,ymax
            i.SetMinimum(ymin)
            i.SetMaximum(ymax)
            if not yaxis :
                yaxis = i.GetHistogram().GetYaxis()
        if issubclass(type(i),TH1) :
            # print 'SetYaxisRanges',ymin,ymax
            i.SetMinimum(ymin)
            i.SetMaximum(ymax)
            if not yaxis: 
                yaxis = i.GetYaxis()
        if issubclass(type(i),THStack) :
            # print 'SetYaxisRanges',ymin,ymax
            i.SetMinimum(ymin)
            i.SetMaximum(ymax)
            if not yaxis :
                yaxis = i.GetHistogram().GetYaxis()
    if not yaxis :
        print 'Warning: SetYaxisRange had no effect. Check that your canvas has plots in it.'
        return
    
    yaxis.SetRangeUser(ymin,ymax)
    can.Modified()
    can.Update()
    return

def SetNdivisions(can,a,b,c) :
    for i in can.GetListOfPrimitives() :
        if hasattr(i,'GetXaxis') :
            i.GetXaxis().SetNdivisions(a,b,c)
    can.Modified()
    can.Update()
    return
    

##
## Returns the y-range of the first plotted histogram.
## If you specify "check_all=True", returns the maximal y-range of all the plots in the canvas
##
def GetYaxisRanges(can,check_all=False) :
    #
    # check_all is if you want to check the maximum extent of all the histograms you plotted.
    #
    from ROOT import TGraph,TH1,TMath,THStack
    ymin = 999999999
    ymax = -999999999
    
    primitives = list(can.GetListOfPrimitives())
    if can.GetPrimitive('stack') :
        the_stack = list(can.GetPrimitive('stack').GetHists())
        primitives += the_stack

    for i in primitives :
        if issubclass(type(i),TGraph) :
            ymin = min(ymin,TMath.MinElement(i.GetN(),i.GetY()))
            ymax = max(ymax,TMath.MaxElement(i.GetN(),i.GetY()))
            if not check_all :
                return ymin,ymax
        if issubclass(type(i),TH1) :
            for bin in range(i.GetNbinsX()) :
                if bin < i.GetXaxis().GetFirst() : continue # X-axis SetRange should be done first
                if bin > i.GetXaxis().GetLast() : continue # X-axis SetRange should be done first
                y = i.GetBinContent(bin+1)
                ye = i.GetBinError(bin+1)
                ymin = min(ymin,y-ye)
                ymax = max(ymax,y+ye)
            if not check_all :
                return ymin,ymax
        if issubclass(type(i),THStack) :
            ymin = min(ymin,i.GetMinimum())
            ymax = max(ymax,i.GetMaximum())
            if not check_all :
                return ymin,ymax

    # print 'GetYaxisRanges',ymin,ymax
    return ymin,ymax

##
## Fit all the data into the canvas (for the x-axis)
##
def FixXaxisRanges(can) :
    (xmin,xmax) = GetXaxisRanges(can,check_all=True)
    SetXaxisRanges(can,xmin,xmax)
    return

##
## Set the x-axis ranges of a canvas
##
def SetXaxisRanges(can,xmin,xmax) :
    if can.GetPrimitive('pad_top') :
        SetXaxisRanges(can.GetPrimitive('pad_top'),xmin,xmax)
        SetXaxisRanges(can.GetPrimitive('pad_bot'),xmin,xmax)
        return
    from ROOT import TGraph,TH1,THStack
    xaxis = 0
    for i in can.GetListOfPrimitives() :
        if issubclass(type(i),TGraph) :
            xaxis = i.GetHistogram().GetXaxis()
            xaxis.SetRangeUser(xmin,xmax)
        if issubclass(type(i),TH1) :
            xaxis = i.GetXaxis()
            xaxis.SetRangeUser(xmin,xmax)
        if issubclass(type(i),THStack) :
            xaxis = i.GetXaxis()
            xaxis.SetRangeUser(xmin,xmax)
    
    if not xaxis :
        print 'Warning: SetXaxisRange had no effect. Check that your canvas has plots in it.'
        return

    can.Modified()
    can.Update()
    return

##
## Returns the x-range of the first plotted histogram.
## If you specify "check_all=True", returns the maximal x-range of all the plots in the canvas
##
def GetXaxisRanges(can,check_all=False) :
    from ROOT import TGraph,TH1
    xmin = 999999999
    xmax = -999999999
    for i in can.GetListOfPrimitives() :
        if issubclass(type(i),TGraph) :
            xaxis = i.GetHistogram().GetXaxis()
            if not check_all :
                return xaxis.GetXmin(),xaxis.GetXmax()
            xmin = min(xmin,xaxis.GetXmin())
            xmax = max(xmax,xaxis.GetXmax())
        if issubclass(type(i),TH1) :
            xaxis = i.GetXaxis()
            if not check_all :
                return xaxis.GetXmin(),xaxis.GetXmax()
            xmin = min(xmin,xaxis.GetXmin())
            xmax = max(xmax,xaxis.GetXmax())
    return xmin,xmax

##
## Puts the overflow into the last bin
##
def PutOverflowIntoLastBin(hist,high) :
    import math
    print hist.GetName(),high
    if high == None :
        high = hist.GetXaxis().GetBinLowEdge(hist.GetNbinsX()+1)
    last_bin = hist.GetXaxis().FindBin(high)-1
    for i in range(hist.GetNbinsX()+1) :
        if hist.GetXaxis().GetBinLowEdge(i+1) >= high :
            hist.SetBinContent(last_bin,hist.GetBinContent(i+1)+hist.GetBinContent(last_bin))
            hist.SetBinError(last_bin,math.sqrt(hist.GetBinError(i+1)**2 + hist.GetBinError(last_bin)**2))
            hist.SetBinContent(i+1,0)
            hist.SetBinError(i+1,0)
    return
